#include "ringbuffer.h"
#include "assert.h"
#include <string.h>
#include <stdlib.h>

enum rl_ringbuffer_state rt_ringbuffer_status(struct rl_ringbuffer *rb)
{
	if (rb->read_index == rb->write_index)
	{
		if (rb->read_mirror == rb->write_mirror)
			return RL_RINGBUFFER_EMPTY;
		else
			return RL_RINGBUFFER_FULL;
	}
	return RL_RINGBUFFER_HALFFULL;
}

void rl_ringbuffer_init(struct rl_ringbuffer *rb, rl_uint8_t *pool, rl_uint16_t size)
{
	assert(rb != NULL);
	assert(size > 0);

	/*initialize read and write index*/
	rb->read_mirror = rb->read_index = 0;
	rb->write_mirror = rb->write_index = 0;

	/*set buffer pool and size*/
	rb->buffer_ptr = pool;
	rb->buffer_size = size;
}

rl_uint32_t rl_ringbuffer_data_len(struct rl_ringbuffer *rb)
{
	switch (rt_ringbuffer_status(rb))
	{
	case RL_RINGBUFFER_EMPTY:
		return 0;
	case RL_RINGBUFFER_FULL:
		return rb->buffer_size;
	case RL_RINGBUFFER_HALFFULL:
	default:
		if (rb->write_index > rb->read_index)
			return rb->write_index - rb->read_index;
		else
			return rb->buffer_size - (rb->read_index - rb->write_index);
	}
}

rl_uint32_t rl_ringbuffer_putchar(struct rl_ringbuffer *rb,rl_uint8_t ch)
{
	assert(rb != NULL);
	if (!rl_ringbuffer_space_len(rb))
		return 0;
	rb->buffer_ptr[rb->write_index] = ch;
	/* flip mirror */
	if (rb->write_index == rb->buffer_size - 1)
	{
		rb->write_mirror = ~rb->write_mirror;
		rb->write_index = 0;
	}
	else
	{
		rb->write_index++;
	}
	return 1;
} 

rl_uint32_t rl_ringbuffer_putchar_force(struct rl_ringbuffer *rb, const rl_uint8_t ch)
{
	enum rl_ringbuffer_state old_state;
	assert(rb != NULL);
	old_state = rt_ringbuffer_status(rb);
	rb->buffer_ptr[rb->write_index] = ch;

	/* flip mirror */
	if (rb->write_index == rb->buffer_size - 1)
	{
		rb->write_mirror = ~rb->write_mirror;
		rb->write_index = 0;
		if (old_state == RL_RINGBUFFER_FULL)
		{
			rb->read_mirror = ~rb->read_mirror;
			rb->read_index = rb->write_index;
		}
	}
	else
	{
		rb->write_index++;
		if (old_state == RL_RINGBUFFER_FULL)
			rb->read_index = rb->write_index;
	}
	return 1;
}

 rl_uint32_t rl_ringbuffer_put(struct rl_ringbuffer *rb, const rl_uint8_t *ptr, rl_uint16_t length)
{
	 rl_uint16_t size;
	 assert(rb != NULL);

	 size = rl_ringbuffer_space_len(rb);

	 /* no space */
	 if (size == 0)
	 {
		 return 0;
	 }

	 /* drop some data */
	 if (size < length)
		 length = size;
	 if (rb->buffer_size - rb->write_index > length)
	 {
		 /* read_index - write_index = empty space */
		 memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
		 rb->write_index += length;
		 return length;
	 }
	 memcpy(&rb->buffer_ptr[rb->write_index], &ptr[0], rb->buffer_size - rb->write_index);
	 memcpy(&rb->buffer_ptr[0], &ptr[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

	 /* we are going into the other side of the mirror */
	 rb->write_mirror = ~rb->write_mirror;
	 rb->write_index = length - (rb->buffer_size - rb->write_index);
	 return length;
}

rl_uint32_t rl_ringbuffer_put_force(struct rl_ringbuffer *rb, const rl_uint8_t *ptr, rl_uint16_t length)
{
	rl_uint16_t space_length;
	assert(rb != NULL);
	space_length = rl_ringbuffer_space_len(rb);
	if (length > rb->buffer_size)
	{
		ptr = &ptr[length - rb->buffer_size];
		length = rb->buffer_size;
	}

	if (rb->buffer_size - rb->write_index > length)
	{
		/* read_index - write_index = empty space */
		memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
		/* this should not cause overflow because there is enough space for length of data in current mirror */
		rb->write_index += length;
		if (length > space_length)
			rb->read_index = rb->write_index;
		return length;
	}
	memcpy(&rb->buffer_ptr[rb->write_index], &ptr[0], rb->buffer_size - rb->write_index);
	memcpy(&rb->buffer_ptr[0], &ptr[rb->buffer_size - rb->write_index], length - (rb->buffer_size - rb->write_index));

	/* we are going into the other side of the mirror */
	rb->write_mirror = ~rb->write_mirror;
	rb->write_index = length - (rb->buffer_size - rb->write_index);
	if (length > space_length)
	{
		rb->read_mirror = ~rb->read_mirror;
		rb->read_index = rb->write_index;
	}
	return length;
}

rl_uint32_t rl_ringbuffer_getchar(struct rl_ringbuffer *rb, rl_uint8_t *ch)
{
	assert(rb != NULL);
	/* ringbuffer is empty */
	if (!rl_ringbuffer_data_len(rb))
		return 0;
	/* put character */
	*ch = rb->buffer_ptr[rb->read_index];
	if (rb->read_index == rb->buffer_size - 1)
	{
		rb->read_mirror = ~rb->read_mirror;
		rb->read_index = 0;
	}
	else
	{
		rb->read_index++;
	}
	return 1;
}

rl_uint32_t rl_ringbuffer_get(struct rl_ringbuffer *rb, rl_uint8_t *ptr, rl_uint16_t length)
{
	rl_uint16_t size;
	assert(rb != NULL);
	/* whether has enough data */
	size = rl_ringbuffer_data_len(rb);
	/* no data */
	if (size == 0)
		return 0;

	/* less data */
	if (size < length)
		length = size;
	if (rb->buffer_size - rb->read_index > length)
	{
		/* copy all of data */
		memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
		/* this should not cause overflow because there is enough space for length of data in current mirror */
		rb->read_index += length;
		return length;
	}
	memcpy(&ptr[0], &rb->buffer_ptr[rb->read_index], rb->buffer_size - rb->read_index);
	memcpy(&ptr[rb->buffer_size - rb->read_index], &rb->buffer_ptr[0], length - (rb->buffer_size - rb->read_index));

	/* we are going into the other isde of the mirror */
	rb->read_mirror = ~rb->read_mirror;
	rb->read_index = length - (rb->buffer_size - rb->read_index);
	return length;
}

void rl_ringbuffer_reset(struct rl_ringbuffer *rb)
{
	assert(rb != NULL);
	rb->read_mirror = 0;
	rb->read_index = 0;
	rb->write_mirror = 0;
	rb->write_index = 0;
}

#if defined(RL_USSING_HEAP)

struct rl_ringbuffer *rl_ringbuffer_create(rl_uint16_t size)
{
	struct rl_ringbuffer *rb;
	rl_uint8_t *pool;

	assert(size > 0);
	size = RL_ALIGN_DOWN(size, RL_ALIGN_SIZE);
	rb = (struct rl_ringbuffer *)malloc(sizeof(struct rl_ringbuffer));
	if (rb == NULL)
		goto exit;
	pool = (rl_uint8_t *)malloc(size);
	if (pool == NULL)
	{
		free(rb);
		rb = NULL;
		goto exit;
	}
	rl_ringbuffer_init(rb, pool, size);

exit:
	return rb;
}

void rl_ringbuffer_destroy(struct rl_ringbuffer *rb)
{
	assert(rb != NULL);
	free(rb->buffer_ptr);
	free(rb);
}

#endif

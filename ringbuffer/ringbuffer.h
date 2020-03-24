#pragma once
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__
#include "rl_def.h"

#define rl_ringbuffer_space_len(rb)		((rb)->buffer_size - rl_ringbuffer_data_len(rb))

/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. RT_ALIGN(13, 4)
 * would return 16.
 */
#define RL_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

 /**
  * @ingroup BasicDef
  *
  * @def RT_ALIGN_DOWN(size, align)
  * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
  * would return 12.
  */
#define RL_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define RL_ALIGN_SIZE 4

struct rl_ringbuffer {
	rl_uint8_t *buffer_ptr;
	rl_uint16_t read_mirror : 1;
	rl_uint16_t read_index : 15;
	rl_uint16_t write_mirror : 1;
	rl_uint16_t write_index : 15;
	rl_uint16_t buffer_size;
};

enum rl_ringbuffer_state
{
	RL_RINGBUFFER_EMPTY,
	RL_RINGBUFFER_FULL,
	RL_RINGBUFFER_HALFFULL,
};

void rl_ringbuffer_init(struct rl_ringbuffer *rb, rl_uint8_t *pool, rl_uint16_t size);
rl_uint32_t rl_ringbuffer_data_len(struct rl_ringbuffer *rb);

#endif // !__RINGBUFFER_H__
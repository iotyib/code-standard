#include "app_inc.h"


unsigned long GetMessageDataLen(unsigned char *pBufIn, unsigned char offset, uint16_t bufSize)
{
    unsigned char count;
    unsigned long val = 0;
    for (count = offset;; count++)
    {
        if ((pBufIn[count] >= '0') && (pBufIn[count] <= '9'))
        {
            val *= 10;
            val += (pBufIn[count] - '0');
        } else
        {
            break;
        }
    }
    count++;
    return val;
}




/**
 * ��ASCII���ִ�ת��ΪHEX����
 * 
 * 
 * @param ascii 
 * @param hex 
 * @param ascii_len 
 * 
 * @return unsigned short 
 */
unsigned short AsciiToHexData(unsigned char ascii[], unsigned char hex[], unsigned short ascii_len)
{
    unsigned short i = 0, j = 0;
    for (i = 0; i < ascii_len; i++, i++)
    {
        hex[j++] = (AsciiToHex(ascii[i]) << 4 | AsciiToHex(ascii[i + 1]));
    }
    return j;
}


/**
 * @brief hexתascii
 * 
 * @retval unsigned short: 
 */
unsigned short HexToAscii(unsigned char ascii[], unsigned char hex[], unsigned short data_len)
{
    unsigned short i, j = 0;
    for (i = 0; i < data_len; i++)
    {
        sprintf((char *)ascii + j++ * 2, "%02X", hex[i]);
    }
    return i * 2;
}

/**
 * @brief ascii ת hex
 * 
 * @param ascii: 
 * @retval u8: 
 */
unsigned char AsciiToHex(unsigned char ascii)
{
    if ((ascii >= '0') && (ascii <= '9')) ascii -= '0';
    else if ((ascii >= 'A') && (ascii <= 'F')) ascii = ascii - 'A' + 10;
    else if ((ascii >= 'a') && (ascii <= 'f')) ascii = ascii - 'a' + 10;
    else ascii = 0;
    return ascii;
}


static bool crc_tab16_init  =   false;
static uint16_t crc_tab16[256];


void init_crc16_tab(void)
{
    uint16_t i, j, crc, c;
    for (i = 0; i < 256; i++)
    {
        crc = 0;
        c = i;
        for (j = 0; j < 8; j++)
        {
            if ((crc ^ c) & 0x0001)
            {
                crc = (crc >> 1) ^ CRC_POLY_16;
            } else
            {
                crc = crc >> 1;
            }
            c = c >> 1;
        }
        crc_tab16[i] = crc;
    }
    crc_tab16_init = true;
}


uint16_t crc_16(const unsigned char *input_str, size_t num_bytes)
{
    uint16_t crc, tmp, short_c, i;
    const unsigned char *ptr;
    if (!crc_tab16_init)
    {
        init_crc16_tab();
    }
    crc = CRC_START_16;
    ptr = input_str;
    if (ptr != NULL)
    {
        for (i = 0; i < num_bytes; i++)
        {
            short_c = 0x00ff & (uint16_t)*ptr;
            tmp = crc ^ short_c;
            crc = (crc >> 8) ^ crc_tab16[tmp & 0xff];
            ptr++;
        }
    }
    return crc;
}
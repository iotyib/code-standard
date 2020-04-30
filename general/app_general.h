#ifndef __GENERAL_H__
#define __GENERAL_H__

#define		CRC_POLY_16			0xA001
#define		CRC_POLY_32			0xEDB88320L
#define		CRC_POLY_CCITT		0x1021
#define		CRC_POLY_DNP		0xA6BC
#define		CRC_POLY_KERMIT		0x8408
#define		CRC_POLY_SICK		0x8005

#define		CRC_START_8				0x00
#define		CRC_START_16			0x0000
#define		CRC_START_MODBUS		0xFFFF
#define		CRC_START_XMODEM		0x0000
#define		CRC_START_CCITT_1D0F	0x1D0F
#define		CRC_START_CCITT_FFFF	0xFFFF
#define		CRC_START_KERMIT		0x0000
#define		CRC_START_SICK			0x0000
#define		CRC_START_DNP			0x0000
#define		CRC_START_32			0xFFFFFFFFL










void init_crc16_tab(void);
uint16_t crc_16(const unsigned char *input_str,size_t num_bytes);


unsigned long GetMessageDataLen(unsigned char *pBufIn, unsigned char offset, uint16_t bufSize);
unsigned short AsciiToHexData(unsigned char ascii[], unsigned char hex[], unsigned short ascii_len);
unsigned short HexToAscii(unsigned char ascii[], unsigned char hex[], unsigned short data_len);
unsigned char AsciiToHex(unsigned char ascii);


#endif


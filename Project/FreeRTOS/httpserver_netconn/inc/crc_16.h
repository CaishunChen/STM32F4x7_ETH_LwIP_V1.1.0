#ifndef __CRC_16_H
#define __CRC_16_H

#include "stdint.h"

uint16_t crc_ccitt(uint8_t *q, uint16_t len);
uint16_t crc16(uint8_t *buf,uint16_t length);
uint16_t GetCRC16( uint8_t* bufData, uint16_t sizeData );
uint16_t UART_CRC16_Work(uint8_t *CRC_Buf,uint8_t CRC_Leni);

#endif


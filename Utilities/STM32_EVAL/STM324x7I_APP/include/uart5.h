#ifndef _UART5_H_
#define _UART5_H_
#include "stm32f4xx.h"

extern uint8_t  RX5_BUF[336];
extern bool     RX5_RCV ;

void Uart5_Configuration(void);
void Uart5_Send(uint8_t *p,uint32_t lenght);
void UART5_IRQHandler(void);

#endif


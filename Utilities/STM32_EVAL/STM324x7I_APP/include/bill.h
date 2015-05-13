#ifndef __bill__
#define __bill__
#include "stdint.h"
#include "stm32f4xx.h"
#define ack_flag    '1'
#define nack_flag   '0'
extern uint8_t NewCoinsCnt;
extern uint8_t OldCoinsCnt;

void SetBills(void);
void DisableBills(void);
void Uart4_Bill(uint8_t *p,uint8_t sizeData);

uint8_t  ReadMoneyFun(uint8_t BillActionStep);
uint8_t  CashCodeInit(void);
uint8_t  SetReceUsersMoney(void);
uint8_t  SetReceChangeMoney(void);
uint8_t  NoAcceptMoneyFun(void);
uint8_t  ReadBill(void);
uint8_t  ReadBills(void);

bool StringToHexGroup(uint8_t *OutHexBuffer, uint8_t *InStrBuffer, uint32_t strLength);


#endif


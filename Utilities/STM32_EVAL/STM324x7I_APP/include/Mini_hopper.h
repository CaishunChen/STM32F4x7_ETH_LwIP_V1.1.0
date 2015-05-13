#ifndef __Mini_hopper_h
#define __Mini_hopper_h

#include <stm32f4xx.h>

extern uint32_t Coins_cnt; //一键退币计数
extern uint32_t Coins_totoal; //硬币总数统计
extern uint8_t ErrorType;
void InitMiniGPIO(void);
uint8_t SendOutN_Coin(uint32_t num);
uint8_t StatisticalCoin(void);
void CloseCoins(void);
void OpenCoins(void);
void RefundButton(uint16_t coins_cnt);
void StopRefond(void);

#endif


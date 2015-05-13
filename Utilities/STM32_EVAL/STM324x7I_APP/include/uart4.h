
#include <stm32f4xx.h>

extern uint8_t  BillDataBuffer[20];
extern uint8_t  RX4Buffer[40];
void UART4_IRQHandler(void);
void Uart4_Configuration(void);

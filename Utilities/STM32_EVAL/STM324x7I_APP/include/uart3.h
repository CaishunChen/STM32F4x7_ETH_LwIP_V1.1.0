#ifndef __Uart3__h
#define __Uart3__h
#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "queue.h"


/*串口帧结构体*/
typedef struct _ScreenFrameStruct //(_ScreenFrameStruct 是可以不要的)
{
	uint8_t frame_head[2];      //帧头
	uint8_t frame_length[1];    //帧长度
	uint8_t frame_command[1];   //帧命令
	uint8_t frame_variable[2];  //帧变量
	uint8_t frame_content_len[1]; //内容长度
	uint8_t frame_content[2];   //帧内容
	uint8_t frame_crc[2];       //帧校验
}ScreenFrameStruct;

extern ScreenFrameStruct ScreenFrame;

extern   uint8_t LedRecv[8] ; 
extern   xQueueHandle pUart3_xQueue;
void Uart3_Configuration(void);
void Uart3_Send(const uint8_t *p,uint8_t length);
void Uart3_Sent(const uint8_t *p,uint8_t length);

void _LCD_Enable_RxInit(void);
void _LCD_Disable_RxInit(void);

uint8_t USART3_GetChar(uint8_t *Chr);
uint32_t UART3_GetCharsInRxBuf(void);
void UART3_ClrRxBuf(void);
void USART3_IRQHandler(void); 
#endif


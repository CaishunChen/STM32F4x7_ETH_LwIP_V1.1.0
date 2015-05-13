#include <string.h>
#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "dgus_task.h"
#include "uart3.h"
#include "serialscreen.h"
#include "crc_16.h"

#define  DGUS_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Dgus_task(void * pvParameters)
{
	ScreenFrameStruct  *pRxScreenFrame;
	portBASE_TYPE  xStatus; 
	MyUnion my_union;
	uint16_t crc_value_get=0,crc_value_cal=0,VariableAdress=0;
	uint8_t  RegisterData[5]={0},VariableData[5]={0};  
	uint8_t  crc_value[20]={0},RegisterLength = 0,VariableLength = 0,RegisterAdress=0;
	
	PageChange(Menu_interface);
	pUart3_xQueue = xQueueCreate( 10, sizeof(  struct _ScreenFrameStruct * ) );
	for(;;)
	{
		xStatus = xQueueReceive(pUart3_xQueue,&(pRxScreenFrame),( portTickType ) 10 );
		if(xStatus == pdPASS)
		{
			memcpy(crc_value,pRxScreenFrame->frame_command,sizeof(pRxScreenFrame->frame_command));
			memcpy(crc_value+sizeof(pRxScreenFrame->frame_command),pRxScreenFrame->frame_variable,sizeof(pRxScreenFrame->frame_variable));
			memcpy(crc_value+sizeof(pRxScreenFrame->frame_command)+sizeof(pRxScreenFrame->frame_variable),pRxScreenFrame->frame_content_len,sizeof(pRxScreenFrame->frame_content_len));
			memcpy(crc_value+sizeof(pRxScreenFrame->frame_command)+sizeof(pRxScreenFrame->frame_variable)+sizeof(pRxScreenFrame->frame_content_len),pRxScreenFrame->frame_content,sizeof(pRxScreenFrame->frame_content));
			crc_value_cal = UART_CRC16_Work(crc_value,(*(pRxScreenFrame->frame_length)-2));	
			crc_value_get = ((pRxScreenFrame->frame_crc[0]))|(pRxScreenFrame->frame_crc[1]<<8);
			if(crc_value_cal == crc_value_get)
			{
				if(pRxScreenFrame->frame_command[0] == 0x83) //读数据存储器返回命令
				{
					my_union.adr[1] = pRxScreenFrame->frame_variable[0]; //有可能是小端模式引起的
					my_union.adr[0] = pRxScreenFrame->frame_variable[1];
					VariableAdress = my_union.adress;
					VariableLength = pRxScreenFrame->frame_content_len[0];
                    memcpy(VariableData,pRxScreenFrame->frame_content,sizeof(pRxScreenFrame->frame_content));
					//加入修改变量地址数据的功能
					ChangeVariableValues(VariableAdress,VariableData,VariableLength);
				}
				else if(pRxScreenFrame->frame_command[0] == 0x81) //读寄存器返回命令
				{
					
				}	
			}	
		}
	}
}



void DgusInit(void)
{
	xTaskCreate(Dgus_task,(int8_t *)"Dgus", configMINIMAL_STACK_SIZE , NULL,DGUS_TASK_PRIO, NULL);
}


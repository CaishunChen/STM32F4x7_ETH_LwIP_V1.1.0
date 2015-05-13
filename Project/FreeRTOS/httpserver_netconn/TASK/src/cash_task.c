#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "cash_task.h"
#include "bill.h"
#include "coin.h"
#include "bsp.h"

#define  CASH_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Cash_task(void * pvParameters)
{
	xSemaphoreHandle xMoneyMutex ;  // 
	xQueueHandle xMoneyRecved,xMoneyComand;    //
	xMoneyMutex = xSemaphoreCreateRecursiveMutex(); //创建互斥量
	xMoneyRecved = xQueueCreate( 8, sizeof( uint8_t) ); //创建对队列
	xMoneyComand = xQueueCreate( 2, sizeof( uint8_t) ); //创建对队列
	uint8_t MoneyRecved = 0,MoneyComand = 0;
	for(;;)
	{
		if(xQueueReceive(xMoneyComand,&MoneyComand,1)) //接受纸币机的命令
		{
			if(MoneyComand == 0) CloseCashSystem();
			else
			{
				OpenCashSystem();
			}	
		}		
		MoneyRecved = ReadBill();
		if(MoneyRecved>0)
		{
			xQueueSend(xMoneyRecved,&MoneyRecved,1); //接收到钱后，直接将钱的的数据发到 xMoneyRecved
		}
		#if 0
		if( xMoneyMutex != NULL )
		{
			if( xSemaphoreTakeRecursive( xMoneyMutex, ( portTickType ) 1 ) == pdTRUE )
			{
				xSemaphoreGiveRecursive( xMoneyMutex );
			}
			else
			{
				MoneyRecved = ReadBill();
				if(MoneyRecved>0)
				{
					
				}
			}
		}
		#endif
	}
}



void CashInit(void)
{
	xTaskCreate(Cash_task,(int8_t *)"Cash", configMINIMAL_STACK_SIZE , NULL,CASH_TASK_PRIO, NULL);
}


#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "card_task.h"

#define  CRAD_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Card_task(void * pvParameters)
{
	for(;;);
}



void CardInit(void)
{
	xTaskCreate(Card_task,(int8_t *)"Dgus", configMINIMAL_STACK_SIZE , NULL,CRAD_TASK_PRIO, NULL);
}


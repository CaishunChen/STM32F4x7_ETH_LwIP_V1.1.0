#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "hopper_task.h"

#define  HOPPER_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Hopper_task(void * pvParameters)
{
	for(;;);
}



void HopperInit(void)
{
	xTaskCreate(Hopper_task,(int8_t *)"Hopper", configMINIMAL_STACK_SIZE , NULL,HOPPER_TASK_PRIO, NULL);
}


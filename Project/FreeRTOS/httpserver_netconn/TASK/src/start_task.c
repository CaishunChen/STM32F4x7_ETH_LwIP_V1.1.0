#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "start_task.h"

#define  START_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Start_task(void * pvParameters)
{
	for(;;);
}



void StartInit(void)
{
	xTaskCreate(Start_task,(int8_t *)"Start", configMINIMAL_STACK_SIZE , NULL,START_TASK_PRIO, NULL);
}


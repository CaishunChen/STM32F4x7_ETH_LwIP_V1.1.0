#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "tkmeal_task.h"

#define  TKMEAL_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Tkmeal_task(void * pvParameters)
{
	for(;;);
}



void TkmealInit(void)
{
	xTaskCreate(Tkmeal_task,(int8_t *)"Tkmeal", configMINIMAL_STACK_SIZE , NULL,TKMEAL_TASK_PRIO, NULL);
}


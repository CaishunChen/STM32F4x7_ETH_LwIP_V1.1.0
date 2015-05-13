
#include <stm32f4xx.h>
#include "timers.h"
#include "delay.h"
__IO uint32_t ntime;								    
//使用SysTick进行精确延时
//ms延时
void delay_ms(uint32_t ms)
{	 		  	  
	vTaskDelay(ms);
}   
//us延时		    								   
void delay_us(uint32_t us)
{		
}
//粗略延时
void Delay(uint32_t count)
{
	while(count--);
}





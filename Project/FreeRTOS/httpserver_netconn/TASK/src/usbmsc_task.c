
#include <stm32f4xx.h>
#include "usbmsc_task.h"
#include "freeRTOS.h"
#include "task.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

#define  USBMSC_TASK_PRIO  ( ( configMAX_PRIORITIES - 6 ) )

/** @defgroup USBH_USR_MAIN_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
/**
* @}
*/ 


void usbmsc_task(void * pvParameters)
{
	 __IO uint32_t i = 0;
	  /* Init Host Library */
  USBH_Init(&USB_OTG_Core, 
#ifdef USE_USB_OTG_FS  
            USB_OTG_FS_CORE_ID,
#else 
            USB_OTG_HS_CORE_ID,
#endif 
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);
	for(;;)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
		if (i++ == 0x10000)
		{
		  STM_EVAL_LEDToggle(LED1);
		  STM_EVAL_LEDToggle(LED2);
		  STM_EVAL_LEDToggle(LED3);
		  STM_EVAL_LEDToggle(LED4);
		  i = 0;
		}   
	}
}


void USB_MSC_Init(void)
{
	xTaskCreate(usbmsc_task,(int8_t *)"Wan", configMINIMAL_STACK_SIZE * 8 , NULL,USBMSC_TASK_PRIO, NULL);
}


/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <arm_math.h>
#include <stm32f4xx.h>
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "tcpip.h"
#include "httpserver-netconn.h"
#include "serial_debug.h"
#include "tcp_server.h"
#include "ff.h"
#include "diskio.h"
#include "serialscreen.h"
#include "uart3.h"
#include "bsp.h"
#include "crc_16.h"
#include "flash.h"
#include "task.h"
#include "card_task.h"
#include "cash_task.h"
#include "dgus_task.h"
#include "hopper_task.h"
#include "record_task.h"
#include "server_task.h"
#include "start_task.h"
#include "tkmeal_task.h"
#include "wanwuyun_task.h"
#include "usbmsc_task.h"
#include "rtc.h"
#include "intel_flash.h"
#include "bootmode.h"
#include "img_utils.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*--------------- LCD Messages ---------------*/
#if defined (STM32F40XX)
#define MESSAGE1   "    STM32F40/41x     "
#elif defined (STM32F427X)
#define MESSAGE1   "     STM32F427x      "
#endif
#define MESSAGE2   "  STM32F-4 Series   "
#define MESSAGE3   "Basic WebServer Demo"
#define MESSAGE4   "                    "

/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO   ( tskIDLE_PRIORITY + 1 )
#define DHCP_TASK_PRIO   ( configMAX_PRIORITIES - 4 )  
#define LED_TASK_PRIO    ( tskIDLE_PRIORITY + 2 )
#define KEY_TASK_PRIO    ( tskIDLE_PRIORITY + 3 )
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void LCD_LED_Init(void);
void ToggleLed4(void * pvParameters);
void KeyScan4(void * pvParameters);
void Main_task(void * pvParameters);

void iap_load_app(void);

/*extern */
extern void tcpecho_init(void);

/* Private functions ---------------------------------------------------------*/
void Dgus_task(void * pvParameters);


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
	/*!< At this stage the microcontroller clock setting is already configured to 
		 168 MHz, this is done through SystemInit() function which is called from
		 startup file (startup_stm32f4xx.s) before to branch to application main.
		 To reconfigure the default setting of SystemInit() function, refer to
		 system_stm32f4xx.c file
	   */
	/* iap check the gpio and load app */   
	iap_load_app();	

	/* Configures the priority grouping: 4 bits pre-emption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Init task */
	xTaskCreate(Main_task,(int8_t *)"Main", configMINIMAL_STACK_SIZE , NULL,MAIN_TASK_PRIO, NULL);
	
	/* Start scheduler */
	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler */
	for( ;; );
	

}

/**
  * @brief  Main task
  * @param  pvParameters not used
  * @retval None
  */
#if 0
void Main_task(void * pvParameters)
{
	static xTaskHandle xHandle;
/*======硬件初始化============*/
	HardwaveInit();
/*=======开始建立任务==========*/
	ServerInit(); //? 在RecordInit();就不行，应该
//	RecordInit(); 
//	StartInit();
//	DgusInit();
//	CashInit();
//	CardInit();
//	HopperInit();
	
//	TkmealInit();
	for(;;)
	{
		vTaskDelete(xHandle);
	}
}
#endif
/**
  * @brief  Main task
  * @param  pvParameters not used
  * @retval None
  */
#if 1
void Main_task(void * pvParameters)
{
#ifdef SERIAL_DEBUG
	DebugComPort_Init();
#endif
	/*Initialize LCD and Leds */ 
	LCD_LED_Init();

	/* configure Ethernet (GPIOs, clocks, MAC, DMA) */ 
	//ETH_BSP_Config();

	/*Initialize RTC */ 
	AppRTC_Init();
	
	/*Initialize USB */
	USB_MSC_Init();
	
	/* Initilaize the LwIP stack */
	//LwIP_Init();

	/* Initialize webserver demo */
	//http_server_netconn_init();
	
	/* Initialize tcpserver demo */
	//tcpecho_init();
	
	/* Initialize tcpcliennt demo */
	//Tcpclient_Init();
	
	/* Initialize wanwuyun demo */
    //Wanwuyun_Init();
	 
#ifdef USE_DHCP
  /* Start DHCPClient */
	xTaskCreate(LwIP_DHCP_task, (int8_t *) "DHCP", configMINIMAL_STACK_SIZE , NULL,DHCP_TASK_PRIO, NULL);
#endif

	/* Start toogleLed4 task : Toggle LED4  every 250ms */
	//xTaskCreate(ToggleLed4, (int8_t *) "LED4", DEFAULT_THREAD_STACKSIZE, NULL, LED_TASK_PRIO, NULL);
	
	/* Start keyscan4 task : scan 4 kinds of keys  every 250ms */
	//xTaskCreate(KeyScan4, (int8_t *) "KEY4", configMINIMAL_STACK_SIZE, NULL, KEY_TASK_PRIO, NULL);
	
	for( ;; )
	{
		vTaskDelete(NULL);
	}
}
#endif

/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
static DATA_TIME_STRUCT DataTime;
SENSOR_STRUCT  Sensor,*pSensor;
//如何加入浮点运算
void ToggleLed4(void * pvParameters)
{
	//static portCHAR PAGE_BODY[512]={0x00};
//	memcpy(Sensor.dev_id,"stm32_test",strlen("stm32_test"));
//	Sensor.temperature[0] = 25.6;
//	Sensor.latitude[0] = 143.5;
//	Sensor.longitude[0] = 145.123;
	for( ;; )
	{
		/* Toggle LED4 each 250ms */
		STM_EVAL_LEDToggle(LED4);
		RTC_TimeShow(&DataTime); 
//		pSensor = &Sensor;
//		xQueueSend(pRxSensor_xQueue,( void * )&pSensor,( portTickType )10);
		//串口输出任务的状态
		//vTaskList((signed char *)(PAGE_BODY ));
		//vPrintString("%s",PAGE_BODY);
		//memset(PAGE_BODY,0x00,sizeof(PAGE_BODY));
		vTaskDelay(200);
	}
}

/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
extern xQueueHandle xQueue;
extern long lReceivedValue;
#if 0
void KeyScan4(void * pvParameters)
{
	long cmd;
	uint32_t key_value;
	portBASE_TYPE xStatus;
	JOYState_GPIO_Init();
	for(;;)
	{
		if((key_value = Read_JOYState())>0)
		{
			switch(key_value)
			{
				case JOY_LEFT:{cmd = SignInReq;};break;
				case JOY_UP:{cmd = MealComparReq;};break;
				case JOY_RIGHT:{cmd = StatuUploadReq;};break;
				case JOY_DOWN:{cmd = EchoReq;};break;
				case JOY_SEL:{cmd = TkMealReq;};break;
				default:{cmd = 0;}break;
			}
			//vPrintString("cmd = %04X,\r\n",cmd);
			xStatus = xQueueSend(xQueue,&cmd,5);
			if(xStatus!= pdPASS)
			{
				switch(xStatus)
				{
					case errQUEUE_FULL:vPrintString("errQUEUE_FULL.\r\n");break;
					case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:vPrintString("errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY.\r\n");break;
					case errNO_TASK_TO_RUN:vPrintString("errNO_TASK_TO_RUN.\r\n");break;
					case errQUEUE_BLOCKED:vPrintString("errQUEUE_BLOCKED.\r\n");break;
					case errQUEUE_YIELD:vPrintString("errQUEUE_YIELD.\r\n");break;
				}
			}
			else
			{
				taskYIELD();
				vTaskDelay(100);
			}
		}
		vTaskDelay(250);
	}
}
#else
void KeyScan4(void * pvParameters)
{
	portBASE_TYPE xStatus;
	
	JOYState_GPIO_Init();
	memcpy(Sensor.dev_id,"stm32_test",strlen("stm32_test"));
	Sensor.temperature[0] = 25.6;
	Sensor.latitude[0] = 143.5;
	Sensor.longitude[0] = 145.123;
	for(;;)
	{
		if((Read_JOYState())>0)
		{
			pSensor = &Sensor;
			xQueueSend(pRxSensor_xQueue,&pSensor,( portTickType )1);
			if(xStatus!= pdPASS)
			{
				switch(xStatus)
				{
					case errQUEUE_FULL:vPrintString("errQUEUE_FULL.\r\n");break;
					case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:vPrintString("errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY.\r\n");break;
					case errNO_TASK_TO_RUN:vPrintString("errNO_TASK_TO_RUN.\r\n");break;
					case errQUEUE_BLOCKED:vPrintString("errQUEUE_BLOCKED.\r\n");break;
					case errQUEUE_YIELD:vPrintString("errQUEUE_YIELD.\r\n");break;
					default:break;
				}
			}
			taskYIELD();
			vTaskDelay(100);
		}
		vTaskDelay(250);	
	}		
}
#endif
/**
  * @brief  Initializes the STM324xG-EVAL's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
void LCD_LED_Init(void)
{
#ifdef USE_LCD
  /* Initialize the STM324xG-EVAL's LCD */
	STM324xG_LCD_Init();
#endif

  /* Initialize STM324xG-EVAL's LEDs */
	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
 
#ifdef USE_LCD
	/* Clear the LCD */
	LCD_Clear(Black);

	/* Set the LCD Back Color */
	LCD_SetBackColor(Black);

	/* Set the LCD Text Color */
	LCD_SetTextColor(White);

	/* Display message on the LCD*/  
	LCD_DisplayStringLine(Line0, (uint8_t*)MESSAGE1);
	LCD_DisplayStringLine(Line1, (uint8_t*)MESSAGE2);
	LCD_DisplayStringLine(Line2, (uint8_t*)MESSAGE3);
	LCD_DisplayStringLine(Line3, (uint8_t*)MESSAGE4);  
#endif
}

#if 1	
typedef  void (*pFunction)(void);
uint32_t JumpAddress;
void iap_load_app(void)
{
	register pFunction Jump_To_Application;
	JOYState_GPIO_Init();
	if (Read_JOYState()==0)
	{	
		if (((*(__IO uint32_t*)USER_FLASH_FIRST_PAGE_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{
		  /* Jump to user application */
		  JumpAddress = *(__IO uint32_t*) (USER_FLASH_FIRST_PAGE_ADDRESS + 4);
		  Jump_To_Application = (pFunction) JumpAddress;
		  /* Initialize user application's Stack Pointer */
		  
		  __set_MSP(*(__IO uint32_t*) USER_FLASH_FIRST_PAGE_ADDRESS);
		  Jump_To_Application();
		}
		else
		{
		  /* Otherwise, do nothing */
		  STM_EVAL_LEDOn(LED3);
		  /* do nothing */
		  while(1);
		}	
	}
}

/*******************************************************************************  
* Function Name  : SystemReset  
* Description    : Configures the port pin connected to the push button. GPIO_D_4  
* Input          : None  
* Output         : None  
* Return         : None  
*******************************************************************************/  
void SoftReset(void) 
{  
	__set_FAULTMASK(1);      // 关闭所有中端
	 NVIC_SystemReset();	// 复位
}

#endif


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
	while (1)
	{}
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

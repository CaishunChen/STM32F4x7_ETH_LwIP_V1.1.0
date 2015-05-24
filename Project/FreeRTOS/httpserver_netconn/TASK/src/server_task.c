
#include <string.h>
#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "queue.h"
#include "task.h"
#include "server_task.h"
#include "tcp_server.h"
#include "uart5.h"

#define  SERVER_TASK_PRIO  ( tskIDLE_PRIORITY + 2 )

void Server_task(void * pvParameters)
{
	portBASE_TYPE xStatus;
	xQueueHandle xUpload;
	uint32_t lReceivedValue,lSendValue;
	const portTickType xTicksToWait =  1 / portTICK_RATE_MS;
	lSendValue = SignInReq;
	xUpload = xQueueCreate( 5, sizeof( uint32_t ) );
	xStatus = xQueueSend(xUpload,&lSendValue,10);
	lSendValue = MealComparReq;
	xStatus = xQueueSend(xUpload,&lSendValue,10);
	for(;;)
	{
		xStatus = xQueueReceive( xUpload,
								 &lReceivedValue,
								 xTicksToWait );
		if(xStatus == pdPASS)                    
		{
			switch(lReceivedValue)
			{
				case SignInReq:{
					auto uint8_t sign_in_buf[Totoal_SignIn_Lenth+ 8]={0};  //使用auto，该类具有自动存储期
					package_buff(SignInReq,sign_in_buf);   //将要发送的数据填入sendbuf    
					Uart5_Send(sign_in_buf,sizeof(sign_in_buf));  //发送数据
				};break;
				case MealComparReq:{
					static uint8_t meal_compar_buf[Totoal_MealCompar_Lenth+ 8]={0};  //使用auto，该类具有自动存储期
					package_buff(MealComparReq,meal_compar_buf);   //将要发送的数据填入sendbuf  
					Uart5_Send(meal_compar_buf,sizeof(meal_compar_buf));  //发送数据                  
				};break;
				case StatuUploadReq:{
					auto uint8_t statu_upload_buf[Totoal_StatuUpload_Lenth+ 8]={0};  //使用auto，该类具有自动存储期
					package_buff(StatuUploadReq,statu_upload_buf);   //将要发送的数据填入sendbuf    
					Uart5_Send(statu_upload_buf,sizeof(statu_upload_buf));  //发送数据                                
				};break;
				case TkMealReq:{
					auto uint8_t tk_meal_buf[Tk_Meal_Lenth+ 8]={0};  //使用auto，该类具有自动存储期
					package_buff(StatuUploadReq,tk_meal_buf);   //将要发送的数据填入sendbuf    
					Uart5_Send(tk_meal_buf,sizeof(tk_meal_buf));  //发送数据   					
				};break;
				case EchoReq:{
					auto uint8_t echo_buf[Totoal_Echo_Lenth+ 8]={0};  //使用auto，该类具有自动存储期
					package_buff(EchoReq,echo_buf);   //将要发送的数据填入sendbuf    
					Uart5_Send(echo_buf,sizeof(echo_buf));  //发送数据  					
				};break;
				default:break;
			}
		}
		if(RX5_RCV)
		{
			decode_host_data(RX5_BUF);
			RX5_RCV = false;
			memset(RX5_BUF,0,sizeof(RX5_BUF));
			//lSendValue = MealComparReq;
			//xStatus = xQueueSend(xUpload,&lSendValue,10);
		}
		vTaskDelay(400);
	}
}

void ServerInit(void)
{
	xTaskCreate(Server_task,(int8_t *)"Server", configMINIMAL_STACK_SIZE , NULL,SERVER_TASK_PRIO, NULL);
}


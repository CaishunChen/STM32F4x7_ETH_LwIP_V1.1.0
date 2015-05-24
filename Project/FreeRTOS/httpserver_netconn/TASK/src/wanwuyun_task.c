
#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "queue.h"
#include "task.h"
#include "wanwuyun_task.h"
#include "cjson.h"
#include "serial_debug.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"

#define  WANWUYUN_TASK_PRIO  ( ( configMAX_PRIORITIES - 6 ))

char SignInResponse[]="{	\"code\":	\"0\",	\"seckey\":	\"HgqoOLTlav5jTsefyj3nL5AkRu8UAFRf\"	}";
char DataUpResponse[]="{	\"code\":	\"0\"	}";
xQueueHandle pRxSensor_xQueue;
void wanwuyun_task(void * pvParameters)
{
	//创建Queue
	SENSOR_STRUCT *pRxSensorFrame;
	portBASE_TYPE  xStatus; 
	struct ip_addr WanWuYunIPaddr;
	struct netconn *wanwuyun_clientconn;
	static err_t err,recv_err;
	
	//网络相关结构体
	struct   netbuf *inbuf;
	uint8_t  *buf;
	uint16_t buflen;	
	bool wan_send_success_flag = false;	
	
	//创建Json的结构体
	cJSON   *SignInReqJson ,*SignInResJson, *DataUpReqJson, *RawJson, *DataUpResJson;
	char 	*DataOut;
	
#if 0	
	SignInReqJson=cJSON_CreateObject();
	cJSON_AddStringToObject(SignInReqJson, "username", "jackeyjiang");
	cJSON_AddStringToObject(SignInReqJson, "accessid", "AMFJA2V5AMLHBMCXNDI5NZE2NDIXMTMW");
	cJSON_AddStringToObject(SignInReqJson, "appid", "9785739981");
	cJSON_AddStringToObject(SignInReqJson, "dev_id", "stm32_test");
	DataOut = cJSON_Print(DataUpReqJson);
	cJSON_Delete(SignInReqJson);
	free(DataOut);
	
		
	DataUpReqJson=cJSON_CreateObject();
	cJSON_AddStringToObject(DataUpReqJson, "seckey", "HgqoOLTlav5jTsefyj3nL5AkRu8UAFRf");
	cJSON_AddItemToObject(DataUpReqJson, "row", RawJson=cJSON_CreateObject());
	cJSON_AddStringToObject(RawJson, "DEV_ID", "stm32_test");
	cJSON_AddNumberToObject(RawJson, "TEMPERATURE", 24.5);
	cJSON_AddNumberToObject(RawJson, "LATITUDE", 11.567);
	cJSON_AddNumberToObject(RawJson, "LONGITUDE", 134.5);
	
	cJSON_Delete(DataUpReqJson);

	//解析返回的Json数据
	SignInResJson = cJSON_Parse(SignInResponse);
	if (!SignInResJson) 
	vPrintString("Error before: [%s]\n",cJSON_GetErrorPtr());
	else {};
	DataUpResJson = cJSON_Parse(DataUpResponse);
	if (!DataUpResJson) vPrintString("Error before: [%s]\n",cJSON_GetErrorPtr());
	else {};	
#endif	
	//创建传感器队列
	pRxSensor_xQueue = xQueueCreate( 2, sizeof(  struct _SENSOR_STRUCT * ) );

	//建立短连接，接收到队列传过来的数据，将其打包成json数据传送到万物云。
	IP4_ADDR( &WanWuYunIPaddr, WANWUYUN_IP_ADDR0, WANWUYUN_IP_ADDR1, WANWUYUN_IP_ADDR2, WANWUYUN_IP_ADDR3 );
	for(;;)
	{
		xStatus = xQueueReceive(pRxSensor_xQueue,&(pRxSensorFrame),( portTickType ) 10 );
		if(xStatus == pdPASS)
		{
			//提取结构体中的数据
			DataUpReqJson=cJSON_CreateObject();
			cJSON_AddStringToObject(DataUpReqJson, "seckey", "HgqoOLTlav5jTsefyj3nL5AkRu8UAFRf");
			cJSON_AddItemToObject(DataUpReqJson, "row", RawJson=cJSON_CreateObject());
			cJSON_AddStringToObject(RawJson, "DEV_ID", "stm32_test");
			cJSON_AddNumberToObject(RawJson, "TEMPERATURE", pRxSensorFrame->temperature[0]);
			cJSON_AddNumberToObject(RawJson, "LATITUDE", pRxSensorFrame->latitude[0]);
			cJSON_AddNumberToObject(RawJson, "LONGITUDE", pRxSensorFrame->longitude[0]);
			//转换数据为cJSON数据
			DataOut = cJSON_Print(RawJson);
			cJSON_Delete(DataUpReqJson);
			vPortFree(DataOut);
			
			while(1)
			{
			  /* Create a new connection identifier. */
				wanwuyun_clientconn = netconn_new(NETCONN_TCP);
				if (wanwuyun_clientconn!=NULL)
				{  
					/*built a connect to server*/
					err = netconn_connect(wanwuyun_clientconn,&WanWuYunIPaddr,WANWUYUN_SERVER_PORT);
					if (err != ERR_OK)  netconn_delete(wanwuyun_clientconn); 
					else if (err == ERR_OK)
					{
						/*timeout to wait for new data to be received <Avoid death etc.> */
						netconn_set_sendtimeout(wanwuyun_clientconn,10);
						netconn_set_recvtimeout(wanwuyun_clientconn,800);
						while(1)
						{
							err=netconn_write(wanwuyun_clientconn,\
								DataOut,sizeof(DataOut),\
								NETCONN_COPY);
							if(err != ERR_OK)  
								printf("StatuUploadReq erro code is %d\r\n",err);
							recv_err = netconn_recv(wanwuyun_clientconn, &inbuf);
							if (recv_err == ERR_OK)
							{
								if (netconn_err(wanwuyun_clientconn) == ERR_OK)
								{ 
									netbuf_data(inbuf, (void**)&buf, &buflen);
									//decode_host_data(buf);
									DataUpResJson = cJSON_Parse((char *)buf);
									netbuf_delete(inbuf);
									wan_send_success_flag = false;	
									//使用短链接，成功后跳出while(1)
									break;																										 
								}
								else
								{
									printf("recv_err != ERR_OK \r\n");
								}
							}
							else if((recv_err == ERR_TIMEOUT)&&(wan_send_success_flag == true)) 
							{
								wan_send_success_flag = false;
								printf("recv_err == %d\r\n",recv_err);
								netconn_close(wanwuyun_clientconn);
								netbuf_delete(inbuf);
								netconn_delete(wanwuyun_clientconn);
								break;
							}								
						}
					}
				}
			}					
		}
	}
}



void Wanwuyun_Init(void)
{
	xTaskCreate(wanwuyun_task,(int8_t *)"Wanwuyun", DEFAULT_THREAD_STACKSIZE * 4 , NULL,WANWUYUN_TASK_PRIO, NULL);
}

#endif


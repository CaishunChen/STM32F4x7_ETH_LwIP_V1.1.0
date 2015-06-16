
#include <string.h>
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

#define  WANWUYUN_TASK_PRIO  ( ( configMAX_PRIORITIES - 5 ) )

char SignInResponse[]="{	\"code\":	\"0\",	\"seckey\":	\"HgqoOLTlav5jTsefyj3nL5AkRu8UAFRf\"	}";
char DataUpResponse[]="{	\"code\":	\"0\"	}";
xQueueHandle pRxSensor_xQueue;

static void wanwuyun_callback(struct netconn *conn, enum netconn_evt evt, u16_t len);

struct netconn *wanwuyun_clientconn;

void wanwuyun_task(void * pvParameters)
{
	//创建Queue
	SENSOR_STRUCT *pRxSensorFrame;
	portBASE_TYPE  xStatus; 
	struct ip_addr WanWuYunIPaddr;
	static err_t err,recv_err;
	
	//网络相关结构体
	struct   netbuf *inbuf;
	uint8_t  *buf;
	uint16_t buflen;
	//成功发送 = true,其他状态为 = false	
	bool wan_send_success_flag = false;	
	
	//创建Json的结构体
	cJSON   *DataUpReqJson, *RowJson, *DataUpResJson,*fld;
	char 	*DataOut;
	
	char double_string[]={0};
#if 0	
	cJSON   *SignInReqJson ,*SignInResJson;
	SignInReqJson=cJSON_CreateObject();
	cJSON_AddStringToObject(SignInReqJson, "username", "jackeyjiang");
	cJSON_AddStringToObject(SignInReqJson, "accessid", "AMFJA2V5AMLHBMCXNDI5NZE2NDIXMTMW");
	cJSON_AddStringToObject(SignInReqJson, "appid", "9785739981");
	cJSON_AddStringToObject(SignInReqJson, "dev_id", "stm32_test");
	DataOut = cJSON_Print(DataUpReqJson);
	cJSON_Delete(SignInReqJson);
	vPortFree(DataOut);
	
	DataUpReqJson=cJSON_CreateArray();
	cJSON_AddItemToObject(DataUpReqJson,NULL,fld = cJSON_CreateObject());
	cJSON_AddStringToObject(fld, "seckey", "HgqoOLTlav5jTsefyj3nL5AkRu8UAFRf");
	cJSON_AddItemToObject(fld, "row", RowJson=cJSON_CreateObject());
	cJSON_AddStringToObject(RowJson, "DEV_ID", "stm32_test");
	cJSON_AddNumberToObject(RowJson, "TEMPERATURE", 12.5);
	cJSON_AddNumberToObject(RowJson, "LATITUDE", 12.7);
	cJSON_AddNumberToObject(RowJson, "LONGITUDE", 12.8);
	//转换数据为cJSON数据
	DataOut = cJSON_Print(DataUpReqJson);
	cJSON_Delete(DataUpReqJson);
	printf("%s",DataOut);
	vPortFree(DataOut);	

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
	
	while(1)
	{
		/* Create a new connection identifier. */
		STM_EVAL_LEDOff(LED2);
		wanwuyun_clientconn = netconn_new_with_callback(NETCONN_TCP,wanwuyun_callback); //测试
		if (wanwuyun_clientconn!=NULL)
		{  
			/*built a connect to wanwuyun.com server*/
			//netconn_set_nonblocking(wanwuyun_clientconn, 1); //测试
			err = netconn_connect(wanwuyun_clientconn,&WanWuYunIPaddr,WANWUYUN_SERVER_PORT);
			if (err != ERR_OK)  netconn_delete(wanwuyun_clientconn); 
			else if (err == ERR_OK)
			{
				STM_EVAL_LEDOn(LED2);
				vPrintString("netconn_connect wanwuyun_clientconn\r\n");
				/*timeout to wait for new data to be received <Avoid death etc.> */
				netconn_set_sendtimeout(wanwuyun_clientconn,300);
				netconn_set_recvtimeout(wanwuyun_clientconn,700);
				while(!ERR_IS_FATAL(wanwuyun_clientconn->last_err)) 
				{
					STM_EVAL_LEDToggle(LED3);
					xStatus = xQueueReceive(pRxSensor_xQueue,&(pRxSensorFrame),( portTickType ) 1 );
					if(xStatus == pdPASS)
					{	
						//提取结构体中的数据
						DataUpReqJson=cJSON_CreateArray();
						cJSON_AddItemToObject(DataUpReqJson,NULL,fld = cJSON_CreateObject());
						cJSON_AddStringToObject(fld, "seckey", "HgqoOLTlav5jTsefyj3nL5AkRu8UAFRf");
						cJSON_AddItemToObject(fld, "row", RowJson=cJSON_CreateObject());
						cJSON_AddStringToObject(RowJson, "DEV_ID", "stm32_test");
						sprintf(double_string,"%f",pRxSensorFrame->temperature[0]);
						cJSON_AddStringToObject(RowJson, "TEMPERATURE", double_string);
						sprintf(double_string,"%f",pRxSensorFrame->latitude[0]);
						cJSON_AddStringToObject(RowJson, "LATITUDE", double_string);
						sprintf(double_string,"%f",pRxSensorFrame->longitude[0]);
						cJSON_AddStringToObject(RowJson, "LONGITUDE", double_string);
						//转换数据为cJSON数据
						DataOut = cJSON_Print(DataUpReqJson);
						//printf("%d",strlen(DataOut));
						cJSON_Delete(DataUpReqJson);
						vPrintString("%s\r\n",DataOut);
						//vPortFree(DataOut);					
						err=netconn_write(wanwuyun_clientconn,\
							DataOut,strlen(DataOut),\
							NETCONN_COPY);	
						if(err != ERR_OK)  
						{
							vPortFree(DataOut);
							vPrintString("StatuUploadReq erro code is %d\r\n",err);
							break;
						}	
						else
						{
							wan_send_success_flag = true; //表示数据已经发送了
							vPortFree(DataOut);	
						}
					}
					//netconn_recved(wanwuyun_clientconn,100); //测试
					recv_err = netconn_recv(wanwuyun_clientconn, &inbuf);
					if (recv_err == ERR_OK)
					{
						if (netconn_err(wanwuyun_clientconn) == ERR_OK)
						{ 
							netbuf_data(inbuf, (void**)&buf, &buflen);
							DataUpResJson = cJSON_Parse((char *)buf);
							DataOut = cJSON_Print(DataUpResJson);
							vPrintString("%s\r\n",DataOut);
							netbuf_delete(inbuf);
							wan_send_success_flag = false;
							//使用短链接，成功后跳出while(1)
							//break;
						}
						else
						{
							vPrintString("recv_err != ERR_OK \r\n");
						}
					}
					#if 1  //测试当断开连接的时候的状态
					else if((recv_err == ERR_TIMEOUT)&&(wan_send_success_flag == true)) 
					{
						wan_send_success_flag = false;
						vPrintString("recv_err == %d\r\n",recv_err);
						netconn_close(wanwuyun_clientconn);
						netbuf_delete(inbuf);
						netconn_delete(wanwuyun_clientconn);
						//为发送的数据重新入队
						xQueueSendToFront(pRxSensor_xQueue,&(pRxSensorFrame),( portTickType )1);
						break;
					}
					#endif
				}
			}
		}
	}	
}
/**
 * Callback registered in the netconn layer for each socket-netconn.
 * Processes recvevent (data available) and wakes up tasks waiting for select.
   可以通过当前的状态，进行网络通信正常的判断
 */
static void
wanwuyun_callback(struct netconn *pconn, enum netconn_evt event, u16_t len)
{
    switch(event) 
	{
		case NETCONN_EVT_RCVPLUS:
		{
			if(len > 0) 
			{
				vPrintString("NETCONN_EVT_RCVPLUS\r\n");
				//HTTPD_POST_MESSAGE(pconn, len, HTTPD_CMD_READRDY);
			}
			else if(pconn == wanwuyun_clientconn) 
			{
				vPrintString("queue message noting that a connection is requested\r\n");
				/* queue message noting that a connection is requested, will accept when ready */
			} /* otherwise this is caused by closing the socket ... */
		}break;
		case NETCONN_EVT_RCVMINUS:  
		{
			vPrintString("NETCONN_EVT_RCVMINUS\r\n");
		}break; /* not useful? */
		case NETCONN_EVT_SENDPLUS:
		{
			if(len > 0) /* otherwise doesn't seem to be useful */
			{
				vPrintString("NETCONN_EVT_SENDPLUS\r\n");
				//HTTPD_POST_MESSAGE(pconn, len, HTTPD_CMD_TXDONE);
			}
		}break;
		case NETCONN_EVT_SENDMINUS:
		{
			vPrintString("NETCONN_EVT_SENDMINUS\r\n");
		}
		break; /* not useful? */
		case NETCONN_EVT_ERROR:  
		{
			vPrintString("NETCONN_EVT_ERROR\r\n");
		}break;// HTTPD_POST_MESSAGE(pconn, len, HTTPD_CMD_ERROR); 	
		default:
		{
			vPrintString("Default\r\n");
		}break;
	}
	
}


void Wanwuyun_Init(void)
{
	xTaskCreate(wanwuyun_task,(int8_t *)"Wan", DEFAULT_THREAD_STACKSIZE , NULL,WANWUYUN_TASK_PRIO, NULL);
}

#endif


#ifndef __wanwuyun_task__
#define __wanwuyun_task__

#include  <stdint.h>
#include "queue.h"

/*串口帧结构体*/
typedef struct _SENSOR_STRUCT //
{
	uint8_t dev_id[12];      //帧头
	double  temperature[1];    //帧长度
	double  latitude[1];   //帧命令
	double 	longitude[1];  //帧变量
}SENSOR_STRUCT;

extern xQueueHandle pRxSensor_xQueue;

void Wanwuyun_Init(void);

/* 万物云服务器地址*/
  #define WANWUYUN_IP_ADDR0  	218
  #define WANWUYUN_IP_ADDR1  	94
  #define WANWUYUN_IP_ADDR2  	134
  #define WANWUYUN_IP_ADDR3  	66
/* 端口 */
  #define WANWUYUN_SERVER_PORT  8913

#endif


#include <string.h>
#include <stm32f4xx.h>
#include "freeRTOS.h"
#include "task.h"
#include "record_task.h"
#include "diskio.h"
#include "ff.h"
#include "flash.h"

#define  RECORD_TASK_PRIO  ( tskIDLE_PRIORITY + 4 )

#define USER_RECORD_STAR   (6*4096)
#define USER_RECORD_END    (6*4096 + 4096/2) 
uint8_t tx_buff[]={"stm32 flash fatfs test\r\n"};
uint8_t rx_buff[20]={0};
void Record_task(void * pvParameters)
{
	static FATFS fs_sd;
	static FIL fil_sd;
	static FATFS fs_flash;
	static FIL fil_flash;
	static FRESULT res;
	uint32_t   br = 0;
	uint32_t   bw = 0;
	static xTaskHandle xHandle;
	static uint8_t fbuf[512] = {0};
	
	/*flash擦除*/
	//SPI_Flash_Init();
	//SPI_Flash_Erase_Chip();	
	
	
	res = f_mount(EX_FLASH,&fs_flash);
	res = f_mount(SD_CARD,&fs_sd);
#if 0
	res = f_mkfs(EX_FLASH , 0, _MAX_SS);  //格式化4s
	res = f_open(&fil_sd,"0:config.txt",FA_READ);
	res = f_read(&fil_sd,fbuf,512,&br);
	res = f_open(&fil_flash,"1:config.txt",FA_WRITE|FA_CREATE_ALWAYS); 
	res = f_write(&fil_flash,fbuf,512,&bw);
	f_close(&fil_sd);
	f_close(&fil_flash);
#endif	
	//res = f_unlink("config.txt");
	//res = f_close(&fil_sd);
	  
	//res = f_open(&fil_flash,"1:config.txt",FA_WRITE); 
	//res = f_read(&fil_flash,"1:config.txt",512,&br);
	//res = f_printf(&fil_flash,"%d",1234); 
	//res = f_write(&fil_flash,tx_buff,256,&br);// write 256 Bytes
	//res = f_close(&fil_flash); 
	memset(fbuf,0,sizeof(fbuf));  
	res = f_open(&fil_flash,"1:config.txt",FA_READ);
	res = f_read(&fil_flash,fbuf,512,&br);
	res = f_close(&fil_flash); 
	  
	//需要初始化才行，SPI_Flash_Init 不行(why?)
//	SPI_Flash_Write(tx_buff,USER_RECORD_STAR,sizeof(tx_buff));
//	SPI_Flash_Read(rx_buff,USER_RECORD_STAR,sizeof(tx_buff)); 	
	
	for(;;)
	{
		vTaskDelete(xHandle);
	}
	
}



void RecordInit(void)
{
	xTaskCreate(Record_task,(int8_t *)"Record", configMINIMAL_STACK_SIZE , NULL,RECORD_TASK_PRIO, NULL);
}


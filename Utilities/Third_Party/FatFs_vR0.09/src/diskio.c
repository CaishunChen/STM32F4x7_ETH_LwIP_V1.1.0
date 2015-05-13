#include "mmc_sd.h"
#include "diskio.h"
#include "flash.h"	 		   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//FATFS disio.c 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/18
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


#define FLASH_SECTOR_SIZE 	512			  //每个block有512字节,有八个blook 
//对于W25Q64 
//前6M字节给fatfs用,6M字节后~6M+500K给用户用,6M+500K以后,用于存放字库,字库占用1.5M.		 			    
uint16_t  FLASH_SECTOR_COUNT =  2048*6;  //6M字节,默认为W25Q64
#define FLASH_BLOCK_SIZE  	8            //每个BLOCK有8个扇区



//初始化磁盘
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
	uint8_t res=0;	    
	switch(drv)
	{
		case SD_CARD://SD卡
		{
			res = SD_Initialize();
		 	if(res)//STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常
			{
                return STA_NODISK;
			}
  			break;
  		}
		case EX_FLASH://外部flash
		{
			SPI_Flash_Init();
			if(SPI_FLASH_TYPE==W25Q64)FLASH_SECTOR_COUNT=2048*6;//W25Q64
			else FLASH_SECTOR_COUNT=2048*2;						//其他
 			break;
 		}
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功
}   
//获得磁盘状态
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{		   
    return 0;
}
 //读扇区
 //drv:磁盘编号0~9
 //*buff:数据接收缓冲首地址
 //sector:扇区地址
 //count:需要读取的扇区数
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	uint8_t res=0; 
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(drv)
	{
		case SD_CARD://SD卡
		{
			res=SD_ReadDisk(buff,sector,count);	 
		 	if(res)//STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常
			{
				//SD_SPI_SpeedLow();
				SD_SPI_ReadWriteByte(0xff);//提供额外的8个时钟
				//SD_SPI_SpeedHigh();
			}
		}break;	
		case EX_FLASH://外部flash
		{
			for(;count>0;count--)
			{
				SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
		}break;
		default:
			res=1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}  
 //写扇区
 //drv:磁盘编号0~9
 //*buff:发送数据首地址
 //sector:扇区地址
 //count:需要写入的扇区数	    
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	        /* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	uint8_t res=0;  
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(drv)
	{
		case SD_CARD://SD卡
		{
			res=SD_WriteDisk((uint8_t*)buff,sector,count);
		}break;
		case EX_FLASH://外部flash
		{
			for(;count>0;count--)
			{										    
				SPI_Flash_Write((uint8_t*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
		}break;
		default:
			res=1; 
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;		 
}
#endif /* _READONLY */

//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{	
	DRESULT res;						  			     
	if(drv==SD_CARD)//SD卡
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
		    {
				Reset_SD_CS;
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				Set_SD_CS;
		    }break;	 
		    case GET_SECTOR_SIZE:
		    {
		        *(WORD*)buff = 512;
		        res = RES_OK;
		    }break;	 
		    case GET_BLOCK_SIZE:
		    {
		        *(WORD*)buff = 8;
		        res = RES_OK;
		    }break;	 
		    case GET_SECTOR_COUNT:
		    {
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		    }break;
		    default:
		    {
		        res = RES_PARERR;
		    }break;
	    }
	}else if(drv==EX_FLASH)	//外部FLASH  
	{
	    switch(ctrl)
	    {
            case CTRL_SYNC:
           {
				res = RES_OK; 
            }break;	 
		    case GET_SECTOR_SIZE:
            {
               *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		    }break;	 
		    case GET_BLOCK_SIZE:
		    {
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		    }break;	 
		    case GET_SECTOR_COUNT:
		    {
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		    }break;
		    default:
		    {
		        res = RES_PARERR;
		    }break;
	    }
	}
	else 
        res=RES_ERROR;//其他的不支持
        return res;
}   
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 














#include "mmc_sd.h"
#include "diskio.h"
#include "flash.h"	 		   

#include "usb_conf.h"
#include "diskio.h"
#include "usbh_msc_core.h"

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;


//对于W25Q64 前6M字节给fatfs用,6M字节后~6M+500K给用户用,6M+500K以后,用于存放字库,字库占用1.5M.		
#define FLASH_SECTOR_SIZE 	512			  //每个block有512字节,有八个blook 
#define FLASH_BLOCK_SIZE  	8            //每个BLOCK有8个扇区
 			    
uint16_t  FLASH_SECTOR_COUNT =  2048*6;  //6M字节,默认为W25Q64



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
 		case USB_DISK://U盘
 		{
 			if(HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				Stat &= ~STA_NOINIT;
			}
			return Stat;
 		}
		default:
		{
			res=1; 
		}break;
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功
}   
//获得磁盘状态
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{		   
  //if (drv) return STA_NOINIT;		//仅支持单磁盘操作，drv =0,1,2，否则返回参数错误
  return Stat;
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
	switch(drv)
	{
		case SD_CARD://SD卡
		{

			if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		
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
			if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		
			for(;count>0;count--)
			{
				SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
		}break;
		case USB_DISK: //U盘
		{
			BYTE status = USBH_MSC_OK;
			  
			//if (drv || !count) return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
			if (!count) return RES_PARERR;
			if (Stat & STA_NOINIT) return RES_NOTRDY;
			  
			if(HCD_IsDeviceConnected(&USB_OTG_Core))
			{   
			    do
			    {
			      	status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512 * count);
			      	USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);

				    if(!HCD_IsDeviceConnected(&USB_OTG_Core))
				    { 
				        return RES_ERROR;
				    }      
			    }
			    while(status == USBH_MSC_BUSY );
			}
			if(status == USBH_MSC_OK)
			    return RES_OK;
			return RES_ERROR;
		}break;
		default:
		{
			res=1; 
		}break;
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
		case USB_DISK://U盘
		{
			BYTE status = USBH_MSC_OK;
			//if (drv || !count) return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
			if (!count) return RES_PARERR;  
			if (Stat & STA_NOINIT) return RES_NOTRDY;
			if (Stat & STA_PROTECT) return RES_WRPRT;

			if(HCD_IsDeviceConnected(&USB_OTG_Core))
			{  
			    do
			    {
			    	status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512 * count);
			    	USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
			      
			    	if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			    	{ 
			    		return RES_ERROR;
			    	}
			    }
		    while(status == USBH_MSC_BUSY ); 
			}
			if(status == USBH_MSC_OK)
				return RES_OK;
			return RES_ERROR;			
		}
		default:
		{
			res=1; 
		}break;
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
	else if (drv==USB_DISK)
	{
		DRESULT res = RES_OK;

		if (drv) return RES_PARERR;

		res = RES_ERROR;

		if (Stat & STA_NOINIT) return RES_NOTRDY;

		switch (ctrl) 
		{
			case CTRL_SYNC :
			{
				res = RES_OK;
			}break;		/* Make sure that no pending write process */
			case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			{
				*(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
				res = RES_OK;
			}break;
			case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			{
				*(WORD*)buff = 512;
				res = RES_OK;
			}break;
			case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			{
				*(DWORD*)buff = 512;
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
	uint32_t ttime;
	uint32_t date=0;
	
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;
    
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	
    ttime = RTC_DateStructure.RTC_Year+20;	
 	date |= ttime<<25;
    ttime = RTC_DateStructure.RTC_Month;	
 	date |= ttime<<21;
	ttime = RTC_DateStructure.RTC_Date;	  	
 	date |= ttime<<16;
	ttime = RTC_TimeStructure.RTC_Hours;  	
 	date |= ttime<<11;
	ttime = RTC_TimeStructure.RTC_Minutes;	
 	date |= ttime<<5;
 	date |= RTC_TimeStructure.RTC_Seconds>>1;
	
    return date;
}			 














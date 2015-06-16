/**
	******************************************************************************
	* @file    flash_if.c 
	* @author  MCD Application Team
	* @version V1.0.0
	* @date    31-October-2011
	* @brief   This file provides high level routines to manage internal Flash 
	*          programming (erase and write). 
	******************************************************************************
	* @attention
	*
	* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
	* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
	* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
	* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
	* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
	* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
	*
	* <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
	******************************************************************************
	*/

/* Includes ------------------------------------------------------------------*/
#include "intel_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
#if 0
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/

#define DATA_32                 ((uint32_t)0x12345678)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t StartSector = 0, EndSector = 0, Address = 0, SectorCounter = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
	
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint32_t GetSector(uint32_t Address);
/**
	* @brief  Unlocks Flash for write access
	* @param  None
	* @retval None
	*/
void FLASH_If_Init(void)
{ 
	FLASH_Unlock(); 
}

/**
	* @brief  This function does an erase of all user flash area
	* @param  StartSector: start of user flash area
	* @retval 0: user flash area successfully erased
	*         1: error occurred
	*/
int8_t FLASH_If_Erase(uint32_t StartSector)
{
	uint32_t FlashAddress;
	
	FlashAddress = StartSector;

	/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		 be done by word */ 
 
	if (FlashAddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
	{
		FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3); /* 128 Kbyte */
		FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3); /* 128 Kbyte */
		FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3); /* 128 Kbyte */
		FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3); /* 128 Kbyte */
		FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3); /* 128 Kbyte */
		FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); /* 128 Kbyte */
	}
	else
	{
		return (1);
	}

	return (0);
}
/**
	* @brief  This function writes a data buffer in flash (data are 32-bit aligned).
	* @note   After writing data buffer, the flash content is checked.
	* @param  FlashAddress: start address for writing data buffer
	* @param  Data: pointer on data buffer
	* @param  DataLength: length of data buffer (unit is 32-bit word)   
	* @retval 0: Data successfully written to Flash memory
	*         1: Error occurred while writing data in Flash memory
	*         2: Written Data in flash memory is different from expected one
	*/
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
	uint32_t i = 0;

	for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
			 be done by word */ 
		if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
		{
		 /* Check the written value */
			if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
			{
				/* Flash content doesn't match SRAM content */
				return(2);
			}
			/* Increment FLASH destination address */
			*FlashAddress += 4;
		}
		else
		{
			/* Error occurred while writing data in Flash memory */
			return (1);
		}
	}

	return (0);
}



uint32_t Flash_Write(__IO uint32_t FlashAddress, uint8_t* ptr ,uint16_t iNbrToWrite)
{
	int cnt_t=0;
	uint32_t iAddress_temp=0;
	/*!< At this stage the microcontroller clock setting is already configured, 
			 this is done through SystemInit() function which is called from startup
			 file (startup_stm32f4xx.s) before to branch to application main.
			 To reconfigure the default setting of SystemInit() function, refer to
			 system_stm32f4xx.c file
		 */

	/* Get the number of the start and end sectors */
	StartSector = GetSector(FlashAddress);
	EndSector =   GetSector(FlashAddress+iNbrToWrite);
	iAddress_temp = FlashAddress; 
	
	/* Unlock the Flash to enable the flash control register access *************/ 	  	
	FLASH_Unlock();
		
	/* Erase the user Flash area
		(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
									FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

	StartSector = GetSector(FlashAddress);

	for (cnt_t = StartSector; cnt_t < EndSector; cnt_t += 1)
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
			 be done by word */ 
		if (FLASH_EraseSector(cnt_t, VoltageRange_1) != FLASH_COMPLETE)
		{ 
			/* Error occurred while sector erase. 
				 User can add here some code to deal with this error  */
			while (1)
			{
			}
		}
	}
//     StartSector = GetSector(FlashAddress);

//     for (SectorCounter = StartSector; SectorCounter < (StartSector+iNbrToWrite); SectorCounter += 1)
//     {
//		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
//		   be done by word */ 
//		if (FLASH_EraseSector(SectorCounter, VoltageRange_1) != FLASH_COMPLETE)
//		{ 
//		  /* Error occurred while sector erase. 
//			 User can add here some code to deal with this error  */
//		  while (1)
//		  {
//		  }
//     }
//  }

	/* Program the user Flash area word by word
		(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	while (FlashAddress < (iAddress_temp+iNbrToWrite))
	{
		if (FLASH_ProgramByte(FlashAddress, *ptr++) == FLASH_COMPLETE)
		{
			FlashAddress =FlashAddress + 1;
		}
		else
		{ 
			/* Error occurred while writing data in Flash memory. 
				 User can add here some code to deal with this error */
			while (1)
			{
			}
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
		 to protect the FLASH memory against possible unwanted operation) *********/
	FLASH_Lock();	
	return cnt_t;
}



/**
	* @brief  Gets the sector of a given address
	* @param  None
	* @retval The sector of a given address
	*/
uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;
	
	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
	{
		sector = FLASH_Sector_11;  
	}

	return sector;
}
#endif
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STM32内部FLASH读写 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

 
 
//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(vu32*)faddr; 
}  
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
uint16_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}
//从指定地址开始写入指定长度的数据
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void STMFLASH_Write(uint32_t WriteAddr,uint8_t *pBuffer,uint32_t NumToWrite)	
{ 
	FLASH_Status status = FLASH_COMPLETE;
	uint32_t addrx=0;
	uint32_t endaddr=0;	
	uint32_t word_temp=0;
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
	FLASH_Unlock();									//解锁 
	FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
		
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite;	//写入的结束地址
	if(addrx<0X1FFF0000)			//只有主存储区,才需要执行擦除操作!!
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V之间!!
				if(status!=FLASH_COMPLETE)break;	//发生错误了
			}else addrx+=4;
		} 
	}
	if(status==FLASH_COMPLETE)
	{
		while(WriteAddr<endaddr)//写数据
		{
			word_temp = (*(pBuffer+3))<<24;
			word_temp+= (*(pBuffer+2))<<16;
			word_temp+= (*(pBuffer+1))<<8;
			word_temp+= (*(pBuffer+0));
			if(FLASH_ProgramWord(WriteAddr,word_temp)!=FLASH_COMPLETE)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer+=4;
		} 
	}
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
	uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}















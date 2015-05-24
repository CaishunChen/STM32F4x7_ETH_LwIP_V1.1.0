
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stm32f4xx.h>
#include "rtc.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */ /* LSI used as RTC source clock. The RTC Clock
									  may varies due to LSI frequency dispersion. */ 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef RTC_InitStructure;

RTC_TimeTypeDef  RTC_TimeStampStructure;
RTC_DateTypeDef  RTC_TimeStampDateStructure;
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

uint8_t AppRTC_Init(void)
{
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)//未设置时间
	//if(1)
	{
		RTC_Config();
		RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;//RTC 异步除数 （<0X7F）
		RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;//RTC 同步除数 （<0X7FFF）
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24小时制
		if(RTC_Init(&RTC_InitStructure) == ERROR) return 1;
		RTC_TimeRegulate();//设置时间 
	}
	else  //已设置时间
	{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET){}
			
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		PWR_BackupAccessCmd(ENABLE);//使能RTC操作
		RTC_WaitForSynchro();//等待RTC APB寄存器同步
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除RTC闹钟标志
	}
	return 0;
}

/*******************************************************************************
 * 函数名称:RTC_Config  配置RTC                                                               
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2015年5月13日                                                                    
 *******************************************************************************/
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;
    
#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Enable The TimeStamp */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);    
}


/*******************************************************************************
 * 函数名称:RTC_TimeShow  获取时间                                                              
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2015年5月13日                                                                    
 *******************************************************************************/
void RTC_TimeShow(DATA_TIME_STRUCT *pDataTime)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
	pDataTime->Hours    =RTC_TimeStructure.RTC_Hours;
	pDataTime->Minutes  =RTC_TimeStructure.RTC_Minutes;
	pDataTime->Senconds =RTC_TimeStructure.RTC_Seconds;
	pDataTime->WeekDay  =RTC_DateStructure.RTC_WeekDay;
	pDataTime->Date     =RTC_DateStructure.RTC_Date;
	pDataTime->Month    =RTC_DateStructure.RTC_Month;
	pDataTime->Year     =RTC_DateStructure.RTC_Year;
	/* Unfreeze the RTC DR Register */
   (void)RTC->DR;
}

/*******************************************************************************
 * 函数名称:RTC_TimeRegulate  设置时间                                                                
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2015年5月13日                                                                    
 *******************************************************************************/

uint8_t tmp_hour = 0x23, tmp_minute = 0x07, tmp_second = 0x30;
uint8_t tmp_data = 0x21, tmp_week = 0x02  , tmp_month  = 0x05, tmp_year   = 0x15; 

void RTC_TimeRegulate(void)
{
	//配置时间
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_TimeStructure.RTC_H12= RTC_HourFormat_24;		
	RTC_TimeStructure.RTC_Hours = tmp_hour;
	RTC_TimeStructure.RTC_Minutes = tmp_minute;
	RTC_TimeStructure.RTC_Seconds = tmp_second;	
	if(RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure)!=ERROR)
	RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
	//配置日期	  
	RTC_DateStructure.RTC_WeekDay = tmp_week;
	RTC_DateStructure.RTC_Date = tmp_data;
	RTC_DateStructure.RTC_Month =  tmp_month;
	RTC_DateStructure.RTC_Year = tmp_year;
	if(RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure)!=ERROR)
	RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);	  	    
	RTC_DateStructInit(&RTC_DateStructure);
}



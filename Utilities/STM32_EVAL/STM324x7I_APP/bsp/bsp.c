
#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>
#include "bsp.h"
#include "uart1.h"
#include "printer.h"
#include "vioce.h"
#include "uart4.h"
#include "serialscreen.h"
#include "uart3.h"
#include "bill.h"
#include "coin.h"
#include "mini_hopper.h"
#include "user_record.h"
#include "delay.h"
#include "uart5.h"
#include "flash.h"
#include "rtc.h"
 /*******************************************************************************
 * 函数名称:WaitTimeInit                                                                     
 * 描    述:把WaitTime从新安排到新的值                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
 void  WaitTimeInit(uint8_t *Time)
 {
	*Time = 120;
 }
 
	/*******************************************************************************
 * 函数名称:MachineHeatSet()                                                                     
 * 描    述:根据时间设置机器的加热和制冷                                                               
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2015年04月215日                                                                    
 *******************************************************************************/ 
#define heat_hours    8
#define heat_minutes  00
#define close_hours   13
#define close_minute  30
uint8_t old_hour=0;
uint8_t old_minute=0;
void MachineHeatSet(void)
{
#ifndef test
	static uint32_t heat_minute_total = 0;
	static uint32_t close_minute_totoal = 0;
	static uint32_t current_minute_totoal = 0;
	heat_minute_total = heat_hours*60 + heat_minutes ;
	close_minute_totoal = close_hours*60 + close_minute ;
	current_minute_totoal = TimeDate.Hours*60+TimeDate.Minutes;
	RTC_TimeShow();//获取时间的结构体
	if((current_minute_totoal>=heat_minute_total)&&(current_minute_totoal<close_minute_totoal))
	{
		if(0==OrderSetTemper(0x3C)) OrderSetTemper(0x3C); //加热
	}
	else
	{
		if(0==OrderSetTemper(0x63)) OrderSetTemper(0x63); //关加热
	} 

#endif
}
 /*******************************************************************************
 * 函数名称:FindMeal                                                                     
 * 描    述:查找那个地方有餐                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
 uint16_t   Balance;
 uint8_t    DisplayTimes;
 uint8_t    Line = 0,Column = 0 ;
 bool FindMeal(MealAttribute *DefineMeal) 
 {
	for(Line = 0; Line < FloorMealNum; Line++)  //查找层
	{
		for(Column = 0; Column < 3; Column++)     //查找列
		{
			if(DefineMeal[UserActMessageWriteToFlash.UserAct.MealID - 1].Position[Line][Column] > 0)    
			return true;                                   //当该位置有餐的话返回0
		}
	}
	return false ;
 }

	/*******************************************************************************
 * 函数名称:MoveToFisrtMeal();                                                                    
 * 描    述:找到用户选择的第一份餐品，并且移动到相应位置                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年6月25日                                                                    
 *******************************************************************************/ 
void MoveToFisrtMeal(void)
{
	if(UserActMessageWriteToFlash.UserAct.MealCnt_1st>0)
	{
		UserActMessageWriteToFlash.UserAct.MealID = 0x01; 
	}               
	else if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd>0)
	{
        UserActMessageWriteToFlash.UserAct.MealID = 0x02;
	}               
	else if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd>0)
	{
		UserActMessageWriteToFlash.UserAct.MealID = 0x03;   
	}
	else if(UserActMessageWriteToFlash.UserAct.MealCnt_4th>0)
	{
		UserActMessageWriteToFlash.UserAct.MealID = 0x04;   
	}
	if(FindMeal(DefineMeal)) /*查找餐品ID的位置*/
	{
		//OrderSendCoord(Line,Column);
	}   
}
 
	/*******************************************************************************
 * 函数名称:PrintTick                                                                     
 * 描    述:是否打印小票                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
			
void PrintTickFun(uint32_t *PrintTickFlag)
{      
	if(*PrintTickFlag == 0x00000001 )
	{
		/*打印小票的函数*/
		SPRT();
	}    
	if(*PrintTickFlag == 0x00000002 )
	{
        /*不打印*/
	}
	
}   
	/*******************************************************************************
 * 函数名称:StrToBCD                                                                
 * 描    述:将刷卡的数据读取转换后添加到数组中                                                             
 *           可以将字符串先转换为Int                                                                    
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:-1：错误 , 返回数组的长度                                                             
 * 修改日期:2014年9月2日                                                                    
 *******************************************************************************/ 
int8_t StrToBCD( uint8_t* pDest,const uint8_t* pSrc, uint8_t destLen)
{
	 uint8_t i=0;
	 for(i=0;i<destLen;i++)
	 {
		 if((pSrc[i]<'0')||(pSrc[i]>'9')) 
		 return -1;
	 }
	 if( destLen/2==0)
	 {
		for(i=0;i<destLen;i+=2)
		{
			pDest[i/2] =(((pSrc[i]-0x30)<<4)|(pSrc[i+1]-0x30));
		}
		return  destLen/2;
	 }
	 else
	 {
		pDest[0]= (pSrc[0]-0x30);
		for(i=1;i<destLen;i+=2)
		{
			 pDest[(i+1)/2] =(((pSrc[i]-0x30)<<4)|(pSrc[i+1]-0x30));
		}
		return  destLen/2;          
	 }
}

	/*******************************************************************************
 * 函数名称:IntToBCD                                                                
 * 描    述:将刷卡的数据读取转换后添加到数组中                                                             
 *           将int型的变量转换为BCD数组                                                               
 * 输    入:int                                                                
 * 输    出:bcd码的数组                                                                    
 * 返    回:-1：错误 , 返回数组的长度                                                             
 * 修改日期:2014年9月2日                                                                    
 *******************************************************************************/ 
void IntToBCD( uint8_t* pDes,const uint32_t  iv, uint8_t destLen)
{
	uint8_t sv[20]={0};
	StrToBCD(pDes,sv,sprintf((char *)sv,"%010u",iv));
}
	/*******************************************************************************
 * 函数名称:TrackDateToBuff                                                                
 * 描    述:将刷卡的数据读取转换后添加到数组中                                                             
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年9月2日                                                                    
 *******************************************************************************/ 
//void TrackDateToBuff(void)
//{
//  uint8_t i;
//  uint8_t  temp_t[10]={0};
//  uint8_t  TraceNum[10]={0};
//  //银联
//  if(UserActMessageWriteToFlash.UserAct.PayType == '2') //银行卡
//  {
//    for(i=0;i<10;i++)
//      PosDevNum[3+i]=0x00; 
//    PosDevNum[2]= strlen(UpDeductData.szPosId);
//    for(i=0;i<PosDevNum[2];i++)
//    {
//      PosDevNum[3+i]= UpDeductData.szPosId[i]; //终端号
//    }
//    PosTenantNum[2]= strlen(UpInitData[0].szMerchantId);
//    for(i=0;i<PosTenantNum[2];i++)
//    {
//      PosTenantNum[3+i]=UpInitData[0].szMerchantId[i]; //商户号
//    }
//    PosBatchNum[2]= sprintf(TraceNum,"%d",UpDeductData.lTraceNo);
//    for(i=0;i<PosBatchNum[2];i++)
//    {
//      PosBatchNum[3+i]=TraceNum[i];  //流水号
//    }
//    PosUserNum[2]= strlen(UpDeductData.szCardNo);
//    for(i=0;i<PosUserNum[2];i++)
//    {
//      PosUserNum[3+i]= UpDeductData.szCardNo[i]; //卡号
//    } 
//  }
//  //UpDeductData.szPosId;
//  //UpInitData.szMerchantId;
//  //UpDeductData.lTraceNo;
//  //UpDeductData.szCardNo
//  //深圳通
//  else if(UserActMessageWriteToFlash.UserAct.PayType == '3')
//  {
//    PosDevNum[2]= sprintf(temp_t,"%d",SztReductInf.machineNum);
//    for(i=0;i<PosDevNum[2];i++)
//      PosDevNum[3+i]=temp_t[i];  //终端号
//    
//    PosTenantNum[2]= strlen(UpInitData[0].szMerchantId);   
//    for(i=0;i<PosTenantNum[2];i++)
//      PosTenantNum[3+i]=UpInitData[0].szMerchantId[i];  //商户号
//    
//    PosBatchNum[2]= sprintf(temp_t,"%d",SztReductInf.TradeNum);  
//    for(i=0;i<PosBatchNum[2];i++)
//      PosBatchNum[3+i]=temp_t[i];  //流水号
//    
//    PosUserNum[2]= sprintf(temp_t,"%d",SztReductInf.CardNum);    
//    for(i=0;i<PosUserNum[2];i++)
//      PosUserNum[3+i]= temp_t[i];   //卡号 
//  }    
//  //SztReductInf.machineNum;
//  //UpDeductData.szMerchantld;  
//  //SztReductInf.TradeNum;
//  //SztReductInf.CardNum;  
//  //s
//}


	/*******************************************************************************
 * 函数名称:WaitPayMoney                                                                     
 * 描    述:等待接收用户投的钱                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 

uint8_t CurrentPoint = 1 ; 
uint8_t UserPayMoney =0 ;
uint8_t NewCoinsCnt=0;
uint8_t OldCoinsCnt=0;
uint8_t  WaitPayMoney(void)
{
	uint8_t reduce_money_flag = 0;
	uint32_t temp1=0,temp2=0;
	//VariableChage(wait_payfor,WaitTime);
	switch(CurrentPoint)
	{
		case 1 : 
		{
		/*显示付款方式，现金，银行卡，深圳通*/
			if(WaitTime<53)
			{
				//CurrentPoint = 3;
				/*支付方式*/           
				//UserActMessageWriteToFlash.UserAct.PayType = 0x31;/* 现金支付*/
				//取消自动
				//PlayMusic(VOICE_3);
				//if(!OpenCashSystem()){OpenCashSystem();};// printf("cash system is erro2");  //关闭现金接受
			}
		}break;
		case 2:  //由屏幕控制跳转
		{
		/*跳到选择付款方式界面*/
			CurrentPoint = 3 ;
			UserActMessageWriteToFlash.UserAct.PayType = 0x31 ;/* 现金支付*/
			GetDiscountCost(UserActMessageWriteToFlash.UserAct.PayType);/*根据支付方式计算餐品的折后价格和应付价格*/
		}break; 
		case 3 :  //读钱
		{
			UserPayMoney = ReadBills();
			if(UserPayMoney !=0 )    //表示收到了钱
			{
				UserActMessageWriteToFlash.UserAct.PayAlready  += UserPayMoney;
				UserActMessageWriteToFlash.UserAct.PayForBills += UserPayMoney;   
				//VariableChage(payment_bill,UserActMessageWriteToFlash.UserAct.PayForBills);
				UserPayMoney = 0 ;
			}
			CurrentPoint = 6 ;
		}break;
		case 5 ://会员卡支付   
		{
			UserActMessageWriteToFlash.UserAct.PayType = 0x34 ;/*会员卡支付*/
			GetDiscountCost(UserActMessageWriteToFlash.UserAct.PayType);/*根据支付方式计算餐品的折后价格和应付价格*/
			UserActMessageWriteToFlash.UserAct.PayForCards = UserActMessageWriteToFlash.UserAct.LastPayShould - UserActMessageWriteToFlash.UserAct.PayAlready;
			UserActMessageWriteToFlash.UserAct.PayAlready += UserActMessageWriteToFlash.UserAct.PayForCards ;
			UART3_ClrRxBuf();
			CurrentPoint = 6;
		}break;
		case 6 : //统计钱数
		{
			VariableChage(payment_coin,UserActMessageWriteToFlash.UserAct.PayAlready);     
			if(UserActMessageWriteToFlash.UserAct.PayAlready +UserActMessageWriteToFlash.UserAct.PayForCards>=UserActMessageWriteToFlash.UserAct.LastPayShould)     //投的钱大于等于要付的钱
			{
				CurrentPoint = 9;
			}
			else
			{ 
				CurrentPoint = 3; 
			}   
		}break;
		case 7 :  /*银行卡支付由屏幕控制*/
		{
			WaitTimeInit(&WaitTime);
			PageChange(SwipingCard_interface);
			UserActMessageWriteToFlash.UserAct.PayType = 0x32 ;/* 银行卡支付*/
			GetDiscountCost(UserActMessageWriteToFlash.UserAct.PayType);/*根据支付方式计算餐品的折后价格和应付价格*/
#ifndef test
			reduce_money_flag = GpbocDeduct((UserActMessageWriteToFlash.UserAct.LastPayShould-UserActMessageWriteToFlash.UserAct.PayAlready)*100);
#else
			//reduce_money_flag = GpbocDeduct(UserActMessageWriteToFlash.UserAct.LastPayShould-UserActMessageWriteToFlash.UserAct.PayAlready);
#endif
			if(reduce_money_flag == 1)
			{
				UserActMessageWriteToFlash.UserAct.PayForCards = UserActMessageWriteToFlash.UserAct.LastPayShould - UserActMessageWriteToFlash.UserAct.PayAlready;
				UserActMessageWriteToFlash.UserAct.PayAlready += UserActMessageWriteToFlash.UserAct.PayForCards ;
				UART3_ClrRxBuf();
				CurrentPoint =6;
			}
			else
			{
				WaitTimeInit(&WaitTime);
				PageChange(Acount_interface);
				CurrentPoint = 0;
				/*支付方式*/
				UserActMessageWriteToFlash.UserAct.PayType = 0x30;//清空支付方式        
				GetDiscountCost(UserActMessageWriteToFlash.UserAct.PayType);/*根据支付方式计算餐品的折后价格和应付价格*/        
				UART3_ClrRxBuf();
			}
		}break;
		case 8 :/*深圳通支付由屏幕控制*/
		{
			WaitTimeInit(&WaitTime);
			PageChange(SwipingCard_interface);
			UserActMessageWriteToFlash.UserAct.PayType = 0x33 ;/* 深圳通支付*/
			GetDiscountCost(UserActMessageWriteToFlash.UserAct.PayType);/*根据支付方式计算餐品的折后价格和应付价格*/
#ifndef test
			reduce_money_flag = SztDeduct((UserActMessageWriteToFlash.UserAct.LastPayShould- UserActMessageWriteToFlash.UserAct.PayAlready)*100);
#else
			//reduce_money_flag = SztDeduct(UserActMessageWriteToFlash.UserAct.LastPayShould- UserActMessageWriteToFlash.UserAct.PayAlready);//扣一分
#endif
			if(reduce_money_flag == 1)
			{
				UserActMessageWriteToFlash.UserAct.PayForCards = UserActMessageWriteToFlash.UserAct.LastPayShould - UserActMessageWriteToFlash.UserAct.PayAlready;
				UserActMessageWriteToFlash.UserAct.PayAlready += UserActMessageWriteToFlash.UserAct.PayForCards ;
				UART3_ClrRxBuf();
				CurrentPoint =6;
			}
			else
			{
				WaitTimeInit(&WaitTime);
				PageChange(Acount_interface);
				CurrentPoint = 0;
				/*支付方式*/
				UserActMessageWriteToFlash.UserAct.PayType = 0x30;//清空支付方式
				GetDiscountCost(UserActMessageWriteToFlash.UserAct.PayType);/*根据支付方式计算餐品的折后价格和应付价格*/
				UART3_ClrRxBuf();
			}
		}break;
		case 9 :  //付款成功关闭所有的收银系统
		{
			UserActMessageWriteToFlash.UserAct.MoneyBack = UserActMessageWriteToFlash.UserAct.PayAlready - UserActMessageWriteToFlash.UserAct.LastPayShould;    
			UserActMessageWriteToFlash.UserAct.Meal_totoal = UserActMessageWriteToFlash.UserAct.MealCnt_4th+UserActMessageWriteToFlash.UserAct.MealCnt_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_2nd+UserActMessageWriteToFlash.UserAct.MealCnt_1st;
			UserActMessageWriteToFlash.UserAct.MoneyBackShould = UserActMessageWriteToFlash.UserAct.MoneyBack; //记录付钱之后第一次应该退的币   
			VariableChage(mealout_totle,UserActMessageWriteToFlash.UserAct.Meal_totoal);  
			//将需要打印的数据赋值到相关结构体    
			Print_Struct.P_Type1st   = UserActMessageWriteToFlash.UserAct.MealType_1st;
			Print_Struct.P_Type2nd   = UserActMessageWriteToFlash.UserAct.MealType_2nd;
			Print_Struct.P_Type3rd   = UserActMessageWriteToFlash.UserAct.MealType_3rd;
			Print_Struct.P_Type4th   = UserActMessageWriteToFlash.UserAct.MealType_4th;
			Print_Struct.P_Number1st = UserActMessageWriteToFlash.UserAct.MealCnt_1st;
			Print_Struct.P_Number2nd = UserActMessageWriteToFlash.UserAct.MealCnt_2nd;
			Print_Struct.P_Number3rd = UserActMessageWriteToFlash.UserAct.MealCnt_3rd;
			Print_Struct.P_Number4th = UserActMessageWriteToFlash.UserAct.MealCnt_4th;
			Print_Struct.P_Price1st   = UserActMessageWriteToFlash.UserAct.MealPrice_1st;
			Print_Struct.P_Price2nd   = UserActMessageWriteToFlash.UserAct.MealPrice_2nd;
			Print_Struct.P_Price3rd   = UserActMessageWriteToFlash.UserAct.MealPrice_3rd;
			Print_Struct.P_Price4th   = UserActMessageWriteToFlash.UserAct.MealPrice_4th;      
			Print_Struct.P_Cost1st   = UserActMessageWriteToFlash.UserAct.LastMealCost_1st;
			Print_Struct.P_Cost2nd   = UserActMessageWriteToFlash.UserAct.LastMealCost_2nd;
			Print_Struct.P_Cost3rd   = UserActMessageWriteToFlash.UserAct.LastMealCost_3rd;
			Print_Struct.P_Cost4th   = UserActMessageWriteToFlash.UserAct.LastMealCost_4th;
			Print_Struct.P_paymoney  = UserActMessageWriteToFlash.UserAct.PayForBills + UserActMessageWriteToFlash.UserAct.PayForCoins +UserActMessageWriteToFlash.UserAct.PayForCards ;
			Print_Struct.P_PayShould = UserActMessageWriteToFlash.UserAct.LastPayShould ;
			Print_Struct.P_MoneyBack = UserActMessageWriteToFlash.UserAct.MoneyBack ;
			CurrentPoint = 0 ;
			return  Status_OK;
		}
		default :break;
	}
	if(WaitTime<=1) 
	{
		WaitTimeInit(&WaitTime);
		PageChange(Menu_interface);//超时退出用户餐品数量选择界面
		if(!CloseCashSystem()){CloseCashSystem();};//printf("cash system is erro6\r\n");  //关闭现金接受
		CurrentPoint = 0 ;
		temp1= UserActMessageWriteToFlash.UserAct.MoneyBack= UserActMessageWriteToFlash.UserAct.PayAlready; //超时将收到的钱以硬币的形式返还
		temp2= UserActMessageWriteToFlash.UserAct.MoneyBackShould= UserActMessageWriteToFlash.UserAct.PayAlready; //数据需要记录
		ClearUserBuffer();//清空用户数据
		UserActMessageWriteToFlash.UserAct.MoneyBack= temp1;
		UserActMessageWriteToFlash.UserAct.MoneyBackShould= temp2;
		//SaveUserData();//保存用户数据  
		if(UserActMessageWriteToFlash.UserAct.MoneyBack>0)
			Current= hpper_out;
		else
			Current= current_temperature;
	}
	else
	{
		if(WaitTime%5==1) 
		{
			if(UserActMessageWriteToFlash.UserAct.PayType==0x31)
			{
				PlayMusic(VOICE_3);
			}
			else
			{
				PlayMusic(VOICE_2);
			}        
		}    
	}
	return  Status_Action;
}                                               


uint8_t WaitMeal(void)
{
//	uint8_t MealoutCurrentPointer=0;
//	uint8_t takemeal_timecnt=0;
//	//static uint8_t Cmd[20]={0x05, 0x31, 0x30, 0x30, 0x31, 0x30 ,0x30, 0x36, 0x34, 0x30, 0x30, 0x30, 0x34, 0x4D, 0x31, 0x35, 0x43, 0x03, 0x0D ,0x0A};
//	uint8_t temp;
//	do
//	{
//		manageusart6data();  //将机械手的数据处理放在取餐头，不适用while(1)
//		DealSeriAceptData(); //只处理打印数据
//		switch(MealoutCurrentPointer)
//		{
//			case 0 : /*查找用户所选餐品的位置*/
//			{
//				//赋值当前UserActMessageWriteToFlash.UserAct.MealID
//				if(UserActMessageWriteToFlash.UserAct.MealCnt_1st>0)
//				{
//					UserActMessageWriteToFlash.UserAct.MealID = UserActMessageWriteToFlash.UserAct.MealType_1st; 
//				}               
//				else if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd>0)
//				{
//					UserActMessageWriteToFlash.UserAct.MealID = UserActMessageWriteToFlash.UserAct.MealType_2nd; 
//				}               
//				else if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd>0)
//				{
//					UserActMessageWriteToFlash.UserAct.MealID = UserActMessageWriteToFlash.UserAct.MealType_3rd; 
//				}
//				else if(UserActMessageWriteToFlash.UserAct.MealCnt_4th>0)
//				{
//					UserActMessageWriteToFlash.UserAct.MealID = UserActMessageWriteToFlash.UserAct.MealType_4th; 
//				}
//				else
//				{   
//					 //如果餐品全部出完，退出到主界面      
//					 //printf("takeafter_meal\r\n");        
//					PlayMusic(VOICE_12);                
//					return takeafter_meal;
//				}
//				if(FindMeal(DefineMeal)) /*查找餐品ID的位置*/
//				{
//					PlayMusic(VOICE_8); 
//					MealoutCurrentPointer= 1;
//				}
//			//printf("case 0/UserAct.MealID == %d\r\n",UserAct.MealID);             
//			}break;
//			case 1 : /* 发送餐的数目减一*/
//			{            
//				FloorMealMessageWriteToFlash.FloorMeal[Line].MealCount --; 
//				switch(Column)
//				{
//					case 0 :   FloorMealMessageWriteToFlash.FloorMeal[Line].FCount -- ;   break;
//					case 1 :   FloorMealMessageWriteToFlash.FloorMeal[Line].SCount -- ;   break;
//					case 2 :   FloorMealMessageWriteToFlash.FloorMeal[Line].TCount -- ;   break;
//					default:  break;
//				}
//				DefineMeal[UserActMessageWriteToFlash.UserAct.MealID - 1].Position[Line][Column]--;
//				DefineMeal[UserActMessageWriteToFlash.UserAct.MealID - 1].MealCount--;
//				WriteMeal();
//				StatisticsTotal();
//				DispLeftMeal(); 
//				MealoutCurrentPointer = 2 ;
//			}break ;   
//			case 2 : //根据[Line][Column]的值发送坐标 等待ACK 
//			{
//				//printf("发送行和列的位置，等待响应\r\n");
//				temp =0;
//				temp = OrderSendCoord(Line,Column);
//				
//				if( temp ==1)//发送成功
//				{
//					LinkTime =0;
//					machinerec.rerelative =0;   //不是待机位置
//					MealoutCurrentPointer = 3 ;
//				}
//				else                //发送失败
//				{
//					//printf("send coord error\r\n");
//					err_record |= (1<<SendUR6Erro);
//					return takemeal_erro;
//				}
//			}break;  
//			case 3 :    /*发送取餐命令*/
//			{
//				//查询机械手是否准备好，如果准备好发送取餐命令
//				machinerec.remealaway = 0;  /*初始化*/
//				//如果超时则 返回错误，
//				if(LinkTime > 10)    //超时
//				{
//					LinkTime =0;
//					//printf("move to coord timeout!\r\n");
//					err_record |= (1<<SendUR6Erro);
//					return takemeal_erro;
//				}       
//				if(machinerec.regoal ==1)   //到达取餐点
//				{
//					machinerec.regoal =0 ;
//					temp =0;
//					temp = OrderGetMeal();   //发送取餐命令
//					LinkTime =0;
//					MealoutCurrentPointer=4;                
//					if(temp ==1)       // 取餐命令发送成功
//					{
//						LinkTime =0;
//						MealoutCurrentPointer=4;  
//					}
//					else          //发送失败     
//					{
//						//printf(" send getmeal order error\r\n");
//						err_record |= (1<<SendUR6Erro);
//						return takemeal_erro;
//					}
//				}
//			}break;
//			case 4 :    /*播放语音，请取餐*/
//			{
//				//如果餐品到达取餐口播放语音
//				//如果餐品取出则 跳出子程序进行数据上传  
//				if(machinerec.retodoor == 1)   //到达出餐口
//				{
//					machinerec.retodoor = 0;
//					LinkTime =0;  
//					takemeal_timecnt=1;
//					//播放请取餐语音
//					PlayMusic(VOICE_9);
//				}
//				else if(machinerec.reenablegetmeal ==1)  //取餐5秒了还未取到餐
//				{
//					machinerec.reenablegetmeal =0; //新加的需要把相关标记清零
//					//printf("取餐5秒了还未取到餐\r\n");  
//					err_record |= (1<<GetMealError);
//					return takemeal_erro;
//				}
//				//printf("餐未超过了三分钟还未被取走\r\n");
//				else if( machinerec.remealaway == 1) //餐已被取走
//				{
//				//printf("餐已被取走\r\n");
//					LinkTime =0;
//					machinerec.remealaway = 0;
//					MealoutCurrentPointer=5;
//					break;
//				}
//				else if( machinerec.remealnoaway == 1)  //餐在取餐口过了20秒还未被取走
//				{
//					if( LinkTime >=25) //餐在出餐口未被取走，一直等待时间时间大于20s播放语音提示取餐
//					{
//						PlayMusic(VOICE_10);
//						LinkTime= 0;
//					}
//				}                   
//				else if(takemeal_timecnt==1) //如果没有收到过了20S餐盒没有被取走则从到达出餐口的位置计时
//				{
//					if(LinkTime >=25)
//					{
//						if(UserActMessageWriteToFlash.UserAct.MealID==UserActMessageWriteToFlash.UserAct.MealType_1st)UserActMessageWriteToFlash.UserAct.MealCnt_1st--;
//						else if(UserActMessageWriteToFlash.UserAct.MealID==UserActMessageWriteToFlash.UserAct.MealType_2nd)UserActMessageWriteToFlash.UserAct.MealCnt_2nd--;
//						else if(UserActMessageWriteToFlash.UserAct.MealID==UserActMessageWriteToFlash.UserAct.MealType_3rd)UserActMessageWriteToFlash.UserAct.MealCnt_3rd--;
//						else if(UserActMessageWriteToFlash.UserAct.MealID==UserActMessageWriteToFlash.UserAct.MealType_4th)UserActMessageWriteToFlash.UserAct.MealCnt_4th--;
//						err_record |= (1<<MealNoAway);
//						return takemeal_erro;
//					}
//				}   
//				else   //从发送完取餐命令计时
//				{
//					if(LinkTime>60)
//					{
//						err_record |= (1<<GetMealError);
//						return takemeal_erro; 
//					}                       
//				} 
//			}break;             
//			case 5:     /*对用户数据进行减一*/  //?? 如果需要进行错误退币，需要修该返回值所在范围
//			{
//				machinerec.remealnoaway = 0;
//				machinerec.remealaway = 0;
//				UserActMessageWriteToFlash.UserAct.Meal_takeout++;//取餐数据加
//				VariableChage(mealout_already,UserActMessageWriteToFlash.UserAct.Meal_takeout); //UI显示
//				MealoutCurrentPointer= 0;
//				//printf("case 5/UserAct.MealID == %d\r\n",UserAct.MealID);
//				if(UserActMessageWriteToFlash.UserAct.MealID == UserActMessageWriteToFlash.UserAct.MealType_1st)
//				{
//					UserActMessageWriteToFlash.UserAct.MealCnt_1st--;
//					//if(UserActMessageWriteToFlash.UserAct.MealCnt_1st==0)
//					if(0)
//					{
//						//printf("tookkind_meal\r\n");
//						return tookkind_meal;
//					}
//					else
//					{
//						//printf("tookone_meal\r\n");
//						return tookone_meal;
//					}
//				}
//				else if(UserActMessageWriteToFlash.UserAct.MealID == UserActMessageWriteToFlash.UserAct.MealType_2nd)
//				{
//					UserActMessageWriteToFlash.UserAct.MealCnt_2nd--;
//					//if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd==0)
//					if(0)
//					{
//						//printf("tookkind_meal\r\n");
//						return tookkind_meal;
//					}
//					else
//					{
//						//printf("tookone_meal\r\n");
//					return tookone_meal;
//					}                       
//				}
//				else if(UserActMessageWriteToFlash.UserAct.MealID == UserActMessageWriteToFlash.UserAct.MealType_3rd)
//				{
//					UserActMessageWriteToFlash.UserAct.MealCnt_3rd--;
//					//if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd==0)
//					if(0)
//					{
//						//printf("tookkind_meal\r\n");
//						return tookkind_meal;
//					}
//					else
//					{
//						//printf("tookone_meal\r\n");
//					return tookone_meal;
//					}                       
//				}               
//				else if(UserActMessageWriteToFlash.UserAct.MealID == UserActMessageWriteToFlash.UserAct.MealType_4th)
//				{
//					UserActMessageWriteToFlash.UserAct.MealCnt_4th--;   
//					//if(UserActMessageWriteToFlash.UserAct.MealCnt_4th==0)
//					if(0)
//					{
//						//printf("tookkind_meal\r\n");
//						return tookkind_meal;
//					}
//					else
//					{
//						//printf("tookone_meal\r\n");
//						return tookone_meal;
//					}                       
//				}        
//				else 
//				{
//					printf("tookone_meal err\r\n");
//				}
//			}
//			default:break;
//		}
//	}while(1);
    return tookone_meal;
}   

	/*******************************************************************************
 * 函数名称:CloseCashSystem                                                                     
 * 描    述:关闭现金接受                                                        
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
#define bill_time  600  //500ms不会出错，100ms纸币机反应不过来
bool CloseCashSystem(void)
{
	uint8_t cnt_t=253;
	CloseCoinMachine();               //关闭投币机 
	delay_ms(bill_time);
	memset(BillDataBuffer,0xFE,sizeof(BillDataBuffer));
	DisableBills();             //设置并关闭纸币机
	do
	{
		cnt_t--;
		delay_ms(1);
		if((RX4Buffer[0]=='0')&&(RX4Buffer[1]=='0'))
		{
			return true;
		}
		else if((RX4Buffer[0]=='F')&&(RX4Buffer[1]=='F'))
		{
			return false;           
		}
	}while(cnt_t);           
	 return false; 
}

bool OpenCashSystem(void)
{
	uint8_t cnt_t=254;  
	OpenCoinMachine();    //打开投币机   
	delay_ms(bill_time);        //需要控制
	memset(BillDataBuffer,0xFE,sizeof(BillDataBuffer));
	SetBills();           //设置并打开纸币机
	do
	{
		cnt_t--;
		delay_ms(1);
		if((RX4Buffer[0]=='0')&&(RX4Buffer[1]=='0'))
		{
			return true;
		}
		else if((RX4Buffer[0]=='F')&&(RX4Buffer[1]=='F'))
		{
			return false;           
		}
	}while(cnt_t);  
	return false; 
}

	/*******************************************************************************
 * 函数名称:ClearingFuntion                                                                     
 * 描    述:结算命令 检验数据是否发送成功，加入没有发送成功，继续发送。全部发送完毕后结算。                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void ClearingFuntion(void)
{
	 
//	RTC_TimeShow();//获得时间
//	if(TimeDate.Hours == 5 || TimeDate.Hours == 19)
//	{
//		// 结算命令 ；
//		//   ClearingFun();     //后台结算命令
//		// BankFlashCard_Upload(); //数据上送    不是退签
//	}                
}
	/*******************************************************************************
 * 函数名称:AcountCopy                                                                   
 * 描    述:用户数据拷贝                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void AcountCopy(void)
{
	UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= UserActMessageWriteToFlash.UserAct.MealCnt_1st;
	UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= UserActMessageWriteToFlash.UserAct.MealCnt_2nd;
	UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= UserActMessageWriteToFlash.UserAct.MealCnt_3rd;
	UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= UserActMessageWriteToFlash.UserAct.MealCnt_4th; 
}

	/*******************************************************************************
 * 函数名称:GetDiscountCost                                                                   
 * 描    述:获取折后价格 及总的应付款项                                                            
 *                                                                               
 * 输    入:支付方式                                                                   
 * 输    出:true / false                                                                   
 * 返    回:void                                                               
 * 修改日期:2013年1月16日                                                                    
 *******************************************************************************/ 
uint8_t last_discount_1st; //第一种餐品的付款时折扣
uint8_t last_discount_2nd;
uint8_t last_discount_3rd;
uint8_t last_discount_4th;
bool GetDiscountCost(uint8_t payment)
{
	switch(payment)
	{
		case 0x30:{  /*未选择支付方式*/
			last_discount_1st = 100;
			last_discount_2nd = 100;
			last_discount_3rd = 100;
			last_discount_4th = 100;      
		}break;
		/*折扣计算出问题了，需要获取当前餐品的折扣信息*/
		case 0x31:{  /*现金*/
			 /*已知现金支付，根据选择的第一份餐品的内部序号，将该序号下的餐品的折扣信息赋值给用户第一份餐品的折扣用于计算*/
			switch(UserActMessageWriteToFlash.UserAct.MealType_1st) 
			{
				case 0x01:{last_discount_1st = cashcut_1st;};break;
				case 0x02:{last_discount_1st = cashcut_2nd;};break;
				case 0x03:{last_discount_1st = cashcut_3rd;};break;
				case 0x04:{last_discount_1st = cashcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_2nd) 
			{
				case 0x01:{last_discount_2nd = cashcut_1st;};break;
				case 0x02:{last_discount_2nd = cashcut_2nd;};break;
				case 0x03:{last_discount_2nd = cashcut_3rd;};break;
				case 0x04:{last_discount_2nd = cashcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_3rd) 
			{
				case 0x01:{last_discount_3rd = cashcut_1st;};break;
				case 0x02:{last_discount_3rd = cashcut_2nd;};break;
				case 0x03:{last_discount_3rd = cashcut_3rd;};break;
				case 0x04:{last_discount_3rd = cashcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_4th) 
			{
				case 0x01:{last_discount_4th = cashcut_1st;};break;
				case 0x02:{last_discount_4th = cashcut_2nd;};break;
				case 0x03:{last_discount_4th = cashcut_3rd;};break;
				case 0x04:{last_discount_4th = cashcut_4th;};break;
				default :break;
			}
		}break;
		case 0x32:{  /*银联闪付卡*/
			switch(UserActMessageWriteToFlash.UserAct.MealType_1st)
			{
				case 0x01:{last_discount_1st = gboccut_1st;};break;
				case 0x02:{last_discount_1st = gboccut_2nd;};break;
				case 0x03:{last_discount_1st = gboccut_3rd;};break;
				case 0x04:{last_discount_1st = gboccut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_2nd) 
			{
				case 0x01:{last_discount_2nd = gboccut_1st;};break;
				case 0x02:{last_discount_2nd = gboccut_2nd;};break;
				case 0x03:{last_discount_2nd = gboccut_3rd;};break;
				case 0x04:{last_discount_2nd = gboccut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_3rd) 
			{
				case 0x01:{last_discount_3rd = gboccut_1st;};break;
				case 0x02:{last_discount_3rd = gboccut_2nd;};break;
				case 0x03:{last_discount_3rd = gboccut_3rd;};break;
				case 0x04:{last_discount_3rd = gboccut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_4th) 
			{
				case 0x01:{last_discount_4th = gboccut_1st;};break;
				case 0x02:{last_discount_4th = gboccut_2nd;};break;
				case 0x03:{last_discount_4th = gboccut_3rd;};break;
				case 0x04:{last_discount_4th = gboccut_4th;};break;
				default :break;
			}
		}break;
		case 0x33:{  /*深圳通*/
			switch(UserActMessageWriteToFlash.UserAct.MealType_1st)
			{
				case 0x01:{last_discount_1st = sztcut_1st;};break;
				case 0x02:{last_discount_1st = sztcut_2nd;};break;
				case 0x03:{last_discount_1st = sztcut_3rd;};break;
				case 0x04:{last_discount_1st = sztcut_4th;};break;
				default :break; 
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_2nd) 
			{
				case 0x01:{last_discount_2nd = sztcut_1st;};break;
				case 0x02:{last_discount_2nd = sztcut_2nd;};break;
				case 0x03:{last_discount_2nd = sztcut_3rd;};break;
				case 0x04:{last_discount_2nd = sztcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_3rd) 
			{
				case 0x01:{last_discount_3rd = sztcut_1st;};break;
				case 0x02:{last_discount_3rd = sztcut_2nd;};break;
				case 0x03:{last_discount_3rd = sztcut_3rd;};break;
				case 0x04:{last_discount_3rd = sztcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_4th) 
			{
				case 0x01:{last_discount_4th = sztcut_1st;};break;
				case 0x02:{last_discount_4th = sztcut_2nd;};break;
				case 0x03:{last_discount_4th = sztcut_3rd;};break;
				case 0x04:{last_discount_4th = sztcut_4th;};break;
				default :break;
			}        
		}break;
		case 0x34:{ /*会员卡*/
			switch(UserActMessageWriteToFlash.UserAct.MealType_1st)
			{
				case 0x01:{last_discount_1st = vipcut_1st;};break;
				case 0x02:{last_discount_1st = vipcut_2nd;};break;
				case 0x03:{last_discount_1st = vipcut_3rd;};break;
				case 0x04:{last_discount_1st = vipcut_4th;};break;
				default :break; 
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_2nd) 
			{
				case 0x01:{last_discount_2nd = vipcut_1st;};break;
				case 0x02:{last_discount_2nd = vipcut_2nd;};break;
				case 0x03:{last_discount_2nd = vipcut_3rd;};break;
				case 0x04:{last_discount_2nd = vipcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_3rd) 
			{
				case 0x01:{last_discount_3rd = vipcut_1st;};break;
				case 0x02:{last_discount_3rd = vipcut_2nd;};break;
				case 0x03:{last_discount_3rd = vipcut_3rd;};break;
				case 0x04:{last_discount_3rd = vipcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_4th) 
			{
				case 0x01:{last_discount_4th = vipcut_1st;};break;
				case 0x02:{last_discount_4th = vipcut_2nd;};break;
				case 0x03:{last_discount_4th = vipcut_3rd;};break;
				case 0x04:{last_discount_4th = vipcut_4th;};break;
				default :break;
			}         
		}break;
		case 0x35:{ /*微信支付*/
			switch(UserActMessageWriteToFlash.UserAct.MealType_1st)
			{
				case 0x01:{last_discount_1st = vchatcut_1st;};break;
				case 0x02:{last_discount_1st = vchatcut_2nd;};break;
				case 0x03:{last_discount_1st = vchatcut_3rd;};break;
				case 0x04:{last_discount_1st = vchatcut_4th;};break;
				default :break; 
			} 
			switch(UserActMessageWriteToFlash.UserAct.MealType_2nd) 
			{
				case 0x01:{last_discount_2nd = vchatcut_1st;};break;
				case 0x02:{last_discount_2nd = vchatcut_2nd;};break;
				case 0x03:{last_discount_2nd = vchatcut_3rd;};break;
				case 0x04:{last_discount_2nd = vchatcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_3rd) 
			{
				case 0x01:{last_discount_3rd = vchatcut_1st;};break;
				case 0x02:{last_discount_3rd = vchatcut_2nd;};break;
				case 0x03:{last_discount_3rd = vchatcut_3rd;};break;
				case 0x04:{last_discount_3rd = vchatcut_4th;};break;
				default :break;
			}
			switch(UserActMessageWriteToFlash.UserAct.MealType_4th) 
			{
				case 0x01:{last_discount_4th = vchatcut_1st;};break;
				case 0x02:{last_discount_4th = vchatcut_2nd;};break;
				case 0x03:{last_discount_4th = vchatcut_3rd;};break;
				case 0x04:{last_discount_4th = vchatcut_4th;};break;
				default :break;
			}         
		}break;
		default: return false;		
	}
	UserActMessageWriteToFlash.UserAct.LastMealPrice_1st = UserActMessageWriteToFlash.UserAct.MealPrice_1st*last_discount_1st/100;
	UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd = UserActMessageWriteToFlash.UserAct.MealPrice_2nd*last_discount_2nd/100;
	UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd = UserActMessageWriteToFlash.UserAct.MealPrice_3rd*last_discount_3rd/100;
	UserActMessageWriteToFlash.UserAct.LastMealPrice_4th = UserActMessageWriteToFlash.UserAct.MealPrice_4th*last_discount_4th/100;
	UserActMessageWriteToFlash.UserAct.LastMealCost_1st  = UserActMessageWriteToFlash.UserAct.LastMealPrice_1st\
															 *UserActMessageWriteToFlash.UserAct.MealCnt_1st;  
	UserActMessageWriteToFlash.UserAct.LastMealCost_2nd  = UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd\
															 *UserActMessageWriteToFlash.UserAct.MealCnt_2nd;  
	UserActMessageWriteToFlash.UserAct.LastMealCost_3rd  = UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd\
															 *UserActMessageWriteToFlash.UserAct.MealCnt_3rd;  
	UserActMessageWriteToFlash.UserAct.LastMealCost_4th  = UserActMessageWriteToFlash.UserAct.LastMealPrice_4th\
															 *UserActMessageWriteToFlash.UserAct.MealCnt_4th;  
	UserActMessageWriteToFlash.UserAct.LastPayShould = UserActMessageWriteToFlash.UserAct.LastMealCost_1st+UserActMessageWriteToFlash.UserAct.LastMealCost_2nd\
														 +UserActMessageWriteToFlash.UserAct.LastMealCost_3rd+UserActMessageWriteToFlash.UserAct.LastMealCost_4th;
	VariableChage(mealtotoal_cost,UserActMessageWriteToFlash.UserAct.LastPayShould);
	return true;
}

	/*******************************************************************************
 * 函数名称:GetDiscountValue                                                                   
 * 描    述:获取当前餐品的折扣数据                                                           
 *                  NG                                                             
 * 输    入:支付方式                                                                   
 * 输    出:true / false                                                                   
 * 返    回:void                                                               
 * 修改日期:2013年1月16日                                                                    
 *******************************************************************************/ 
uint8_t GetDiscountValue(uint8_t meal_order)
{
	uint8_t value_return=0;
	switch(meal_order)
	{
		case 0x01:value_return = UserActMessageWriteToFlash.UserAct.LastMealPrice_1st;break;
		case 0x02:value_return = UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd;break;
		case 0x03:value_return = UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd;break;
		case 0x04:value_return = UserActMessageWriteToFlash.UserAct.LastMealPrice_4th;break;
		default:break;
	}
	return value_return;
}
	/*******************************************************************************
 * 函数名称:PowerupAbnormalHandle                                                                    
 * 描    述:开机异常处理，只显示错误标识，更根据密码显示用户操作记录                                                                
 *            首先需要判断是否断电，再判断UserActMessageWriteToFlash.UserAct.paybacke是否大于0，显示用户选择的
				餐品数，未出的餐品数，以及钱数                                                                   
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 
void PowerupAbnormalHandle(uint32_t err_record_t)
{
	AbnormalHandle(err_record_t);
	err_record = 0;
}

	/*******************************************************************************
 * 函数名称:PollAbnormalHandle                                                                   
 * 描    述:轮询                                                                 
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 
void PollAbnormalHandle(void)
{
	uint8_t i=0;
	for(i=32;i>0;i--) //32位记录各种异常，开机需要处理 ，可以通过管理员清理错误
	{
		if(err_record&(1<<i))
		AbnormalHandle(i);  
	}   
}

/*******************************************************************************
 * 函数名称:AbnormalHandle                                                                    
 * 描    述:异常处理 ，包含用户数据的显示                                                                
 *           包含开机异常处理，需要进行判断，相当于开机判断                                                                     
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint32_t err_flag=0;
void AbnormalHandle(uint32_t err)
{   
	err_record |= (1<<err); //在开始异常处理的时候需要用到
	//printf("err_record2nd 0x%08X\r\n",err_record);
	err_flag = err; //只是在错误处理与判断时需要用到，
	switch(err)
	{
		case outage_err:      //断电：只有在开机的时候判断是否有断电的情况发生 ，计算应该退的钱，还有就是付钱的时候断电
		{            /*取餐出错的情况*/              /*退币出错的情况,包含付钱的时候断电*/
			if((UserActMessageWriteToFlash.UserAct.Meal_totoal!=UserActMessageWriteToFlash.UserAct.Meal_takeout)||(UserActMessageWriteToFlash.UserAct.MoneyBack>0))//先判断是否还有餐品没有取出和再判断用户未退的钱
			{
				/*这里加上调试信息，查看以上连个条件的数值*/
				if(UserActMessageWriteToFlash.UserAct.MealID>0)
				{
				//计算总的应退币数
					UserActMessageWriteToFlash.UserAct.MoneyBackShould +=
					(UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd
					+UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);                                      
					//计算总的未退币数
					UserActMessageWriteToFlash.UserAct.MoneyBack += 
					(UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd
					+UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);   
					//计算上传的交易金额
					payfor_meal+=(UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd
					+UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th); 
					//计算已退币的钱（需要处理，要两次退币的数据）
					UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already= (UserActMessageWriteToFlash.UserAct.MoneyBackShould-UserActMessageWriteToFlash.UserAct.MoneyBack);
			
					//显示用户已付 和  应退  和 已退
					VariableChage(record_UserActPayAlready,UserActMessageWriteToFlash.UserAct.PayAlready);
					//应退的钱 = 总的应该退币的钱
					VariableChage(record_UserActPayBack,UserActMessageWriteToFlash.UserAct.MoneyBackShould);
					//已退的钱 = 总的应该退币的钱- 还未退的钱
					VariableChage(record_UserActPayBackAlready,UserActMessageWriteToFlash.UserAct.MoneyBackShould-UserActMessageWriteToFlash.UserAct.MoneyBack);            
						
					//DataUpload(Failed);//只有当UserActMessageWriteToFlash.UserAct.MealID!=0的时候才上传餐品的数据
					//SaveUserData();
				}
				DisplayAbnormal("E070");
				PageChange(Err_interface);
			}
			else 
			{
				err_record &= ~(1<<err); //如果需要处理其他异常呢
				err_flag= 0;
				return;
			}
		}break;
		case sdcard_err:     //SD卡存储异常
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E000");
			PageChange(Err_interface);                    
			////StatusUploadingFun(0xE000); //状态上送
		}break;
		case billset_err:    //纸币机异常
		{
			PlayMusic(VOICE_11);              
			DisplayAbnormal("E010"); 
			PageChange(Err_interface);  
			//StatusUploadingFun(0xE010); //状态上送
		}break;
		case coinset_err:      //投币机
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E020");
			PageChange(Err_interface);                
			//StatusUploadingFun(0xE020); //状态上送
		}break;
		case coinhooperset_err:    //退币机
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E030");
			PageChange(Err_interface);                
			//StatusUploadingFun(0xE030); //状态上送
			//没有币可退的时候，UserActMessageWriteToFlash.UserAct.Payback 不为0
		}break;
		case coinhooperset_empty:   //找零用光
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E032");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE032); //状态上送
		}break;
		case printer_err:      //打印机异常
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E040");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE040); //状态上送
		}break;
		case cardread_err:     //读卡器异常
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E050");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE050); //状态上送
		}break;
		case network_err:     //网络异常
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E060");
			PageChange(Err_interface);
		}break;
		case signin_err:    //签到异常
		{
			PlayMusic(VOICE_11); 
			DisplayAbnormal("E061");           
			PageChange(Err_interface);  
		}break;
		case upload_err:     //数据上传的时候突然断电
		{
			//DataUpload(Success);
			err_record &= ~(1<<upload_err);
			return;
		};
		case arm_limit: //机械手禁止复位
		{
			if((UserActMessageWriteToFlash.UserAct.Meal_totoal!=UserActMessageWriteToFlash.UserAct.Meal_takeout)||(UserActMessageWriteToFlash.UserAct.MoneyBack>0))//先判断是否还有餐品没有取出和再判断用户未退的钱
			{
				/*这里加上调试信息，查看以上连个条件的数值*/
				if(UserActMessageWriteToFlash.UserAct.MealID>0)
				{
					//SaveUserData();
				}
				DisplayAbnormal("E100");
				PageChange(Err_interface);
			}
			else 
			{
				err_record &= ~(1<<err); //如果需要处理其他异常呢
				err_flag= 0;
				return;
			}
		}break;           
		case X_timeout:        //x轴传感器超时
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E101");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE101); //状态上送
			//DataUpload(Failed);
		}break;
		case X_leftlimit:      //马达左动作极限输出
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E102");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE102); //状态上送
			//DataUpload(Failed);
		}break;
		case X_rightlimit:     //马达右动作极限输出
		{
			DisplayAbnormal("E103");
			PlayMusic(VOICE_11);
			//StatusUploadingFun(0xE103); //状态上送  
			PageChange(Err_interface);
			//DataUpload(Failed);
		}break;
		case mealtake_timeout: //取餐口传感器超时
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E201");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE201); //状态上送
			//DataUpload(Failed);
		}break;
		case Y_timeout:        //y轴传感器超时
		{
			PlayMusic(VOICE_11);
			DisplayAbnormal("E301");    
			PageChange(Err_interface);
			//StatusUploadingFun(0xE302); //状态上送
			//DataUpload(Failed);
		}break;
		case link_timeout:     //链接超时
		{
			PlayMusic(VOICE_11);
			DisplayAbnormal("E401");    
			PageChange(Err_interface);
			//StatusUploadingFun(0xE401); //状态上送
		}break;
		case Z_timeout:        //z轴传感器超时
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E501");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE501); //状态上送
			//DataUpload(Failed);
		}break;
		case Z_uplimit:        //z轴马达上动作超出
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E502");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE502); //状态上送
			//DataUpload(Failed);
		}break;
		case Z_downlimit:      //z马达下动作超出
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E503");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE503); //状态上送
			//DataUpload(Failed);
		}break;
		case solenoid_timeout: //电磁阀超时  ???有时有异常
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E601");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE601); //状态上送
			//DataUpload(Failed);
		}break;
		case Eeprom_erro:      //eeprom 异常
		{
			PlayMusic(VOICE_11);  
			DisplayAbnormal("E711");
			PageChange(Err_interface);
			//StatusUploadingFun(0xE711); //状态上送
			//DataUpload(Failed);
		}break;
		case SendUR6Erro:      //机械手串口通讯超时
		{
			PlayMusic(VOICE_11);
			DisplayAbnormal("E801");    
			PageChange(Err_interface);
			//StatusUploadingFun(0xE801); //状态上送
			//DataUpload(Failed);
		}break;
		case GetMealError:     //机械手5秒取不到餐
		{
			PlayMusic(VOICE_11);
			DisplayAbnormal("E802");    
			PageChange(Err_interface);
			//StatusUploadingFun(0xE802); //状态上送
			//DataUpload(Failed);
		}break;
		case MealNoAway:       //餐在出餐口20秒还未被取走
		{
			PlayMusic(VOICE_11);
			DisplayAbnormal("E803");    
			PageChange(Err_interface);
			//StatusUploadingFun(0xE803); //状态上送
			//DataUpload(Success);
		}break;
		default:break;
	}
	//SaveUserData(); //错误处理一次数据保存一次
	while(1)
	{
		DealSeriAceptData();
		if(err_flag==0)
		{
			if(err_record&(1<<arm_limit)) //开机的机械手禁止复位
			{
				PageChange(OnlymachieInit_interface);                   
				//OnlymachieInit(); //机械手的初始化
				err_record &= ~(1<<arm_limit);
				RTC_WriteBackupRegister(RTC_BKP_DR13, err_record);
			}
			else
			{
				UserActMessageWriteToFlash.UserAct.MoneyBack=0;
				UserActMessageWriteToFlash.UserAct.MoneyBackShould=0;
				UserActMessageWriteToFlash.UserAct.MoneyBackAlready=0;
				ClearUserBuffer();
				//SaveUserData();
			}
			if(err_record>=(1<<X_timeout))//如果是机械售错误值上传一次数据，对高位取反
			{
				err_record&=~0xFFFF0000; //只显示一次异常
			}
			err_record &= ~(1<<err); //一次只处理一次异常
			RTC_WriteBackupRegister(RTC_BKP_DR13, err_record);
			break;
		}
		MachineHeatSet();//防止进入错误处理的时候卡死
	}
}
	/*******************************************************************************
 * 函数名称:ErrRecHandle                                                                     
 * 描    述:错误记录处理                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年7月2日                                                                    
 *******************************************************************************/ 
void ErrRecHandle(void)
{
	if(err_record!=0) //当有错误记录，需要进行处理
	{
		AbnormalHandle(outage_err);//相当于开机异常处理
		if(err_record&(1<<upload_err))
		{
			if(UserActMessageWriteToFlash.UserAct.MealID!=0)
			AbnormalHandle(upload_err);//需要处理数据上传的断电
		}
		err_record=0;
	}
	else
	{
		ClearUserBuffer(); //清除之前读取的数据
		UserActMessageWriteToFlash.UserAct.MoneyBack=0;//
	}
	//SaveUserData();  
}

 /*
	PVD ---- 低电压检测                抢占优先级  0      亚优先级 0        用于保护sd卡
	硬币机               外部中断5                 0                1
	纸币机               串口4                     0                2
	IC卡                 串口2                     0                3  
	屏幕                 串口中断3                 0                4
	打印                 串口1                     0                6
	退币倒计时           定时器4                   1                0
	定时倒计时60s        定时器3                   1                1
 */
	/*******************************************************************************
 * 函数名称:hardfawreInit                                                                     
 * 描    述:硬件初始化                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void HardwaveInit(void)
{
	Uart4_Configuration();     //纸币机串口初始化 1, 2
	Uart1_Configuration();     //打印机串口初始化
	Uart3_Configuration();     //串口屏初始化  0 , 3
	Uart5_Configuration();     //服务器通信初始化
	AppRTC_Init();             //RTC初始化
	RTC_TimeRegulate();      //设置时间
	
	InitCoins();               //投币机初始化
	InitMiniGPIO() ;           //退币器始化   
	InitVoice();               //语音初始化
	SPI_Flash_Init();          //flash提前初始化
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable); //关闭看门狗 
}   

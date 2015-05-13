
/*===============================================================

* 程序功能：记录顾客的触摸屏操作记录

* 日    期：2015年4月30
* 作    者：jackey

================================================================*/
#include <string.h>
#include <stm32f4xx.h>
#include "user_record.h"
#include "serialscreen.h"

uint8_t  Current = 0;     /*主状态*/
uint64_t err_record = 0;  /*错误标记位*/
uint8_t  sell_type[4]={0x01,0x02,0x03,0x04}; /*餐品菜单*/

uint8_t WaitTime ;
uint8_t WaitMealTime ;
uint32_t payfor_meal ;

/*餐品的详细信息*/
uint16_t MealID_1st[4]={0}; uint16_t MealNo_1st=0, price_1st= 0; 
uint16_t MealID_2nd[4]={0}; uint16_t MealNo_2nd=0, price_2nd= 0; 
uint16_t MealID_3rd[4]={0}; uint16_t MealNo_3rd=0, price_3rd= 0; 
uint16_t MealID_4th[4]={0}; uint16_t MealNo_4th=0, price_4th= 0; 
uint8_t sztcut_1st=0, gboccut_1st=0, vipcut_1st=0, vchatcut_1st=0, Order_1st=0, cashcut_1st=0;
uint8_t sztcut_2nd=0, gboccut_2nd=0, vipcut_2nd=0, vchatcut_2nd=0, Order_2nd=0, cashcut_2nd=0;
uint8_t sztcut_3rd=0, gboccut_3rd=0, vipcut_3rd=0, vchatcut_3rd=0, Order_3rd=0, cashcut_3rd=0;
uint8_t sztcut_4th=0, gboccut_4th=0, vipcut_4th=0, vchatcut_4th=0, Order_4th=0, cashcut_4th=0;
uint8_t MealType_1st[4]={0},MealType_2nd[4]={0},MealType_3rd[4]={0},MealType_4th[4]={0};
uint8_t mealname_1st[20]= {'\0'},mealname_2nd[20]= {'\0'},mealname_3rd[20]= {'\0'},mealname_4th[20]= {'\0'};

/*相关结构体*/
MealAttribute             DefineMeal[MealKindTotoal];       //定义MealKindTotoal个菜系
UserActMessage            UserActMessageWriteToFlash;         //用户数据写入flash
CoinsTotoalMessage        CoinsTotoalMessageWriteToFlash;     //硬币数结构体
FloorMealMessage          FloorMealMessageWriteToFlash;       //将数据写入flash的结构
FloorAttribute            FloorMeal[FloorMealNum], CurFloor;  //餐品放置位置及数量的结构体
MealCompareDataStruct     MealCompareData;                    //餐品对比数据显示
UserSlecMealInfoStruct    UserSlectInfo;
MEAL_UNION                Meal_Union;


 /*******************************************************************************
 * 函数名称:GetMealDetail                                                                   
 * 描    述:通过结构体获取结构体中的变量                                                                                                                         
 * 输    入:无                                                                
 * 输    出:无                                                                    
 * 返    回:void                                                               
 * 修改日期:2014年11月6日                                                                   
 *******************************************************************************/ 
void GetMealDetail(void)
{
	/*获取餐品数量*/
	MealNo_1st= Meal_Union.Meal[0].MealCnt[0]*16 + Meal_Union.Meal[0].MealCnt[1];
	MealNo_2nd= Meal_Union.Meal[1].MealCnt[0]*16 + Meal_Union.Meal[1].MealCnt[1];
	MealNo_3rd= Meal_Union.Meal[2].MealCnt[0]*16 + Meal_Union.Meal[2].MealCnt[1];
	MealNo_4th= Meal_Union.Meal[3].MealCnt[0]*16 + Meal_Union.Meal[3].MealCnt[1];

	/*获取价格信息*/
	price_1st=((Meal_Union.Meal[0].MealPrice[4]&0xf0)>>4)*10;
	price_1st+=Meal_Union.Meal[0].MealPrice[4]&0x0f;
	price_2nd= ((Meal_Union.Meal[1].MealPrice[4]&0xf0)>>4)*10;
	price_2nd+=Meal_Union.Meal[1].MealPrice[4]&0x0f;
	price_3rd= ((Meal_Union.Meal[2].MealPrice[4]&0xf0)>>4)*10;
	price_3rd+=Meal_Union.Meal[2].MealPrice[4]&0x0f;
	price_4th= ((Meal_Union.Meal[3].MealPrice[4]&0xf0)>>4)*10;
	price_4th+=Meal_Union.Meal[3].MealPrice[4]&0x0f;

	/*获取普通折扣信息*/
	cashcut_1st= ((Meal_Union.Meal[0].MealCut[0]-'0')*10+Meal_Union.Meal[0].MealCut[1]-'0');
	cashcut_2nd= ((Meal_Union.Meal[1].MealCut[0]-'0')*10+Meal_Union.Meal[1].MealCut[1]-'0');
	cashcut_3rd= ((Meal_Union.Meal[2].MealCut[0]-'0')*10+Meal_Union.Meal[2].MealCut[1]-'0');
	cashcut_4th= ((Meal_Union.Meal[3].MealCut[0]-'0')*10+Meal_Union.Meal[3].MealCut[1]-'0');

	if(cashcut_1st==0x00) cashcut_1st=100;
	if(cashcut_2nd==0x00) cashcut_2nd=100;
	if(cashcut_3rd==0x00) cashcut_3rd=100;
	if(cashcut_4th==0x00) cashcut_4th=100;

	/*获取深圳通折扣信息*/
	sztcut_1st= ((Meal_Union.Meal[0].SztCut[0]-'0')*10+Meal_Union.Meal[0].SztCut[1]-'0');
	sztcut_2nd= ((Meal_Union.Meal[1].SztCut[0]-'0')*10+Meal_Union.Meal[1].SztCut[1]-'0');
	sztcut_3rd= ((Meal_Union.Meal[2].SztCut[0]-'0')*10+Meal_Union.Meal[2].SztCut[1]-'0');
	sztcut_4th= ((Meal_Union.Meal[3].SztCut[0]-'0')*10+Meal_Union.Meal[3].SztCut[1]-'0');
	if(sztcut_1st==0x00) sztcut_1st=100;
	if(sztcut_2nd==0x00) sztcut_2nd=100;
	if(sztcut_3rd==0x00) sztcut_3rd=100;
	if(sztcut_4th==0x00) sztcut_4th=100;

	/*获取银联折扣信息*/
	gboccut_1st= ((Meal_Union.Meal[0].GbocCut[0]-'0')*10+Meal_Union.Meal[0].GbocCut[1]-'0');
	gboccut_2nd= ((Meal_Union.Meal[1].GbocCut[0]-'0')*10+Meal_Union.Meal[1].GbocCut[1]-'0');
	gboccut_3rd= ((Meal_Union.Meal[2].GbocCut[0]-'0')*10+Meal_Union.Meal[2].GbocCut[1]-'0');
	gboccut_4th= ((Meal_Union.Meal[3].GbocCut[0]-'0')*10+Meal_Union.Meal[3].GbocCut[1]-'0');  
	if(gboccut_1st==0x00) gboccut_1st=100;
	if(gboccut_2nd==0x00) gboccut_2nd=100;
	if(gboccut_3rd==0x00) gboccut_3rd=100;
	if(gboccut_4th==0x00) gboccut_4th=100; 

	/*获取会员卡折扣信息*/
	vipcut_1st= ((Meal_Union.Meal[0].VipCut[0]-'0')*10+Meal_Union.Meal[0].VipCut[1]-'0');
	vipcut_2nd= ((Meal_Union.Meal[1].VipCut[0]-'0')*10+Meal_Union.Meal[1].VipCut[1]-'0');
	vipcut_3rd= ((Meal_Union.Meal[2].VipCut[0]-'0')*10+Meal_Union.Meal[2].VipCut[1]-'0');
	vipcut_4th= ((Meal_Union.Meal[3].VipCut[0]-'0')*10+Meal_Union.Meal[3].VipCut[1]-'0');
	if(vipcut_1st==0x00) vipcut_1st=100;
	if(vipcut_2nd==0x00) vipcut_2nd=100;
	if(vipcut_3rd==0x00) vipcut_3rd=100;
	if(vipcut_4th==0x00) vipcut_4th=100; 

	/*获取餐品的位置信息*/
	Order_1st= (Meal_Union.Meal[0].MealOrder[0]);
	Order_2nd= (Meal_Union.Meal[1].MealOrder[0]);
	Order_3rd= (Meal_Union.Meal[2].MealOrder[0]);
	Order_4th= (Meal_Union.Meal[3].MealOrder[0]);

	/*获取餐品ID信息*/
	memcpy(MealID_1st,Meal_Union.Meal[0].MealID,sizeof(Meal_Union.Meal[0].MealID));
	memcpy(MealID_2nd,Meal_Union.Meal[1].MealID,sizeof(Meal_Union.Meal[1].MealID));
	memcpy(MealID_3rd,Meal_Union.Meal[2].MealID,sizeof(Meal_Union.Meal[2].MealID));
	memcpy(MealID_4th,Meal_Union.Meal[3].MealID,sizeof(Meal_Union.Meal[3].MealID)); 

	/*获取餐品名*/
	memcpy(mealname_1st,Meal_Union.Meal[0].MealName,sizeof(Meal_Union.Meal[0].MealName));
	memcpy(mealname_2nd,Meal_Union.Meal[1].MealName,sizeof(Meal_Union.Meal[1].MealName));
	memcpy(mealname_3rd,Meal_Union.Meal[2].MealName,sizeof(Meal_Union.Meal[2].MealName));
	memcpy(mealname_4th,Meal_Union.Meal[3].MealName,sizeof(Meal_Union.Meal[3].MealName));

	/*餐品的类型*/
	memcpy(MealType_1st,Meal_Union.Meal[0].MealType,sizeof(Meal_Union.Meal[0].MealType));
	memcpy(MealType_2nd,Meal_Union.Meal[1].MealType,sizeof(Meal_Union.Meal[1].MealType));
	memcpy(MealType_3rd,Meal_Union.Meal[2].MealType,sizeof(Meal_Union.Meal[2].MealType));
	memcpy(MealType_4th,Meal_Union.Meal[3].MealType,sizeof(Meal_Union.Meal[3].MealType));   
}

 /*******************************************************************************
 * 函数名称:SearchMeal                                                                   
 * 描    述:查找当前的餐品ID是否在用户已选择的餐品中，并将用户选择的餐品书和ID填
 *          入用户选择的结构体中                                                                                                                        
 * 输    入:当前的餐品的ID，当前餐品的数量                                                          
 * 输    出:无                                                                    
 * 返    回:falde:错误 true：正确                                                            
 * 修改日期:2014年9月9日                                                                   
 *******************************************************************************/ 
bool SearchMeal(uint32_t MealID ,uint32_t MealNum)
{
	/*如果有则赋值相关的餐品ID的数量*/
	if(UserActMessageWriteToFlash.UserAct.MealType_1st== UserActMessageWriteToFlash.UserAct.MealID)
	{
		UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= MealNum;
		UserActMessageWriteToFlash.UserAct.MealPrice_1st=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1); //»ñÈ¡µ±Ç°²ÍÆ·µÄ¼Û¸ñ
	}
	else if(UserActMessageWriteToFlash.UserAct.MealType_2nd== UserActMessageWriteToFlash.UserAct.MealID)
	{
		UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= MealNum;
		UserActMessageWriteToFlash.UserAct.MealPrice_2nd=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
	}
	else if(UserActMessageWriteToFlash.UserAct.MealType_3rd== UserActMessageWriteToFlash.UserAct.MealID)
	{
		UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= MealNum;
		UserActMessageWriteToFlash.UserAct.MealPrice_3rd=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);    
	}
	else if(UserActMessageWriteToFlash.UserAct.MealType_4th== UserActMessageWriteToFlash.UserAct.MealID)
	{
		UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= MealNum;
		UserActMessageWriteToFlash.UserAct.MealPrice_4th=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
	}
	/*如果没有则查找那一栏的餐品为空*/
	else
	{
		if(UserActMessageWriteToFlash.UserAct.MealType_1st== 0) //Èç¹ûµÚÒ»¸öIDÎª¿ÕµÄ»°£¬Ö±½Ó½«ID¸³Öµ¸ø
		{
			UserActMessageWriteToFlash.UserAct.MealType_1st= MealID;
			UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= MealNum;
			UserActMessageWriteToFlash.UserAct.MealPrice_1st=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
		}
		else if(UserActMessageWriteToFlash.UserAct.MealType_2nd== 0)
		{
			UserActMessageWriteToFlash.UserAct.MealType_2nd= MealID;
			UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= MealNum;
			UserActMessageWriteToFlash.UserAct.MealPrice_2nd=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
		}
		else if(UserActMessageWriteToFlash.UserAct.MealType_3rd== 0)
		{
			UserActMessageWriteToFlash.UserAct.MealType_3rd= MealID;
			UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= MealNum;
			UserActMessageWriteToFlash.UserAct.MealPrice_3rd=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
		}
		else if(UserActMessageWriteToFlash.UserAct.MealType_4th== 0)
		{
			UserActMessageWriteToFlash.UserAct.MealType_4th= MealID;
			UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= MealNum;
			UserActMessageWriteToFlash.UserAct.MealPrice_4th=GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
		}
		else
		{
			return false; //错误
		}
	}
	return true; //正确
}

 /*******************************************************************************
 * 函数名称:StatisticsTotal                                                                   
 * 描    述:统计餐品数量                                                                                                                       
 * 输    入:                                                         
 * 输    出:无                                                                    
 * 返    回:                                                       
 * 修改日期:2014年9月9日                                                                   
 *******************************************************************************/ 
 void StatisticsTotal(void)
 {	 
    uint8_t i,j;
	for(j = 0; j < MealKindTotoal; j++)	 //4´ú±í²ÍIDºÅ
	{
    	DefineMeal[j].MealCount = 0;
    	for(i = 0; i < 15; i++)
        {
	        if(FloorMealMessageWriteToFlash.FloorMeal[i].MealID == j + 1)	 /*Ç°ÃæÄÇ¸ö±íÊ¾µÄÊÇÐ¶ÔØflashÀïÃæµÄID ÓëÍâÃæµÄ½øÐÐ±È½Ï*/
	        {
	          DefineMeal[j].MealCount      = DefineMeal[j].MealCount + FloorMealMessageWriteToFlash.FloorMeal[i].MealCount;
	          DefineMeal[j].Position[i][0] = FloorMealMessageWriteToFlash.FloorMeal[i].FCount;
	          DefineMeal[j].Position[i][1] = FloorMealMessageWriteToFlash.FloorMeal[i].SCount;
	          DefineMeal[j].Position[i][2] = FloorMealMessageWriteToFlash.FloorMeal[i].TCount;
	        }
      	}
	}
 }
 /*******************************************************************************
 * 函数名称:InitSetting                                                                   
 * 描    述:
                                                                                                                   
 * 输    入:                                                       
 * 输    出:无                                                                    
 * 返    回:                                                         
 * 修改日期:2014年9月9日                                                                   
 *******************************************************************************/ 
void InitSetting(void)
{
    if(FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealID == CurFloor.MealID)
    {
		CurFloor.MealCount = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealCount;
		CurFloor.FCount    = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].FCount;
		CurFloor.SCount    = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].SCount;
		CurFloor.TCount    = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].TCount;
	}
	else
	{
		CurFloor.MealCount = 0;
		CurFloor.FCount    = 0;
		CurFloor.SCount    = 0;
		CurFloor.TCount    = 0;
	}

	VariableChage(row_1st,CurFloor.FCount);
	VariableChage(row_2nd,CurFloor.SCount);
	VariableChage(row_3rd,CurFloor.TCount);
}

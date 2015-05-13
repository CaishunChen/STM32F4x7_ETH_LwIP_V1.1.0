#ifndef __USER_ROCORD__
#define __USER_ROCORD__
#include <stdint.h>
#include <stm32f4xx.h>

#define FloorMealNum    15          /*定义有多少个行*/
#define MealKindTotoal  4           /*定义有几个餐品*/
typedef struct MealAt
{
    uint8_t MealPrice;         //餐价格
    uint8_t Position[15][3];   //餐放置的位置
    uint8_t MealCount;         //餐剩余总数
}MealAttribute;	               //定义单个菜系的属性。

extern MealAttribute DefineMeal[MealKindTotoal];	//定义MealKindTotoal个菜系

#define UserActiontotoaDatalSize  45
/*用户操作记录*/
typedef struct UserAction
{
    uint32_t MealType_1st;       //用户选择第一份餐品的ID
    uint32_t MealType_2nd;       //用户选择第二份餐品的ID
    uint32_t MealType_3rd;       //用户选择第三份餐品的ID
    uint32_t MealType_4th;       //用户选择第四份餐品的ID
    uint32_t MealCnt_1st_t;      //用户选择第一种餐品的数量未按下放入购物车键时的临时数量
    uint32_t MealCnt_2nd_t;      //用户选择第二种餐品的数量未按下放入购物车键时的临时数量
    uint32_t MealCnt_3rd_t;      //用户选择第三种餐品的数量未按下放入购物车键时的临时数量
    uint32_t MealCnt_4th_t;      //用户选择第四种餐品的数量未按下放入购物车键时的临时数量	
    uint32_t MealCnt_1st;        //用户选择第一份餐品的数量
    uint32_t MealCnt_2nd;        //用户选择第二份餐品的数量
    uint32_t MealCnt_3rd;        //用户选择第三份餐品的数量
    uint32_t MealCnt_4th;        //用户选择第四份餐品的数量
    uint32_t MealPrice_1st;      //用户选择第一份餐品的单价
    uint32_t MealPrice_2nd;      //用户选择第二份餐品的单价
    uint32_t MealPrice_3rd;      //用户选择第三份餐品的单价
    uint32_t MealPrice_4th;      //用户选择第四份餐品的单价   
    uint32_t LastMealPrice_1st;  //用户选择第一份餐品的单价
    uint32_t LastMealPrice_2nd;  //用户选择第二份餐品的单价
    uint32_t LastMealPrice_3rd;  //用户选择第三份餐品的单价
    uint32_t LastMealPrice_4th;  //用户选择第四份餐品的单价 
    uint32_t MealCost_1st;       //用户选择第一份餐品的总价
    uint32_t MealCost_2nd;       //用户选择第二份餐品的总价
    uint32_t MealCost_3rd;       //用户选择第三份餐品的总价
    uint32_t MealCost_4th;       //用户选择第四份餐品的总价  
    uint32_t LastMealCost_1st;   //用户选择第一份餐品的折后总价
    uint32_t LastMealCost_2nd;   //用户选择第二份餐品的折后总价
    uint32_t LastMealCost_3rd;   //用户选择第三份餐品的折后总价
    uint32_t LastMealCost_4th;   //用户选择第四份餐品的折后总价
    uint32_t PayShould;          //用户应付款总额
    uint32_t LastPayShould;      //用户应付款折后总额
    uint32_t MealID;             //用户当前选择的餐品ID 
    uint32_t Meal_totoal;        //用户选餐的总数
    uint32_t Meal_takeout;       //用户已取出的餐品数
    uint32_t PayForCoins;        //用户投入的硬币数	
    uint32_t PayForBills;        //用户投入的纸币数
    uint32_t PayForCards;        //用户应经刷卡的数  
    uint32_t PayAlready;         //用户已经付款总额
    uint32_t MoneyBack;          //用户付款之后要退的钱，也就是还有多少钱要找个客户
    uint32_t MoneyBackShould;    //应退钱的显示数据
    uint32_t MoneyBackAlready;   //已退钱的显示数据
    uint32_t PrintTick;          //是否打印小票标志
    uint32_t PayType;            //支付方式                   
    uint32_t Cancle;             //用户取消购买
    uint32_t MoneyPayBack_Already ;     //数据上传用                
    uint32_t MoneyPayBack_Already_1st;  //付钱后的找币    
    uint32_t MoneyPayBack_Already_2nd;  //取餐出错后的找币           
}UserSelection;

typedef union _UserActMessage
{
    UserSelection   UserAct;
    uint8_t         FlashBuffer[UserActiontotoaDatalSize*4];
}UserActMessage;
extern UserActMessage UserActMessageWriteToFlash;//用户数据写入flash

typedef struct FloorMeal
{
    uint8_t MealID;      //当前层存放餐的ID
    uint8_t MealCount;   //当前层存放餐的总数
    uint8_t FCount;      //第一列放餐的数量
    uint8_t SCount;      //第二列放餐的数量
    uint8_t TCount;	     //第三列放餐的数量
    uint8_t FloorNum;    //当前是第几层
}FloorAttribute;

typedef union _FloorMealMessage
{
    FloorAttribute      FloorMeal[FloorMealNum];
    uint8_t	            FlashBuffer[FloorMealNum*6]; 
}FloorMealMessage;

extern 	FloorMealMessage FloorMealMessageWriteToFlash;/*将数据写入flash的结构*/
extern  FloorAttribute FloorMeal[FloorMealNum], CurFloor;
extern uint8_t WaitTime ;
extern uint8_t WaitMealTime ;
extern uint32_t payfor_meal ;

typedef union _CoinsTotoalMessage
{
    uint16_t CoinTotoal;
    uint8_t  CoinsCnt[2];
}CoinsTotoalMessage;
extern CoinsTotoalMessage CoinsTotoalMessageWriteToFlash;/*硬币数结构体*/ 

typedef union _MealCompareData
{
    uint32_t MealCompareTotoal;
    uint8_t  MealComparePart[4];
}MealCompareDataStruct;
extern MealCompareDataStruct MealCompareData; /*餐品对比数据显示*/

/******************************************
相关折扣信息
*******************************************/
extern uint16_t MealNo_1st, price_1st; 
extern uint16_t MealNo_2nd, price_2nd; 
extern uint16_t MealNo_3rd, price_3rd; 
extern uint16_t MealNo_4th, price_4th; 

extern uint8_t sztcut_1st, gboccut_1st, vipcut_1st, vchatcut_1st, Order_1st, cashcut_1st;
extern uint8_t sztcut_2nd, gboccut_2nd, vipcut_2nd, vchatcut_2nd, Order_2nd, cashcut_2nd;
extern uint8_t sztcut_3rd, gboccut_3rd, vipcut_3rd, vchatcut_3rd, Order_3rd, cashcut_3rd;
extern uint8_t sztcut_4th, gboccut_4th, vipcut_4th, vchatcut_4th, Order_4th, cashcut_4th;

extern uint8_t  MealType_1st[4],MealType_2nd[4],MealType_3rd[4],MealType_4th[4];
extern uint8_t  mealname_1st[20],mealname_2nd[20],mealname_3rd[20],mealname_4th[20];
extern uint16_t MealID_1st[4],MealID_2nd[4],MealID_3rd[4],MealID_4th[4];

/*******************************************
下发餐品详细的结构体

*******************************************/
#define FloorMealNum    15           //一共15层
#define MealKindTotoal  4            // 有四种餐品

typedef struct _Meal_struction
{
	uint8_t MealID[4];     
	uint8_t MealName[20];  
	uint8_t MealCnt[2];    
	uint8_t MealPrice[6]; 
	uint8_t MealCut[2];   
	uint8_t VipCut[2];    
	uint8_t SztCut[2];   
	uint8_t GbocCut[2];    
	uint8_t VchatCut[2];  
	uint8_t MealOrder[1];  
	uint8_t MealType[4];   
}Meal_struction;  /*餐品的详细信息*/

typedef union _MEAL_UNION
{
	Meal_struction Meal[MealKindTotoal];
	uint8_t MealBuf[(sizeof(Meal_struction))*MealKindTotoal];
}MEAL_UNION;   /*四种餐品的数据*/

extern MEAL_UNION Meal_Union;

/*******************************************
这个结构体是用户选择取餐的时候需要定义的
********************************/
typedef struct
{
    uint8_t   MealID[4] ;        /*餐品的ID*/
    uint8_t   MealName[20];      /*餐品的名称*/
    uint8_t   MealNo ;           /*餐品的数量*/
    uint8_t   DealBalance[6];    /*支付金额*/
    uint8_t   PayType;            /*支付方式*/
    uint8_t   MealPrice[6];      /*餐品的价格*/
    uint8_t   Change[6] ;        /*应该找回多少钱*/
    uint8_t   RemainMealNum[2] ; /*剩余餐品数*/
}UserSlecMealInfoStruct;
extern UserSlecMealInfoStruct UserSlecMealInfo;
extern uint8_t  sell_type[4];
extern uint64_t err_record; 
extern uint8_t  Current;

void GetMealDetail(void);
bool SearchMeal(uint32_t MealID ,uint32_t MealNum);
void InitSetting(void);
 void StatisticsTotal(void);

#endif

#ifndef _bsp_H
#define _bsp_H

#include <stm32f4xx.h>
#include "user_record.h"

/*发送成功或者失败*/
#define Success 0x01
#define Failed  0x02

/******是否为测试状态********/
#define test

/*******主状态***********/
#define current_temperature  0x01
#define waitfor_money        0x02
#define payment_success      0x03
#define hpper_out            0x04
#define print_tick           0x05
#define meal_out             0x06
#define data_upload          0x07
#define data_record          0x08
#define erro_hanle           0x09

/**********等待餐品状态************/

#define takeing_meal   0x01 //正在取餐
#define tookone_meal   0x02 //已取一份餐
#define tookkind_meal  0x03 //已取一种餐
#define takeafter_meal 0x04 //取餐完毕
#define takemeal_erro  0x05 //取餐错误

/**********机内硬币保留数*********/
#define coins_remain   50  //保留50个

/**********异常******************/
#define outage_err              0x01 //断电
#define sdcard_err              0x02 //SD卡存储异常
#define billset_err             0x03 //纸币机异常
#define coinset_err             0x04 //投币机
#define coinhooperset_err       0x05 //退币机
#define coinhooperset_empty     0x06 //找零用光
#define printer_err             0x07 //打印机异常
#define cardread_err            0x08 //读卡器异常
#define network_err             0x09 //网络异常
#define signin_err              0x0a //签到异常
#define cardchck_err            0x0b //深圳通银行卡签到异常
#define upload_err              0x0e //数据上传的时候突然断电了
#define arm_limit               0x0f //机械手禁止初始化，标记

#define X_timeout               0x10 //x轴传感器超时
#define X_leftlimit             0x11 //马达左动作极限输出
#define X_rightlimit            0x12 //马达右动作极限输出
#define mealtake_timeout        0x13 //取餐口传感器超时
#define Y_timeout               0x14 //y轴传感器超时
#define link_timeout            0x15 //链接超时
#define Z_timeout               0x16 //z轴传感器超时
#define Z_uplimit               0x17 //z轴马达上动作超出
#define Z_downlimit             0x18 //z马达下动作超出
#define solenoid_timeout        0x19 //电磁阀超时
#define Eeprom_erro             0x1a //eeprom 异常
#define SendUR6Erro             0x1b //发送数据异常或超时
#define GetMealError            0x1c //机械手5秒取不到餐
#define MealNoAway              0x1d //餐在出餐口20秒还未被取走

typedef enum
{
    Status_OK= 0 ,       //函数操作成功 返回 0
    Status_Error ,      // 函数出错  返回1
    Status_Action     // 函数还执行 返回 2 
} status ;




typedef struct 
{
    uint8_t   F_RX_Length ;    //接受数据长度标志
    uint8_t   Response[6];     //ACK数组
    uint8_t   F_RX1_SYNC;     //是否开始接受有效数据
    uint8_t   PacketData[1000]; //接受的数据
    uint8_t   PacketDCS;       //校验码
    uint16_t  DataLength;    //数据长度
    uint16_t   RX_NUM ;       //定位
    uint8_t   Status   ;       //状态
    uint8_t   Checkoutright ;   //数据校验是否成功
    uint8_t   ACKStatus;
}USART_StatusTypeDef;

extern  USART_StatusTypeDef  STATUS ;

extern bool   cash_limit_flag;
extern uint8_t   exitflag;
extern uint8_t   CoinCountFlag;
extern uint8_t   TemperatureFlag  ;
extern uint8_t   BillActionFlag;      //表示纸币机在收钱中。这个时候不能关闭纸币机
extern uint8_t   CurrentPoint ;
extern uint8_t   CoinFlag;           //用于退币机的标志
extern uint8_t   TemperatureCur;
extern uint8_t   Current ;    //主状态
extern uint8_t   CurrentPointer; //餐品出餐状态
extern uint32_t  err_flag;  //错误状态
extern uint32_t  flash_record[18];

extern uint8_t last_discount_1st; //第一种餐品的付款时折扣
extern uint8_t last_discount_2nd;
extern uint8_t last_discount_3rd;
extern uint8_t last_discount_4th;

bool CloseCashSystem(void);
bool OpenCashSystem(void);
void MoveToFisrtMeal(void);
bool FindMeal(MealAttribute *DefineMeal); 
int8_t StrToBCD( uint8_t* pDest,const uint8_t* pSrc, uint8_t destLen);
uint8_t WaitPayMoney(void);
uint8_t WaitMeal(void);
void HardwaveInit(void);
void WaitTimeInit(uint8_t *Time);
void StateSend(void);
void DataUpload(unsigned char takemeal_flag);
void PrintTickFun(uint32_t *PrintTickFlag);
void PowerupAbnormalHandle(uint32_t erro_record);
void AbnormalHandle(uint32_t erro);
void PollAbnormalHandle(void);
void SaveUserData(void);
void ReadUserData(void);
void AcountCopy(void);
void PayBackUserMoney(void);
void ErrRecHandle(void);
void TrackDateToBuff(void);
void MachineHeatSet(void);
bool GetDiscountCost(uint8_t payment);
uint8_t GetDiscountValue(uint8_t meal_order);

#endif

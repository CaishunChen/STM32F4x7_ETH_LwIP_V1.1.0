#include "stm32f4xx.h"
#include "sdio.h"
/***************************************
硬件spi接口
***************************************/

/*************************************************************************
一下是模拟SPI	 start
**************************************************************************/
void SD_GPIO_Configuration(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;	   //CS
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;	   //MISO
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//GPIO_Mode_OUT;
// GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 	;   //SCLK
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;	   //MOSI
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
}

 /******************************************************************
 - 功能描述：延时函数
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：time:time值决定了延时的时间长短
 - 返回说明：无
 - 注：.....
 ******************************************************************/
uint8_t is_init;  //在初始化的时候设置此变量为1,同步数据传输（SPI）会放慢

#define DELAY_TIME 1000 //SD卡的复位与初始化时SPI的延时参数，根据实际速率修改其值，否则会造成SD卡复位或初始化失败

void delay(uint32_t time)
{
   while(time--);
}

void SD_spi_write(uint8_t x)
{
 uint8_t i;
 for(i=0;i<8;i++)
 {
	 ResetSCLK ;
	 if(x&0x80)
	 {
     GPIOB->BSRRL = GPIO_Pin_5;
	 }
	 else
	 {
     GPIOB->BSRRH = GPIO_Pin_5;
	 }
	 if(is_init) delay(DELAY_TIME);
	 SetSCLK 	;
   x <<= 1 ;
 }
}

/******************************************************************
 - 功能描述：IO模拟SPI，读取一个字节
 - 隶属模块：SD/SDHC模块
 - 函数属性：内部
 - 参数说明：无
 - 返回说明：返回读到的字节
 ******************************************************************/

uint8_t SD_spi_read() //SPI读一个字节
{
  uint8_t i,temp=0;

  for(i=0;i<8;i++)
  {
    SetSCLK ;
    if(is_init) delay(DELAY_TIME);
    ResetSCLK ;
    if(is_init) delay(DELAY_TIME);
    if(GPIOA->IDR & GPIO_Pin_6)
    {
      temp|=(0x80>>i);
    }
  }
  return (temp);
}

/*************************************************************************
以上是模拟SPI	 end
**************************************************************************/	

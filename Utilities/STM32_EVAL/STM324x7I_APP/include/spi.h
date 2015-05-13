#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//SPI驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  SPI_FLASH SPI Interface pins
  */

#define SPI_FLASH_SPI                           SPI2


#define SPI_FLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_10                  /* PA.05 */



#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPI_FLASH_SPI_SCK_SOURCE                GPIO_PinSource10

#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_2                  /* PA.06 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOC                       /* GPIOA */
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPI_FLASH_SPI_MISO_SOURCE               GPIO_PinSource2

#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_3                  /* PA.07 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOC                       /* GPIOA */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPI_FLASH_SPI_MOSI_SOURCE               GPIO_PinSource3

#define SPI_FLASH_CS_PIN                        GPIO_Pin_9                 /* PC.04 */
#define SPI_FLASH_CS_GPIO_PORT                  GPIOB                       /* GPIOC */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB

/* Exported macro ------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)

/* Exported functions ------------------------------------------------------- */
				    
// SPI总线速度设置 
#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 		4
#define SPI_SPEED_64 		5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7
						  	    													  
void SPI2_Init(void);			 //初始化SPI2口
void SPI2_SetSpeed(uint8_t SpeedSet); //设置SPI2速度   
uint8_t SPI2_ReadWriteByte(uint8_t TxData);//SPI2总线读写一个字节

void SPI1_Init(void);			 //初始化SPI2口
uint8_t SPI1_ReadWriteByte(uint8_t TxData);//SPI2总线读写一个字节		 
#endif


#ifndef __SD_H
#define __SD_H

#include "stm32f4xx.h"

#define SetSCLK 	 	      GPIOA->BSRRL = GPIO_Pin_5;
#define ResetSCLK 	 	      GPIOA->BSRRH = GPIO_Pin_5;

extern uint8_t is_init; 

uint8_t SD_SPI_ReadWriteByte(uint8_t TxData);
uint8_t SD_spi_read(void); 
void    SD_spi_write(uint8_t x);
void    SD_GPIO_Configuration(void);

#endif




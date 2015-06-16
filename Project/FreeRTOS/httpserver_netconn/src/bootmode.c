

#include <stm32f4xx.h>
#include "bootmode.h"
#include "app_info.h"

static bool bootmode_check_gpi(void)
{
	bool ret = false;
	uint8_t bit = 0;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(GPIOE,&GPIO_InitStructure);

	/* Check the state of the GPI Pin to see if a load was requested */
	bit = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6);

	if (bit == 0)
		ret = true;

	GPIO_DeInit(GPIOE);

	return ret;
}

static bool bootmode_check_software_flag(void)
{
	struct app_handshake_block *handshake = (struct app_handshake_block*)HANDSHAKE_ADDR;
	bool ret = false;
	
	if (handshake->loader_magic == LOADER_KEY) {
		handshake->loader_magic = 0;
		ret = true;
	}

	return ret;
}

bool bootmode_upgrade_requested(void)
{
	bool gpi, sw;
	bool ret = false;

	gpi = bootmode_check_gpi();
	sw = bootmode_check_software_flag();

	if (gpi || sw)
		ret = true;

	return ret;
}

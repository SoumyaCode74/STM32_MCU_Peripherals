/**
 *@file stm32f429xx_rcc_driver.c
 *@brief Created on: Feb 15, 2024
 *@author: Soumyadeep Bose
 */
#include "stm32f429xx_rcc_driver.h"

void RCC_EnableClock(RCC_Handle_t * pRCC, uint8_t port){
	uint32_t * pAHB1ENR = (uint32_t *)&pRCC->pRCC->AHB1ENR; /*!< Obtain the address of the AHB1ENR */
	pAHB1ENR[pRCC->RCC_Config.bus] |= (0x1 << port); /*!< Configure the ENR of the required bus using the address of AHB1ENR and offset */
}
void RCC_ResetClock(RCC_Handle_t * pRCC, uint8_t port){
	uint32_t * pAHB1RST = (uint32_t *)&pRCC->pRCC->AHB1RSTR; /*!< Obtain the address of the AHB1RST */
	pAHB1RST[pRCC->RCC_Config.bus] |= (0x1 << port); /*!< Configure the RST of the required bus using the address of AHB1ENR and offset */

}
void RCC_SetPrescaler(RCC_Handle_t * pRCC, uint8_t scaler){
	/*!< Obtain whether clock bus is AHB category */
	ClockBus_t selected_bus = pRCC->RCC_Config.bus;
	if(selected_bus == AHB1 || selected_bus == AHB2 || selected_bus == AHB3){
		pRCC->pRCC->CFGR &= ~(0xF << 4);
		pRCC->pRCC->CFGR |=  (scaler << 4);
	}
	else if(selected_bus == APB1){
		pRCC->pRCC->CFGR &= ~(0x7 << 10);
		pRCC->pRCC->CFGR |=  (scaler << 10);
	}
	else{
		pRCC->pRCC->CFGR &= ~(0x7 << 13);
		pRCC->pRCC->CFGR |=  (scaler << 13);
	}
}
ClockSrc_t RCC_GetSystemClock(RCC_Handle_t * pRCC){
	return ( (pRCC->pRCC->CFGR >> 2) & 0x3);
}
uint16_t RCC_GetPrescaler(RCC_Handle_t * pRCC){
	/*!< Obtain whether clock bus is AHB category */
	ClockBus_t selected_bus = pRCC->RCC_Config.bus;
	uint16_t prescaler_setting = 0;
	if(selected_bus == AHB1 || selected_bus == AHB2 || selected_bus == AHB3){
		prescaler_setting = (pRCC->pRCC->CFGR >> 4) & 0xF;
		switch(prescaler_setting){
			case AHB_DIVIDE_BY_2:
				return 2;
			case AHB_DIVIDE_BY_4:
				return 4;
			case AHB_DIVIDE_BY_8:
				return 8;
			case AHB_DIVIDE_BY_16:
				return 16;
			case AHB_DIVIDE_BY_64:
				return 64;
			case AHB_DIVIDE_BY_128:
				return 128;
			case AHB_DIVIDE_BY_256:
				return 256;
			case AHB_DIVIDE_BY_512:
				return 512;
			default:
				return 1;
		}
	}
	else if(selected_bus == APB1){
		prescaler_setting = (pRCC->pRCC->CFGR >> 10) & 0x7;
		switch(prescaler_setting){
			case APB1_DIVIDE_BY_2:
				return 2;
			case APB1_DIVIDE_BY_4:
				return 4;
			case APB1_DIVIDE_BY_8:
				return 8;
			case APB1_DIVIDE_BY_16:
				return 16;
			default:
				return 1;
		}
	}
	else{
		prescaler_setting = (pRCC->pRCC->CFGR >> 13) & 0x7;
		switch(prescaler_setting){
			case APB2_DIVIDE_BY_2:
				return 2;
			case APB2_DIVIDE_BY_4:
				return 4;
			case APB2_DIVIDE_BY_8:
				return 8;
			case APB2_DIVIDE_BY_16:
				return 16;
			default:
				return 1;
		}
	}
}




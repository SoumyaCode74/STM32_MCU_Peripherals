/*
 * stm32f429xx_gpio_driver.c
 *
 *  Created on: Feb 15, 2024
 *      Author: Soumyadeep Bose
 */

#include "stm32f429xx_gpio_driver.h"

void GPIO_ConfigureMode(GPIO_Handle_t * pGPIOHandle){
	uint8_t pin = pGPIOHandle->GPIO_PinConfig.Pin;
	pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (pin * 2));
	pGPIOHandle->pGPIOx->MODER |= ( (pGPIOHandle->GPIO_PinConfig.Mode) << (pin * 2));
}
void GPIO_ConfigureOutputType(GPIO_Handle_t * pGPIOHandle){
	uint8_t pin = pGPIOHandle->GPIO_PinConfig.Pin;
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pin);
	pGPIOHandle->pGPIOx->OTYPER |= ( (pGPIOHandle->GPIO_PinConfig.OType) << pin);
}
void GPIO_ConfigureOutputSpeed(GPIO_Handle_t * pGPIOHandle){
	uint8_t pin = pGPIOHandle->GPIO_PinConfig.Pin;
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (pin * 2));
	pGPIOHandle->pGPIOx->OSPEEDR |= ( (pGPIOHandle->GPIO_PinConfig.OutputSpeed) << (pin * 2));
}
void GPIO_ConfigurePullUpDown(GPIO_Handle_t * pGPIOHandle){
	uint8_t pin = pGPIOHandle->GPIO_PinConfig.Pin;
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (pin * 2));
	pGPIOHandle->pGPIOx->PUPDR |= ( (pGPIOHandle->GPIO_PinConfig.PullUpDown) << (pin * 2));
}
void GPIO_ConfigureAlternateFunction(GPIO_Handle_t * pGPIOHandle){
	uint8_t pin = pGPIOHandle->GPIO_PinConfig.Pin;
	pGPIOHandle->pGPIOx->AFR[(pin / 8)] &= ~(0xF << (pin * 4));
	pGPIOHandle->pGPIOx->AFR[(pin / 8)] |= ( (pGPIOHandle->GPIO_PinConfig.AlternateFunction) << (pin * 4));
}
void GPIO_Init(GPIO_Handle_t * pGPIOHandle){
	GPIO_Port Port = ((uint32_t)pGPIOHandle->pGPIOx - (uint32_t)GPIOA_BASE)/1024;	/*!< Determine which port is passed to this function */
	RCC_EnableClock(pGPIOHandle->hRCC, Port);										/*!< Enable clock to the determined port */
	GPIO_ConfigureMode(pGPIOHandle);												/*!< Set mode of GPIO pin */
	GPIO_ConfigurePullUpDown(pGPIOHandle); 											/*!< Set pin to pull-up */
	if(pGPIOHandle->GPIO_PinConfig.Mode == 0x2){
		GPIO_ConfigureAlternateFunction(pGPIOHandle); 								/*!< Set pin to alternate function mode if mode is set to be */
	}
	/*!< Further API calls to be developed based on mode configuration of the desired pin */
}
void GPIO_Set_Bit(GPIO_Handle_t * pGPIOHandle){
	pGPIOHandle->pGPIOx->BSRR |= (0x1 << pGPIOHandle->GPIO_PinConfig.Pin);
}
void GPIO_Clear_Bit(GPIO_Handle_t * pGPIOHandle){
	pGPIOHandle->pGPIOx->BSRR &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.Pin);
	pGPIOHandle->pGPIOx->BSRR |= (0x1 << (16 + pGPIOHandle->GPIO_PinConfig.Pin));
}

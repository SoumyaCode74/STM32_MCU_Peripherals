/**
 * @file stm32f429xx_i2c_driver.c
 * @brief Driver source code for I2C peripheral of STM32F429xx MCU
 * 		  Created: 15-02-2024
 * @author Soumyadeep Bose
 */

#include "stm32f429xx_i2c_driver.h"

uint32_t GetPCLKFrequency(I2C_Handle_t * pI2CHanlde){
	/*!< First find the clock source */
	/*!< Second, find the prescaler setting */
	/*!< Third, calculate and return the PCLK frequency */
	/*!< This function is not yet developed for PLL */
	uint8_t clock_source = pI2CHanlde->hGPIO[0].hRCC->RCC_Config.clock_source;
	uint16_t prescaler	 = RCC_GetPrescaler(pI2CHanlde->hGPIO->hRCC);
	if(clock_source == HSI || clock_source == PLL){
			return HSI_VALUE/prescaler;
	}
	else{
		return HSE_VALUE/prescaler;
	}
}

void I2C_Init(I2C_Handle_t * pI2CHandle){
	uint32_t pclk_freq			= 0;		///< Variable to store the current configured PCLK frequency
	/*!< Disable the I2C peripheral */
	pI2CHandle->pI2Cx->CR1 &= I2C_DISABLE;
	/*!< Configure the mode (Standard or Fast) */
	if(pI2CHandle->I2C_PinConfig.I2C_SCLSpeed <= I2C_SM_SPEED_LIMIT){
		pI2CHandle->pI2Cx->CCR &= ~BIT15;
	}
	else{
		pI2CHandle->pI2Cx->CCR |= BIT15;
	}
	/*!< Configure the duty cycle of fast mode */
	if(pI2CHandle->I2C_PinConfig.I2C_SCLSpeed > I2C_SM_SPEED_LIMIT){
		if(pI2CHandle->I2C_PinConfig.I2C_FMDutyCycle == PERCENT_36){
			pI2CHandle->pI2Cx->CCR |= BIT14;
		}
		else{
			pI2CHandle->pI2Cx->CCR &= ~BIT14;
		}
	}
	/*!< Configure the speed of the serial clock (SCLK) */
	pclk_freq = GetPCLKFrequency(pI2CHandle);
	pI2CHandle->pI2Cx->CR2 &= ~(BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
	pI2CHandle->pI2Cx->CR2 |= pclk_freq/(1000000);	///< Write the number corresponding to the APB clock frequency
	/*!< Configure the value of CCR	 */
	/*!< T_sclk = n x CCR x T_PCLK */
	if(pI2CHandle->I2C_PinConfig.I2C_SCLSpeed <= I2C_SM_SPEED_LIMIT){
		pI2CHandle->pI2Cx->CCR |= (pclk_freq/(2 * pI2CHandle->I2C_PinConfig.I2C_SCLSpeed)) & 0xFFF; ///< The first 12-bits of CCR register is CCR
	}
	else{
		if(pI2CHandle->I2C_PinConfig.I2C_FMDutyCycle == 0){
			pI2CHandle->pI2Cx->CCR |= (pclk_freq/(3 * pI2CHandle->I2C_PinConfig.I2C_SCLSpeed)) & 0xFFF;
		}

		else{
			pI2CHandle->pI2Cx->CCR |= (pclk_freq/(25 * pI2CHandle->I2C_PinConfig.I2C_SCLSpeed)) & 0xFFF;
		}
	}
	/*!< Configure the device address (if the device is slave) */
	/*!< Enable the peripheral */
	pI2CHandle->pI2Cx->CR1 |= BIT0;
	/*!< Enable the Acking after enabling I2C peripheral */
	pI2CHandle->pI2Cx->CR1 |= BIT10;
	/*!< Configure the rise time for I2C pins */
}

void I2C_MasterSendData(I2C_Handle_t * pI2CHandle, const uint8_t * pSlaveAddress, uint8_t * pData){

}

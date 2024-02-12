/*
 * stm32f429xx_i2c_driver.h
 *
 *  Created on: Feb 11, 2024
 *      Author: Soumyadeep Bose
 */

#ifndef STM32F429XX_I2C_DRIVER_H_
#define STM32F429XX_I2C_DRIVER_H_

#include "stm32f429xx_general.h"

typedef struct{
	uint32_t	I2C_SCLSpeed;
	uint8_t		I2C_DeviceAddress;
	uint8_t		I2C_AckControl : 1;
	uint16_t	I2C_FMDutyCycle : 3;
}I2C_PinConfig_t;

typedef struct{
	I2C_RegAddr_t *pI2Cx;			// Pointer to the base address of the I2C peripheral needed
	I2C_PinConfig_t I2C_PinConfig;
}I2C_Handle_t;

void I2C_Init(I2C_Handle_t *); // Sending pointer to structure to modify the same I2C object
void I2C_Master_TX(I2C_Handle_t *);
void I2C_Master_RX(I2C_Handle_t *);
void I2C_Slave_TX(I2C_Handle_t *);
void I2C_Slave_RX(I2C_Handle_t *);
void I2C_Error_Interrupt_Handler(I2C_Handle_t *);
void I2C_Event_Interrupt_Handler(I2C_Handle_t *);


#endif /* STM32F429XX_I2C_DRIVER_H_ */

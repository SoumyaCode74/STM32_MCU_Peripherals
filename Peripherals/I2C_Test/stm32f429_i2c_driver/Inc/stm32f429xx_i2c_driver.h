/**
 ****************************************************************************
 * @file stm32f429xx_i2c_driver.h
 * @brief 	Classes and API functions for driving an I2C peripheral for STM32F429xx MCUs
 *  		Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 ****************************************************************************
 */

/** @addtogroup I2C_Driver_Header
  * @{
  */
/**
 * @brief 	Classes and API functions for driving an I2C peripheral for STM32F429xx MCUs
 *  		Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 */
#ifndef STM32F429XX_I2C_DRIVER_H_
#define STM32F429XX_I2C_DRIVER_H_

/** @addtogroup Includes
  * @{
*/
#include "stm32f429xx_general.h"
/**
 * @}
 */
/** @addtogroup Classes
 * @{
 */
typedef struct{
	uint32_t	I2C_SCLSpeed;			///< The SCLK speed required for the I2C communication
	uint8_t		I2C_DeviceAddress;  	///< The 7-bit device address to configure, if it's a slave device
	uint8_t		I2C_AckControl : 1;		///< The acknowledgement bit: 0 for No ACK, 1 for ACK, from slave
	uint8_t		I2C_FMDutyCycle : 3;	///< The duty cycle setting for Fast mode communication
}I2C_PinConfig_t;	///< Class defining the basic configuration to have for the I2C peripheral

typedef struct{
	I2C_RegAddr_t *pI2Cx;				///< Pointer to the base address of the I2C peripheral needed
	I2C_PinConfig_t I2C_PinConfig;		///< The PinConfig object for the basic configuration settings
}I2C_Handle_t;	///< Class defining the handle objects for the I2C peripheral to work with
/**
 * @}
 */

/**
 * @addtogroup Functions
 * @{
 */

/**
  * @brief  Initialize the I2C peripheral module based on the SCLK frequency,
  *         Device address (if it is a slave), Acknowledgement enable, and Fast Mode Duty Cycle
  * @param  I2C_Handle_t * Pointer to instance of class I2C_Handle_t
  * @retval None
  */
void I2C_Init(I2C_Handle_t *);
/**
  * @brief  De-initialize the I2C peripheral module
  * @param  I2C_Handle_t * Pointer to instance of class I2C_Handle_t
  * @retval None
  */
void I2C_DeInit(I2C_Handle_t *);
/**
  * @brief  Transmit the data byte to the slave
  * @param  pI2CHandle  	Pointer to the master I2C instance of class I2C_Handle_t
  * @param	pSlaveAddress	Pointer to the slave address
  * @param	pData			Pointer to the data byte to be sent
  * @retval None
  */
void I2C_MasterSendData(I2C_Handle_t * pI2CHandle, const uint8_t * pSlaveAddress, uint8_t * pData);
/**
  * @brief  Receive the data byte from the slave
  * @param  pI2CHandle  	Pointer to the master I2C instance of class I2C_Handle_t
  * @param	pSlaveAddress	Pointer to the slave address
  * @param	pReceive		Pointer to the data byte where the data to receive
  * @retval None
  */
void I2C_MasterReceiveData(I2C_Handle_t * pI2CHandle, const uint8_t * pSlaveAddress, uint8_t * pReceive);
/**
  * @brief  Transmit the data byte to the master
  * @param  pI2CHandle  	Pointer to the slave I2C instance of class I2C_Handle_t
  * @param	pData			Pointer to the data byte to be sent
  * @retval None
  */
void I2C_SlaveSendData(I2C_Handle_t * pI2CHandle, uint8_t * pData);
/**
  * @brief  Receive the data byte from the master
  * @param  pI2CHandle  	Pointer to the slave I2C instance of class I2C_Handle_t
  * @param	pReceive		Pointer to the data byte where the data to receive
  * @retval None
  */
void I2C_SlaveReceiveData(I2C_Handle_t * pI2CHandle, uint8_t * pReceive);
/**
  * @brief  Callback function for I2C Error Interrupt handler
  * @param  pI2CHandle  	Pointer to the slave/master I2C instance of class I2C_Handle_t
  * @retval None
  */
void I2C_Error_Interrupt_Handler(I2C_Handle_t * pI2CHandle);
/**
  * @brief  Callback function for I2C Event Interrupt handler
  * @param  pI2CHandle  	Pointer to the slave/master I2C instance of class I2C_Handle_t
  * @retval None
  */
void I2C_Event_Interrupt_Handler(I2C_Handle_t * pI2CHandle);
/**
 * @}
 */
#endif /* STM32F429XX_I2C_DRIVER_H_ */

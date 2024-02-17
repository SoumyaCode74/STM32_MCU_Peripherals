/**
 ****************************************************************************
 * @file stm32f429xx_gpio_driver.h
 * @brief 	Classes and API functions for driving a GPIO peripheral for STM32F429xx MCUs
 *  		Created on: Feb 15, 2024
 * @author: Soumyadeep Bose
 ****************************************************************************
 */

/** @addtogroup GPIO_Driver_Header
  * @{
  */
/**
 * @brief 	Classes and API functions for driving an I2C peripheral for STM32F429xx MCUs
 *  		Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 */
#ifndef STM32F429XX_GPIO_DRIVER_H_
#define STM32F429XX_GPIO_DRIVER_H_

/** @addtogroup Includes
  * @{
*/
#include "stm32f429xx_general.h"
#include "stm32f429xx_rcc_driver.h"
/**
 * @}
 */
/** @addtogroup Classes
 * @{
 */
typedef enum{
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
	PORT_G,
	PORT_H,
	PORT_I,
	PORT_J,
	PORT_K
}GPIO_Port; 								///< Enum to determine which port is used

typedef struct{
	uint8_t			Pin  : 4;				///< The pin to configure
	uint8_t			Mode : 2;				///< The direction to set for the GPIO pin
	uint8_t			OType : 1;				///< Output type: 0 - Push-pull, 1 - Open Drain
	uint8_t			PullUpDown : 2;			///< Push-pull
	uint8_t			OutputSpeed : 2;		///< Speed at which output should be delivered
	uint8_t			AlternateFunction : 4;	///< Alternate function mode value if required
}GPIO_PinConfig_t;							///< Class defining the basic configuration to have for the GPIO pin

typedef struct{
	RCC_Handle_t	*hRCC;					///< Pointer to the RCC handle created for the currently used GPIO instance
	GPIO_RegAddr_t *pGPIOx;					///< Pointer to the base address of the GPIO port needed
	GPIO_PinConfig_t GPIO_PinConfig;		///< The PinConfig object for the basic configuration settings
}GPIO_Handle_t;								///< Class defining the handle objects for the GPIO port to work with
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
void GPIO_Init(GPIO_Handle_t * pGPIOHandle);
/**
  * @brief  De-initialize the I2C peripheral module
  * @param  I2C_Handle_t * Pointer to instance of class I2C_Handle_t
  * @retval None
  */
void GPIO_DeInit(GPIO_Handle_t * pGPIOHandle);
/**
  * @brief  Transmit the data byte to the slave
  * @param  pI2CHandle  	Pointer to the master I2C instance of class I2C_Handle_t
  * @param	pSlaveAddress	Pointer to the slave address
  * @param	pData			Pointer to the data byte to be sent
  * @retval None
  */
void GPIO_ConfigureMode(GPIO_Handle_t * pGPIOHandle);
void GPIO_ConfigureOutputType(GPIO_Handle_t * pGPIOHandle);
void GPIO_ConfigureOutputSpeed(GPIO_Handle_t * pGPIOHandle);
void GPIO_ConfigurePullUpDown(GPIO_Handle_t * pGPIOHandle);
void GPIO_ConfigureAlternateFunction(GPIO_Handle_t * pGPIOHandle);
void GPIO_Set_Bit(GPIO_Handle_t * pGPIOHandle);
/**
  * @brief  Receive the data byte from the slave
  * @param  pI2CHandle  	Pointer to the master I2C instance of class I2C_Handle_t
  * @param	pSlaveAddress	Pointer to the slave address
  * @param	pReceive		Pointer to the data byte where the data to receive
  * @retval None
  */
void GPIO_Clear_Bit(GPIO_Handle_t * pGPIOHandle);
/**
  * @brief  Transmit the data byte to the master
  * @param  pI2CHandle  	Pointer to the slave I2C instance of class I2C_Handle_t
  * @param	pData			Pointer to the data byte to be sent
  * @retval None
  */
uint8_t GPIO_Get_BitValue(GPIO_Handle_t * pGPIOHandle);
/**
  * @brief  Receive the data byte from the master
  * @param  pI2CHandle  	Pointer to the slave I2C instance of class I2C_Handle_t
  * @param	pReceive		Pointer to the data byte where the data to receive
  * @retval None
  */
/**
 * @}
 */
#endif /* STM32F429XX_I2C_DRIVER_H_ */

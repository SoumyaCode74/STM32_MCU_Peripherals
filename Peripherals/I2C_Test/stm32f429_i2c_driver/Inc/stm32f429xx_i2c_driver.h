/**
 ****************************************************************************
 * @file stm32f429xx_i2c_driver.h
 * @brief 	Classes and API functions for driving an I2C peripheral for STM32F429xx MCUs
 *  		Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 ****************************************************************************
 */

/** @defgroup I2C_Driver_Header
  * @{
  */
/**
 * @brief 	Classes and API functions for driving an I2C peripheral for STM32F429xx MCUs
 *  		Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 */
#ifndef STM32F429XX_I2C_DRIVER_H_
#define STM32F429XX_I2C_DRIVER_H_

/** @defgroup Includes
  * @{
*/
#include "stm32f429xx_general.h"
#include "stm32f429xx_gpio_driver.h"
/**
 * @}
 */
/**
 * @defgroup Defines
 * @{
 */
#define I2C_DISABLE				(~BIT0)				///< I2C Disable macro
#define I2C_SM_SPEED_LIMIT		(100*1000)			///< I2C Standard mode speed limit 100kHz
#define I2C_FM_SPEED_LIMIT		(400*1000)			///< I2C Fast mode speed limit 400kHz
#define I2C_TIMEOUT_F			BIT14				///< I2C Timeout flag
#define I2C_PECERR_F			BIT12				///< I2C PEC Error flag
#define I2C_OVR_F				BIT11				///< I2C Overrun/Underrun error
#define I2C_AF_F				BIT10				///< I2C Acknowledge failure error
#define I2C_ARLO_F				BIT9				///< I2C Arbitration lost error
#define I2C_BERR_F				BIT8				///< I2C Bus error
#define I2C_TXE_F				BIT7				///< I2C Transmitter data register empty
#define I2C_RXNE_F				BIT6				///< I2C Receiver data register not empty
#define I2C_STOP_F				BIT4				///< I2C Stop detection error
#define I2C_ADD10_F				BIT3				///< I2C 10-bit header sent (Master mode)
#define I2C_BTF_F				BIT2				///< I2C Byte transfer finished error
#define I2C_ADDR_F				BIT1				///< I2C Address sent(master mode)/matched (slave mode)
#define I2C_SB_F				BIT0				///< I2C Start bit error (Master mode)

/**
 * @}
 */

/** @defgroup Classes
 * @{
 */
typedef struct{
	uint32_t	I2C_SCLSpeed;			///< The SCLK speed required for the I2C communication
	uint8_t		I2C_DeviceAddress;  	///< The 7-bit device address to configure, if it's a slave device
	uint8_t		I2C_AckControl : 1;		///< The acknowledgement bit: 0 for No ACK, 1 for ACK, from slave
	uint8_t		I2C_FMDutyCycle : 3;	///< The duty cycle setting for Fast mode communication
}I2C_PinConfig_t;						///< Class defining the basic configuration to have for the I2C peripheral

typedef struct{
	GPIO_Handle_t	*hGPIO;				///< Pointer to the GPIO handle created for the currently used I2C instance
	RCC_Handle_t	*hRCCPeriph;		///< Pointer to RCC handle created specifically for the currently used I2C peripheral instance
	I2C_RegAddr_t 	*pI2Cx;				///< Pointer to the base address of the I2C peripheral needed
	I2C_PinConfig_t I2C_PinConfig;		///< The PinConfig object for the basic configuration settings
	uint8_t 		*pTxBuffer; 		/* !< To store the app. Tx buffer address > */
	uint8_t 		*pRxBuffer;			/* !< To store the app. Rx buffer address > */
	uint32_t 		TxLen;				/* !< To store Tx len > */
	uint32_t 		RxLen;				/* !< To store Tx len > */
	uint8_t 		TxRxState;			/* !< To store Communication state > */
	uint8_t 		DevAddr;			/* !< To store slave/device address > */
    uint32_t        RxSize;				/* !< To store Rx size  > */
    uint8_t         Sr;					/* !< To store repeated start value  > */
}I2C_Handle_t;							///< Class defining the handle objects for the I2C peripheral to work with

typedef enum{
	PERCENT_33,
	PERCENT_36
}FastMode_DutyCycle;
/**
 * @}
 */

/**
 * @defgroup Functions
 * @{
 */

/**
  * @brief  Return the clock frequency of PCLK
  * @param  I2C_Handle_t * Pointer to instance of class I2C_Handle_t
  * @retval Clock frequency of PCLK
  */
uint32_t GetPCLKFrequency(I2C_Handle_t *);
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
  * @param	pLength			Pointer to the length of the data bytes to be sent
  * @retval None
  */
void I2C_MasterSendData(I2C_Handle_t * pI2CHandle, const uint8_t * pSlaveAddress, uint8_t * pData, uint32_t * pLength);
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
 * @brief Get the status of flags from status register
 * @param	pI2CHandle		Pointer to the I2C instance of class I2C_Handle_t
 * @param	flag_pos		Bit position of the flag in the status register
 * @retval	Flag bit status
 */
uint8_t I2C_GetFlagStatus(I2C_RegAddr_t * pI2Cx, uint8_t flag_pos);
/**
 * @}
 */
#endif /* STM32F429XX_I2C_DRIVER_H_ */

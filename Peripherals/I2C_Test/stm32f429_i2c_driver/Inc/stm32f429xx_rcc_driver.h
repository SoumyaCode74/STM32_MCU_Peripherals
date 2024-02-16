/**
 ****************************************************************************
 * @file stm32f429xx_i2c_driver.h
 * @brief 	Classes and API functions for driving an I2C peripheral for STM32F429xx MCUs
 *  		Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 ****************************************************************************
 */

/** @addtogroup RCC_Driver_Header
  * @{
  */
/**
 * @brief 	Classes and API functions for driving an RCC peripheral for STM32F429xx MCUs
 *  		Created on: Feb 15, 2024
 * @author: Soumyadeep Bose
 */
#ifndef STM32F429XX_RCC_DRIVER_H_
#define STM32F429XX_RCC_DRIVER_H_
/** @defgroup Includes
  * @{
*/
#include "stm32f429xx_general.h"	///< General includes and defines
/**
 * @}
 */
/**
 * @defgroup Defines
 * @{
 */
#define HSI_VALUE    (16000000U)	///< HSI clock frequency 16 MHz
#define HSE_VALUE	 (25000000U)	///< HSE clock frequency 25 MHz
/**
 * @}
 */
/**
 * @defgroup Classes
 * @{
 */
typedef enum{
	AHB1,
	AHB2,
	AHB3,
	APB1,
	APB2
}ClockBus_t;
typedef enum{
	HSI,
	HSE,
	PLL
}ClockSrc_t;
typedef struct{
	uint8_t		clock_source;
	ClockBus_t 	bus;
	uint8_t		prescaler;
}RCC_Config_t;
typedef struct{
	RCC_RegAddr_t *pRCC;
	RCC_Config_t RCC_Config;
}RCC_Handle_t;
/**
 * @}
 */


/**
 * @addtogroup Functions
 * @{
 */
void RCC_EnableClock(RCC_Handle_t * pRCC, uint8_t pos);
void RCC_ResetClock(RCC_Handle_t * pRCC, uint8_t pos);
void RCC_SetPrescaler(RCC_Handle_t * pRCC, uint8_t scaler);
ClockSrc_t RCC_GetSystemClock();
uint8_t RCC_GetPrescaler(RCC_Handle_t * pRCC);
/**
 * @}
 */
/**
 * @}
 */
#endif /* STM32F429XX_I2C_DRIVER_H_ */

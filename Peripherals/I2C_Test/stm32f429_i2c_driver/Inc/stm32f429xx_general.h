/**
 * @file stm32f429xx_general.h
 * @brief All purpose defines and includes
 *  Created on: Feb 11, 2024
 * @author: Soumyadeep Bose
 */

#ifndef STM32F429XX_GENERAL_H_
#define STM32F429XX_GENERAL_H_

/**
 * @addtogroup Includes
 * @{
 */
#include <stdint.h>
#include "stm32f429xx.h"
/**
 * @}
 */
/**
 * @defgroup Defines
 * @{
 */
/**
 * @}
 */
/**
 * @defgroup Aliases
 * @{
 */
typedef RCC_TypeDef RCC_RegAddr_t;
typedef GPIO_TypeDef GPIO_RegAddr_t;
typedef USART_TypeDef USART_RegAddr_t;
typedef SPI_TypeDef SPI_RegAddr_t;
typedef I2C_TypeDef I2C_RegAddr_t;
/**
 * @}
 */

/**
 * @defgroup Classes
 * @{
 */
typedef enum{
	BIT0 = (0x1 << 0),
	BIT1 = (BIT0 << 1),
	BIT2 = (BIT1 << 1),
	BIT3 = (BIT2 << 1),
	BIT4 = (BIT3 << 1),
	BIT5 = (BIT4 << 1),
	BIT6 = (BIT5 << 1),
	BIT7 = (BIT6 << 1),
	BIT8 = (BIT7 << 1),
	BIT9 = (BIT8 << 1),
	BIT10 = (BIT9 << 1),
	BIT11 = (BIT10 << 1),
	BIT12 = (BIT11 << 1),
	BIT13 = (BIT12 << 1),
	BIT14 = (BIT13 << 1),
	BIT15 = (BIT14 << 1),
}Bit_t;

#endif /* STM32F429XX_GENERAL_H_ */

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
	BIT16 = (BIT15 << 1),
	BIT17 = (BIT16 << 1),
	BIT18 = (BIT17 << 1),
	BIT19 = (BIT18 << 1),
	BIT20 = (BIT19 << 1),
	BIT21 = (BIT20 << 1),
	BIT22 = (BIT21 << 1),
	BIT23 = (BIT22 << 1),
	BIT24 = (BIT23 << 1),
	BIT25 = (BIT24 << 1),
	BIT26 = (BIT25 << 1),
	BIT27 = (BIT26 << 1),
	BIT28 = (BIT27 << 1),
	BIT29 = (BIT28 << 1),
	BIT30 = (BIT29 << 1),
	BIT31 = (BIT30 << 1),
}Bit_t;

#endif /* STM32F429XX_GENERAL_H_ */

/**
 * @file stm32f429xx_uart_driver.h
 * @brief Prototype of peripheral driver API functions for the UART peripheral of STM32F429xx MCU
 * @note  Written and tested on STM32F429 Discovery board
 * @author Soumyadeep Bose
 */

#ifndef STM32F429XX_UART_DRIVER_H_
#define STM32F429XX_UART_DRIVER_H_

/**
 * @defgroup Includes
 * @{
 */

#include "stm32f429xx_general.h"

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
 * @defgroup Classes
 * @{
 */
typedef struct{
	uint8_t 	mode 			:1; 	///< UART mode: 0 - Receiver, 1 - Transmitter
	uint8_t 	stop_bits 		:2;		///< UART Number of stop bits: 0 - 1 bit, 1 - 0.5 bits, 2 - 2 stop bits, 3 - 1.5 stop bits
	uint32_t	baud_rate 		:17;	///< UART baud rate: min 2400, max 115200
	uint8_t 	word_length		:1;		///< UART Word length: 0 - 8-bit message, 1 - 9-bit message
	uint8_t		parity_enable	:1;		///< UART choice to enable parity
	uint8_t		parity_choice	:1;		///< UART choice of parity: 0 - even, 1 - odd
	uint8_t		oversampling	:1;		///< UART choice of oversampling: 0 - 16-bit, 1 - 8-bit
	uint8_t		hw_control		:1;		///< UART Hardware flow control: 0 - no hw control, 1 - CTS and RTS enabled
}UART_PinConfig;
typedef struct{
	uint8_t		periph_enable	:1;		///< UART peripheral enable interrupt: 0 - disable, 1 - enable
	uint8_t		transmit_enable	:1;		///< UART transmit enable interrupt: 0 - disable, 1 - enable
	uint8_t		transmit_complt	:1;		///< UART transmit complete interrupt: 0 - disable, 1 - enable
	uint8_t		receive_enable	:1;		///< UART receive enable interrupt: 0 - disable, 1 - enable
	uint8_t		idle_enable		:1;		///< UART IDLE detection enable interrupt: 0 - disable, 1 - enable
	uint8_t		hw_ctrl_enable	:1;		///< UART Hardware flow control interrupt: 0 - disable, 1 - enable
}UART_InterruptConfig;
typedef struct{
	USART_RegAddr_t *pUARTx;
	UART_PinConfig	UART_Pin_Config;
	UART_InterruptConfig UART_Interrupt_Config;
}UART_Handle;

/**
 * @}
 */
/**
 * @defgroup Functions
 * @{
 */

/**
 * @}
 */
#endif /* STM32F429XX_UART_DRIVER_H_ */

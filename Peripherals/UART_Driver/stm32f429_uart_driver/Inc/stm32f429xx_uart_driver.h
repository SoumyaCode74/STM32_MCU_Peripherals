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
#include "stm32f429xx_gpio_driver.h"
#include "stm32f429xx_rcc_driver.h"

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
typedef enum baud{
	BAUD_1200 	= 1200,
	BAUD_2400 	= 2400,
	BAUD_9600 	= 9600,
	BAUD_19200 	= 19200,
	BAUD_38400	= 38400,
	BAUD_57600	= 57600,
	BAUD_115200	= 115200,
	BAUD_230400 = 230400,
	BAUD_460800 = 460800,
	BAUD_921600	= 921600,
	BAUD_2M		= 2000000,
	BAUD_3M		= 3000000
}BaudRate_t;
typedef struct pinconfig{
	uint8_t 	mode 			:1; 	///< UART mode: 0 - Receiver, 1 - Transmitter
	uint8_t 	stop_bits 		:2;		///< UART Number of stop bits: 0 - 1 bit, 1 - 0.5 bits, 2 - 2 stop bits, 3 - 1.5 stop bits
	BaudRate_t	baud_rate 		:24;	///< UART baud rate: min 1200, max 3000000
	uint8_t 	word_length		:1;		///< UART Word length: 0 - 8-bit message, 1 - 9-bit message
	uint8_t		parity_enable	:1;		///< UART choice to enable parity
	uint8_t		parity_choice	:1;		///< UART choice of parity: 0 - even, 1 - odd
	uint8_t		oversampling	:1;		///< UART choice of oversampling: 0 - 16-bit, 1 - 8-bit
	uint8_t		hw_control		:1;		///< UART Hardware flow control: 0 - no hw control, 1 - CTS and RTS enabled
}UART_PinConfig_t;
typedef struct interrupts{
	uint8_t		periph_enable	:1;		///< UART peripheral enable interrupt: 0 - disable, 1 - enable
	uint8_t		transmit_enable	:1;		///< UART transmit enable interrupt: 0 - disable, 1 - enable
	uint8_t		transmit_complt	:1;		///< UART transmit complete interrupt: 0 - disable, 1 - enable
	uint8_t		receive_enable	:1;		///< UART receive enable interrupt: 0 - disable, 1 - enable
	uint8_t		idle_enable		:1;		///< UART IDLE detection enable interrupt: 0 - disable, 1 - enable
	uint8_t		hw_ctrl_enable	:1;		///< UART Hardware flow control interrupt: 0 - disable, 1 - enable
}UART_InterruptConfig_t;
typedef struct handle{
	USART_RegAddr_t *pUARTx;				///< For UART peripheral registers
	GPIO_Handle_t *hUARTx_GPIO;			///< For GPIOs to act for the UART peripheral
	RCC_Handle_t  *hUARTx_RCC;				///< For the clock access to bus and periph enable
	UART_PinConfig_t	UART_Pin_Config;			///< For UART communication specifics
	UART_InterruptConfig_t UART_Interrupt_Config;	///< For UART interrupt configs
}UART_Handle_t;
typedef enum flags{
	PARITY_ERROR = 0,
	FRAME_ERROR,
	NOISE_DETECTION,
	OVERRUN_ERROR,
	IDLE_DETECTION,
	RX_DATA_REG_NOT_EMPTY,
	TX_DATA_COMPLETE,
	TX_DATA_REG_EMPTY,
	LIN_BREAK_DETECTION,
	CTS_LINE_CHANGE
}UART_Flag_t;

/**
 * @}
 */
/******************************************************************************************
 *	@brief						APIs supported by this driver
 *	@note	 For more information about the APIs check the function definitions
 ******************************************************************************************
*/

/**
 * @defgroup Functions
 * @{
 */
/**
  * @brief  Return the clock frequency of PCLK
  * @param  Pointer to instance of class UART_Handle_t
  * @retval Clock frequency of PCLK
  */
uint32_t GetPCLKFrequency(UART_Handle_t * hUARTHandle);
/**
 * @brief 		Peripheral Clock setup
 * @param[in]	Pointer to UARTx peripheral register addresses
 * @param[in]	Enable or Disable selection
 * @retval		None
 */
void UART_PeriClockControl(USART_RegAddr_t *pUARTx, uint8_t EnorDi);
/**
 * @brief		Function to configure the baud-rate in USART_BRR register
 * @param[in]	Pointer to USART register addresses
 * @param[in]	Baudrate to set
 * @retval		None
 */
void USART_SetBaudRate(USART_RegAddr_t *pUSARTx, uint32_t BaudRate);
/**
 * @brief 		Function to initialize the UART peripheral
 * @param[in] 	Pointer to the UART handle instance
 * @retval 		None
 */
void UART_Init(UART_Handle_t * pUartHandle);
/**
 * @brief 		Function to de-initialize the UART peripheral
 * @param[in] 	Pointer to the UART peripheral register addresses
 * @retval 		None
 */
void UART_DeInit(USART_RegAddr_t *pUARTx);
/**
 * @brief Data Send and Receive APIs with blocking (polling mode)
 */
void UART_SendData(USART_RegAddr_t *pUARTx,uint8_t *pTxBuffer, uint32_t Len);
void UART_ReceiveData(USART_RegAddr_t *pUARTx, uint8_t *pRxBuffer, uint32_t Len);
/**
 * @brief Data Send and Receive APIs with non-blocking (interrupt mode)
 */

uint8_t UART_SendDataIT(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len);
uint8_t UART_ReceiveDataIT(UART_Handle_t *pUARTHandle, uint8_t *pRxBuffer, uint32_t Len);

/**
 * @brief IRQ Configuration and ISR handling functions
 */
void UART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void UART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void UART_IRQHandling(UART_Handle_t *pHandle);

/**
 * @brief Other Peripheral Control APIs
 */
void UART_PeripheralControl(USART_RegAddr_t *pUARTx, uint8_t EnOrDi);
uint8_t	UART_GetFlagStatus(USART_RegAddr_t * pUART, UART_Flag_t flag);
void UART_ClearFlag(USART_RegAddr_t *pUARTx, uint16_t StatusFlagName);

/**
 * @brief Application callback
 */
void UART_ApplicationEventCallback(UART_Handle_t *pUARTHandle,uint8_t AppEv);
/**
 * @}
 */
#endif /* STM32F429XX_UART_DRIVER_H_ */

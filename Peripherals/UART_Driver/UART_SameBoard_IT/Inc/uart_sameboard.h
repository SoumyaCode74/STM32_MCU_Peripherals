/**
 * @file 	uart_sameboard.h
 * @brief	Header containing the handle for the UART module, interrupt callback function declarations
 * @author 	Soumyadeep Bose
 */

#ifndef UART_SAMEBOARD_H_
#define UART_SAMEBOARD_H_

/**
 * @defgroup Defines
 * @{
 */
#ifndef SystemClock
#define	SystemClock		(16000000U)
#endif

/**
 * @}
 */

/**
 * @defgroup Include
 * @{
 */
#include <stdint.h>
#include "stm32f429xx.h"

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
}PeriphBus;

typedef enum{
	PORTA,
	PORTB,
	PORTC,
	PORTD,
	PORTE,
	PORTF,
	PORTG
}GPIO_UART_Port;

typedef enum{
	OVR16,/**< OVR16 */
	OVR8  /**< OVR8 */
}Oversampling;

typedef enum{
	EIGHT_BIT,
	NINE_BIT
}Word_Length;

typedef enum{
	FALSE,
	TRUE
}Bool;

typedef enum baud{
	BAUD_1200 	= 1200U,
	BAUD_2400 	= 2400U,
	BAUD_9600 	= 9600U,
	BAUD_19200 	= 19200U,
	BAUD_38400	= 38400U,
	BAUD_57600	= 57600U,
	BAUD_115200	= 115200U,
	BAUD_230400 = 230400U,
	BAUD_460800 = 460800U,
	BAUD_921600	= 921600U,
	BAUD_2M		= 2000000U,
	BAUD_3M		= 3000000U
}BaudRate_t;

typedef enum{
	EVEN_PARITY,
	ODD_PARITY
}Parity;

typedef struct{
	Oversampling  oversampling       : 1;
	uint8_t  enable				: 1;
	Word_Length	 word_length        : 1;
	Bool  parity_enable      : 1;
	Parity  parity_setting     : 1;
	Bool	 interrupt_enable	: 1;
	uint8_t  interrupt_number;
	Bool  receiver_interrupt : 1;
	Bool  transmit_interrupt : 1;
	Bool	 to_transmit        : 1;
	Bool  to_receive		    : 1;
	BaudRate_t baud_rate;
	uint8_t * pTxBuffer;
	uint8_t * pRxBuffer;
	uint32_t TxLength;
	uint32_t TxIndex;
	uint32_t RxLength;
	uint32_t RxIndex;
	USART_TypeDef * pUART;
	GPIO_TypeDef * pGPIO;
	RCC_TypeDef * pRCC;
	PeriphBus	GPIO_bus;
	GPIO_UART_Port GPIO_port;
	PeriphBus	UART_bus;
	uint8_t Clk_pin;
	uint8_t	RX_pin;
	uint8_t TX_pin;
}UARTHandle_t;

/**
 * @}
 */

/**
 * @defgroup Functions
 * @{
 */
/**
 * @brief Function to enable clock access to the GPIO port for the UART
 * @param hUART
 */
void UART_GPIO_ClockEnable(UARTHandle_t * hUART);

/**
 * @brief Function to configure the GPIO pins for the UART
 * @param hUART
 */
void UART_GPIOConfig(UARTHandle_t * hUART);

/**
 * @brief Function to enable clock access to UART
 * @param hUART
 */

void UART_Periph_ClockEnable(UARTHandle_t * hUART);

/**
 * @brief Function to initialize the UART peripheral
 * @param hUART
 */
void UART_Init(UARTHandle_t * hUART);

/**
 * @brief Function to de-initialize the UART
 * @param hUART
 */
void UART_DeInit(UARTHandle_t * hUART);

/**
 * @brief Function to transmit data from the UART pointed by hUART
 * @param hUART
 * @param pTX
 * @param length
 */
void UART_SendData(UARTHandle_t * hUART, uint8_t * pTX, uint32_t length);
/**
 * @brief Function to receive data to the UART pointed by hUART
 * @param hUART
 * @param pRX
 * @param index
 */
void UART_ReceiveData(UARTHandle_t * hUART, uint8_t * pRX, uint32_t index);
/**
 * @brief Function to calculate the value for the USARTx_BRR register for Baud Rate
 * @param hUART
 * @return Value to set to the USARTx_BRR register
 */
uint16_t UART_BaudRate(UARTHandle_t * hUART);
/**
 * @brief Function to complete reception process after RXNE interrupt occurs
 * @param hUART
 */
void UART_RxCmpltCallBack(UARTHandle_t * hUART);
/**
 * @brief Handler function to be called from the UARTx_Handler functions
 * @param CallBack
 */
void UART_RxHandler(void (*CallBack)(UARTHandle_t *), UARTHandle_t * hUART);

/**
 * @}
 */

#endif /* UART_SAMEBOARD_H_ */

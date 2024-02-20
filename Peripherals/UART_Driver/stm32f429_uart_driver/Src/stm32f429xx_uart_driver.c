/**
 * @file 	stm32f429xx_uart_driver.c
 * @brief 	Driver source code for UART peripheral driver of STM32F429xx MCU
 * @note 	Written and tested on STM32F429 Discovery board
 * @author	Soumyadeep Bose
 */

#include "stm32f429xx_uart_driver.h"

uint32_t UART_GetPCLKFrequency(UART_Handle_t * hUARTHandle){
	/*!< First find the clock source */
	/*!< Second, find the prescaler setting */
	/*!< Third, calculate and return the PCLK frequency */
	/*!< This function is not yet developed for PLL */
	uint8_t clock_source =  hUARTHandle->hUARTx_RCC->RCC_Config.clock_source;
	uint16_t prescaler	 = RCC_GetPrescaler(hUARTHandle->hUARTx_RCC);
	if(clock_source == HSI || clock_source == PLL){
			return HSI_VALUE/prescaler;
	}
	else{
		return HSE_VALUE/prescaler;
	}
}

void USART_SetBaudRate(USART_RegAddr_t *pUSARTx, uint32_t BaudRate){
	/*!< Obtain the peripheral clock frequency, pre-scaled */
	/*!< Check for oversampling, 8 times or 16 times */
	/*!< Calculate the Mantissa part */
	/*!< Place the Mantissa part in USART_BRR */
	/*!< Extract the fraction part */
	/*!< Calculate the final fraction value */
	/*!< Place the fraction value in USART_BRR */

}

void UART_InterruptConfig(USART_RegAddr_t * pUART, UART_InterruptConfig_t interruptconfig){
	;
}

uint8_t	UART_GetFlagStatus(USART_RegAddr_t * pUART, UART_Flag_t flag){
	return 0;
}

void UART_Init(UART_Handle_t * pUartHandle){
	;
}

void UART_Transmit(USART_RegAddr_t * pUART, uint8_t * pData, uint32_t length){
	;
}
void UART_Receive(USART_RegAddr_t * pUART, uint8_t * pData){
	;
}
void USART1_IRQHandler(void){
	;
}
void USART2_IRQHandler(void){
	;
}
void USART3_IRQHandler(void){
	;
}
void UART4_IRQHandler(void){
	;
}
void UART5_IRQHandler(void){
	;
}
void USART6_IRQHandler(void){
	;
}

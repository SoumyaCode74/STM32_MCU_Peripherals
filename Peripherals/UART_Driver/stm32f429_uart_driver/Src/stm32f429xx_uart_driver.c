/**
 * @file 	stm32f429xx_uart_driver.c
 * @brief 	Driver source code for UART peripheral driver of STM32F429xx MCU
 * @note 	Written and tested on STM32F429 Discovery board
 * @author	Soumyadeep Bose
 */

#include "stm32f429xx_uart_driver.h"

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

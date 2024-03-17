/**
 * @file   uart_sameboard.c
 * @brief  Source file for UART functions for the same board project
 * @author Soumyadeep Bose
 */

#include "uart_sameboard.h"

extern UARTHandle_t hUART1;
extern UARTHandle_t hUART2;

void UART_GPIO_ClockEnable(UARTHandle_t * hUART){
	switch(hUART->GPIO_bus){
	case AHB1:
		hUART->pRCC->AHB1ENR |= (0x1 << hUART->GPIO_port);
		break;
	case AHB2:
		hUART->pRCC->AHB2ENR |= (0x1 << hUART->GPIO_port);
		break;
	case AHB3:
		hUART->pRCC->AHB3ENR |= (0x1 << hUART->GPIO_port);
		break;
	case APB1:
		hUART->pRCC->APB1ENR |= (0x1 << hUART->GPIO_port);
		break;
	case APB2:
		hUART->pRCC->APB2ENR |= (0x1 << hUART->GPIO_port);
		break;
	}
}

void UART_GPIOConfig(UARTHandle_t * hUART){
	hUART->pGPIO->MODER &= ~(0x3 << (hUART->TX_pin * 2));
	hUART->pGPIO->MODER |= (0x2 << (hUART->TX_pin * 2));
	hUART->pGPIO->MODER &= ~(0x3 << (hUART->RX_pin * 2));
	hUART->pGPIO->MODER |= (0x2 << (hUART->RX_pin * 2));
	if(hUART->pUART == USART1 || hUART->pUART == USART2 || hUART->pUART == USART3){
		hUART->pGPIO->AFR[hUART->TX_pin / 8] &= ~(0xF << (hUART->TX_pin % 8) * 4);
		hUART->pGPIO->AFR[hUART->TX_pin / 8] |= (0x7 << (hUART->TX_pin % 8) * 4);
		hUART->pGPIO->AFR[hUART->RX_pin / 8] &= ~(0xF << (hUART->RX_pin % 8) * 4);
		hUART->pGPIO->AFR[hUART->RX_pin / 8] |= (0x7 << (hUART->RX_pin % 8) * 4);

	}
	else{
		hUART->pGPIO->AFR[hUART->TX_pin / 8] &= ~(0xF << (hUART->TX_pin % 8) * 4);
		hUART->pGPIO->AFR[hUART->TX_pin / 8] |= (0x8 << (hUART->TX_pin % 8) * 4);
		hUART->pGPIO->AFR[hUART->RX_pin / 8] &= ~(0xF << (hUART->RX_pin % 8) * 4);
		hUART->pGPIO->AFR[hUART->RX_pin / 8] |= (0x8 << (hUART->RX_pin % 8) * 4);
	}

}

void UART_Periph_ClockEnable(UARTHandle_t * hUART){
	switch(hUART->UART_bus){
	case AHB1:
		hUART->pRCC->AHB1ENR |= (0x1 << hUART->Clk_pin);
		break;
	case AHB2:
		hUART->pRCC->AHB2ENR |= (0x1 << hUART->Clk_pin);
		break;
	case AHB3:
		hUART->pRCC->AHB3ENR |= (0x1 << hUART->Clk_pin);
		break;
	case APB1:
		hUART->pRCC->APB1ENR |= (0x1 << hUART->Clk_pin);
		break;
	case APB2:
		hUART->pRCC->APB2ENR |= (0x1 << hUART->Clk_pin);
		break;
	}
}

void UART_Init(UARTHandle_t * hUART){
	/** Reset the UART peripheral */
	UART_DeInit(hUART);
	/** Configure clock of GPIO */
	UART_GPIO_ClockEnable(hUART);
	/** Configure GPIO pins */
	UART_GPIOConfig(hUART);
	/** Configure clock for UART */
	UART_Periph_ClockEnable(hUART);
	/** Configure oversampling */
	hUART->pUART->CR1 |= (hUART->oversampling << 15);
	/** Configure data frame format */
	hUART->pUART->CR1 |= (hUART->word_length << 12);
	/** Configure parity enable */
	hUART->pUART->CR1 |= (hUART->parity_enable << 10);
	/** Configure parity setting */
	hUART->pUART->CR1 |= (hUART->parity_setting << 9);
	/** Configure interrupt enable */
	if (hUART->interrupt_enable){
		NVIC->ISER[hUART->interrupt_number/32] |= (0x1 << (hUART->interrupt_number % 32));
		/** Configure transmit interrupt enable */
		hUART->pUART->CR1 |= (hUART->transmit_interrupt << 7);
		/** Configure receive interrupt enable */
		hUART->pUART->CR1 |= (hUART->receiver_interrupt << 5);
	}
	/** Configure baud rate */
	hUART->pUART->BRR = ((SystemClock+(hUART->baud_rate/2U))/hUART->baud_rate);
//	hUART->pUART->BRR = UART_BaudRate(hUART);
	/** Configure transmit enable */
	hUART->pUART->CR1 |= (hUART->to_transmit << 3);
	/** Configure receive enable */
	hUART->pUART->CR1 |= (hUART->to_receive << 2);
	/** Enable/Disable peripheral */
	hUART->pUART->CR1 |= (hUART->enable << 13);
}

void UART_DeInit(UARTHandle_t * hUART){
	/** Reset control registers */
	hUART->pUART->CR1 = 0;
	hUART->pUART->CR2 = 0;
	hUART->pUART->CR3 = 0;
	/** Reset baud rate register */
	hUART->pUART->BRR = 0;
	/** Clear data register */
	hUART->pUART->DR = 0;
}

uint16_t UART_BaudRate(UARTHandle_t * hUART){
	/** Enable floating point unit only during the floating point baud rate calculation */
	SCB->CPACR |= (0xF << 20);
	float USARTDIV = ((float)SystemClock)/((float)(8 * (2 - hUART->oversampling) * hUART->baud_rate));
	/** Obtain the mantissa */
	uint16_t mantissa = ((uint16_t)(USARTDIV));
	/** Obtain the fraction */
	float remainder = (USARTDIV - mantissa)*(8 * (2 - hUART->oversampling));
	uint16_t fraction = (uint8_t)(remainder) + 1;
	mantissa += (fraction / 16);
	fraction %= 16;
	uint16_t baud_rate_regval = (mantissa << 4) | fraction;
	/** Disable floating point unit */
	SCB->CPACR &= ~(0xF << 20);
	/** Return the value to write to the USARTx_BRR */
	return baud_rate_regval;
}

void UART_RxCmpltCallBack(UARTHandle_t * hUART){
	hUART->pRxBuffer[hUART->RxIndex] = hUART->pUART->DR;
	while(!((hUART->pUART->SR >> 7) & 0x1));
	hUART->pUART->DR = (uint8_t)(hUART->pRxBuffer[hUART->RxIndex]);
	while(!((hUART->pUART->SR >> 7) & 0x1));
	hUART->pUART->DR = '\r';
	while(!((hUART->pUART->SR >> 7) & 0x1));
	hUART->pUART->DR = '\n';
	if (hUART == &hUART1){
		while(!((hUART2.pUART->SR >> 7) & 0x1));
		hUART2.pUART->DR = (uint8_t)(hUART->pRxBuffer[hUART->RxIndex]);
	}
	else{
		while(!((hUART1.pUART->SR >> 7) & 0x1));
		hUART1.pUART->DR = (uint8_t)(hUART->pRxBuffer[hUART->RxIndex]);
	}
	while(!((hUART->pUART->SR >> 6) & 0x1));
}

void UART_RxHandler(void (*CallBack)(UARTHandle_t *), UARTHandle_t * hUART){
	CallBack(hUART);
	NVIC->ICPR[hUART->interrupt_number/32] |= (0x1 << (hUART->interrupt_number % 32));
}

void USART1_IRQHandler(void){
	UART_RxHandler(UART_RxCmpltCallBack, &hUART1);
}

void USART2_IRQHandler(void){
	UART_RxHandler(UART_RxCmpltCallBack, &hUART2);
}


/*
 * usart.h for stm32f401xe MCUs
 *
 *  Created on: Nov 7, 2022
 *      Author: soumy
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include "General.h"

#define USART1					((USART_TypeDef *)0x40011000UL)
#define USART2					((USART_TypeDef *)0x40004400UL)
#define USART6					((USART_TypeDef *)0x40011400UL)

#define GPIOAEN   				(1U<<0)
#define USART2EN  				(1U<<17)
#define CR1_TE					(1U<<3)
#define CR1_RE					(1U<<2)
#define CR1_UE					(1U<<13)
#define SR_TXE					(1U<<7)
#define SR_RXNE					(1U<<5)
#define BAUD_RATE_SET(x, y)		((x+(y/2U))/y)

typedef struct{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
}USART_TypeDef;

void USART2_init(uint32_t PeriphClk, uint32_t BaudRate);
void USART2_write(int);
void delayMs(int delay);
int __io_putchar(int);
void uart_set_baudrate(USART_TypeDef *, uint32_t, uint32_t);
uint32_t calculate_bd(uint32_t, uint32_t);
uint8_t USART2_read(void);
void USART2_rx_interrupt_init(uint32_t, uint32_t);

#endif

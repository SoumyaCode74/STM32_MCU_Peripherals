/*
 * uart.c
 *
 *  Created on: 09-Nov-2022
 *      Author: soumy
 */
#include "usart.h"

extern void USART1_init(uint32_t PeriphClk, uint32_t BaudRate) {

	/*************Configure gpio pin for usart operation***************/
	RCC->AHB1ENR  |= GPIOAEN;						//Enable clock for PORTA
	/***Configure PA2 and PA3 for alternate functions (AF07)**********/
	GPIOA->MODER  &= ~(0x0F<<9*2); 					//Reset the required positions
	GPIOA->MODER  |= (0x0A<<9*2); 					//Configure direction for alternate function
	GPIOA->AFR[9/8] &= ~(0xFF<<((9%8)*4)); 					//Reset the required positions
	GPIOA->AFR[9/8] |= (0x77<<((9%8)*4)); 					//Configure for AF07 for both PA2 and PA3
	/*************Configure usart module for transmission**************/
	RCC->APB2ENR  |= USART1EN; 						//Enable clock access to USART2 peripheral
	uart_set_baudrate(USART1, PeriphClk, BaudRate); //Configure USART2 baud rate
	USART1->CR1    = CR1_TE; 						//Activate transmitter
	USART1->CR1   |= CR1_RE;						//Activate receiver
	USART1->CR1   |= CR1_UE;						//Enable the USART module
	/*
	RCC->APB1ENR |= 0x20000;
    RCC->AHB1ENR |= 1;
    GPIOA->AFR[0] |=  0x0700;
    GPIOA->MODER  |=  0x0020;   //Set PA2 to alternate function


    USART2->BRR = 0x0683;       // 9600  @ 16 MHz
    USART2->CR1 = 0x0008;       // enable Tx
    USART2->CR1 |= 0x2000;      // enable USART2
    */
}
void USART1_rx_interrupt_init(uint32_t PeriphClk, uint32_t Baud_rate)
{
	/*
	 * Disable global interrupts
	 * Enable clock to USART2
	 * Set up USART2 for RX and TX with interrupt for RX
	 * Activate the RXNIE interrupt bit in USART2_CR register
	 * Identify IRQ number from NVIQ table
	 * Enable interrupt from respective ISER register of NVIQ file
	 * Enable global interrupts
	 */
	__asm volatile("CPSID I"); //Disable global interrupts
//	RCC->APB2ENR |= (1U << 4);
	USART1_init(PeriphClk, Baud_rate);
	USART1->CR1 |= (1U << 5);
	NVIC->ISER[1] |= (1U << 5); //IRQ37 for USART1
	__asm volatile("CPSIE I"); //Enable global interrupts
}
void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
	USARTx->BRR = BAUD_RATE_SET(PeriphClk, BaudRate);
}

extern void USART1_write (int ch) {
     // wait until Tx buffer empty
    while (!(USART1->SR & 0x0080)) {}
    USART1->DR = (ch & 0xFF);
}
int __io_putchar(int ch){
	USART1_write(ch);
	return ch;
}

extern void delayMs(int delay) {
    int i;
    for (; delay > 0; delay--)
        for (i = 0; i < 3195; i++) ;
}

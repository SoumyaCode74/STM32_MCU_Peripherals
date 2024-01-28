/*
 * uart.c
 *
 *  Created on: 09-Nov-2022
 *      Author: soumy
 */
#include "stm32f4xx.h"
#include "usart.h"


extern void USART2_init(uint32_t PeriphClk, uint32_t BaudRate) {

	/*************Configure gpio pin for usart operation***************/
	RCC->AHB1ENR  |= GPIOAEN;						//Enable clock for PORTA
	/***Configure PA2 and PA3 for alternate functions (AF07)**********/
	GPIOA->MODER  &= ~(0x0F<<4); 					//Reset the required positions
	GPIOA->MODER  |= (0x0A<<4); 					//Configure direction for alternate function
	GPIOA->AFR[0] &= ~(0xFF<<8); 					//Reset the required positions
	GPIOA->AFR[0] |= (0x77<<8); 					//Configure for AF07 for both PA2 and PA3
	/*************Configure usart module for transmission**************/
	RCC->APB1ENR  |= USART2EN; 						//Enable clock access to USART2 peripheral
	uart_set_baudrate(USART2, PeriphClk, BaudRate); //Configure USART2 baud rate
	USART2->CR1    = CR1_TE; 						//Activate transmitter
	USART2->CR1   |= CR1_RE;						//Activate receiver
	USART2->CR1   |= CR1_UE;						//Enable the USART module
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

void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
	USARTx->BRR = BAUD_RATE_SET(PeriphClk, BaudRate);
}

extern void USART2_write (int ch) {
     // wait until Tx buffer empty
    while (!(USART2->SR & 0x0080)) {}
    USART2->DR = (ch & 0xFF);
}
int __io_putchar(int ch){
	USART2_write(ch);
	return ch;
}

unsigned char USART2_read(void) {
    while (!(USART2->SR & 0x0020)) {}   // wait until character arrives
    return USART2->DR;
}

extern void delayMs(int delay) {
    int i;
    for (; delay > 0; delay--)
        for (i = 0; i < 3195; i++) ;
}

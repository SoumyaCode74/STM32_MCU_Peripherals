/*
 * usart.h for stm32f401xe MCUs
 *
 *  Created on: Nov 7, 2022
 *      Author: soumy
 */

#ifndef USART_H_
#define USART_H_
#define GPIOAEN   				(1U<<0)
#define USART2EN  				(1U<<17)
#define CR1_TE					(1U<<3)
#define CR1_RE					(1U<<2)
#define CR1_UE					(1U<<13)
#define SR_TXE					(1U<<7)
#define SR_RXNE					(1U<<5)
#define BAUD_RATE_SET(x, y)		((x+(y/2U))/y)

extern void USART2_init(uint32_t PeriphClk, uint32_t BaudRate);
extern void USART2_write(int);
extern void delayMs(int delay);
int __io_putchar(int);
void uart_set_baudrate(USART_TypeDef *, uint32_t, uint32_t);
uint32_t calculate_bd(uint32_t, uint32_t);
unsigned char USART2_read(void);

/*void uart_set_baudrate(USART_TypeDef *, uint32_t, uint32_t);
void uart2_write(uint8_t);
void uart2_write_word(char *);
uint32_t uart2_read(void);


void uart2_rxtx_init(uint32_t PeriphClk, uint32_t BaudRate)
{
	//Configure gpio pin for usart operation
	RCC->AHB1ENR  |= GPIOAEN;						//Enable clock for PORTA
	//Configure PA2 and PA3 for alternate functions (AF07)
	GPIOA->MODER  &= ~(0x0F<<4); 					//Reset the required positions
	GPIOA->MODER  |= (0x0A<<4); 					//Configure direction for alternate function
	GPIOA->AFR[0] &= ~(0xFF<<8); 					//Reset the required positions
	GPIOA->AFR[0] |= (0x77<<8); 					//Configure for AF07 for both PA2 and PA3
	//Configure usart module for transmission
	RCC->APB1ENR  |= USART2EN; 						//Enable clock access to USART2 peripheral
	uart_set_baudrate(USART2, PeriphClk, BaudRate); //Configure USART2 baud rate
	USART2->CR1    = CR1_TE; 						//Activate transmitter
	USART2->CR1   |= CR1_RE;						//Activate receiver
	USART2->CR1   |= CR1_UE;						//Enable the USART module
}

int __io_putchar(unsigned int ch)
{
	uart2_write(ch);
	return ch;
}

void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
	USARTx->BRR = BAUD_RATE_SET(PeriphClk, BaudRate);
}

uint32_t uart2_read(void)
{
	while((USART2->SR & SR_RXNE) == 0); //Wait for received data

	return USART2->DR; //Return the received data
}

void uart2_write(uint8_t ch)
{
	//Wait until the transmit data register is empty
	while((USART2->SR & SR_TXE) == 0);
	//Write to transmit data register
	USART2->DR = (ch & 0xFF);
}

void uart2_write_word(char *word)
{
	volatile char letter;
	for(uint32_t volatile i = 0; i < sizeof(word); i++)
	{
		//Wait until the transmit data register is empty
		while((USART2->SR & SR_TXE) == 0);
		//Write to transmit data register
		letter = word[i];
		USART2->DR = (letter & 0xFF);
	}
}*/

#endif

#include "General.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

/*Tested with ARM Compiler : version 5*/

/* Function to be called upon USART2 Interrupt handling */
static void USART2_RX_Callback(void);
/* Function to toggle an external LED for counts 
 * equal to the ASCII equivalent of the character received from the PC over UART
 */ 
void LED_play(unsigned int);

volatile unsigned int times = 0; //Variable to record number of times the main thread is interrupted through UART 
volatile char c; //To store ASCII equivalent of the key pressed and received over UART

int main(void){
	/* Initialize USART2 module for UART operation with receive interrupt enabled */
	/* Clock used: HSI (16 MHz); Baud Rate: 115.2 Kbps */
	USART2_rx_interrupt_init(HSICLK, 115200UL); 
	/* Initialize System Tick timer for accurate time delays */
	systick_init();
	RCC->AHB1ENR |= (1U << 0);   //Enable clock access to GPIOA
	GPIOA->MODER &= ~(3U << 2);  //Reset direction of PIN1
	GPIOA->MODER &= ~(3U << 10); //Reset direction of PIN5
	GPIOA->MODER |= (1U << 2);   //Set direction of PIN1 as output
	GPIOA->MODER |= (1U << 10);  //Set direction of PIN5 as output

	while(1)
	{
		printf("Awaiting interrupt and toggling LED. Now: %ld\r\n",((GPIOA->ODR >> 1) & 1U));
		GPIOA->ODR ^= (1U << 1);
		delay_time_ms(250, HSICLK);
	}
}

void USART2_IRQHandler(void){
	USART2_RX_Callback();
}

static void USART2_RX_Callback(void)
{
	++times;
	c = USART2->DR; //Read from data register and reset the RXNIE bit
	printf("Hello, you pressed key %c!\n\r",c);
	printf("Times interrupted: %d\n\r", times);
	LED_play(c);
}

void LED_play(unsigned int value) {
    for (; value > 0; value--) {
        GPIOA->ODR ^= (1U << 5);   // toggle LED
        delay_time_ms(100, HSICLK);
    }
}

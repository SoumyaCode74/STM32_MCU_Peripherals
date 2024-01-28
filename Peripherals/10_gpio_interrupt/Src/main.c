#include "General.h"
#include "systick.h"
#include "usart.h"
#include "exti.h"
#include <stdio.h>

void EXTI15_10_IRQHandler(void);
static void EXTI13_Callback(void);

volatile int times = 0;
int main(void){
	pc13_exti_init();            //Initialize PC13 as external interrupt
	USART2_init(HSICLK, 115200UL); //Initialize USART2 with HSI, and Baud rate 9600 bits per second

	RCC->AHB1ENR |= (1U << 0);
	GPIOA->MODER &= ~(3U << 2);
	GPIOA->MODER &= ~(3U << 10);
	GPIOA->MODER |= (1U << 2);
	GPIOA->MODER |= (1U << 10);

	while(1)
	{
//		printf("Awaiting interrupt and toggling LED. Now: %ld\r\n",((GPIOA->ODR >> 1) & 1U));
		GPIOA->ODR ^= (1U << 1);
		delay_time_sec(1, HSICLK);
	}
}

/*
 * Obtain the interrupt handler name from the startup file
 * Define the interrupt handler and the interrupt callback functions
 */

void EXTI15_10_IRQHandler(void) //Execute ISR when IRQ40 happens
{
	if(EXTI->PR >> 13)
	{
		EXTI->PR |= (1U << 13); //Clear pending interrupt at PC13
		EXTI13_Callback();
	}

}

static void EXTI13_Callback(void)
{
	++times;
	printf("Button pressed! Times: %d\n\r", times);
	GPIOA->ODR |= (1U << 5);
	delay_time_ms(750, HSICLK);
	GPIOA->ODR &= ~(1U << 5);
}

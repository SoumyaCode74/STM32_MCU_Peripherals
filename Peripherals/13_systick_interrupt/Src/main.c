#include "General.h"
#include "systick.h"
#include "usart.h"
#include "stm32f4xx.h"
#include <stdio.h>
#define   	DELAY_MS		(500U)
#define		DELAY_S			(2U)

uint32_t delay_ms = 2000;
uint32_t time_passed = 0;
int main(void){

	USART2_init(HSICLK, BD_9600);
	systick_init_interrupt_ms(HSICLK, delay_ms);

	RCC->AHB1ENR |= 1U;
	GPIOA->MODER &= ~((3U << 2) | (3U << 10));
	GPIOA->MODER |= (1U << 2) | (1U << 10);

	while(1)
	{
		GPIOA->ODR ^= (1U << 1);
		for(uint32_t volatile i = 1; i <= 200UL; i++);
	}
}

static void Systick_callback(void)
{
	time_passed += delay_ms;
	printf("%ld milliseconds passed!\n\r", time_passed);
	GPIOA->ODR ^= (1U << 5);

}

void SysTick_Handler(void)
{
	//Do something
	Systick_callback();
}

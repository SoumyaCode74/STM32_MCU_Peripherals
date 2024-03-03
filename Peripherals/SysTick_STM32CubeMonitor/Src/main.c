#include "General.h"
#include "systick.h"
//#include "stm32f4xx.h"
//#include <stdio.h>
#define   	DELAY_MS		(500U)
#define		DELAY_S			(2U)

uint32_t delay_ms = 100;
uint32_t time_passed = 0;
uint32_t count_val = 0;
int main(void){

	systick_init_interrupt_ms();

	RCC->AHB1ENR |= 1U;
	GPIOA->MODER &= ~(0x3 << 0);
	GPIOA->MODER |= (1U << 0);
	GPIOA->ODR ^= (1U << 0);
	delay_time_ms_IT(delay_ms, HSICLK);
	while(1)
	{
//		GPIOA->ODR ^= (1U << 1);
		count_val = SYSTICK->STCVR;
	}
}

static void Systick_callback(void)
{
	time_passed += delay_ms;
	GPIOA->ODR ^= (1U << 0);
//	delay_time_ms_IT(500, CLK_SOURCE);
}

void SysTick_Handler(void)
{
	//Do something
	Systick_callback();
}

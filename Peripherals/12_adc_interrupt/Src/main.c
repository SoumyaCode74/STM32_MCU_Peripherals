#include "General.h"
#include "systick.h"
#include "usart.h"
#include "adc.h"
#include <stdio.h>

void ADC_IRQHandler(void);
static void ADC_Callback(void);

int main(void){

	USART2_init(HSICLK, BD_9600);
	printf("UART initiated");
	pa0_ADC1_interrupt_init();
	start_conversion_cont();
	RCC->AHB1ENR |=  (1U << 0);   //Enable clock access to GPIOA
	GPIOA->MODER &= ~((3U << 2) | (3U << 10));  //Reset direction of PIN1 and PIN5
	GPIOA->MODER |= (1U << 2) | (1U << 10);   //Set direction of PIN1 and PIN5 as output

	while(1)
	{
		GPIOA->ODR ^= (1U << 1);
		for(uint32_t volatile i = 1; i <= 200000; i++);
	}
}

/*
 * Obtain the interrupt handler name from the startup file
 * Define the interrupt handler and the interrupt callback functions
 */
void ADC_IRQHandler(void)  //ISR for ADC
{
	if(((ADC1->SR >> 1) & 0x1) == 1) //Check pending interrupt IRQ18 (ADC_interrupt)
	{
		ADC_Callback();
		ADC1->SR &= ~(1U << 1); //Clear pending ADC interrupt
	}
}

static void ADC_Callback(void)
{
	input_voltage(ADC1->DR, 5U, 12U);
//	printf("Value obtained: %ld\n\r",sensor_value);
	GPIOA->ODR |= (1U << 5);
	delay_time_ms(1000, HSICLK);
	GPIOA->ODR &= ~(1U << 5);
	delay_time_ms(1000, HSICLK);
}

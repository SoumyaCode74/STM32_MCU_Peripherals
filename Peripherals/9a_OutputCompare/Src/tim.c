/*
 * tim.c
 *
 *  Created on: 11-Nov-2022
 *      Author: soumy
 */

#include "../Inc/tim.h"
#include "../Inc/General.h"


void tim2_init(void)
{
	/*Enable clock access to TIM2
	*/
	RCC->APB1ENR |= TIM2EN;
}
void tim2_cont_ms(uint32_t wait_time)
{
	 /* Call the tim2_init for timer initialization
		  * Calculate the period value and load the period value to auto-reload register of TIM2
		  * Clear the current count value of TIM2
		  * Disable one pulse mode
		  * Enable TIM2
		  * Wait for counting to finish
		 */
		tim2_init();
		TIM2->PSC = PSC_MS - 1;	//With assumption that the HSI clock is acting as the system clock
		TIM2->ARR = (uint16_t)(wait_time*(HSICLK/(1000*(TIM2->PSC+1))) - 1); //Maximum 16-bit period allowed
		TIM2->CNT &= ~0xFFFF; //Clear counter value
		TIM2->CR1 &= ~(1U<<3);//Disable one pulse mode
		TIM2->CR1 |= CNTEN;	  //Enable counting

		while((TIM2->SR & CNTEND) == 0); //Wait for counting to finish
		TIM2->SR &= ~0x01;  //Resetting the update interrupt flag
}
void tim2_cont_us(uint32_t wait_time)
{
	tim2_init();
	TIM2->PSC = PSC_US - 1;
	TIM2->ARR = (uint16_t)(wait_time*((HSICLK/1000)/(1000*(TIM2->PSC+1))));
	TIM2->CNT &= ~0xFFFF;
	TIM2->CR1 |= CNTEN;
}

void tim2_pwm_pa5(uint32_t on_time, uint32_t period)
{
	/*
	 * Enable clock access to GPIOA port
	 * Set PA5 to alternating function mode
	 * Enable clock access to TIM2 module
	 * Set the pre-scaler value
	 * Set the on_time to TIM2_CCR
	 * Set the period to TIM2_ARR
	 * Set the TIM2 in output PWM mode
	 * Set TIM2 in up-counting mode
	 * Enable Channel 1 as output compare
	 * Clear count register
	 * Enable TIM2
	 * Wait for on-time event to occur. Reset interrupt flag
	 * Wait for period event to occur. Reset update interrupt flag
	 */
	RCC->AHB1ENR |= BIT0;
	GPIOA->MODER &= ~(BIT10 | BIT11);
	GPIOA->MODER |= BIT11;
	GPIOA->AFR[0] &= ~(BIT20 | BIT21 | BIT22 | BIT23);
	GPIOA->AFR[0] |= BIT20;

	tim2_init();
	TIM2->PSC  = PSC_MS;
	TIM2->CCR1 = (uint16_t)(on_time*(HSICLK/(1000*(TIM2->PSC+1))) - 1);
	TIM2->ARR = (uint16_t)(period*(HSICLK/(1000*(TIM2->PSC+1))) - 1);

	TIM2->CCMR1 &= ~BIT7; //Output compare clear enable not set
	TIM2->CCMR1 &= ~(BIT6 | BIT5 | BIT4);
	TIM2->CCMR1 |=  (BIT6 | BIT5);
	TIM2->CCMR1 &= ~(BIT3 | BIT2 | BIT1 | BIT0);

	TIM2->CCER |= BIT0;
	TIM2->CNT   = 0U;
	TIM2->CR1  |= BIT0;

	while((TIM2->SR & 0x02) == 0);
	TIM2->SR  &= ~BIT1;
	while((TIM2->SR & 0x01) == 0);
	TIM2->SR  &=  ~BIT0;
}
uint32_t tim3_input_capture_pb4(void)
{
	/*
	 * Define an unsigned int value for timestamp
	 * Enable clock access to GPIOB port
	 * Set direction of PB4 to alternate function
	 * Set the alternate function value for PB4
	 * Enable clock access to TIM3 timer
	 * Set pre-scaler value
	 * Configure TIM3 in input capture mode
	 * Apply filter, choose a desirable sampling frequency
	 * Set polarity to falling edge
	 * Enable the capture mode
	 * Enable TIM3
	 * Wait for the capture interrupt to set.
	 * Read the timestamp
	 * Reset the update interrupt
	 * return the timestamp
	 */
	uint32_t timestamp = 0;

	RCC->AHB1ENR 	|= BIT1;

	GPIOB->MODER 	&= ~(BIT8 | BIT9);
	GPIOB->MODER 	|= BIT9;
	GPIOB->AFR[0]	&= ~(BIT16 | BIT17 | BIT18 | BIT19);
	GPIOB->AFR[0]	|= BIT17;

	RCC->APB1ENR	|= TIM3EN;

	TIM3->PSC		= 16000 - 1;

	TIM3->CCMR1		|= BIT0;

	TIM3->CCMR1		|= (BIT4 | BIT5);

	TIM3->CCER		&= ~BIT3;
	TIM3->CCER		|= BIT1;

	TIM3->CCER		|= BIT0;

	TIM3->CR1		|= CNTEN;

	while((TIM3->SR & BIT1) == 0);

	timestamp		 = TIM3->CCR[0];

	if (timestamp > 65535)
		timestamp += 65535;

	TIM3->SR		&= ~BIT0;

	return timestamp;

}

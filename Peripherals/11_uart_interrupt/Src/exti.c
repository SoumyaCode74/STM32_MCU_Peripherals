/*
 * exti.c
 *
 *  Created on: 12-Nov-2022
 *      Author: soumy
 */

#include "exti.h"
#include "General.h"

void pc13_exti_init(void)
{
	/*Disable global interrupts
	 * Enable clock access to the GPIOC port
	 * Set direction of PC13 as input
	 * Enable clock access to SYSCFG peripheral
	 * Configure the corresponding EXTI bits of the corresponding SYSCFG_CR register for interrupt
	 * Unmask the interrupt
	 * Set rising edge or falling edge trigger
	 * Enable global interrupts
	 * Enable corresponding interrupt in the NVIC. Identify the IRQn from reference manual
	 * Set priority in NVIC (optional)
	 */
//	__disable_irq();
	__asm volatile("CPSID I");
	RCC->AHB1ENR 		|= (1U << 2);
	GPIOC->MODER 		&= ~(3U << 26);
	RCC->APB2ENR 		|= (1U << 14);
	SYSCFG->EXTICR[3] 	&= ~(15U << 4);
	SYSCFG->EXTICR[3] 	|=  (1U << 5);
	EXTI->IMR 			|= (1U << 13);
	EXTI->RTSR			|= (1U << 13);
//	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC->ISER[1]       |= (1U << 8);
//	__enable_irq();
	__asm volatile("CPSIE I");

}

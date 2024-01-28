/*
 * adc.c
 *
 *  Created on: Nov 8, 2022
 *      Author: soumy
 */

#include "adc.h"
#include "General.h"
#include "usart.h"
#include <math.h>
#include <stdio.h>

void pa0_ADC1_init(void)
{
	/***Configure the ADC GPIO pin***/
	/*Enable clock access to GPIO port*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set the direction of GPIO pin PA0 to analog*/
	GPIOA->MODER |= (3U<<0);
	/***Configure the ADC module***/
	/*Enable clock access to ADC module*/
	RCC->APB2ENR |= ADC1_EN;
	/*Configure ADC parameters*/
//	ADC1->SQR3  &= ~0x1F; 	 			//Clear the bits for 1st sequence
	ADC1->SQR[2]  &= ~ADC1_CH0;  			//Set channel 0 as 1st sequence conversion
	ADC1->CR1   &= ~(3U<<24);			//Set resolution to 12-bit
	ADC1->SMPR[1] &= ~(7U<<0);			//Set sampling rate to 3 cycles per sample for CH0
	ADC1->CR2	&= ~ADC1_ALIGN;			//Right shift converted result
	ADC1->CR2   |= ADC1_ON;				//Power on the ADC module

}

void pa1_ADC1_init(void)
{
	/***Configure the ADC GPIO pin***/
	/*Enable clock access to GPIO port*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set the direction of GPIO pin PA0 to analog*/
	GPIOA->MODER |= (3U<<2);
	/***Configure the ADC module***/
	/*Enable clock access to ADC module*/
	RCC->APB2ENR |= ADC1_EN;
	/*Configure ADC parameters*/
	ADC1->SQR[2]  &= ~0x1F; 	 			//Clear the bits for 1st sequence
	ADC1->SQR[2]  |= ADC1_CH1;  			//Set channel 0 as 1st sequence conversion
	ADC1->CR1   &= ~(3U<<24);			//Set resolution to 12-bit
	ADC1->SMPR[1] &= ~(7U<<3);			//Set sampling rate to 3 cycles per sample for CH0
	ADC1->CR2	&= ~ADC1_ALIGN;			//Right shift converted result
	ADC1->CR2   |= ADC1_ON;				//Power on the ADC module

}

void pa0_ADC1_interrupt_init(void)
{
	/*
	 * Disable global interrupts
	 * Enable clock access to ADC1
	 * Enable clock access to GPIOA
	 * Set direction of PA0 as analog input
	 * Set regular sequence 1 to ADC1_CH0
	 * Set resolution
	 * Set sampling rate
	 * Set right alignment
	 * Enable EOC interrupt bit
	 * Enable ADC1 interrupt in NVIC
	 * Power on ADC1
	 * Start conversion
	 * Enable global interrupts
	 */
//	__disable_irq();
	__asm volatile("CPSID I");
	RCC->APB2ENR |=  (1U << 8);
	RCC->AHB1ENR |=  (1U << 0);
	GPIOA->MODER |=  (3U << 0);
	ADC1->SQR[2]   &= ~(31U << 0);
	ADC1->CR1	 &= ~(3U << 24);
	ADC1->SMPR[1]	 &= ~(3U << 0);
	ADC1->CR2	 &= ~(1U << 11);
	ADC1->CR1	 |=  (1U << 5);
	NVIC->ISER[0]|=  (1U << 18);
	ADC1->CR2	 |=  (1U << 0);
//	__enable_irq();
	__asm volatile("CPSIE I");

}

void start_conversion_cont(void){
	//Enable continuous conversion
	ADC1->CR2 |= (1U << 1);
	//Check and start conversion
		while((ADC1->SR & (1U<<4)) == 0){
			ADC1->CR2 |= ADC1_START;
//			for(uint32_t volatile i = 1; i <= 1000; i++);
		}
}

void start_conversion_single(void){
	//Disable continuous conversion
	ADC1->CR2 &= ~(1U << 1);
	//Check and start conversion
		while((ADC1->SR & (1U<<4)) == 0){
			ADC1->CR2 |= ADC1_START;
//			for(uint32_t volatile i = 1; i <= 1000; i++);
		}
}

uint32_t perform_conversion(void)
{

	ADC1->SR &= ~(1U<<4); //Clear the start conversion status bit
	while((ADC1->SR & ADC1_EOC) == 0); //Wait for conversion to end
	return ADC1->DR; //Return the converted value
}

void input_voltage(uint32_t result, uint8_t vref, uint8_t resolution)
{
	USART2_init(HSICLK, BD_9600);
	uint32_t result_beforedecimal = (uint32_t)((5*result)/4096);
	uint32_t result_afterdecimal = (uint32_t)((5*result)%4096);
	if(result_afterdecimal < 1000)
	{
		printf("Input voltage: %ld.0%ld V\r\n", result_beforedecimal, result_afterdecimal);
	}
	else
	{
		printf("Input voltage: %ld.%ld V\r\n", result_beforedecimal, result_afterdecimal);
	}
	//delay_time_ms(200, HSICLK);//Wait for 200 ms betweeen each message
}

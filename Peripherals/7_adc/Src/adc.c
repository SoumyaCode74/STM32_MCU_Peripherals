/*
 * adc.c
 *
 *  Created on: Nov 8, 2022
 *      Author: soumy
 */

#include "stm32f4xx.h"
#include "adc.h"
#include "General.h"
#include <math.h>

void pa1_ADC1_init(void)
{
	/***Configure the ADC GPIO pin***/
	/*Enable clock access to GPIO port*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set the direction of GPIO pin PA1 to analog*/
	GPIOA->MODER |= (3U<<2);
	/***Configure the ADC module***/
	/*Enable clock access to ADC module*/
	RCC->APB2ENR |= ADC1_EN;
	/*Configure ADC parameters*/
	ADC1->SQR3  &= ~0x1F; 	 			//Clear the bits for 1st sequence
	ADC1->SQR3  |=  ADC1_CH1;  			//Set channel 1 as 1st sequence conversion
	ADC1->CR1   &= ~(3U<<24);			//Set resolution to 12-bit
	ADC1->SMPR2 &= ~(7U<<3);			//Set sampling rate to 3 cycles per sample
	ADC1->CR2	&= ~ADC1_ALIGN;			//Right shift converted result
	ADC1->CR2   |= ADC1_ON;				//Power on the ADC module

}

void start_conversion(void){
	//Check and start conversion
		while((ADC1->SR & (1U<<4)) == 0){
			ADC1->CR2 |= ADC1_START;
			for(uint32_t volatile i = 1; i <= 1000; i++);
		}
}

uint32_t perform_conversion(void)
{

	ADC1->SR &= ~(1U<<4); //Clear the start conversion status bit
	while((ADC1->SR & ADC1_EOC) == 0); //Wait for conversion to end
	return ADC1->DR; //Return the converted value
}

uint32_t input_voltage(uint32_t result, uint8_t vref, uint8_t resolution)
{
	return ((vref)/(pow(2, resolution)))*(result);
}

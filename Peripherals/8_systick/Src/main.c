#include "General.h"
#include "systick.h"
#include "stm32f4xx.h"

int main(void){
	RCC->AHB1ENR |= 0x01;
	GPIOA->MODER &= ~(0xFFF<<0);
	GPIOA->MODER |=  (0x555<<0);
	while(1)
	{
		GPIOA->ODR &= ~(0x1F);
		delay_time_ms(100, HSICLK); //Wait using System counter
		for(unsigned int volatile i = 0; i < 4; i++)
		{
			GPIOA->ODR |= (1U << i);
			if((GPIOA->ODR & 0x4) != 0) //Routing PA2 to PA4
			{
				GPIOA->ODR |= (1U << 4);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 4);
			}
			if((GPIOA->ODR & 0x8) != 0) //Routing PA3 to PA5
			{
				GPIOA->ODR |= (1U << 5);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 5);
			}
			delay_time_ms(250, HSICLK); //Wait using System counter

		}
		for(unsigned int volatile i = 0; i < 4; i++)
		{
			GPIOA->ODR &= ~(1U << (3-i));
			if((GPIOA->ODR & 0x4) != 0) //Routing PA2 to PA4
			{
				GPIOA->ODR |= (1U << 4);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 4);
			}
			if((GPIOA->ODR & 0x8) != 0) //Routing PA3 to PA5
			{
				GPIOA->ODR |= (1U << 5);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 5);
			}
			delay_time_ms(250, HSICLK); //Wait using System counter

		}
		GPIOA->ODR &= ~(0x1F);
		delay_time_sec(1, HSICLK); //Wait using System counter
		for(unsigned int volatile i = 0; i < 4; i++)
		{
			GPIOA->ODR |= (1U << i);
			if((GPIOA->ODR & 0x4) != 0) //Routing PA2 to PA4
			{
				GPIOA->ODR |= (1U << 4);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 4);
			}
			if((GPIOA->ODR & 0x8) != 0) //Routing PA3 to PA5
			{
				GPIOA->ODR |= (1U << 5);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 5);
			}
			delay_time_sec(1, HSICLK); //Wait using System counter

		}
		for(unsigned int volatile i = 0; i < 4; i++)
		{
			GPIOA->ODR &= ~(1U << (3-i));
			if((GPIOA->ODR & 0x4) != 0) //Routing PA2 to PA4
			{
				GPIOA->ODR |= (1U << 4);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 4);
			}
			if((GPIOA->ODR & 0x8) != 0) //Routing PA3 to PA5
			{
				GPIOA->ODR |= (1U << 5);
			}
			else
			{
				GPIOA->ODR &= ~(1U << 5);
			}
			delay_time_sec(1, HSICLK); //Wait using System counter

		}
//		GPIOA->ODR   &= ~(0xFF);
//		value = (GPIOA->ODR >> 3) ^ 0x1;
//		GPIOA->ODR |= value;
//		delay_time_ms(500, SYSCLK); //Wait using System counter
//		for(unsigned int volatile i = 0; i < 3; i++)
//		{
//			GPIOA->ODR <<= 1;
//			if((GPIOA->ODR & 0x4) != 0) //Routing PA2 to PA4
//			{
//				GPIOA->ODR |= (1U << 4);
//			}
//			else
//			{
//				GPIOA->ODR &= ~(1U << 4);
//			}
//			if((GPIOA->ODR & 0x8) != 0) //Routing PA3 to PA5
//			{
//				GPIOA->ODR |= (1U << 5);
//			}
//			else
//			{
//				GPIOA->ODR &= ~(1U << 5);
//			}
//			delay_time_ms(500, SYSCLK); //Wait using System counter
//			GPIOA->ODR |= 0x1;
//			delay_time_ms(500, SYSCLK); //Wait using System counter
//		}
//		GPIOA->ODR &= ~0x1;
//		for(unsigned int volatile i = 0; i < 3; i++)
//		{
//			GPIOA->ODR <<= 1;
//			if(GPIOA->ODR & 0x4) //Routing PA2 to PA4
//			{
//				GPIOA->ODR |= (1U << 4);
//			}
//			else
//			{
//				GPIOA->ODR &= ~(1U << 4);
//			}
//			if(GPIOA->ODR & 0x8) //Routing PA3 to PA5
//			{
//				GPIOA->ODR |= (1U << 5);
//			}
//			else
//			{
//				GPIOA->ODR &= ~(1U << 5);
//			}
//			delay_time_ms(500, SYSCLK); //Wait using System counter
//			GPIOA->ODR &= ~0x1;
//			delay_time_ms(500, SYSCLK); //Wait using System counter
//		}
//
//		/*if(GPIOA->ODR & 0x4)
//		{
//			GPIOA->ODR |= (1U << 4);
//		}
//		else
//		{
//			GPIOA->ODR &= ~(1U << 4);
//		}
//		delay_time_ms(500, SYSCLK); //Wait using System counter
//		GPIOA->ODR <<= 1;
//		value = (GPIOA->ODR >> 3) ^ 0x1;
//		GPIOA->ODR ^= value;  //Write to PA0, PA1, PA2, PA3, PA4
////		GPIOA->ODR &= ~(0x1F<<0);    //Reset outputs for next value
//		 *
//		 */
	}
}

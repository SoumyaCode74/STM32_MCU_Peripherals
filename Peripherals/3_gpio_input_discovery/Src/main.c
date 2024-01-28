#include "stm32f4xx.h"

#define GPIODEN 		(1U<<3)
#define GPIOAEN 		(1U<<0)

#define PIN12			(1U<<12)
#define PIN13			(1U<<13)
#define PIN14			(1U<<14)
#define PIN15			(1U<<15)
#define LED_1			PIN12
#define LED_2			PIN13
#define LED_3			PIN14
#define LED_4			PIN15

#define BTN_EN			(3U<<0)
#define BTN_PIN			(1U<<0)

#define DELAY_HIGH		(500000UL)

int main(void)
{
	RCC->AHB1ENR 		 |= GPIODEN; //Enable clock for GPIOD port, where LEDs are connected
	RCC->AHB1ENR 		 |= GPIOAEN; //Enable clock for GPIOA port, where pushbutton connected
	GPIOA->MODER 		 &= ~(BTN_EN); //Set direction of GPIOA pin 0 to input
	GPIOD->MODER 		 &= ~(0xFF<<24); //Clear all bits from pins 12 to 15
	GPIOD->MODER 		 |= (0x55<<24); //Set pins 12 to 15 as gp output
	uint32_t switchstatus = 0;
	uint32_t trigger      = 0;
	while(1)
	{
		trigger = (GPIOA->IDR & BTN_PIN);
		switchstatus ^= trigger; //Remember the switch status for toggle
		if(switchstatus)
		{
			GPIOD->ODR &= ~(15U<<12);
			GPIOD->ODR |= (15U<<12);
			for(uint32_t volatile i = 1; i <= DELAY_HIGH; i++);
			GPIOD->ODR &= ~(15U<<12);
			for(uint32_t volatile i = 1; i <= DELAY_HIGH; i++);
		}
		else
		{
			GPIOD->ODR &= ~(10U<<12);
			GPIOD->ODR ^= (5U<<12);
			for(uint32_t volatile i = 1; i <= DELAY_HIGH; i++);
		}
	}
}

#include "stm32f4xx.h"
#include "General.h"
#include "usart.h"
#include "systick.h"
#include "tim.h"
#include <stdio.h>

/*Toggle LED at PA5 after 1 second of delay using TIMER 2*/

int main(void)
{

	volatile uint16_t period  = 2000;
#if 0
	volatile uint16_t on_time = 800;
	while(1)
	{

		tim2_pwm_pa5(on_time, period);
	}
#else
	volatile uint32_t i = 0;
	while(1)
	{

			tim2_pwm_pa5(i, period);
			i+=50;
			if (i > period)
				i = 0;
	}
#endif

}

#include "systick.h"
#include <stdint.h>

void systick_init(void){
	//Provide clock source for Systick
	SYSTICK->STCSR |= CLK_SOURCE;
	//Clear current value
	SYSTICK->STCVR = TICK_CLEAR;
	//Disable systick interrupt request
	SYSTICK->STCSR &= ~(TICK_EXCEPTION);
}

void systick_init_interrupt_ms(uint32_t clk, uint32_t delay_ms){
	//Provide clock source for Systick
	SYSTICK->STCSR |= CLK_SOURCE;
	//Clear current value
	SYSTICK->STCVR = TICK_CLEAR;
	//Enable systick interrupt request
	SYSTICK->STCSR |= (TICK_EXCEPTION);
	//Calculate count value
	uint32_t volatile N = (clk/1000)*delay_ms;
	SYSTICK->STRVR = N - 1;
	//Restart counter
	SYSTICK->STCSR |= TICK_ENABLE;

}

void delay_time_hr(uint32_t delay_hr, uint32_t clk)
{
	for(uint32_t volatile i = 0; i < delay_hr; i++)
	{
		delay_time_min(i*60, clk);
	}
}


void delay_time_min(uint32_t delay_min, uint32_t clk)
{
	for(uint32_t volatile i = 0; i < delay_min; i++)
	{
		delay_time_sec(i*60, clk);
	}
}

void delay_time_sec(uint32_t delay_s, uint32_t clk)
{
	for(uint32_t volatile i = 0; i < delay_s;i++)
	{
		delay_time_ms(1000, clk);
	}
}

void delay_time_ms(uint32_t delay_ms, uint32_t clk)
{
	uint32_t N = (clk/1000U)*delay_ms;
	SYSTICK->STRVR = N - 1;
	systick_init();  							//Initialize counter
	SYSTICK->STCSR |= TICK_ENABLE;				//Restart counter
	while((SYSTICK->STCSR & CNT_CMPLT) == 0);   //Wait for countdown to complete
}

void delay_time_us(uint32_t delay_us, uint32_t clk)
{
	uint32_t N = (clk/1000000UL)*delay_us;
	SYSTICK->STRVR = N - 1;
	systick_init();  							//Initialize counter
	SYSTICK->STCSR |= TICK_ENABLE;				//Restart counter
	while((SYSTICK->STCSR & CNT_CMPLT) == 0);   //Wait for countdown to complete
}

void delay_time_ns(uint32_t delay_ns, uint32_t clk)
{
	uint32_t N = ((clk/1000000UL)/1000U)*delay_ns;
	SYSTICK->STRVR = N - 1;
	systick_init();  							//Initialize counter
	SYSTICK->STCSR |= TICK_ENABLE;				//Restart counter
	while((SYSTICK->STCSR & CNT_CMPLT) == 0);   //Wait for countdown to complete
}

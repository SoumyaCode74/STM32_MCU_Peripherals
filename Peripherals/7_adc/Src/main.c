#include <stdio.h>
#include "General.h"
#include "adc.h"
#include "usart.h"
#include "systick.h"

uint32_t result;
uint16_t result_beforedecimal;
uint16_t result_afterdecimal;

int main(void)
{
	pa1_ADC1_init();
	USART2_init(HSICLK, UART_BAUDRATE);
	while(1)
	{
		start_conversion();   //Initiate conversion for single conversion mode every time.
							  //For continuous conversion mode, initiate conversion only once
		result = perform_conversion();
		result_beforedecimal = (unsigned int)((5*result)/4096);
		result_afterdecimal = (unsigned int)((5*result)%4096);
		if(result_afterdecimal < 1000)
		{
			printf("Input voltage: %d.0%d V\r\n", result_beforedecimal, result_afterdecimal);
		}
		else
		{
			printf("Input voltage: %d.%d V\r\n", result_beforedecimal, result_afterdecimal);
		}
		delay_time_ms(200, HSICLK);//Wait for 200 ms betweeen each message
	}

}

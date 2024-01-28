#include "stm32f4xx.h"
#include <stdio.h>
#include "usart.h"
#include "General.h"
#define  UART_BAUDRATE			9600UL
/*Tested with ARM Compiler : version 5*/

void LED_play(int value);

char c;
char *word;
int main (void) {


	 RCC->AHB1ENR |=  1;             /* enable GPIOA clock */
     GPIOA->MODER |=  0x00000400;    /* set pin 5 to output mode */

     USART2_init(HSICLK, UART_BAUDRATE);          /* initialize USART2 */

    while(1) {              /* Loop forever */
    	/*c = USART2_read();
    	printf("Key entered: %c\r\n",c);*/
    	unsigned int volatile i = 0;
    	for(; i < 256; i++)
    	{
    		c = USART2_read();
    		USART2_write(c);
    		if (c == '\n')
    		{
    			printf("\n\r");
    			break;
    		}
    		else
    		{
    			word[i] = c;
    		}
    	}
    	word[i] = '\0';
    }
}

/* Read a character from USART2 */


/* turn on or off the LEDs according to the value */
void LED_play(int value) {
    value %= 16;                    /* cap the max count at 15 */

    for (; value > 0; value--) {
        GPIOA->BSRR = 0x00000020;    /* turn on LED */
        delayMs(100);
        GPIOA->BSRR = 0x00200000;    /* turn off LED */
        delayMs(100);
    }
    delayMs(400);
}

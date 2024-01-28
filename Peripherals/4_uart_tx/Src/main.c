#include <stdio.h>

#define GPIOAEN					(1U<<0)
#define USART2EN				(1U<<17)
#define SYS_FREQ				16000000UL //Default system frequency internal RC oscillator
#define APB1_CLK				SYS_FREQ
#define UART_BAUDRATE			9600UL

/*Tested with ARM Compiler : version 5*/
int main (void){
    USART2_init(APB1_CLK, UART_BAUDRATE);          /* initialize USART2 */

    while(1) {              /* Loop forever */
        /*USART2_write('Y');
        USART2_write('i');*/
    	printf("Hello Soumyadeep, from STM32F401RET%d.........\r\n",6);
    	delayMs(500);        /* leave a gap between messages */
    	printf("Hatti matim tim! Tara mathe pare dim!\r\n");
    	delayMs(500);        /* leave a gap between messages */
    	printf("Tader khara duto shingh!\r\n");
    	delayMs(500);        /* leave a gap between messages */
    	printf("Tara hatti matim tim!\r\n");
    	delayMs(500);        /* leave a gap between messages */
    	printf("\r\n");
        delayMs(500);        /* leave a gap between messages */
    }
}



/*#include "usart.h"
#include "General.h"

#define GPIOAEN					(1U<<0)
#define USART2EN				(1U<<17)
#define SYS_FREQ				16000000UL //Default system frequency internal RC oscillator
#define APB1_CLK				SYS_FREQ
#define UART_BAUDRATE			115200UL

int main(void)
{
	uart2_rxtx_init(APB1_CLK, UART_BAUDRATE);
	while(1)
	{
		uart2_write('U');
//		USART2->DR = 65;
	}
}
*/

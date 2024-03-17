#include <stdio.h>
#include "usart.h"

#define GPIOAEN					(1U<<0)
#define USART2EN				(1U<<17)
#define SYS_FREQ				16000000UL //Default system frequency internal RC oscillator
#define APB1_CLK				SYS_FREQ
#define UART_BAUDRATE			9600UL

static void USART1_RX_Callback(void);
/*Tested with ARM Compiler : version 5*/
int main (void){
	USART1_rx_interrupt_init(APB1_CLK, UART_BAUDRATE);          /* initialize USART2 */

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
void USART1_IRQHandler(void){
	USART1_RX_Callback();
}

static void USART1_RX_Callback(void)
{
	static int times = 0;
	++times;
	char c = USART1->DR; //Read from data register and reset the RXNIE bit
    while (!((USART1->SR >> 0x7) & 0x1)) {}
    USART1->DR = c+1;
//	USART1_write(c+1);

//	printf(USART1->DR);
//	printf("Hello, you pressed key %c!\n\r",c);
//	printf("Times interrupted: %d\n\r", times);
//	LED_play(c);
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

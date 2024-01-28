/*
 * uart1_discovery_board.c
 *
 *  Created on: Apr 23, 2023
 *      Author: soumy
 */

#include "uart1_discovery_board.h"
#include <stdio.h>


void configure_UART1_Rx_interrupt(uint32_t clk, uint32_t baud){
	/* Set baud rate */
	pUSART1_base[2] = BAUD_RATE_SET(clk, baud);
	/* enable RXNEIE, TE, RE */
	pUSART1_base[3] |= (0x01 << 5) | (0x01 << 3) | (0x01 << 2);
	/* enable USART1 interrupt */
	pNVIC_ISER[1] |= (0x01 << (37 - 32));
	/* enable USART1 module */
	pUSART1_base[3] |= (0x01 << 13);
}

void UART1_write(int letter){
	//Wait for TXE to be set
	while (!(pUSART1_base[0] & (0x01 << 7)));
	//Write to DR
	pUSART1_base[1] = (letter & 0xFF);
}

int __io_putchar(int ch){
	UART1_write(ch);
	return ch;
}

/* Assigning weak attribute so that it can be overriden in the main.c */
__attribute__ ((weak)) void USART1_Callback(){
	UART1_write(pUSART1_base[1]);
}


void board_init(void){
	pRCC_base[12] |= 0x01;		//Enable clock access to GPIOA
	pRCC_base[17] |= (0x01 << 4); //Enable clock access to USART1
	pGPIOA_base[0] |= (0x0A << 18); //Set PA9 and PA10 to alternate function mode
	pGPIOA_base[9] |= (0x77 << 4); //Set AF07 for PA9 and PA10
	pSHCRS[0]	   |= (0x01 << 18); //Enable UsageFault exception if ORE is set
}

void delay(uint32_t count){
	for(int i = 1; i <= count; i++){
		for(int j = 1; j <= i; j++){
			;
		}
	}
}

void enable_floating_point(void){
	pCPACR[0] |= (0b1111 << 20); //Enable CP10 and CP11
}

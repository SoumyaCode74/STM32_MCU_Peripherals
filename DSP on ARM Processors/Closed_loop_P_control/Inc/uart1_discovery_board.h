/*
 * uart1_discovery_board.h
 *
 *  Created on: Apr 23, 2023
 *      Author: soumy
 */

#ifndef UART1_DISCOVERY_BOARD_H_
#define UART1_DISCOVERY_BOARD_H_

#include <stdint.h>

#define pRCC_base		((uint32_t *)0x40023800)
#define pGPIOA_base     ((uint32_t *)0x40020000)
#define pUSART1_base    ((uint32_t *)0x40011000)
#define pNVIC_ISER      ((uint32_t *)0xE000E100)
#define pSHCRS	        ((uint32_t *)0xE000ED24)
#define pCPACR 	        ((uint32_t *)0xE000ED88)

#define BAUD_RATE_SET(x, y)		((x+(y/2U))/y)

extern void configure_UART1_Rx_interrupt(uint32_t, uint32_t);
extern void UART1_write(int);
extern void USART1_Callback();
extern void enable_floating_point(void);
extern void delay(uint32_t);
extern void board_init(void);



#endif /* UART1_DISCOVERY_BOARD_H_ */

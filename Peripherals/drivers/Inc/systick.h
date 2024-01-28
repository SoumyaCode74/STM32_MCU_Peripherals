/*
 * systick.h
 *
 *  Created on: 10-Nov-2022
 *      Author: soumya
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

#define __IO				volatile
#define __I				volatile
#define CLK_SOURCE			(1U<<2)
#define TICK_EXCEPTION		(1U<<1)
#define TICK_ENABLE			(1U<<0)
#define TICK_CLEAR			(1U<<0)
#define CNT_CMPLT			(1U<<16)
#define SYSTICK_BASE		(0xE000E010UL)
#define SYSTICK				((Systick_TypeDef *) SYSTICK_BASE)

typedef struct{
	__IO uint32_t STCSR;
	__IO uint32_t STRVR;
	__IO uint32_t STCVR;
	__I  uint32_t STCR;
}Systick_TypeDef;

void systick_init(void);

void systick_init_interrupt_ms(uint32_t, uint32_t);

void delay_time_hr(uint32_t, uint32_t);

void delay_time_min(uint32_t, uint32_t);

void delay_time_sec(uint32_t, uint32_t);

void delay_time_ms(uint32_t, uint32_t);

void delay_time_us(uint32_t, uint32_t);

void delay_time_ns(uint32_t, uint32_t);

#endif /* SYSTICK_H_ */

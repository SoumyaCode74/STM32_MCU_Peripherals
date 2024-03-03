/*
 * General.h
 *
 *  Created on: 06-Nov-2022
 *      Author: soumy
 */

#ifndef GENERAL_H_
#define GENERAL_H_

#include "systick.h"

#define DELAY_HIGH			500000UL
#define DELAY_LOW  			200000UL
#define SMALL_DELAY			1000U
#define HSICLK				(16000000UL)
#define BD_9600				(9600U)
#define BD_115200			(115200UL)
#define BIT0				(1U << 0)
#define BIT1				(1U << 1)
#define BIT2				(1U << 2)
#define BIT3				(1U << 3)
#define BIT4				(1U << 4)
#define BIT5				(1U << 5)
#define BIT6				(1U << 6)
#define BIT7				(1U << 7)
#define BIT8				(1U << 8)
#define BIT9				(1U << 9)
#define BIT10				(1U << 10)
#define BIT11				(1U << 11)
#define BIT12				(1U << 12)
#define BIT13				(1U << 13)
#define BIT14				(1U << 14)
#define BIT15				(1U << 15)
#define BIT16				(1U << 16)
#define BIT17				(1U << 17)
#define BIT18				(1U << 18)
#define BIT19				(1U << 19)
#define BIT20				(1U << 20)
#define BIT21				(1U << 21)
#define BIT22				(1U << 22)
#define BIT23				(1U << 23)
#define BIT24				(1U << 24)
#define BIT25				(1U << 25)
#define BIT26				(1U << 26)
#define BIT27				(1U << 27)
#define BIT28				(1U << 28)
#define BIT29				(1U << 29)
#define BIT30				(1U << 30)
#define BIT31				(1U << 31)

#define NVIC				((NVIC_TypeDef *)0xE000E100UL)
#define RCC					((RCC_TypeDef *)0x40023800UL)
#define GPIOA				((GPIO_TypeDef *)0x40020000UL)
#define GPIOB				((GPIO_TypeDef *)0x40020400UL)
#define GPIOC				((GPIO_TypeDef *)0x40020800UL)
#define GPIOD				((GPIO_TypeDef *)0x40020C00UL)
#define GPIOE				((GPIO_TypeDef *)0x40021000UL)

typedef struct{
	uint32_t reserved[12];
	volatile uint32_t AHB1ENR;
	uint32_t reserved1[3];
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
}RCC_TypeDef;

typedef struct{
	volatile uint32_t ISER[8];
	uint32_t reserved1[24];
	volatile uint32_t ICER[8];
	uint32_t reserved2[24];
	volatile uint32_t ISPR[8];
	uint32_t reserved3[24];
	volatile uint32_t ICPR[8];
	uint32_t reserved4[24];
	volatile uint32_t IABR[8];
	uint32_t reserved5[24];
	volatile uint32_t IPR[60];
	uint32_t reserved6[643];
	volatile uint32_t STIR;
}NVIC_TypeDef;

typedef struct{
	volatile uint32_t MODER;
	volatile uint32_t OYTPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFR[2];
}GPIO_TypeDef;

void activate_floating_point_unit(void);


//#define SYSCLK				16000000UL //Default internal RC oscillator frequency = 16 MHz

#endif /* GENERAL_H_ */

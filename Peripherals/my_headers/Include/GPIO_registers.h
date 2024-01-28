/*
 * GPIO_registers.h
 *
 *  Created on: Oct 22, 2022
 *      Author: soumy
 */

#include <stdint.h>
#ifndef GPIO_REGISTERS_H_
#define GPIO_REGISTERS_H_

#define __IO volatile

#define ADDR_GPIOA_BASE 			0x40020000ul
#define ADDR_GPIOB_BASE 			0x40020400ul
#define ADDR_GPIOC_BASE 			0x40020800ul
#define ADDR_GPIOD_BASE 			0x40020C00ul
#define ADDR_GPIOE_BASE 			0x40021000ul
#define ADDR_GPIOF_BASE 			0x40021400ul
#define ADDR_GPIOG_BASE 			0x40021800ul
#define ADDR_GPIOH_BASE 			0x40021C00ul
#define ADDR_GPIOI_BASE 			0x40022000ul
#define ADDR_GPIOJ_BASE 			0x40022400ul
#define ADDR_GPIOK_BASE 			0x40022800ul

#define GPIOx_MODER_OFFSET   			0x00
#define GPIOx_OTYPER_OFFSET  			0x04
#define GPIOx_OSPEEDR_OFFSET 			0x08
#define GPIOx_PUPDR_OFFSET   			0x0C
#define GPIOx_IDR_OFFSET     			0x10
#define GPIOx_ODR_OFFSET     			0x14
#define GPIOx_BSRR_OFFSET    			0x18
#define GPIOx_LCKR_OFFSET    			0x1C
#define GPIOx_AFRL_OFFSET    			0x20
#define GPIOx_AFRH_OFFSET    			0x24

#define PIN_SET_HIGH					0x01
#define PIN_SET_LOW						0x00

#define GPIOA_REG						((GPIO_TypeDef volatile *) ADDR_GPIOA_BASE)
#define GPIOB_REG						((GPIO_TypeDef volatile *) ADDR_GPIOB_BASE)
#define GPIOC_REG						((GPIO_TypeDef volatile *) ADDR_GPIOC_BASE)
#define GPIOD_REG						((GPIO_TypeDef volatile *) ADDR_GPIOD_BASE)

typedef struct
{
	union
	{
		__IO uint32_t moder;
		struct
		{
			__IO uint32_t moder0  :2;
			__IO uint32_t moder1  :2;
			__IO uint32_t moder2  :2;
			__IO uint32_t moder3  :2;
			__IO uint32_t moder4  :2;
			__IO uint32_t moder5  :2;
			__IO uint32_t moder6  :2;
			__IO uint32_t moder7  :2;
			__IO uint32_t moder8  :2;
			__IO uint32_t moder9  :2;
			__IO uint32_t moder10 :2;
			__IO uint32_t moder11 :2;
			__IO uint32_t moder12 :2;
			__IO uint32_t moder13 :2;
			__IO uint32_t moder14 :2;
			__IO uint32_t moder15 :2;
		}MODER;

	};
	union
	{
		__IO uint32_t ot;
		struct
		{
			__IO uint32_t ot0      :1;
			__IO uint32_t ot1      :1;
			__IO uint32_t ot2      :1;
			__IO uint32_t ot3      :1;
			__IO uint32_t ot4      :1;
			__IO uint32_t ot5      :1;
			__IO uint32_t ot6      :1;
			__IO uint32_t ot7      :1;
			__IO uint32_t ot8      :1;
			__IO uint32_t ot9      :1;
			__IO uint32_t ot10     :1;
			__IO uint32_t ot11     :1;
			__IO uint32_t ot12     :1;
			__IO uint32_t ot13     :1;
			__IO uint32_t ot14     :1;
			__IO uint32_t ot15     :1;
			__IO uint16_t reserved[1];
		}OT;
	};
	union
	{
		__IO uint32_t ospeedr;
		struct
		{
			__IO uint32_t ospeedr0  :2;
			__IO uint32_t ospeedr1  :2;
			__IO uint32_t ospeedr2  :2;
			__IO uint32_t ospeedr3  :2;
			__IO uint32_t ospeedr4  :2;
			__IO uint32_t ospeedr5  :2;
			__IO uint32_t ospeedr6  :2;
			__IO uint32_t ospeedr7  :2;
			__IO uint32_t ospeedr8  :2;
			__IO uint32_t ospeedr9  :2;
			__IO uint32_t ospeedr10 :2;
			__IO uint32_t ospeedr11 :2;
			__IO uint32_t ospeedr12 :2;
			__IO uint32_t ospeedr13 :2;
			__IO uint32_t ospeedr14 :2;
			__IO uint32_t ospeedr15 :2;
		}OSPEEDR;
	};
	union
	{
		__IO uint32_t pupdr;
		struct
		{
			__IO uint32_t pupdr0  :2;
			__IO uint32_t pupdr1  :2;
			__IO uint32_t pupdr2  :2;
			__IO uint32_t pupdr3  :2;
			__IO uint32_t pupdr4  :2;
			__IO uint32_t pupdr5  :2;
			__IO uint32_t pupdr6  :2;
			__IO uint32_t pupdr7  :2;
			__IO uint32_t pupdr8  :2;
			__IO uint32_t pupdr9  :2;
			__IO uint32_t pupdr10 :2;
			__IO uint32_t pupdr11 :2;
			__IO uint32_t pupdr12 :2;
			__IO uint32_t pupdr13 :2;
			__IO uint32_t pupdr14 :2;
			__IO uint32_t pupdr15 :2;
		}PUPDR;

	};
	union
	{
		__IO uint32_t idr;
		struct
		{
			__IO uint32_t idr0  :1;
			__IO uint32_t idr1  :1;
			__IO uint32_t idr2  :1;
			__IO uint32_t idr3  :1;
			__IO uint32_t idr4  :1;
			__IO uint32_t idr5  :1;
			__IO uint32_t idr6  :1;
			__IO uint32_t idr7  :1;
			__IO uint32_t idr8  :1;
			__IO uint32_t idr9  :1;
			__IO uint32_t idr10 :1;
			__IO uint32_t idr11 :1;
			__IO uint32_t idr12 :1;
			__IO uint32_t idr13 :1;
			__IO uint32_t idr14 :1;
			__IO uint32_t idr15 :1;
			__IO uint16_t reserved[1];
		}IDR;
	};
	union
	{
		__IO uint32_t odr;
		struct
		{
			__IO uint32_t odr0  :1;
			__IO uint32_t odr1  :1;
			__IO uint32_t odr2  :1;
			__IO uint32_t odr3  :1;
			__IO uint32_t odr4  :1;
			__IO uint32_t odr5  :1;
			__IO uint32_t odr6  :1;
			__IO uint32_t odr7  :1;
			__IO uint32_t odr8  :1;
			__IO uint32_t odr9  :1;
			__IO uint32_t odr10 :1;
			__IO uint32_t odr11 :1;
			__IO uint32_t odr12 :1;
			__IO uint32_t odr13 :1;
			__IO uint32_t odr14 :1;
			__IO uint32_t odr15 :1;
			__IO uint16_t reserved[1];

		}ODR;
	};
	union
	{
		__IO uint32_t bs;
		struct
		{
			__IO uint32_t bs0  :1;
			__IO uint32_t bs1  :1;
			__IO uint32_t bs2  :1;
			__IO uint32_t bs3  :1;
			__IO uint32_t bs4  :1;
			__IO uint32_t bs5  :1;
			__IO uint32_t bs6  :1;
			__IO uint32_t bs7  :1;
			__IO uint32_t bs8  :1;
			__IO uint32_t bs9  :1;
			__IO uint32_t bs10 :1;
			__IO uint32_t bs11 :1;
			__IO uint32_t bs12 :1;
			__IO uint32_t bs13 :1;
			__IO uint32_t bs14 :1;
			__IO uint32_t bs15 :1;

			__IO uint32_t br0  :1;
			__IO uint32_t br1  :1;
			__IO uint32_t br2  :1;
			__IO uint32_t br3  :1;
			__IO uint32_t br4  :1;
			__IO uint32_t br5  :1;
			__IO uint32_t br6  :1;
			__IO uint32_t br7  :1;
			__IO uint32_t br8  :1;
			__IO uint32_t br9  :1;
			__IO uint32_t br10 :1;
			__IO uint32_t br11 :1;
			__IO uint32_t br12 :1;
			__IO uint32_t br13 :1;
			__IO uint32_t br14 :1;
			__IO uint32_t br15 :1;
		}BSBR;
	};
	union
	{
		__IO uint32_t lck;
		struct
		{
			__IO uint32_t lck0  :1;
			__IO uint32_t lck1  :1;
			__IO uint32_t lck2  :1;
			__IO uint32_t lck3  :1;
			__IO uint32_t lck4  :1;
			__IO uint32_t lck5  :1;
			__IO uint32_t lck6  :1;
			__IO uint32_t lck7  :1;
			__IO uint32_t lck8  :1;
			__IO uint32_t lck9  :1;
			__IO uint32_t lck10 :1;
			__IO uint32_t lck11 :1;
			__IO uint32_t lck12 :1;
			__IO uint32_t lck13 :1;
			__IO uint32_t lck14 :1;
			__IO uint32_t lck15 :1;
			__IO uint32_t lckk  :1;
			__IO uint16_t reserved[1];
		}LCK;
	};
	union
	{
		__IO uint32_t afrl;
		struct
		{
			__IO uint32_t afrl0 :4;
			__IO uint32_t afrl1 :4;
			__IO uint32_t afrl2 :4;
			__IO uint32_t afrl3 :4;
			__IO uint32_t afrl4 :4;
			__IO uint32_t afrl5 :4;
			__IO uint32_t afrl6 :4;
			__IO uint32_t afrl7 :4;
		}AFRL;
	};
	union
	{
		__IO uint32_t afrh;
		struct
		{
			__IO uint32_t afrh0 :4;
			__IO uint32_t afrh1 :4;
			__IO uint32_t afrh2 :4;
			__IO uint32_t afrh3 :4;
			__IO uint32_t afrh4 :4;
			__IO uint32_t afrh5 :4;
			__IO uint32_t afrh6 :4;
			__IO uint32_t afrh7 :4;
		}AFRH;
	};
}GPIO_TypeDef;

#endif /* GPIO_REGISTERS_H_ */
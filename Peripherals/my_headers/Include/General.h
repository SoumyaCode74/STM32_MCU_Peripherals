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
#define HSICLK				(16000000UL + (RCC->CR & (0xFF << 8)))
//#define SYSCLK				16000000UL //Default internal RC oscillator frequency = 16 MHz

#endif /* GENERAL_H_ */

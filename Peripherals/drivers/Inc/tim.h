/*
 * tim.h
 *
 *  Created on: 11-Nov-2022
 *      Author: soumy
 */

#ifndef TIM_H_
#define TIM_H_
#include "General.h"

#define TIM2                            ((TIM2_to_TIM5_TypeDef *)0x40000000)
#define TIM3                            ((TIM2_to_TIM5_TypeDef *)0x40000400)
#define TIM4                            ((TIM2_to_TIM5_TypeDef *)0x40000800)
#define TIM5                            ((TIM2_to_TIM5_TypeDef *)0x40000C00)

#define TIM2EN  BIT0
#define TIM3EN	BIT1
#define CNTEN   BIT0
#define CNTEND	BIT0
#define PSC_MS	(16000U)
#define PSC_US	(16U)

typedef struct{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    uint32_t reserved1;
    volatile uint32_t CCR[4];
    uint32_t reserved2;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t TIM2_OR;
    volatile uint32_t TIM5_OR;
    
}TIM2_to_TIM5_TypeDef;

void tim2_init(void);

void tim2_cont_ms(uint32_t);

void tim2_cont_us(uint32_t);

void tim2_pwm_pa5(uint32_t, uint32_t);

uint32_t tim3_input_capture_pb4(void);
#endif /* TIM_H_ */

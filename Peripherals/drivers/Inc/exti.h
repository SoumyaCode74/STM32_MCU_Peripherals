/*
 * exti.h
 *
 *  Created on: 12-Nov-2022
 *      Author: soumy
 */
#include <stdint.h>
#include "General.h"

#ifndef EXTI_H_
#define EXTI_H_

#define SYSCFG                  ((SYSCFG_TypeDef *)0x40013800)
#define EXTI                    ((EXTI_TypeDef *)0x40013C00)

typedef struct{
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
}EXTI_TypeDef;

typedef struct{
    volatile uint32_t MEMRMP;
    volatile uint32_t PMC;
    volatile uint32_t EXTICR[4];
    volatile uint32_t CMPCR;
}SYSCFG_TypeDef;

void pc13_exti_init(void);



#endif /* EXTI_H_ */

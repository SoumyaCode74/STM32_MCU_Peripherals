#ifndef ADC_H
#define ADC_H
#include <stdint.h>

#define GPIOAEN		(1U<<0)
#define GPIOCEN		(1U<<2)
#define ADC1_EN		(1U<<8)
#define ADC1_EOC	(1U<<1)
#define ADC1_START	(1U<<30)
#define ADC1_ALIGN	(1U<<11)
#define ADC1_ON		(1U<<0)
#define SINGLE		(1U<<1)
#define CONT		(1U<<1)
#define ADC1_CH1	(0x01U)
#define ADC1_CH2	(0x02U)
#define ADC1_CH3	(0x03U)
#define ADC1_CH4	(0x04U)
#define ADC1_CH5	(0x05U)
#define ADC1_CH6	(0x06U)
#define ADC1_CH7	(0x07U)
#define ADC1_CH8	(0x08U)
#define ADC1_CH9	(0x09U)
#define ADC1_CH10	(0x0AU)
#define ADC1_CH11	(0x0BU)
#define ADC1_CH12	(0x0CU)
#define ADC1_CH13	(0x0DU)
#define ADC1_CH14	(0x0EU)
#define ADC1_CH15	(0x0FU)

#define VREF_5V		(5U)
#define VREF_3V3	(3.3U)

void pa1_ADC1_init(void);

void start_conversion(void);

uint32_t perform_conversion(void);

uint32_t input_voltage(uint32_t, uint8_t, uint8_t);

#endif



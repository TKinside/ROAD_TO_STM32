//
// Created by tKinsde on 2025/4/21.
//

#ifndef STM32_ADC_DMA_NTC_H
#define STM32_ADC_DMA_NTC_H

#include "main.h"
#include "math.h"
float ADC2Resistance(uint32_t adc_value);
float resistance2Temperature(float R1);

#endif //STM32_ADC_DMA_NTC_H

//
// Created by tKinsde on 2025/4/21.
//
#include "ntc.h"
float ADC2Resistance(uint32_t adc_value) {
    return (adc_value / (4095.0f - adc_value)) * 10000.0f;
}
float resistance2Temperature(float R1) {
    float B = 3950.0f;
    float R2 = 10000.0f;
    float T2 = 25.0f;
    return (1.0 / ((1.0 / B) * log(R1 / R2) + (1.0 / (T2 + 273.15))) - 273.15);
}
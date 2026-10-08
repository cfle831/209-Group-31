

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <avr/io.h>
#define  V_REF 5

void adc_init(void);
uint16_t adc_read(uint8_t chan);
uint16_t adc_convert_mv(uint16_t adc_count);
uint32_t calculate_power(uint16_t voltage[], uint16_t current[], uint8_t samples);
float calculate_RMS(uint16_t values[], uint8_t samples);

#endif
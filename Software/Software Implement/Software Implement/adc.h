

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <avr/io.h>
#define  V_REF 5

// Initialise registers for adc 
void adc_init(void);
// Read the adc value in a specified channel
uint16_t adc_read(uint8_t chan);
// Convert an adc reading to mV
uint16_t adc_convert_mv(uint16_t adc_count);
// Calculate power from stored current and voltage readings (in mV) 
uint32_t calculate_power(const uint16_t voltage[], const uint16_t current[], uint8_t samples);
// Calculate the RMS Value of a signal 
uint16_t calculate_RMS(const uint16_t values[], uint8_t samples);

#endif
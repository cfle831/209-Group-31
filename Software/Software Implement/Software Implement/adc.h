

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <avr/io.h>

#define  V_REF 5
#define ESTIMATED_SAMPLE_TIME 0.000104f // Accurate for prescaler 16
#define SAMPLES_TO_TAKE 40 //  Possibly should be changed


// Initialise registers for adc 
void adc_init(void);

// Enable and disable the adc sampling
void adc_sample_enable(void);
void adc_sample_disable(void);

// Read the adc value in a specified channel
uint16_t adc_read(uint8_t chan);
// Convert an adc reading to mV
uint16_t adc_convert_mv(uint16_t adc_count);
// Calculate power from stored current and voltage readings (in mV) 
uint32_t calculate_power(const uint16_t voltage[], const uint16_t current[], uint8_t samples);
// Calculate the RMS Value of a signal 
uint16_t calculate_RMS(const uint16_t values[], uint8_t samples);
// Removes gain's and offsets to find the input signal's values
int32_t find_load_voltage(uint16_t adc_measurement);

int32_t find_load_current(uint16_t adc_measurement);

uint32_t square_root(uint32_t n);
// Approximates half a step back to reduce percieved phase shift
void approximate_step_back_all(uint16_t values[], uint8_t samples);


#endif
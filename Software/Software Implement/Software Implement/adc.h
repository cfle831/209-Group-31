

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define  V_REF 5
#define SAMPLES_TO_TAKE			160 //  Possibly should be changed

#define VOLTAGE_ADC				(0)
#define CURRENT_ADC				(1)


// Initialise registers for adc 
void adc_init(void);

void zero_crossing_init(void);

// Enable and disable the adc sampling
void adc_sample_enable(void);
void adc_sample_disable(void);

// Read the adc value in a specified channel
uint16_t adc_read(uint8_t chan);
// Convert an adc reading to mV
uint16_t adc_convert_mv(uint16_t adc_count);

// Calculate instantaneous power from stored current and voltage readings (in mV & mA) returns power in mW  
void calculate_inst_power(int32_t voltage[], int32_t current[], uint8_t samples, int16_t array_to_fill[]);

// Removes gain's and offsets to find the input signal's values
int32_t find_load_voltage(uint16_t adc_measurement);

int32_t find_load_current(uint16_t adc_measurement);

uint32_t square_root(uint32_t n);
// Approximates half a step back to reduce percieved phase shift
void approximate_step_back_all(uint16_t values[], uint8_t samples);

// for testing
double calculate_rms(const int32_t *samples, int32_t count);


#endif
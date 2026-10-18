

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define SAMPLES_TO_TAKE			160 //  Possibly should be changed

#define VOLTAGE_ADC				(0)
#define CURRENT_ADC				(1)


// Initialise registers for adc 
void adc_init(void);

void zero_crossing_init(void);

// Read the adc value in a specified channel
uint16_t adc_read(uint8_t chan);
// Convert an adc reading to mV
uint16_t adc_convert_mv(uint16_t adc_count);

// Calculate instantaneous power from stored current and voltage readings (in mV & mA) returns power in mW  
void calculate_inst_power(int16_t voltage[], int16_t current[], uint8_t samples, int32_t array_to_fill[]);

// Removes gain's and offsets to find the input signal's values
int16_t find_load_voltage(uint16_t adc_measurement);

int16_t find_load_current(uint16_t adc_measurement);

uint32_t square_root(uint32_t n);
// Approximates half a step back to reduce percieved phase shift
//void approximate_step_back_all(uint16_t values[], uint8_t samples);

// for testing
double calculate_rms(const int16_t *samples, uint8_t count);


#endif
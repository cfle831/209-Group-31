

#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define SAMPLES_TO_TAKE			(160) //  Possibly should be changed
#define	ADC_RESOLUTION			(1024)

#define VOLTAGE_ADC				(0)
#define CURRENT_ADC				(1)

// 1st order filter: 0.817
#define FILTER_GAIN				(0.817)

/* Gain values in direction towards measurement pins:
* Vmeas Opamp: 1
* 2.11 V offset ---> 432
* Input voltage divider: 0.04511
* Corrective value: 1.05 
*/
#define V_OFFSET				(432)
#define V_CORRECTIVE			(1.05)
#define VIN_DIVIDER				(0.04511)

/*	CURRENT 
	Gain values in direction towards measurement pins:
	1st order filter: 0.817
	Imeas Opamp:  2.47177
	Offset: 2.033 ---> 416 adc counts 
	Input voltage divider: V= I * Rshunt so I = V / 0.5021
	Corrective value: 1.07
*/
#define	I_OFFSET				(416)
#define I_OPAMP					(2.4718)
#define R_SHUNT					(0.5021)
#define I_CORRECTIVE			(1.07)

// Initialise registers for adc 
void adc_init(void);

void zero_crossing_init(void);

// Calculates instantaneous power for a single voltage/current pair (in mW)
int32_t calculate_instantaneous_power_sample(int16_t v_mV, int16_t i_mA);

// Populates an array with instantaneous power values (in mW) - optional for waveform logging
void calculate_instantaneous_power_array(const int16_t voltage[], const int16_t current[], uint8_t samples, int32_t inst_power[]);

// Calculates True Real Power directly from voltage and current arrays (in mW)
int32_t calculate_real_power(const int16_t voltage[], const int16_t current[], uint8_t samples);

// Removes gain's and offsets to find the input signal's values
int16_t find_load_voltage(uint16_t adc_measurement);

int16_t find_load_current(uint16_t adc_measurement);

uint32_t square_root(uint32_t n);
// Approximates half a step back to reduce percieved phase shift
//void approximate_step_back_all(uint16_t values[], uint8_t samples);

// for testing
double calculate_rms(const int16_t *samples, uint8_t count);

#endif
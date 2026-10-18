/*
 * Lab4ADC.c
 *
 * Created: 12/09/2026 1:44:14 pm
 * Author : TFE, Ferre
 */ 
#define F_CPU 2000000UL
#include "adc.h"
#include "UART.h"

#include <avr/io.h>
#include <util/delay.h>
#include <float.h>
#include <stdint.h> //
#include <math.h> //

extern volatile uint8_t adc_samples_full; // From main.c
extern volatile uint8_t start_sampling;
static volatile uint8_t voltage_read = 0;
volatile uint16_t voltage[SAMPLES_TO_TAKE];
volatile uint16_t current[SAMPLES_TO_TAKE];
static volatile uint8_t idx = 0;

/* in order to acheive a *maximum* sampling frequency of 10kHz, samples must be taken ~100us apart */
/* Sampling frequency must not be a multiple of 500Hz*/

void zero_crossing_init(void){
	EICRA |= (3 << ISC00); // INT0 interrupt on rising edge
	EIMSK |= (1 << INT0); // enable INT0 interrupts
}

void adc_init(void){
	// Enable ADC converter
	ADCSRA = (1<<ADEN);
	ADMUX = (1 << REFS0);
	// Clock prescaler of 16
	ADCSRA |= (1 << ADPS2);
	ADCSRB = 0x00;
	DDRC &= ~(7<<PINC0); // For ADC pins 0, 1, 2 as input
	DDRD &= ~(1<<PIND2); // Zero Crossing Detection pin as input
	
	ADCSRA |= (1 << ADSC); // dummy conversion to settle values
}

// backstep through gain stages of the circuit to find initial value
int32_t find_load_voltage(uint16_t adc_measurement){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Vmeas Opamp: 1
	* 2.11 V offset 
	* Input voltage divider: 0.04511
	* Corrective value 1.1
	*/
	
	int32_t return_value = ((int32_t)adc_measurement-432)/0.817; // Finding exact offset voltage is very important for accuracy
	return_value *= 22; //   (0.04511)^-1
	return_value *= 5000*1.1;
	return_value /= 1024;
	return return_value;
}

int32_t find_load_current(uint16_t adc_measurement){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Imeas Opamp:  2.47177
	* Offset: 2.03 V 
	* Input voltage divider: V= I * Rshunt so I = V / 0.5021
	* Corrective value: 1.78
	*/
	int32_t return_value = adc_measurement;
	return_value -= 415;
	
	return_value *= 5000; // to get pure ac value in mV
	return_value /= 1024;
	return_value /= (2.47177*0.817*0.5021);
	return_value *= 1.78;
	return (return_value);
}

uint16_t adc_convert_mv(uint16_t adc_count){
	uint32_t value = (uint32_t)adc_count;
	value = (((uint32_t)value*V_REF*1000) / (1023));
	return value;
}


// Edits the input array
void approximate_step_back_all(uint16_t values[], uint8_t samples){
	for (uint8_t i = 1 ; i<samples; i++){
		values[i] = ((values[i] + values[i-1])>>1);
	}
}


// Returns an array in mW
void calculate_inst_power(int32_t voltage[], int32_t current[], uint8_t samples, int16_t array_to_fill[]){
	
	for (uint8_t i = 0; i<samples; i++){
		array_to_fill[i] = (voltage[i] * current[i] / 1000);
	}
	return;
}


// Calculate rms used for testing 
double calculate_rms(const int32_t *samples, int32_t count) {
	

	double sum_of_squares = 0.0;

	for (int32_t i = 0; i < count; i++) {
		double val = (double)samples[i];
		sum_of_squares += val * val;
	}

	return sqrt(sum_of_squares / count);
}




/////// ISR ///////

ISR(ADC_vect) { // ADC conversion complete interrupt
	if (!start_sampling) return;
	// sample adc
	// sample voltage then current adc
	
	// Store values from most recent conversion
	uint16_t adc_val = ADC;
	
	if (voltage_read){
		// Log the voltage adc sample in array
		voltage[idx] = adc_val;
	} else {
		// Store current adc sample in array
		current[idx] = adc_val;
	}
	// increment array index
	idx++;
	if (idx == SAMPLES_TO_TAKE){
		// Reset the index
		idx = 0;
		// If the full array was current, change flag
		if (!voltage_read){
			adc_samples_full = 1;
		} else {
			// Enable interrupt for current sampling
			EIMSK |= (1 << INT0);
		}
		// Disable this interrupt
		ADCSRA &= ~(1<<ADIE);
		start_sampling = 0;
	}
}

ISR(INT0_vect){
	// Enable the adc interrupts
	ADCSRA |= (1 << ADIE);
	
	// channel switching
	if (voltage_read){
		// Switch to the current sense channel
		// second to be sampled
		voltage_read = 0;
		ADMUX &= (0xF0); // Lower MUX bits
		ADMUX |= CURRENT_ADC;
	} else {
		// Switch to voltage sense channel
		// first to be sampled
		voltage_read = 1;
		ADMUX &= (0xF0); // Lower MUX bits
		ADMUX |= VOLTAGE_ADC;
	}
	
	idx = 0;
	start_sampling = 1;
	EIMSK &= ~(1 << INT0); // Disable (this) INT0 interrupt
}
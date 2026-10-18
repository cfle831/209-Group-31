/*
 * Lab4ADC.c
 *
 * Created: 12/09/2026 1:44:14 pm
 * Author : Loelene
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
volatile uint8_t voltage_read = 1;
volatile uint16_t voltage[SAMPLES_TO_TAKE];
volatile uint16_t current[SAMPLES_TO_TAKE];
volatile uint8_t idx = 0;

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

void adc_sample_enable(void){
	adc_samples_full = 0;
	ADCSRA |= (1 << ADIE);
}

void adc_sample_disable(void){
	adc_samples_full = 1;
	ADCSRA &= ~(1<<ADIE); // Disable interrupt
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
	* Corrective value: 1.4855
	*/
	int32_t return_value = adc_measurement;
	return_value -= 415;
	
	return_value *= 5000; // to get pure ac value in mV
	return_value /= 1024;
	return_value /= (2.47177*0.817*0.5021);
	return_value *= 1.4855;
	return (return_value);
}

uint16_t adc_convert_mv(uint16_t adc_count){
	uint32_t value = (uint32_t)adc_count;
	value = (((uint32_t)value*V_REF*1000) / (1023));
	return value;
}

// Approximates a step between index and the next value (either forwards or backwards)
// Used to better align the current and voltage data sets
uint16_t approximate_step(uint8_t d_backwards, const uint16_t values[], uint8_t index){
	uint16_t return_value;
	if (d_backwards){
		return_value = ((uint32_t)values[index] + values[index - 1])/2;	
	} else{
		return_value = ((uint32_t)values[index] + values[index + 1])/2;
	}
	return return_value;
}
// Edits the input array
void approximate_step_back_all(uint16_t values[], uint8_t samples){
	for (uint8_t i = 1 ; i<samples; i++){
		values[i] = ((values[i] + values[i-1])>>1);
	}
}

uint32_t calculate_power(const uint16_t voltage[], const uint16_t current[], uint8_t samples){
	uint64_t total_sum_IV = 0;
	
	for (uint8_t i=0; i<samples; i++){
		total_sum_IV += (uint32_t)(voltage[i]*current[i]);
	}
	uint32_t power = total_sum_IV/samples;
	return power;
}

uint32_t square_root(uint32_t n)
{
	uint32_t result = 0;
	uint32_t bit = 1UL << 30;

	// Start with the highest power of four <= n
	while (bit > n)
	bit >>= 2;
	while (bit != 0) {
		if (n >= result + bit) {
			n -= result + bit;
			result = (result >> 1) + bit;
			} else {
			result >>= 1;
		}
		bit >>= 2;
	}

	return result;
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

uint16_t calculate_peak(const uint16_t values[], uint8_t samples){
	// Just takes the largest value within the sample set
	// Possible improvement could instead use if value[i-1] < value[i] < value[i+1] & and averaging these peak values.
	uint16_t peak_value = 0;
	for(uint8_t i = 0; i<samples; i++){
		if (values[i] > peak_value){
			peak_value = values[i];
		}
	}
	return peak_value;
}


/////// ISR ///////

ISR(ADC_vect) { // ADC conversion complete interrupt
	if (!start_sampling) return;
	// sample adc
	// alternate between voltage and current adc
	
	// Store values from most recent conversion
	/*
	uint8_t value_low = ADCL;
	uint16_t value_high = ADCH << 8;
	uint16_t adc_val = value_high | value_low;
	*/
	uint16_t adc_val = ADC;
	
	if (voltage_read){
		// Log the voltage adc sample in array
		voltage[idx] = adc_val;
			
		// Switch to the current sense channel
		ADMUX &= (0xF0); // Lower MUX bits
		ADMUX |= CURRENT_ADC;
		voltage_read = 0;
			
	} else {
		// Store current adc sample in array
		current[idx] = adc_val;
			
		// Switch to voltage sense channel
		ADMUX &= (0xF0); // Lower MUX bits
		ADMUX |= VOLTAGE_ADC; // theory - reading vzc
		voltage_read = 1;
			
		// increment array index
		idx++;
		if (idx == SAMPLES_TO_TAKE){
			idx = 0;
			adc_sample_disable();
		}
	}
}

ISR(INT0_vect){
	adc_sample_enable();
	voltage_read = 1;
	start_sampling = 1;
	EIMSK &= ~(1 << INT0); // Disable (this) INT0 interrupt
	ADMUX = (ADMUX & 0xF0) | VOLTAGE_ADC;
}
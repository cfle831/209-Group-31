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

extern volatile uint8_t adc_samples_full; // From timer.c

/* in order to acheive a *maximum* sampling frequency of 10kHz, samples must be taken ~100us apart */
/* Sampling frequency must not be a multiple of 500Hz*/


void adc_init(void){
	// Enable ADC converter
	ADCSRA = (1<<ADEN);
	ADMUX = (1 << REFS0);
	// Clock prescaler of 16
	ADCSRA |= (1 << ADPS2);
	ADCSRB = 0x00;
	DDRC &= ~(7<<PINC0); // For ADC pins 0, 1, 2 as input
	DDRD &= ~(1<<PIND2); // Zero Crossing Detection pin as input
}

void adc_sample_enable(void){
	adc_samples_full = 0;
	TIMSK0 |= (1<<OCIE0A);
}

void adc_sample_disable(void){
	adc_samples_full = 1;
	TIMSK0 &= ~(1<<OCIE0A); // Disable interrupt
}


// backstep through gain stages of the circuit to find initial value
int32_t find_load_voltage(uint16_t adc_measurement){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Vmeas Opamp: 1
	* 2.12 V offset accurate ---> TODO
	* Input voltage divider: 0.04511
	*/
	
	int32_t return_value = ((int32_t)adc_measurement-430)/0.817; // Finding exact offset voltage is very important for accuracy
	return_value *= 22; //   (0.04511)^-1
	return return_value;
}

int32_t find_load_current(uint16_t adc_measurement){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Imeas Opamp:  2.47177
	* Offset: 2.04 V --> TODO
	* Input voltage divider: V= I * Rshunt so I = V / 0.5021
	*/
	int32_t adc_value = adc_measurement;
	adc_value -= 420;
	
	adc_value *= 5000; // to get pure ac value in mV
	adc_value /= 1024;
	adc_value /= (2.47177*0.817*0.5021);
	return (adc_value);
}

uint16_t adc_read(uint8_t chan){
	// Select channel from input
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= chan;
	
	// Start conversion
	ADCSRA |= (1<<ADSC);
	// Wait
	while(!(ADCSRA  & (1<<ADIF)));
	uint8_t return_value_low = ADCL;
	uint8_t return_value_high = ADCH;
	uint16_t return_value = (((uint16_t)return_value_high << 8) | return_value_low);
	return return_value;
	
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



uint16_t calculate_RMS(const uint16_t values[], uint8_t samples){
	float total_sum = 0.0f;
	for (uint8_t i=0; i<samples; i++){
		total_sum += (uint32_t)values[i]*values[i];
	}
	
	float integration_time = (float)samples;
	
	return (uint16_t)(sqrtf(total_sum/integration_time));
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



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
#include <stdbool.h>

#define estimated_sample_time 0.000104f // Accurate for prescaler 16
#define samples_to_take 40 //  Possibly should be changed 

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

// backstep through gain stages of the circuit to find initial value
uint16_t find_load_voltage(){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Vmeas Opamp: 1
	* Input voltage divider: 0.04511
	*/
	
	
	/* Steps
	* Get value at adc pin in mv
	* remove 2.1 offset (sqrt(rms^2 - 2.1^2)
	* divide by filter gain, then opamp gain, then voltage divider gain
	* return
	*/
	return 0;
}

uint16_t find_load_current_from_adc(uint16_t adc_measurement){ // TODO consider negatives, and that integers cant be smaller than 1
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Imeas Opamp:  2.47177
	* Offset: 2.053 V --> 420
	* Input voltage divider: V= I * Rshunt so I = V / 0.5021
	*/
	int32_t adc_value = adc_measurement;
	adc_value -= 420;
	adc_value *= (5/1024);
	return (adc_measurement / (0.817*2.47177*0.5021)); // may be cut off due to value < 0 as an integer, better to multiply rather than divide
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
uint16_t approximate_step(bool d_backwards, const uint16_t values[], uint8_t index){
	uint16_t return_value;
	if (d_backwards){
		return_value = ((uint32_t)values[index] + values[index - 1])/2;	
	} else{
		return_value = ((uint32_t)values[index] + values[index + 1])/2;
	}
	return return_value;
}

uint32_t calculate_power(const uint16_t voltage[], const uint16_t current[], uint8_t samples){
	uint64_t total_sum_IV = 0.0f;
	for (uint8_t i=0; i<samples; i++){
	total_sum_IV += (uint32_t)(voltage[i]*approximate_step(true, current, i));
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


uint16_t calculate_RMS(const uint16_t values[], uint8_t samples)
{
	uint64_t total_sum = 0;

	for (uint8_t i = 0; i < samples; i++) {
		total_sum += (uint32_t)values[i] * values[i];
	}

	uint32_t mean_square = total_sum/samples;

	return (uint16_t)square_root(mean_square);
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
/*
int main(void)
{
	adc_init();
	usart_init(((F_CPU/((uint32_t)16*BAUD)) - 1));
	uint16_t voltage[samples_to_take];
	uint16_t current[samples_to_take];
	uint8_t index = 0;	
    // Replace with your application code 
	//adc_read(0); // dummy conversion to settle values
    while (1) 
    {
		voltage[index] = adc_convert_mv(adc_read(0));
		current[index] = adc_convert_mv(adc_read(1));
		index++;
		if (index == samples_to_take){
			break;
		}
    }
	transmit_text("Voltage, Current", 16);
	transmit_new_line();
	transmit_excel_two_columns(voltage, current, 40);
	transmit_new_line();
	transmit_new_line();
	transmit_text("RMS V", 5);
	transmit_new_line();
	transmit_string(decimal_to_char(calculate_RMS(voltage, samples_to_take)));
	
}*/



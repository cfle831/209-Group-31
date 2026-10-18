/*
 * adc.c
 *
 * Created: 12/09/2026 1:44:14 pm
 * Author : TFE, Ferre
 */ 
#define F_CPU 2000000UL
#define V_REF 5
#include "adc.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h> //
#include <math.h> //
#include "UART.h"

extern volatile uint8_t adc_samples_full; // From main.c
extern volatile uint8_t start_sampling;
static volatile uint8_t voltage_read = 1;
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
	
	ADMUX &= (0xF0); // Lower MUX bits
	ADMUX |= VOLTAGE_ADC;
	
	ADCSRA |= (1 << ADSC); // dummy conversion to settle values
	while(!(ADCSRA  & (1<<ADIF)));
	// Enable adc interrupts
	ADCSRA |= (1 << ADIE);
}

// backstep through gain stages of the circuit to find initial value
int16_t find_load_voltage(uint16_t adc_measurement){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Vmeas Opamp: 1
	* 2.11 V offset 
	* Input voltage divider: 0.04511
	* Corrective value 0.7109
	*/
	
	int32_t return_value = ((int32_t)adc_measurement-432)/0.817; // Finding exact offset voltage is very important for accuracy
	return_value *= 22; //   (0.04511)^-1
	return_value *= 1000*V_REF;
	return_value /= 1024;
	return return_value;
}

int16_t find_load_current(uint16_t adc_measurement){
	/* Gain values in direction towards measurement pins:
	* 1st order filter: 0.817
	* Imeas Opamp:  2.47177
	* Offset: 2.03 V 
	* Input voltage divider: V= I * Rshunt so I = V / 0.5021
	* Corrective value: 0.8763
	*/
	int32_t return_value = adc_measurement;
	return_value -= 415;
	
	return_value *= 1000*V_REF; // to get pure ac value in mV
	return_value /= 1024;
	return_value /= (2.4718*0.817*0.5021);
	return_value *= 0.8763;
	return (return_value);
}

uint16_t adc_convert_mv(uint16_t adc_count){
	uint32_t value = (uint32_t)adc_count;
	value = (((uint32_t)value*V_REF*1000) / (1023));
	return value;
}

/*
// Edits the input array
void approximate_step_back_all(uint16_t values[], uint8_t samples){
	for (uint8_t i = 1 ; i<samples; i++){
		values[i] = ((values[i] + values[i-1])>>1);
	}
}
*/

// Returns an array in mW
void calculate_inst_power(int16_t voltage[], int16_t current[], uint8_t samples, int32_t array_to_fill[]){
	
	for (uint8_t i = 0; i<samples; i++){
		array_to_fill[i] = (voltage[i] * current[i] / 1000);
	}
	return;
}

// show raw adc values for testing
void print_raw_waveform(void){
	transmit_string("Voltage");
	transmit_new_line();
	for (uint8_t i = 0; i<SAMPLES_TO_TAKE;i++){
		transmit_string(decimal_to_char(voltage[i]));
		transmit_string(" ");
		transmit_string(decimal_to_char(i));
		transmit_new_line();
	}
	
	transmit_string("Current");
	transmit_new_line();
	for(uint8_t i = 0;i<SAMPLES_TO_TAKE;i++){
		transmit_string(decimal_to_char(current[i]));
		transmit_string(" ");
		transmit_string(decimal_to_char(i));
		transmit_new_line();
	}
}

// Calculate rms used for testing 
double calculate_rms(const int16_t *samples, uint8_t count) {
	

	double sum_of_squares = 0.0;

	for (uint8_t i = 0; i < count; i++) {
		double val = (double)samples[i];
		sum_of_squares += val * val;
	}

	return sqrt(sum_of_squares / count);
}




/////// ISR ///////

ISR(ADC_vect) { // ADC conversion complete interrupt
	if (!start_sampling || adc_samples_full) return;
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

		// If the full array was current, change flag
		if (!voltage_read){
			// Switch to voltage next time
			voltage_read = 1;
			ADMUX &= (0xF0); // Lower MUX bits
			ADMUX |= VOLTAGE_ADC;
			// Start power calculations
			adc_samples_full = 1;
		} else {
			// Enable interrupt for current sampling
			voltage_read = 0;
			ADMUX &= (0xF0); // Lower MUX bits
			ADMUX |= CURRENT_ADC;
			EIMSK |= (1 << INT0);
		}
		// Reset the index
		idx = 0;
		// stop sampling until next INT0 interrupt
		start_sampling = 0;
	}
}

ISR(INT0_vect){
	idx = 0;
	start_sampling = 1;
	EIMSK &= ~(1 << INT0); // Disable (this) INT0 interrupt
}
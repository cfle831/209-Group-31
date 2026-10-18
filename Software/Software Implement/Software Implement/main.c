/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre, TFE
 */ 

#define F_CPU 2000000UL
#define V_OFFSET_MEAS_PIN 2

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "adc.h"
#include "timer.h"
#include "display.h"


#include <stdlib.h> // testing
#include <stdint.h> // 
volatile uint8_t adc_samples_full = 0; 
volatile uint8_t start_sampling = 0;
extern volatile uint16_t voltage[SAMPLES_TO_TAKE];
extern volatile uint16_t current[SAMPLES_TO_TAKE];



int main(void)
{
	volatile int16_t input_voltage[SAMPLES_TO_TAKE]; // mV
	volatile int16_t load_current[SAMPLES_TO_TAKE]; // mA
	volatile int16_t instantaneous_power[SAMPLES_TO_TAKE]; // mW
	usart_init(12);
	
	transmit_text("start", 5); //
	transmit_new_line();		//
	
	pwm_init(); // in timer.c
	timer2_init(); // For display updating
	zero_crossing_init(); // in adc.c
	display_init();
	adc_init();
	sei();
	
	
	// The PWM signal is inverted at the shift register
	set_duty_cycle(50); 
	
    while (1) 
    {
		if (adc_samples_full){
			adc_samples_full = 0;
			// Using voltage samples get input voltage
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				input_voltage[i] = find_load_voltage(voltage[i]);
			}
			
			// Approximate half a step back for current values to prevent perceived phase shift
			approximate_step_back_all(current, SAMPLES_TO_TAKE);
			// Using current samples get load current
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				load_current[i] = find_load_current(current[i]);
			}
			// Calculate instantaneous power
			calculate_inst_power(input_voltage, load_current, SAMPLES_TO_TAKE, instantaneous_power);
			
			//  testing
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
			// End of testing 
			
			// Enable zero crossing detection
			EIMSK |= (1 << INT0);
			
			
		} else if(start_sampling) {
			ADCSRA |= (1 << ADSC); // Start a conversion
		}
    }
}


/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre, TFE
 */ 

#define F_CPU 2000000UL
#define AVE_COUNT 20
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
extern volatile int16_t voltage[SAMPLES_TO_TAKE];
extern volatile int16_t current[SAMPLES_TO_TAKE];



volatile uint8_t testing_count = 0;


int main(void)
{

	int32_t instantaneous_power[SAMPLES_TO_TAKE]; // mW
	usart_init(12);

	
	pwm_init(); // in timer.c
	timer0_init();
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
			transmit_string("a");
			// Convert voltage samples to input voltage
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				voltage[i] = find_load_voltage(voltage[i]);
			}
			
			// Convert current samples to load current
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				current[i] = find_load_current(current[i]);
			}
			
			// Calculate instantaneous power
			calculate_instantaneous_power_array(voltage, current, SAMPLES_TO_TAKE, instantaneous_power);
			
			
			// Testing 
			double rms_voltage = (calculate_rms(voltage, SAMPLES_TO_TAKE));
			double rms_current = (calculate_rms(current, SAMPLES_TO_TAKE));
			

			
			int32_t real_power[SAMPLES_TO_TAKE];
			calculate_instantaneous_power_array(voltage, current, SAMPLES_TO_TAKE, real_power);
			//transmit_string(decimal_to_char(real_power));
			//transmit_new_line();
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				transmit_string(decimal_to_char(real_power[i]));
				transmit_string(" ");
				transmit_string(decimal_to_char(i));
				transmit_new_line();
			}
			break;
			
			// End of testing 
			
			TIMSK0 |= (1<<OCIE0A); //enable Display interrupt
			
			_delay_ms(500); // Ensure LED display is on for long enough to read the number

			adc_samples_full = 0;
			EIMSK |= (1 << INT0); // Enable zero crossing detection
			
			
		} 
    }
}


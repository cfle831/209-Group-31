/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre, TFE
 */ 

#define F_CPU 2000000UL
#define BAUD 9600
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
	int32_t real_power_mW = 0;
	int32_t real_power_W = 0;
	

	usart_init(((F_CPU/((uint32_t)16*BAUD)) - 1)); // Initialise UART peripheral
	pwm_init(); // in timer.c, for pwm of LED display
	timer0_init(); // Sampling timer
	timer2_init(); // For display updating
	zero_crossing_init(); // in adc.c, For starting first 
	
	display_init(); 
	adc_init();
	sei();
	
	
	// The PWM signal is inverted at the shift register
	set_duty_cycle(50); 
	
    while (1) 
    {	
		if (adc_samples_full){
			
			// Convert voltage samples to input voltage
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				voltage[i] = find_load_voltage(voltage[i]);
			}
			
			// Convert current samples to load current
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				current[i] = find_load_current(current[i]);
			}
			
			
			
			int16_t rms_voltage = (int16_t)(calculate_rms(voltage, SAMPLES_TO_TAKE));
			//int16_t rms_current = (int16_t)(calculate_rms(current, SAMPLES_TO_TAKE));
			

			
			real_power_mW = calculate_real_power(voltage, current, SAMPLES_TO_TAKE);
			real_power_W = real_power_mW / 1000;
			change_display_value(rms_voltage);
			
			// Transmit Real power via uart
			
			transmit_string("Real Power:");
			transmit_new_line();
			transmit_string(decimal_to_char(real_power_W));
			transmit_string(".");
			transmit_string(decimal_to_char(real_power_mW - real_power_W*1000));
			transmit_string(" W");
			transmit_new_line();
			transmit_string("RMS Voltage:");
			transmit_new_line();
			transmit_string(decimal_to_char(rms_voltage/1000));
			transmit_string(".");
			transmit_string(decimal_to_char(rms_voltage%1000));
			transmit_string("mV");
			transmit_new_line();
			transmit_new_line();
			
			
			
			adc_samples_full = 0;
			EIMSK |= (1 << INT0); // Enable zero crossing detection
			
			
		} 
    }
}


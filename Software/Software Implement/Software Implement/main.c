/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre
 */ 

#define F_CPU 2000000UL
#define SAMPLES_TO_TAKE 40
#define VOLTAGE_MEAS_PIN 0
#define CURRENT_MEAS_PIN 1
#define V_OFFSET_MEAS_PIN 2

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "adc.h"
#include "timer.h"
#include "display.h"

extern volatile uint8_t adc_samples_full; // From timer.c
extern volatile uint16_t voltage[SAMPLES_TO_TAKE];
extern volatile uint16_t current[SAMPLES_TO_TAKE];
volatile int32_t input_voltage[SAMPLES_TO_TAKE];
volatile int32_t load_current[SAMPLES_TO_TAKE];
int main(void)
{
	_delay_ms(1000);
	volatile uint16_t display_value = 1024;
	usart_init(12);
	//transmit_text("start", 5); //
	//transmit_new_line();		//
	pwm_init();
	timer2_init();
	display_init();
	adc_init();
	sei();
	
	// Replace with your application code
	adc_read(0); // dummy conversion to settle values
	
	// The PWM signal is inverted at the shift register
	set_duty_cycle(75); 
	
	
	
	change_display_value(display_value);
    while (1) 
    {
		if (adc_samples_full){
			
			// Using voltage samples get input voltage
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				input_voltage[i] = find_load_voltage(voltage[i]);
			}
			
			// Approximate half a step back for current values to prevent percieved phase shift
			
			// Using current samples get load current
			for (uint8_t i = 0; i<SAMPLES_TO_TAKE; i++){
				load_current[i] = find_load_current(current[i]);
			}
			//calculate_RMS();
			
		}
    }
}


/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre
 */ 

#define F_CPU 2000000UL
#define SAMPLES_TO_TAKE 253
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



volatile uint8_t adc_samples_full = 0;
int main(void)
{
	_delay_ms(1000);
	volatile uint16_t display_value = 1024;
	usart_init(12);
	//transmit_text("start", 5); //
	//transmit_new_line();		//
	pwm_timer1_init();
	timer2_init();
	display_init();
	adc_init();
	sei();
	
	uint16_t voltage[SAMPLES_TO_TAKE];
	uint16_t current[SAMPLES_TO_TAKE];
	uint8_t index = 0;
	// Replace with your application code
	adc_read(0); // dummy conversion to settle values
	set_duty_cycle(50);
	
	
	
	change_display_value(display_value);
    while (1) 
    {
		
		/* LED display */
		
		
		
		/* ADC */
		
		if (!adc_samples_full){
			voltage[index] = adc_convert_mv(adc_read(0));
			current[index] = adc_convert_mv(adc_read(1));
			index++;
			if (index == SAMPLES_TO_TAKE){
				adc_samples_full = 1;
				index = 0;
			}
		} else{
			change_display_value(calculate_RMS(voltage, SAMPLES_TO_TAKE)); // display rms of voltage at pin 
			adc_samples_full = 0;
		}
		
	
		
    }
}


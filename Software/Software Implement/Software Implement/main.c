/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre
 */ 
#define F_CPU 2000000UL
#define SAMPLES_TO_TAKE 40 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "adc.h"
#include "timer.h"
#include "display.h"

volatile uint8_t display_flag = 0;
volatile uint8_t adc_samples_full = 0;
int main(void)
{
	_delay_ms(2000);
	volatile uint16_t display = 1024;
	usart_init(12);
	//transmit_text("start", 5); //
	//transmit_new_line();		//
	timer2_init();
	display_init();
	adc_init();
	sei();
	
	uint16_t voltage[SAMPLES_TO_TAKE];
	uint16_t current[SAMPLES_TO_TAKE];
	uint8_t index = 0;
	// Replace with your application code
	adc_read(0); // dummy conversion to settle values
	
    while (1) 
    {
		//transmit_text("ab", 2);
		//transmit_new_line();
		
		/* LED display */
		
		//_delay_ms(8);
		if (display_flag){
			display_number(display);
			display_flag = 0;
		}
		
		/* ADC */
		
		if (!adc_samples_full){
			voltage[index] = adc_convert_mv(adc_read(0));
			current[index] = adc_convert_mv(adc_read(1));
			index++;
			if (index == SAMPLES_TO_TAKE){
				adc_samples_full = 1;
				index = 0;
			}
		}else{
			if (index < SAMPLES_TO_TAKE){
				
				transmit_string(decimal_to_char(voltage[index]));
				transmit_new_line();
				index++;
			} else{ // TESTING
				transmit_text("----", 4);
				transmit_new_line();
				transmit_string(decimal_to_char(calculate_RMS(voltage, SAMPLES_TO_TAKE)));
				break;
			}
		}
		
	
		
    }
}


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



int main(void)
{
	_delay_ms(1000);
	volatile uint16_t display_value = 1024;
	usart_init(12);
	//transmit_text("start", 5); //
	//transmit_new_line();		//
	//pwm_init();
	timer2_init();
	display_init();
	adc_init();
	sei();
	
	// Replace with your application code
	adc_read(0); // dummy conversion to settle values
	
	// The PWM signal is inverted at the shift register
	//set_duty_cycle(75); 
	
	
	
	change_display_value(display_value);
    while (1) 
    {
		
    }
}


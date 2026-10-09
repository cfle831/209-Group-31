/*
 * Software Implement.c
 *
 * Created: 1/10/2026 3:21:14 pm
 * Author : ferre
 */ 
#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "adc.h"
#include "timer.h"
#include "display.h"

volatile uint8_t display_flag = 0;
int main(void)
{
	volatile uint16_t display = 1024;
	usart_init(12);
	timer2_init();
	display_init();
	adc_init();
	sei();
    /* Replace with your application code */
    while (1) 
    {
		//transmit_text("ab", 2);
		//transmit_new_line();
		
		//_delay_ms(8);
		if (display_flag){
			display_number(display);
			display_flag = 0;
		}
    }
}


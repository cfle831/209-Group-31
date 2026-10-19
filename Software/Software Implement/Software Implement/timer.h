#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// MACRO HELPER
// Writes to 16 bit registers
#define SIXTEEN_BIT_REG_SET(high, low, num) {	\
	high = ((num >> 8) & 0xFF);					\
	low = (num & 0xFF);							\
	}
	
// Initialises PWM registers
void pwm_init();

// Sets the duty cycle of the PWM for the LED display (Brightness)
void set_duty_cycle(uint8_t percentage);

// Timer for LED display
void timer2_init();

// Timer for adc sampling
void timer0_init();

#endif
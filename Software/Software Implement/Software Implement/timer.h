#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include <avr/io.h>

// MACRO HELPER

// Writes to 16 bit registers
#define SIXTEEN_BIT_REG_SET(high, low, num) {	\
	high = ((num >> 8) & 0xFF);					\
	low = (num & 0xFF);							\
	}
	
// Initialises PWM registers
void pwm_timer1_init();

// Changes OCR1B based on the desire duty cycle
void pwm_set_duty_cycle_timer1(uint8_t percentage);
void timer2_init();

#endif
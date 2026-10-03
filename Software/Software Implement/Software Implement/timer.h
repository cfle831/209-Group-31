#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define PWM_PERIOD		20000
#define SIXTEEN_BIT_REG_SET(high, low, num) {	\
	uint8_t sreg = SREG;						\
	cli();										\
	high = ((num >> 8) & 0xFF);					\
	low = (num & 0xFF);							\
	SREG = sreg;								\
	}
	

void pwm_init();
void pwm_set_duty_cycle(uint8_t percentage);

#endif
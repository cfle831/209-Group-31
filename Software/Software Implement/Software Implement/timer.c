#define F_CPU 2000000UL

#include "timer.h"

void pwm_init(void){
	
	DDRB |= (1 << DDB2);
	
	TCCR1A |= (1<<COM1B1) | (1<<WGM11);
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS10);
	// set TOP to 19999, for a PWM frequency of 100hz
	SIXTEEN_BIT_REG_SET(ICR1H, ICR1L, 19999);
	// set Output Compare Register to 10000 for duty cycle of 50%
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, 10000);
}

void pwm_set_duty_cycle(uint8_t percentage){
	if ((percentage > 100) || (percentage < 0)) {
		return;
	}
	uint16_t OC1B = 200*percentage;
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, OC1B);
}
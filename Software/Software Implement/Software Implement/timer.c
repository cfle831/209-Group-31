#define F_CPU 2000000UL

#include "timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
extern volatile uint8_t display_flag;
ISR(TIMER2_COMPA_vect){
	display_flag = 1;
}

void pwm_timer1_init(void){
	
	DDRB |= (1 << DDB2);
	
	TCCR1A |= (1<<COM1B1) | (1<<WGM11);
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS10);
	// set TOP to 19999, for a PWM frequency of 100hz
	SIXTEEN_BIT_REG_SET(ICR1H, ICR1L, 19999);
	// set Output Compare Register to 10000 for duty cycle of 50%
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, 10000);
}

void pwm_set_duty_cycle_timer1(uint8_t percentage){
	if ((percentage > 100) || (percentage < 0)) {
		return;
	}
	uint16_t OC1B = 200*percentage;
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, OC1B);
}



void timer2_init(){
	TCCR2A |= (1<<WGM21);
	TCCR2B |= (1<<CS222); // Prescaler 64 - CS22
	OCR2A = 14; // For ~ 500us should be changed to much longer                                                                                                 
	TCNT2 = 1;
	TIMSK2 |= (1<<OCIE2A);	
}
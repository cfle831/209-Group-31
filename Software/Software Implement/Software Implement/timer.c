#define F_CPU 2000000UL

#include "timer.h"
#include "display.h"
#include "UART.h"

void pwm_init(void){
	
	DDRB |= (1 << PINB2); // OE pin as output
	
	TCCR1A |= (1<<COM1B1) | (1<<WGM11); // Fast PWM, Clear pin on compare match
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS10); // Fast pwm, no prescaler
	// set TOP to 3999, for a PWM frequency of 500hz
	SIXTEEN_BIT_REG_SET(ICR1H, ICR1L, 3999);
	// set Output Compare Register to 2000 for duty cycle of 50%
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, 2000);
}

void set_duty_cycle(uint8_t percentage){
	if (percentage > 100) {
		return;
	}
	uint16_t OC1B = 40*percentage;
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, OC1B);
}



void timer2_init(void){
	TCCR2A |= (1<<WGM21);
	TCCR2B |= (1<<CS22); // Prescaler 64 - CS22
	OCR2A = 124; // For ~ 1ms                                                                                    
	TCNT2 = 0;
	TIMSK2 |= (1<<OCIE2A);	
}

void timer0_init(void){
	TCCR0A |= (1<<WGM01); // set CTC
	TCCR0B |= (1<<CS00); // No prescaler 
	
	OCR0A = 199; // Set compare every 100us, 10khz
	TCNT0 = 0; // reset timer/counter1
	TIMSK0 |= (1<<OCIE0A); //enable compare interrupt
}

/////// ISR ///////

ISR(TIMER2_COMPA_vect){
	update_display();
}
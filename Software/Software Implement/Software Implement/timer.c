#define F_CPU 2000000UL

#include "timer.h"
#include "display.h"
#include "UART.h"

extern uint8_t start_sampling;


/////// ISR ///////

// ISR_NOBLOCK is necessary here in order to prevent the LED update from preventing ADC sampling
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK){
	update_display(); // Turn on next digit of LED display
}

ISR(TIMER0_COMPA_vect){
	if (start_sampling){
		ADCSRA |= (1<<ADSC);
	}
}

void pwm_init(void){
	
	DDRB |= (1 << PINB2); // OE pin as output
	
	TCCR1A |= (1<<COM1B1) | (1<<WGM11); // Fast PWM, Clear pin on compare match
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS10); // Fast pwm, no prescaler

	// set TOP to 3999, for a PWM frequency of 500hz
	SIXTEEN_BIT_REG_SET(ICR1H, ICR1L, 3999);

	// set Output Compare Register to 2000 for duty cycle of 50%
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, 2000);
}

// Sets the duty cycle of the PWM for the LED display (Brightness)
void set_duty_cycle(uint8_t percentage){
	if (percentage > 100) {
		return;
	}
	uint16_t OC1B = 40*percentage;
	SIXTEEN_BIT_REG_SET(OCR1BH, OCR1BL, OC1B);
}



void timer2_init(void){
	TCCR2A |= (1<<WGM21); // Set CTC mode
	TCCR2B |= (1<<CS22); // Prescaler 64 - CS22
	OCR2A = 124; // For ~ 1ms                                                                                    
	TCNT2 = 0; // Reset the count 
	TIMSK2 |= (1<<OCIE2A);	// Enable Compare match interrupt
}

void timer0_init(void){
	TCCR0A |= (1<<WGM01); // Set CTC mode
	TCCR0B |= (1<<CS00); // No prescaler 
	
	OCR0A = 199; // Set compare every 100us, 10khz
	TCNT0 = 0; // reset timer/counter0
	TIMSK0 |= (1<<OCIE0A); // Enable Compare match interrupt
}


#define F_CPU 2000000UL

#include "timer.h"
#include "display.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#define SAMPLES_TO_TAKE 40

volatile uint16_t test = 0;
volatile uint8_t voltage_read = 1;
volatile uint8_t adc_samples_full = 0;
volatile uint16_t voltage[SAMPLES_TO_TAKE];
volatile uint16_t current[SAMPLES_TO_TAKE];
volatile uint8_t index = 0;

ISR(TIMER2_COMPA_vect){
	update_display();
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
	OCR2A = 124; // For ~ 500us                                                                                    
	TCNT2 = 0;
	TIMSK2 |= (1<<OCIE2A);	
}

void timer1_init(void){
	TCCR1B |= (1<<WGM12) | (1<<CS10); // No prescaler and set CTC
	
	SIXTEEN_BIT_REG_SET(OCR1AH, OCR1AL, 199); // Set compare every 100us, 10khz
	SIXTEEN_BIT_REG_SET(TCNT1H, TCNT1L, 0); // reset timer/counter1
	TIMSK1 |= (1<<OCIE1A); //enable compare interrupt
}

ISR(TIMER1_COMPA_vect) {
	// sample adc
	// alternate between voltage and current adc
	if (!adc_samples_full){
		if (voltage_read){
			voltage[index] = adc_read(0);
			voltage_read = 0;
		} else{
			current[index] = adc_read(1);
			voltage_read = 1;
			index++;
			if (index == SAMPLES_TO_TAKE){
				index = 0;
				adc_samples_full = 1;
			}
		}
	}
	
}
/*
 * Display.c
 *
 * Created: 1/10/2026 3:34:34 pm
 * Author : Loelene
 */ 

#define F_CPU 2000000UL
#include <util/delay.h>
#include <stdint.h>
#include "display.h"

// Pin definitions
#define SH_CP (PINC3)
#define SH_DS (PINC4)
#define SH_ST (PINC5)

volatile uint8_t dig_pins[4] = {(1<<PIND4), (1<<PIND5), (1<<PIND6), (1<<PIND7)};
volatile uint8_t segment_0_to_9[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
volatile uint16_t cur_display_value = 0;
void send_next_char(uint8_t value){
	PORTC = 0;
	value = segment_0_to_9[value];
	for (int8_t i = 7; i>=0; i--){
		PORTC |= (((value & (1 << i)) >> i) << SH_DS);
		PINC |= (1<<SH_CP);
		PINC |= (1<<SH_CP);
		PORTC = 0;
	}
	PINC |= (1<<SH_ST);
	PINC |= (1<<SH_ST);
}

void set_seg(uint8_t value, uint8_t dig){
	// Disable All digits
	PORTD = 0xFF;
	send_next_char(value);
	PORTD &= ~(dig_pins[dig - 1]);	
}

void set_seg_all_digits(uint8_t value){
	// Enable All Digits
	PORTD = 0;
	send_next_char(value);
}

void display_number(uint16_t number){ // Takes ~2ms per call
	set_seg((number/1000) % 10,1);
	_delay_us(500);
	set_seg((number/100) % 10,2);
	_delay_us(500);
	set_seg((number/10) % 10,3);
	_delay_us(500);
	set_seg(number % 10,4);
	_delay_us(500);
	disable_seg(4);
}

void disable_seg(uint8_t dig){
	PORTD |= (dig_pins[dig - 1]);
}

void display_init(){ 
	DDRC |= (7<<PINC3); // The SH outputs to display
	DDRD |= (15<<PIND4); // DS outputs 
}

void change_display_value(uint16_t number){
	cur_display_value = number;
}

// Turns on each 7 seg sequentially, one per call
void update_display(){  // POSSIBLE IMPROVEMENT store the differnet values / 10, 1000 etc in different variables so as little maths as possible is occuring 
	static volatile uint8_t cur_dig = 1;
	if (cur_dig == 1){
		set_seg((cur_display_value/1000) % 10,1);
		cur_dig++;
	} else if (cur_dig == 2){
		set_seg((cur_display_value/100) % 10,2);
		cur_dig++;
	} else if (cur_dig == 3) {
		set_seg((cur_display_value/10) % 10,3);
		cur_dig++;
	}else {
		set_seg(cur_display_value % 10,4);
		cur_dig = 1;
	}
}



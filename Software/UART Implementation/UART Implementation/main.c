/*
 * UART Implementation.c
 *
 * Created: 23/08/2026 8:34:48 pm
 * Author : Team 31 - Connor Fleming
 */ 
#include <string.h>

#define F_CPU  2000000UL
#define UPPERLIM 300
#define  BAUD 9600

// Example Values for Power, Current and Voltage
#define rmsVoltage 30
#define peakCurrent 100
#define Power 60


#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



volatile uint16_t checkVal;
void usart_init(uint16_t ubrr){
	
	// Operating Mode - Async
	UCSR0C &= ~((1<<6) | (1<<7));
	// Character Size - 8 bit
	UCSR0B &= ~(1 << UCSZ02);
	UCSR0C |= (1 << UCSZ01);
	UCSR0C |= (1 << UCSZ00);
	// Clock Polarity - Drive low for Async
	UCSR0C &= ~(1 << UCPOL0);
	// Baude Rate of 9600
	
	UBRR0L = (uint8_t)ubrr;
	// Transmitter Enable
	UCSR0B |= (1 << TXEN0);
	// Parity Mode - disabled
	UCSR0C &= ~((1<<UPM00) | (1<<UPM01));
	// Stop Bits - 1
	UCSR0C &= ~(1<<USBS0);
	
	
}

void usart_transmit(uint8_t data){
	// Check if able to transmit / Safe to Load
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
	
}
void transmit_decimal(uint16_t dec_val){
	// Hard Coded for three digit values - May need to be improved
	uint8_t digits[3] = {};
	digits[0] = ((dec_val / 100) % 10);
	digits[1] = ((dec_val / 10) % 10);
	digits[2] = ((dec_val) % 10);
	if (!(digits[0] <=0)) {
		usart_transmit(digits[0] + 48);
		usart_transmit(digits[1] + 48);
		usart_transmit(digits[2] + 48);
		usart_transmit(',');
		usart_transmit(' ');
		} else if (!(digits[1] <=0)){
		usart_transmit(digits[1] + 48);
		usart_transmit(digits[2] + 48);
		usart_transmit(',');
		usart_transmit(' ');
		} else{
		usart_transmit(digits[2] + 48);
		usart_transmit(',');
		usart_transmit(' ');
	}
	
}

volatile char string_3_dig[3] = {};
char* string_from_3_dig(uint16_t dec_val){
	string_3_dig[0] = ((dec_val / 100) % 10) + 48;
	string_3_dig[1] = ((dec_val / 10) % 10) + 48;
	string_3_dig[2] = (dec_val % 10) + 48;
	return string_3_dig;
}

void transmit_string_with_val(char text[], char value[], uint8_t length){
	for (uint8_t i = 0; i<length; i++){
		usart_transmit(text[i]);
		_delay_ms(5);
	}
	for (uint8_t n = 0; n<3; n++){
		usart_transmit(value[n]);
		_delay_ms(5);
	}
}

int main(void)
{
	// Initialize the UART
	usart_init(12);
	volatile uint32_t value = ((F_CPU/(16*BAUD)) - 1);
	while (1)
	{
		
		_delay_ms(1000);
		printf("hello?");
		transmit_string_with_val("RMS Voltage is: ", string_from_3_dig(rmsVoltage), strlen("RMS Voltage is: "));
		usart_transmit('\n');
		usart_transmit('\r');
		transmit_string_with_val("Peak Current is: ", string_from_3_dig(peakCurrent), strlen("Peak Current is: "));
		usart_transmit('\n');
		usart_transmit('\r');
		transmit_string_with_val("Power is: ", string_from_3_dig(Power), strlen("Power is: "));
		usart_transmit('\n');
		usart_transmit('\r');
	}
}


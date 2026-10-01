#define F_CPU 2000000UL

#include <util/delay.h>
#include "UART.h"
#include <string.h>
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

char string_10_dig[11] = {};
char* decimal_to_char(uint16_t value){
	uint32_t divisor = 1000000000;
	// Clear string_10_dig
	memset(string_10_dig, 0, 11);
	string_10_dig[10] = '\0';
	// Convert digits to characters
	for (uint8_t index = 0; index<10; index++){
		string_10_dig[index] = (char)(((value / divisor) % 10)+48);
		divisor /= 10;
	}
	
	// Trim Leading zeros
	// Does not trim if there are no non-zero values
	uint8_t first_dig_index = 0;
	for (uint8_t index = 0; index<10; index++){
		if (string_10_dig[index] != '0'){
			first_dig_index = index;
			break;
		}
	}
	// Rearrange to trim
	if (first_dig_index){
		for (uint8_t index = 0; index<11; index++){
			if ((index + first_dig_index) >=10){
				string_10_dig[index] = '\0';
				
				break;
			} else{
				string_10_dig[index] = string_10_dig[index + first_dig_index];	
			}
		
		}
	}
	
	return string_10_dig;
}
	
 
void usart_transmit(uint8_t data){
	 // Check if able to transmit / Safe to Load
	 while (!(UCSR0A & (1 << UDRE0)));
	 UDR0 = data;
	 
 }
 
 
void transmit_string_with_val(char text[], char value[], uint8_t length){
	 for (uint8_t i = 0; i<length; i++){
		 usart_transmit(text[i]);
		 _delay_ms(5);
	 }
	 for (uint8_t n = 0; n<10; n++){
		if (value[n] == '\0'){
			break;
		}
		 usart_transmit(value[n]);
		 _delay_ms(5);
		  
	 }
 }
 
 void transmit_string(char text[]){
	 for (uint8_t i = 0; i<32; i++){
		 if (text[i] == '\0'){
			 break;
		 }
		 usart_transmit(text[i]);
		 _delay_ms(5);
	 }
 }
 
 void transmit_text(char text[], uint8_t length){
	 for (uint8_t i = 0; i<length; i++){
		 usart_transmit(text[i]);
		 _delay_ms(5);
	 }
 }
 
 void transmit_new_line(){
	 usart_transmit('\n');
	 usart_transmit('\r');
 }
 
 
 void transmit_excel_two_columns(uint16_t col_1[], uint16_t col_2[], uint8_t samples){
	for (uint8_t index = 0; index<samples; index++){
		transmit_string(decimal_to_char(col_1[index]));
		usart_transmit(','); usart_transmit(' '); usart_transmit(9);
		transmit_string(decimal_to_char(col_2[index]));
		transmit_new_line();
	}
 }
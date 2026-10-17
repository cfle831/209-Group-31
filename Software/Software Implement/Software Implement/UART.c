#define F_CPU 2000000UL

#include <util/delay.h>
#include "UART.h"
#include <string.h>
#include <stdlib.h>
void usart_init(uint16_t ubrr){
	
	// Operating Mode - Async
	UCSR0C &= ~((1<<UMSEL00) | (1<<UMSEL01));
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

char string_9_dig[11] = {};
// Converts a decimal number to a string (char array with terminating null character).
// Input can handle up to 9 Digits. 
char* decimal_to_char(int32_t value)
{
    uint32_t magnitude;
    uint8_t pos = 10;

    string_9_dig[10] = '\0';

    // Handle INT32_MIN without signed integer overflow
    if (value < 0) {
        magnitude = (uint32_t)(-(int64_t)value);
    } else {
        magnitude = (uint32_t)value;
    }

    do {
        string_9_dig[--pos] = (char)('0' + magnitude % 10);
        magnitude /= 10;
    } while (magnitude > 0);

    if (value < 0) {
        string_9_dig[--pos] = '-';
    }

    // Shift the result to the start of the buffer
    memmove(string_9_dig, &string_9_dig[pos], 11 - pos);

    return string_9_dig;
}
// Transmit a single character to UART
void usart_transmit(uint8_t data){
	 // Check if able to transmit / Safe to Load
	 while (!(UCSR0A & (1 << UDRE0)));
	 UDR0 = data;
	 
 }
 
/* Transmits a string to the UART. Requires a null character Ending the string
*/
 void transmit_string(char text[]){
	 for (uint8_t i = 0; i<32; i++){
		 if (text[i] == '\0'){
			 break;
		 }
		 usart_transmit(text[i]);
		 _delay_ms(5);
	 }
	 
 }
 /* Transmits a char array to the UART. Requires the length of the string 
 */
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
 
// Transmits two columns of data to UART
 void transmit_excel_two_columns(uint16_t col_1[], uint16_t col_2[], uint8_t samples){
	for (uint8_t index = 0; index<samples; index++){
		transmit_string(decimal_to_char(col_1[index]));
		usart_transmit(','); usart_transmit(' '); usart_transmit(9);
		transmit_string(decimal_to_char(col_2[index]));
		transmit_new_line();
	}
 }
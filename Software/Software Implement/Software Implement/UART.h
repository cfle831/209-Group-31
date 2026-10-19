 
 
#ifndef _UART_H
#define _UART_H

#include <stdint.h>
#include <avr/io.h>

#define BAUD 9600

void usart_init(uint16_t ubrr);
/* Converts a decimal number to a string (char array with terminating null character).
* Input can handle up to 9 Digits. */
char* decimal_to_char(int32_t value);

void transmit_new_line();
// Transmits a string to the UART. Requires a null character termination in the array 
void transmit_string(char text[]);

#endif
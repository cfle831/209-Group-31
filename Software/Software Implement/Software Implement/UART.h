 
 
#ifndef _UART_H
#define _UART_H

#include <stdint.h>
#include <avr/io.h>

#define BAUD 9600
 
void usart_init(uint16_t ubrr);
char* decimal_to_char(uint16_t value);
void usart_transmit(uint8_t data);
void transmit_string_with_val(char text[], char value[], uint8_t length);
void transmit_new_line();
void transmit_string(char text[]);
void transmit_excel_two_columns(uint16_t col_1[], uint16_t col_2[], uint8_t samples);
 void transmit_text(char text[], uint8_t length);

#endif
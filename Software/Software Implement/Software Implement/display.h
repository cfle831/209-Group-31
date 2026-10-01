 
 
#ifndef _DISPLAY_H
#define _DISPLAY_H


#include <avr/io.h>

#define BAUD 9600
void display_init();
void display_number(uint16_t number);

#endif
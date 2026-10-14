 
 
#ifndef _DISPLAY_H
#define _DISPLAY_H


#include <avr/io.h>

void display_init();
void display_number(uint16_t number);
void change_display_value(uint16_t number);
void disable_seg(uint8_t dig);
void update_display();

#endif
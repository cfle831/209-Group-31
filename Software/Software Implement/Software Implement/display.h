 
 
#ifndef _DISPLAY_H
#define _DISPLAY_H


#include <avr/io.h>
// Initialise registers for the display
void display_init();
// Out-dated function that uses _delay to display all digits of the number inputted
void display_number(uint16_t number);
// Changes the value that will be displayed on the LED display
void change_display_value(uint16_t number);
// Turns off a dig of the LED display
void disable_seg(uint8_t dig);
// Sequentially activates 1 Dig of the LED display, one per call
void update_display();

#endif
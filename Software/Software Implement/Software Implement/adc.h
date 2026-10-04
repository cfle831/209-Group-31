 
 
#ifndef _ADC_H
#define _ADC_H


#include <avr/io.h>


uint32_t calculate_power(uint16_t voltage[], uint16_t current[], uint8_t samples);
float calculate_RMS(uint16_t values[], uint8_t samples);

#endif
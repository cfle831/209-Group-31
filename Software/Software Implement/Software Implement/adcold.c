#include "adc.h"
#include "global.h"


void adc_init(void){
	// Enable ADC converter
	ADCSRA = (1<<ADEN);
	ADMUX = (1 << REFS0);
	// Clock prescaler of 16
	ADCSRA |= (1 << ADPS2);
	ADCSRB = 0x00;
}

uint16_t adc_read(uint8_t chan){
	// Select channel from input
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= chan;
	
	// Start conversion
	ADCSRA |= (1<<ADSC);
	// Wait
	while(!(ADCSRA  & (1<<ADIF)));
	uint8_t return_value_low = ADCL;
	uint8_t return_value_high = ADCH;
	uint16_t return_value = (((uint16_t)return_value_high << 8) | return_value_low);
	return return_value;
	
}

uint16_t adc_convert_mv(uint16_t adc_count){
	uint32_t value = (uint32_t)adc_count;
	value = (((uint32_t)value*V_REF*1000) / (1023));
	return value;
}


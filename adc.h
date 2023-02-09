/* Analog to Digital converter
 *
 * Initialize using adc_init
 * Uses one pin from PORTC (ADC channel)
 * If other pins are needed feel free to run `DDRC = ...` after adc_init()
 * 
 */

#ifndef ADC_H__
#define ADC_H__

#include <avr/io.h>
#include <xc.h>

// Must be used ONLY when ADC interrupts are disabled
uint16_t raw_adc_data(void) {
    ADCSRA |= 1 << ADSC;        //start ADC conversion    
    while((ADCSRA >> ADSC)&1);  //wait for it to finish
    return ADC;
}

// Initializes to `trigger_source` mode on channel `adc_channel` (0-7 for PC0-7)
// with interrupts enabled or disabled
void adc_init(char adc_channel, char int_enabled, char trigger_source) {
    // initialize ADC
    DDRC = 0;                           //PORTC is input
    ADMUX = 0b01000000 + adc_channel;   //ADC channel `adc_channel`
    ADCSRA = 0b10000111;                //prescaler 128
    ADCSRB = trigger_source;            //trigger_source
    if (int_enabled) ADCSRA |= 1<<ADIE; //enable interrupts if the user wants it
}

#endif /* adc.h */
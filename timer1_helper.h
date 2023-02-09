/* Timer Helper
 *
 * Initialize using 
 * Pins:
 * 
 */

#ifndef TIMER_HELPER_H__
#define TIMER_HELPER_H__

#include <avr/io.h>
#include <util/delay.h>
#include <xc.h>

void simple_timer1_init() {
    // enable timer overflow interrupt
    TIMSK1 = 1 << TOIE1;
    TCCR1B = 0;
}

//
void pwm_timer1_init() {
    TCCR1A = (1<<WGM10) | (1<<COM1A1);
    TCCR1B = (1<<WGM12) | (1<<CS11);
}



#endif /* timer_helper.h */
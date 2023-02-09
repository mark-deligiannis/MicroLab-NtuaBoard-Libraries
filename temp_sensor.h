/* TEMPERATURE SENSOR
 *
 * No init needed
 * Uses PORTD (bit PD4)
 * 
 */

#ifndef TEMP_SENSOR_H__
#define TEMP_SENSOR_H__

#include <avr/io.h>
#include <xc.h>
#include <util/delay.h>

char one_wire_reset() {
    DDRD = (1 << PD4);      //set PD4 as output
    PORTD &= ~(1 << PD4);   //clear bit PD4
    _delay_us(480);

    DDRD &= ~(1 << PD4);    //set PD4 as input
    PORTD &= ~(1 << PD4);   //disable pull-up
    _delay_us(100);

    unsigned char data = PIND;            //read PORTD
    _delay_us(380);        

    if (data & 0x10) return 0;     //no device detected           
    return 1;               //device detected
}

unsigned char one_wire_receive_bit() {
    DDRD = (1 << PD4);      //set PD4 as output
    PORTD &= ~(1 << PD4);   //clear bit PD4
    _delay_us(2);

    DDRD &= ~(1 << PD4);    //set PD4 as input
    PORTD &= ~(1 << PD4);   //disable pull-up
    _delay_us(10);

    unsigned char data = PIND;            //read PORTD
    _delay_us(49);
    return data;
}

void one_wire_transmit_bit(unsigned char data) {          //data must be 0x01 or 0x00
    DDRD = (1 << PD4);      //set PD4 as output
    PORTD &= ~(1 << PD4);   //clear bit PD4
    _delay_us(2);

    PORTD = (data << PD4);  //write data to PD4
    _delay_us(58);

    DDRD &= ~(1 << PD4);    //set PD4 as input
    PORTD &= ~(1 << PD4);   //disable pull-up
    _delay_us(1);
}

unsigned char one_wire_receive_byte() {
    unsigned char data_byte = 0, data;
    for(int i=0; i<8; i++) {
        data = one_wire_receive_bit();         //receive bit in data variable
        data = (data >> PD4)&1;
        data_byte |= data << i;              //logical or with data_byte
    }
    return data_byte;
}

void one_wire_transmit_byte(unsigned char data_byte) {
    for(int i=0; i<8; ++i) {    
        one_wire_transmit_bit(data_byte & 1);        
        data_byte = (data_byte >> 1);   //next bit becomes LSB
    }
}

uint16_t take_measurement() {                //the result will be returned as one
                                            // 16-bit result variable                                          

    if (!(one_wire_reset())) return 0x8000;     //initialize and check if device is connected
                                                //no device => return 0x8000

                                                //transmit to all devices
    one_wire_transmit_byte(0xCC);               //(only one device is connected anyway)
    one_wire_transmit_byte(0x44);               //start measurement

    while(!one_wire_receive_bit());              //wait for confirmation bit
    
    if (!(one_wire_reset())) return 0x8000; //reset device

    one_wire_transmit_byte(0xCC);               //transmit to all devices
    one_wire_transmit_byte(0xBE);               //read scratchpad (from LSB to MSB)
    _delay_ms(750);
    
    uint16_t high, low;
    
    low = one_wire_receive_byte();                //read LSB
    high = one_wire_receive_byte();               //read MSB

    return high << 8 | low;           //combine them into one 16-bit variable
}

#endif /* temp_sensor.h */
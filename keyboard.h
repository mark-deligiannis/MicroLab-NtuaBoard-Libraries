#ifndef KEYBOARD_H__
#define KEYBOARD_H__

#include "twi.h"

// scans row specified by user
// MUST be called with row values: 1,2,3,4 (bottom -> up)
// outputs whatever it reads with the 4 MSBs zeroed
unsigned char scan_row(unsigned char row)
{
    PCA9555_0_write(REG_OUTPUT_1, ~(1<<(row-1)));  
                                //sends 0111, 1011, 1101 or 
                                //1110 depending on the line we want to scan
    _delay_us(ROW_RD_DEL);
    return (PCA9555_0_read(REG_INPUT_1) >> 4) & 0x0F;   
                                //returns the state of
                                //the row buttons, 0=>pressed, 1=>not pressed
}

// scans whole keyboard
struct buttons scan_keypad(void)
{
    struct buttons but;        
    unsigned char row;
    for (row=1; row<5; ++row) but.b[row-1] = scan_row(row);
                                //for each row call scan_row() once
                                //and return the result as an array at the end
    return but;
}

unsigned char scan_keypad_rising_edge(void)
{
    unsigned char *before, *after, result;
    int i,j, ret;
    
    // get current state
    before = current_state.b;
    // delay
    _delay_ms(SCAN_KEYP_DEL);
    // get next state
    after  = scan_keypad().b;
    
    for (i=0; i<4; ++i)             //for every element corresponding to one row
    {
        result = before[i] & (~after[i]);   //if a button was not pressed before
                                            //and is pressed now (rising edge),
                                            //a 1 will be written 
        for (j=0; j<4; ++j)                 //checking bits (buttons) one by one
        {
            if (result&1) { ret = i*4+j+1; goto end; }
                                            //if a rising edge is found return
                                            //the corresponding character's position
                                            //on the character table
            result >>= 1;   //shift to check next bit
        }
    }
    ret = 0;                //if no rising edge is found return 0
    
    end:
    // update state
    for (i=0; i<4; ++i)
        current_state.b[i] = after[i];
    
    return ret;
}
        
unsigned char keypad_to_ascii(void)
{
    unsigned char temp = scan_keypad_rising_edge();
    if (temp) return char_table[temp-1];        //if a rising edge is found (not 0)
                                                //return the character corresponding
                                                //to this table position
    return 0;
}

// // This function causes a delay of ms' milliseconds where ms' is the closest
// // value to ms such that ms'> ms and ms'=multiple of SCAN_KEYP_DEL
// // This function also reads from the keypad every SCAN_KEYP_DEL and updates
// // the state variable accordingly
// void delay_ms_and_poll_keypad(int ms) {
//     for (int i=0; i<ms; i+=SCAN_KEYP_DEL) {
//         key = keypad_to_ascii();
//         if (key=='1') state = 1;
//         else if (key=='#') state = 0;
//     }
// }

#endif /* keyboard.h */
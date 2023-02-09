/* LCD
 *
 * Initialize with lcd_full_init
 * Can be configured to either use PORTD or the port expander
 * `#define LCD_MODE` to 0 for PORTD and 1 for TWI. Default is PORTD
 * 
 * Added `lcd_full_init` and `clear_and_reset_cursor`
 * 
 */


#ifndef LCD_H__
#define LCD_H__

#include <avr/io.h>
#include <xc.h>
#include <util/delay.h>
#include "twi.h"

#if LCD_MODE 1
#define LCD_PRIM_WRITE(X) ( PCA9555_0_write(REG_OUTPUT_0, X) )
#define LCD_PRIM_READ() ( PCA9555_0_read(0) )
#define LCD_LOGIC_OR(X) ( PCA9555_0_write_or(X) )
#define LCD_LOGIC_AND(X) ( PCA9555_0_write_and(X) )
#else 
#define LCD_PRIM_WRITE(X) ( PORTD = X )
#define LCD_PRIM_READ() ( PIND )
#define LCD_LOGIC_OR(X) ( PORTD |= X )
#define LCD_LOGIC_AND(X) ( PORTD &= X )
#endif


void write_2_nibbles (unsigned char message) {
    LCD_PRIM_WRITE((LCD_PRIM_READ() & 0x0f) | (message&0xf0));
    LCD_LOGIC_OR(8);
    asm("nop");
    asm("nop");
    LCD_LOGIC_AND(~8);
    LCD_PRIM_WRITE((LCD_PRIM_READ() & 0x0f) | ((message&0x0f) << 4));
    LCD_LOGIC_OR(8);
    asm("nop");
    asm("nop");
    LCD_LOGIC_AND(~8);
}

void lcd_data (unsigned char data) {
    LCD_LOGIC_OR(4);
    write_2_nibbles(data);
    _delay_us(43);
}

void lcd_command(unsigned char data) {
    LCD_LOGIC_AND(~4);
    write_2_nibbles(data);
    _delay_us(39);
}

void lcd_init() {
    _delay_ms(40);
    
    LCD_PRIM_WRITE(0x30);
    LCD_LOGIC_OR(8);
    asm("nop");
    asm("nop");
    LCD_LOGIC_AND(~8);
    _delay_us(39);

    LCD_PRIM_WRITE(0x30);
    LCD_LOGIC_OR(8);
    asm("nop");
    asm("nop");
    LCD_LOGIC_AND(~8);
    _delay_us(39);
    

    LCD_PRIM_WRITE(0x20);
    LCD_LOGIC_OR(8);
    asm("nop");
    asm("nop");
    LCD_LOGIC_AND(~8);
    _delay_us(39);
    
    lcd_command(0x28);
    lcd_command(0x0c);
    lcd_command(0x01);
    _delay_ms(2);
    lcd_command(0x06);
    _delay_ms(2);
}

void lcd_print(const char* arr) {
    for (int i=0; arr[i]!='\0'; ++i) {
        lcd_data(arr[i]);
        _delay_ms(2);
    }
}

void clear_and_reset_cursor() {
    // Prepare LCD
    lcd_command(0x01);  //clear screen
    _delay_ms(2);
    lcd_command(0x80);  //reset DDRAM to type to line 1
    _delay_us(100);
}

// Full initialization of LCD to 2-line mode, no extra commands needed
void lcd_full_init (char init_lcd) {
    // If we do the job via the port expander initialize twi
    if (LCD_MODE == 1) {
        if (init_lcd) twi_init();
        PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // Set EXT_PORT0 as output
    }
    // Initialize LCD
    lcd_init();
    _delay_ms(2);
    lcd_command(0x28);  //set LCD to 2-line mode
    _delay_us(500);

    // Clear and set cursor
    clear_and_reset_cursor();
}

#endif /* lcd.h */
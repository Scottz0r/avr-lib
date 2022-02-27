#include <avr/io.h>
#include "fast_lcd_prj.h"

#define _set_bit(r, f) (r |= (1 << f))
#define _set_bit_if(r, f, con) (r |= ((con) ? 1 : 0) << f)
#define clear_data() (PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5)))

#define DATA_PIN_MASK (0xf << FLC_DATA_BIT_SHIFT) 

// Initialize pins and data direction.
void fast_lcd_prj_init_data_pins()
{
    // Set data pints as outputs
    /*
    _set_bit(FLC_DATA_DDR, PD2);
    _set_bit(FLC_DATA_DDR, PD3);
    _set_bit(FLC_DATA_DDR, PD4);
    _set_bit(FLC_DATA_DDR, PD5);
    */
    
    FLC_DATA_DDR |= DATA_PIN_MASK;
}

// Set data pins to the given nibble.
void fast_lcd_prj_set_data_pins(char nibble)
{
    // Slower method that sets pins individually.
    /*
    clear_data();
    _set_bit_if(PORTD, PD2, nibble & 0x01);
    _set_bit_if(PORTD, PD3, nibble & 0x02);
    _set_bit_if(PORTD, PD4, nibble & 0x04);
    _set_bit_if(PORTD, PD5, nibble & 0x08);
    */

    // Faster, set in a few instructions. This only works when the data pins are on sequential pins.
    FLC_DATA_PORT &= ~(0x0f << 2);
    char write_val = (nibble & 0xf) << FLC_DATA_BIT_SHIFT;
    FLC_DATA_PORT |= write_val;
}

char fast_lcd_prj_read_data_pins()
{
    // Read pins.
    char data = (FLC_DATA_PORT >> FLC_DATA_BIT_SHIFT) & 0x0f;
    return data;
}

void fast_lcd_prj_pin_input()
{
    // Switch data pins to input (set to 0 on AVR). Must set DDR of pins to low to ensure pull ups are _not_ enabled.
    FLC_DATA_PORT &= ~(DATA_PIN_MASK);
    FLC_DATA_DDR &= ~(DATA_PIN_MASK);
}

void fast_lcd_prj_pin_output()
{
    // Switch data direction back to output.
    FLC_DATA_DDR |= DATA_PIN_MASK;
}

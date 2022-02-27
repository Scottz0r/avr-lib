#ifndef _FAST_LCD_PRJ_H_INCLUDE_GUARD
#define _FAST_LCD_PRJ_H_INCLUDE_GUARD

#include "hwcfg.h"
#include <avr/io.h>
#include <util/delay.h>

// Amount of time to wait to initialize display after powerup.
#define LCD_POWERUP_DELAY_MS 50

// Maximum number of times to try a busy check before stopping.
#define LCD_MAX_BUSY_TRY 100

// Amount of time to delay when waiting for a busy read.
#define FLC_BUSY_DELAY_US 5

#define FLC_RS_PORT PORTB
#define FLC_RS_DDR DDRB
#define FLC_RS_PIN PB0

#define FLC_EN_PORT PORTB
#define FLC_EN_DDR DDRB
#define FLC_EN_PIN PB1

#define FLC_RW_PORT PORTD
#define FLC_RW_DDR DDRD
#define FLC_RW_PIN PD7

#define FLC_DATA_PORT PORTD
#define FLC_DATA_DDR DDRD
#define FLC_DATA_BIT_SHIFT 2

// Project specific hardware interface:

// Set data pins to the given nibble.
void fast_lcd_prj_set_data_pins(char nibble);

// Read data from data pins.
char fast_lcd_prj_read_data_pins();

// Set data pins to inputs.
void fast_lcd_prj_pin_input();

// Set data pins to outputs.
void fast_lcd_prj_pin_output();

// Wrapper around hardward-specific delay (microsecond)
#define fast_lcd_prj_delay_us(amt) _delay_us(amt)

// Wrapper around hardward-specific delay (millisecond)
#define fast_lcd_prj_delay_ms(amt) _delay_ms(amt)

#endif // _FAST_LCD_PRJ_H_INCLUDE_GUARD

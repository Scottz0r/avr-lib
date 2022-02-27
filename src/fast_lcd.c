#include "fast_lcd.h"
#include "fast_lcd_prj.h"

#define _set_bit(r, f) (r |= (1 << f))
#define _clear_bit(r, f) (r &= ~(1 << f))

#define read_bit(r, f) ((r & (1 << f)) ? 1 : 0)

// Mode for sending commands or data to the LCD.
#define MODE_DATA 1
#define MODE_CMD 0

// Commands and data bits
#define LCD_FUNCTIONSET 0x20
#define LCD_2LINE 0x08

#define LCD_DISPLAYCONTROL 0x08
#define LCD_DISPLAYON 0x04

#define LCD_CLEARDISPLAY 0x01

#define LCD_ENTRYMODESET 0x04
#define LCD_ENTRYLEFT 0x02

// DRAM address in bits 0 through 6.
#define LCD_SETDDRAMADDR 0x80

static void lcd_send(char value, char mode);
static void lcd_send_4_bits(char value, char mode);
static void lcd_wait_busy();
static int lcd_read_busy();

void lcd_init()
{
    _set_bit(FLC_RS_DDR, FLC_RS_PIN);
    _set_bit(FLC_EN_DDR, FLC_EN_PIN);
    _set_bit(FLC_RW_DDR, FLC_RW_PIN);

    // Set data pins to outputs.
    fast_lcd_prj_pin_output();

    // Pull enable and RS low for initialization.
    _clear_bit(FLC_EN_PORT, FLC_EN_PIN);
    _clear_bit(FLC_RS_PORT, FLC_RS_PIN);
    _clear_bit(FLC_RW_PORT, FLC_RW_PIN);

    // Clean data on pins.
    fast_lcd_prj_set_data_pins(0);

    fast_lcd_prj_delay_ms(LCD_POWERUP_DELAY_MS);

    // This is what data sheet says to do for startup, including 40us delay between setup calls.
    lcd_send_4_bits(0x03, MODE_CMD);
    fast_lcd_prj_delay_us(40);
    lcd_send(0x02, MODE_CMD);
    fast_lcd_prj_delay_us(40);
    lcd_send(0x02, MODE_CMD);
    fast_lcd_prj_delay_us(40);

    // Set to 2 line mode.
    lcd_wait_busy();
    lcd_send((LCD_FUNCTIONSET | LCD_2LINE), MODE_CMD);

    // Turn display on.
    lcd_wait_busy();
    lcd_send((LCD_DISPLAYCONTROL | LCD_DISPLAYON ), MODE_CMD);

    // Clear display. This one takes a while to recover from.
    lcd_wait_busy();
    lcd_send(LCD_CLEARDISPLAY, MODE_CMD);
    fast_lcd_prj_delay_ms(2); // 1.52 ms delay when clearing the display.

    // Set text left to right.
    lcd_wait_busy();
    lcd_send(LCD_ENTRYMODESET | LCD_ENTRYLEFT, MODE_CMD);
}

void lcd_clear()
{
    lcd_wait_busy();
    lcd_send(LCD_CLEARDISPLAY, MODE_CMD);
}

void lcd_set_cursor(unsigned char row, unsigned char col)
{
    if (row < 2 && col < 0x27)
    {
        lcd_wait_busy();
        lcd_addr_t full_addr = (row * 0x40) + col;
        lcd_send(LCD_SETDDRAMADDR | full_addr, MODE_CMD);
    }
}

void lcd_write(char c)
{
    lcd_wait_busy();
    lcd_send(c, MODE_DATA);
}

void lcd_write_str(const char* str)
{
    const char *p = str;
    while(*p != 0)
    {
        lcd_wait_busy();
        lcd_send(*p, MODE_DATA);
        ++p;
    }    
}

static void lcd_send(char value, char mode)
{
    // Send MSB first
    lcd_send_4_bits(value >> 4, mode);
    lcd_send_4_bits(value, mode);
}

static void lcd_send_4_bits(char value, char mode)
{
    // Set register select based on mode.
    if (mode)
    {
        _set_bit(FLC_RS_PORT, FLC_RS_PIN);
    }
    else
    {
        _clear_bit(FLC_RS_PORT, FLC_RS_PIN);
    }

    // Project specific function to set data line.
    fast_lcd_prj_set_data_pins(value);

    // Start with enable low.
    _clear_bit(FLC_EN_PORT, FLC_EN_PIN);

    // Set up data before pulse.
    fast_lcd_prj_delay_us(1);

    // Toggle enable high to trigger read. Has 40ns required set up time and 10ns hold time. 1us should be enough
    // to transmit data.
    _set_bit(FLC_EN_PORT, FLC_EN_PIN);
    fast_lcd_prj_delay_us(1);
    
    // Pull enable low to stop transmission.
    _clear_bit(FLC_EN_PORT, FLC_EN_PIN);

    // Must have 1.2 seconds between enable cycles. Wait this out here, which is not very efficient.
    // The datsheet calls this out in the timing diagram.
    fast_lcd_prj_delay_ms(1);
}

static void lcd_wait_busy()
{
    int is_busy = lcd_read_busy();

    // Put an upper bound on the loop to not lock up MCU if the LCD never returns a non-busy state.
    unsigned int counter = 0;

    while(is_busy && counter < LCD_MAX_BUSY_TRY)
    {
        // Wait a tiny delay between reads of busy flag.
        fast_lcd_prj_delay_us(FLC_BUSY_DELAY_US);
        is_busy = lcd_read_busy();
        ++counter;
    }
}

static int lcd_read_busy()
{
    fast_lcd_prj_pin_input();
    fast_lcd_prj_delay_us(1);

    _clear_bit(FLC_EN_PORT, FLC_EN_PIN);
    _clear_bit(FLC_RS_PORT, FLC_RS_PIN); // Set RS to 0
    _set_bit(FLC_RW_PORT, FLC_RW_PIN); // Set RW to 1

    _set_bit(FLC_EN_PORT, FLC_EN_PIN);
    fast_lcd_prj_delay_us(1);
    
    char msb = fast_lcd_prj_read_data_pins();

    _clear_bit(FLC_EN_PORT, FLC_EN_PIN);
    fast_lcd_prj_delay_us(2);

    // Pulse another time to read the lower bits. Ignore these because the busy flag is in the MSB.
    _set_bit(FLC_EN_PORT, FLC_EN_PIN);
    fast_lcd_prj_delay_us(1);

    fast_lcd_prj_read_data_pins();

    _clear_bit(FLC_EN_PORT, FLC_EN_PIN);

    // Reset state
    _clear_bit(FLC_RW_PORT, FLC_RW_PIN);
    fast_lcd_prj_pin_output();

    return (msb & 0x08);
}

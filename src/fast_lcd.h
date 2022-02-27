#ifndef _FAST_LCD_INCLUDE_GUARD
#define _FAST_LCD_INCLUDE_GUARD

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char lcd_addr_t;

void lcd_init();

void lcd_clear();

void lcd_set_cursor(lcd_addr_t row, lcd_addr_t col);

void lcd_write(char c);

void lcd_write_str(const char* str);

#ifdef __cplusplus
}
#endif

#endif // _FAST_LCD_INCLUDE_GUARD

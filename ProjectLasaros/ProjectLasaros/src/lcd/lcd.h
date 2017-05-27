/*
 * lcd.h
 *
 * This file contains subroutines for communicating with a Hitachi HD44780
 * LCD controller (or compatible circuit).
 *
 * Author:	Mathias Beckius
 *
 * Date:	2014-11-28
 */ 

#ifndef LCD_H_
#define LCD_H_

#include <inttypes.h>

/* Address of hardware registers that control the LCD */
#define PIO_PER_PIOC    0x400E1200U  /* PIO Controller PIO Enable Register of PIOC */
#define PIO_OER_PIOC    0x400E1210U  /* PIO Controller Output Enable Register of PIOC */
#define PIO_SODR_PIOC   0x400E1230U  /* PIO Controller Set Output Data Register of PIOC */
#define PIO_CODR_PIOC   0x400E1234U  /* PIO Controller Clear Output Data Register of PIOC */

#define PIO_PER_PIOD    0x400E1400U  /* PIO Controller PIO Enable Register of PIOD */
#define PIO_OER_PIOD    0x400E1410U  /* PIO Controller Output Enable Register of PIOD */
#define PIO_SODR_PIOD   0x400E1430U  /* PIO Controller Set Output Data Register of PIOD */
#define PIO_CODR_PIOD   0x400E1434U  /* PIO Controller Clear Output Data Register of PIOD */

#define PIO_WPMR_PIOC   0x400E12E4U  /* PIO Write Protect Mode Register of PIOC */
#define PIO_WPMR_PIOD   0x400E14E4U  /* PIO Write Protect Mode Register of PIOD */
#define WPKEY           (uint32_t) 0x50494F

/*
 * To be used with lcd_write()
 */
enum lcd_register
{
	INSTRUCTION,	// to write data to the Instruction Register
	DATA			// to write data (characters) to the Data Register
};

/*
 * To be used with lcd_set_cursor_mode()
 */
enum lcd_cursor
{
	CURSOR_OFF		= 0x0C,     // Cursor off
	CURSOR_ON		= 0x0E,		// UPPGIFT: ändra värdet! Cursor on, but not blinking
	CURSOR_BLINK	= 0x0F	    // UPPGIFT: ändra värdet! Cursor on, blinking
};

void lcd_init(void);
void lcd_write(enum lcd_register, uint8_t);
void lcd_write_str(char *);
void lcd_clear(void);
void lcd_set_cursor_mode(enum lcd_cursor);
void lcd_set_cursor_pos(uint8_t, uint8_t);
void display_ascii_nbr(uint32_t value);

#endif /* LCD_H_ */
/*
 * lcd.c
 *
 * This file contains subroutines for communicating with a Hitachi HD44780
 * LCD controller (or compatible circuit).
 *
 * Author:	Danial Mahmoud
 *
 * Date:	2016-06-08
 */

#include <asf.h>
#include <inttypes.h>
#include "lcd.h"
#include "common.h"   

/* Pointers to access hardware register */
uint32_t *const p_PIO_PER_PIOC = (uint32_t *) PIO_PER_PIOC;  
uint32_t *const p_PIO_OER_PIOC = (uint32_t *) PIO_OER_PIOC;
uint32_t *const p_PIO_SODR_PIOC = (uint32_t *) PIO_SODR_PIOC;
uint32_t *const p_PIO_CODR_PIOC = (uint32_t *) PIO_CODR_PIOC;

uint32_t *const p_PIO_PER_PIOD = (uint32_t *) PIO_PER_PIOD;
uint32_t *const p_PIO_OER_PIOD = (uint32_t *) PIO_OER_PIOD;
uint32_t *const p_PIO_SODR_PIOD = (uint32_t *) PIO_SODR_PIOD;
uint32_t *const p_PIO_CODR_PIOD = (uint32_t *) PIO_CODR_PIOD;

uint32_t *const p_PIO_WPMR_PIOC = (uint32_t *) PIO_WPMR_PIOC;
uint32_t *const p_PIO_WPMR_PIOD = (uint32_t *) PIO_WPMR_PIOD;

static void write_4bit_msb(uint8_t);

/*
 * Write the 4 MSB's of 'data' to the LCD.
 *
 * parameter:
 *	data: 4 bits of data
 */
static void write_4bit_msb(uint8_t data)
{
	//write data to LCD
	for (int i = 0; i < 8; i++)
	{
		if(((1<<i) & data) == 0)
		{
			*p_PIO_CODR_PIOC = (1<<i);
		}
		else
		{
			*p_PIO_SODR_PIOC = (1<<i);
		}
	}
		
	// generate pulse on the Enable pin
	SET_BIT(*p_PIO_SODR_PIOD, 7); 
	delay_us(1);
	CLR_BIT(*p_PIO_CODR_PIOD, 7);
}

/*
 * Initialization of the LCD:
 *	- configuration of ports and pins
 *	- configuration of LCD communication
 */
void lcd_init(void)
{
	/* Disable Write Protect */
	*p_PIO_WPMR_PIOC = WPKEY;
	*p_PIO_WPMR_PIOC &= ~(1<<0);
	*p_PIO_WPMR_PIOD = WPKEY;
	*p_PIO_WPMR_PIOD &= ~(1<<0);
	// PIOC4-PIOC7 (D36-D39) as outputs (LCD D4-D7)
	*p_PIO_PER_PIOC |= 0xF0; 
	*p_PIO_OER_PIOC |= 0xF0;
	// PIOD6 & PIOD7 (D29 & D11) as outputs (LCD RS & LCD E)
	*p_PIO_PER_PIOD |= 0xC0;
	*p_PIO_OER_PIOD |= 0xC0;
	
	// wait more than 15 ms after power-up!
	delay_ms(20);
	// select Instruction Register
	CLR_BIT(*p_PIO_CODR_PIOD, 6);
	// Function Set: 8 bit data interface
	write_4bit_msb(0x30);
	delay_ms(10);
	write_4bit_msb(0x30);
	delay_ms(10);
	write_4bit_msb(0x30);
	delay_ms(10);

	// Function Set: switch to 4 bit data interface
	write_4bit_msb(0x20);
	delay_us(45);
	
	// Function Set: 4 bit data interface, 2 lines, font 5x10
	lcd_write(INSTRUCTION, 0x28);		

	// Display on, no cursor
	lcd_set_cursor_mode(CURSOR_OFF);

	// clear display
	lcd_clear();

	// Entry Mode Set: incremental cursor movement, no display shift
	lcd_write(INSTRUCTION, 0x06);
}

/*
 * Clears the LCD and moves the cursor to position row 0, column 0.
 */
void lcd_clear(void)
{
	lcd_write(INSTRUCTION, 0x01);
	delay_ms(2);
}

/*
 * Sets mode of the cursor.
 * The cursor can be set to:
 *	CURSOR_OFF:		cursor is turned off
 *	CURSOR_ON:		cursor is turned on
 *	CURSOR_BLINK:	cursor is blinking
 *
 * parameter:
 *	mode: cursor mode
 */
void lcd_set_cursor_mode(enum lcd_cursor mode)
{
	uint8_t cursor_mode;
	cursor_mode = 0x0C | mode;
	lcd_write(INSTRUCTION, cursor_mode);
};

/*
 * Set position of the cursor.
 *
 * parameter:
 *	row: 0 is the first row, 1 is the second row
 *	col: 0 is the first column, 15 is the last visible column
 */
void lcd_set_cursor_pos(uint8_t row, uint8_t col)
{
	uint8_t cursor_pos;
	cursor_pos = 0x80 | (row << 6) | col;
	lcd_write(INSTRUCTION, cursor_pos);
}

/*
 * Writes data or instruction to the LCD.
 *
 * parameter:
 *	lcd_reg: register to communicate with (Instruction or Data register)
 *	data: 8-bit instruction or data (character)
 */
void lcd_write(enum lcd_register lcd_reg, uint8_t data)
{
	// select register
	if (lcd_reg == INSTRUCTION) 
	{
		CLR_BIT(*p_PIO_CODR_PIOD, 6);   // Instruction Register
	} 
	else 
	{
		SET_BIT(*p_PIO_SODR_PIOD, 6);	// Data Register
	}
	// write data
	write_4bit_msb(data);
	write_4bit_msb(data << 4);
	delay_us(45);
}

/*
 * Synthesize an integer value to decimal fractions arithmetically. 
 * Displays an RTT-value on the LCD
 * 
 * parameter:
 *  value: uint32_t value to be compartmentalized, must not exceed 9999
 */
void display_ascii_nbr(uint32_t value){
	uint8_t ascii_chr = 0;
	
	if((value >= 1000) && (value < 10000))
	{
		ascii_chr = ((value / 1000) + 48);
		lcd_write(DATA, ascii_chr);
		ascii_chr = 0;
		ascii_chr = ((value / 100) % 10) + 48;
		lcd_write(DATA, ascii_chr);
		ascii_chr = 0;
		ascii_chr = (((value / 10) % 10) + 48);
		lcd_write(DATA, ascii_chr);
		ascii_chr = 0;
		ascii_chr = ((value % 10) + 48);
		lcd_write(DATA, ascii_chr);
	}
	else if((value >= 100) && (value < 1000))
	{
		ascii_chr = ((value / 100) + 48);
		lcd_write(DATA, ascii_chr);
		ascii_chr = 0;
		ascii_chr = ((value / 10) % 10) + 48;
		lcd_write(DATA, ascii_chr);
		ascii_chr = 0;
		ascii_chr = ((value % 10) + 48);
		lcd_write(DATA, ascii_chr);
	}
	else if((value < 100) && (value >= 10))
	{
		ascii_chr = (value / 10) + 48;
		lcd_write(DATA, ascii_chr);
		ascii_chr = 0;
		ascii_chr = (value % 10) + 48;
		lcd_write(DATA, ascii_chr);
	}
	else if(value <= 9){
		ascii_chr = value + 48;
		lcd_write(DATA, ascii_chr);
	}
}


/*
 * Write a string of character to the LCD.
 *
 * parameter:
 *	p_str: pointer to the string's first character
 */
void lcd_write_str(char *p_str)
{
	while(*p_str != '\0'){  /* loop until we reach null char (end of string) */
		lcd_write(DATA, *p_str);  /* choose data-register (RS = 1). Allows user to send data to be displayed on the LCD */
		p_str++;                  /* increment pointer, i.e. point to next character in string */
	}
}

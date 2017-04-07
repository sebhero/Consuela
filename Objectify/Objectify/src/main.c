/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <delay.h>
#include <stdio.h>
#include <stdio_serial.h>

#define trig PIO_PC26_IDX
#define echo PIO_PC25_IDX



void configureConsole()
{
	const usart_serial_options_t uart_serial_options = 
	{
		.baudrate = 115200,
		.paritytype = UART_MR_PAR_NO
	};
	
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);
}


long pulseIn()
{
	//printf("pulseread");
	//timeout zone
	unsigned long numloops = 0;
	unsigned long maxloops = 5000000;
	unsigned long width = 0;
	// wait for any previous pulse to end
	while ( ioport_get_pin_level(echo) == HIGH)
	{
		if (numloops++ == maxloops) break;
	}
	// wait for the pulse to start
	while (ioport_get_pin_level(echo)== LOW)
	{
		if (numloops++ == maxloops) break;
	}
	
	// wait for the pulse to stop @ here we are measuring the pulse width = incrementing the WIDTH value by one each cycle. atmega328 1 micro second is equal to 16 cycles.
	while (ioport_get_pin_level(echo)==HIGH)
	{
		if (numloops++ == maxloops) break;
		width++;
	}
	return width/5.1;
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	sysclk_init();
	board_init();
	configureConsole();
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	unsigned long duration, distance;
	while(1)
	{
	
 		ioport_set_pin_level(trig, LOW);
 		delay_us(2);
 		ioport_set_pin_level(trig, HIGH);
 		delay_us(10);
 		ioport_set_pin_level(trig, LOW);
 		duration = pulseIn();
 		distance = duration / 58;
		printf("%d cm\n", distance);
		delay_ms(500);
	}
	/* Insert application code here, after the board has been initialized. */
}

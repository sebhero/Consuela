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
#include <spi.h>
#include <spi_master.h>
#include "UltraServo/ultra_servo.h"



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

int main (void)
{
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(servo, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(servo,LOW);
	//turn of watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	//init clock
	sysclk_init();
	//init board
	board_init();
	//init serial communication, printf ..
	configureConsole();
	//test ultrasound sensor
<<<<<<< HEAD
	//testingUltraSound();
	
	//test rfid
	//testingRFID();
	
	while(1)
	{
		
		puts("hello world");
	}
	
=======
	while(1)
	{	
	testingUltraSound();
	//servoDirection(dir);
>>>>>>> e1caa3e575e9c863d963f5854e0ee907330183e9
	
	/**TESTNING AV SYMMETRISK VRIDNING.
	remove slash to comment -->**/

	//testServo();
	//**/
	}

}

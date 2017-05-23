//
//  ConsoleFunctions.c
//  Code to be used in task 1401c since there is a need to print on the terminal window
//
//	Ulrik Eklund 2014
//
//

#include <stdio_serial.h>
#include <asf.h>
#include "conf_board.h"
#include "consoleFunctions.h"

void configureConsole(void)
/* Enables feedback through the USB-cable back to terminal within Atmel Studio */
/* Note that  the baudrate, parity and other parameters must be set in conf/conf_uart_serial.h */
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
	
	printf("Console ready\n");
	printf("=============\n");
}
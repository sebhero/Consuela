/**
 * Main file for testing different movement things for motors
 *	Author: Robin Johnsson, Viktor Malmgren, Daniel M
 *
 */
#include <asf.h>
#include <inttypes.h>
#include <stdio_serial.h>
#include "conf_board.h"
#include "motorFunc.h"
#include "distanceSensor.h"
#include "pulseCounterHandler.h"
#include "buttonInterrupt.h"
#include "Hjulreglering.h"



static void configure_console(void)
/* Enables feedback through the USB-cable back to terminal within Atmel Studio */
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

int main (void)
{
	sysclk_init();
	board_init();
	configure_console();
	//Makes pin 24 on the Due-board an output
	ioport_set_pin_dir(pin24,IOPORT_DIR_OUTPUT);
	
	
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	uint32_t rotateVal = 90;
	
	//Starts with a delay simply to reduce the chance of an error occuring when reseting the program.
	delay_ms(2000);

	/*
		Loop that runs the functions called upon from the "motorFunc.h" file
		It is recommended that the "stop()" function is called between different sets of commands ex:
		forwardDrive, then stop, then reverseDrive.
		DO NOT go: forwardDrive into reverseDrive	
		This is to ensure that the motors don't get damaged.
	*/
	/*
	pulse(reverseBaseSpeed);
	delay_us(1100);
	pulse(baseSpeedLeft);
	*/
	delay_ms(2000);
	
	while(1){
		
	//int ek = (counterA - counterB);	
	//printf("ek: %d\n", ek);
	rotateRightByDegrees(rotateVal);
	delay_ms(5000);
	rotateLeftByDegrees(rotateVal);
	//reglerahjul3(ek);
	//rotateVal +=90;
	delay_ms(5000);
	
	} 
	return 0;
}


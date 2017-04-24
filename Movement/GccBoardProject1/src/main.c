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
	unsigned long distance;
	double ratio;
	
	//Starts with a delay simply to reduce the chance of an error occuring when reseting the program.
	delay_ms(2000);

	/*
		Loop that runs the functions called upon from the "motorFunc.h" file
		It is recommended that the "stop()" function is called between different sets of commands ex:
		forwardDrive, then stop, then reverseDrive.
		DO NOT go: forwardDrive into reverseDrive	
		This is to ensure that the motors don't get damaged.
	*/
	pulse(baseSpeed);
	delay_us(1100);
	pulse(baseSpeed);
	
	delay_ms(2000);
	while(1){
		ioport_get_pin_level(A);
		ioport_get_pin_level(B);
		printf("Awheel: %d\n", counterA);
		printf("Bwheel: %d\n", counterB);
		//int ek = counterA - counterB;
		double aVal= (double)counterA;
		double bVal = (double)counterB;
		ratio = aVal/bVal;
		//distance = distance_forward();
	//	printf("Pulse counter A = %i\n", counterA);
	//	printf("Pulse counter B = %i\n", counterB);
	
/*		if (distance<75)
		{
			stop();
			rotate();
			 counterA = 0;
			 counterB = 0;
			
		} 
		else
		{
			
		} */
		reglerahjul3(ratio);
		//forwardDrive();
	
	//reglerahjul(ek);
	//reglerahjul2(ek);
	//forwardDrive();
	//printf(counterB);
	//turnLeft();
	delay_ms(100);

	} 
	return 0;
}


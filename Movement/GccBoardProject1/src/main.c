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
#include "navigation.h"



static void configure_console(void)
/* Enables feedback through the USB-cable back to terminal within Atmel Studio */
{
	//Use this for normal serial print into console
	/*const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	 Configure console UART. 
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
	
	printf("Console ready\n");
	printf("=============\n");*/
	
	//THis is for navigation
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY,
		.charlength = CONF_UART_CHAR_LENGTH,
		.stopbits = CONF_UART_STOP_BITS
	};
	sysclk_enable_peripheral_clock(BOARD_USART1_BASE);
	usart_serial_init(CONF_UART, &uart_serial_options);
}

void USART1_Handler(){
	CONF_UART->US_CR |=(1<<US_CR_RSTRX);
}

int main (void)
{
	sysclk_init();
	board_init();
	configure_console();
	
	//used for navigation
	NVIC_EnableIRQ((IRQn_Type) ID_USART1);
	usart_enable_interrupt(CONF_UART, UART_IER_RXRDY);
	
	//Makes pin 24 on the Due-board an output
	ioport_set_pin_dir(pin24,IOPORT_DIR_OUTPUT);
	
	
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
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
	uint8_t foo = 0;
	int degreesToPos;
	double tempVariabel = 0;
	while(foo<4){
		
		valuesCalc(foo);
		degreesToPos = angleToPos();
		if (degreesToPos<0){
			degreesToPos = abs(degreesToPos);
			rotateRightByDegrees(degreesToPos);
			updateAngle();
		} else{
			rotateLeftByDegrees(degreesToPos);
			updateAngle();
		}
		while (distanceToPosition(foo)>30.0){
			delay_ms(500);
			int ek = counterA-counterB;
			tempVariabel = counterA*1.355;
			reglerahjul3(ek);
			updatePos(tempVariabel);
			tempVariabel = 0;
		}
		foo++;
		stop();
	} 
	
	return 0;
}


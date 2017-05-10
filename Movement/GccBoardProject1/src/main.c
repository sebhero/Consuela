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

//Cancer from Gustaf
uint8_t c_counter = 0;
char rx[16];

static void configure_console(void)
/* Enables feedback through the USB-cable back to terminal within Atmel Studio */
{
	//Use this for normal serial print into console
	/*const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};*/

	 //Configure console UART. 
	 /*
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
	CONF_UART->US_CR |= (1 << US_CR_RSTRX);
	rx[c_counter++] = CONF_UART->US_RHR & US_RHR_RXCHR_Msk;
	if (c_counter > 15)
	{
		c_counter = 0;
	}
}
void stringToInt(uint16_t *p_variable, char *p_string) {
	*p_variable = (*p_string++ - '0') * 1000;
	*p_variable = *p_variable + (*p_string++ - '0') * 100;
	*p_variable = *p_variable + (*p_string++ - '0') * 10;
	*p_variable = *p_variable + (*p_string - '0');
}


uint16_t x_coordinate(){
		char str1[4];
		str1[0] = rx[0];
		str1[1] = rx[1];
		str1[2] = rx[2];
		str1[3] = rx[3];
		
		uint16_t x1 = 0;

		stringToInt(&x1, str1);
		
		return x1;
}

uint16_t y_coordinate(){
			char str2[4];
			str2[0] = rx[4];
			str2[1] = rx[5];
			str2[2] = rx[6];
			str2[3] = rx[7];
			
			uint16_t x2 = 0;
			
			stringToInt(&x2, str2);
			
			return x2;
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
		rotationChooser(degreesToPos);
		/*if (degreesToPos<0){
			degreesToPos = abs(degreesToPos);
			rotateRightByDegrees(degreesToPos);
			updateAngle();
		} else{
			rotateLeftByDegrees(degreesToPos);
			updateAngle();
		}*/
		//callForData(x_coordinate(),y_coordinate());
		x1_pos = x_coordinate();
		y1_pos = y_coordinate();
		while (distanceToPosition(foo)>30.0){
			/*delay_ms(500);
			int ek = counterA-counterB;
			tempVariabel = counterA*1.355;
			reglerahjul3(ek);
			updatePos(tempVariabel);
			tempVariabel = 0;*/
			
			delay_ms(500);
			int ek = counterA-counterB;
			reglerahjul3(ek);
			x1_pos = x_coordinate();
			y1_pos = y_coordinate();
			angleCheck();
			
		}
		delay_ms(8);
		foo++;
		stop();
	} 
	
	return 0;
}


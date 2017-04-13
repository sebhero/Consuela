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
#include <stdio_serial.h>
#include <asf.h>
#include "conf_board.h"
#include "buttonInterrupt.h"
#include "pulseCounterHandler.h"

#define redLED  IOPORT_CREATE_PIN(PIOA, 14) // digital pin 23
#define greenLED  IOPORT_CREATE_PIN(PIOB, 26) // digital pin 22
#define pin24 PIO_PA15_IDX

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

//Sends the pulse to the engine
void pulse(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
}

//Sets both engines to go the same direction with the "same" speed
void forwardDrive(){
	pulse(2000);
	delay_us(1100);
	pulse(2000);
	delay_ms(100);
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();
	configure_console();
	
	ioport_init();
	ioport_set_pin_dir(redLED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(greenLED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(pin24, IOPORT_DIR_OUTPUT);
	//button_config(ID_PIOC, PIOC, PIO_PC13);
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	delay_ms(2000);
	forwardDrive();
	
	while(1){
		/*ioport_set_pin_level(redLED, 1);
		delay_ms(1000);
		ioport_set_pin_level(redLED, 0);
		delay_ms(1000);*/
		
		ioport_get_pin_level(A);
		ioport_get_pin_level(B);
		printf("Pulse counter A = %i\n", counterA);
		printf("Pulse counter B = %i\n", counterB);

	}
	/* Insert application code here, after the board has been initialized. */
}

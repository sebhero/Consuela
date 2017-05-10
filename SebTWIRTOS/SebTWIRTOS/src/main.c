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
#include "../com/Com.h"

void configureConsole()
{
	const usart_serial_options_t uart_serial_options =
	{
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);
}

int main (void)
{

	/* Initialize the common clock service, board-specific initialization, and
	* interrupt vector support prior to using the TWI master interfaces.
	*/
	sysclk_init();

	board_init();
	
	configureConsole();

	 // TWI master initialization options.
	 //initTwiMaster();
	 initTwi();
	 
	 //send to arduino conf
	 //sendToArduino();
	 //delay_s(5);
	 //readSlave();
	//readWrite();
	uint8_t data[1];
	uint8_t i;
	uint8_t dum[3];
	dum[0]=TWI_CMD_FROM_ARM_ID;
	dum[1]=0;
	dum[2]=0;
	while(1)
	{
		//delay_ms(1000);
		
		sendArm(dum,3);
		//sendArmCmd(TWI_CMD_FROM_ARM_ID);
		//delay_ms(1000);
		reciveFromArm(3);
		delay_ms(100);
		//delay_ms(1000);
		
		

	}
}

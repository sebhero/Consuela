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
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <inttypes.h>
#include "motorFunc.h"
//#define pin24 PIO_PA15_IDX
//Defines pin 24 on the Due-board as a pin to be used.
//#define pin24 PIO_PA15_IDX

//Sets the base pulse width to 1000 micro seconds.
static uint16_t motorSpeed = 1300;	//Motor 1, under 1500 ger bakåt
static uint16_t motorSpeed2 = 1700; //Motor 2 över 1500 ger frammåt. 1500 stannar


int main (void)
{
	sysclk_init();
	board_init();
	//Makes pin 24 on the Due-board an output
	ioport_set_pin_dir(pin24,IOPORT_DIR_OUTPUT);
	
	//Loop that runs the temp. program
	//Turns the motors forward, stops them and lastly turn them backwards
	while(1){
		delay_ms(2000);
		while (motorSpeed<1750)
		{
			pulse(motorSpeed);
			//This delay makes it so the 2nd motor gets the signal
			delay_us(1100); //Man måste ha en viss delay för att nästa puls ska gå till andra hjulet.
			pulse(motorSpeed2);
			motorSpeed = motorSpeed+50; //Ökar hastigheten för motor 1
			motorSpeed2 = motorSpeed2-50; //Minskar hastigheten för motor 2
			//Needs a delay of at least 5.25ms for the "timeout period" to pass
			delay_ms(1000);
		}
		stop();
		
		delay_ms(2000);
		forwardDrive();
		delay_ms(2000);
		stop();
		delay_ms(2000);
		rotate();
		delay_ms(2000);
		stop();
		delay_ms(2000);
		reverseDrive();
	}
	return 0;
}


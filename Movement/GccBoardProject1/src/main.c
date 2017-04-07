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

#define pin24 PIO_PA15_IDX
static uint16_t motorSpeed = 1000;


void motor(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
}

int main (void)
{
	sysclk_init();
	board_init();
	ioport_set_pin_dir(pin24,IOPORT_DIR_OUTPUT);
	
	/*while(1){
		ioport_set_pin_level(pin24, HIGH);
		delay_ms(3000);
		ioport_set_pin_level(pin24, LOW);
		delay_ms(3000);
	}*/
	//Loop that runs the temp. program
	//Turns the motors forward, stops them and lastly turn them backwards
	while(1){
		delay_ms(2000);
		while (motorSpeed<2050)
		{
			motor(motorSpeed);
			delay_us(1100);
			motor(motorSpeed);
			motorSpeed = motorSpeed+50;
			delay_ms(1000);
		}
		motor(1500);
		delay_us(1100);
		motor(1500);
	}
	return 0;
}


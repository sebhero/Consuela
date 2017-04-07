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

#define LED  IOPORT_CREATE_PIN(PIOB, 26) // digital pin 22

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();
	ioport_init();
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);   // LED pin set as output
	
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);   // LED pin set as output

	/* Insert application code here, after the board has been initialized. */
	
	while(1){
		ioport_set_pin_level(LED, true);
		delay_ms(1000);
		ioport_set_pin_level(LED, false);
		delay_ms(1000);
	}
	
}

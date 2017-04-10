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
#include "buttonInterrupt.h"

#define redLED  IOPORT_CREATE_PIN(PIOA, 14) 
#define greenLED  IOPORT_CREATE_PIN(PIOB, 26)
//#define button IOPORT_CREATE_PIN(PIOC, 13)

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();
	
	ioport_init();
	ioport_set_pin_dir(redLED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(greenLED, IOPORT_DIR_OUTPUT);
	button_config(ID_PIOC, PIOC, PIO_PC13);
	//pio_set_input(PIOC, PIO_PC13, PIO_PULLUP);
	
	/* Insert application code here, after the board has been initialized. */
	
	while(1){
		ioport_set_pin_level(redLED, 1);
		delay_ms(1000);
		ioport_set_pin_level(redLED, 0);
		delay_ms(1000);
		/*
		if(pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC13) == 1){
			ioport_set_pin_level(greenLED, 1);
			ioport_set_pin_level(redLED, 0);
		}else{
			ioport_set_pin_level(greenLED, 0);
			ioport_set_pin_level(redLED, 1);
		}
		*/
	}
	
}

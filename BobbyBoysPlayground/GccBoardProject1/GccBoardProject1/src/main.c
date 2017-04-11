
#include <asf.h>

#include "button/button.h"
#include "pwm/pwm.h"

#define LED IOPORT_CREATE_PIN(PIOC, 26)



	
int main (void) {
	sysclk_init();

	board_init();
	
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	
	configure_pwm();
	
	configure_buttons();
	
	
	
	while(1) {
		ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);
		delay_ms(500);
		ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
		delay_ms(500);
		
	}

}

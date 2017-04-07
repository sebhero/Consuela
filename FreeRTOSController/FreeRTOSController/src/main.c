
#include <asf.h>


#define LED  IOPORT_CREATE_PIN(PIOB, 26) // digital pin 22

int main (void)
{
	sysclk_init();
    ioport_init();
	board_init();
	
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);   // LED pin set as output
	
	
	/* Insert application code here, after the board has been initialized. */
	
	
	while(1){
		ioport_set_pin_level(LED, true);
		delay_ms(1000);
		ioport_set_pin_level(LED, false);
		delay_ms(1000);
	}
}

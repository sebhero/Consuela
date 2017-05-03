/*!
 * \file pulse.c
 *
 */
 #include <asf.h>
 #include "pulse.h"

 // Forward declare the functions which should not be exposed

 void pulse_ioport_init_channel(uint32_t ch); 
 






void pulse_init() {
    pulse_generator_init_channel(0);
    pulse_generator_init_channel(1);
	
	pulse_timer_init_channel(0);
	pulse_timer_init_channel(1);
	
	
	pulse_counter_init_channel(0);
	/*
	pulse_ioport_init_channel(0);
	*/
}








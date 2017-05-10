/*
 * buttonInterrupt.c
 *
 * Created: 2017-04-07 14:35:08
 *  Author: Danial Mahmoud
 */ 

#include "buttonInterrupt.h"
#include <asf.h>

#define greenLED  IOPORT_CREATE_PIN(PIOB, 26)
#define redLED  IOPORT_CREATE_PIN(PIOA, 14)

void button_handler(const uint32_t id, const uint32_t index){
	if(id == ID_PIOC && index == PIO_PC13){	
		if(pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC13) == 1){
			ioport_set_pin_dir(greenLED, IOPORT_DIR_OUTPUT);
			ioport_set_pin_level(greenLED, 1);
		}else{
			ioport_set_pin_dir(greenLED, IOPORT_DIR_OUTPUT);
			ioport_set_pin_level(greenLED, 0);
		}
	}
}

void button_config(uint32_t ul_id, Pio *p_pio, const uint32_t ul_mask){
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ul_id);
	pio_set_input(p_pio, ul_mask, PIO_PULLUP);
	pio_handler_set(p_pio, ul_id, ul_mask, PIO_IT_EDGE, button_handler);
	pio_enable_interrupt(p_pio, ul_mask);
	NVIC_EnableIRQ(PIOC_IRQn);
}
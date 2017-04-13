/*
 * pulseCounterHandler.c
 *
 * Created: 2017-04-12 10:29:15
 *  Author: Danial Mahmoud
 */ 

#include <asf.h>
#include "pulseCounterHandler.h" /* Declaration made available here */

int counterA = 0;

void pulseCounter_handler(const uint32_t id, const uint32_t index){
	if (ioport_get_pin_level(A))
	{
		counterA++;
	}
	
}

void pulseCounter_config(uint32_t ul_id, Pio *p_pio, const uint32_t ul_mask){
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ul_id);
	//pio_set_output(p_pio, ul_mask, LOW, DISABLE, ENABLE);
	pio_set_input(p_pio, ul_mask, PIO_PULLUP);
	pio_handler_set(p_pio, ul_id, ul_mask, PIO_IT_EDGE, pulseCounter_handler);
	pio_enable_interrupt(p_pio, ul_mask);
	NVIC_EnableIRQ(PIOC_IRQn);
}
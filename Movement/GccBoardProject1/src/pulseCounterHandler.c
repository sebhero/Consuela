/*
 * pulseCounterHandler.c
 *
 * Created: 2017-04-12 10:29:15
 *  Author: Danial Mahmoud
 */ 

#include <asf.h>
#include <stdio_serial.h>
#include "pulseCounterHandler.h" /* Declaration made available here */

/* Initialization of extern variables goes only with definition */
 int counterA = 0;
 int counterB = 0;
 

void pulseCounter_handlerA(const uint32_t id, const uint32_t index){
	if (ioport_get_pin_level(A))
	{
		counterA++;
	}

}

void pulseCounter_handlerB(const uint32_t id, const uint32_t index){
	if (ioport_get_pin_level(B))
	{
		counterB++;
	}
	
}

void pulseCounter_configA(uint32_t ul_id, Pio *p_pio, const uint32_t ul_mask){
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ul_id);
	//pio_set_output(p_pio, ul_mask, LOW, DISABLE, ENABLE);
	pio_set_input(p_pio, ul_mask, PIO_PULLUP);
	pio_handler_set(p_pio, ul_id, ul_mask, PIO_IT_EDGE, pulseCounter_handlerA);
	pio_enable_interrupt(p_pio, ul_mask);
	NVIC_EnableIRQ(PIOC_IRQn);
}

void pulseCounter_configB(uint32_t ul_id, Pio *p_pio, const uint32_t ul_mask){
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ul_id);
	//pio_set_output(p_pio, ul_mask, LOW, DISABLE, ENABLE);
	pio_set_input(p_pio, ul_mask, PIO_PULLUP);
	pio_handler_set(p_pio, ul_id, ul_mask, PIO_IT_EDGE, pulseCounter_handlerB);
	pio_enable_interrupt(p_pio, ul_mask);
	NVIC_EnableIRQ(PIOC_IRQn);
}
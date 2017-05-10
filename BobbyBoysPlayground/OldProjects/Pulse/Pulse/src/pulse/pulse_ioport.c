#include <asf.h>
#include "pulse_ioport.h"



static void pulse_ioport_configure(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr);
static uint32_t pulse_ioport_pin_to_ID_Port(ioport_pin_t pin);
static IRQn_Type pulse_ioport_pin_to_Port_IRQn(ioport_pin_t pin);


pulse_ioport_t pulse_ioports[] = {
	{
		.pin = PIO_PB27_IDX,
		.mode = (PIO_PULLUP | PIO_DEBOUNCE),
		.handler = ch0_handler,
		.trigg_attr = PIO_IT_RISE_EDGE,
		.cnt = 0
	}
};


/*
* \brief Configure a ioport channel for input
*
*/
void pulse_ioport_init_channel(uint32_t ch) {
	pulse_ioport_configure(pulse_ioports[ch].pin, pulse_ioports[ch].handler, pulse_ioports[ch].mode,  pulse_ioports[ch].trigg_attr);
}

/**
 * \file
 *
 * \brief Template configuration for a ioport
 *
 */
static void pulse_ioport_configure(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr) {
    Pio *pio_base = arch_ioport_pin_to_base(pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(pin);
    IRQn_Type IRQn = pulse_ioport_pin_to_Port_IRQn(pin);
    uint32_t ID = pulse_ioport_pin_to_ID_Port(pin);
    
    pio_set_input(pio_base, pin_mask, pin_attr);
    pio_handler_set(pio_base, ID, pin_mask, trig_attr, handler);
    pio_enable_interrupt(pio_base, pin_mask);
    NVIC_ClearPendingIRQ(IRQn);
    NVIC_EnableIRQ(IRQn);
}

uint32_t pulse_ioport_get_cnt(uint32_t ch_n) {
	return pulse_ioports[ch_n].cnt;
}

/**
 * \file
 *
 * \brief Helper function to get the peripheral ID from a pin
 *
 */
static uint32_t pulse_ioport_pin_to_ID_Port(ioport_pin_t pin) {
     uint32_t ID = 0;
    switch(arch_ioport_pin_to_port_id(pin)) {
        case(IOPORT_PIOA):
        ID = ID_PIOA;
        break;
        case(IOPORT_PIOB):
        ID = ID_PIOB;
        break;
        case(IOPORT_PIOC):
        ID = ID_PIOC;
        break;
        case(IOPORT_PIOD):
        ID = ID_PIOD;
        break;        
    }
    return ID;
}

/**
 * \file
 *
 * \brief Helper function to get the port IRQn from a pin
 *
 */
static IRQn_Type pulse_ioport_pin_to_Port_IRQn(ioport_pin_t pin) {
    IRQn_Type IRQn = 0;    
    switch(arch_ioport_pin_to_port_id(pin)) {
        case(IOPORT_PIOA):
        IRQn = PIOA_IRQn;        
        break;
        case(IOPORT_PIOB):
        IRQn = PIOB_IRQn;
        break;
        case(IOPORT_PIOC):
        IRQn = PIOC_IRQn;
        break;
        case(IOPORT_PIOD):
        IRQn = PIOD_IRQn;
        break;
    }
    return IRQn;
}



/**
 * \file
 *
 * \brief Handler for ch0
 *
 */
void ch0_handler(const uint32_t id, const uint32_t index) {
    if ((id == pulse_ioport_pin_to_ID_Port(pulse_ioports[0].pin)) && (index == ioport_pin_to_mask(pulse_ioports[0].pin))){
        pulse_ioports[0].cnt += 1;
    }
}
 
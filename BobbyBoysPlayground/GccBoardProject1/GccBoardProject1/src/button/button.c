/*
 * button.c
 *
 * Created: 2017-04-11 17:29:21
 *  Author: bob
 */ 

#include <asf.h>
#include "button.h"
#include "../pwm/pwm.h"

/**
 * \file
 *
 * \brief Template configuration for a button
 *
 */
static void configure_button(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr) {
    Pio *pio_base = arch_ioport_pin_to_base(pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(pin);
    IRQn_Type IRQn = ioport_pin_to_Port_IRQn(pin);
    uint32_t ID = ioport_pin_to_ID_Port(pin);
    
    pio_set_input(pio_base, pin_mask, pin_attr);
    pio_handler_set(pio_base, ID, pin_mask, trig_attr, handler);
    pio_enable_interrupt(pio_base, pin_mask);
    NVIC_ClearPendingIRQ(IRQn);
    NVIC_EnableIRQ(IRQn);
}


#define BTN IOPORT_CREATE_PIN(PIOC, 21)


/**
 * \file
 *
 * \brief Helper function to get the peripheral ID from a pin
 *
 */
static uint32_t ioport_pin_to_ID_Port(ioport_pin_t pin) {
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
 * \brief Helper function to get the port IRQn froma pin
 *
 */
static IRQn_Type ioport_pin_to_Port_IRQn(ioport_pin_t pin) {
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
 * \brief Handler for BTN1
 *
 */
static void btn_handler(const uint32_t id, const uint32_t index) {
    if ((id == ioport_pin_to_ID_Port(BTN)) && (index == ioport_pin_to_mask(BTN))){
		static pwm_channel_t pwm_channel_instance = {
			.ul_prescaler = PWM_CMR_CPRE_CLKA,
			.ul_period = 50000,
			.ul_duty = 0,
			.channel = PWM_CHANNEL_1
		};
			
        pwm_channel_instance.channel = PWM_CHANNEL_1;
        pwm_channel_instance.ul_duty = (pwm_channel_instance.ul_duty + 10000) % 60000;
        pwm_channel_disable(PWM, PWM_CHANNEL_1);
        pwm_channel_init(PWM, &pwm_channel_instance);
        pwm_channel_enable(PWM, PWM_CHANNEL_1);		
    }
}



void configure_buttons() {
    configure_button(BTN, btn_handler, (PIO_PULLUP | PIO_DEBOUNCE),  PIO_IT_FALL_EDGE);
}
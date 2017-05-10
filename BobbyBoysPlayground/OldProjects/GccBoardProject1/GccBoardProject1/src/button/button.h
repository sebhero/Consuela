/*
 * button.h
 *
 * Created: 2017-04-11 17:29:06
 *  Author: bob
 */ 


//#ifndef BUTTON_H_
//#define BUTTON_H_
#pragma once

static void configure_button(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr);
static void btn_handler(const uint32_t id, const uint32_t index);
void configure_buttons();
static IRQn_Type ioport_pin_to_Port_IRQn(ioport_pin_t pin);
static uint32_t ioport_pin_to_ID_Port(ioport_pin_t pin);


//#endif /* BUTTON_H_ */




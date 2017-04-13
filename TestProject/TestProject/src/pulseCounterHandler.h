/*
 * pulseCounterHandler.h
 *
 * Created: 2017-04-12 10:29:32
 *  Author: Danial Mahmoud
 */ 

#include <asf.h>

#ifndef PULSECOUNTERHANDLER_H_
#define PULSECOUNTERHANDLER_H_

extern int counterA; /* Declaration of the variable */
extern int counterB;

#define A IOPORT_CREATE_PIN(PIOC, 28) //digital pin 3

void pulseCounter_handler(const uint32_t id, const uint32_t index); 
void pulseCounter_config(uint32_t ul_id, Pio *p_pio, const uint32_t ul_mask);


#endif /* PULSECOUNTERHANDLER_H_ */
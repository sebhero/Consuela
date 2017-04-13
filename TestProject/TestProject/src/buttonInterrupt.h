/*
 * buttonInterrupt.h
 *
 * Created: 2017-04-07 14:35:53
 *  Author: Danial Mahmoud
 */ 

#include <asf.h>

#ifndef BUTTONINTERRUPT_H_
#define BUTTONINTERRUPT_H_

void button_handler(const uint32_t id, const uint32_t index);
void button_config(uint32_t ul_id, Pio *p_pio, const uint32_t ul_mask);

#endif /* BUTTONINTERRUPT_H_ */
/*
 * pulseCounterHandler.h
 *
 * Created: 2017-04-12 10:29:32
 *  Author: Spellabbet
 */ 


#ifndef PULSECOUNTERHANDLER_H_
#define PULSECOUNTERHANDLER_H_

extern int counterA; /* Declaration of the variable */
extern int counterB;
void pulseCounter_handler(int counterA); //const uint32_t idA, const uint32_t indexA, const uint32_t idB, const uint32_t indexB
//void pulseCounter_config(uint32_t ul_idA, Pio *p_pioA, const uint32_t ul_maskA, uint32_t ul_idB, Pio *p_pioB, const uint32_t ul_maskB);


#endif /* PULSECOUNTERHANDLER_H_ */
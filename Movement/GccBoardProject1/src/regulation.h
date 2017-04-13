/*
 * regulation.h
 *
 * Created: 13/04/2017 14:45:08
 *  Author: Robin_2
 */ 


#ifndef REGULATION_H_
#define REGULATION_H_
#include <inttypes.h>

uint16_t calculateSpeed(uint16_t distanceArray[]);
void regulatedForward(uint16_t speedR,uint16_t speedL);


#endif /* REGULATION_H_ */
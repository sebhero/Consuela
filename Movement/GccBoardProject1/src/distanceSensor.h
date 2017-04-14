/*
 * distanceSensor.h
 *
 * Created: 10/04/2017 10:32:52
 *  Author: Robin_2
 */ 


#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_
#include <asf.h>

#define trig PIO_PC26_IDX
#define echo PIO_PC25_IDX

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

long pulseIn(void);
long distance_forward(void);
//uint16_t calculateSpeed(uint16_t distanceArray[]);



#endif /* DISTANCESENSOR_H_ */
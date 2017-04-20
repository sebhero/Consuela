/*
 * distanceSensor.h
 *
 * Created: 10/04/2017 10:32:52
 *  Author: Robin_2
 */ 


#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_

#define trig PIO_PC26_IDX
#define echo PIO_PC25_IDX

long pulseIn(void);
long distance_forward(void);



#endif /* DISTANCESENSOR_H_ */
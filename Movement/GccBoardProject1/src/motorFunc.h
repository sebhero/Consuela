/*
 * motorFunc.h
 *
 * Created: 07/04/2017 12:54:59
 *  Author: Robin_2
 */ 


#ifndef MOTORFUNC_H_
#define MOTORFUNC_H_
#define pin24 PIO_PA15_IDX
#include <inttypes.h>

void pulse(uint16_t motorSpeed);
void forwardDrive(void);
void reverseDrive(void);
void rotate(void);
void stop(void);





#endif /* MOTORFUNC_H_ */
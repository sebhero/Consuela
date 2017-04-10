/*
 * motorFunc.h
 *
 * Created: 07/04/2017 12:54:59
 *  Author: Robin Johnsson
 */ 


#ifndef MOTORFUNC_H_
#define MOTORFUNC_H_
//Defines pin 24 on the Due-board as a pin to be used.
#define pin24 PIO_PA15_IDX
#define baseSpeed 1750
#define reverseBaseSpeed 1250
#define motorSwitch 1100
#define timeOut 2000



#include <inttypes.h>

void pulse(uint16_t motorSpeed);
void forwardDrive(void);
void reverseDrive(void);
void rotate(void);
void stop(void);
void rotateByDegrees(uint8_t degree, uint16_t speed);




#endif /* MOTORFUNC_H_ */
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

//Temporary "baseSpeed" that will be used until we get positioning and regulation
#define baseSpeed 1700

//Tested that this 1713 on the left wheel gives a kinda straight line when the other motor is 1750 (forward driving)
#define baseSpeedLeft 1660

#define reverseBaseSpeed 1280
#define motorSwitch 1100
#define timeOut 600



#include <inttypes.h>

void pulse(uint16_t motorSpeed);
void forwardDrive(void);
void reverseDrive(void);
void rotate(void);
void stop(void);
void turnLeft(void);
void turnRight(void);
void rotateLeftByDegrees(int degree);
void rotateRightByDegrees(int degree);



#endif /* MOTORFUNC_H_ */
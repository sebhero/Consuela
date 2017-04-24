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
#define baseSpeed 1750

//Tested that this 1713 on the left wheel gives a kinda straight line when the other motor is 1750 (forward driving)
#define baseSpeedLeft 1750

#define reverseBaseSpeed 1250
#define motorSwitch 1100
#define timeOut 2000



#include <inttypes.h>

void pulse(uint16_t motorSpeed);
void forwardDrive(void);
void reverseDrive(void);
void rotate(void);
void stop(void);
void turnLeft(void);
void turnRight(void);
void rotateByDegrees(uint8_t degree, uint16_t speed);



#endif /* MOTORFUNC_H_ */
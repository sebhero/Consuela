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
#define timeOut 200



#include <inttypes.h>

/*
	Sends a pulse on the designated pin that varies
	with the "motorSpeed" input
*/
void pulse(uint16_t motorSpeed);

/*
	Sets both motors to forward direction until the desired
	distance has been covered
*/
void forwardDrive(int distance);

/*
	Sets both motors to backwards direction until the desired
	distance has been covered
*/
void reverseDrive(int distance);

/*
	Stops both of the motors
*/
void stop(void);
/*
	Rotates the platform to the left (anti-clockwise) for
	desired amount of degrees
*/
void rotateLeftByDegrees(int degree);

/*
	Rotates the platform to the right (clockwise) for
	desired amount of degrees
*/
void rotateRightByDegrees(int degree);

/*
	Splits up the rotation into segments of a maximum of 
	45 degrees at a time
*/
void rotateRight(int degree);

/*
	Splits up the rotation into segments of a maximum of 
	45 degrees at a time
*/
void rotateLeft(int degree);

/*
	Controls the two motors so that they keep the same speed
	This is done by checking the difference of the two counters
	on the wheels, and then adding the difference to the left
	wheel.
*/
void wheelControl(int ek);


#endif /* MOTORFUNC_H_ */
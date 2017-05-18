/*
 * navigation.h
 *
 * Created: 03/05/2017 09:00:19
 *  Author: Robin Johnsson
 */ 


#ifndef NAVIGATION_H_
#define NAVIGATION_H_
#include <inttypes.h>
#include <asf.h>
#include <math.h>
#include <stdio_serial.h>
#include "motorFunc.h"

#define sock_x 136
#define sock_y 200
#define glass_x 56
#define glass_y 362
#define cube_x 434
#define cube_y 35
#define dropOff_x 451
#define dropOff_y 350
#define PI 3.141592

extern uint16_t x1_pos;
extern uint16_t y1_pos;
extern int currentAngle;

/*
	Calculates the midpoint of the platform with the help of two sets of coordinates.
	In the case that only one set is given, that will be used as the midpoint.
*/
void calcMidPos(void);

/*
	Calculates the distance from the platform to a given object
*/
double distanceToPosition(uint8_t obj);

/*
	Updates the position of the platform by calculating the distance traveled along with 
	the angle traveled in.
	
	This function is only used when Dead Reckoning is being used.
*/
void updatePos(double hyp);

/*
	Calculate the angle from the platform to the given position/object.
	Returns a negative number if the angle is closer clockwise than anti-clockwise
*/
int angleToPos(void);

/*
	Calculates the difference in X/Y between the platform and a given position
	Also calculates the angle to the object from the platform
*/
void valuesCalc(uint8_t obj);

/*
	Updates the angle of the platform to what it should be according
	to calculated values.
*/
void updateAngle(void);

/*
	Checks if the angle that the platform actually travels in is the same as the believed one.
	If they differ the platform will stop and recalibrate to fix this.
*/
void angleCheck(void);

/*
	Choose the direction of the rotation depending on the value of input
*/
void rotationChooser(int degreesToPos);


#endif /* NAVIGATION_H_ */
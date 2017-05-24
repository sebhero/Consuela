/*
 * navigation.h
 *
 * Created: 03/05/2017 09:00:19
 *  Author: Robin_2
 */ 


#ifndef NAVIGATION_H_
#define NAVIGATION_H_
#include <inttypes.h>
#include <math.h>
#include <stdio_serial.h>
#include "driving/motorFunc.h"
#include "pulse/pulseCounterHandler.h"
#include "com/TwiComHandler.h"

#define sock_x 136
#define sock_y 200
#define glass_x 56
#define glass_y 362
#define cube_x 434
#define cube_y 35
#define dropOff_x 451
#define dropOff_y 350
#define PI 3.141592

//Temporary
/*
typedef enum _object_t {
	SOCK = 2,
	SQUARE = 3,
	GLASS = 4
} Object;*/

extern int x1_pos;
extern int y1_pos;

/*
	Sets the correct objects with their coordinates and object info
*/
void setObjectSimple(objectinfo_t theObj);

/*
	Sets the correct objects with their coordinates and object info
*/
void setObject(Object obj,int x, int y);

/*
	Decides if the arm can pick up all objects before dropoff, or if it needs to take one at a time
*/
void setCollectAll(uint8_t getAll);

/*
	Sets a suspension variable to false
	done after a pickup
*/
void setDonePickup(void);

/*
	Sets a suspension variable to false
	done after a dropoff
*/
void setDropoffDone(void);

/*
	Runs EVERYTHING that has to do with navigation.
	Decides on a travel path if none is existent
	Turns the platform towards its destination
	Moves the platform towards the destination
	Stops when has approached the destination
	Repeats until all destinations has been reached
*/
uint8_t goToNext();
void setPath(void);

void initNav(void);

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
void updateAngle(int16_t angleDiff);

/*
	Checks if the angle that the platform actually travels in is the same as the believed one.
	If they differ the platform will stop and recalibrate to fix this.
*/
void angleCheck(void);

/*
	Choose the direction of the rotation depending on the value of input
*/
void rotationChooser(int degreesToPos);

void setRadius(uint8_t armRadius);

Object getCurrentObject(void);

#endif /* NAVIGATION_H_ */
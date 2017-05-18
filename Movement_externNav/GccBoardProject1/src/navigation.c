/*
 * navigation.c
 *
 * Created: 03/05/2017 08:59:50
 *  Author: Robin Johnsson
 *
 *	Contains the navigation functions used by the platform.
 *	The functions use angles, positions and distances to calculate what should be done.
 */ 
#include "navigation.h"

//Start of variables
//Coordinates of the platform
uint16_t x1_pos = 0;
uint16_t y1_pos = 0;
uint16_t x2_pos = 0;
uint16_t y2_pos = 0;

//Can be removed?
uint8_t data[8];
uint8_t temp;


double distanceLeft;

//Calculation variables
int deltaX;
int deltaY;

//Different angle variables
int angle;
double angleRad;
int platformAngle;
int objectAngle;
int angleVal;
int angleTemp;
int currentAngle = 90;

//Variables for the former position of the platform
uint16_t last_x;
uint16_t last_y;

//Mid values of the platform, will be x1 and y1 if only one set of coordinates is given
static uint16_t mid_x;
static uint16_t mid_y;

//Struct used to define objects/positions
typedef struct {
	uint16_t x_pos;
	uint16_t y_pos;
	const char *name_p;
	} object_pos_t;

//Array of objects
object_pos_t objects[] = {
	{
		.x_pos = sock_x,
		.y_pos = sock_y,
		.name_p = "Sock"
	},
	{
		.x_pos = glass_x,
		.y_pos = glass_y,
		.name_p = "Glass"
	},
	{
		.x_pos = cube_x,
		.y_pos = cube_y,
		.name_p = "Cube"
	},
	{
		.x_pos = dropOff_x,
		.y_pos = dropOff_y,
		.name_p = "Drop off"
	}
};

/*
	Calculates the midpoint of the platform with the help of two sets of coordinates.
	In the case that only one set is given, that will be used as the midpoint.
*/
void calcMidPos(){
	/*	This part is used if 2 sets of coordinates are given
		mid_x = (x1_pos+x2_pos)/2;
		mid_y = (y1_pos+y2_pos)/2;
	*/
	//Use this part if only 1 set of coordinates are given.
	mid_x = x1_pos;
	mid_y = y1_pos;
	
}

/*
	Calculates the distance from the platform to a given object
*/
double distanceToPosition(uint8_t obj){
	//Call that calculates angle of the object and X/Y differences
	valuesCalc(obj);
	//Calculates the hypotenuse 
	distanceLeft = sqrt(pow(deltaX,2)+pow(deltaY,2));
	//Returns the distance/hypotenuse
	if (distanceLeft>60)
	{
		ioport_set_pin_level(PIO_PD1_IDX, HIGH);
	}else{
		ioport_set_pin_level(PIO_PD1_IDX, LOW);
	}
	return distanceLeft;
}
/*
	Calculate the angle from the platform to the given position/object.
	Returns a negative number if the angle is closer clockwise than anti-clockwise
*/
int angleToPos(){
	//Sets the current angle into a temp variable
	platformAngle = currentAngle;
	//Transforms the angle to the objective to a usable value(0-360)
	objectAngle = 180 - angle;
	
	//Calculates the angle-difference between the platform and the objective
	angleVal = abs(((abs(platformAngle-360) + objectAngle)%360) - 360);
	
	//If the angle-difference is greater than 180, make it negative
	(angleVal > 180) ? (angleVal -= 360) : (0);
	
	return angleVal;
}

/*
	Calculates the difference in X/Y between the platform and a given position
	Also calculates the angle to the object from the platform
*/
void valuesCalc(uint8_t obj){
	//Calculates the position of the platform
	calcMidPos();
	
	//Calculates the X/Y difference
	deltaX = objects[obj].x_pos - mid_x;
	deltaY = objects[obj].y_pos - mid_y;
	
	//Calculates the angle between the platform and object in radians
	angleRad = atan2(deltaY,deltaX);
	
	//Converts the angle from radians to degrees
	angle = (angleRad*180)/PI;
}

/*
	Updates the angle of the platform to what it should be according
	to calculated values.
*/
void updateAngle(){
	currentAngle = ((currentAngle - angleVal)+360)%360;
}
/*
	Updates the position of the platform by calculating the distance traveled along with 
	the angle traveled in.
	
	This function is only used when Dead Reckoning is being used.
	When fetching the location data from an external source instead 
	use "callForData" and update the position!
*/
void updatePos(double hyp){
	angleRad = currentAngle * PI / 180;
	mid_x = mid_x+(-(cos(angleRad) * hyp));
	mid_y = mid_y+(sin(angleRad) * hyp);
	x1_pos = mid_x;
	y1_pos = mid_y;
}

/*
	Checks if the angle that the platform actually travels in is the same as the believed one.
	If they differ to much, the platform will stop and recalibrate to fix this.
	
	Didn't work in the first practical test, but that might be a consequence of other factors
	Will now only be called once half of the distance to a destination has been traveled
*/
void angleCheck(){
	//Saves the last position
	last_x = mid_x;
	last_y = mid_y;
	
	//Calculates the new position of the platform
	calcMidPos();
	
	//Calculates the X/Y-difference of the platform and the object
	deltaX = mid_x - last_x;
	deltaY = mid_y - last_y;
	
	//Calculates the angle to the object from the platform
	angleRad = atan2(deltaY,deltaX);
	angleTemp = (angleRad*180)/PI;
	angleTemp = 180 - angleTemp;
	
	//If the angle-difference of the current travel-angle and the angle to the object is bigger than 4
	//then fix the positioning so that the angle is equal.
	if (angleTemp!=currentAngle)
	{
		//Stops the movement
		stop();
		//Sets currentAngle to the actual angle of the platform
		currentAngle = angleTemp;
		//Rotates the platform the needed amount in the correct direction.
		rotationChooser(angleToPos());
		
	}
	
}

/*
	Choose the direction of the rotation depending on the value of input
*/
void rotationChooser(int degreesToPos){
	//If the value is negative rotate right, else left
	if (degreesToPos<0){
		degreesToPos = abs(degreesToPos);
		//Rotates the platform clockwise by the input value
		rotateRight(degreesToPos);
		//Update the angle
		updateAngle();
	} else{
		//Makes the value into a positive value
		
		//Rotates the platform anti-clockwise by the input value
		rotateLeft(degreesToPos);
		//Update the angle
		updateAngle();
	}
}
/*
 * navigation.c
 *
 * Created: 03/05/2017 08:59:50
 *  Author: Robin Johnsson
 */ 
#include "navigation.h"

//Dummy-v�rden f�r nuvarande
uint16_t x1_pos = 0;
uint16_t y1_pos = 0;
uint16_t x2_pos = 0;
uint16_t y2_pos = 0;

uint8_t data[8];
uint8_t temp;
double distanceLeft;

int deltaX;
int deltaY;
int angle;
double angleRad;

int platformAngle;
int objectAngle;
int angleVal;
int angleTemp;

static uint16_t mid_x;
static uint16_t mid_y;
int currentAngle = 90;

typedef struct {
	uint16_t x_pos;
	uint16_t y_pos;
	const char *name_p;
	} object_pos_t;

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
	Function that calls the navigation-unit and receives the data representing coordinates.
	This then gets placed at the correct positions and the values can then be used.
*/

void callForData(uint16_t x1, uint16_t x2){
	//data;//Function call
	x1_pos = x1;
	y1_pos = x2;
	/*
	x1_pos = (data[0] << 8) | (data[1] << 0);
	
	y1_pos = (data[2] << 8) | (data[3] << 0);
	
	x2_pos = (data[4] << 8) | (data[5] << 0);
	
	y2_pos = (data[6] << 8) | (data[7] << 0);*/
}

void calcMidPos(){
	/*	Anv�nds d� navigeringsgruppen ger oss 2 s�tt av koordinater
		mid_x = (x1_pos+x2_pos)/2;
		mid_y = (y1_pos+y2_pos)/2;
	*/
	mid_x = x1_pos;
	mid_y = y1_pos;
	
}

double distanceToPosition(uint8_t obj){
	valuesCalc(obj);
	distanceLeft = sqrt(pow(deltaX,2)+pow(deltaY,2));
	return distanceLeft;
}

int angleToPos(){
	platformAngle = currentAngle;
	objectAngle = 180 - angle;
	angleVal = abs(((abs(platformAngle-360) + objectAngle)%360) - 360);
	
	(angleVal > 180) ? (angleVal -= 360) : (0);
	return angleVal;
}

void valuesCalc(uint8_t obj){
	calcMidPos();
	deltaX = objects[obj].x_pos - mid_x;
	deltaY = objects[obj].y_pos - mid_y;
	angleRad = atan2(deltaY,deltaX);
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

uint16_t last_x;
uint16_t last_y;
void angleCheck(){
	last_x = mid_x;
	last_y = mid_y;
	calcMidPos();
	deltaX = mid_x - last_x;
	deltaY = mid_y - last_y;
	angleRad = atan2(deltaY,deltaX);
	angleTemp = (angleRad*180)/PI;
	
	if (abs(angle-currentAngle)>4)
	{
		stop();
		currentAngle = angle;
		rotationChooser(angleToPos());
		
	}
	
}

void rotationChooser(int degreesToPos){
	if (degreesToPos<0){
		degreesToPos = abs(degreesToPos);
		rotateRightByDegrees(degreesToPos);
		updateAngle();
		} else{
		rotateLeftByDegrees(degreesToPos);
		updateAngle();
	}
}
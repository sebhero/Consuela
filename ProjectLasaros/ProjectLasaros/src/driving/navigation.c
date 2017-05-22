/*
 * navigation.c
 *
 * Created: 03/05/2017 08:59:50
 *  Author: Robin Johnsson
 *
 *	Contains the navigation functions used by the platform.
 *	The functions use angles, positions and distances to calculate what should be done.
 *
 *	@TODO	Clean up code, remove variables that aren't needed or those that are just duplicates
 */ 
#include "navigation.h"
#include "com/Com.h"
//Start of variables
//Coordinates of the platform
int x1_pos = 0;
int y1_pos = 0;
int x2_pos = 0;
int y2_pos = 0;

//Can be removed?
double tempVariabel;
uint8_t data[8];
uint8_t temp;


uint8_t dropoffAmount= 0;
double distanceLeft;
int degreesToPos;
static int currentAngle = 90;
double totalLength = 0;
int radius = 80;

uint8_t status = 0;
uint8_t objIndex = 0;
uint8_t getAllObj = 0;
Bool suspendNav = false;
uint8_t travelPath[8];


//Calculation variables
int deltaX;
int deltaY;
int ek;

//Different angle variables
int angle;
double angleRad;
int platformAngle;
int objectAngle;
int angleVal;
int angleTemp;


//Variables for the former position of the platform
int last_x;
int last_y;

//Mid values of the platform, will be x1 and y1 if only one set of coordinates is given
int mid_x;
int mid_y;

// Struct used to define objects/positions
	typedef struct {
		int x_pos;
		int y_pos;
		uint8_t name;
		} object_poss_t;

//Array of objects
/*
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
};*/
object_poss_t objects[8];

/*
	Sets the correct objects with their coordinates and object info
*/
void setObjectSimple(objectinfo_t theObj){
	setObject(theObj.theObject, theObj.xpos, theObj.ypos);
}

/*
	Sets the correct objects with their coordinates and object info
*/
void setObject(Object obj, int x, int y){
	switch(obj){
		case SOCK:
		objects[2].x_pos = x;
		objects[2].y_pos = y;
		objects[2].name = obj;
		break;
		case SQUARE:
		objects[3].x_pos = x;
		objects[3].y_pos = y;
		objects[3].name = obj;
		break;
		case GLASS:
		objects[4].x_pos = x;
		objects[4].y_pos = y;
		objects[4].name = obj;
		break;	
		default:
		break;
	}
	objects[0].x_pos  = 50;
	objects[0].y_pos = 0;
	objects[0].name = 0;
}

/*
	Decides if the arm can pick up all objects before dropoff, or if it needs to take one at a time
*/
void setCollectAll(uint8_t getAll){
	getAllObj = getAll;
}

/*
	Sets a suspension variable to false
	done after a pickup
*/
void setDonePickup(){
	suspendNav = false;
}

/*
	Sets a suspension variable to false
	done after a dropoff
*/
void setDropoffDone(){
	suspendNav = false;
}

void initNav(){
	currentAngle = 90;
	x1_pos = 0;
	y1_pos = 0;
}

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
	//Negative value = turn right, positive = turn left
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
	
	//Testing
	last_x = mid_x;
	last_y = mid_y;
	
	
	mid_x = mid_x+(-(cos(angleRad) * hyp));
	mid_y = mid_y+(sin(angleRad) * hyp);
	x1_pos = mid_x;
	y1_pos = mid_y;
}

/*
	Checks if the angle that the platform actually travels in is the same as the believed one.
	If they differ to much, the platform will stop and recalibrate to fix this.
	
	Still not tested! 
*/
void angleCheck(){
	//Saves the last position
	//last_x = mid_x;
	//6last_y = mid_y;
	
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
		printf("Fixing angle\n");
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
		//rotateRightByDegrees(degreesToPos);
		rotateRight(degreesToPos);
		//Update the angle
		updateAngle();
	} else{
		//Makes the value into a positive value
		//Rotates the platform anti-clockwise by the input value
		//rotateLeftByDegrees(degreesToPos);
		rotateLeft(degreesToPos);
		//Update the angle
		updateAngle();
	}
	
	
}

void setPath(){
	if (getAllObj){
		travelPath[0] = SQUARE;
		travelPath[1] = SOCK;
		travelPath[2] = GLASS;
		travelPath[3] = 0;
		printf(" %d %d %d %d", travelPath[0],travelPath[1],travelPath[2],travelPath[3]);
	} 
	else{
		travelPath[0] = SQUARE;
		travelPath[1] = 0;
		travelPath[2] = SOCK;
		travelPath[3] = 0;
		travelPath[4] = GLASS;
		travelPath[5] = 0;
		printf(" %d %d %d %d %d %d\n", travelPath[0],travelPath[1],travelPath[2],travelPath[3],travelPath[4],travelPath[5]);
	}
}

/*
	Runs EVERYTHING that has to do with navigation.
	Decides on a travel path if none is existent
	Turns the platform towards its destination
	Moves the platform towards the destination
	Stops when has approached the destination
	Repeats until all destinations has been reached
*/
uint8_t goToNext(){
	printf("currentAngle, objAngle: %d %d\n", currentAngle, objectAngle);
	if (!travelPath[0]){
		//printf("path: ");
		setPath();
		objIndex = 0;
		status = 6;
	}
	if (status){
		valuesCalc(travelPath[objIndex]);
		degreesToPos = angleToPos();
		rotationChooser(degreesToPos);
		totalLength = distanceToPosition(travelPath[objIndex]);
		status = 0;
	}
	//printf("distance left: %d\n",(int)distanceToPosition(travelPath[objIndex]) );
	if (distanceToPosition(travelPath[objIndex])>radius){
		if (distanceToPosition(travelPath[objIndex])<(totalLength/2)){
			angleCheck();
			totalLength = 0;
		}
		ek = counterA - counterB;
		tempVariabel = counterA*1.355;
		wheelControl(ek);
		//H�mta nya koordinater
 		updatePos(tempVariabel);
 		tempVariabel = 0;

		int x1=0;
		int y1=0;
		
		int x2=0;
		int y2=0;

//todo fix me
		//twi_navGetXY(XY1,&x1,&y1);
		//twi_navGetXY(XY2,&x2,&y2);
		
		printf("x1 = %d y1= %d\n",x1,y1);
		printf("x2 = %d y2= %d\n",x2,y2);

		/*
		twi_navGetXY(XY1,&x1_pos,&y1_pos);
		twi_navGetXY(XY2,&x2_pos,&y2_pos);
		*/
		status = 0;
		return status;
	}
	if (travelPath[objIndex]){
		suspendNav = true;
		status = 1;
		objIndex++;
		stop();
		return status;
	}
	stop();
	suspendNav = true;
	status = 2;
	if(getAllObj)
	{
		//IS alla done!!
		return 2;
	}
	else
	{
		dropoffAmount++;
		if(dropoffAmount >3)
			return 3;
	}
	objIndex++;
	return status;
}
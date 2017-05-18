/*
 * CFile1.c
 *
 * Created: 07/04/2017 12:53:10
 *  Author: Robin Johnsson
 *
 *
 *	File containing motor related functions
 */ 
#include <asf.h>
#include "motorFunc.h"
#include "pulseCounterHandler.h"

double distanceToMove;
double kP = 1;
uint16_t bS= baseSpeed;
uint16_t bSL= baseSpeedLeft;

/*
	Sends a pulse on the designated pin that varies
	with the "motorSpeed" input
*/
void pulse(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
};

/*
	Sets both motors to forward direction until the desired
	distance has been covered
*/
void forwardDrive(int distance){
	stop();
	distanceToMove = distance/1.355;
	counterA = 0;
	counterB = 0;
	
	delay_ms(8);
	pulse(1600);
	delay_us(motorSwitch);
	pulse(1560);
	
	while (counterA<distanceToMove)
	{
		delay_ms(1);
	}
	stop();
	counterA = 0;
	counterB = 0;
}

/*
	Sets both motors to backwards direction until the desired
	distance has been covered
*/
void reverseDrive(int distance){
	stop();
	distanceToMove = distance/1.355;
	counterA = 0;
	counterB = 0;
	
	pulse(reverseBaseSpeed);
	delay_us(motorSwitch);
	pulse(reverseBaseSpeed);
	
	while (counterA<distanceToMove)
	{
		delay_ms(1);
	}
	
	stop();
	counterA = 0;
	counterB = 0;
}

/*
	Stops both of the motors
*/
void stop(){
	pulse(1500);
	delay_us(motorSwitch);
	pulse(1500);
	delay_ms(timeOut);
}

/*
	Splits up the rotation into segments of a maximum of 
	45 degrees at a time
*/
void rotateRight(int degree){
	while(degree>45)
	{
		degree = degree-45;
		rotateRightByDegrees(45);
	}
	rotateRightByDegrees(degree);
}

/*
	Rotates the platform to the right (clockwise) for
	desired amount of degrees
*/
void rotateRightByDegrees(int degree){
	stop();
	degree=degree*1.1;
	degree=(degree/4)-1;
	degree = max(degree,0);
	if (degree>0)
	{
		
		pulse(reverseBaseSpeed);
		delay_us(motorSwitch);
		pulse(baseSpeedLeft);
	}
	
	
	counterA = 0;
	counterB = 0;
	
	while((counterA<degree)){
		delay_ms(1);
	}
	
	stop();
	counterA = 0;
	counterB = 0;
}

/*
	Splits up the rotation into segments of a maximum of 
	45 degrees at a time
*/
void rotateLeft(int degree){
	while(degree>45)
	{
		degree = degree-45;
		rotateLeftByDegrees(45);
	}
	rotateLeftByDegrees(degree);
}

/*
	Rotates the platform to the left (anti-clockwise) for
	desired amount of degrees
*/
void rotateLeftByDegrees(int degree){
	
	stop();
	degree = degree*1.05;
	degree=(degree/4)-1;
	
	degree = max(degree,0);
	if (degree>0)
	{
		pulse(baseSpeed);
		delay_us(motorSwitch);
		pulse(reverseBaseSpeed);
	}
	counterA = 0;
	counterB = 0;
	
	while((counterA<degree)){
		delay_ms(1);
	}
	stop();
	counterA = 0;
	counterB = 0;
}

/*
	Controls the two motors so that they keep the same speed
	This is done by checking the difference of the two counters
	on the wheels, and then adding the difference to the left
	wheel.
*/
void wheelControl(int ek){
	//Resets the counters
	counterA = 0;
	counterB = 0;
	
	//Updates the left wheels speed
	bSL = bSL+(ek*kP);
	
	pulse(bS);
	delay_us(motorSwitch);
	pulse(bSL);
}
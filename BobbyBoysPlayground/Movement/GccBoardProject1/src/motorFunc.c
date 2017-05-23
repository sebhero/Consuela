/*
 * CFile1.c
 *
 * Created: 07/04/2017 12:53:10
 *  Author: Robin Johnsson
 *
 *
 *	Alot of these will be changed when integrated with RTOS.
 *	The delays etc will be removed since RTOS will call functions correctly
 *
 */ 
#include <asf.h>
#include "motorFunc.h"
#include "pulseCounterHandler.h"

double distanceToMove;
double kP = 1;
uint16_t bS= baseSpeed;
uint16_t bSL= baseSpeedLeft;

//Sends the pulse to the engine
void pulse(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
};

//Sets both engines to go the same direction with the "same" speed
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

//Sets both engines to go the same direction with the "same" speed (reverse)
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

//Rotates the platform by setting the directions of the engines the opposite of each other
//@Depricated
void rotate(){
	pulse(baseSpeed);
	delay_us(motorSwitch);
	pulse(reverseBaseSpeed);
	delay_ms(timeOut);
}

//Stops both of the engines
void stop(){
	pulse(1500);
	delay_us(motorSwitch);
	pulse(1500);
	delay_ms(timeOut);
}

//@Depricated
void turnLeft(){
	pulse(baseSpeed);
	delay_us(motorSwitch);
	pulse(baseSpeed+100);
	delay_ms(timeOut);
}

//@Depricated
void turnRight(){
	pulse(baseSpeed+100);
	delay_us(motorSwitch);
	pulse(baseSpeed);
	delay_ms(timeOut);
}

void rotateRight(int degree){
	while(degree>45)
	{
		//printf("degrees left: %d\n",degree);
		degree = degree-45;
		rotateRightByDegrees(45);
	}
	if (degree<8)
	{
		degree = degree+(8-degree);
	}
	//printf("degrees to turn the final time: %d\n",degree);
	rotateRightByDegrees(degree);
}


void rotateRightByDegrees(int degree){
	stop();
	degree=degree*1.07;
	degree=(degree/4)-1;
	degree =max(degree,0);
	
	counterA = 0;
	counterB = 0;
	
	if (degree>0)
	{
	pulse(reverseBaseSpeed);
	delay_us(motorSwitch);
	pulse(baseSpeedLeft);
	}
	
	
	while((counterA<degree)){
		delay_ms(1);
	}
	
	stop();
	counterA = 0;
	counterB = 0;
}

void rotateLeft(int degree){
	while(degree>45)
	{
		//printf("degrees left: %d\n",degree);
		degree = degree-45;
		rotateLeftByDegrees(45);
	}
	
	if (degree<8)
	{
		degree = degree+(8-degree);
	}
	//printf("degrees to turn the final time: %d\n",degree);
	rotateLeftByDegrees(degree);
}

void rotateLeftByDegrees(int degree){
	stop();
	degree = degree*1.07;
	degree=(degree/4)-1;
	degree =max(degree,0);
	
	counterA = 0;
	counterB = 0;
	
	if (degree>0)
	{
		pulse(baseSpeed);
		delay_us(motorSwitch);
		pulse(reverseBaseSpeed);
	}
	
	
	while((counterA<degree)){
		delay_ms(1);
	}
	stop();
	counterA = 0;
	counterB = 0;
}

void wheelControl(int ek){
	counterA = 0; //Nollställer räknarna
	counterB = 0;
	bSL = bSL+(ek*kP);
	pulse(bS);
	delay_us(motorSwitch);
	pulse(bSL);
}
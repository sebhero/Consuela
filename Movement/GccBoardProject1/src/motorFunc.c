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

//Sends the pulse to the engine
void pulse(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
};

//Sets both engines to go the same direction with the "same" speed
void forwardDrive(){
	pulse(baseSpeed);
	delay_us(motorSwitch);
	pulse(baseSpeedLeft);
	delay_ms(100);
}

//Sets both engines to go the same direction with the "same" speed (reverse)
void reverseDrive(){
	pulse(reverseBaseSpeed);
	delay_us(motorSwitch);
	pulse(reverseBaseSpeed);
	delay_ms(timeOut);
}

//Rotates the platform by setting the directions of the engines the opposite of each other
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

void turnLeft(){
	pulse(baseSpeed);
	delay_us(motorSwitch);
	pulse(baseSpeed+100);
	delay_ms(timeOut);
}

void turnRight(){
	pulse(baseSpeed+100);
	delay_us(motorSwitch);
	pulse(baseSpeed);
	delay_ms(timeOut);
}

void rotateRightByDegrees(int degree){
	stop();
	degree=degree*1.1;
	degree=(degree/4)-1;
	
	pulse(reverseBaseSpeed);
	delay_us(motorSwitch);
	pulse(baseSpeedLeft);
	
	counterA = 0;
	counterB = 0;
	
	while((counterA<degree)){
		delay_ms(1);
	}
	
	stop();
	counterA = 0;
	counterB = 0;
}

void rotateLeftByDegrees(int degree){
	
	stop();
	degree = degree*1.05;
	degree=(degree/4)-1;
	
	pulse(baseSpeed);
	delay_us(motorSwitch);
	pulse(reverseBaseSpeed);
	
	counterA = 0;
	counterB = 0;
	
	while((counterA<degree)){
		delay_ms(1);
	}
	stop();
	counterA = 0;
	counterB = 0;
}
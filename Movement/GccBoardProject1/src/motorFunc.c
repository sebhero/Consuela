/*
 * CFile1.c
 *
 * Created: 07/04/2017 12:53:10
 *  Author: Robin_2
 */ 
#include <asf.h>
#include "motorFunc.h"

uint16_t baseSpeed = 1750;
uint16_t reverseBaseSpeed = 1250; 

//Sends the pulse to the motor
void pulse(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
};

//Sets both motors to go the same direction with the "same" speed
void forwardDrive(){
	pulse(baseSpeed);
	delay_us(1100);
	pulse(baseSpeed);
}

void reverseDrive(){
	pulse(reverseBaseSpeed);
	delay_us(1100);
	pulse(reverseBaseSpeed);
}

//Rotates the platform by setting the directions of the motors the opposite
void rotate(){
	pulse(baseSpeed);
	delay_us(1100);
	pulse(reverseBaseSpeed);
}

void stop(){
	pulse(1500);
	delay_us(1100);
	pulse(1500);
}

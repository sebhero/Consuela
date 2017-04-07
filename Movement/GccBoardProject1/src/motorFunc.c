/*
 * CFile1.c
 *
 * Created: 07/04/2017 12:53:10
 *  Author: Robin_2
 */ 
#include "motorFunc.h"
#include <inttypes.h>
static uint16_t baseSpeed = 1750;
static uint16_t reverseBaseSpeed = 1500 -(baseSpeed - 1500); 

void pulse(uint16_t motorSpeed){
	ioport_set_pin_level(pin24, HIGH);
	delay_us(motorSpeed);
	ioport_set_pin_level(pin24, LOW);
};

void forwardDrive(){
	pulse(baseSpeed);
	delay_us(1100);
	pulse(baseSpeed);
}

void rotate(){
	pulse(baseSpeed);
	delay_us(1100);
	pulse(reverseBaseSpeed);
}

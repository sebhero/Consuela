/*
 * regulation.c
 *
 * Created: 13/04/2017 14:44:54
 *  Author: Robin_2
 */ 

#include "regulation.h"
#include "distanceSensor.h"
#include "motorFunc.h"

double kValue = 500.0/270.0;


uint16_t calculateSpeed(uint16_t distanceArray[]){
	uint16_t dist = 0;
	uint8_t i;
	for (i = 0; i < 5; ++i) {
		dist += distanceArray[i];
	}
	dist = dist/4;
	uint16_t speed = dist * kValue;
	speed =1500 + MIN(speed,500);
	return speed;
}

void regulatedForward(uint16_t speedLeft, uint16_t speedRight){
	pulse(speedRight);
	delay_us(motorSwitch);
	pulse(speedLeft);
	delay_ms(10);
}
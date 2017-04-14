/*
 * distanceSensor.c
 *
 * Created: 10/04/2017 10:32:36
 *  Author: Robin_2
 */ 
#include "distanceSensor.h"


unsigned long duration, distance;
//unsigned double kValue = 500.0/270.0;


long pulseIn()
{
	//printf("pulseread");
	//timeout zone
	unsigned long numloops = 0;
	unsigned long maxloops = 5000000;
	unsigned long width = 0;
	// wait for any previous pulse to end
	while ( ioport_get_pin_level(echo) == HIGH)
	{
		if (numloops++ == maxloops) break;
	}
	// wait for the pulse to start
	while (ioport_get_pin_level(echo)== LOW)
	{
		if (numloops++ == maxloops) break;
	}
	
	// wait for the pulse to stop @ here we are measuring the pulse width = incrementing the WIDTH value by one each cycle. atmega328 1 micro second is equal to 16 cycles.
	while (ioport_get_pin_level(echo)==HIGH)
	{
		if (numloops++ == maxloops) break;
		width++;
	}
	return width/5.1;
}

long distance_forward(){
	ioport_set_pin_level(trig, LOW);
	delay_us(2);
	ioport_set_pin_level(trig, HIGH);
	delay_us(10);
	ioport_set_pin_level(trig, LOW);
	duration = pulseIn();
	distance = duration / 58;
	return distance;
}

/*
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
*/
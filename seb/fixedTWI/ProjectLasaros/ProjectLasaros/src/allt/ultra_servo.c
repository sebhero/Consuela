/*
 * ultra_servo.c
 *
 * Created: 2017-05-10 10:04:19
 *  Author: Johnatan
 */ 

#include <asf.h>
#include <delay.h>
#include <stdio.h>
#include <stdio_serial.h>
#include "ultra_servo.h"

uint8_t static preDist = 0;
uint8_t static mappingDist = 0;
uint8_t static mappingAngle = 0;
uint8_t static status = STATUS_SEARCHING;
uint32_t static pulseWidth = 0;
uint16_t static sum = 0;
uint8_t static errors = 0;
uint16_t static tick = 0;
uint8_t static angle = 0;
uint8_t static reset_tick = 0;
uint8_t WITHIN_RANGE_FLAG = 0;

int distanceUltraSensor = 0;
int angleUltraSensor = 0;

long pulseIn()
{
	//printf("pulseread");
	//timeout zone
	unsigned long numloops = 0;
	unsigned long maxloops = 250000;
	unsigned long width = 0;
	// wait for the pulse to start
	while (ioport_get_pin_level(echo)== LOW)
	{
		if (numloops++ == maxloops)
		{
			printf("brokenLOW\n");
			break;
		}
	}
	
	// wait for the pulse to stop @ here we are measuring the pulse width = incrementing the WIDTH value by one each cycle. atmega328 1 micro second is equal to 16 cycles.
	while (ioport_get_pin_level(echo)==HIGH)
	{
		if (numloops++ == maxloops)
		{
			printf("brokenHIGH: Too far away\n");
			break;
		}
		width++;
	}
	return width/scaling;
}

void servoControll(unsigned long dist)
{
	switch(status){
		
		case STATUS_SEARCHING:
			WITHIN_RANGE_FLAG = 0;
			if(preDist>0 && dist<70)
			{
				mappingDist = dist;
				status = STATUS_FOUND;
				mappingAngle = angle;
				angleUltraSensor = mappingAngle;
				tick++;
				printf("Mapping started\n");
			}
			
			angle++;
		break;
		
		//case status_mapping:
			//within_range_flag = 0;
			//if(dist >= (mappingdist*0.80) && dist <= (mappingdist*1.20)){
			//
				//printf("%d / 5 readings within mapping distance\n", tick);
				//tick++;
				////sum = sum + deg;
			//}
		//
			//if(tick>5){
				//status = status_found;
				//mappingangle = angle; //onödig?
				//printf("found something at angle: %d degrees\n", angle);
				//printf("the distance to object is: %d cm\n", dist);
				//distanceultrasensor = (int) dist;
				//angleultrasensor = angle;
				//tick = 0;
			//} else if(angle > (mappingangle + 10))
			//{
				//printf("too few readings within mapping distance\n");
				//tick = 0;
				//status = status_searching;
				//printf("started search\n");
			//}
		//
			//angle++;
		//break;
		
		case STATUS_FOUND:
			distanceUltraSensor = (int) mappingDist;
			WITHIN_RANGE_FLAG = 1; // object found, alert Controller-task
			if(dist < (mappingDist*0.80) || dist > (mappingDist*1.20) || dist > 60)
			{
				printf(" %d / 5 readings divert to much from mapping distance\n", errors);
				errors++;
				if(errors >5)
				{
					errors = 0;
					printf("Object lost; starting search \n");
					status = STATUS_SEARCHING;
				}
				break;
			}
			errors = 0;
		break;
		
		case ANGLE_RESET:
			WITHIN_RANGE_FLAG = 0;
			angle = 0;
			reset_tick++;
			if (reset_tick == 30)
			{
				reset_tick = 0;
				status = STATUS_SEARCHING;
			}
		
	}
	

	if(angle > 180)
	{
		angle = 0;
		status = ANGLE_RESET;
	}
	servoDirection(angle);
	preDist = dist;
}

void servoDirection(uint8_t angle)
{
	
	//printf("Angle: %d\n", angle);
	pulseWidth = ((angle*10.6) + 500);
	ioport_set_pin_level(servo, HIGH);
	delay_us(pulseWidth);
	ioport_set_pin_level(servo, LOW);
	delay_us(20000);
	//printf("pulseWidth: %d\n", pulseWidth);
}

void testingUltraSound()
{
	unsigned long distance;
	
	ioport_set_pin_level(trig, LOW);
	delay_us(2);
	ioport_set_pin_level(trig, HIGH);
	delay_us(10);
	ioport_set_pin_level(trig, LOW);
	distance = pulseIn();
	//distance = 0;
	//printf("%d cm\n", distance);
	servoControll(distance);
	
	
}


void testServo(void)
{
	
	if(angle > 180) angle = 0;
	pulseWidth = 0;
	servoDirection(angle);
	if(angle == 180)
	{
		delay_ms(400);
	}
	angle++;
}
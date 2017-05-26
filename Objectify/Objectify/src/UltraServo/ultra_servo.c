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
#include <spi.h>
#include <spi_master.h>
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
	
	// pin for echo is set high when waiting for the sound the return. While the soundpulse is sent numloops is increasing. If it reaches
	//the maxloops the message "brokenHIGH" is written to indicate that the sensor hasn't retrieved the soundpulse. This can be seen as a timeout 
	// to make sure the program won't get stuck in the loop.
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
/**
This method is used to controll the servo. It has four states:
STATUS_SEARCHING: in this state the servo is waiting for input to the ultrasound sensor.
				  While in this state, the servo moves one degree each loop. When an object have been detekted
				  it moves on to STATUS_MAPPING
				  
STATUS_MAPPING:	  If an object have been detekted the program has to make sure it is actually an object and not
				  bad readings. To make sure it's an object, while in this state, the servo keeps moving and more input
				  is being checked from the ultrasound sensor to see if the next values are similar to the previous ones.
				  if there are at least 5 more readings in the same interval it is considered a found object.
				  
STATUS_FOUND	  If an object have been flagged as found, it enters this state. Here the servo stoppes moving and focuses on the object
				  It keeps checking the distance and regularely updates it. If atleast 5 readings in a row are stray from the mapped distance
				  of the found object it is considered lost and goes back to searching.
				  
ANGLE_RESET		  If the servo gets to 180 degrees without finding an object it resets to 0 degrees and starts the search again.

@param dist Distance from ultrasound sensor.
				  
**/
void servoControll(unsigned long dist)
{
	switch(status){
		
		case STATUS_SEARCHING:
		
		if(preDist>0 && dist <= (preDist/1.2) && dist<60)
		{
			mappingDist = dist;
			status = STATUS_MAPPING;
			mappingAngle = angle;
			tick++;
			printf("Mapping started\n");
		}
		
		angle++;
		break;
		
		case STATUS_MAPPING:
		
		if(dist >= (mappingDist*0.80) && dist <= (mappingDist*1.20)){
			
			printf("%d / 2 readings within mapping distance\n", tick);
			tick++;
			//sum = sum + deg;
		}
		
		if(tick>2){
			status = STATUS_FOUND;
			mappingAngle = angle;
			printf("Found something at angle: %d degrees\n", angle);
			printf("The distance to object is: %d cm\n", dist);
			tick = 0;
		} else if(angle > (mappingAngle + 10))
		{
			printf("Too few readings within mapping distance\n");
			tick = 0;
			status = STATUS_SEARCHING;
			printf("Started Search\n");
		}
		
		angle++;
		break;
		
		case STATUS_FOUND:
		
		if(dist < (mappingDist*0.80) || dist > (mappingDist*1.20) || dist > 60)
		{
			printf(" %d / 5 readings divert too much from mapping distance\n", errors);
			errors++;
			if(errors > 5)
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
/**
This method decides how the servo will move. It uses the increasing angle (0 - 180) and multiplies it
with 10.6 whereof it then adds it to 500. This translates an angle between 0 - 180 to a pulsewidth to the servo. Which in turn
rotates to the desired angle.

@param angle The angle the servo should rotate to.
**/
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


/**
This method starts the ultrasound - servo combination.
Reads input from ultrasound and controlls the servo.
**/
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
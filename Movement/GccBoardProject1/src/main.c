/**
 * Main file for testing different movement things for motors
 *	Author: Robin Johnsson, Viktor Malmgren, Daniel M
 *
 */
#include <asf.h>
#include <inttypes.h>
#include <stdio_serial.h>
#include "consoleFunctions.h"
#include "conf_board.h"
#include "motorFunc.h"
#include "distanceSensor.h"
#include "pulseCounterHandler.h"
#include "buttonInterrupt.h"
#include "navigation.h"


int main (void)
{
	sysclk_init();
	board_init();
	configureConsole();
	
	
	//Makes pin 24 on the Due-board an output
	ioport_set_pin_dir(pin24,IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(PIO_PD1_IDX,IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(trig,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
	//Starts with a delay simply to reduce the chance of an error occuring when reseting the program.
	delay_ms(2000);

	
	//	Loop that runs the functions called upon from the "motorFunc.h" file
	//	It is recommended that the "stop()" function is called between different sets of commands ex:
	//	forwardDrive, then stop, then reverseDrive.
	//	DO NOT go: forwardDrive into reverseDrive	
	//	This is to ensure that the motors don't get damaged.
	
	uint8_t foo = 0;
	int degreesToPos;
	double tempVariabel = 0;
	double totalLength = 0;
	double someVal=0;
	/*
	while(foo<4){
		
		valuesCalc(foo);
		degreesToPos = angleToPos();
		rotationChooser(degreesToPos);
		someVal = totalLength = distanceToPosition(foo);
		while (someVal>60.0){
			printf("distance left: %d\n",(int)someVal);
			if (someVal<(totalLength/2)){
				angleCheck();
				totalLength = 0;
			}
			delay_ms(500);
			int ek = counterA-counterB;
			tempVariabel = counterA*1.355;
			reglerahjul3(ek);
			updatePos(tempVariabel);
			tempVariabel = 0;
			someVal = distanceToPosition(foo);
			delay_ms(8);
		}
		foo++;
		stop();
	} */
	setObject(SQUARE,100,300);
	setObject(SOCK, 300, 300);
	setObject(GLASS, 300, 100);
	setCollectAll(0);
	while (1)
	{
		goToNext();
		delay_ms(500);
	}
	return 0;
}
/**
 * Main file for testing different movement things for motors
 *	Author: Robin Johnsson, Viktor Malmgren
 *
 */
#include <asf.h>
#include <inttypes.h>
#include "motorFunc.h"


int main (void)
{
	sysclk_init();
	board_init();
	//Makes pin 24 on the Due-board an output
	ioport_set_pin_dir(pin24,IOPORT_DIR_OUTPUT);
	
	//Starts with a delay simply to reduce the chance of an error occuring when reseting the program.
	delay_ms(2000);

	/*
		Loop that runs the functions called upon from the "motorFunc.h" file
		It is recommended that the "stop()" function is called between different sets of commands ex:
		forwardDrive, then stop, then reverseDrive.
		DO NOT go: forwardDrive into reverseDrive	
		This is to ensure that the motors don't get damaged.
	*/
	//Starting with a stop(); command is also advisable, as to not run into problems when doing a reset
	while(1){
		stop();
		forwardDrive();
		stop();
		rotate();
		stop();
		reverseDrive();
		//HELP
	}
	return 0;
}


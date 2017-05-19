/*
 * Motorfunc.c
 *
 * Created: 2017-05-03 13:32:19
 *  Author: Viktor
 */ 
#include <asf.h>
#include "pulseCounterHandler.h"
#include "pulse.h"
#include "MotorfuncViktor.h"

#define pulseh_ch 0
#define pulsev_ch 1
#define pulseh_timer 0
#define pulsev_timer 1

double distanceToMove;

void stop(){
	pulse_set_period(pulseh_ch, 1500);
	pulse_set_period(pulsev_ch, 1500);
}

void rotateRightByDegrees(double degree){
	stop();
	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
	degree=degree*1.1;
	degree=(degree/4)-1;
	
	pulse_set_period(pulseh_ch, 1290); 
	pulse_set_period(pulsev_ch, 1660); 
	
	counterA = 0;
	counterB = 0;
	while((counterA<degree)){
		delay_ms(1);
	}
	stop();

	counterA = 0;
	counterB = 0;
}

void rotateLeftByDegrees(double degree){
	printf("Inside rotateLeft");
	stop();
	degree = degree*1.05;
	degree=(degree/4)-1;
	
	pulse_set_period(pulseh_ch, 1700);
	pulse_set_period(pulsev_ch, 1310);
	
	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
	
	counterA = 0;
	counterB = 0;
	while((counterA<degree)){
		delay_ms(1);
	}
	stop();
	counterA = 0;
	counterB = 0;
}

//Sets both engines to go the same direction with the "same" speed
void forwardDrive(int distance){
	stop();
	
	distanceToMove = distance/1.355;
	counterA = 0;
	counterB = 0;
	printf("Driving");
	
	pulse_set_period(pulsev_ch, 1660);
	pulse_set_period(pulseh_ch, 1700);
	
	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);

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
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
	pulse_init();
	
	stop();
	distanceToMove = distance/1.355;
	counterA = 0;
	counterB = 0;
	
	pulse_set_period(pulseh_ch, 1280);
	pulse_set_period(pulsev_ch, 1280);
	
	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
	
	while (counterA<distanceToMove)
	{
		delay_ms(1);
	}
	
	stop();
	counterA = 0;
	counterB = 0;
}


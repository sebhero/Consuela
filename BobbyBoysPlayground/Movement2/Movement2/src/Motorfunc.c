/*
 * Motorfunc.c
 *
 * Created: 2017-05-03 13:32:19
 *  Author: Viktor
 */ 
#include <asf.h>
#include "pulseCounterHandler.h"
#include "buttonInterrupt.h"
#include "pulse.h"
#define pulseh_ch 0
#define pulsev_ch 1
#define pulseh_timer 0
#define pulsev_timer 1

void stop(){
	pulse_set_period(pulseh_ch, 1500);
	pulse_set_period(pulsev_ch, 1500);
	
}

void rotateRightByDegrees(double degree){

	stop();
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

	stop();
	degree = degree*1.05;
	degree=(degree/4)-1;
	
	pulse_set_period(pulseh_ch, 1700);
	pulse_set_period(pulsev_ch, 1310);
	
	counterA = 0;
	counterB = 0;
	while((counterA<degree)){
		delay_ms(1);
	}
	stop();
	counterA = 0;
	counterB = 0;
}


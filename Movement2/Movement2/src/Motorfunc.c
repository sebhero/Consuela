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
#include "TimerCounter.h"
#define pulseh_ch 0 //Vilken kanal höger motor ligger på
#define pulsev_ch 1 //Vilken kanal vänster motor ligger på
#define pulseh_timer 0
#define pulsev_timer 1

void stop(){
	pulse_set_period(pulseh_ch, 1500); //Pulslängden 1500 till höger motor stannar den
	pulse_set_period(pulsev_ch, 1500); //Pulslängden 1500 till vänster motor stannar den
	
}

void rotateRightByDegrees(double degree){ //Metod som gör att roboten roterar till höger de grader som matas in.
	
	
	stop(); //Stannar motorerna
	//double engradtal = 459461.4694444444; //används ej

	//degree=degree*engradtal; //Används ej
	
	degree = degree/4; //Delas med 4 eftersom det roteras 4 grader per puls till motorn.
	
	//Hastigheterna på hjulen är samma vid dessa pulslängder som skickas till motorerna.
	pulse_set_period(pulseh_ch, 1365); //Hastigheten på höger hjul blir satt här, är den under 1500 gör att den går baklänges.
	pulse_set_period(pulsev_ch, 1606); //Hastigheten på vänster hjul blir satt här, över 1500 gör att den går framlänges. 
	//uint32_t startspin  = 0;
	//TC0_init();
	//startspin = 0;	
	counterA = 0;
	counterB = 0; 
	//while((read_counter_value()<degree)){	
	//}
	while(counterA<degree){ //Roterar tills vi har kommit upp i rätt uträknade pulser
		delay_ms(1);
	}
	stop(); //Stannar motorerna
	//Nollställer räknarna
	counterA = 0;
	counterB = 0;
}

void rotateLeftByDegrees(double degree){ //Metod som gör att roboten roterar till vänster de grader som matas in.

	stop(); //Stannar motorerna
	//degree = degree*1.05;
	//degree=(degree/4)-1;
	
	degree = degree*1;
	degree=(degree/4);
	
	//pulse_set_period(pulseh_ch, 1700);
	//pulse_set_period(pulsev_ch, 1310);
	
	pulse_set_period(pulseh_ch, 1635); //Hastigheten på höger hjul blir satt här, över 1500 gör att den går framlänges. 
	pulse_set_period(pulsev_ch, 1355); //Hastigheten på vänster hjul blir satt här, under 1500 gör att den går baklänges. 
	
	//Nollställer räknarna
	counterA = 0; 
	counterB = 0;
	while((counterA<degree)){ //Roterar tills vi har kommit upp i rätt uträknade pulser
		delay_ms(1);
	}
	stop(); //Stannar motorerna
	//Nollställer räknarna
	counterA = 0;
	counterB = 0;
}


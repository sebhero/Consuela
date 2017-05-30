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
#define pulseh_ch 0 //Vilken kanal h�ger motor ligger p�
#define pulsev_ch 1 //Vilken kanal v�nster motor ligger p�
#define pulseh_timer 0
#define pulsev_timer 1

void stop(){
	pulse_set_period(pulseh_ch, 1500); //Pulsl�ngden 1500 till h�ger motor stannar den
	pulse_set_period(pulsev_ch, 1500); //Pulsl�ngden 1500 till v�nster motor stannar den
	
}

void rotateRightByDegrees(double degree){ //Metod som g�r att roboten roterar till h�ger de grader som matas in.
	
	
	stop(); //Stannar motorerna
	//double engradtal = 459461.4694444444; //anv�nds ej

	//degree=degree*engradtal; //Anv�nds ej
	
	degree = degree/4; //Delas med 4 eftersom det roteras 4 grader per puls till motorn.
	
	//Hastigheterna p� hjulen �r samma vid dessa pulsl�ngder som skickas till motorerna.
	pulse_set_period(pulseh_ch, 1365); //Hastigheten p� h�ger hjul blir satt h�r, �r den under 1500 g�r att den g�r bakl�nges.
	pulse_set_period(pulsev_ch, 1606); //Hastigheten p� v�nster hjul blir satt h�r, �ver 1500 g�r att den g�r framl�nges. 
	//uint32_t startspin  = 0;
	//TC0_init();
	//startspin = 0;	
	counterA = 0;
	counterB = 0; 
	//while((read_counter_value()<degree)){	
	//}
	while(counterA<degree){ //Roterar tills vi har kommit upp i r�tt utr�knade pulser
		delay_ms(1);
	}
	stop(); //Stannar motorerna
	//Nollst�ller r�knarna
	counterA = 0;
	counterB = 0;
}

void rotateLeftByDegrees(double degree){ //Metod som g�r att roboten roterar till v�nster de grader som matas in.

	stop(); //Stannar motorerna
	//degree = degree*1.05;
	//degree=(degree/4)-1;
	
	degree = degree*1;
	degree=(degree/4);
	
	//pulse_set_period(pulseh_ch, 1700);
	//pulse_set_period(pulsev_ch, 1310);
	
	pulse_set_period(pulseh_ch, 1635); //Hastigheten p� h�ger hjul blir satt h�r, �ver 1500 g�r att den g�r framl�nges. 
	pulse_set_period(pulsev_ch, 1355); //Hastigheten p� v�nster hjul blir satt h�r, under 1500 g�r att den g�r bakl�nges. 
	
	//Nollst�ller r�knarna
	counterA = 0; 
	counterB = 0;
	while((counterA<degree)){ //Roterar tills vi har kommit upp i r�tt utr�knade pulser
		delay_ms(1);
	}
	stop(); //Stannar motorerna
	//Nollst�ller r�knarna
	counterA = 0;
	counterB = 0;
}


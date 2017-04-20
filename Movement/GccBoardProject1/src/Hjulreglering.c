/*
 * Hjulreglering.c
 *
 * Created: 2017-04-12 13:32:07
 *  Author: Viktor, Daniel, Robin
 */ 

#include <asf.h>
#include "Hjulreglering.h"
#include "motorFunc.h"
#include "pulseCounterHandler.h"
#include "buttonInterrupt.h"

uint16_t bS= baseSpeed;
uint16_t bSL= baseSpeedLeft;

/*
void reglerahjul(int ek){
	//int ek = counterA - counterB;
	if(ek>0){
		bS = bS - 10; 
		pulse(bS);
		delay_us(1100);
		pulse(1600);
		
	}
	else{
		bS = bS +10;
		pulse(bS);
		delay_us(1100);
		pulse(1600);
		
	}
	
}

*/
void reglerahjul2(int ek){ //Den som fungerar
	if(ek==0 && ((bS >= 1750) || (bSL >= 1750))){ //F�r att resetta hastigheten s� att det inte �kar o�ndligt.
		bS= 1600; //S�tter h�ger hjul till hastigheten 1600 
		bSL = 1600; //S�tter v�nster hjul till hastigheten 1600 
		
		counterA = 0; //Nollst�ller r�knarna
		counterB = 0;
	}
	
	if(ek>0){
		bSL = bSL+2; //Om h�ger hjul g�r f�r snabbt s� �kar vi hastigheten p� v�nster hjul
		pulse(bS);
		delay_us(1100);
		pulse(bSL);
	}
	
	else if(ek == 0){ 
		delay_us(1100);
		pulse(bSL);
		
	}
	else{
		bS = bS+3; bSL+2; //Om v�nster hjul g�r f�r snabbt s� �kar vi hastigheten p� h�ger hjul
		pulse(bS);
		delay_us(1100);
		pulse(bSL);
	}


}

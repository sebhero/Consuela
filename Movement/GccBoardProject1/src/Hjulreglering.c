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
	if(ek==0 && ((bS >= 1750) || (bSL >= 1750))){ //För att resetta hastigheten så att det inte ökar oändligt.
		bS= 1600; //Sätter höger hjul till hastigheten 1600 
		bSL = 1600; //Sätter vänster hjul till hastigheten 1600 
		
		counterA = 0; //Nollställer räknarna
		counterB = 0;
	}
	
	if(ek>0){
		bSL = bSL+4; //Om höger hjul går för snabbt så ökar vi hastigheten på vänster hjul
		pulse(bS);
		delay_us(1100);
		pulse(bSL);
	}
	
	else if(ek == 0){ 
		pulse(bS);
		delay_us(1100);
		pulse(bSL);
		
	}
	else{
		
		bS = bS+5; 
		//bSL = bSL+2; //Om vänster hjul går för snabbt så ökar vi hastigheten på höger hjul
		pulse(bS);
		delay_us(1100);
		pulse(bSL);
	}


}

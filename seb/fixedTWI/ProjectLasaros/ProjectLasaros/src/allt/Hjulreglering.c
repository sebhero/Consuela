/*
 * Hjulreglering.c
 *
 * Created: 2017-04-12 13:32:07
 *  Author: Viktor, Daniel, Robin
 */ 

#include <asf.h>
#include "Hjulreglering.h"
#include "pulseCounterHandler.h"
#include "pulse.h"

//uint16_t bS= baseSpeed;
//uint16_t bSL= baseSpeedLeft;

double kP = 1.0;
double kD = 1.0;
double kI = 1.0;
double i_del = 0.0;
double d_del = 0.0;
int ek_1 = 0;
double tS = 0.01;
uint8_t foo = 1;


#define pulseh_ch 0
#define pulsev_ch 1
#define pulseh_timer 0
#define pulsev_timer 1
uint32_t periodv = 1660; //1660 getp� v hjul ger runt 1700
uint32_t periodh = 1700;

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
/*
void reglerahjul2(int ek){ //Den som fungerar
	if(ek==0 && ((bS >= 1750) || (bSL >= 1750))){ //F�r att resetta hastigheten s� att det inte �kar o�ndligt.
		bS= 1600; //S�tter h�ger hjul till hastigheten 1600 
		bSL = 1600; //S�tter v�nster hjul till hastigheten 1600 
		
		counterA = 0; //Nollst�ller r�knarna
		counterB = 0;
	}
	
	if(ek>0){
		bSL = bSL+4; //Om h�ger hjul g�r f�r snabbt s� �kar vi hastigheten p� v�nster hjul
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
		//bSL = bSL+2; //Om v�nster hjul g�r f�r snabbt s� �kar vi hastigheten p� h�ger hjul
		pulse(bS);
		delay_us(1100);
		pulse(bSL);
	}


}
*/
//simpel P-reglering
void reglerahjul3(int ek){
	int fool = 1;
	counterA = 0; //Nollst�ller r�knarna
	counterB = 0;
	
	if (fool)
	{
		i_del = (i_del+ek)*kI/tS;
		d_del = (ek-ek_1)*tS/kD;
		fool = 0;
	}
	ek_1 = ek;
	
	periodh=min(2000, periodh);
	
	
	int uk = kP * (ek + i_del + d_del); //pid-regleringen
	periodv += uk;
	pulse_set_period(pulsev_ch, periodv); 
	pulse_set_period(pulseh_ch, periodh); 
	
}

//PID-reglering
//kI och kD �r inte framtagna p� ordentligt s�tt, utan bara satta till 1
//Detta �r en mockup p� hur regleringen kan se ut

/*
void pidReg(int ek){
	counterA = 0; //Nollst�ller r�knarna
	counterB = 0;
	
	if (foo)
	{
	i_del = (i_del+ek)*kI/tS;
	d_del = (ek-ek_1)*tS/kD;
	foo = 0;
	}
	ek_1 = ek;
	
	bSL += kP * (ek + i_del + d_del);
	bSL=min(2000, bSL);
	pulse(bS);
	delay_us(1200);
	pulse(bSL);
}
*/
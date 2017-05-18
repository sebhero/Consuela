
#include <asf.h>
#include "pulseCounterHandler.h"
#include "buttonInterrupt.h"
#include "pulse.h"
#include "TimerCounter.h"
#include "Motorfunc.h"
#include <math.h>
#include "navigation.h"
#include <inttypes.h>
#include "Hjulreglering.h"


#define pulseh_ch 0
#define pulsev_ch 1
#define pulseh_timer 0
#define pulsev_timer 1


// Define a name for the pulse and say which channel and period to use


void configure_console() {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

int main (void)
{
	sysclk_init();
	TC0_init();
	board_init();
	
	
	configure_console();
	printf("\nHello, World!");
	
	
	
	
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
	// Initialize the pulse channels
	pulse_init();
	// Set the period
	pulse_set_period(pulseh_ch, 1500); //Bägge hjulen startar stillastående
	pulse_set_period(pulsev_ch, 1500); //Bägge hjulen startar stillastående
	// Start the pulse output
	// Output will be on digital pin 35(PIO_PC3_IDX) for pulse channel 0
	// and on digital pin 37(PIO_PC5_IDX) for channel 1
	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
	

	
	uint32_t channel = 0;
	uint16_t periodv = 1365; //1660 getpå v hjul ger runt 1700
	uint16_t periodh = 1635; 
	double kp = 1; //Förstärkningen
	double kD = 1.0;
	double kI = 1.0;
	double i_del = 0.0; 
	double d_del = 0.0;
	int ek_1 = 0;
	double tS = 0.01;
	uint8_t fool = 1;
	
	double x = 300;
	double y = 300;
	double testdegrees = 0;

	
	pulse_set_period(pulsev_ch, periodv); //Sätter hastigheten på vänster hjul
	pulse_set_period(pulseh_ch, periodh); //Sätter hastigheten på höger hjul
	
	
	testdegrees = atan(y/x);
	testdegrees = (testdegrees*180)/(3.1414592);
	
	
	/*
	while(1) {
			
			
			
			ioport_get_pin_level(A);
			ioport_get_pin_level(B);
			int ek = counterA - counterB; //Counter A är höger hjul, counterB vänster
			
			printf("\n\rCOUNTERA: %lu", counterA);//Skriver ut räknarna och felvärdet i konsollen.
			printf("\n\rCOUNTERB: %lu", counterB);
			printf("\nEK: %d", ek);
			
			counterA = 0;
			counterB = 0;
			
			if (fool)
			{
				i_del = (i_del+ek)*kI/tS;
				d_del = (ek-ek_1)*tS/kD;
				fool = 0;
			}
			ek_1 = ek;
			
			periodh=min(2000, periodh);
			
			
			int uk = kp * (ek + i_del + d_del); //pid-regleringen 
			
			pulse_timer_start(pulseh_timer);
			pulse_timer_start(pulsev_timer);
			//delay_ms(50);
			uint32_t pulseh_us = pulse_timer_get(pulseh_timer); //Få ut pulstiden på höger hjul
			uint32_t pulsev_us = pulse_timer_get(pulsev_timer); //Få ut pulstiden på vänster hjul
			
			
			/*
			if(pulseh_us > pulsev_us) {
				periodh += 6;
			} else {
				periodh -= 5;
			}
			*/
			

			
			
			/*
			
			
			periodv += uk; //Lägger styrvärdet på vänster hjul
			
			if(periodh<1500){ //För att inte roboten ska köra backlänges eller fucka ur när vi startar.
				periodh = periodv;
			}
			pulse_set_period(pulsev_ch, periodv); //Sätter hastigheten på vänster hjul
			pulse_set_period(pulseh_ch, periodh); //Sätter hastigheten på höger hjul
			
			
			
			//printf("\n\rPulse h length: %lu", pulseh_us);
			//printf("\n\rPulse v length: %lu", pulsev_us);
			
	
			
			
			rotateRightByDegrees(testdegrees);
			
			delay_ms(2000);
	}
	
	*/
			
			
			delay_ms(3000);
			//uint32_t start = 0;
			//uint32_t stop = 0;
			//uint32_t result = 0;
			
			
			while(1){
			//start = read_counter_value();	
	        rotateLeftByDegrees(28);
			//stop = read_counter_value();
			//result = (stop - start);
			//printf("\nResult: %u", result);
			delay_ms(2000);
	
			}
		
			
			
			/*
			while(1){
			
				int ek = counterA - counterB; //Counter A är höger hjul, counterB vänster
				printf("\n\rCOUNTERA: %lu", counterA, "\n");//Skriver ut räknarna och felvärdet i konsollen.
				printf("\n\rCOUNTERB: %lu", counterB, "\n");
				printf("EK: %d\n", ek);
				
				counterA = 0;
				counterB = 0;
				
		
				int uk = kp * ek;
				
				periodv = periodv-uk;
				
				periodv=max(1350, periodv);
				periodv=min(1500, periodv);
				pulse_set_period(pulseh_ch, periodh);
				pulse_set_period(pulsev_ch, periodv);
				
				if(ek == 0){
					
					printf("Left Wheel: %d\n", periodv);
				}
				delay_ms(8);
				
			}
			
			*/
	
	/*
	
	uint8_t foo = 0;
	int degreesToPos;
	double tempVariabel = 0;
	while(foo<4){
		
		valuesCalc(foo);
		degreesToPos = angleToPos();
		if (degreesToPos<0){
			degreesToPos = abs(degreesToPos);
			rotateRightByDegrees(degreesToPos);
			updateAngle();
			} else{
			rotateLeftByDegrees(degreesToPos);
			updateAngle();
		}
		while (distanceToPosition(foo)>30.0){
			delay_ms(500);
			int ek = counterA-counterB;
			tempVariabel = counterA*1.355;
			reglerahjul3(ek);
			updatePos(tempVariabel);
			tempVariabel = 0;
		}
		foo++;
		stop();
	}
	*/
}

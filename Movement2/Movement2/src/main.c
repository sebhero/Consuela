
#include <asf.h>
#include "pulseCounterHandler.h"
#include "buttonInterrupt.h"
#include "pulse.h"


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

	board_init();
	
	configure_console();
	printf("\nHello, World!");
	
	
	
	
	
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
	// Initialize the pulse channels
	pulse_init();
	// Set the period
	pulse_set_period(pulseh_ch, 1500);
	pulse_set_period(pulsev_ch, 1500);
	// Start the pulse output
	// Output will be on digital pin 35(PIO_PC3_IDX) for pulse channel 0
	// and on digital pin 37(PIO_PC5_IDX) for channel 1
	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
	

	
	uint32_t channel = 0;
	uint32_t periodv = 1660; //1660 getpå v hjul ger runt 1700
	uint32_t periodh = 1700; 
	double kp = 1;
	
	pulse_set_period(pulsev_ch, periodv);
	pulse_set_period(pulseh_ch, periodh);
	while(1) {
			
			
			
			ioport_get_pin_level(A);
			ioport_get_pin_level(B);
			int ek = counterA - counterB; //Counter A är höger hjul, counterB vänster
			counterA = 0;
			counterB = 0;
			int uk = kp * ek;
			
			pulse_timer_start(pulseh_timer);
			pulse_timer_start(pulsev_timer);
			//delay_ms(50);
			uint32_t pulseh_us = pulse_timer_get(pulseh_timer);
			uint32_t pulsev_us = pulse_timer_get(pulsev_timer);
			
			
			/*
			if(pulseh_us > pulsev_us) {
				periodh += 6;
			} else {
				periodh -= 5;
			}
			*/
			

	
			
			
			printf("\n\rCOUNTERA: %lu", counterA);
			printf("\n\rCOUNTERB: %lu", counterB);
			printf("\nEK: %d", ek);
			
			periodv += uk;
			
			if(periodh<1500){
				periodh = periodv;
			}
			pulse_set_period(pulsev_ch, periodv);
			pulse_set_period(pulseh_ch, periodh);
			
			
			
			//printf("\n\rPulse h length: %lu", pulseh_us);
			//printf("\n\rPulse v length: %lu", pulsev_us);
			
			
		
			
			
			delay_ms(10);
	}

}

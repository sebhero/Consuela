
#include <asf.h>

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
	
	uint32_t state = 0;
	uint32_t channel = 0;
	uint32_t periodv = 1600;
	uint32_t periodh = 1600;
	uint32_t result = 0;
	pulse_set_period(pulsev_ch, periodv);
	pulse_set_period(pulseh_ch, periodh);
	while(1) {
			
			
			pulse_timer_start(pulseh_timer);
			pulse_timer_start(pulsev_timer);
			delay_ms(50);
			uint32_t pulseh_us = pulse_timer_get(pulseh_timer);
			uint32_t pulsev_us = pulse_timer_get(pulsev_timer);
			if(pulseh_us > pulsev_us) {
				periodh += 6;
			} else {
				periodh -= 5;
			}
			
			if(periodh<1500){
				periodh = periodv;
			}
			pulse_set_period(pulsev_ch, periodv);
			pulse_set_period(pulseh_ch, periodh);
			
			printf("\n\rPulse h length: %lu", pulseh_us);
			printf("\n\rPulse v length: %lu", pulsev_us);
			
			
			//period = period + 100;
			
			
			//delay_ms(1000);
	}

}

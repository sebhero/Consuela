
#include <asf.h>

#include "pulse/pulse.h"

// Define a name for the pulse and say which channel and period to use
#define pulseA_ch 0
#define pulseA_period 1560

#define pulseB_ch 1
#define pulseB_period 1540

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
	pulse_set_period(pulseA_ch, pulseA_period);
	pulse_set_period(pulseB_ch, pulseB_period);
	// Start the pulse output
	// Output will be on digital pin 35(PIO_PC3_IDX) for pulse channel 0
	// and on digital pin 37(PIO_PC5_IDX) for channel 1
	pulse_start(pulseA_ch);
	pulse_start(pulseB_ch);
	
	uint32_t state = 0;
	uint32_t channel = 0;
	uint32_t period = 0;
	uint32_t result = 0;
	while(1) {
		switch(state) {
			case 0:				
				printf("\nSelect channel [0|1]: ");
				state = 1;
				break;
			case 1:
				result = scanf("%lu", &channel);
				if(result && channel < 2) {
					state = 2;
				} else {
					state = 0;
				}
				break;
			case 2:
				printf("\nEnter period in us [0-2000]: ");
				state = 3;
				break;
			case 3:
				result = scanf("%lu", &period);
				if(result && period <= 2000) {
					state = 4;
					} else {
					state = 2;
				}
				break;
			case 4:
				pulse_set_period(channel, period);
				state = 5;
				break;
			case 5:
				// Start the timer channels and measure the period
				pulse_timer_start(0);
				pulse_timer_start(1);
				delay_ms(500);
				uint32_t pulse0_us = pulse_timer_get(0);
				uint32_t pulse1_us = pulse_timer_get(1);
				printf("\n\rPulse 0 length: %lu", pulse0_us);
				printf("\n\rPulse 1 length: %lu", pulse1_us);
				state = 0;
				break;
			default:
				state = 0;				
		}
		
		
		
		
		
	}

}


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
	pulse_start(pulseA_ch);
	pulse_start(pulseB_ch);
	while(1) {
		pulse_timer_start(0);
		delay_ms(500);
		uint32_t pulse_us = pulse_timer_get(0);
		printf("\nPulse length: %u", pulse_us);
		delay_ms(2000);
	}

}

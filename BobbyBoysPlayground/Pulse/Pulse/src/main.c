
#include <asf.h>

#include "pulse/pulse.h"

// Define a name for the pulse and say which channel and period to use
#define pulseA_ch 0
#define pulseA_period 1560

#define pulseB_ch 1
#define pulseB_period 18000

uint32_t flag_ISR = 0;

void configure_console() {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/*
void configure_tc() {
	ioport_set_pin_dir(PIO_PB26_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIO_PB26_IDX,  IOPORT_MODE_MUX_B);
	ioport_disable_pin(PIO_PB26_IDX);

	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ID_TC2);

	tc_init(TC0, 2, TC_CMR_TCCLKS_XC0 | TC_CMR_CPCTRG);
	tc_enable_interrupt(TC0, 2, TC_IER_CPCS);
	tc_write_rc(TC0, 2, 3);
	NVIC_EnableIRQ(TC2_IRQn);
}
*/

void TC2_Handler() {
	uint32_t status = tc_get_status(TC0, 2);
	pulse_generator_stop(1);
	//pulse_stop(pulseB_ch);
	printf("\nISR cv: %lu", tc_read_cv(TC0, 2));
	flag_ISR = 1;
}

int main (void)
{
	sysclk_init();

	board_init();
	
	configure_console();
	
	pulse_init();
	
	pulse_generator_set_period(1, 20000);
	pulse_counter_set_cnt(0, 10);
	
	printf("\nHello, World!");

	
	// Initialize the pulse channels
	//pulse_init();
	// Set the period
	//pulse_set_period(pulseB_ch, pulseB_period);
	
	// Start the pulse output
	// Output will be on digital pin 35(PIO_PC3_IDX) for pulse channel 0
	// and on digital pin 37(PIO_PC5_IDX) for channel 1
	
	uint32_t state = 0;
	uint32_t result = 0;
	char c;
	while(1) {
		switch(state) {
			case 0:
				printf("\nType 's' to start: ");
				state = 1;
				break;
			case 1:
				result = scanf("%c", &c);
				if(result) {
					state = 2;
				} else {
					state = 0;
				}
				break;
			case 2:
				if(c == 's') {
					state = 3;
				} else {
					state = 0;
				}
				break;				
			case 3:
				pulse_generator_start(1);
				pulse_counter_start(0);
				state = 4;
				flag_ISR = 0;
				break;	
			case 4:
				delay_ms(100);
				printf("\ncnt: %lu", pulse_counter_get_cnt(0));
				if(flag_ISR) {
					state = 0;
				}
				break;		
			default:
				state = 0;				
		}
	}

}

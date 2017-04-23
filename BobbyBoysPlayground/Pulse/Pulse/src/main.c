
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

void TC2_Handler() {
	uint32_t status = tc_get_status(TC0, 2);
	//pulse_stop(pulseB_ch);
	printf("\nISR cv: %lu", tc_read_cv(TC0, 2));
	flag_ISR = 1;
}

int main (void)
{
	sysclk_init();

	board_init();
	
	configure_console();
	
	configure_tc();
	
	printf("\nHello, World!");
	ioport_set_pin_dir(PIO_PC5_IDX, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIO_PC5_IDX, IOPORT_PIN_LEVEL_LOW);
	
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
				ioport_set_pin_level(PIO_PC5_IDX, IOPORT_PIN_LEVEL_LOW);
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
				tc_start(TC0, 2);
				TC0->TC_CHANNEL[2].TC_CCR = TC_CCR_SWTRG;
				printf("\n#cv: %lu", tc_read_cv(TC0,2));
				delay_ms(200);				
				state = 4;
				flag_ISR = 0;
				break;	
			case 4:
				ioport_toggle_pin_level(PIO_PC5_IDX);
				delay_ms(500);				
				printf("\ncv: %lu", tc_read_cv(TC0,2));
				if(flag_ISR) {
					state = 0;
				}
				break;		
			default:
				state = 0;				
		}
	}

}

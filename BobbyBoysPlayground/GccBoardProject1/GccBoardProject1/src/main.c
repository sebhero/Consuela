
#include <asf.h>

#include "button/button.h"
#include "pwm/pwm.h"

#define LED IOPORT_CREATE_PIN(PIOC, 26)
#define HB_LED IOPORT_CREATE_PIN(PIOC, 2)

#define PA IOPORT_CREATE_PIN(PIOA, 2)

static void tc_capture_initialize(void) {
	ioport_set_pin_dir(PA, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PA,  IOPORT_MODE_MUX_A |  IOPORT_MODE_PULLUP);
	ioport_disable_pin(PA);
		
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ID_TC1);
	

	tc_init(TC0, 1,	TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_LDRA_RISING | TC_CMR_LDRB_FALLING	
	| TC_CMR_ABETRG | TC_CMR_ETRGEDG_FALLING);
	

	
	tc_start(TC0, 1);
	tc_enable_interrupt(TC0, 1, TC_IER_LDRBS );
	NVIC_EnableIRQ(TC1_IRQn);
}

void TC1_Handler(void) {
	tc_get_status(TC0,1);
	NVIC_DisableIRQ(TC1_IRQn);
	ioport_toggle_pin_level(LED);
	uint32_t rb = tc_read_rb(TC0, 1);	
	uint32_t ra = tc_read_ra(TC0, 1);
	uint32_t diff = rb - ra;
	uint32_t duration = (diff * 1000) / (CHIP_FREQ_CPU_MAX / 128); 
	printf("%lu \n", duration);
	NVIC_EnableIRQ(TC1_IRQn);
}

void configure_console() {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

	
int main (void) {
	sysclk_init();

	board_init();
	
	configure_console();
	
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	
	tc_capture_initialize();
	
	//configure_pwm();
	
	//configure_buttons();
	

	ioport_set_pin_dir(HB_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(HB_LED, IOPORT_PIN_LEVEL_LOW);
	
	while(1) {		
		ioport_set_pin_level(HB_LED, IOPORT_PIN_LEVEL_HIGH);
		delay_ms(500);		
		ioport_set_pin_level(HB_LED, IOPORT_PIN_LEVEL_LOW);
		delay_ms(200);
	}

}

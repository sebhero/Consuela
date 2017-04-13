
#include <asf.h>


#define LED IOPORT_CREATE_PIN(PIOC, 26)

void TC2_Handler() {
	uint32_t status = tc_get_status(TC0, 2);	
	if(status & TC_SR_CPAS) {
		ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);		
	}
	if(status & TC_SR_CPCS) {
		ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);		
	}	
}

void configure_tc(uint32_t freq, uint32_t dty_pct) {
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ID_TC2);
	tc_init(TC0, 2, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC |	TC_CMR_TCCLKS_TIMER_CLOCK4);

	uint32_t rc = (CHIP_FREQ_CPU_MAX / 128) / freq;
	tc_write_ra(TC0, 2, (rc * dty_pct)/100);
	tc_write_rc(TC0, 2, rc);
	tc_start(TC0, 2);
	tc_enable_interrupt(TC0, 2, TC_IER_CPCS | TC_IER_CPAS);
	NVIC_EnableIRQ(TC2_IRQn);
}


int main (void) {
	sysclk_init();

	board_init();

	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	
	configure_tc(10, 40);
	
	while(1) {
		//ioport_toggle_pin_level(LED);
		delay_ms(300);
	}
}

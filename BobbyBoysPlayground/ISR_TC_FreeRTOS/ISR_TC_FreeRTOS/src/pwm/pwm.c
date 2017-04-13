#include "pwm.h"

#define LED IOPORT_CREATE_PIN(PIOC, 26)

pwm_ch pwm_0 = {
	.TC = TC0,
	.ch = 2,
	.IRQn = TC2_IRQn,
	.ID_TC = ID_TC2
};

pwm_ch channels[1];

void init_pwm() {

	channels[0] = pwm_0;
}

void TC2_Handler() {
	uint32_t status = tc_get_status(TC0, 2);
	if(status & TC_SR_CPAS) {
		channels[0].cpra_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
	}
	if(status & TC_SR_CPCS) {
		channels[0].cprc_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);
	}
}

void change_pct_dty(pwm_t pwm) {
	pwm_ch channel = channels[pwm.pwm_ch_id];
	uint32_t rc = (CHIP_FREQ_CPU_MAX / 128) / pwm.freq;
	tc_write_ra(channel.TC, channel.ch, (rc * (100-pwm.pct_dty))/100);
	tc_write_rc(channel.TC, channel.ch, rc);
}

void configure_pwm(pwm_t pwm) {

	pwm_ch channel = channels[pwm.pwm_ch_id];
	
	channels[pwm.pwm_ch_id].cpra_callback = pwm.cpra_callback;
	channels[pwm.pwm_ch_id].cprc_callback = pwm.cprc_callback;
	
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(channel.ID_TC);	
	tc_init(channel.TC, channel.ch, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC |	TC_CMR_TCCLKS_TIMER_CLOCK4);
	change_pct_dty(pwm);
	tc_start(channel.TC, channel.ch);
	tc_enable_interrupt(channel.TC, channel.ch, TC_IER_CPCS | TC_IER_CPAS);
	NVIC_EnableIRQ(channel.IRQn);
}

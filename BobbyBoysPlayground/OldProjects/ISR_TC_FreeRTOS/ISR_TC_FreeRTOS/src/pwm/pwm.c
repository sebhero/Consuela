#include "pwm.h"

#define LED IOPORT_CREATE_PIN(PIOC, 26)


// Add something to go from clock to prescaler


pwm_ch pwm_0 = {
	.TC = TC0,
	.ch = 0,
	.IRQn = TC0_IRQn,
	.ID_TC = ID_TC0
};

pwm_ch pwm_1 = {
	.TC = TC0,
	.ch = 1,
	.IRQn = TC1_IRQn,
	.ID_TC = ID_TC1
};

pwm_ch pwm_2 = {
	.TC = TC0,
	.ch = 2,
	.IRQn = TC2_IRQn,
	.ID_TC = ID_TC2
};

pwm_ch channels[3];

void init_pwm() {
	channels[0] = pwm_0;
	channels[1] = pwm_1;
	channels[2] = pwm_2;
}

void TC0_Handler() {
	uint32_t status = tc_get_status(channels[0].TC, channels[0].ch);
	if(status & TC_SR_CPAS) {
		channels[0].cpra_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
	}
	if(status & TC_SR_CPCS) {
		channels[0].cprc_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);
	}
}

void TC1_Handler() {
	uint32_t status = tc_get_status(channels[1].TC, channels[1].ch);
	if(status & TC_SR_CPAS) {
		channels[1].cpra_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
	}
	if(status & TC_SR_CPCS) {
		channels[1].cprc_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);
	}
}

void TC2_Handler() {
	uint32_t status = tc_get_status(channels[2].TC, channels[2].ch);
	if(status & TC_SR_CPAS) {
		channels[2].cpra_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
	}
	if(status & TC_SR_CPCS) {
		channels[2].cprc_callback();
		//ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);
	}
}

void change_pulse_us(pwm_t pwm) {
	pwm_ch channel = channels[pwm.pwm_ch_id];
	uint32_t rc = (CHIP_FREQ_CPU_MAX / 32) / pwm.freq;
	tc_write_ra(channel.TC, channel.ch, rc - (((CHIP_FREQ_CPU_MAX / 32)/1000) * pwm.pulse_us)/1000);//(rc * (100-pwm.pulse_us))/100);
	tc_write_rc(channel.TC, channel.ch, rc);
}

void configure_pwm(pwm_t pwm) {

	pwm_ch channel = channels[pwm.pwm_ch_id];
	
	channels[pwm.pwm_ch_id].cpra_callback = pwm.cpra_callback;
	channels[pwm.pwm_ch_id].cprc_callback = pwm.cprc_callback;
	
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(channel.ID_TC);	
	tc_init(channel.TC, channel.ch, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC |	TC_CMR_TCCLKS_TIMER_CLOCK3);
	change_pulse_us(pwm);
	tc_start(channel.TC, channel.ch);
	tc_enable_interrupt(channel.TC, channel.ch, TC_IER_CPCS | TC_IER_CPAS);
	NVIC_EnableIRQ(channel.IRQn);
}

#include <asf.h>
#include "pulse_counter.h"

#define TC2_Handler_pulse_counter_idx 0

pulse_counter_t pulse_counters[] = {
	{
		.tc = TC0,
		.tc_ch = 2,
		.id = ID_TC2,
		.IRQn = TC2_IRQn,
		.tc_mode = (TC_CMR_TCCLKS_XC0 | TC_CMR_CPCTRG | TC_CMR_ETRGEDG_RISING),
		.pin = PIO_PB26_IDX,
		.mux = IOPORT_MODE_MUX_B,
		.ioport_mode = IOPORT_DIR_INPUT,
		.trig_cnt = 0
	}
};


void pulse_counter_init_channel(uint32_t ch_n) {
	
	ioport_set_pin_dir(pulse_counters[ch_n].pin, pulse_counters[ch_n].ioport_mode);
	ioport_set_pin_mode(pulse_counters[ch_n].pin,  pulse_counters[ch_n].mux);
	ioport_disable_pin(pulse_counters[ch_n].pin);

	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(pulse_counters[ch_n].id);

	tc_init(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch, pulse_counters[ch_n].tc_mode);
	tc_enable_interrupt(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch, TC_IER_CPCS);
	tc_write_rc(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch, pulse_counters[ch_n].trig_cnt);
	NVIC_EnableIRQ(pulse_counters[ch_n].IRQn);
}

void pulse_counter_start(uint32_t ch_n) {
	//uint32_t status = tc_get_status(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch);
	tc_start(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch);
}

void pulse_counter_set_trig_cnt(uint32_t ch_n, uint32_t n) {	
	tc_write_rc(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch, n-1);
	tc_enable_interrupt(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch, TC_IER_CPCS);
}

uint32_t pulse_counter_get_cnt(uint32_t ch_n) {	
	return tc_read_cv(pulse_counters[ch_n].tc, pulse_counters[ch_n].tc_ch);	
}


void TC2_Handler() {
	uint32_t status = tc_get_status(pulse_counters[TC2_Handler_pulse_counter_idx].tc, pulse_counters[TC2_Handler_pulse_counter_idx].tc_ch);
	tc_disable_interrupt(pulse_counters[TC2_Handler_pulse_counter_idx].tc, pulse_counters[TC2_Handler_pulse_counter_idx].tc_ch, TC_IDR_CPCS);
	//pulse_generator_stop(1);
	//pulse_stop(pulseB_ch);
	printf("\nISR cv: %lu", tc_read_cv(pulse_counters[TC2_Handler_pulse_counter_idx].tc, pulse_counters[TC2_Handler_pulse_counter_idx].tc_ch));	
	
}
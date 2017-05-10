#include <asf.h>
#include "pulse_counter.h"

#define TC2_Handler_pulse_counter_idx 0
#define TC5_Handler_pulse_counter_idx 1

pulse_counter_t pulse_counters[] = {
	{
		.tc = TC0,
		.tc_ch = 2,
		.id = ID_TC2,
		.IRQn = TC2_IRQn,
		.tc_mode = (TC_CMR_TCCLKS_XC0
                    | TC_CMR_CPCTRG
                    | TC_CMR_ETRGEDG_FALLING),
		.pin = PIO_PB26_IDX,  // Digital 22
		.mux = IOPORT_MODE_MUX_B,
		.ioport_mode = IOPORT_DIR_INPUT,
		.trig_cnt = 0,
        .callback_function = NULL
	},
	{
    	.tc = TC1,
    	.tc_ch = 2,
    	.id = ID_TC5,
    	.IRQn = TC5_IRQn,
    	.tc_mode = (TC_CMR_TCCLKS_XC0
                    | TC_CMR_CPCTRG
                    | TC_CMR_ETRGEDG_FALLING),
    	.pin = PIO_PA22_IDX,  // Analog 3
    	.mux = IOPORT_MODE_MUX_B,
    	.ioport_mode = IOPORT_DIR_INPUT,
    	.trig_cnt = 0,
        .callback_function = NULL
	}
};


void pulse_counter_init_channel(uint32_t ch_n) {

	ioport_set_pin_dir(pulse_counters[ch_n].pin,
                       pulse_counters[ch_n].ioport_mode);
	ioport_set_pin_mode(pulse_counters[ch_n].pin,
                        pulse_counters[ch_n].mux);
	ioport_disable_pin(pulse_counters[ch_n].pin);

	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(pulse_counters[ch_n].id);

	tc_init(pulse_counters[ch_n].tc,
            pulse_counters[ch_n].tc_ch,
            pulse_counters[ch_n].tc_mode);
    tc_start(pulse_counters[ch_n].tc,
             pulse_counters[ch_n].tc_ch);
	tc_enable_interrupt(pulse_counters[ch_n].tc,
                        pulse_counters[ch_n].tc_ch,
                        TC_IER_CPCS);
	tc_write_rc(pulse_counters[ch_n].tc,
                pulse_counters[ch_n].tc_ch,
                pulse_counters[ch_n].trig_cnt);
	NVIC_EnableIRQ(pulse_counters[ch_n].IRQn);
}

void pulse_counter_start(uint32_t ch_n) {
	//uint32_t status = tc_get_status(pulse_counters[ch_n].tc,
    //                                pulse_counters[ch_n].tc_ch);
	tc_start(pulse_counters[ch_n].tc,
             pulse_counters[ch_n].tc_ch);
    tc_enable_interrupt(pulse_counters[ch_n].tc,
                        pulse_counters[ch_n].tc_ch,
                        TC_IER_CPCS);
}

void pulse_counter_set_trig_cnt(uint32_t ch_n, uint32_t n) {
	tc_write_rc(pulse_counters[ch_n].tc,
                pulse_counters[ch_n].tc_ch,
                n-1);
}

void pulse_counter_set_callback_function(uint32_t ch_n, void (*fn)(void)) {
    pulse_counters[ch_n].callback_function = fn;
}

uint32_t pulse_counter_get_cnt(uint32_t ch_n) {
	return tc_read_cv(pulse_counters[ch_n].tc,
                      pulse_counters[ch_n].tc_ch) + 1;
}


void TC2_Handler() {
    pulse_counter_t pc = pulse_counters[TC2_Handler_pulse_counter_idx];
	uint32_t status = tc_get_status(pc.tc, pc.tc_ch);
	tc_disable_interrupt(pc.tc, pc.tc_ch, TC_IDR_CPCS);
    if(pc.callback_function != NULL) {
        pc.callback_function();
    }
}

void TC5_Handler() {
    pulse_counter_t pc = pulse_counters[TC5_Handler_pulse_counter_idx];
    uint32_t status = tc_get_status(pc.tc, pc.tc_ch);
    tc_disable_interrupt(pc.tc, pc.tc_ch, TC_IDR_CPCS);
    if(pc.callback_function != NULL) {
        pc.callback_function();
    }
    //pc.callback_function?pc.callback_function():NULL;
}
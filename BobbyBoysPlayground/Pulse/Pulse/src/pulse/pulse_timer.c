#include <asf.h>
#include "pulse_timer.h"

#define TC0_Handler_pulse_timer_idx 0
#define TC1_Handler_pulse_timer_idx 1
pulse_timer_t pulse_timers[] = {
	{
		.tc = TC0,
		.tc_ch = 0,
		.id = ID_TC0,
		.IRQ_n = TC0_IRQn,
		.tc_mode = TC_CMR_TCCLKS_TIMER_CLOCK4
		           | TC_CMR_LDRA_RISING
		           | TC_CMR_LDRB_FALLING
		           | TC_CMR_ABETRG
		           | TC_CMR_ETRGEDG_FALLING,
		.pin = PIO_PB25_IDX,  // Digital 2
		.mux = IOPORT_MODE_MUX_B,
		.ioport_mode = IOPORT_MODE_PULLUP,
		.divider = 128
	},
	{
		.tc = TC0,
		.tc_ch = 1,
		.id = ID_TC1,
		.IRQ_n = TC1_IRQn,
		.tc_mode = TC_CMR_TCCLKS_TIMER_CLOCK4
		           | TC_CMR_LDRA_RISING
		           | TC_CMR_LDRB_FALLING
		           | TC_CMR_ABETRG
		           | TC_CMR_ETRGEDG_FALLING,
		.pin = PIO_PA2_IDX,  // Analog 7
		.mux = IOPORT_MODE_MUX_A,
		.ioport_mode = IOPORT_MODE_PULLUP,
		.divider = 128
	}
};


void pulse_timer_init_channel(uint32_t ch_n) {

	ioport_set_pin_dir(pulse_timers[ch_n].pin,
                        IOPORT_DIR_INPUT);
	ioport_set_pin_mode(pulse_timers[ch_n].pin,
                        pulse_timers[ch_n].mux |
                        pulse_timers[ch_n].ioport_mode);
	ioport_disable_pin(pulse_timers[ch_n].pin);

	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(pulse_timers[ch_n].id);

	tc_init(pulse_timers[ch_n].tc,
            pulse_timers[ch_n].tc_ch,
            pulse_timers[ch_n].tc_mode);

	tc_start(pulse_timers[ch_n].tc,
            pulse_timers[ch_n].tc_ch);
	tc_enable_interrupt(pulse_timers[ch_n].tc,
                        pulse_timers[ch_n].tc_ch,
                        TC_IER_LDRBS );
	NVIC_DisableIRQ(pulse_timers[ch_n].IRQ_n);
}

void pulse_timer_start(uint32_t ch_n) {
	//tc_write_ra(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, 0U);
	//tc_write_rb(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, 0U);
	//tc_write_rb(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, 0U);

	//NVIC_EnableIRQ(pulse_timers[ch_n].IRQ_n); ???

}

uint32_t pulse_timer_get(uint32_t ch_n) {
    uint32_t status = tc_get_status(pulse_timers[ch_n].tc,
                                    pulse_timers[ch_n].tc_ch);
    uint32_t duration = 0;
    // If RB has not loaded since last read of status
    // return 0 for duation
    if(TC_SR_LDRBS & status) {
	    uint32_t rb = tc_read_rb(pulse_timers[ch_n].tc,
                                 pulse_timers[ch_n].tc_ch);
	    uint32_t ra = tc_read_ra(pulse_timers[ch_n].tc,
                                 pulse_timers[ch_n].tc_ch);
	    uint32_t diff = rb - ra;
	    // Need a better way to do this calculation
	    // Calculate the duration in microseconds.
	    duration = (diff*1000 ) /
                ((CHIP_FREQ_CPU_MAX / pulse_timers[ch_n].divider)/1000);
    }
	return duration;
}

void TC0_Handler(void) {
	tc_get_status(pulse_timers[TC0_Handler_pulse_timer_idx].tc,
                  pulse_timers[TC0_Handler_pulse_timer_idx].tc_ch);
	NVIC_DisableIRQ(pulse_timers[TC0_Handler_pulse_timer_idx].IRQ_n);
}

void TC1_Handler(void) {
	tc_get_status(pulse_timers[TC1_Handler_pulse_timer_idx].tc,
                  pulse_timers[TC1_Handler_pulse_timer_idx].tc_ch);
	NVIC_DisableIRQ(pulse_timers[TC1_Handler_pulse_timer_idx].IRQ_n);
}


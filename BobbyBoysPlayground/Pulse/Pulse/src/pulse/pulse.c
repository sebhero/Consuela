/*
 * pulse.c
 *
 */
 #include <asf.h>
 #include "pulse.h"

 // Define the clock, see pwm_clock_t
 pwm_clock_t pulse_clock_setting = {
    .ul_clka = 1000000L,
    .ul_clkb = 0,
    .ul_mck = BOARD_MCK // or CHIP_FREQ_CPU_MAX
 };

 // Define the pulse channels, ie. the supporting PWM channels
 pulse_channel_t pulse_channels[] = {
    {
        .pwm = PWM,
        .pwm_channel = {
            .channel = PWM_CHANNEL_0,
            .ul_prescaler = PWM_CMR_CPRE_CLKA,
            .ul_duty = 0,
            .ul_period = 50000,
            .polarity = PWM_HIGH
        },
        .id = ID_PWM,
        .pin = PIO_PC3_IDX,
        .mux = IOPORT_MODE_MUX_B,
        .dty_max = 2000
    },
    {
        .pwm = PWM,
        .pwm_channel = {
            .channel = PWM_CHANNEL_1,
            .ul_prescaler = PWM_CMR_CPRE_CLKA,
            .ul_duty = 0,
            .ul_period = 50000,
            .polarity = PWM_HIGH
        },
        .id = ID_PWM,
        .pin = PIO_PC5_IDX,
        .mux = IOPORT_MODE_MUX_B,
        .dty_max = 2000
    }
 };

pulse_timer_t pulse_timers[] = {
	{
		.tc = TC0,
		.tc_ch = 1,
		.id = ID_TC1,
		.IRQn = TC1_IRQn,
		.tc_mode = TC_CMR_TCCLKS_TIMER_CLOCK1 
			| TC_CMR_LDRA_RISING 
			| TC_CMR_LDRB_FALLING
			| TC_CMR_ABETRG 
			| TC_CMR_ETRGEDG_FALLING,
		.pin = PIO_PA2_IDX,
		.mux = IOPORT_MODE_MUX_A,
		.ioport_mode = IOPORT_MODE_PULLUP		
	}	
};

 /*
 * \brief Initialize the specified pulse channel
 *
 * \note Need to call pulse_start(ch_n) to start output
 *
 */
 static void pulse_init_channel(uint32_t ch_n) {
    ioport_set_pin_mode(pulse_channels[ch_n].pin, pulse_channels[ch_n].mux);
    ioport_disable_pin(pulse_channels[ch_n].pin);

    pmc_enable_periph_clk(pulse_channels[ch_n].id);
    pwm_channel_disable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
    pwm_init(pulse_channels[ch_n].pwm, &pulse_clock_setting);
 }

static void pulse_timer_init_channel(uint32_t ch_n) {

	ioport_set_pin_dir(pulse_timers[ch_n].pin, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(pulse_timers[ch_n].pin, pulse_timers[ch_n].mux | pulse_timers[ch_n].ioport_mode);
	ioport_disable_pin(pulse_timers[ch_n].pin);

	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(pulse_timers[ch_n].id);

	tc_init(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, pulse_timers[ch_n].tc_mode);

	tc_start(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch);
	tc_enable_interrupt(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, TC_IER_LDRBS );
	NVIC_DisableIRQ(pulse_timers[ch_n].IRQn);
}

void pulse_timer_start(uint32_t ch_n) {
	NVIC_EnableIRQ(pulse_timers[ch_n].IRQn);
}

uint32_t pulse_timer_get(uint32_t ch_n) {
	uint32_t rb = tc_read_rb(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch);
	uint32_t ra = tc_read_ra(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch);
	uint32_t diff = rb - ra;
	uint32_t duration = (diff ) / (((CHIP_FREQ_CPU_MAX/1000)/1000) / 2);
	return duration;
}

void TC1_Handler(void) {
	tc_get_status(pulse_timers[0].tc, pulse_timers[0].tc_ch);
	NVIC_DisableIRQ(pulse_timers[0].IRQn);
}

void pulse_init() {
    pulse_init_channel(0);
    pulse_init_channel(1);
	pulse_timer_init_channel(0);
}

void pulse_start(uint32_t ch_n) {
    pwm_channel_init(pulse_channels[ch_n].pwm, &(pulse_channels[ch_n].pwm_channel));
    pwm_channel_enable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
}

void pulse_stop(uint32_t ch_n) {
    pwm_channel_enable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
}

uint32_t pulse_set_period(uint32_t ch_n, uint32_t period_us) {
    if(period_us > pulse_channels[ch_n].dty_max) {
        return 1;
    }
    pwm_channel_update_duty(pulse_channels[ch_n].pwm, &(pulse_channels[ch_n].pwm_channel), period_us);
    return 0;
}
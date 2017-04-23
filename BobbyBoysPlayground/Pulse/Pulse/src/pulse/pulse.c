/*
 * pulse.c
 *
 */
 #include <asf.h>
 #include "pulse.h"

 // Forward declare the static functions
 static void pulse_generator_init_channel(uint32_t ch_n);
 static void pulse_ioport_init_channel(uint32_t ch);
 static void pulse_timer_init_channel(uint32_t ch_n);
 static void pulse_ioport_configure(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr);
 static uint32_t pulse_ioport_pin_to_ID_Port(ioport_pin_t pin);
 static IRQn_Type pulse_ioport_pin_to_Port_IRQn(ioport_pin_t pin);
 static void pulse_counter_init_channel(uint32_t ch_n);
 
 // Define the clock, see pwm_clock_t
 pwm_clock_t pulse_clock_setting = {
    .ul_clka = 100000L,
    .ul_clkb = 0,
    .ul_mck = BOARD_MCK // or CHIP_FREQ_CPU_MAX
 };

 // Define the pulse channels, ie. the supporting PWM channels
 pulse_generator_t pulse_channels[] = {
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
        .dty_max = 20000
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
        .dty_max = 20000
    }
 };

#define TC1_Handler_pulse_timer_idx 0
#define TC2_Handler_pulse_timer_idx 1
pulse_timer_t pulse_timers[] = {
	{
		.tc = TC0,
		.tc_ch = 1,
		.id = ID_TC1,
		.IRQn = TC1_IRQn,
		.tc_mode = TC_CMR_TCCLKS_TIMER_CLOCK4
			| TC_CMR_LDRA_RISING 
			| TC_CMR_LDRB_FALLING
			| TC_CMR_ABETRG 
			| TC_CMR_ETRGEDG_FALLING,
		.pin = PIO_PA2_IDX,
		.mux = IOPORT_MODE_MUX_A,
		.ioport_mode = IOPORT_MODE_PULLUP,
		.divider = 128
	},
	{
		.tc = TC0,
		.tc_ch = 0,
		.id = ID_TC0,
		.IRQn = TC0_IRQn,
		.tc_mode = TC_CMR_TCCLKS_TIMER_CLOCK4
		| TC_CMR_LDRA_RISING
		| TC_CMR_LDRB_FALLING
		| TC_CMR_ABETRG
		| TC_CMR_ETRGEDG_FALLING,
		.pin = PIO_PB25_IDX,
		.mux = IOPORT_MODE_MUX_B,
		.ioport_mode = IOPORT_MODE_PULLUP,
		.divider = 128
	}
};

pulse_ioport_t pulse_ioports[] = {
	{
		.pin = PIO_PB27_IDX,
		.mode = (PIO_PULLUP | PIO_DEBOUNCE),
		.handler = ch0_handler,
		.trigg_attr = PIO_IT_RISE_EDGE,
		.cnt = 0
	}	
};

void pulse_init() {
    pulse_generator_init_channel(0);
    pulse_generator_init_channel(1);
	pulse_timer_init_channel(0);
	pulse_timer_init_channel(1);
	pulse_ioport_init_channel(0);
}

 /*
 * \brief Initialize the specified pulse channel
 *
 * \note Need to call pulse_start(ch_n) to start output
 *
 */
 static void pulse_generator_init_channel(uint32_t ch_n) {
    ioport_set_pin_mode(pulse_channels[ch_n].pin, pulse_channels[ch_n].mux);
    ioport_disable_pin(pulse_channels[ch_n].pin);

    pmc_enable_periph_clk(pulse_channels[ch_n].id);
    pwm_channel_disable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
    pwm_init(pulse_channels[ch_n].pwm, &pulse_clock_setting);
 }

void pulse_generator_start(uint32_t ch_n) {
    pwm_channel_init(pulse_channels[ch_n].pwm, &(pulse_channels[ch_n].pwm_channel));
    pwm_channel_enable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
}

void pulse_generator_stop(uint32_t ch_n) {
    pwm_channel_disable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
}

uint32_t pulse_generator_set_period(uint32_t ch_n, uint32_t period_us) {
    if(period_us > pulse_channels[ch_n].dty_max) {
        return 1;
    }
    pwm_channel_update_duty(pulse_channels[ch_n].pwm, &(pulse_channels[ch_n].pwm_channel), period_us);
    return 0;
}
 /*
 * \brief Initialize the specified pulse timer channel
 *
 */
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
	//tc_write_ra(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, 0U);
	//tc_write_rb(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, 0U);
	//tc_write_rb(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch, 0U);
	NVIC_EnableIRQ(pulse_timers[ch_n].IRQn);
}

uint32_t pulse_timer_get(uint32_t ch_n) {
	uint32_t rb = tc_read_rb(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch);
	uint32_t ra = tc_read_ra(pulse_timers[ch_n].tc, pulse_timers[ch_n].tc_ch);
	uint32_t diff = rb - ra;
	// Need a better way to do this calculation
	// Calculate the duration in microseconds.
	uint32_t duration = (diff*1000 ) / ((CHIP_FREQ_CPU_MAX / pulse_timers[ch_n].divider)/1000);
	return duration;
}

/*
* \brief Configure a ioport channel for input
*
*/
static void pulse_ioport_init_channel(uint32_t ch) {
	pulse_ioport_configure(pulse_ioports[ch].pin, pulse_ioports[ch].handler, pulse_ioports[ch].mode,  pulse_ioports[ch].trigg_attr);
}

/**
 * \file
 *
 * \brief Template configuration for a ioport
 *
 */
static void pulse_ioport_configure(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr) {
    Pio *pio_base = arch_ioport_pin_to_base(pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(pin);
    IRQn_Type IRQn = pulse_ioport_pin_to_Port_IRQn(pin);
    uint32_t ID = pulse_ioport_pin_to_ID_Port(pin);
    
    pio_set_input(pio_base, pin_mask, pin_attr);
    pio_handler_set(pio_base, ID, pin_mask, trig_attr, handler);
    pio_enable_interrupt(pio_base, pin_mask);
    NVIC_ClearPendingIRQ(IRQn);
    NVIC_EnableIRQ(IRQn);
}

uint32_t pulse_ioport_get_cnt(uint32_t ch_n) {
	return pulse_ioports[ch_n].cnt;
}

/**
 * \file
 *
 * \brief Helper function to get the peripheral ID from a pin
 *
 */
static uint32_t pulse_ioport_pin_to_ID_Port(ioport_pin_t pin) {
     uint32_t ID = 0;
    switch(arch_ioport_pin_to_port_id(pin)) {
        case(IOPORT_PIOA):
        ID = ID_PIOA;
        break;
        case(IOPORT_PIOB):
        ID = ID_PIOB;
        break;
        case(IOPORT_PIOC):
        ID = ID_PIOC;
        break;
        case(IOPORT_PIOD):
        ID = ID_PIOD;
        break;        
    }
    return ID;
}

/**
 * \file
 *
 * \brief Helper function to get the port IRQn from a pin
 *
 */
static IRQn_Type pulse_ioport_pin_to_Port_IRQn(ioport_pin_t pin) {
    IRQn_Type IRQn = 0;    
    switch(arch_ioport_pin_to_port_id(pin)) {
        case(IOPORT_PIOA):
        IRQn = PIOA_IRQn;        
        break;
        case(IOPORT_PIOB):
        IRQn = PIOB_IRQn;
        break;
        case(IOPORT_PIOC):
        IRQn = PIOC_IRQn;
        break;
        case(IOPORT_PIOD):
        IRQn = PIOD_IRQn;
        break;
    }
    return IRQn;
}


static void pulse_counter_init_channel(uint32_t ch_n) {
	
}

void pulse_counter_start(uint32_t ch_n) {
	
}

void pulse_counter_stop(uint32_t ch_n) {
	
}

uint32_t pulse_counter_get_cnt(uint32_t cn_n) {
	return -1;	
}

/**
 * \file
 *
 * \brief Handler for ch0
 *
 */
void ch0_handler(const uint32_t id, const uint32_t index) {
    if ((id == pulse_ioport_pin_to_ID_Port(pulse_ioports[0].pin)) && (index == ioport_pin_to_mask(pulse_ioports[0].pin))){
        pulse_ioports[0].cnt += 1;
    }
}

void TC1_Handler(void) {
	tc_get_status(pulse_timers[TC1_Handler_pulse_timer_idx].tc, pulse_timers[TC1_Handler_pulse_timer_idx].tc_ch);
	NVIC_DisableIRQ(pulse_timers[TC1_Handler_pulse_timer_idx].IRQn);
}

void TC0_Handler(void) {
	tc_get_status(pulse_timers[TC2_Handler_pulse_timer_idx].tc, pulse_timers[TC2_Handler_pulse_timer_idx].tc_ch);
	NVIC_DisableIRQ(pulse_timers[TC2_Handler_pulse_timer_idx].IRQn);
}


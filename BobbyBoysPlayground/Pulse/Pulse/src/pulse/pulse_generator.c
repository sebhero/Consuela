/*!
 * \file pulse_generator.c
 *
 */

#include <asf.h>
#include "pulse_generator.h"


// Define the clock, see pwm_clock_t
pwm_clock_t pulse_generator_clock_setting = {
	.ul_clka = 1000000L,
	.ul_clkb = 0,
	.ul_mck = BOARD_MCK // or CHIP_FREQ_CPU_MAX or get_freq_max
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
		.pin = PIO_PC3_IDX, // Digital 35
		.mux = IOPORT_MODE_MUX_B,
		.dty_max = 49000
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
		.pin = PIO_PC5_IDX, // Digital 37
		.mux = IOPORT_MODE_MUX_B,
		.dty_max = 49000
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
		.pin = PIO_PC7_IDX, // Digital 39
		.mux = IOPORT_MODE_MUX_B,
		.dty_max = 49000
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
		.pin = PIO_PC9_IDX, // Digital 41
		.mux = IOPORT_MODE_MUX_B,
		.dty_max = 49000
	}
	// The other PWM pins are not grouped as nicely on the DUE
	// If needed they do exists but the polarity might have to
	// be switched to give high period instead of low
};
 
 
void pulse_generator_init_channel(uint32_t ch_n) {
    ioport_set_pin_mode(pulse_channels[ch_n].pin, pulse_channels[ch_n].mux);
    ioport_disable_pin(pulse_channels[ch_n].pin);

    pmc_enable_periph_clk(pulse_channels[ch_n].id);
    pwm_channel_disable(pulse_channels[ch_n].pwm, pulse_channels[ch_n].pwm_channel.channel);
    pwm_init(pulse_channels[ch_n].pwm, &pulse_generator_clock_setting);
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
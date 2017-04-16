/*
 * pulse.c
 *
 */
 #include <asf.h>
 #include "pulse.h"


 pwm_clock_t pulse_clock_setting = {
     .ul_clka = 1000000L,
     .ul_clkb = 0,
     .ul_mck = BOARD_MCK // or CHIP_FREQ_CPU_MAX
 };

 pulse_channel_t pulse_channel_0 = {
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
    .mux = IOPORT_MODE_MUX_B
 };

 pulse_channel_t pulse_channel_1 = {
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
    .mux = IOPORT_MODE_MUX_B
 };

 static void pulse_init_channel(uint32_t ch_n) {
    ioport_set_pin_mode(pulse_channels[ch_n]->pin, pulse_channels[ch_n]->mux);
    ioport_disable_pin(pulse_channels[ch_n]->pin);

    pmc_enable_periph_clk(pulse_channels[ch_n]->id);
    pwm_channel_disable(pulse_channels[ch_n]->pwm, pulse_channels[ch_n]->pwm_channel.channel);
    pwm_init(pulse_channels[ch_n]->pwm, &pulse_clock_setting);
 }


static void initialize_pulse_channels(void) {

    pulse_channels[0] = &pulse_channel_0;
    pulse_channels[1] = &pulse_channel_1;

    pulse_init_channel(0);
    pulse_init_channel(1);
}

void pulse_init() {
    initialize_pulse_channels();
}

void pulse_start(uint32_t ch_n) {
    pwm_channel_init(pulse_channels[ch_n]->pwm, &(pulse_channels[ch_n]->pwm_channel));
    pwm_channel_enable(pulse_channels[ch_n]->pwm, pulse_channels[ch_n]->pwm_channel.channel);
}

void pulse_stop(uint32_t ch_n) {
    pwm_channel_enable(pulse_channels[ch_n]->pwm, pulse_channels[ch_n]->pwm_channel.channel);
}

void pulse_set_period(uint32_t ch_n, uint32_t period_us) {
    pwm_channel_update_duty(pulse_channels[ch_n]->pwm, &(pulse_channels[ch_n]->pwm_channel), period_us);
}
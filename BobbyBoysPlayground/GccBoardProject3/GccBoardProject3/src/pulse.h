/*
 * pulse.h
 *
 */


#ifndef PULSE_H_
#define PULSE_H_

#include <asf.h>



//pwm_channel_t pwm_channel_0 = {
    //.channel = PWM_CHANNEL_0,
    //.ul_prescaler = PWM_CMR_CPRE_CLKA,
    //.ul_duty = 0,
    //.ul_period = 50000,
    //.polarity = PWM_HIGH
//};
//
//pwm_channel_t pwm_channel_1 = {
    //.channel = PWM_CHANNEL_1,
    //.ul_prescaler = PWM_CMR_CPRE_CLKA,
    //.ul_duty = 0,
    //.ul_period = 50000,
    //.polarity = PWM_HIGH
//};

typedef struct {
    Pwm *pwm;
    pwm_channel_t pwm_channel;
    uint32_t id;
    ioport_pin_t pin;
    ioport_mode_t mux;
} pulse_channel_t;

pulse_channel_t *pulse_channels[2];

void pulse_init(void);
void pulse_start(uint32_t ch_n);
void pulse_stop(uint32_t ch_n);
void pulse_set_period(uint32_t ch_n, uint32_t period_us);

#endif /* PULSE_H_ */
/*!
 * \file pulse.h
 *
 */

#ifndef PULSE_H_
#define PULSE_H_

#include <asf.h>

/*
* \brief Parameter container for pulse channel
*/
typedef struct {
    // The PWM instance to use
    Pwm *pwm;
    // see pwm_channel_t
    pwm_channel_t pwm_channel;
    // Pulse channel ID
    uint32_t id;
    // Pulse channel output pin
    ioport_pin_t pin;
    // Pulse channel peripheral MUX
    ioport_mode_t mux;
    // Pulse channel period in ms
    uint32_t dty_max;
} pulse_generator_t;

typedef struct {
	// The TC instance to use
	Tc *tc;
	// the channel
	uint32_t tc_ch;
	// TC channel ID
	uint32_t id;
	// IRQ
	IRQn_Type IRQn;
	// TC mode
	uint32_t tc_mode;
	// Timer channel input pin
	ioport_pin_t pin;
	// Timer channel peripheral MUX
	ioport_mode_t mux;
	// additional mode
	ioport_mode_t ioport_mode;
	// clock divider
	uint32_t divider;	
} pulse_timer_t;

typedef struct {
	ioport_pin_t pin;
	uint32_t mode;
	void (*handler)(const uint32_t, const uint32_t);
	uint32_t trigg_attr;
	uint32_t cnt;
} pulse_ioport_t;

typedef struct {
	// The TC instance to use
	Tc *tc;
	// the channel
	uint32_t tc_ch;
	// TC channel ID
	uint32_t id;
	// IRQ
	IRQn_Type IRQn;
	// TC mode
	uint32_t tc_mode;
	// Timer channel input pin
	ioport_pin_t pin;
	// Timer channel peripheral MUX
	ioport_mode_t mux;
	// additional mode
	ioport_mode_t ioport_mode;
	// Trigger counter
	uint32_t trig_cnt;
} pulse_counter_t;

/*
* \brief Initialize the pulse generator channels
*
* \note Need to call pulse_generator_start(ch_n) to start the pulse output
*
*/
void pulse_init(void);

/*
* \brief Starts pulse generation on the specified channel
*
* \param ch_n Pulse channel number
*
*/
void pulse_generator_start(uint32_t ch_n);

/*
* \brief Stops pulse generation on the specified channel
*
* \note Call pulse_start(ch_n) to start the pulse output again
*
* \param ch_n Pulse channel number
*
*/
void pulse_generator_stop(uint32_t ch_n);

/*
* \brief Set the pulse period on the specified channel to the specified period in us
*
* \param ch_n Pulse channel number
* \param period_us Pulse high period in microseconds
*
* \retval 0 Success
* \retval 1 Pulse period too long
*
*/
uint32_t pulse_generator_set_period(uint32_t ch_n, uint32_t period_us);

/*
* \brief Start timer on the specified channel
*
* This starts a one-off timer. When the timer has triggered, it has to be
* restarted if performing another measurement.
*
*/
void pulse_timer_start(uint32_t ch_n);

/*
* \brief Get the timer result
*
* \retval The timer inteval in micro-seconds
*/
uint32_t pulse_timer_get(uint32_t ch_n);

uint32_t pulse_ioport_get_cnt(uint32_t ch_n);

void pulse_counter_start(uint32_t ch_n);

uint32_t pulse_counter_get_cnt(uint32_t ch_n);
void pulse_counter_set_cnt(uint32_t ch_n, uint32_t n);

void ch0_handler(const uint32_t id, const uint32_t index);
#endif /* PULSE_H_ */
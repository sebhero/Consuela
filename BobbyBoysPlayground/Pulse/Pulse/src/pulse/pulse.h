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
} pulse_channel_t;

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
	// additinal mode
	ioport_mode_t ioport_mode;	
	
} pulse_timer_t;


void pulse_timer_start(uint32_t ch_n);
uint32_t pulse_timer_get(uint32_t ch_n);

/*
* \brief Initialize the pulse channels
*
* \note Need to call pulse_start(ch_n) to start the pulse output
*
*/
void pulse_init(void);

/*
* \brief Starts pulse generation on the specified channel
*
* \param ch_n Pulse channel number
*
*/
void pulse_start(uint32_t ch_n);

/*
* \brief Stops pulse generation on the specified channel
*
* \note Call pulse_start(ch_n) to start the pulse output again
*
* \param ch_n Pulse channel number
*
*/
void pulse_stop(uint32_t ch_n);

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
uint32_t pulse_set_period(uint32_t ch_n, uint32_t period_us);

#endif /* PULSE_H_ */
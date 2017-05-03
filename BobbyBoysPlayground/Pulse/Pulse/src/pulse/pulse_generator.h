/*!
 * \file pulse_generator.h
 *
 */
#ifndef PULSE_GENERATOR_H_
#define PULSE_GENERATOR_H_

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


  /*
 * \brief Initialize the specified pulse channel
 *
 * \note Need to call pulse_start(ch_n) to start output
 *
 */
 void pulse_generator_init_channel(uint32_t ch_n);

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
* \brief Set the pulse high period of the specified channel to the specified period in us
*
* \param ch_n Pulse channel number
* \param period_us Pulse high period in microseconds
*
* \retval 0 Success
* \retval 1 Pulse period too long
*
*/
uint32_t pulse_generator_set_period(uint32_t ch_n, uint32_t period_us);



#endif /* PULSE_GENERATOR_H_ */
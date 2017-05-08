
#ifndef PULSE_COUNTER_H_
#define PULSE_COUNTER_H_

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
    // Callback handler
    void (*callback_function)(void);
} pulse_counter_t;

 /*
 * \brief Initialize the specified counter channel
 *
 */
void pulse_counter_init_channel(uint32_t ch_n);

/*
* \brief Starts pulse counting on the specified channel
*
* \param ch_n Pulse channel number
*
*/
void pulse_counter_start(uint32_t ch_n);


/*
* \brief Set the callback functiont to be called once the counter has reached
*  the trigger value
*
* \param ch_n Pulse channel number
* \param fn Callback function to be used
*
*/
void pulse_counter_set_callback_function(uint32_t ch_n, void (*fn)(void));

/*
* \brief Get the number of counted pulses
*
* \param ch_n Pulse channel number
*
* \retval Number of pulsed which have been counted
*/
uint32_t pulse_counter_get_cnt(uint32_t ch_n);

/*
* \brief Set the number of pulsed which sould generated a trigger event on the
*  specified channel
*
* \param ch_n Pulse channel number
* \param n Number of pulses
*/
void pulse_counter_set_trig_cnt(uint32_t ch_n, uint32_t n);


#endif /* PULSE_COUNTER_H_ */
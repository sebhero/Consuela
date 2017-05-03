
#ifndef PULSE_TIMER_H_
#define PULSE_TIMER_H_

typedef struct {
	// The TC instance to use
	Tc *tc;
	// the channel
	uint32_t tc_ch;
	// TC channel ID
	uint32_t id;
	// IRQ
	IRQn_Type IRQ_n;
	// TC mode
	uint32_t tc_mode;
	// Timer channel input pin
	ioport_pin_t pin;
	// Timer channel peripheral MUX
	ioport_mode_t mux;
	// additional mode
	ioport_mode_t ioport_mode;
	// clock divider, this depends on the  clock used in the tc_mode
	uint32_t divider;
} pulse_timer_t;

 /*
 * \brief Initialize the specified pulse timer channel
 *
 */
 void pulse_timer_init_channel(uint32_t ch_n);

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



#endif /* PULSE_TIMER_H_ */

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
} pulse_counter_t;

void pulse_counter_init_channel(uint32_t ch_n);

void pulse_counter_start(uint32_t ch_n);

uint32_t pulse_counter_get_cnt(uint32_t ch_n);

void pulse_counter_set_trig_cnt(uint32_t ch_n, uint32_t n);


#endif /* PULSE_COUNTER_H_ */
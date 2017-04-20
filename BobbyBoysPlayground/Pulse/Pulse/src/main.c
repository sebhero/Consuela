
#include <asf.h>

#include "pulse/pulse.h"

// Define a name for the pulse and say which channel and period to use
#define pulseA_ch 0
#define pulseA_period 1560

#define pulseB_ch 1
#define pulseB_period 1540

int main (void)
{
	sysclk_init();

	board_init();
	
	// Initialize the pulse channels
	pulse_init();
	// Set the period
	pulse_set_period(pulseA_ch, pulseA_period);
	pulse_set_period(pulseB_ch, pulseB_period);
	// Start the pulse output
	// Output will be on digital pin 35(PIO_PC3_IDX) for pulse channel 0
	pulse_start(pulseA_ch);
	pulse_start(pulseB_ch);
	while(1) {
		// spin
	}

}

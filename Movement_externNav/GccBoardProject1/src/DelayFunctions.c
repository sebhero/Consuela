//
//  DelayFunctions.c
//  Code to be used in task 1401c
//
//	Ulrik Eklund 2014
//
//

#include "asf.h"
#include "DelayFunctions.h"

int delayInit(void)		/* Initializes the timer used for delays */
{
	pmc_enable_periph_clk(ID_TC0);	/* power on the peripheral clock for timers */
	tc_init(TC0,0,0);				/* TC0, channel 0, TCLK1 och capturemode */
	tc_set_block_mode(TC0,0);
	tc_stop(TC0,0);					/* making sure the timer does not run  */
	return 0;						/* evertyhing is ok, typical response */
}


void delayMicroseconds(uint32_t us)		/* A simple implementation for a delay in us (not calibrated) */
{
	tc_start(TC0,0);
	while (tc_read_cv(TC0,0) < us*42);
	tc_stop(TC0,0);
}
/*
 * RealTimeTimer.h
 *
 * Created: 2017-05-01 14:22:49
 *  Author: Danial Mahmoud
 */ 

#include <inttypes.h> 

#ifndef REALTIMETIMER_H_
#define REALTIMETIMER_H_

/* The Power Management Controller has no effect on RTT behavior since it is continuously fed by SLCK (Slow Clock) */
#define RTT_MR          0x400E1A30U     /* address of hardware register Real-time Timer Mode Register */
#define RTT_VR          0x400E1A38U     /* address of hardware register Real-time Timer Value Register */
#define RTPRES          0x00008000U     /* defines the number of SLCK periods required to increment RTT */

extern uint32_t realTimeValue;

static void feed_counter_signal(void);
uint32_t get_current_value(void);
void start_RTT(void);

#endif /* REALTIMETIMER_H_ */
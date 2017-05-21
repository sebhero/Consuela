/*
 * RealTimeTimer.c
 *
 * Explanatory comments of code are derived from Atmel Studio's data sheet for SAM3X8E
 * 
 * The Real-time Timer is built around a 32-bit counter that is fed by an SLCK (Slow clock) signal. This signal can be divided
 * by a programmable 16-bit value which is defined in the 16-bit field called RTPRES of the Real-time Mode Register. Programming
 * RTPRES at 0x00008000 corresponds to feed the real-time counter with a 1 Hz signal. Since the SLCK is operating at 32.768 kHz
 * the SLCK period at which the RTT-counter increments will become 1 second (RTPRES = 0x00008000 = 1*2^15 = 32 768 ---> inc. period
 * equals RTPRES*(1/ SLCK freq.) = 32768/(1/32768) = 1 sec.). This means that the counter can count up to 2^32 seconds which is 
 * tantamount to approximately 136 years.
 *
 * Created: 2017-05-01 14:21:52
 * Author: Danial Mahmoud
 * 
 */ 

#include "RealTimeTimer.h"
#include <inttypes.h> 

uint32_t *const p_RTT_MR = (uint32_t *) RTT_MR;
uint32_t *const p_RTT_VR = (uint32_t *) RTT_VR;

uint32_t realTimeValue = 0;

/* Feed the real-time counter with 1 Hz signal, making it count up once per second */
static void feed_counter_signal(){
	*p_RTT_MR &= RTPRES;
}

/* Returns the current value of the Real-time Timer */
uint32_t read_current_value(){
	realTimeValue = *p_RTT_VR; // the value can be read any time in the register Real-time Value Register 
	return realTimeValue;
}

void start_RTT(){
	feed_counter_signal();
}
                                        
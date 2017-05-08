/*!
 * \file pulse.h
 *
 */

#ifndef PULSE_H_
#define PULSE_H_

#include <asf.h>
#include "pulse_generator.h"
#include "pulse_timer.h"
#include "pulse_counter.h"
//#include "pulse_ioport.h"

/*
* \brief Initialize the pulse generator channels
*
* \note Need to call pulse_generator_start(ch_n) to start the pulse output
*
*/
void pulse_init(void);





#endif /* PULSE_H_ */
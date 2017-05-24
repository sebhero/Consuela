/*
 * common.h
 * This header file contains methods for setting and clearing certain bits in a register.
 *
 * Author:	Danial Mahmoud
 *
 * Date:	2016-05-08
 */ 

#ifndef COMMON_H_
#define COMMON_H_

/*
 * This macro is used to set a specific bit in a register.
 */
#define SET_BIT(reg, pos)	(reg) = (reg) | (1<<pos)

/*
 * This macro is used to clear a specific bit in a register.
 * To clear a bit in PIO-register on SAM-board 1's are written to PIO_CODR-register
 */
#define CLR_BIT(reg, pos)	(reg) = (reg) | (1<<pos) //(reg) = (reg) & ~ (1<<pos)

#endif /* COMMON_H_ */

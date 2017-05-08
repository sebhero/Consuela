/*
 * TimerCounter.h
 *
 * Created: 2017-04-27 16:49:47
 *  Author: Danial Mahmoud
 */ 

#ifndef TIMERCOUNTER_H_
#define TIMERCOUNTER_H_

#define PERIPH_ID_TCC0    27                 /* Peripheral ID of Timer Counter Channel 0 */
#define PMC_PCSR0         0x400E0618U        /* address of hardware register Peripheral Clock Status Register 0 of PMC */
#define PMC_PCER0         0x400E0610U        /* address of hardware register Peripheral Clock Enable Register 0 of PMC */
#define PMC_WPMR          0x400E06E4U        /* address of hardware register Writer Protect Mode Register of PMC */
#define TC_CMR0           0x40080004U        /* address of hardware register TC Channel Mode Register: Capture Mode, channel 0 of TC0 */
#define TC_WPMR           0x400800E4U        /* address of hardware register TC Write Protection Mode Register, channel 0 of TC0 */
#define TC_CCR            0x40080000U        /* address of hardware register TC Channel Control register, channel 0 of TC0 */
#define TC_CV             0x40080010U        /* address of hardware register TC Counter Value Register, channel 0 of TC0 */
#define TC_SR0            0x40080020U        /* address of hardware register TC Status Register, channel 0 of TC0 */
#define WPKEY_WPMR_PMC    (uint32_t)0x504D43
#define WPKEY_WPMR_TC     (uint32_t)0x54494D
#define WAVE              15                 /* bit in TC_CMR that sets mode */
#define CLKEN             0                  /* bit in TC_CCR that enables clock */
#define CLKDIS            1                  /* bit in TC_CCR that disables clock */
#define SWTRG             2                  /* bit in TC_CCR that commands software trigger */
#define COVFS             0                  /* bit in TC_SR, short for Counter Overflow status */

extern uint32_t counterValue;

static uint32_t disable_write_protect(uint32_t key);
static void enable_periph_clk(uint32_t periph_id);
static void enable_clock_TC0CH0(void);
static void select_capture_mode(void);
static void select_clock(void);
static void start_clock(void);
uint32_t read_counter_value(void);
void TC0_init(void);


#endif /* TIMERCOUNTER_H_ */
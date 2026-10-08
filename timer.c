// Author: Hussein Taleb

#include "timer.h"

// timer address
#define TIMER_BASE 0x04000020

// timer registers
#define TIMER_STATUS  0
#define TIMER_CONTROL 1
#define TIMER_PERIODL 2
#define TIMER_PERIODH 3
#define TIMER_SNAPL   4
#define TIMER_SNAPH   5

// timer control values
#define CTRL_ITO   0x1
#define CTRL_CONT  0x2
#define CTRL_START 0x4
#define CTRL_STOP  0x8

// timer interrupt number
#define TIMER_IRQ 16

// enables interrupts
extern void enable_interrupt(void);

// pointer to the timer
static volatile unsigned int *timer =
    (volatile unsigned int *) TIMER_BASE;

// becomes 1 when a timer interrupt occurs
static volatile int tick_pending = 0;


// changes the timer period
void timer_set_period_ms(unsigned int period_ms)
{
    unsigned int period;

    // convert milliseconds to timer ticks
    period = period_ms * (TIMER_FREQ_HZ / 1000);

    // stop the timer while changing the period
    timer[TIMER_CONTROL] = CTRL_STOP;

    // set the lower 16 bits
    timer[TIMER_PERIODL] = (period - 1) & 0xFFFF;

    // set the upper 16 bits
    timer[TIMER_PERIODH] = ((period - 1) >> 16) & 0xFFFF;

    // clear old timeout
    timer[TIMER_STATUS] = 0;

    // clear old game tick
    tick_pending = 0;

    // start the timer with interrupts
    timer[TIMER_CONTROL] = CTRL_START | CTRL_CONT | CTRL_ITO;
}


// initializes the timer
void timer_init(unsigned int period_ms)
{
    timer_set_period_ms(period_ms);

    // allow the processor to receive interrupts
    enable_interrupt();
}


// handles timer interrupts
void handle_interrupt(unsigned int cause)
{
    // check if the interrupt came from the timer
    if (cause == TIMER_IRQ) {

        // clear the timer interrupt
        timer[TIMER_STATUS] = 0;

        // tell the game that it is time for a new step
        tick_pending = 1;
    }
}


// returns 1 when a game tick is available
int timer_take_tick(void)
{
    if (tick_pending == 1) {

        // remove the game tick after reading it
        tick_pending = 0;

        return 1;
    }

    return 0;
}


// returns the current timer value
unsigned int timer_snapshot(void)
{
    unsigned int low;
    unsigned int high;
    unsigned int value;

    // save the current timer value in the snapshot registers
    timer[TIMER_SNAPL] = 0;

    // read the lower and upper parts
    low = timer[TIMER_SNAPL];
    high = timer[TIMER_SNAPH];

    // combine the two 16-bit parts
    value = (high << 16) | (low & 0xFFFF);

    return value;
}
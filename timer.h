// Author: Hussein Taleb

#ifndef TIMER_H
#define TIMER_H

// timer frequency is 30 MHz
#define TIMER_FREQ_HZ 30000000

// starts the timer with a period in milliseconds
void timer_init(unsigned int period_ms);

// changes the timer period
void timer_set_period_ms(unsigned int period_ms);

// returns 1 if a timer interrupt has occurred
int timer_take_tick(void);

// returns the current timer value
unsigned int timer_snapshot(void);

#endif
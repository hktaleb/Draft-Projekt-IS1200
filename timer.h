// timer.h - TIMER i projektträdet.
// Hårdvarutimern ger ett avbrott per spelsteg (game tick).

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Timern räknar med 30 MHz, alltså 30 000 000 tick per sekund
#define TIMER_FREQ_HZ 30000000

// Startar timern med en period i millisekunder och slår på avbrott
void timer_init(uint32_t period_ms);

// Hastighetskontroll: byter period (kortare period = snabbare spel)
void timer_set_period_ms(uint32_t period_ms);

// Game tick: returnerar 1 (och nollställer) om ett timeravbrott har skett sedan sist
int timer_take_tick(void);

// Timerdata för random seed: timerns nuvarande räknarvärde
uint32_t timer_snapshot(void);

#endif

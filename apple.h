// apple.h - APPLE i projektträdet.
// Äpplets position och generering av ny position med pseudoslumptal.

#ifndef APPLE_H
#define APPLE_H

#include <stdint.h>
#include "snake.h"

// Startvärde (frö) för slumptalen, sätts vid spelstart
void apple_seed(uint32_t seed);

// Genererar en ny position innanför väggarna som inte upptas av ormen, och ritar äpplet.
// Förutsätter att ormen inte fyller hela planen.
void apple_spawn(void);

position apple_position(void);

// 1 om äpplet ligger på position p
int apple_is_at(position p);

#endif

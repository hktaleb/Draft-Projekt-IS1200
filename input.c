// input.c - INPUT i projektträdet.
// Kortet har bara en programmerbar knapp, så riktningarna styrs med switcharna SW0-SW3
// och knappen används för start/omstart. Allt läses med polling från huvudloopen.

#include "input.h"
#include <stdint.h>
#include "snake.h" // För DIR_UP, DIR_DOWN osv.

#define SWITCH_ADDR 0x04000010 // Switcharna SW0-SW9
#define BUTTON_ADDR 0x040000d0 // Den programmerbara knappen

#define DIRECTION_SWITCHES 4 // SW0-SW3 används för styrning

// Hur många avläsningar i rad som måste ge samma värde innan det räknas (några ms)
#define DEBOUNCE_SAMPLES 5000

// Debounce: håller koll på en insignal så att studsar (snabbt fladder) ignoreras
typedef struct {
    uint32_t stable;   // Senast godkända värde
    uint32_t last_raw; // Senaste råa avläsning
    int count;         // Antal avläsningar i rad med samma råvärde
} debouncer;

// Vilken riktning varje switch styr (SW3 sitter längst till vänster på kortet)
static const int switch_direction[DIRECTION_SWITCHES] = {
    DIR_RIGHT, // SW0
    DIR_DOWN,  // SW1
    DIR_UP,    // SW2
    DIR_LEFT,  // SW3
};

// Knappstatus (avstudsad) och föregående knappstatus
static debouncer switch_db;
static debouncer button_db;
static uint32_t previous_switches;
static uint32_t previous_button;

// ===================== Debounce =====================

// Godkänner ett nytt värde först när det varit oförändrat i DEBOUNCE_SAMPLES avläsningar
static uint32_t debounce(debouncer* d, uint32_t raw) {
    if (raw != d->last_raw) {
        d->last_raw = raw; // Värdet ändrades: börja räkna om
        d->count = 0;
    } else if (d->count < DEBOUNCE_SAMPLES) {
        d->count++;        // Samma värde igen: räkna upp
    } else {
        d->stable = raw;   // Stabilt länge nog: godkänn
    }
    return d->stable;
}

// ===================== Polling =====================

// Läser SW0-SW3 (övriga switchar maskas bort)
static uint32_t read_switches(void) {
    return *(volatile uint32_t*)SWITCH_ADDR & ((1u << DIRECTION_SWITCHES) - 1);
}

// Läser knappen: 1 = nedtryckt
static uint32_t read_button(void) {
    return *(volatile uint32_t*)BUTTON_ADDR & 0x1;
}

void input_init(void) {
    // Utgå från nuvarande läge så att inget räknas som en ändring vid start
    uint32_t switches = read_switches();
    uint32_t button = read_button();

    switch_db = (debouncer){switches, switches, 0};
    button_db = (debouncer){button, button, 0};
    previous_switches = switches;
    previous_button = button;
}

// ===================== Önskad riktning =====================

int input_get_direction(void) {
    uint32_t current = debounce(&switch_db, read_switches());
    uint32_t changed = current ^ previous_switches; // XOR: 1 för varje switch som ändrats
    previous_switches = current;

    // Första switchen som slagits om (åt valfritt håll) bestämmer riktningen
    for (int sw = 0; sw < DIRECTION_SWITCHES; sw++) {
        if (changed & (1u << sw))
            return switch_direction[sw];
    }
    return DIR_NONE;
}

// ===================== Knappen =====================

int input_button_pressed(void) {
    uint32_t current = debounce(&button_db, read_button());
    int pressed = current && !previous_button; // Bara när knappen går från uppe till nere
    previous_button = current;
    return pressed;
}

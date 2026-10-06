// apple.c - APPLE i projektträdet.
// Äpplet placeras på en slumpad ledig cell innanför väggarna.

#include "apple.h"
#include <stdint.h>
#include "snake.h"
#include "vga.h"

static position apple;        // Position X/Y
static uint32_t random_state; // Slumptalsgeneratorns tillstånd

// Pseudoslumptal

void apple_seed(uint32_t seed) {
    random_state = seed;
}

// Linjär kongruensgenerator: räknar fram nästa "slumptal" ur det förra
static uint32_t random_next(void) {
    random_state = random_state * 1103515245u + 12345u;
    return random_state >> 16; // De höga bitarna är "slumpigast"
}

// Generering av ny position

void apple_spawn(void) {
    // Välj en slumpad startcell bland cellerna innanför väggarna (38 x 28)
    int start = random_next() % PLAYFIELD_CELLS;

    // Gå framåt från startcellen tills vi hittar en cell som ormen inte upptar
    for (int i = 0; i < PLAYFIELD_CELLS; i++) {
        int index = (start + i) % PLAYFIELD_CELLS;

        // Begränsning till spelplanen: cellnummer -> (x, y), +1 hoppar över väggen
        position p = {1 + index % PLAYFIELD_WIDTH, 1 + index / PLAYFIELD_WIDTH};

        // Kontroll att positionen inte upptas av ormen
        if (!snake_occupies(p)) {
            apple = p;
            vga_draw_apple(p.x, p.y);
            return;
        }
    }
}

// ===================== Position =====================

position apple_position(void) {
    return apple;
}

int apple_is_at(position p) {
    return p.x == apple.x && p.y == apple.y;
}

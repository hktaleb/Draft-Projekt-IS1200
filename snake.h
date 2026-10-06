// snake.h - SNAKE i projektträdet.
// Ormens position, segment, längd, riktning, rörelse, tillväxt och kollisioner.

#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include "vga.h"

// Position X/Y i rutnätet (används av både ormen och äpplet)
typedef struct {
    int8_t x, y;
} position;

// Direction. Ordningen gör att motsatt riktning fås med XOR 1 (UP<->DOWN, LEFT<->RIGHT).
#define DIR_NONE  -1 // Ingen ny riktning
#define DIR_UP     0
#define DIR_DOWN   1
#define DIR_LEFT   2
#define DIR_RIGHT  3
#define DIR_COUNT  4 // Antal riktningar

// Ger motsatt riktning, t.ex. DIR_OPPOSITE(DIR_UP) == DIR_DOWN
#define DIR_OPPOSITE(d) ((d) ^ 1)

// Maxlängd: ormen kan som mest fylla hela ytan innanför väggarna (38 x 28 celler)
#define SNAKE_MAX_LEN   PLAYFIELD_CELLS
#define SNAKE_START_LEN 3

// Initialisering: lägger ormen mitt på planen, riktad åt höger, och ritar den
void snake_init(void);

// Riktningsbyte: tar emot önskad riktning, men blockerar 180-graderssväng
void snake_set_direction(int dir);

// Var huvudet hamnar i nästa steg
position snake_next_head(void);

// Rörelse (och tillväxt om grow = 1): flyttar ormen ett steg och ritar om det som ändrats
void snake_move(int grow);

// Collision detection
int snake_hits_wall(position p);           // Väggkollision
int snake_hits_self(position p, int grow); // Självkollision

// Positionskontroll: 1 om ormen täcker cellen p (p måste ligga på rutnätet)
int snake_occupies(position p);

int snake_length(void);
int snake_is_full(void); // 1 om ormen nått maxlängd
#endif

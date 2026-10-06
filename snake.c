// snake.c - SNAKE i projektträdet.
// Ormen lagras som en array av (X, Y)-positioner. Vid varje steg tar varje segment
// föregående segments position och huvudet flyttas en cell i riktningen.

#include "snake.h"
#include <stdint.h>
#include "vga.h"

// Hur mycket X och Y ändras per steg i varje riktning
static const position dir_delta[DIR_COUNT] = {
    [DIR_UP]    = { 0, -1},
    [DIR_DOWN]  = { 0,  1},
    [DIR_LEFT]  = {-1,  0},
    [DIR_RIGHT] = { 1,  0},
};

// Segment-array: segments[0] är huvudet, segments[length - 1] är svansen
static position segments[SNAKE_MAX_LEN];
static int length; // Längd: antal segment just nu

// 1 där ormen finns. Gör att vi direkt ser om en cell är upptagen
// utan att behöva gå igenom hela segment-arrayen.
static uint8_t occupied[GRID_HEIGHT][GRID_WIDTH];

static int heading;  // Riktningen ormen rörde sig i senaste steget
static int next_dir; // Riktningen ormen ska ta i nästa steg

// ===================== Initialisering =====================

void snake_init(void) {
    // Töm rutnätet
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            occupied[y][x] = 0;

    // Ormen startar mitt på planen med huvudet längst till höger
    length = SNAKE_START_LEN;
    for (int i = 0; i < length; i++) {
        segments[i] = (position){GRID_WIDTH / 2 - i, GRID_HEIGHT / 2};
        occupied[segments[i].y][segments[i].x] = 1;
        if (i == 0)
            vga_draw_snake_head(segments[i].x, segments[i].y);
        else
            vga_draw_snake_body(segments[i].x, segments[i].y);
    }

    heading = DIR_RIGHT;
    next_dir = DIR_RIGHT;
}

// ===================== Riktningsbyte och blockering av 180°-sväng =====================

void snake_set_direction(int dir) {
    // Rakt bakåt skulle betyda att huvudet går in i kroppen direkt, så det ignoreras.
    // Vi jämför med heading (senaste steget), så två snabba svängar kan inte lura spärren.
    if (dir != DIR_NONE && dir != DIR_OPPOSITE(heading))
        next_dir = dir;
}

// ===================== Rörelse och tillväxt =====================

position snake_next_head(void) {
    position head = segments[0];
    return (position){head.x + dir_delta[next_dir].x, head.y + dir_delta[next_dir].y};
}

void snake_move(int grow) {
    position old_head = segments[0];
    position old_tail = segments[length - 1];
    position new_head = snake_next_head();
    heading = next_dir;

    if (grow && length < SNAKE_MAX_LEN) {
        length++; // Tillväxt: den gamla svansen blir kvar som nytt sista segment
    } else {
        // Ingen tillväxt: svansen flyttar sig, så dess cell blir ledig
        occupied[old_tail.y][old_tail.x] = 0;
        vga_erase_cell(old_tail.x, old_tail.y);
    }

    // Varje segment tar föregående segments position, sedan flyttas huvudet
    for (int i = length - 1; i > 0; i--)
        segments[i] = segments[i - 1];
    segments[0] = new_head;
    occupied[new_head.y][new_head.x] = 1;

    // Rita bara det som ändrats: gamla huvudet blir kropp, nya huvudet ritas
    vga_draw_snake_body(old_head.x, old_head.y);
    vga_draw_snake_head(new_head.x, new_head.y);
}

// ===================== Collision Detection =====================

// Väggkollision: väggarna är den yttersta raden/kolumnen av celler
int snake_hits_wall(position p) {
    return p.x <= 0 || p.x >= GRID_WIDTH - 1 || p.y <= 0 || p.y >= GRID_HEIGHT - 1;
}

// Självkollision: går huvudet in i en cell som ormen redan täcker?
int snake_hits_self(position p, int grow) {
    position tail = segments[length - 1];
    // Utan tillväxt flyttar svansen undan i samma steg, så dess cell räknas som ledig
    if (!grow && p.x == tail.x && p.y == tail.y)
        return 0;
    return snake_occupies(p);
}

// ===================== Positionskontroll =====================

int snake_occupies(position p) {
    return occupied[p.y][p.x];
}

// ===================== Längd och maxlängd =====================

int snake_length(void) {
    return length;
}

int snake_is_full(void) {
    return length >= SNAKE_MAX_LEN;
}

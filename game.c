// game.c - GAME i projektträdet.
// Tillståndsmaskin: START -> (knapptryck) -> PLAYING -> (krock) -> GAME_OVER -> (knapptryck) -> PLAYING
// Här bestäms vad som händer varje spelsteg; själva ormen, äpplet och ritningen
// sköts av snake.c, apple.c och vga.c.

#include "game.h"
#include <stdint.h>
#include "snake.h"
#include "apple.h"
#include "timer.h"
#include "vga.h"

// Spelstegets längd: börjar på 150 ms och blir 5 ms kortare per äpple, ner till 50 ms
#define START_PERIOD_MS 150
#define SPEEDUP_MS      5
#define MIN_PERIOD_MS   50

// 7-segmentsdisplayerna (visar poängen under spelets gång)
#define HEX_BASE_ADDR 0x04000050 // Första displayen, nästa ligger +0x10
#define HEX_COUNT     6
#define HEX_BLANK     0xFF       // Alla segment släckta

// Segmentmönster för 0-9 (0 = segmentet lyser, bit 7 = decimalpunkt, hålls släckt)
static const uint8_t hex_digits[10] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};

// ===================== Game State =====================

typedef enum {
    STATE_START,     // Startskärmen, väntar på knapptryck
    STATE_PLAYING,   // Spelet pågår
    STATE_GAME_OVER  // Visar poäng, väntar på knapptryck
} game_state;

static game_state state; // Nuvarande tillstånd
static unsigned score;   // Antal uppätna äpplen

// ===================== Score =====================

// Visar poängen på 7-segmentsdisplayerna (display 0 längst till höger = ental)
static void show_score(void) {
    unsigned value = score;
    for (int i = 0; i < HEX_COUNT; i++) {
        volatile uint32_t* hex = (volatile uint32_t*)(HEX_BASE_ADDR + i * 0x10);
        // Släck inledande nollor, men visa alltid entalssiffran
        *hex = (value == 0 && i > 0) ? HEX_BLANK : hex_digits[value % 10];
        value /= 10;
    }
}

// ===================== Hastighetsuppdatering =====================

// Fler äpplen = kortare spelsteg = snabbare orm
static void update_speed(void) {
    int period_ms = START_PERIOD_MS - (int)score * SPEEDUP_MS;
    if (period_ms < MIN_PERIOD_MS)
        period_ms = MIN_PERIOD_MS;
    timer_set_period_ms(period_ms);
}

// ===================== Spelstart / Restart / Reset =====================

// Nollställer allt och startar en ny omgång. seed = startvärde för äpplets slumptal.
static void game_start(uint32_t seed) {
    score = 0;
    show_score();

    vga_clear_screen();
    vga_draw_board();
    snake_init();

    apple_seed(seed); // Pseudoslumptal för Apple
    apple_spawn();

    update_speed(); // Börja på starthastigheten
    state = STATE_PLAYING;
}

// ===================== Game Over =====================

static void game_over(void) {
    state = STATE_GAME_OVER;
    vga_draw_game_over_screen(score);
}

// ===================== Apple-hantering =====================

// Anropas när ormen har ätit äpplet (ormen har redan vuxit)
static void eat_apple(void) {
    score++;
    show_score();

    if (snake_is_full()) { // Hela planen är fylld, ingen plats för fler äpplen
        game_over();
        return;
    }
    apple_spawn();  // Nytt äpple på en ledig plats
    update_speed(); // Snabbare för varje äpple
}

// ===================== Speluppdatering (ett spelsteg) =====================

static void game_step(void) {
    position next = snake_next_head();  // Dit huvudet är på väg
    int ate = apple_is_at(next);        // Äter ormen äpplet i detta steg?

    // Krock med vägg eller sig själv? (väggen kollas först, så next ligger alltid på rutnätet)
    if (snake_hits_wall(next) || snake_hits_self(next, ate)) {
        game_over();
        return;
    }

    snake_move(ate); // Flytta ormen, och låt den växa om den åt äpplet
    if (ate)
        eat_apple();
}

// ===================== Tillståndsmaskinen =====================

void game_init(void) {
    score = 0;
    show_score();
    vga_draw_start_screen();
    state = STATE_START;
}

void game_update(int direction, int button_pressed, int tick) {
    switch (state) {
    case STATE_START:
    case STATE_GAME_OVER:
        // Knapptryck startar en ny omgång. Tidpunkten för trycket ger slumpfröet.
        if (button_pressed)
            game_start(timer_snapshot());
        break;

    case STATE_PLAYING:
        snake_set_direction(direction); // Önskad riktning (DIR_NONE ignoreras)
        if (tick)                       // Ett spelsteg per timeravbrott
            game_step();
        break;
    }
}

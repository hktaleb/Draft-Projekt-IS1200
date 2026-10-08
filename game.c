// Author: Hussein Taleb

#include "game.h"
#include "snake.h"
#include "timer.h"
#include "vga.h"

// game states
#define STATE_START     0
#define STATE_PLAYING   1
#define STATE_GAME_OVER 2

// snake speed
#define START_PERIOD_MS 150
#define SPEEDUP_MS 5
#define MIN_PERIOD_MS 50

// addresses for the 7-segment displays
#define HEX_BASE_ADDR 0x04000050
#define HEX_COUNT 6
#define HEX_BLANK 0xFF

// current game state
static int state;

// current score
static int score;

// apple position
static position apple;

// value used to generate new apple positions
static unsigned int random_value;


// returns the display pattern for a digit
static int digit_pattern(int digit)
{
    if (digit == 0)
        return 0xC0;

    if (digit == 1)
        return 0xF9;

    if (digit == 2)
        return 0xA4;

    if (digit == 3)
        return 0xB0;

    if (digit == 4)
        return 0x99;

    if (digit == 5)
        return 0x92;

    if (digit == 6)
        return 0x82;

    if (digit == 7)
        return 0xF8;

    if (digit == 8)
        return 0x80;

    if (digit == 9)
        return 0x90;

    return HEX_BLANK;
}


// shows the score on the 7-segment displays
static void show_score(void)
{
    int value = score;

    for (int i = 0; i < HEX_COUNT; i++) {

        volatile int *display;
        display = (volatile int *)(HEX_BASE_ADDR + i * 0x10);

        // show the current digit
        if (value > 0 || i == 0) {
            *display = digit_pattern(value % 10);
        }
        else {
            // hide unused displays
            *display = HEX_BLANK;
        }

        value = value / 10;
    }
}


// generates a new pseudo-random number
static unsigned int random_number(void)
{
    random_value = random_value * 1103515245 + 12345;

    return random_value;
}


// creates a new apple on an empty position
static void spawn_apple(void)
{
    do {
        // keep the apple inside the walls
        apple.x = 1 + random_number() % (GRID_WIDTH - 2);
        apple.y = 1 + random_number() % (GRID_HEIGHT - 2);

    } while (snake_occupies(apple));

    // draw the apple
    vga_draw_apple(apple.x, apple.y);
}


// checks if the apple is at a position
static int apple_is_at(position p)
{
    if (p.x == apple.x && p.y == apple.y) {
        return 1;
    }

    return 0;
}


// updates the speed of the snake
static void update_speed(void)
{
    int period = START_PERIOD_MS - score * SPEEDUP_MS;

    // do not allow the snake to become too fast
    if (period < MIN_PERIOD_MS) {
        period = MIN_PERIOD_MS;
    }

    timer_set_period_ms(period);
}


// starts a new game
static void game_start(void)
{
    score = 0;
    show_score();

    // clear and draw the game field
    vga_clear_screen();
    vga_draw_board();

    // create the snake
    snake_init();

    // use the current timer value as the random starting value
    random_value = timer_snapshot();

    // create the first apple
    spawn_apple();

    // set the starting speed
    update_speed();

    state = STATE_PLAYING;
}


// ends the current game
static void game_over(void)
{
    state = STATE_GAME_OVER;

    vga_draw_game_over_screen(score);
}


// handles an eaten apple
static void eat_apple(void)
{
    score = score + 1;

    show_score();

    // end the game if the snake fills the entire field
    if (snake_is_full()) {
        game_over();
        return;
    }

    // create a new apple
    spawn_apple();

    // increase the snake speed
    update_speed();
}


// performs one game step
static void game_step(void)
{
    position next = snake_next_head();

    int ate = apple_is_at(next);

    // check for wall collision first
    if (snake_hits_wall(next)) {
        game_over();
        return;
    }

    // check for collision with the snake
    if (snake_hits_self(next, ate)) {
        game_over();
        return;
    }

    // move the snake
    snake_move(ate);

    // handle the eaten apple
    if (ate == 1) {
        eat_apple();
    }
}


// initializes the game
void game_init(void)
{
    score = 0;

    show_score();

    vga_draw_start_screen();

    state = STATE_START;
}


// updates the game
void game_update(int direction, int button_pressed, int tick)
{
    // start screen
    if (state == STATE_START) {

        if (button_pressed == 1) {
            game_start();
        }

        return;
    }

    // game over screen
    if (state == STATE_GAME_OVER) {

        if (button_pressed == 1) {
            game_start();
        }

        return;
    }

    // game is running
    if (state == STATE_PLAYING) {

        snake_set_direction(direction);

        if (tick == 1) {
            game_step();
        }
    }
}
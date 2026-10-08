// Authors: Hussein Taleb & Ardwan Al-Geilani

#include "vga.h"

#define VGA_FRAMEBUFFER 0x08000000
#define VGA_CTRL 0x04000100

// game colors
#define COLOR_WALL       COLOR_WHITE
#define COLOR_SNAKE_HEAD COLOR_YELLOW
#define COLOR_SNAKE_BODY COLOR_GREEN
#define COLOR_APPLE      COLOR_RED

// font size
#define FONT_WIDTH 3
#define FONT_HEIGHT 5


// initializes the VGA display
void vga_init(void)
{
    volatile unsigned int *ctrl =
        (volatile unsigned int *) VGA_CTRL;

    ctrl[1] = VGA_FRAMEBUFFER;
    ctrl[0] = 0;
}


// draws a rectangle
void vga_fill_rect(int x, int y, int width, int height, unsigned char color)
{
    volatile unsigned char *framebuffer =
        (volatile unsigned char *) VGA_FRAMEBUFFER;

    int start_x = x;
    int start_y = y;
    int end_x = x + width;
    int end_y = y + height;

    // keep the rectangle inside the screen
    if (start_x < 0)
        start_x = 0;

    if (start_y < 0)
        start_y = 0;

    if (end_x > SCREEN_WIDTH)
        end_x = SCREEN_WIDTH;

    if (end_y > SCREEN_HEIGHT)
        end_y = SCREEN_HEIGHT;

    // draw every pixel
    for (int py = start_y; py < end_y; py++) {
        for (int px = start_x; px < end_x; px++) {
            framebuffer[py * SCREEN_WIDTH + px] = color;
        }
    }
}


// draws one grid cell
void vga_draw_cell(int cell_x, int cell_y, unsigned char color)
{
    int pixel_x = cell_x * CELL_SIZE;
    int pixel_y = cell_y * CELL_SIZE;

    vga_fill_rect(
        pixel_x,
        pixel_y,
        CELL_SIZE,
        CELL_SIZE,
        color
    );
}


// clears the entire screen
void vga_clear_screen(void)
{
    vga_fill_rect(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        COLOR_BLACK
    );
}


// clears the area inside the walls
void vga_clear_playfield(void)
{
    vga_fill_rect(
        CELL_SIZE,
        CELL_SIZE,
        SCREEN_WIDTH - 2 * CELL_SIZE,
        SCREEN_HEIGHT - 2 * CELL_SIZE,
        COLOR_BLACK
    );
}


// returns one row of a character
static unsigned char get_font_row(char c, int row)
{
    // numbers
    static const unsigned char numbers[10][5] = {
        {7, 5, 5, 5, 7},
        {2, 6, 2, 2, 7},
        {7, 1, 7, 4, 7},
        {7, 1, 7, 1, 7},
        {5, 5, 7, 1, 1},
        {7, 4, 7, 1, 7},
        {7, 4, 7, 5, 7},
        {7, 1, 1, 1, 1},
        {7, 5, 7, 5, 7},
        {7, 5, 7, 1, 7}
    };

    if (c >= '0' && c <= '9') {
        return numbers[c - '0'][row];
    }

    // letters
    if (c == 'A') {
        static const unsigned char letter[5] = {2, 5, 7, 5, 5};
        return letter[row];
    }

    if (c == 'B') {
        static const unsigned char letter[5] = {6, 5, 6, 5, 6};
        return letter[row];
    }

    if (c == 'C') {
        static const unsigned char letter[5] = {3, 4, 4, 4, 3};
        return letter[row];
    }

    if (c == 'E') {
        static const unsigned char letter[5] = {7, 4, 6, 4, 7};
        return letter[row];
    }

    if (c == 'G') {
        static const unsigned char letter[5] = {3, 4, 5, 5, 3};
        return letter[row];
    }

    if (c == 'K') {
        static const unsigned char letter[5] = {5, 5, 6, 5, 5};
        return letter[row];
    }

    if (c == 'M') {
        static const unsigned char letter[5] = {5, 7, 7, 5, 5};
        return letter[row];
    }

    if (c == 'N') {
        static const unsigned char letter[5] = {6, 5, 5, 5, 5};
        return letter[row];
    }

    if (c == 'O') {
        static const unsigned char letter[5] = {2, 5, 5, 5, 2};
        return letter[row];
    }

    if (c == 'P') {
        static const unsigned char letter[5] = {6, 5, 6, 4, 4};
        return letter[row];
    }

    if (c == 'R') {
        static const unsigned char letter[5] = {6, 5, 6, 5, 5};
        return letter[row];
    }

    if (c == 'S') {
        static const unsigned char letter[5] = {3, 4, 2, 1, 6};
        return letter[row];
    }

    if (c == 'T') {
        static const unsigned char letter[5] = {7, 2, 2, 2, 2};
        return letter[row];
    }

    if (c == 'V') {
        static const unsigned char letter[5] = {5, 5, 5, 5, 2};
        return letter[row];
    }

    return 0;
}


// draws one character
static void draw_char(char c, int x, int y, int scale, unsigned char color)
{
    for (int row = 0; row < FONT_HEIGHT; row++) {

        unsigned char font_row = get_font_row(c, row);

        for (int col = 0; col < FONT_WIDTH; col++) {

            int bit = 1 << (FONT_WIDTH - 1 - col);

            if (font_row & bit) {
                vga_fill_rect(
                    x + col * scale,
                    y + row * scale,
                    scale,
                    scale,
                    color
                );
            }
        }
    }
}


// draws text in the center of the screen
static void draw_text_centered(const char *text, int y, int scale,
                               unsigned char color)
{
    int length = 0;

    // count the characters
    while (text[length] != '\0') {
        length = length + 1;
    }

    int distance = (FONT_WIDTH + 1) * scale;
    int text_width = length * distance - scale;
    int start_x = (SCREEN_WIDTH - text_width) / 2;

    // draw every character
    for (int i = 0; i < length; i++) {
        draw_char(
            text[i],
            start_x + i * distance,
            y,
            scale,
            color
        );
    }
}


// draws a number in the center of the screen
static void draw_number_centered(unsigned int number, int y, int scale,
                                 unsigned char color)
{
    char text[11];
    int index = 10;

    text[index] = '\0';

    do {
        index = index - 1;

        text[index] = '0' + number % 10;

        number = number / 10;

    } while (number > 0);

    draw_text_centered(&text[index], y, scale, color);
}


// draws the walls
void vga_draw_board(void)
{
    // draw top and bottom walls
    for (int x = 0; x < GRID_WIDTH; x++) {
        vga_draw_cell(x, 0, COLOR_WALL);
        vga_draw_cell(x, GRID_HEIGHT - 1, COLOR_WALL);
    }

    // draw left and right walls
    for (int y = 0; y < GRID_HEIGHT; y++) {
        vga_draw_cell(0, y, COLOR_WALL);
        vga_draw_cell(GRID_WIDTH - 1, y, COLOR_WALL);
    }
}


// draws the snake head
void vga_draw_snake_head(int cell_x, int cell_y)
{
    vga_draw_cell(cell_x, cell_y, COLOR_SNAKE_HEAD);
}


// draws the snake body
void vga_draw_snake_body(int cell_x, int cell_y)
{
    vga_draw_cell(cell_x, cell_y, COLOR_SNAKE_BODY);
}


// removes one cell
void vga_erase_cell(int cell_x, int cell_y)
{
    vga_draw_cell(cell_x, cell_y, COLOR_BLACK);
}


// draws the apple
void vga_draw_apple(int cell_x, int cell_y)
{
    vga_draw_cell(cell_x, cell_y, COLOR_APPLE);
}


// draws the score
void vga_draw_score(unsigned int score)
{
    draw_text_centered("SCORE", 100, 3, COLOR_WHITE);

    draw_number_centered(score, 130, 5, COLOR_YELLOW);
}


// draws the start screen
void vga_draw_start_screen(void)
{
    vga_clear_screen();

    vga_draw_board();

    draw_text_centered("SNAKE", 70, 6, COLOR_GREEN);

    draw_text_centered("PRESS BTN", 150, 3, COLOR_WHITE);
}


// draws the game over screen
void vga_draw_game_over_screen(unsigned int score)
{
    vga_clear_playfield();

    draw_text_centered("GAME OVER", 40, 4, COLOR_RED);

    vga_draw_score(score);

    draw_text_centered("PRESS BTN", 200, 2, COLOR_WHITE);
}
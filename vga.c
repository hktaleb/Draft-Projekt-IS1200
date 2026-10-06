// vga.c - VGA i projektträdet.
// Ritar genom att skriva direkt i kortets framebuffer (en byte per pixel).
// Spelet tänker i celler (40 x 30), här omvandlas de till pixlar (320 x 240).

#include "vga.h"
#include <stdint.h>

#define VGA_FRAMEBUFFER 0x08000000 // Framebuffer: 320 x 240 pixlar, 1 byte per pixel
#define VGA_CTRL        0x04000100 // Styrregister för vilken buffert som visas

// Färger för spelets delar
#define COLOR_WALL       COLOR_WHITE
#define COLOR_SNAKE_HEAD COLOR_YELLOW
#define COLOR_SNAKE_BODY COLOR_GREEN
#define COLOR_APPLE      COLOR_RED

// ===================== DTEK-V VGA =====================

void vga_init(void) {
    volatile uint32_t* ctrl = (volatile uint32_t*)VGA_CTRL;
    ctrl[1] = VGA_FRAMEBUFFER; // Sätt bakre bufferten till vår framebuffer
    ctrl[0] = 0;               // Byt buffert, så att den blir den som visas
}

// ===================== Pixelhantering =====================

void vga_fill_rect(int x, int y, int w, int h, uint8_t color) {
    volatile uint8_t* framebuffer = (volatile uint8_t*)VGA_FRAMEBUFFER;

    // Klipp rektangeln så att vi aldrig skriver utanför skärmen
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w > SCREEN_WIDTH ? SCREEN_WIDTH : x + w;
    int y1 = y + h > SCREEN_HEIGHT ? SCREEN_HEIGHT : y + h;

    // Pixeln (px, py) ligger på plats py * 320 + px i framebuffern
    for (int py = y0; py < y1; py++) {
        for (int px = x0; px < x1; px++) {
            framebuffer[py * SCREEN_WIDTH + px] = color;
        }
    }
}

// ===================== Cellhantering och koordinatkonvertering =====================

void vga_draw_cell(int cell_x, int cell_y, uint8_t color) {
    // Cellkoordinat * 8 = pixelkoordinat
    vga_fill_rect(cell_x * CELL_SIZE, cell_y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

// ===================== Skärmrensning =====================

void vga_clear_screen(void) {
    vga_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BLACK);
}

void vga_clear_playfield(void) {
    // Allt utom den yttersta raden/kolumnen av celler (väggarna)
    vga_fill_rect(CELL_SIZE, CELL_SIZE,
                  SCREEN_WIDTH - 2 * CELL_SIZE, SCREEN_HEIGHT - 2 * CELL_SIZE,
                  COLOR_BLACK);
}

// ===================== Text (hjälp för Score, Startskärm och Game Over) =====================

// Typsnitt: varje tecken är 3 x 5 "pixlar", en byte per rad (bit 2 = vänstra pixeln)
#define FONT_WIDTH  3
#define FONT_HEIGHT 5

typedef struct {
    char c;                    // Tecknet
    uint8_t rows[FONT_HEIGHT]; // Dess utseende, rad för rad
} glyph;

// Alla tecken vi behöver. T.ex. '0' = 111, 101, 101, 101, 111.
static const glyph font[] = {
    {'0', {7, 5, 5, 5, 7}}, {'1', {2, 6, 2, 2, 7}}, {'2', {7, 1, 7, 4, 7}},
    {'3', {7, 1, 7, 1, 7}}, {'4', {5, 5, 7, 1, 1}}, {'5', {7, 4, 7, 1, 7}},
    {'6', {7, 4, 7, 5, 7}}, {'7', {7, 1, 1, 1, 1}}, {'8', {7, 5, 7, 5, 7}},
    {'9', {7, 5, 7, 1, 7}},
    {'A', {2, 5, 7, 5, 5}}, {'B', {6, 5, 6, 5, 6}}, {'C', {3, 4, 4, 4, 3}},
    {'E', {7, 4, 6, 4, 7}}, {'G', {3, 4, 5, 5, 3}}, {'K', {5, 5, 6, 5, 5}},
    {'M', {5, 7, 7, 5, 5}}, {'N', {6, 5, 5, 5, 5}}, {'O', {2, 5, 5, 5, 2}},
    {'P', {6, 5, 6, 4, 4}}, {'R', {6, 5, 6, 5, 5}}, {'S', {3, 4, 2, 1, 6}},
    {'T', {7, 2, 2, 2, 2}}, {'V', {5, 5, 5, 5, 2}},
};

// Letar upp ett tecken i typsnittet, returnerar dess rader eller 0 om det saknas
static const uint8_t* find_glyph(char c) {
    for (unsigned i = 0; i < sizeof(font) / sizeof(font[0]); i++) {
        if (font[i].c == c)
            return font[i].rows;
    }
    return 0; // Okända tecken (t.ex. mellanslag) ritas inte
}

// Ritar ett tecken med övre vänstra hörnet i (x, y); varje typsnittspixel blir scale x scale pixlar
static void draw_char(char c, int x, int y, int scale, uint8_t color) {
    const uint8_t* rows = find_glyph(c);
    if (!rows)
        return;

    for (int row = 0; row < FONT_HEIGHT; row++) {
        for (int col = 0; col < FONT_WIDTH; col++) {
            if (rows[row] & (1 << (FONT_WIDTH - 1 - col))) // Är pixeln tänd?
                vga_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
        }
    }
}

// Skriver text centrerad i sidled på höjden y
static void draw_text_centered(const char* text, int y, int scale, uint8_t color) {
    // Räkna antal tecken
    int len = 0;
    while (text[len] != '\0')
        len++;

    int advance = (FONT_WIDTH + 1) * scale;               // Avstånd mellan tecken (med en tom kolumn)
    int x = (SCREEN_WIDTH - (len * advance - scale)) / 2; // Startposition så att texten hamnar i mitten

    for (int i = 0; i < len; i++)
        draw_char(text[i], x + i * advance, y, scale, color);
}

// Skriver ett tal centrerat i sidled på höjden y
static void draw_number_centered(unsigned number, int y, int scale, uint8_t color) {
    // Gör om talet till text, siffra för siffra bakifrån
    char buf[11]; // Räcker för ett 32-bitars tal + '\0'
    int i = sizeof(buf) - 1;
    buf[i] = '\0';
    do {
        buf[--i] = '0' + number % 10; // Sista siffran
        number /= 10;                 // Ta bort sista siffran
    } while (number > 0);

    draw_text_centered(&buf[i], y, scale, color);
}

// ===================== Rendering =====================

// Spelplan: väggar runt hela skärmen
void vga_draw_board(void) {
    // Topp och botten
    for (int x = 0; x < GRID_WIDTH; x++) {
        vga_draw_cell(x, 0, COLOR_WALL);
        vga_draw_cell(x, GRID_HEIGHT - 1, COLOR_WALL);
    }
    // Vänster och höger
    for (int y = 0; y < GRID_HEIGHT; y++) {
        vga_draw_cell(0, y, COLOR_WALL);
        vga_draw_cell(GRID_WIDTH - 1, y, COLOR_WALL);
    }
}

// Snake
void vga_draw_snake_head(int cell_x, int cell_y) {
    vga_draw_cell(cell_x, cell_y, COLOR_SNAKE_HEAD);
}

void vga_draw_snake_body(int cell_x, int cell_y) {
    vga_draw_cell(cell_x, cell_y, COLOR_SNAKE_BODY);
}

void vga_erase_cell(int cell_x, int cell_y) {
    vga_draw_cell(cell_x, cell_y, COLOR_BLACK);
}

// Apple
void vga_draw_apple(int cell_x, int cell_y) {
    vga_draw_cell(cell_x, cell_y, COLOR_APPLE);
}

// Score: rubrik och antal äpplen mitt på skärmen
void vga_draw_score(unsigned score) {
    draw_text_centered("SCORE", 100, 3, COLOR_WHITE);
    draw_number_centered(score, 130, 5, COLOR_YELLOW);
}

// Startskärm
void vga_draw_start_screen(void) {
    vga_clear_screen();
    vga_draw_board();
    draw_text_centered("SNAKE", 70, 6, COLOR_GREEN);
    draw_text_centered("PRESS BTN", 150, 3, COLOR_WHITE);
}

// Game Over-skärm: väggarna står kvar, resten ersätts med text och poäng
void vga_draw_game_over_screen(unsigned score) {
    vga_clear_playfield();
    draw_text_centered("GAME OVER", 40, 4, COLOR_RED);
    vga_draw_score(score);
    draw_text_centered("PRESS BTN", 200, 2, COLOR_WHITE);
}

// vga.h - VGA i projektträdet.
// All ritning på skärmen: pixlar, celler (rutor), spelplan, orm, äpple, poäng och skärmar.

#ifndef VGA_H
#define VGA_H

#include <stdint.h>

// --- Skärm och rutnät ---
#define SCREEN_WIDTH  320 // Skärmens bredd i pixlar
#define SCREEN_HEIGHT 240 // Skärmens höjd i pixlar
#define CELL_SIZE     8   // En cell (ruta) är 8x8 pixlar

// Rutnätet: 40 x 30 celler, där den yttersta raden/kolumnen är vägg
#define GRID_WIDTH  (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)

// Spelytan innanför väggarna: 38 x 28 celler
#define PLAYFIELD_WIDTH  (GRID_WIDTH - 2)
#define PLAYFIELD_HEIGHT (GRID_HEIGHT - 2)
#define PLAYFIELD_CELLS  (PLAYFIELD_WIDTH * PLAYFIELD_HEIGHT)

// --- Färger i RGB332-format (8 bitar: RRRGGGBB) ---
#define COLOR_BLACK  0x00
#define COLOR_WHITE  0xFF
#define COLOR_RED    0xE0
#define COLOR_GREEN  0x1C
#define COLOR_BLUE   0x03
#define COLOR_YELLOW 0xFC

// Ser till att VGA visar vår framebuffer
void vga_init(void);

// Pixelhantering: fyller en rektangel, (x, y) och storlek i pixlar
void vga_fill_rect(int x, int y, int w, int h, uint8_t color);

// Cellhantering: fyller en cell i rutnätet (cellkoordinat omvandlas till pixlar)
void vga_draw_cell(int cell_x, int cell_y, uint8_t color);

// Skärmrensning
void vga_clear_screen(void);    // Hela skärmen svart
void vga_clear_playfield(void); // Allt innanför väggarna svart

// Rendering
void vga_draw_board(void);                       // Väggarna runt spelplanen
void vga_draw_snake_head(int cell_x, int cell_y);
void vga_draw_snake_body(int cell_x, int cell_y);
void vga_erase_cell(int cell_x, int cell_y);     // Suddar en cell (t.ex. där svansen var)
void vga_draw_apple(int cell_x, int cell_y);
void vga_draw_score(unsigned score);             // "SCORE" + antal äpplen
void vga_draw_start_screen(void);
void vga_draw_game_over_screen(unsigned score);

#endif

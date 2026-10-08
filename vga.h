// Authors: Hussein Taleb & Ardwan Al-Geilani

#ifndef VGA_H
#define VGA_H

// screen size in pixels
/* VGA Screen Buffer (320x240x2)	0x08000000-0x080257ff
   VGA Buffer 0x8000000 - 0x80257ff + 1 = 0x25800 = 153600 bytes 
   320 × 240 = 76800 pixels
   153600 bytes / 76800 pixels = 2 bytes per pixel
*/
#define SCREEN_WIDTH  320 // these values come from documentation
#define SCREEN_HEIGHT 240

// size of one grid cell in pixels
#define CELL_SIZE 8 // one square is 8x8 pixels

// grid size
#define GRID_WIDTH  (SCREEN_WIDTH / CELL_SIZE) // 320 / 8 = 40
#define GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE) // 240 / 8 = 30

// playable area inside the walls
#define PLAYFIELD_WIDTH  (GRID_WIDTH - 2) // 40 - 2 = 38
#define PLAYFIELD_HEIGHT (GRID_HEIGHT - 2) // 30 -2 = 28
#define PLAYFIELD_CELLS  (PLAYFIELD_WIDTH * PLAYFIELD_HEIGHT) // 38x28 = 1064 squares

// colors
#define COLOR_BLACK  0x00
#define COLOR_WHITE  0xFF
#define COLOR_RED    0xE0
#define COLOR_GREEN  0x1C
#define COLOR_BLUE   0x03
#define COLOR_YELLOW 0xFC

// initializes the VGA display
void vga_init(void);

// draws a rectangle
void vga_fill_rect(int x, int y, int width, int height, unsigned char color);

// draws one grid cell
void vga_draw_cell(int cell_x, int cell_y, unsigned char color);

// clears the screen
void vga_clear_screen(void);

// clears the area inside the walls
void vga_clear_playfield(void);

// draws the walls around the game field
void vga_draw_board(void);

// draws the snake head
void vga_draw_snake_head(int cell_x, int cell_y);

// draws the snake body
void vga_draw_snake_body(int cell_x, int cell_y);

// removes a cell from the screen
void vga_erase_cell(int cell_x, int cell_y);

// draws the apple
void vga_draw_apple(int cell_x, int cell_y);

// draws the score
void vga_draw_score(unsigned int score);

// draws the start screen
void vga_draw_start_screen(void);

// draws the game over screen
void vga_draw_game_over_screen(unsigned int score);

#endif
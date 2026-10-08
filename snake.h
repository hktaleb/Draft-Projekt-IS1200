// Author: Ardwan Al-Geilani

#ifndef SNAKE_H // if not defined
#define SNAKE_H // define


#include "vga.h" // include header file

// position x/y (used both by snake and apple)
typedef struct{
    int x;
    int y;
} position;

// direction for snake
#define DIR_NONE  -1 // no new direction
#define DIR_UP     0 // snake moves up
#define DIR_DOWN   1 // snake moves down
#define DIR_LEFT   2 // snake moves left
#define DIR_RIGHT  3 // snake moves right
#define DIR_COUNT  4 // possible directions

// returns the opposite direction
int opposite_direction(int dir); // This is used to prevent opposite change of direction

// maxlength: the snake can fill the entire area inside the walls
#define SNAKE_MAX_LEN   PLAYFIELD_CELLS
#define SNAKE_START_LEN 3

// initializes the snake in the middle of the board, facing right, and draws it
// empties the grid-field
void snake_init(void);

// changes the snakes direction and prevents 180-degree turns
void snake_set_direction(int dir);

// returns the position where the snakes head will be after the next move
position snake_next_head(void);

// moves the snake one step and redraws the changed parts.
// if grow is 1, the snake also grows by one segment
void snake_move(int grow);

// collision detection
int snake_hits_wall(position p); // checks if the position collides with a wall
int snake_hits_self(position p, int grow); // checks if the position collides with the snakes body

// returns 1 if the snake occupies the given position
int snake_occupies(position p);

int snake_length(void); // returns the current length of the snake
int snake_is_full(void); // returns 1 if the snake has reached its maximum length
#endif

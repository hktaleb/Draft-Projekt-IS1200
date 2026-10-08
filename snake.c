// Author: Ardwan Al-Geilani

#include "snake.h" // include header file

// segment-array: segments[0] is the head, segments[length - 1] is the tail
static position segments[SNAKE_MAX_LEN]; // 1064 cells
static int length; // the snakes length

// stores which grid cells are occupied by the snake, this includes the bounds
static unsigned char occupied[GRID_HEIGHT][GRID_WIDTH];

static int heading; // direction of the snake
static int next_dir; // direction of the snakes next movement

void snake_init(void){
    // empty the grid-field
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            occupied[y][x] = 0;

    // snake starts with 3 segments
    length = SNAKE_START_LEN;

    // this loop places the snake in the middle of the grid-field
    for (int i = 0; i < length; i++){

        segments[i].x = GRID_WIDTH / 2 - i; // this part puts the head, body and tail one step after each on the x-axis
        segments[i].y = GRID_HEIGHT / 2; // this part places everything on the y-axis

        occupied[segments[i].y][segments[i].x] = 1; // mark as occupied

        if (i == 0)
            vga_draw_snake_head(segments[i].x, segments[i].y); // draws the head of the snake at first index
        else
            vga_draw_snake_body(segments[i].x, segments[i].y); // draws the rest of the body on other indexes
    }

    // snake starts by moving right
    heading = DIR_RIGHT; // where the snake is moving at the moment
    next_dir = DIR_RIGHT; // where it will go afterwards
}

/* this function returns the opposite diretion which was defined at snake header file 
    it is used for preventing turning the opposite direction*/
int opposite_direction(int dir){
    if (dir == DIR_UP) // if 0
        return DIR_DOWN; // return 1

    if (dir == DIR_DOWN) // if 1
        return DIR_UP; // return 0

    if (dir == DIR_LEFT) // if 2
        return DIR_RIGHT; // return 3

    if (dir == DIR_RIGHT) // if 3
        return DIR_LEFT; // return 2

    return DIR_NONE; // if none of the above, return -1
}

/* this function sets the next direction and also prevents turning 180 degrees */
void snake_set_direction(int dir){
    // no button was pressed so it does nothing
    if (dir == DIR_NONE)
        return;

    // does not allow the snake to turn backwards by doing nothing
    if (dir == opposite_direction(heading))
        return;

    // if another value was given then it will be the next direction 
    next_dir = dir;
}

/* eg. the snake starts at position (0,0) if it moves up it wil be (-1,0), 
    this is from the vga file */
position snake_next_head(void){
    position head = segments[0];

    if (next_dir == DIR_UP) // y will decrease if moving up
        head.y = head.y - 1;

    if (next_dir == DIR_DOWN) // y will increase if going down
        head.y = head.y + 1;

    if (next_dir == DIR_LEFT)
        head.x = head.x - 1;

    if (next_dir == DIR_RIGHT)
        head.x = head.x + 1;

    return head;
}

/* this function moves the snake and/or increases the length of the snake */
void snake_move(int grow){
    position old_head = segments[0];
    position old_tail = segments[length - 1];
    position new_head = snake_next_head();

    // save the direction the snake is now moving in
    heading = next_dir;

    // if the snake eats an apple, increase its length
    if (grow == 1 && length < SNAKE_MAX_LEN)
        length = length + 1;
    else{
        // remove the old tail
        occupied[old_tail.y][old_tail.x] = 0;
        vga_erase_cell(old_tail.x, old_tail.y);
    }

    // move every body segment forward, but not the head
    for (int i = length - 1; i > 0; i--)
        segments[i] = segments[i - 1];

    // move the head to its new position
    segments[0] = new_head;

    // mark the new head position as occupied
    occupied[new_head.y][new_head.x] = 1;

    // draw the snake
    vga_draw_snake_body(old_head.x, old_head.y); // the old head becomes part of the body
    vga_draw_snake_head(new_head.x, new_head.y);
}

// collision with walls
int snake_hits_wall(position p){
    if (p.x <= 0) // the left wall
        return 1;

    if (p.x >= GRID_WIDTH - 1) //  the right wall
        return 1;

    if (p.y <= 0) // the top
        return 1;

    if (p.y >= GRID_HEIGHT - 1) // the bottom
        return 1;

    return 0;
}

// collision with self
int snake_hits_self(position p, int grow){
    position tail = segments[length - 1];

    // if the snake is not growing, the tail will move away
    if (grow == 0)
        if (p.x == tail.x && p.y == tail.y)
            return 0;

    // check if the position is occupied by the snake
    if (snake_occupies(p) == 1)
        return 1;

    return 0;
}

// returns whether position p is occupied by the snake
int snake_occupies(position p){
    return occupied[p.y][p.x];
}

// returns the current snake length
int snake_length(void){
    return length;
}

// returns whether the snake has reached its maximum length
// 1 if true, else 0
int snake_is_full(void){
    return length >= SNAKE_MAX_LEN;
}

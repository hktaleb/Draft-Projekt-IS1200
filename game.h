// Author: Hussein Taleb

#ifndef GAME_H
#define GAME_H

// initializes the game and shows the start screen
void game_init(void);

// updates the game
// direction = selected direction or DIR_NONE
// button_pressed = 1 if the button was pressed
// tick = 1 if it is time to move the snake
void game_update(int direction, int button_pressed, int tick);

#endif
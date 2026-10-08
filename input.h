// Author: Ardwan Al-Geilani

#ifndef INPUT_H // if not defined
#define INPUT_H // define

// initializes the input
void input_init(void);

// returns the direction selected by the player
int input_get_direction(void);

// returns 1 when the button is pressed
int input_button_pressed(void);

#endif
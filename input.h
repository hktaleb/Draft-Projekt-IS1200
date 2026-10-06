// input.h - INPUT i projektträdet.
// Läser kortets switchar (riktning) och knapp (start/omstart) med polling och debounce.

#ifndef INPUT_H
#define INPUT_H

// Sparar nuvarande läge på switchar och knapp, anropas en gång vid start
void input_init(void);

// Önskad riktning: returnerar DIR_UP/DOWN/LEFT/RIGHT (se snake.h) när en
// riktningsswitch slagits om, annars DIR_NONE
int input_get_direction(void);

// Returnerar 1 en gång per knapptryckning
int input_button_pressed(void);

#endif

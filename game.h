// game.h - GAME i projektträdet.
// Tillståndsmaskinen (START, PLAYING, GAME_OVER) som styr spelet.

#ifndef GAME_H
#define GAME_H

// Visar startskärmen, anropas en gång vid start
void game_init(void);

// Speluppdatering: anropas varje varv i huvudloopen med det som lästs från Input och Timer
//   direction      = önskad riktning eller DIR_NONE
//   button_pressed = 1 om knappen just tryckts ned
//   tick           = 1 om det är dags för ett spelsteg
void game_update(int direction, int button_pressed, int tick);

#endif

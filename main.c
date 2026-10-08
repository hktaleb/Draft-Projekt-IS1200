// Authors: Hussein Taleb & Ardwan Al-Geilani

#include "vga.h"
#include "input.h"
#include "timer.h"
#include "game.h"

int main(void)
{
    // initialize VGA
    vga_init();

    // initialize switches and button
    input_init();

    // initialize the timer
    timer_init(150);

    // initialize the game
    game_init();

    // main loop
    while (1){

        // read the selected direction
        int direction = input_get_direction();

        // check if the button was pressed
        int button_pressed = input_button_pressed();

        // check if a timer interrupt has occurred
        int tick = timer_take_tick();

        // update the game
        game_update(direction, button_pressed, tick);
    }

    return 0;
}
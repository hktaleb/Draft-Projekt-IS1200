// main.c - MAIN i projektträdet.
// Initierar systemet och kör huvudloopen, som samordnar Input, Timer och Game.

#include "vga.h"
#include "input.h"
#include "timer.h"
#include "game.h"

int main(void) {
    // ===== Systeminitialisering =====
    vga_init();      // VGA visar vår framebuffer
    input_init();    // Läs av switchar/knapp som startläge
    timer_init(100); // Starta timern med avbrott (spelet ställer in sin egen hastighet vid start)
    game_init();     // Visa startskärmen

    // ===== Huvudloop =====
    while (1) {
        // Samordning: läs Input och Timer, och låt Game avgöra vad som ska hända
        int direction = input_get_direction(); // Önskad riktning eller DIR_NONE
        int pressed = input_button_pressed();  // 1 om knappen just tryckts ned
        int tick = timer_take_tick();          // 1 om timern gett ett nytt spelsteg

        game_update(direction, pressed, tick); // Game ritar via VGA
    }

    return 0; // Nås aldrig, men main ska returnera int
}

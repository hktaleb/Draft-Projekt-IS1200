// Author: Ardwan Al-Geilani

#include "input.h"
#include "snake.h"

// addresses for the switches and button
#define SWITCH_ADDR 0x04000010
#define BUTTON_ADDR 0x040000d0

// number of identical readings required before accepting the input as stable
#define DEBOUNCE_SAMPLES 5000

// switch debounce values
static int stable_switches;
static int last_switches;
static int switch_count;

// button debounce values
static int stable_button;
static int last_button;
static int button_count;

// previous accepted values
static int previous_switches;
static int previous_button;

// reads the last four switches to the far right (SW0-SW3)
static int read_switches(void){
    volatile int *switches = (int*) SWITCH_ADDR;
    return *switches & 0xF;
}

// reads the button (the lower one)
static int read_button(void){
    volatile int *button = (int*) BUTTON_ADDR;
    return *button & 0x1;
}

// initializes the input
void input_init(void){
    int switches = read_switches(); // puts the four switches in variable switches
    int button = read_button(); // puts the button in variable button

    stable_switches = switches; // declare them as stable
    last_switches = switches; // save the latest values
    switch_count = 0; // init this switch counter to zero

    stable_button = button; // declare it as stable
    last_button = button; // save the latest value
    button_count = 0; // init this button counter to zero

    previous_switches = switches; // save the previous accepted switch value
    previous_button = button; // save the previous accepted button value
}

// returns a stable switch value
static int debounce_switches(void){
    int current = read_switches(); // the current values

    // switch value changed
    if(current != last_switches){ // checks if the current value is equal to the last value
        last_switches = current; 
        switch_count = 0;
        return stable_switches; // this returns the old value 
    }

    // returns the stable switches, and increases counter
    if(switch_count < DEBOUNCE_SAMPLES){
        switch_count = switch_count + 1;
        return stable_switches;
    }

    // if the switch_count is greater than 5000 then the current value will be stable
    stable_switches = current;

    return stable_switches; // returns the stable value
}

// returns a stable button value
static int debounce_button(void){
    int current = read_button(); // the current value of the button

    // button value changed
    if(current != last_button){ // checks if the current value is equal to the last value
        last_button = current;
        button_count = 0;
        return stable_button; // this returns the old value 
    }

    // returns the stable button, and increases counter
    if(button_count < DEBOUNCE_SAMPLES){
        button_count = button_count + 1;
        return stable_button;
    }

    // value has been stable long enough
    stable_button = current;

    return stable_button; // returns the stable value
}


// returns the direction selected by the player
int input_get_direction(void){
    int current = debounce_switches();

    // no switch has changed
    if(current == previous_switches)
        return DIR_NONE;

    int changed = current ^ previous_switches; // xor (which ones are different)

    previous_switches = current;

    // SW0 controls right
    if(changed & 0x1) // changed is anded with bit zero (0001), and if true it will go right
        return DIR_RIGHT;

    // SW1 controls down
    if(changed & 0x2) // changed is anded with bit one (0010), and if true it will go down
        return DIR_DOWN;

    // SW2 controls up
    if(changed & 0x4) // changed is anded with bit two (0100), and if true it will go up
        return DIR_UP;

    // SW3 controls left
    if(changed & 0x8) // changed is anded with bit four (1000), and if true it will go left
        return DIR_LEFT;

    return DIR_NONE; // just a failsafe
}


// returns 1 when the button is pressed
int input_button_pressed(void){
    int current = debounce_button();

    // button changed from released to pressed
    if(current == 1 && previous_button == 0){
        previous_button = current;
        return 1;
    }

    previous_button = current;

    return 0;
}
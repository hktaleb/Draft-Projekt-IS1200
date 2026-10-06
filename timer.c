// timer.c - TIMER i projektträdet.
// Timern löper ut en gång per spelsteg och ger då ett avbrott. Avbrottet sätter bara
// en flagga (game tick); huvudloopen ser flaggan och kör ett spelsteg.

#include "timer.h"
#include <stdint.h>

#define TIMER_BASE 0x04000020 // Timerns adress i DTEK-V:s minneskarta

// Timerns register, räknat i ord (4 byte) från TIMER_BASE
#define TIMER_STATUS  0 // Bit 0 = TO, sätts när timern löpt ut
#define TIMER_CONTROL 1 // Start/stopp och inställningar
#define TIMER_PERIODL 2 // Periodens låga 16 bitar
#define TIMER_PERIODH 3 // Periodens höga 16 bitar
#define TIMER_SNAPL   4 // Avläst räknarvärde, låga 16 bitar
#define TIMER_SNAPH   5 // Avläst räknarvärde, höga 16 bitar

// Bitar i kontrollregistret
#define CTRL_ITO   0x1 // Ge avbrott när timern löper ut
#define CTRL_CONT  0x2 // Börja om automatiskt (kontinuerligt läge)
#define CTRL_START 0x4 // Starta timern
#define CTRL_STOP  0x8 // Stoppa timern

#define TIMER_IRQ 16 // Timerns avbrottsnummer

// Finns i boot.S: slår på IRQ16 och avbrott globalt
extern void enable_interrupt(void);

// Pekare till timerns register
static volatile uint32_t* const timer = (volatile uint32_t*)TIMER_BASE;

// Game tick-flagga: sätts av avbrottet, läses av huvudloopen.
// volatile = måste läsas om varje gång, eftersom avbrottet kan ändra den när som helst.
static volatile int tick_pending = 0;

// ===================== Timerperiod och hastighetskontroll =====================

void timer_set_period_ms(uint32_t period_ms) {
    uint32_t period = period_ms * (TIMER_FREQ_HZ / 1000); // ms -> tick

    timer[TIMER_CONTROL] = CTRL_STOP; // Stoppa medan vi ändrar
    // Timern räknar period+1 tick, därför -1. Perioden delas i två 16-bitarsdelar.
    timer[TIMER_PERIODL] = (period - 1) & 0xFFFF;
    timer[TIMER_PERIODH] = ((period - 1) >> 16) & 0xFFFF;
    timer[TIMER_STATUS] = 0; // Nollställ gammal timeout-flagga
    tick_pending = 0;        // Släng ett eventuellt gammalt game tick
    timer[TIMER_CONTROL] = CTRL_START | CTRL_CONT | CTRL_ITO; // Starta igen med avbrott
}

void timer_init(uint32_t period_ms) {
    timer_set_period_ms(period_ms);
    enable_interrupt(); // Låt processorn ta emot timerns avbrott
}

// ===================== Timerinterrupt =====================

// Anropas automatiskt från boot.S vid avbrott, cause = avbrottsnummer
void handle_interrupt(unsigned cause) {
    if (cause == TIMER_IRQ) {      // Interruptidentifiering: kom avbrottet från timern?
        timer[TIMER_STATUS] = 0;   // Interruptkvittering: annars kommer avbrottet direkt igen
        tick_pending = 1;          // Game tick: säg till huvudloopen att köra ett spelsteg
    }
}

// ===================== Game tick =====================

int timer_take_tick(void) {
    if (tick_pending) {
        tick_pending = 0; // Markera steget som hanterat
        return 1;
    }
    return 0;
}

// ===================== Timerdata för random seed =====================

uint32_t timer_snapshot(void) {
    timer[TIMER_SNAPL] = 0; // En skrivning fryser räknarvärdet i snap-registren
    return (timer[TIMER_SNAPH] << 16) | (timer[TIMER_SNAPL] & 0xFFFF); // Sätt ihop till 32 bitar
}

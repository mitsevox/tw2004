// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void OSGetTick();

void fn_8000B3E8(void);
void fn_8000B408(void);
void fn_8000B3E8(void) {
    fn_8000B408();
}

void fn_8000B408(void) {
    OSGetTick();
}

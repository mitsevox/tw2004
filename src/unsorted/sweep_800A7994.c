// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Mov_Init();
void Mov_Exit();
void Mov_Start();
void Mov_Tick();
void fn_800B0858();

void fn_800A7994(void);
void fn_800A79B4(void);
void fn_800A79D4(void);
void fn_800A79F4(void);
void fn_800A7A14(void);
void fn_800A7994(void) {
    Mov_Init();
}

void fn_800A79B4(void) {
    Mov_Exit();
}

void fn_800A79D4(void) {
    Mov_Start();
}

void fn_800A79F4(void) {
    Mov_Tick();
}

void fn_800A7A14(void) {
    fn_800B0858();
}

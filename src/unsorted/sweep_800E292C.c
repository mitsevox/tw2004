// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* gpGame;

void fn_800E292C(void);
void fn_800E292C(void) {
    (*(s32 (**)(s32, void*))((u8*)(gpGame) + 0x1D4))(5, gpGame);
}
// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* gpGame;

void fn_800DCC04(void);
void fn_800DCC04(void) {
    (*(s32 (**)(void*))((u8*)(gpGame) + 0x1F0))(gpGame);
}
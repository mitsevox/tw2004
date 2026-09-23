// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 UStream_Update();
s32 fn_80015164();
s32 fn_8001518C();

void fn_800147A4(void);
void fn_800147A4(void) {
    fn_8001518C();
    do {

    } while (UStream_Update() != 0);
    fn_80015164();
}
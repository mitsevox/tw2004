// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 UStream_Update();
s32 fn_8001510C();
s32 fn_80015214();

void fn_800148EC(void);
void fn_800148EC(void) {
    fn_80015214();
    do {

    } while (UStream_Update() != 0);
    fn_8001510C();
}
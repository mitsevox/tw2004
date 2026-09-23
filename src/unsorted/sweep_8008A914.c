// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800F031C(s32);

void fn_8008A914(void** arg0, s32* arg1);
void fn_8008A914(void** arg0, s32* arg1) {
    if (fn_800F031C((*(s32*)((u8*)(*arg0) + 8))) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}
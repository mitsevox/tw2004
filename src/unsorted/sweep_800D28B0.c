// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800D28B0(s32 arg0, s32 arg1, s32* arg2, s32* arg3);
void fn_800D28B0(s32 arg0, s32 arg1, s32* arg2, s32* arg3) {
    if (arg0 == 0xC) {
        *arg2 = 1;
        *arg3 = arg1 + 1;
        return;
    }
    *arg2 = arg0 + 1;
    *arg3 = arg1;
}
// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s8 GOLFERSTATE_GetCurrentState(s32);

void fn_8008805C(s32* arg0, s32* arg1);
void fn_8008805C(s32* arg0, s32* arg1) {
    if (GOLFERSTATE_GetCurrentState(*arg0) == 3) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}
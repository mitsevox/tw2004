// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 Player_IsCPU(s32);

void fn_800873D4(s32* arg0, s32* arg1);
void fn_800873D4(s32* arg0, s32* arg1) {
    if (Player_IsCPU(*arg0) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}
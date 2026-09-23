// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80013050(s32);

void fn_80083430(s32* arg0, s32* arg1);
void fn_80083430(s32* arg0, s32* arg1) {
    if ((u32) (fn_80013050(*arg0) + 0x74F00000) == 0U) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}
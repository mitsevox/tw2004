// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008AC4C(u16, s32);
s32 fn_800A6F38();

void fn_80089C4C(s32* arg0);
void fn_80089C4C(s32* arg0) {
    s32 temp_r31;

    temp_r31 = *arg0;
    fn_800A6F38();
    fn_8008AC4C((u16) temp_r31, 0);
}
// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32* fn_800770FC();
s32 fn_800A75B4();

void fn_80083358(s32* arg0);
void fn_80083358(s32* arg0) {
    s32* temp_r3;

    temp_r3 = fn_800770FC();
    *temp_r3 = 3;
    if ((s32) *arg0 == -1) {
        *temp_r3 = 1;
    }
    fn_800A75B4();
}
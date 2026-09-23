// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800E53A4(s32);

void fn_80089A50(s32* arg0);
void fn_80089A50(s32* arg0) {
    if ((s32) *arg0 != 0) {
        fn_800E53A4(1);
        return;
    }
    fn_800E53A4(0);
}
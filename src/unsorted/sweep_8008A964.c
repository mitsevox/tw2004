// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800E53E8(s32);

void fn_8008A964(s32* arg0);
void fn_8008A964(s32* arg0) {
    if ((s32) *arg0 == 0) {
        fn_800E53E8(0);
        return;
    }
    fn_800E53E8(1);
}
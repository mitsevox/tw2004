// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A8248();
void** fn_800AD674();

void fn_800AD950(s32 arg0, s32 arg1, s32 arg2);
void fn_800AD950(s32 arg0, s32 arg1, s32 arg2) {
    if (fn_800AD674() != NULL) {
        fn_800A8248(arg0, arg1, arg2);
    }
}
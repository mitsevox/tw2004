// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A8424(u8, s32, f32);
u32 fn_800AD674();

void fn_800ADA94(u8 arg0, s32 arg1, f32 farg0);
void fn_800ADA94(u8 arg0, s32 arg1, f32 farg0) {
    if (fn_800AD674() != 0U) {
        fn_800A8424(arg0, arg1, farg0);
    }
}
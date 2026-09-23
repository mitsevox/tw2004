// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A84A4(s32, s32, f32);
u32 fn_800AD674();

void fn_800ADAF0(s32 arg0, s32 arg1, f32 farg0);
void fn_800ADAF0(s32 arg0, s32 arg1, f32 farg0) {
    if (fn_800AD674() != 0U) {
        fn_800A84A4(arg0, arg1, farg0);
    }
}
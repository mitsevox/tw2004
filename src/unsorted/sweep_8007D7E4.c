// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800770FC();
void fn_800A75B4();

void fn_8007D7E4(void);
void fn_8007D7E4(void) {
    s32 t0;
    t0 = fn_800770FC();
    *(s32*)((u8*)t0) = 2;
    fn_800A75B4();
}

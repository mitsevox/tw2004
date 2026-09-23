// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281B88;
s32 fn_80071854();

void fn_800718E4(void);
void fn_800718E4(void) {
    s32 t0;
    t0 = fn_80071854();
    *(s32*)((u8*)t0) = lbl_80281B88;
}

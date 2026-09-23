// Small functions found by the sweep (sweep.py). Original file: Skin.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8003532C();
void fn_8003612C();

void fn_80035FFC(void);
void fn_80036024(f32 x0);
void fn_80035FFC(void) {
    s32 t0;
    t0 = fn_8003532C();
    fn_8003612C((t0 + 84));
}

void fn_80036024(f32 x0) {
    s32 t0;
    t0 = fn_8003532C();
    *(f32*)(((u8*)t0) + 0x6C) = x0;
}

// Small functions found by the sweep (sweep.py). Original file: UFont.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_80012EC4();

void fn_80012868(s32 p0);
void fn_80012868(s32 p0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(s32*)(((u8*)t0) + 0xA0) = p0;
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80189900[];
s32 fn_800DADC0();

void fn_80089B8C(s32 p0, u8* p1);
void fn_80089B8C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800DADC0();
    *(s32*)p1 = t0;
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801910E0[];
extern s32 lbl_802821C0;
extern s32 lbl_802821C4;
s32 fn_80009B34();

void fn_800B90F4(s32 p0, s32 p1);
void fn_800B9100(u8* p0, s32 p1, s32 p2);
void fn_800B90F4(s32 p0, s32 p1) {
    lbl_802821C4 = p0;
    lbl_802821C0 = p1;
}

void fn_800B9100(u8* p0, s32 p1, s32 p2) {
    s32 t0;
    *(s32*)(p0 + 0xC) = 0;
    t0 = fn_80009B34(((u32)((p2 * p1) * 3) >> 1), 1, 32, lbl_801910E0, 79);
    *(s32*)(p0 + 0x8) = t0;
    *(s32*)p0 = p1;
    *(s32*)(p0 + 0x4) = p2;
}

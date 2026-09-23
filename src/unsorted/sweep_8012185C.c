// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80102A58();
s32 fn_80121A38();

void fn_8012185C(u8* p0, u8* p1);
void fn_80121890(u8* p0);
void fn_8012185C(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80121A38(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_80121890(u8* p0) {
    fn_80102A58(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

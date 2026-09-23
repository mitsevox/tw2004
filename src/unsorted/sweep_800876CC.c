// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800ECA08();
s32 fn_800ECA34();
s32 fn_800ECC14();

void fn_800876CC(s32 p0, u8* p1);
void fn_80087700(u8* p0, u8* p1);
void fn_80087734(s32 p0, u8* p1);
void fn_800876CC(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ECA08();
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80087700(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800ECA34(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_80087734(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ECC14();
    *(s32*)p1 = t0;
}

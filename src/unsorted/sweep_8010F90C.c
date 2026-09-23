// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802824B8;
s32 fn_8010F978();
s32 fn_8010FB70();

void fn_8010F90C(s32 p0, s32 p1, s32 p2);
void fn_8010F90C(s32 p0, s32 p1, s32 p2) {
    s32 t0;
    s32 t1;
    t0 = fn_8010FB70();
    t1 = fn_8010F978(p0, p1);
    *(u8*)(((u8*)t0) + t1) = p2;
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}

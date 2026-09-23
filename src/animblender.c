// animblender.c (TW06's golf/animation/animblender.c by its place in the link order; the name is
// not proven, and TW06's SKATime_* functions sit near its end): blends the golfer's animations.
// Mostly not yet decompiled; the machine-style code below is the sweep's.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

f32 fn_800728D8();
f32 fn_80072938();
void fn_800725BC(void* arg0, s32 arg1, f32 farg0);
void fn_8007325C(u8* p0);
void fn_8007326C(void* arg0);
void Anim_SetTime(u8* p0, f32 x0);
extern f32 lbl_80283A24;
extern f32 lbl_80283A28;
extern f32 lbl_80283A2C;
f32 fn_800732B8(f32 farg0, f32 farg1, f32 farg2, f32 farg3);
s32 fn_800734D0();
s32 fn_800734A0(void);

void fn_800725BC(void* arg0, s32 arg1, f32 farg0) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 4)) = 1;
        (*(f32*)((u8*)(arg0) + 0x18)) = farg0;
        (*(s32*)((u8*)(arg0) + 0x20)) = arg1;
        (*(f32*)((u8*)(arg0) + 0x10)) = fn_800728D8();
        (*(f32*)((u8*)(arg0) + 0x14)) = fn_80072938(arg0);
    }
}

void fn_8007325C(u8* p0) {
    *(s32*)(p0 + 0x4) = (*(s32*)(p0 + 0x4) | 2);
}

void fn_8007326C(void* arg0) {
    (*(s32*)((u8*)(arg0) + 4)) = (s32) ((*(s32*)((u8*)(arg0) + 4)) & 0xFFFFFFFC);
}

void Anim_SetTime(u8* p0, f32 x0) {
    f32 t0;
    t0 = fn_800732B8(x0, *(f32*)(p0 + 0x18), *(f32*)(p0 + 0x1C), *(f32*)(p0 + 0x20));
    *(f32*)(p0 + 0x18) = t0;
}

f32 fn_800732B8(f32 farg0, f32 farg1, f32 farg2, f32 farg3) {
    if (lbl_80283A24 == farg0) {
        return farg3;
    }
    if (lbl_80283A28 == farg0) {
        return farg1;
    }
    if (lbl_80283A2C == farg0) {
        return farg2;
    }
    return farg0;
}

s32 fn_800734A0(void) {
    s32 t0;
    t0 = fn_800734D0();
    return ((u32)((1 - t0) | (t0 - 1)) >> 31);
}

// ---- end of sweep code ----

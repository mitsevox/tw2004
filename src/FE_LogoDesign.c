// FE_LogoDesign.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80193D28[];
extern s32 lbl_802824B8;
extern s32 lbl_802824BC;
extern u8 lbl_802824C0;
void fn_80005AE8();
s32 fn_80009B34();
void fn_80009E70();
void fn_8010FAF4();
void fn_8010F6C8(void);
void fn_8010F718(void);
void fn_8010F748(void);
void fn_8010F794(void);
void fn_8010F7C0(s32 p0);
s32 fn_8010F7D8(void);
void fn_8010F7E4(s32 p0);
void fn_8010F880(void);
s32 fn_8010F978(s32 arg0, s32 arg1);
s32 fn_8010FB70();
void fn_8010F90C(s32 p0, s32 p1, s32 p2);

void fn_8010F6C8(void) {
    s32 t0;
    t0 = fn_80009B34(512, 0, 0, lbl_80193D28, 47);
    lbl_802824BC = t0;
    fn_80005AE8(t0, 0, 512);
    lbl_802824C0 = 0;
}

void fn_8010F718(void) {
    fn_80009E70(lbl_802824BC);
    lbl_802824BC = 0;
    lbl_802824C0 = 0;
}

void fn_8010F748(void) {
    s32 t0;
    t0 = fn_80009B34(12, 2, 0, lbl_80193D28, 67);
    lbl_802824B8 = t0;
    fn_80005AE8(t0, 0, 12);
    fn_8010FAF4();
}

void fn_8010F794(void) {
    fn_80009E70(lbl_802824B8);
    lbl_802824B8 = 0;
}

void fn_8010F7C0(s32 p0) {
    *(s32*)((u8*)lbl_802824B8) = p0;
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}

s32 fn_8010F7D8(void) {
    return *(s32*)(lbl_802824B8 + 0x0);
}

void fn_8010F7E4(s32 p0) {
    *(s32*)(((u8*)lbl_802824B8) + 0x4) = p0;
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}

void fn_8010F880(void) {
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}

void fn_8010F90C(s32 p0, s32 p1, s32 p2) {
    s32 t0;
    s32 t1;
    t0 = fn_8010FB70();
    t1 = fn_8010F978(p0, p1);
    *(u8*)(((u8*)t0) + t1) = p2;
    *(u8*)(((u8*)lbl_802824B8) + 0x8) = 1;
}

s32 fn_8010F978(s32 arg0, s32 arg1) {
    s32 temp_r0;

    temp_r0 = (*(s32*)((u8*)(lbl_802824B8) + 4));
    if (temp_r0 == 0) {
        if ((arg0 < 0) || (arg0 >= 0x40) || (arg1 < 0) || (arg1 >= 0x40)) {
            return -1;
        }
        return arg0 + (arg1 << 6);
    }
    if (temp_r0 == 1) {
        if ((arg0 < 0) || (arg0 >= 0x80) || (arg1 < 0) || (arg1 >= 0x20)) {
            return -1;
        }
        return arg0 + (arg1 << 7);
    }
    return -1;
}

// ---- end of sweep code ----

// GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void UStream_UnregisterHandler();
void fn_80012EF8();
void fn_80012F18();
void fn_80012F34();
void fn_80012F50();
void fn_80035098();
void fn_80030818(void);
void fn_8003084C(void);
s32 fn_800354BC(s32);
void* fn_800354C4();
s32 fn_800354D0(s32, s32);
s32 fn_800354E4(s32, s32);
s32 fn_800354F4(s32);
void fn_80031084(s32 arg0, s32 arg1, s32 arg2, void* arg3, f32 farg0);
s32 fn_8015929C(s32, s32, s32, s32 (*)(void*, void*));
s32 fn_800318AC(void* arg0, void* arg1);
extern s32 lbl_802810E4;
s32 fn_80031E40(void);
extern u8 lbl_801D3CB0[];
void fn_80030894();
void fn_8003272C();
void fn_80032F88();
void fn_80032AEC(void);
extern f32 lbl_802810C8;
extern s8 lbl_802810CC;
extern f32 lbl_80282F60;
void fn_800332F4(void);
f32 fn_800336E4(void);
f32 fn_800336F4(void);
extern u8 lbl_801B8980[];
double floor();
f32 fn_80035074(f32 x0);
extern s32 lbl_80280DF0;
void fn_80012EF0();
void fn_80013CCC();
void fn_80013D68();
void fn_80013D9C();
void fn_80035240(s32 p0);
void fn_8003526C(void);
void fn_80035294(void);
void fn_800352BC(void);
extern u8* lbl_80281380;
void fn_80035398(void);
void fn_8003541C();
void fn_80035440();
void fn_800352E4(void);
void fn_80035308(void);
s32 fn_8003532C(void);
void fn_80035338(s32 p0);
void fn_8006F334();
void fn_8003534C(void);
void fn_8006EDC0();
void fn_80035370(void);
s32 fn_800350B4(void*, f32, f32);
s32 fn_800350D0(void*, f32);
s32 fn_800350EC(s32, s32, s32);
extern void* lbl_802811E0;
extern f32 lbl_80282FB0;
extern f32 lbl_80282FB4;
extern f32 lbl_80282FB8;

void fn_80030818(void) {
    UStream_UnregisterHandler(1952805408);
    UStream_UnregisterHandler(1952932896);
}

void fn_8003084C(void) {
    fn_80035098(0);
    fn_80012F50(1, 6, 128);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012EF8();
}

void fn_80031084(s32 arg0, s32 arg1, s32 arg2, void* arg3, f32 farg0) {
    s32 temp_r31;

    (*(f32*)((u8*)(arg3) + 8)) = farg0;
    (*(f32*)((u8*)(arg3) + 0xC)) = (f32) (*(f32*)((u8*)(fn_800354C4()) + 0xC));
    (*(s32*)((u8*)(arg3) + 0x10)) = arg1;
    (*(s32*)((u8*)(arg3) + 0x14)) = arg2;
    temp_r31 = fn_800354F4(arg0);
    (*(s32*)((u8*)(arg3) + 0)) = fn_800354E4(arg0, 0);
    (*(s32*)((u8*)(arg3) + 0x18)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 3);
    (*(s32*)((u8*)(arg3) + 0x1C)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 2);
    (*(s32*)((u8*)(arg3) + 0x20)) = fn_800354D0(arg0, 1);
    if (temp_r31 >= 2) {
        (*(s32*)((u8*)(arg3) + 4)) = fn_800354BC((*(s32*)((u8*)(arg3) + 0)));
        return;
    }
    (*(s32*)((u8*)(arg3) + 4)) = 0;
}

s32 fn_800318AC(void* arg0, void* arg1) {
    f32 temp_f0;
    f32 temp_f1;

    temp_f0 = (*(f32*)((u8*)(arg0) + 0x10));
    temp_f1 = (*(f32*)((u8*)(arg1) + 0x10));
    if (temp_f0 > temp_f1) {
        return 1;
    }
    if (temp_f0 < temp_f1) {
        return -1;
    }
    return 0;
}

s32 fn_80031E40(void) {
    return ((u32)((-1 - lbl_802810E4) | (lbl_802810E4 + 1)) >> 31);
}

void fn_80032AEC(void) {
    fn_80030894();
    fn_80032F88(*(s32*)(lbl_801D3CB0 + 0x30), *(s32*)(lbl_801D3CB0 + 0x10E4), *(s32*)(lbl_801D3CB0 + 0x11B8), *(s32*)(lbl_801D3CB0 + 0x11BC));
    fn_8003272C(0);
    fn_80032F88(*(s32*)(lbl_801D3CB0 + 0x24), *(s32*)(lbl_801D3CB0 + 0x10DC), *(s32*)(lbl_801D3CB0 + 0x11B8), *(s32*)(lbl_801D3CB0 + 0x11BC));
    fn_8003272C(1);
    fn_80035098(0);
    fn_8003084C();
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}

void fn_800332F4(void) {
    lbl_802810CC = -1;
    lbl_802810C8 = lbl_80282F60;
}

f32 fn_800336E4(void) {
    return *(f32*)(lbl_801D3CB0 + 0x1178);
}

f32 fn_800336F4(void) {
    return *(f32*)(lbl_801D3CB0 + 0x1170);
}

f32 fn_80035074(f32 x0) {
    f32 t0;
    t0 = floor(x0);
    return t0;
}

void fn_80035240(s32 p0) {
    fn_80013D9C(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

void fn_8003526C(void) {
    fn_80012EF0(*(s32*)((u8*)lbl_80280DF0));
}

void fn_80035294(void) {
    fn_80013CCC(*(s32*)((u8*)lbl_80280DF0));
}

void fn_800352BC(void) {
    fn_80013D68(*(s32*)((u8*)lbl_80280DF0));
}

void fn_800352E4(void) {
    fn_8003541C();
    fn_80035398();
}

void fn_80035308(void) {
    fn_8003532C();
    fn_80035440();
}

s32 fn_8003532C(void) {
    return *(s32*)(lbl_80281380 + 0x230);
}

void fn_80035338(s32 p0) {
    *(s32*)(((u8*)lbl_80281380) + 0x230) = (s32)(lbl_80281380 + (p0 * 140));
}

void fn_8003534C(void) {
    fn_8003532C();
    fn_8006F334();
}

void fn_80035370(void) {
    s32 t0;
    t0 = fn_8003532C();
    fn_8006EDC0((t0 + 84));
}

void fn_80035398(void) {
    fn_800350EC((s32) (*(f32*)((u8*)(lbl_802811E0) + 0x44)), (s32) (*(f32*)((u8*)(lbl_802811E0) + 0x48)), (s32) (*(f32*)((u8*)(lbl_802811E0) + 0x4C)));
    fn_800350D0(lbl_802811E0, lbl_80282FB0 * (*(f32*)((u8*)(lbl_802811E0) + 0x50)));
    fn_800350B4(lbl_802811E0, lbl_80282FB4 * (lbl_80282FB8 + (*(f32*)((u8*)(lbl_802811E0) + 0x50))), lbl_80282FB4);
}

// ---- end of sweep code ----

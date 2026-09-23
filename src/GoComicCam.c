// GoComicCam.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_8018FF78[];
extern s32 lbl_80282178;
s32 fn_80009B34();
void fn_800B34F0(void);
void fn_80009E70();
void fn_800B352C(void);
void fn_800B3D64(void);
void fn_80012EF8();
void fn_80013CCC();
void fn_80013EEC();
void fn_8001614C();
void fn_80016B9C();
void fn_800171D8();
void fn_80035240();
void fn_800352BC();
void fn_800B3F4C(f32 x0, f32 x1);
u8 fn_800B4908(void);

void fn_800B34F0(void) {
    s32 t0;
    t0 = fn_80009B34(516, 2, 0, lbl_8018FF78, 91);
    lbl_80282178 = t0;
}

void fn_800B352C(void) {
    fn_80009E70(lbl_80282178);
}

void fn_800B3D64(void) {
}

void fn_800B3F4C(f32 x0, f32 x1) {
    fn_800171D8(x1, x0);
    fn_800352BC();
    fn_80035240(0);
    fn_8001614C();
    fn_80013CCC();
    fn_8001614C();
    fn_80013EEC();
    fn_80016B9C();
    fn_80012EF8();
}

u8 fn_800B4908(void) {
    return *(u8*)(lbl_80282178 + 0x14);
}

// ---- end of sweep code ----

// hwsMaterial_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80223C48[];
s32 fn_80117510();
void fn_80112254(s32 p0, u8* p1);
void fn_80112438(void);
f64 fn_80009680(f32);
void fn_80112548(f32* arg0, f32* arg1);
s32 fn_80056480();
void fn_800ED6E8();
s32 fn_800ED6F0();
void fn_80112580(s32 p0, u8* p1);
void fn_801125B8(u8* p0);
void fn_801125E0(s32 p0, u8* p1);

void fn_80112254(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80117510(*(u16*)lbl_80223C48);
    *(s32*)p1 = t0;
}

void fn_80112438(void) {
}

void fn_80112548(f32* arg0, f32* arg1) {
    *arg1 = (f32) fn_80009680(*arg0);
}

void fn_80112580(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80056480(6);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_801125B8(u8* p0) {
    fn_800ED6E8((*(s32*)p0 & 0xFF));
}

void fn_801125E0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ED6F0();
    *(s32*)p1 = (t0 & 0xFF);
}

// ---- end of sweep code ----

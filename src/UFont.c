// UFont.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80012EC4();
void fn_80012868(s32 p0);
extern u8* lbl_80280DE0;
void fn_80012898(s32 v);
s32 fn_800128A4(void);
void fn_80011C90();
void fn_80012B2C(f32 x0, f32 x1);
void fn_80012B6C(f32 x0);
void fn_80012B9C(f32 x0, f32 x1);
f32 fn_80012BDC(char* sz, int n);
void fn_80012C54(s32 p0);
void fn_80012C84(s32 p0);
void fn_80012CB4(f32 farg0, f32 farg1, f32 farg2, f32 farg3, s32 arg_sp0);
void fn_800111D8();
void fn_800112DC();
void fn_80012EF8();
void fn_80012F18();
void fn_80012F34();
void fn_80012F50();
void fn_80012D88(void);
void fn_80012DCC(void);
extern f32 lbl_80282B48;
void fn_80012E00(void* arg0);
void fn_80012E1C(u8* p, s32 v);
extern f32 lbl_80282B4C;
void fn_80012E24(void* arg0, void* arg1);

void fn_80012868(s32 p0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(s32*)(((u8*)t0) + 0xA0) = p0;
}

void fn_80012898(s32 v) {
    *(s32*)(lbl_80280DE0 + 0x1B8) = v;
}

s32 fn_800128A4(void) {
    return *(s32*)(lbl_80280DE0 + 0x1B8);
}

void fn_80012B2C(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0x84) = x0;
    *(f32*)(((u8*)t0) + 0x88) = x1;
}

void fn_80012B6C(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0x78) = x0;
}

void fn_80012B9C(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0x7C) = x0;
    *(f32*)(((u8*)t0) + 0x80) = x1;
}

f32 fn_80012C30(char* sz) {
    return fn_80012BDC(sz, 4);
}

void fn_80012C54(s32 p0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(u8*)(((u8*)t0) + 0xA8) = p0;
}

void fn_80012C84(s32 p0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(s32*)(((u8*)t0) + 0x9C) = p0;
}

void fn_80012CB4(f32 farg0, f32 farg1, f32 farg2, f32 farg3, s32 arg_sp0) {
    void* temp_r3;

    temp_r3 = (void*)fn_80012EC4();
    (*(f32*)((u8*)(temp_r3) + 0x8C)) = farg0;
    (*(f32*)((u8*)(temp_r3) + 0x90)) = farg1;
    (*(f32*)((u8*)(temp_r3) + 0x94)) = farg2;
    (*(f32*)((u8*)(temp_r3) + 0x98)) = farg3;
}

void fn_80012D88(void) {
    fn_800111D8();
    fn_80012F50(1, 6, 1);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012EF8();
}

void fn_80012DCC(void) {
    fn_800112DC();
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012EF8();
}

void fn_80012E00(void* arg0) {
    (*(f32*)((u8*)(arg0) + 0xC)) = (f32) (lbl_80282B48 / ((*(f32*)((u8*)(arg0) + 8)) - (*(f32*)((u8*)(arg0) + 4))));
}

void fn_80012E1C(u8* p, s32 v) {
    *(s32*)(p + 0x474) = v;
}

void fn_80012E24(void* arg0, void* arg1) {
    if (((s32) (*(s32*)((u8*)(arg1) + 0x10)) != 0) || (lbl_80282B4C != (*(f32*)((u8*)(arg1) + 0xB8)))) {
        (*(s32*)((u8*)(arg0) + 0x474)) = 1;
    }
}

// ---- end of sweep code ----

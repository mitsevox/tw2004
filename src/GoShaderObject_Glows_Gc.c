// GoShaderObject_Glows_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet
// decompiled; the sweep code below is the matched small functions.

#include "glows.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8009809C();
void fn_800981D0();
void fn_80098350(void);

void fn_80098350(void) {
    fn_8009809C(8, 3);
    fn_8009809C(9, 4);
    fn_8009809C(10, 5);
    fn_8009809C(11, 6);
    fn_8009809C(12, 8);
    fn_8009809C(0, 10);
    fn_8009809C(1, 16);
    fn_8009809C(2, 24);
    fn_8009809C(3, 32);
    fn_800981D0(4, 1);
    fn_800981D0(5, 2);
    fn_800981D0(6, 3);
    fn_800981D0(7, 4);
}

// ---- end of sweep code ----

void fn_80098740(void) {
    s32 i;
    lbl_801D99D0.nCount = 0;
    for (i = 0; i < NUM_GLOWS; i++) {
        lbl_801D99D0.a[i].p4 = NULL;
    }
    fn_80098350();
}

// Free the glows' data. The pointers are left as they were.
void fn_800987D4(void) {
    int i;
    for (i = 0; i < NUM_GLOWS; i++) {
        if (lbl_801D99D0.a[i].p4 != NULL) {
            fn_80009E70(lbl_801D99D0.a[i].p4);
            lbl_801D99D0.nCount--;
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80098844(void);
void fn_80098848(void);
void fn_800985FC();
void fn_8009884C(u8* p0);
void fn_80098884(u8* p0, u8* p1);
void fn_800988A0(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800988B8(f32 farg0, f32 farg1, f32 farg2);
void fn_800988CC(s32 p0);
extern u8 lbl_801D9A40[];
extern s32 lbl_80281F80;
void fn_80036054();
void fn_800360A0();
void fn_800988D8(void);
void fn_80098910(void);

void fn_80098844(void) {
}

void fn_80098848(void) {
}

void fn_8009884C(u8* p0) {
    fn_800985FC(*(s32*)(p0 + 0x4), *(s32*)(p0 + 0xC), ((u32)__cntlzw(*(s32*)(p0 + 0x8)) >> 5));
}

void fn_80098884(u8* p0, u8* p1) {
    *(s32*)(p0 + 0x4) = *(s32*)p1;
    *(s32*)(p0 + 0x8) = *(s32*)(p1 + 0x4);
    *(s32*)(p0 + 0xC) = *(s32*)(p1 + 0x8);
}

void fn_800988A0(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

void fn_800988B8(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_800988CC(s32 p0) {
    *(volatile u8*)0xCC008000 = p0;
}

void fn_800988D8(void) {
    fn_80036054(lbl_801D9A40, 11, 0);
    lbl_80281F80 = 0;
}

void fn_80098910(void) {
    fn_800360A0(lbl_801D9A40);
}

// ---- end of sweep code ----

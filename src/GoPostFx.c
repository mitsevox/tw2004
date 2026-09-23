// GoPostFx.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "gx.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80009E70();
extern void* lbl_80281D80;
void fn_80037F80(void);
void fn_80038010(u8 arg0, s32 arg1, s32 arg2);
void fn_80037FB4(u8 arg0, s32 arg1);
u8 fn_8003944C();
s32 fn_80038314();
s32 fn_800383A8();
s32 fn_80038438();
void fn_800382E0(void);
extern u8 lbl_801D50C0[];
void Vec_Copy();
void fn_800386F0(s32 p0, s32 p1);
void fn_8002A024();
void fn_80038A2C(s32 p0, u8* p1);
void fn_8002A164();
void fn_80038A6C(void);
void fn_800392D0(void);

void fn_80037F80(void) {
    if (lbl_80281D80 != NULL) {
        fn_80009E70(lbl_80281D80);
        lbl_80281D80 = NULL;
    }
}

void fn_80037FB4(u8 arg0, s32 arg1) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_80038010(arg0, var_r31, arg1);
        var_r31 += 1;
    } while (var_r31 < 4);
}

void fn_800382E0(void) {
    if (fn_8003944C() == 0) {
        fn_80038314();
        fn_800383A8();
        fn_80038438();
    }
}

void fn_800386F0(s32 p0, s32 p1) {
    Vec_Copy(((lbl_801D50C0 + (p0 * 20)) + 0x4), p1, (p0 * 20));
}

void fn_80038A2C(s32 p0, u8* p1) {
    fn_8002A024(1, *(s32*)(p1 + 0x14), *(f32*)((u8*)*(s32*)(p1 + 0x14)), *(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0x4), (*(f32*)((u8*)*(s32*)(p1 + 0x14)) + *(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0x8)), (*(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0x4) + *(f32*)(((u8*)*(s32*)(p1 + 0x14)) + 0xC)));
}

void fn_80038A6C(void) {
    fn_8002A164(1);
}

void fn_800392D0(void) {
    if (lbl_80281D80 != NULL) {
        GXSetZMode(0, 3, 0);
        GXSetTexCopySrc(0, 0, 0x200, 0x1C0);
        GXSetTexCopyDst(0x100, 0xE0, 6, 1);
        GXCopyTex(lbl_80281D80, 0);
        GXPixModeSync();
        GXInvalidateTexAll();
    }
}

// ---- end of sweep code ----

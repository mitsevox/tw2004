// GoStaticCam.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801885F8[];
extern s32 lbl_80281E18;
s32 fn_80009B34();
void fn_80009E70();
void fn_80064EA4(void);
void fn_80064E2C(void);
void fn_80064E74(void);
void Vec3Copy();
void fn_80064F54(u8* p0, s32 p1, s32 p2);

// ---- sweep code (not yet cleaned up) ----

void UStream_UnregisterHandler();
void fn_800644CC(void);

void fn_800644CC(void) {
    UStream_UnregisterHandler(1128353091);
}

// ---- end of sweep code ----

void fn_80064E2C(void) {
    s32 t0;
    t0 = fn_80009B34(7784, 2, 0, lbl_801885F8, 380);
    lbl_80281E18 = t0;
    *(s32*)(((u8*)t0) + 0x1E64) = 0;
    fn_80064EA4();
}

void fn_80064E74(void) {
    fn_80064EA4();
    fn_80009E70(lbl_80281E18);
    lbl_80281E18 = 0;
}

void fn_80064EA4(void) {
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E04)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E00)) = 0;
    (*(s8*)((u8*)(lbl_80281E18) + 0x1E08)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E34)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E38)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E3C)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E40)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E44)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E48)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E4C)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E50)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E54)) = 0;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E58)) = 0;
    if ((u32) (*(u32*)((u8*)(lbl_80281E18) + 0x1E64)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281E18) + 0x1E64)));
    }
    (*(u32*)((u8*)(lbl_80281E18) + 0x1E64)) = 0U;
    (*(s32*)((u8*)(lbl_80281E18) + 0x1E60)) = 0;
}

void fn_80064F54(u8* p0, s32 p1, s32 p2) {
    Vec3Copy((p0 + 0x20), p2);
}

// ---- end of sweep code ----

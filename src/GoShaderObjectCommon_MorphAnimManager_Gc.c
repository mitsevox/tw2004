// GoShaderObjectCommon_MorphAnimManager_Gc.c (EA's name, from its asserts): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80189D30[];
extern s32 lbl_80281F70;
void fn_80005AE8();
s32 fn_80009B34();
void fn_80009E70();
void fn_80097208(void);
void fn_80097250(void);

void fn_80097208(void) {
    s32 t0;
    t0 = fn_80009B34(2408, 2, 32, lbl_80189D30, 85);
    lbl_80281F70 = t0;
    fn_80005AE8(t0, 0, 2408);
}

void fn_80097250(void) {
    fn_80009E70(lbl_80281F70);
    lbl_80281F70 = 0;
}

// ---- end of sweep code ----

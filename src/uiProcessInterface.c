// uiProcessInterface.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"
#include "game/frontend.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801D87C0[];
void fn_8008F80C(s32 p0, s32 p1);
s32 fn_80092BC4();
s32 fn_800934F8();
s32 fn_800BA038();
s32 fn_80090400();
void fn_80090664(void);

void fn_8008F80C(s32 p0, s32 p1) {
    *(u8*)((lbl_801D87C0 + p0) + 0x30) = p1;
}

void fn_80090664(void) {
    if (lbl_80281F1C != NULL) {
        fn_80090400(lbl_80281F1C);
    }
    fn_800934F8();
    fn_80092BC4();
    fn_800BA038();
}

// ---- end of sweep code ----

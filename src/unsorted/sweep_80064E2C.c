// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801885F8[];
extern s32 lbl_80281E18;
s32 fn_80009B34();
void fn_80009E70();
void fn_80064EA4();

void fn_80064E2C(void);
void fn_80064E74(void);
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

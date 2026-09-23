// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801894A0[];
extern s32 lbl_80281EC0;
void GXSetArray();
void fn_80005AE8();
s32 fn_80009B34();
void fn_80009E70();

void fn_80076B7C(void);
void fn_80076BC4(void);
void fn_80076BF0(void);
void fn_80076B7C(void) {
    s32 t0;
    t0 = fn_80009B34(1552, 2, 32, lbl_801894A0, 100);
    lbl_80281EC0 = t0;
    fn_80005AE8(t0, 0, 1552);
}

void fn_80076BC4(void) {
    fn_80009E70(lbl_80281EC0);
    lbl_80281EC0 = 0;
}

void fn_80076BF0(void) {
    GXSetArray(23, (lbl_80281EC0 + 512), 32);
}

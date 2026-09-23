// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801888C0[];
extern s32 lbl_80281E48;
s32 fn_80009B34();
void fn_80009E70();

s32 fn_8006BED4(void);
void fn_8006BF20(void);
s32 fn_8006BED4(void) {
    s32 t0;
    t0 = fn_80009B34(86624, 2, 0, lbl_801888C0, 77);
    lbl_80281E48 = t0;
    *(u8*)(((u8*)(t0 + 0x10000)) + 0x525C) = 0;
    return (t0 + 0x10000);
}

void fn_8006BF20(void) {
    fn_80009E70(lbl_80281E48);
    lbl_80281E48 = 0;
}

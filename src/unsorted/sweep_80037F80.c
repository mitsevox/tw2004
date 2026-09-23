// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern u32 lbl_80281D80;

void fn_80037F80(void);
void fn_80037F80(void) {
    if ((u32) lbl_80281D80 != 0U) {
        fn_80009E70(lbl_80281D80);
        lbl_80281D80 = 0U;
    }
}
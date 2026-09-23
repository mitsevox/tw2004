// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern u32 lbl_80281EC8;

void fn_80076EEC(void);
void fn_80076EEC(void) {
    if ((u32) lbl_80281EC8 != 0U) {
        fn_80009E70(lbl_80281EC8);
        lbl_80281EC8 = 0U;
    }
}
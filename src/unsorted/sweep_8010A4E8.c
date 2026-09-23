// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern u32* lbl_80282488;

void fn_8010A4E8(void);
void fn_8010A4E8(void) {
    if ((u32) *lbl_80282488 != 0U) {
        fn_80009E70(*lbl_80282488);
    }
    fn_80009E70(lbl_80282488);
}
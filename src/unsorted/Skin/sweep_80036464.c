// Small functions found by the sweep (sweep.py). Original file: Skin.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_80009E70();
extern u32 lbl_80281D70;
extern s32 lbl_80281D74;
extern s32 lbl_80281D78;

void fn_80036464(void);
void fn_80036464(void) {
    if ((u32) lbl_80281D70 != 0U) {
        fn_80009E70(lbl_80281D70);
    }
    lbl_80281D70 = 0U;
    lbl_80281D78 = 0;
    lbl_80281D74 = 0;
}
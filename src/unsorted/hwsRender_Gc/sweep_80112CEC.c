// Small functions found by the sweep (sweep.py). Original file: hwsRender_Gc.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_80009E70();
extern u32 lbl_802824E0;

void fn_80112CEC(void);
void fn_80112CEC(void) {
    if ((u32) lbl_802824E0 != 0U) {
        fn_80009E70(lbl_802824E0);
        lbl_802824E0 = 0U;
    }
}
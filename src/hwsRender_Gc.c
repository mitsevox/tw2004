// hwsRender_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80009E70();
extern u32 lbl_802824E0;
void fn_80112CEC(void);
s32 fn_80036464();
s32 fn_80037F80();
s32 fn_8011EBF8();
u8 fn_80112B80();
void fn_80112DA0(void);

void fn_80112CEC(void) {
    if ((u32) lbl_802824E0 != 0U) {
        fn_80009E70(lbl_802824E0);
        lbl_802824E0 = 0U;
    }
}

void fn_80112DA0(void) {
    fn_80112CEC();
    if (fn_80112B80() != 0) {
        fn_8011EBF8();
    }
    fn_80037F80();
    fn_80036464();
}

// ---- end of sweep code ----

// DynChain.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8011407C(void);
extern s32 lbl_802824F8;
void fn_80009E70();
void fn_80114080(void);
void fn_80114398(u8* p0);

void fn_8011407C(void) {
}

void fn_80114080(void) {
    fn_80009E70(lbl_802824F8);
    lbl_802824F8 = 0;
}

void fn_80114398(u8* p0) {
    fn_80009E70(*(s32*)(p0 + 0x8));
    fn_80009E70(p0);
}

// ---- end of sweep code ----

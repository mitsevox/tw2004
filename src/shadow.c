// shadow.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_802814A8;
void fn_800B2724(void);

void fn_800B2724(void) {
    *(s32*)(((u8*)lbl_802814A8) + 0x4F0) = 0;
}

// ---- end of sweep code ----

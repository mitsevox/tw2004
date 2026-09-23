// GoLighting.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80009E70();
extern void* lbl_802811D8;
void fn_8006E424(void);

void fn_8006E424(void) {
    if ((u32) (*(u32*)((u8*)(lbl_802811D8) + 0xC)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_802811D8) + 0xC)));
        (*(u32*)((u8*)(lbl_802811D8) + 0xC)) = 0U;
    }
}

// ---- end of sweep code ----

// TibExt.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80194748[];
void fn_80009B34();
s32 fn_8000A0B4();
void TibExtMemAlloc(s32 p0, s32 p1, s32 p2);

void TibExtMemAlloc(s32 p0, s32 p1, s32 p2) {
    s32 t0;
    t0 = fn_8000A0B4();
    fn_80009B34(p1, t0, p2, lbl_80194748, 42);
}

// ---- end of sweep code ----

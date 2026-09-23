// TibExt.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_80077B08();
u8 fn_80102204();
u8 fn_80102228();
s32 fn_80121F0C(s32 arg0);
extern u8 lbl_80194748[];
void fn_80009B34();
s32 fn_8000A0B4();
void TibExtMemAlloc(s32 p0, s32 p1, s32 p2);

s32 fn_80121F0C(s32 arg0) {
    s32 temp_r30;
    u8 var_r31;

    temp_r30 = fn_80077B08();
    var_r31 = 0;
    if ((fn_80102204(temp_r30, arg0) == 0) && (fn_80102228(temp_r30, arg0) != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}

void TibExtMemAlloc(s32 p0, s32 p1, s32 p2) {
    s32 t0;
    t0 = fn_8000A0B4();
    fn_80009B34(p1, t0, p2, lbl_80194748, 42);
}

// ---- end of sweep code ----

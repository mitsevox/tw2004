// hwsOverride_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80112848();
void fn_801128C8(s32 p0, s32 p1);
void fn_801128EC(s32 p0, s32 p1);
s32 fn_80009E70();
void fn_80112910(u32 arg0);
s32 fn_80112964(u8* p);

void fn_801128C8(s32 p0, s32 p1) {
    fn_80112848(p0, p1, 2);
}

void fn_801128EC(s32 p0, s32 p1) {
    fn_80112848(p0, p1, 1);
}

void fn_80112910(u32 arg0) {
    if (arg0 != 0U) {
        fn_80009E70();
    }
}

s32 fn_80112964(u8* p) {
    return *(s32*)(p + 0x30);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8011296C();
void fn_80112A10(s32 p0, s32 p1);
void fn_80112A34(s32 p0, s32 p1);
void fn_80112A58(u32 arg0);

void fn_80112A10(s32 p0, s32 p1) {
    fn_8011296C(p0, p1, 2);
}

void fn_80112A34(s32 p0, s32 p1) {
    fn_8011296C(p0, p1, 1);
}

void fn_80112A58(u32 arg0) {
    if (arg0 != 0U) {
        fn_80009E70();
    }
}

// ---- end of sweep code ----

// SkinPart.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the matched
// small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_800CD7CC(s32 p0, s32 p1);
s32 fn_8011389C();
void fn_800CE128(void* arg0);
void fn_800CE164(void);
void fn_800CE168(void);
void fn_800CE16C(void);

s32 fn_800CD7CC(s32 p0, s32 p1) {
    return p1;
}

void fn_800CE128(void* arg0) {
    if (((u32) (*(u32*)((u8*)((*(void**)((u8*)(arg0) + 0))) + 0x48)) != 0U) && ((*(s32*)((u8*)(arg0) + 0x10D4)) & 2)) {
        fn_8011389C();
    }
}

void fn_800CE164(void) {
}

void fn_800CE168(void) {
}

void fn_800CE16C(void) {
}

// ---- end of sweep code ----

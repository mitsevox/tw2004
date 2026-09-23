// hwsMaterial_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_801126F0();
void fn_801127A0(s32 p0);
s32 fn_80009E70();
void fn_801127C4(void* arg0);

void fn_801127A0(s32 p0) {
    fn_801126F0(p0, 0);
}

void fn_801127C4(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x18)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x18)));
        (*(u32*)((u8*)(arg0) + 0x18)) = 0U;
    }
}

// ---- end of sweep code ----

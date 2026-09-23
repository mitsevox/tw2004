// LLDisp_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801A2350[];
u8* fn_80006DD0(void);
u8* fn_80006EC8(void);
extern u8 lbl_80281B8C;
s32 fn_8000724C(void);
void fn_80007254(void);
u8 fn_80007258(void);
void fn_80007260(void);

u8* fn_80006DD0(void) {
    *(u8*)(lbl_801A2350 + 0xF) = 0;
    *(u8*)(lbl_801A2350 + 0xE) = 0;
    *(u8*)(lbl_801A2350 + 0xC) = 0;
    *(u8*)(lbl_801A2350 + 0xD) = 0;
    *(u8*)(lbl_801A2350 + 0x13) = 0;
    return lbl_801A2350;
}

u8* fn_80006EC8(void) {
    *(u8*)(lbl_801A2350 + 0x13) = 1;
    return lbl_801A2350;
}

s32 fn_8000724C(void) {
    return 0;
}

void fn_80007254(void) {
}

u8 fn_80007258(void) {
    return lbl_80281B8C;
}

void fn_80007260(void) {
}

// ---- end of sweep code ----

// GoFrameBuf.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8006E1C0(void);
void fn_8006E1C4(void);
void fn_80009E70();
void fn_8006E214(void);

void fn_8006E1C0(void) {
}

void fn_8006E1C4(void) {
}

void fn_8006E214(void) {
    fn_80009E70();
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8006E150();
void fn_8006E26C(u8* p0, f32 x0, f32 x1, f32 x2, f32 x3, f32 x4, f32 x5);

void fn_8006E26C(u8* p0, f32 x0, f32 x1, f32 x2, f32 x3, f32 x4, f32 x5) {
    *(f32*)p0 = x0;
    *(f32*)(p0 + 0x4) = x1;
    *(f32*)(p0 + 0x8) = x2;
    *(f32*)(p0 + 0xC) = x3;
    *(f32*)(p0 + 0x10) = x4;
    *(f32*)(p0 + 0x14) = x5;
    fn_8006E150();
}

// ---- end of sweep code ----

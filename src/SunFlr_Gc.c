// SunFlr_Gc.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8009A340(void);
extern u8 lbl_80189E68[];
s32 fn_80009B34();
void memset();
void fn_8009A344(s32 p0, u8* p1);
void fn_80009E70();
void fn_8009A3D0(s32 p0, u8* p1);
void fn_8009A704(void);

void fn_8009A340(void) {
}

void fn_8009A344(s32 p0, u8* p1) {
    s32 t1;
    memset((p1 + 0x10), 0, 64);
    *(s32*)(p1 + 0xC) = 576;
    t1 = fn_80009B34(2304, 2, 32, lbl_80189E68, 110);
    *(s32*)p1 = t1;
    *(s32*)(p1 + 0x2C) = *(s32*)p1;
    *(s32*)(p1 + 0x4C) = (*(s32*)p1 + 576);
    *(s32*)(p1 + 0x6C) = (*(s32*)p1 + 1152);
    *(s32*)(p1 + 0x8C) = (*(s32*)p1 + 1728);
}

void fn_8009A3D0(s32 p0, u8* p1) {
    fn_80009E70(*(s32*)(p1 + 0x0));
}

void fn_8009A704(void) {
}

// ---- end of sweep code ----

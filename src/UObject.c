// UObject.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80048804(void);
void fn_80009E70();
void fn_80048860(s32 p0);
void fn_800488B4();
void fn_80048894(void);

void fn_80048804(void) {
}

void fn_80048860(s32 p0) {
    fn_80048804();
    fn_80009E70(p0);
}

void fn_80048894(void) {
    fn_800488B4();
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

s32 fn_80048AC4(u8* p0, s32 p1);

s32 fn_80048AC4(u8* p0, s32 p1) {
    return *(s32*)(((u8*)*(s32*)(p0 + 0x8)) + (p1 << 2));
}

// ---- end of sweep code ----

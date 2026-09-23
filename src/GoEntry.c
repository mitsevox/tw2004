// GoEntry.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled.
#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8* gpGame;
extern u8* lbl_802811E8;
u8 fn_80008820(void);
void fn_8000882C(void);
s32 Game_GetCourse(void);

u8 fn_80008820(void) {
    return *(u8*)(lbl_802811E8 + 0x1);
}

void fn_8000882C(void) {
}

s32 Game_GetCourse(void) {
    return *(s32*)(gpGame + 0x14);
}

// ---- end of sweep code ----

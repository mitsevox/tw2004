// PsBallFx.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80281408;
void fn_80009E70();
void fn_800360A0();
void fn_800A2E14(void);

void fn_800A2E14(void) {
    fn_800360A0(lbl_80281408);
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x28));
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x2C));
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x30));
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x50));
}

// ---- end of sweep code ----

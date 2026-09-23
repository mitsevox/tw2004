// Small functions found by the sweep (sweep.py). Original file: GoPostFx.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

s32 GXCopyTex(u32, s32);
s32 GXInvalidateTexAll();
s32 GXPixModeSync();
s32 GXSetTexCopyDst(s32, s32, s32, s32);
s32 GXSetTexCopySrc(s32, s32, s32, s32);
s32 GXSetZMode(s32, s32, s32);
extern u32 lbl_80281D80;

void fn_800392D0(void);
void fn_800392D0(void) {
    if ((u32) lbl_80281D80 != 0U) {
        GXSetZMode(0, 3, 0);
        GXSetTexCopySrc(0, 0, 0x200, 0x1C0);
        GXSetTexCopyDst(0x100, 0xE0, 6, 1);
        GXCopyTex(lbl_80281D80, 0);
        GXPixModeSync();
        GXInvalidateTexAll();
    }
}
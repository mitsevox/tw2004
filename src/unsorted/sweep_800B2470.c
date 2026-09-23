// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 GXCopyTex(s32, s32);
s32 GXPixModeSync();
s32 GXSetTexCopyDst(s32, s32, s32, s32);
s32 GXSetTexCopySrc(s32, s32, s32, s32);
extern void* lbl_802814A8;

void fn_800B2470(void);
void fn_800B2470(void) {
    void* temp_r31;

    temp_r31 = lbl_802814A8;
    GXSetTexCopySrc(0, 0, 0x100, 0x100);
    GXSetTexCopyDst(0x100, 0x100, 0x27, 0);
    GXCopyTex((*(s32*)((u8*)(temp_r31) + 0x400)), 0);
    GXPixModeSync();
}
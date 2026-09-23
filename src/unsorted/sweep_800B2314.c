// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 GXGetTexBufferSize(s32, s32, s32, s32, s32);
s32 fn_8002A624();
extern void* lbl_802814A8;

void fn_800B2314(void);
void fn_800B2314(void) {
    void* temp_r31;

    temp_r31 = lbl_802814A8;
    GXGetTexBufferSize(0x100, 0x100, 9, 0, 0);
    (*(s32*)((u8*)(temp_r31) + 0x400)) = fn_8002A624();
}
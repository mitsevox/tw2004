// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpGame;

s32 fn_800E27A8(void);
s32 fn_800E27A8(void) {
    return ((u32)((-*(s32*)(((u8*)gpGame) + 0x294)) | *(s32*)(((u8*)gpGame) + 0x294)) >> 31);
}

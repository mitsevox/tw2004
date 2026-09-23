// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;
extern u8 lbl_80205F3C[];

s32 fn_800EFDFC(s32 p0);
s32 fn_800EFDFC(s32 p0) {
    return (*(s32*)(lbl_80205F3C + 0x704C) + *(s32*)(lbl_80205F3C + (p0 * 100)));
}

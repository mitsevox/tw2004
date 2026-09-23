// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;
extern u8 lbl_80194074[];

s32 fn_80119028(s32 p0);
s32 fn_80119028(s32 p0) {
    return *(s32*)(lbl_80194074 + (p0 << 2));
}

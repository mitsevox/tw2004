// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80211FB8[];

s32 fn_800F3654(s32 p0);
s32 fn_800F3654(s32 p0) {
    return *(s32*)(lbl_80211FB8 + (p0 << 3));
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802124B8[];

s32 fn_80102A44(s32 p0);
s32 fn_80102A44(s32 p0) {
    return *(s32*)(lbl_802124B8 + (p0 * 68));
}

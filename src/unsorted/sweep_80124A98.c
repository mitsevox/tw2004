// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80195340[];

s32 fn_80124A98(s32 p0);
s32 fn_80124A98(s32 p0) {
    return *(s32*)(lbl_80195340 + (p0 << 2));
}

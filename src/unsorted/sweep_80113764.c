// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80223BB0[];

s32 fn_80113764(void);
s32 fn_80113764(void) {
    return *(s32*)(lbl_80223BB0 + 0x58);
}

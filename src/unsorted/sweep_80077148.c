// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D7148[];

s32 fn_80077148(void);
s32 fn_80077148(void) {
    return (((u32)__cntlzw((*(s32*)(lbl_801D7148 + 0x20) - *(s32*)(lbl_801D7148 + 0x24))) >> 5) & 0xFF);
}

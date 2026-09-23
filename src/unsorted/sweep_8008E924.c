// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281EE0;

s32 fn_8008E924(void);
s32 fn_8008E924(void) {
    return (((u32)__cntlzw(*(s32*)(((u8*)lbl_80281EE0) + 0x74)) >> 5) & 0xFF);
}

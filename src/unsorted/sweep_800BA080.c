// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801F8458[];

s32 fn_800BA080(void);
s32 fn_800BA080(void) {
    return ((u32)((-1 - *(s32*)(lbl_801F8458 + 0x10)) | (*(s32*)(lbl_801F8458 + 0x10) + 1)) >> 31);
}

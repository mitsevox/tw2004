// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281908;

void fn_801202F8(s32 v);
void fn_801202F8(s32 v) {
    *(s32*)(lbl_80281908 + 0x36C) = v;
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281970;

void fn_80122488(s32 v);
void fn_80122488(s32 v) {
    *(s32*)(lbl_80281970 + 0x48) = v;
}

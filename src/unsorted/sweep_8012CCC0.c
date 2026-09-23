// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_802825B0;

s32 fn_8012CCC0(void);
void fn_8012CCCC(s32 v);
s32 fn_8012CCC0(void) {
    return *(s32*)(lbl_802825B0 + 0x17C);
}

void fn_8012CCCC(s32 v) {
    *(s32*)(lbl_802825B0 + 0x120) = v;
}

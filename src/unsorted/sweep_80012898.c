// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80280DE0;

void fn_80012898(s32 v);
s32 fn_800128A4(void);
void fn_80012898(s32 v) {
    *(s32*)(lbl_80280DE0 + 0x1B8) = v;
}

s32 fn_800128A4(void) {
    return *(s32*)(lbl_80280DE0 + 0x1B8);
}

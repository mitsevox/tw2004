// Small functions found by the sweep (sweep.py). Original file: UFont.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

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

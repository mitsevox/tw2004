// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802822FD;

void fn_800ED6E8(u8 v);
u8 fn_800ED6F0(void);
void fn_800ED6E8(u8 v) {
    lbl_802822FD = v;
}

u8 fn_800ED6F0(void) {
    return lbl_802822FD;
}

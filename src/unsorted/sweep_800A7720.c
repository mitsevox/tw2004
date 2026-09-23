// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80281419;
extern u8 lbl_8028141A;
u8 fn_800AD618(u8, s32);

u8 fn_800A7720(void);
void fn_800A7748(void);
u8 fn_800A7720(void) {
    return fn_800AD618(lbl_80281419, 0);
}

void fn_800A7748(void) {
    fn_800AD618(lbl_8028141A, 0);
}

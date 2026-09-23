// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];
extern u8 lbl_8028227C;

void fn_800E25CC(s32 p0);
void fn_800E25CC(s32 p0) {
    lbl_8028227C = p0;
    *(u8*)(gSession + 0x10) = p0;
}

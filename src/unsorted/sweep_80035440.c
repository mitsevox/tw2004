// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802811E0;
void Mem_cpy();

void fn_80035440(s32 p0);
void fn_80035440(s32 p0) {
    Mem_cpy(lbl_802811E0, p0, 84);
}

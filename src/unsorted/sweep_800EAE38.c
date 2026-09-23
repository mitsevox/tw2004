// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802822F4;
extern s32 lbl_802822F8;
s32 fn_800EAC94();

void fn_800EAE38(s32 p0);
void fn_800EAE44(void);
void fn_800EAE38(s32 p0) {
    lbl_802822F4 = p0;
    lbl_802822F8 = p0;
}

void fn_800EAE44(void) {
    s32 t0;
    t0 = fn_800EAC94();
    lbl_802822F4 = t0;
    lbl_802822F8 = t0;
}

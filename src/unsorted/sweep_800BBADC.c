// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802811B8;
void fn_80067B1C();

void fn_800BBADC(s32 p0);
void fn_800BBADC(s32 p0) {
    fn_80067B1C(lbl_802811B8, 5, (p0 & 0xFFFF), (lbl_802811B8 + 192));
}

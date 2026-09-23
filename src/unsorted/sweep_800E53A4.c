// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802822BC;
extern u8 lbl_802822DA;

void fn_800E53A4(u8 v);
void fn_800E53AC(void);
u8 fn_800E53B8(void);
void fn_800E53A4(u8 v) {
    lbl_802822BC = v;
}

void fn_800E53AC(void) {
    lbl_802822DA = 1;
}

u8 fn_800E53B8(void) {
    return lbl_802822DA;
}

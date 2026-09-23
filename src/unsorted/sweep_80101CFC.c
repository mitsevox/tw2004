// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802823E3;
extern u8 lbl_802823E4;
void fn_800A6EC8();

void fn_80101CFC(void);
void fn_80101D24(void);
void fn_80101CFC(void) {
    lbl_802823E4 = 1;
    fn_800A6EC8();
}

void fn_80101D24(void) {
    lbl_802823E3 = 1;
    fn_800A6EC8();
}

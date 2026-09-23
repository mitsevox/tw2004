// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D9A40[];
extern s32 lbl_80281F80;
void fn_80036054();
void fn_800360A0();

void fn_800988D8(void);
void fn_80098910(void);
void fn_800988D8(void) {
    fn_80036054(lbl_801D9A40, 11, 0);
    lbl_80281F80 = 0;
}

void fn_80098910(void) {
    fn_800360A0(lbl_801D9A40);
}

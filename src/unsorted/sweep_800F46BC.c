// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8028237C;
extern u8 lbl_80282384;
extern s32 lbl_80282388;
extern s32 lbl_8028238C;
void AI_DefaultTarget();
void fn_800F1EE4();

void fn_800F46BC(void);
void fn_800F46E4(void);
void fn_800F46BC(void) {
    fn_800F46E4();
    AI_DefaultTarget(0);
}

void fn_800F46E4(void) {
    fn_800F1EE4();
    lbl_8028238C = 5;
    lbl_80282388 = 5;
    lbl_80282384 = 0;
    lbl_8028237C = 0;
}

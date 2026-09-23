// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DF0;
void fn_800137D0();
void fn_80013808();

void fn_8006DC4C(s32 p0);
void fn_8006DC78(void);
void fn_8006DC4C(s32 p0) {
    fn_80013808(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

void fn_8006DC78(void) {
    fn_800137D0(*(s32*)((u8*)lbl_80280DF0));
}

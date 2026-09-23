// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DF0;
void fn_80012EF0();
void fn_80013CCC();
void fn_80013D68();
void fn_80013D9C();

void fn_80035240(s32 p0);
void fn_8003526C(void);
void fn_80035294(void);
void fn_800352BC(void);
void fn_80035240(s32 p0) {
    fn_80013D9C(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

void fn_8003526C(void) {
    fn_80012EF0(*(s32*)((u8*)lbl_80280DF0));
}

void fn_80035294(void) {
    fn_80013CCC(*(s32*)((u8*)lbl_80280DF0));
}

void fn_800352BC(void) {
    fn_80013D68(*(s32*)((u8*)lbl_80280DF0));
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80100308();
extern u8 lbl_802823E2;
extern s32 lbl_80282424;
extern s32 lbl_80282428;

void fn_80100308(void);
void fn_80100308(void) {
    if ((u8) lbl_802823E2 != 0) {
        lbl_802823E2 = 0;
        lbl_80282428 = lbl_80282424;
    }
}
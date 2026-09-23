// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_80100294();
void fn_80100308();
extern u8 lbl_802823F0;
extern s32 lbl_80282424;
extern s32 lbl_80282428;

void fn_801002C0(void);
void fn_801002C0(void) {
    if (fn_80100294() != 0) {
        if ((u8) lbl_802823F0 != 0) {
            lbl_802823F0 = 0;
            lbl_80282428 = lbl_80282424;
        }
        fn_80100308();
    }
}
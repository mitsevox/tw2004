// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_8028254C;
extern s32 lbl_80282550;
extern s32 lbl_80282554;
extern s32 lbl_80282558;

void fn_80124238(s32 arg0, s32 arg1);
void fn_80124238(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        lbl_80282558 = arg1;
        return;
    case 1:
        lbl_80282554 = arg1;
        return;
    case 2:
        lbl_80282550 = arg1;
        return;
    case 3:
        lbl_8028254C = arg1;
        return;
    }
}
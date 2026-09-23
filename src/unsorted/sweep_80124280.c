// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_8028254C;
extern s32 lbl_80282550;
extern s32 lbl_80282554;
extern s32 lbl_80282558;

s32 fn_80124280(s32 arg0);
s32 fn_80124280(s32 arg0) {
    switch (arg0) {
    case 0:
        return lbl_80282558;
    case 1:
        return lbl_80282554;
    case 2:
        return lbl_80282550;
    case 3:
        return lbl_8028254C;
    default:
        return 0;
    }
}
// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800ADA28(u8, s32, u8, s32);
extern u8 lbl_8028141B;

void fn_800A73F0(s32 arg0);
void fn_800A73F0(s32 arg0) {
    switch (arg0) {
    case 1:
        fn_800ADA28(lbl_8028141B, 1, 0U, 0);
        return;
    case 6:
        fn_800ADA28(lbl_8028141B, 1, 1U, 0);
        return;
    default:
        fn_800ADA28(lbl_8028141B, 0, (u8) arg0, 0);
        return;
    }
}
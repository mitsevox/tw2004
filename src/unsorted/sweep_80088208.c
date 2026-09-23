// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800E39F0();
s32 fn_800F1DF0();
s32 fn_800FDADC();

void fn_80088208(void);
void fn_80088208(void) {
    if (fn_800E39F0() != 0) {
        fn_800F1DF0();
        return;
    }
    fn_800FDADC();
}
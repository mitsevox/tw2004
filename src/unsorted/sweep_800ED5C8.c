// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802822FE;
extern s32 (*lbl_8028231C)();

s32 fn_800ED5C8(void);
s32 fn_800ED5C8(void) {
    if ((u8) lbl_802822FE != 0) {
        return 1;
    }
    return lbl_8028231C();
}
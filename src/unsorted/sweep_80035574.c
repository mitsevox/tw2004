// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32* lbl_802811F0;

s32 fn_80035574(void);
s32 fn_80035574(void) {
    return *lbl_802811F0 & 2;
}
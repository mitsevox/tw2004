// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281764;

u32 fn_8010D33C(s32* arg0);
u32 fn_8010D33C(s32* arg0) {
    if (arg0 != NULL) {
        *arg0 = lbl_80281764;
    }
    return (u32) ((5 - lbl_80281764) | (lbl_80281764 - 5)) >> 0x1FU;
}
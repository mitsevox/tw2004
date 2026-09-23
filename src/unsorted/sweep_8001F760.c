// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u32 lbl_80281CB4;

u32 fn_8001F760(s32 arg0);
u32 fn_8001F760(s32 arg0) {
    if ((arg0 < 0) || (arg0 >= 2)) {
        return 0U;
    }
    return lbl_80281CB4;
}
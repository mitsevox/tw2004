// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_801048B0(u32 arg0);
s32 fn_801048B0(u32 arg0) {
    if ((arg0 <= 2U) || ((u32) (arg0 - 7) <= 1U) || ((s32) arg0 == 0xC) || ((s32) arg0 == 0x13) || ((s32) arg0 == 0x14)) {
        return 1;
    }
    return 0;
}
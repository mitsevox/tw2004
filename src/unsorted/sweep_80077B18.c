// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80077B18(u32 arg0);
s32 fn_80077B18(u32 arg0) {
    if ((arg0 <= 1U) || ((u32) (arg0 - 3) <= 2U) || ((s32) arg0 == 7) || ((u32) (arg0 - 0xA) <= 1U) || ((u32) (arg0 - 0xD) <= 1U) || ((s32) arg0 == 0x12) || ((u32) (arg0 - 0x15) <= 3U) || ((s32) arg0 == 0x1C)) {
        return 1;
    }
    return 0;
}
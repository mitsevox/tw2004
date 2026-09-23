// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8004C774(u32 arg0);
s32 fn_8004C774(u32 arg0) {
    if (((u32) (arg0 - 1) <= 2U) || (arg0 == 4U)) {
        return 1;
    }
    return 0;
}
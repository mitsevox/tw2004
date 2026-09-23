// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282460;

s32 fn_801061C8(s8 arg0);
s32 fn_801061C8(s8 arg0) {
    if ((arg0 == (s8) (*(u8*)((u8*)(lbl_80282460) + 4))) || (arg0 == 2)) {
        return 1;
    }
    return 0;
}
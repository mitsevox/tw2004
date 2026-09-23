// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_802824B8;

s32 fn_8010F978(s32 arg0, s32 arg1);
s32 fn_8010F978(s32 arg0, s32 arg1) {
    s32 temp_r0;

    temp_r0 = (*(s32*)((u8*)(lbl_802824B8) + 4));
    if (temp_r0 == 0) {
        if ((arg0 < 0) || (arg0 >= 0x40) || (arg1 < 0) || (arg1 >= 0x40)) {
            return -1;
        }
        return arg0 + (arg1 << 6);
    }
    if (temp_r0 == 1) {
        if ((arg0 < 0) || (arg0 >= 0x80) || (arg1 < 0) || (arg1 >= 0x20)) {
            return -1;
        }
        return arg0 + (arg1 << 7);
    }
    return -1;
}
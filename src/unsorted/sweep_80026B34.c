// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801C6068[];

s32 fn_80026B34(u8* p0);
s32 fn_80026B34(u8* p0) {
    return *(s32*)(lbl_801C6068 + (*(s32*)(p0 + 0x34) * 344));
}

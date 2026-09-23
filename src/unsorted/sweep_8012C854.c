// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_802825B0;

s32 fn_8012C854(u8* arg0);
s32 fn_8012C854(u8* arg0) {
    if (arg0 == NULL) {
        return 3;
    }
    if ((void* ) lbl_802825B0 == NULL) {
        return 0x10;
    }
    *arg0 = (*(u8*)((u8*)(lbl_802825B0) + 0x98));
    return 0;
}
// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_802825B0;

s32 fn_8012A900(s32* arg0);
s32 fn_8012A900(s32* arg0) {
    if (arg0 == NULL) {
        return 3;
    }
    if ((s32) *arg0 == 0) {
        fn_80128528((*(void**)((u8*)(lbl_802825B0) + 0x184)), (*(void**)((u8*)(lbl_802825B0) + 0x188)));
        *arg0 = 2;
    }
    return 0;
}
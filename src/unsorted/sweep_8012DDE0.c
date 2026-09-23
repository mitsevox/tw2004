// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8012CCD8(s32);
s32 fn_8012D1A0();
extern void* lbl_802825B8;

s32 fn_8012DDE0(s32* arg0);
s32 fn_8012DDE0(s32* arg0) {
    s32 temp_r3;
    s32 temp_r3_2;

    if (arg0 == NULL) {
        return 3;
    }
    temp_r3_2 = fn_8012CCD8(2);
    if (temp_r3_2 != 0) {
        return temp_r3_2;
    }
    temp_r3 = fn_8012D1A0();
    *arg0 = (*(s32*)((u8*)(lbl_802825B8) + 0x54));
    return temp_r3;
}
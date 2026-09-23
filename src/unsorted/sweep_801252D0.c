// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009D74C(s32, s32);
s32 fn_8009DBAC(s32, s32);
s32 fn_8012D5E4(s32, s32);
s32 fn_80124A98();
s32 fn_80124AAC();
extern void* lbl_80281988;

s32 fn_801252D0(void);
s32 fn_801252D0(void) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r31 = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (temp_r31 == -0x19) {
        return -0x19;
    }
    if (fn_8012D5E4((*(s32*)((u8*)(lbl_80281988) + 8)), (*(s32*)((u8*)(lbl_80281988) + 4))) != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    return temp_r3 & ((s32) (-temp_r3 | temp_r3) >> 0x1F);
}
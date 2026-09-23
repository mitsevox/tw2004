// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009D74C();
s32 fn_8009DBAC();
s32 fn_8012D560();
s32 fn_8012D9B4();
s32 fn_80124A98();
s32 fn_80124AAC();
s32 fn_80125354();
extern u8* lbl_80281988;

s32 fn_80125354(void);
s32 fn_80125354(void) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r31 = fn_8009D74C(0, 0);
    fn_8009DBAC(0, 0);
    if (temp_r31 == -0x19) {
        return -0x19;
    }
    if (fn_8012D560() != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    if ((u8) (*(u8*)((u8*)(lbl_80281988) + 0xC)) == 0) {
        fn_8012D9B4(lbl_80281988 + 0xE);
        (*(u8*)((u8*)(lbl_80281988) + 0xC)) = 1U;
    }
    return 0;
}
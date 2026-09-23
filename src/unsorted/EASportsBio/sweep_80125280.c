// Small functions found by the sweep (sweep.py). Original file: EASportsBio.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_8012D694();
s32 fn_80124A98();
extern void* lbl_80281988;

s32 fn_80125280(void);
s32 fn_80125280(void) {
    s32 temp_r3;

    if (fn_8012D694() != 0) {
        return fn_80124A98();
    }
    temp_r3 = fn_80124AAC();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    (*(s8*)((u8*)(lbl_80281988) + 0xC)) = 0;
    return 0;
}
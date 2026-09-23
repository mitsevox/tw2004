// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8015F844(s8*, s32*);
s32 strcpy(s8*, s32);
extern s32 lbl_80281748;
extern void* lbl_80282460;

s32 fn_8010645C(s32 arg0, s8* arg1);
s32 fn_8010645C(s32 arg0, s8* arg1) {
    u32 temp_r0;

    temp_r0 = (*(u32*)((u8*)(lbl_80282460) + 0xC));
    if (temp_r0 == 0U) {
        return 0;
    }
    if (arg1 == NULL) {
        return 0;
    }
    if (arg0 == -1) {
        return 0;
    }
    strcpy(arg1, temp_r0 + arg0);
    if (fn_8015F844(arg1, &lbl_80281748) == 0) {
        *arg1 = 0;
    }
    return 1;
}
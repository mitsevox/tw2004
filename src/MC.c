// MC.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the sweep
// code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_800A2248();
s32 fn_800A1758(u8* p0);
void fn_800A19F4(void);
s32 fn_8009D74C();
s32 fn_8009DBAC(s32, s32);
s32 fn_8009E918(s32, s32);
s32 fn_800A2100(s32 arg0, s32 arg1);
s32 fn_800A218C(void);
s32 fn_8009F6A0(s32, s32);
s32 fn_8009F734(s32, s32);
s32 fn_800A2194(s32 arg0, s32 arg1);

s32 fn_800A1758(u8* p0) {
    s32 t0;
    t0 = fn_800A2248(*(s32*)p0, *(s32*)(p0 + 0x4));
    return ((u32)__cntlzw(t0) >> 5);
}

void fn_800A19F4(void) {
}

s32 fn_800A2100(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r3 = fn_8009D74C();
    if ((temp_r3 != 0) && (temp_r3 != -0x16) && (temp_r3 != -0x1D) && (temp_r3 != -0x18)) {
        return temp_r3;
    }
    temp_r31 = fn_8009E918(arg0, arg1);
    if (temp_r3 == 0) {
        fn_8009DBAC(arg0, arg1);
    }
    return temp_r31;
}

s32 fn_800A218C(void) {
    return -12;
}

s32 fn_800A2194(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;
    s32 temp_r3_2;

    temp_r3_2 = fn_8009D74C();
    if ((temp_r3_2 != 0) && (temp_r3_2 != -0x16)) {
        return temp_r3_2;
    }
    temp_r3 = fn_8009F734(arg0, arg1);
    if (temp_r3 != 0) {
        if (temp_r3_2 == 0) {
            fn_8009DBAC(arg0, arg1);
        }
        return temp_r3;
    }
    temp_r31 = fn_8009F6A0(arg0, arg1);
    if (temp_r3_2 == 0) {
        fn_8009DBAC(arg0, arg1);
    }
    return temp_r31;
}

// ---- end of sweep code ----

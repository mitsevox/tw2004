// GoDynamicCam.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80187910[];
extern s32 lbl_80281D88;
s32 fn_80009B34();
void fn_80039FF8(void);
s32 fn_80009E70();
void fn_8003A074(void);
s32 fn_8003A76C(void* arg0);
u8 fn_8003D0EC();
s32 fn_8003CAFC(void* arg0);
extern u8 lbl_801879D8[];
s32 fn_8003CBD4(s32 p0);
s32 fn_8003D0A0(s32 p0, s32 p1);
u8 fn_8003CBE8();
u8 fn_8003CD9C(s32, s32, s32);
s32 fn_8003D7A0(s32 arg0, s32 arg1);
u8 fn_800453C8(s32, void*);
void fn_8003DAC8(void* arg0, s32 arg1, f32* arg2, f32* arg3);

void fn_80039FF8(void) {
    s32 t0;
    t0 = fn_80009B34(40, 2, 0, lbl_80187910, 938);
    lbl_80281D88 = t0;
    *(s32*)(((u8*)t0) + 0x4) = 0;
    *(s32*)((u8*)lbl_80281D88) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x8) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0xC) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x10) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x14) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x18) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x20) = 0;
}

void fn_8003A074(void) {
    (*(s32*)((u8*)(lbl_80281D88) + 0x10)) = 0;
    (*(s32*)((u8*)(lbl_80281D88) + 0x14)) = 0;
    (*(s32*)((u8*)(lbl_80281D88) + 0x18)) = 0;
    (*(s32*)((u8*)(lbl_80281D88) + 0x20)) = 0;
    if ((u32) (*(u32*)((u8*)(lbl_80281D88) + 4)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281D88) + 4)));
        (*(u32*)((u8*)(lbl_80281D88) + 4)) = 0U;
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281D88) + 0)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281D88) + 0)));
        (*(u32*)((u8*)(lbl_80281D88) + 0)) = 0U;
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281D88) + 8)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281D88) + 8)));
        (*(u32*)((u8*)(lbl_80281D88) + 8)) = 0U;
    }
    if ((u32) (*(u32*)((u8*)(lbl_80281D88) + 0xC)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(lbl_80281D88) + 0xC)));
        (*(u32*)((u8*)(lbl_80281D88) + 0xC)) = 0U;
    }
    (*(s32*)((u8*)(lbl_80281D88) + 0x1C)) = 0;
    fn_80009E70(lbl_80281D88);
}

s32 fn_8003A76C(void* arg0) {
    u8 temp_r0;

    temp_r0 = (*(u8*)((u8*)(arg0) + 0xAD));
    if ((temp_r0 == 3) || (temp_r0 == 1) || ((temp_r0 >= 0x1DU) && (temp_r0 <= 0x21U)) || ((temp_r0 >= 0x28U) && (temp_r0 <= 0x2DU)) || (temp_r0 == 0xD) || (temp_r0 == 0x1C) || (temp_r0 == 0x22)) {
        return 1;
    }
    return 0;
}

s32 fn_8003CAFC(void* arg0) {
    if (arg0 == NULL) {
        return 0;
    }
    if (fn_8003D0EC() == 0) {
        return 0;
    }
    if ((s32) (*(u8*)((u8*)(arg0) + 0)) != 0x44) {
        return 0;
    }
    if ((s32) (*(u8*)((u8*)(arg0) + 1)) != 0x45) {
        return 0;
    }
    return (s8) (*(u8*)((u8*)(arg0) + 2)) == 0x46;
}

s32 fn_8003CBD4(s32 p0) {
    return *(s32*)(lbl_801879D8 + (p0 << 2));
}

s32 fn_8003D0A0(s32 p0, s32 p1) {
    return ((u32)((-(p0 & (1 << p1))) | (p0 & (1 << p1))) >> 31);
}

s32 fn_8003D7A0(s32 arg0, s32 arg1) {
    if (arg0 == 0) {
        return 0;
    }
    if ((fn_8003CBE8() != 0) && (fn_8003CD9C(arg0, arg1, 1) != 0)) {
        return 1;
    }
    return 0;
}

void fn_8003DAC8(void* arg0, s32 arg1, f32* arg2, f32* arg3) {
    if (arg0 != NULL) {
        if (fn_800453C8(arg1, arg0) != 0) {
            if (((u8) (*(u8*)((u8*)(arg0) + 0xAF)) == 0x15) || ((u8) (*(u8*)((u8*)(arg0) + 0xB0)) == 0x15)) {
                if (arg2 != NULL) {
                    *arg2 = (*(f32*)((u8*)(arg0) + 0x64));
                }
                if (arg3 != NULL) {
                    *arg3 = -(*(f32*)((u8*)(arg0) + 0x60));
                }
            } else {
                if (arg2 != NULL) {
                    *arg2 = -(*(f32*)((u8*)(arg0) + 0x64));
                }
                if (arg3 != NULL) {
                    *arg3 = (*(f32*)((u8*)(arg0) + 0x60));
                }
            }
        } else {
            if (arg2 != NULL) {
                *arg2 = (*(f32*)((u8*)(arg0) + 0x64));
            }
            if (arg3 != NULL) {
                *arg3 = (*(f32*)((u8*)(arg0) + 0x60));
            }
        }
    }
}

// ---- end of sweep code ----

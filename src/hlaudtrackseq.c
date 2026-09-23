#include "game_types.h"

void fn_800AA694(void);
void fn_800AAAA0(void);
void fn_800AAB48(void* arg0, void* arg1);
void fn_800AAD14(void);
void fn_800AAE08();
void fn_800AADE8(void);
void fn_800AAE70(void);
void fn_800AAEEC(u8* p0);
s32 Rand_Next(s32);
s32 fn_800AB32C(u32 arg0);
extern s32 lbl_80282080;
s32 fn_800AB374(void);
extern f32 lbl_80281460;
f32 fn_800AB39C(void);

void fn_800AA694(void) {
}

void fn_800AAAA0(void) {
}

void fn_800AAB48(void* arg0, void* arg1) {
    u8 temp_r3;
    void* temp_r4;

    temp_r4 = (*(void**)((u8*)(arg1) + 0x2C));
    temp_r3 = 1 << (s8) (*(u8*)((u8*)(arg0) + 3));
    if ((u8) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        (*(u8*)((u8*)(temp_r4) + 0)) = (u8) ((*(u8*)((u8*)(temp_r4) + 0)) | temp_r3);
        return;
    }
    (*(u8*)((u8*)(temp_r4) + 1)) = (u8) ((*(u8*)((u8*)(temp_r4) + 1)) | temp_r3);
}

void fn_800AAD14(void) {
}

void fn_800AADE8(void) {
    fn_800AAE08();
}

void fn_800AAE70(void) {
    fn_800AAE08();
}

void fn_800AAEEC(u8* p0) {
    *(u8*)(p0 + 0x66) = *(u8*)(((u8*)*(s32*)(p0 + 0x8)) + 0x3);
}

s32 fn_800AB32C(u32 arg0) {
    if (arg0 != 0) {
        return Rand_Next(1) % arg0;
    }
    return 0;
}

s32 fn_800AB374(void) {
    return (((u32)__cntlzw(lbl_80282080) >> 5) & 0xFF);
}

f32 fn_800AB39C(void) {
    return lbl_80281460;
}

#include "game_types.h"

void fn_800A9AC4(void);
s32 fn_800AAE90();
s32 Stm_Start();
void fn_800AA0D8(void* arg0);
s32 fn_800AAEFC();
s32 Stm_Tick();
void fn_800AA2A4(void* arg0);
void fn_800AA444(u8* p, u8 v);
void fn_800AB118();
void fn_800AB14C();
void fn_800AA2EC(void);
void fn_800AA30C(u8* p, u8 v);
void fn_800AA32C(void);
s32 fn_800AB1B8();
void fn_800AA3D4(u8* arg0);
extern s32 lbl_80282060;
s32 fn_800AA498(s32 p0);

void fn_800A9AC4(void) {
}

void fn_800AA0D8(void* arg0) {
    (*(s8*)((u8*)(arg0) + 0x3E)) = 0;
    if (!(*(*(u8**)((u8*)(arg0) + 8)) & 8)) {
        fn_800AAE90(arg0);
        return;
    }
    Stm_Start();
}

void fn_800AA2A4(void* arg0) {
    (*(f32*)((u8*)(arg0) + 0x4C)) = (f32) ((*(f32*)((u8*)(arg0) + 0x4C)) + (*(f32*)((u8*)(arg0) + 0x50)));
    if (!(*(*(u8**)((u8*)(arg0) + 8)) & 8)) {
        fn_800AAEFC(arg0);
        return;
    }
    Stm_Tick();
}

void fn_800AA2EC(void) {
    fn_800AB118();
}

void fn_800AA30C(u8* p, u8 v) {
    fn_800AA444(p, v);
}

void fn_800AA32C(void) {
    fn_800AB14C();
}

void fn_800AA3D4(u8* arg0) {
    if (!(*arg0 & 8)) {
        fn_800AB1B8();
    }
}

void fn_800AA444(u8* p, u8 v) {
    *(u8*)(p + 0x64) = v;
}

s32 fn_800AA498(s32 p0) {
    return ((u32)((-(lbl_80282060 & (1 << (p0 & 0xFF)))) | (lbl_80282060 & (1 << (p0 & 0xFF)))) >> 31);
}

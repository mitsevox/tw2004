#include "game_types.h"

void fn_800D33F0(void);
void UStream_UnregisterHandler();
void fn_800D3424(void);
extern u8 lbl_80200538[];
void fn_8000E790();
void fn_800D344C(s32 p0);
s32 fn_800D4010(s32 arg0);
s32 fn_800D4EF8(s32 p0, s32 p1);
void fn_800D477C();
void fn_800D7660(void);
void fn_800D4F14();
void fn_800D7684(s32 p0, s32 p1, s32 p2);
s32 fn_800D86DC(s32 arg0);
s32 fn_800D8720(s32 arg0);
extern s32 lbl_80282250;
s32 fn_800D9954(void);
extern u8 lbl_80200268[];
extern u8 lbl_80200290[];
extern u8 lbl_802002B8[];
s32 fn_800D995C(s32 p0);
s32 fn_800D9970(s32 p0);
s32 fn_800D9984(s32 p0);
extern u8 lbl_80191A08[];
s32 fn_800D9E00(s32 p0);

void fn_800D33F0(void) {
}

void fn_800D3424(void) {
    UStream_UnregisterHandler(1163021856);
}

void fn_800D344C(s32 p0) {
    fn_8000E790(p0, 8944, lbl_80200538);
}

s32 fn_800D4010(s32 arg0) {
    u8 var_r0;

    var_r0 = 0;
    if ((arg0 >= 0x17) && (arg0 <= 0x26)) {
        var_r0 = 1;
    }
    return var_r0;
}

s32 fn_800D4EF8(s32 p0, s32 p1) {
    return ((u32)((-(p0 & (1 << p1))) | (p0 & (1 << p1))) >> 31);
}

void fn_800D7660(void) {
    fn_800D477C();
    fn_800D9954();
}

void fn_800D7684(s32 p0, s32 p1, s32 p2) {
    fn_800D4F14(p0, p2);
    fn_800D9954();
}

s32 fn_800D86DC(s32 arg0) {
    switch (arg0) {
    case 16:
        return 0;
    case 17:
        return 1;
    case 13:
        return 2;
    default:
        return 3;
    }
}

s32 fn_800D8720(s32 arg0) {
    switch (arg0) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

s32 fn_800D9954(void) {
    return lbl_80282250;
}

s32 fn_800D995C(s32 p0) {
    return *(s32*)(lbl_802002B8 + (p0 << 2));
}

s32 fn_800D9970(s32 p0) {
    return *(s32*)(lbl_80200290 + (p0 << 2));
}

s32 fn_800D9984(s32 p0) {
    return *(s32*)(lbl_80200268 + (p0 << 2));
}

s32 fn_800D9E00(s32 p0) {
    return *(s32*)(lbl_80191A08 + (p0 << 2));
}

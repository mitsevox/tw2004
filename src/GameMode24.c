#include "game_types.h"

extern u8 lbl_8020CF90[];
void UStream_UnregisterHandler();
void fn_8000E790();
void fn_800F0570(void);
void fn_800F05B0(s32 p0);
void fn_800F05DC(s32 p0);
extern u8 lbl_8028234C;
u8 fn_800F0818(void);
s32 fn_800F0E18(void);
extern s32 lbl_80282350;
extern s32 lbl_80282354;
s32 fn_800F0E20(u8* p0);
void fn_800F0E30(s32 p0, s32 p1);
u8* fn_800F0EA0(s32 p0);
s32 fn_800F0EF4(s32 p0);
s32 fn_800F0F10(s32 p0);
s32 fn_800F0F30(s32 p0);
s32 fn_800F1008(s32 i);
s32 fn_800F102C(void);

void fn_800F0570(void) {
    UStream_UnregisterHandler(1381254499);
    UStream_UnregisterHandler(1381254515);
    UStream_UnregisterHandler(1381254510);
}

void fn_800F05B0(s32 p0) {
    fn_8000E790(p0, 5664, lbl_8020CF90);
}

void fn_800F05DC(s32 p0) {
    fn_8000E790(p0, 14208, (lbl_8020CF90 + 0x1620));
}

u8 fn_800F0818(void) {
    return lbl_8028234C;
}

s32 fn_800F0E18(void) {
    return 1;
}

s32 fn_800F0E20(u8* p0) {
    *(s32*)p0 = lbl_80282354;
    return lbl_80282350;
}

void fn_800F0E30(s32 p0, s32 p1) {
    lbl_80282354 = p1;
    lbl_80282350 = p0;
}

u8* fn_800F0EA0(s32 p0) {
    return (lbl_8020CF90 + (p0 * 48));
}

s32 fn_800F0EF4(s32 p0) {
    return (*(s32*)(lbl_8020CF90 + 0x4DA0) + *(s32*)(lbl_8020CF90 + (p0 * 48)));
}

s32 fn_800F0F10(s32 p0) {
    return (*(s32*)(lbl_8020CF90 + 0x4DA0) + *(s32*)((lbl_8020CF90 + (p0 * 48)) + 0x4));
}

s32 fn_800F0F30(s32 p0) {
    return *(s32*)((lbl_8020CF90 + (*(s32*)((lbl_8020CF90 + (p0 * 48)) + 0xC) << 7)) + 0x1604);
}

s32 fn_800F1008(s32 i) {
    return *(s32*)(fn_800F0EA0(i) + 0x14);
}

s32 fn_800F102C(void) {
    return 0;
}

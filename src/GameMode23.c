#include "game_types.h"

void fn_800EDE78(void);
extern u8 lbl_80205F3C[];
void UStream_UnregisterHandler();
void fn_8000E790();
void fn_800EDEE8(void);
void fn_800EDF34(s32 p0);
void fn_800EDF60(s32 p0);
void fn_800EDF90(s32 p0);
extern s32 gpGame;
extern s32 gpSaveData;
void fn_800EE064(void);
extern u8 lbl_8028233C;
u8 fn_800EE470(void);
extern u8 lbl_80205F30[];
s32 fn_800EE8B0(void);
u8* fn_800EE8B8(void);
void fn_80119934();
void fn_800EF294(void);
s32 fn_800EF834(void);
s32 fn_800EFB88();
s32 fn_800EFBAC(void);
s32 fn_800EFDFC(s32 p0);
s32 fn_800EFA70();
s32 fn_800EFE3C(void);
u8* fn_800EFE60(s32 p0);
s32 fn_801197CC();
void fn_800F009C(void);
u8 fn_8011908C(s32, s32);
s32 fn_8011937C(s32, s32, u8);
void fn_800F018C(void);

void fn_800EDE78(void) {
}

void fn_800EDEE8(void) {
    UStream_UnregisterHandler(1346847075);
    UStream_UnregisterHandler(1346847092);
    UStream_UnregisterHandler(1346847088);
    UStream_UnregisterHandler(1346847086);
}

void fn_800EDF34(s32 p0) {
    fn_8000E790(p0, 3100, lbl_80205F3C);
}

void fn_800EDF60(s32 p0) {
    fn_8000E790(p0, 2604, (lbl_80205F3C + 0xC1C));
}

void fn_800EDF90(s32 p0) {
    fn_8000E790(p0, 132, (lbl_80205F3C + 0x6FC8));
}

void fn_800EE064(void) {
    *(s32*)(((u8*)gpGame) + 0xE0) = *(s32*)((lbl_80205F3C + ((*(s32*)((lbl_80205F3C + (*(s32*)(((u8*)(gpSaveData + 0x10000)) - 0x49C8) * 100)) + 0x4) - 1) * 84)) + 0xC1C);
}

u8 fn_800EE470(void) {
    return lbl_8028233C;
}

s32 fn_800EE8B0(void) {
    return 3;
}

u8* fn_800EE8B8(void) {
    return lbl_80205F30;
}

void fn_800EF294(void) {
    fn_80119934(0);
}

s32 fn_800EF834(void) {
    return 31;
}

s32 fn_800EFBAC(void) {
    s32 t0;
    t0 = fn_800EFB88();
    return (t0 + 2004);
}

s32 fn_800EFDFC(s32 p0) {
    return (*(s32*)(lbl_80205F3C + 0x704C) + *(s32*)(lbl_80205F3C + (p0 * 100)));
}

s32 fn_800EFE3C(void) {
    s32 t0;
    t0 = fn_800EFA70();
    return *(s32*)(((u8*)t0) + 0x10);
}

u8* fn_800EFE60(s32 p0) {
    return ((lbl_80205F3C + (p0 * 100)) + 0x14);
}

void fn_800F009C(void) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, 0);
    t1 = fn_8011908C(0, t0);
    fn_8011937C(0, t0, (((u32)__cntlzw((t1 & 0xFF)) >> 5) & 0xFF));
}

void fn_800F018C(void) {
    fn_8011937C(0, 0, (u8) (fn_8011908C(0, 0) == 0));
}

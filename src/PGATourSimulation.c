#include "game_types.h"

extern u8 lbl_80224070[];
u8* fn_80117628(s32 p0);
void UStream_UnregisterHandler();
void fn_8011766C(void);
extern u8 lbl_8024B9CC[];
void fn_8000E790();
void fn_80117694(s32 p0);
extern u8 lbl_80282500;
u8 fn_80117DE0(void);
void fn_80117DE8(s32 p0, s32 p1);
s32 fn_80118A4C(u8* p0, u8* p1);
extern u8 lbl_80226870[];
void fn_8011A85C(s32 arg0);
s32 fn_80118F10(s32 p0, s32 p1, s32 p2);
extern s32 gpSaveData;
extern u8 lbl_80194074[];
s32 fn_80119028(s32 p0);
s32 fn_80117600();
s32 fn_80119118(void);
u8 fn_8011908C(s32, s32);
s32 fn_8011913C(s32 arg0, s32 arg1, u8 arg2);
extern u8 lbl_80223C70[];
void fn_8011ABF8(s32 arg0);
s32 fn_801197A4(void);
s32 fn_801197CC(s32 p0, s32 p1);
extern u8 lbl_8028184D;
void fn_801198F8(s32 p0, s32 p1);
s32 fn_80119A04(s32 p0, s32 p1);
s32 fn_80119A2C(void);
s32 fn_8011A6F4(s32 p0, s32 p1);
void fn_8011B160(s32 p0);
extern u8 lbl_8028184C;
s32 fn_8011A890(s32 arg0);
s32 fn_8011AAC0(s32);
s32 fn_8011AE1C(s32, s32);
void fn_8011AEC8(s32 arg0);
s32 fn_8011AC40(s32, s32);
void fn_8011AF14(s32 arg0);
s32 fn_8011AF60(s32);
void fn_8011B00C(void);
void fn_8011B048(s32 arg0);
void fn_8011B100(void);
void fn_8011B13C(s32 p0);
void fn_8011B1DC();
void fn_8011B1AC(u8* p0, s32 p1);
void fn_8011B264();
void fn_8011B234(u8* p0, s32 p1);
void fn_8011B2C4(u8* p0, s32 p1);
void fn_8011B2F4(u8* p0, s32 p1);
void fn_8011B324(u8* p0, s32 p1);
void fn_8011B354(u8* p0, s32 p1);
void fn_8011B384(u8* p0, s32 p1);
void fn_8011B3B8(u8* p0, s32 p1);
void fn_8011B3E8(u8* p0, s32 p1);
void fn_8011B418(u8* p0, s32 p1);
void fn_8011B448(u8* p0, s32 p1);
void fn_8011B478(u8* p0, s32 p1);
void fn_8011B4A8(u8* p0, s32 p1);
void fn_8011B4D8(u8* p0, s32 p1);
void fn_8011B508(u8* p0, s32 p1);
void fn_8011B538(u8* p0, s32 p1);
void fn_8011B56C(u8* p0, s32 p1);
void fn_8011B59C(u8* p0, s32 p1);
void fn_8011B5CC(u8* p0, s32 p1);
void fn_8011C054(void);
void fn_8011C058(u8 v);
void fn_8011C060(u8 v);

u8* fn_80117628(s32 p0) {
    return (lbl_80224070 + (p0 * 80));
}

void fn_8011766C(void) {
    UStream_UnregisterHandler(1346851668);
}

void fn_80117694(s32 p0) {
    fn_8000E790(p0, 18096, lbl_8024B9CC);
}

u8 fn_80117DE0(void) {
    return lbl_80282500;
}

void fn_80117DE8(s32 p0, s32 p1) {
    lbl_80282500 = p1;
}

s32 fn_80118A4C(u8* p0, u8* p1) {
    return (*(s32*)p0 - *(s32*)p1);
}

s32 fn_80118F10(s32 p0, s32 p1, s32 p2) {
    fn_8011A85C(p0);
    return *(s32*)((lbl_80226870 + (p1 * 4900)) + (p2 << 2));
}

s32 fn_80119028(s32 p0) {
    return *(s32*)(lbl_80194074 + (p0 << 2));
}

s32 fn_80119118(void) {
    s32 t0;
    t0 = fn_80117600();
    return *(s16*)((u8*)t0);
}

s32 fn_8011913C(s32 arg0, s32 arg1, u8 arg2) {
    s32 var_r31;
    s32 var_r3;

    var_r31 = *(s32*)fn_80117628(arg1);
    if (fn_8011908C(arg0, arg1) != 0) {
        if (arg2 != 0) {
            var_r31 += 1;
        }
    } else if (arg2 == 0) {
        var_r31 -= 1;
    }
    var_r3 = 0x12;
    if (var_r31 <= 0x12) {
        var_r3 = var_r31;
    }
    return var_r3;
}

s32 fn_801197A4(void) {
    s32 t0;
    t0 = fn_80117600();
    return (*(s32*)(((u8*)t0) + 0x14) & 0xFF);
}

s32 fn_801197CC(s32 p0, s32 p1) {
    fn_8011ABF8(p0);
    return *(s32*)(lbl_80223C70 + (p1 << 2));
}

void fn_801198F8(s32 p0, s32 p1) {
    u8* t0;
    t0 = fn_80117628(0);
    *(s32*)((u8*)t0) = p1;
    lbl_8028184D = 1;
}

s32 fn_80119A04(s32 p0, s32 p1) {
    u8* t0;
    t0 = fn_80117628(p1);
    return *(s32*)((u8*)t0);
}

s32 fn_80119A2C(void) {
    s32 t0;
    t0 = fn_80117600();
    return *(s32*)(((u8*)t0) + 0x18);
}

s32 fn_8011A6F4(s32 p0, s32 p1) {
    u8* t0;
    t0 = fn_80117628(p1);
    return (*(s32*)(((u8*)t0) + 0x4C) & 0xFF);
}

void fn_8011A85C(s32 arg0) {
    if ((u8) lbl_8028184C != 0) {
        fn_8011B160(arg0);
        lbl_8028184C = 0;
    }
}

void fn_8011ABF8(s32 arg0) {
    if ((u8) lbl_8028184D != 0) {
        fn_8011A890(arg0);
        lbl_8028184D = 0;
        fn_8011AAC0(arg0);
    }
}

void fn_8011AEC8(s32 arg0) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AE1C(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}

void fn_8011AF14(s32 arg0) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AC40(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x1C);
}

void fn_8011B00C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AF60(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}

void fn_8011B048(s32 arg0) {
    s32 var_r31;

    var_r31 = 0x1C;
    do {
        fn_8011AC40(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x1E);
}

void fn_8011B100(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011B094(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}

void fn_8011B13C(s32 p0) {
    fn_8011AC40(p0, 30);
}

void fn_8011B160(s32 p0) {
    fn_8011AEC8(p0);
    fn_8011AF14(p0);
    fn_8011B00C();
    fn_8011B048(p0);
    fn_8011B100();
    fn_8011B13C(p0);
}

void fn_8011B1AC(u8* p0, s32 p1) {
    fn_8011B1DC(*(s32*)(p0 + 0x8), *(u16*)(p0 + 0x6), p1, p0);
}

void fn_8011B234(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0xE), *(u16*)(p0 + 0x10), p1, p0);
}

void fn_8011B2C4(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x12), *(u16*)(p0 + 0x14), p1, p0);
}

void fn_8011B2F4(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x16), *(u16*)(p0 + 0x2), p1, p0);
}

void fn_8011B324(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x18), *(u16*)(p0 + 0x12), p1, p0);
}

void fn_8011B354(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x1A), *(u16*)(p0 + 0x1C), p1, p0);
}

void fn_8011B384(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x1E), (*(u16*)(p0 + 0x14) - *(u16*)(p0 + 0x12)), p1, *(u16*)(p0 + 0x12));
}

void fn_8011B3B8(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x20), *(u16*)(p0 + 0x22), p1, p0);
}

void fn_8011B3E8(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x14), *(u16*)(p0 + 0x24), p1, p0);
}

void fn_8011B418(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x26), *(u16*)(p0 + 0x2), p1, p0);
}

void fn_8011B448(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x28), *(u16*)(p0 + 0x2A), p1, p0);
}

void fn_8011B478(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x2C), *(u16*)(p0 + 0x2E), p1, p0);
}

void fn_8011B4A8(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x30), *(u16*)(p0 + 0x32), p1, p0);
}

void fn_8011B4D8(u8* p0, s32 p1) {
    fn_8011B264(*(u16*)(p0 + 0x34), *(u16*)(p0 + 0x12), p1, p0);
}

void fn_8011B508(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x36), *(u16*)(p0 + 0x2), p1, p0);
}

void fn_8011B538(u8* p0, s32 p1) {
    fn_8011B264((*(u16*)(p0 + 0x26) + *(u16*)(p0 + 0x24)), *(u16*)(p0 + 0x14), p1, *(u16*)(p0 + 0x26));
}

void fn_8011B56C(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x38), *(u16*)(p0 + 0x2A), p1, p0);
}

void fn_8011B59C(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x3A), *(u16*)(p0 + 0x2E), p1, p0);
}

void fn_8011B5CC(u8* p0, s32 p1) {
    fn_8011B1DC(*(u16*)(p0 + 0x3C), *(u16*)(p0 + 0x32), p1, p0);
}

void fn_8011C054(void) {
}

void fn_8011C058(u8 v) {
    lbl_8028184C = v;
}

void fn_8011C060(u8 v) {
    lbl_8028184D = v;
}

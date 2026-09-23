#include "game_types.h"

s32 fn_800F9BF8(void);
u8 Player_IsHoled(s32);
s32 fn_800F9C00(void);
s32 fn_800FA2C8(void);
void fn_800FA994(void);
void fn_800FD6A0(void);
extern s32 gpGame;
extern u8 lbl_80192C00[];
void fn_800FDC0C(u8* p0, u8* p1, u8* p2);
void fn_800FCCF0();
void fn_800FDF38(void);
s32 fn_800FDF58(void);
void fn_800E53F0();
void fn_800FDFC4(s32 p0, s32 p1, s32 p2);
void fn_80062D38();
void fn_800FDFFC(s32 p0, s32 p1);
void fn_80062D6C();
void fn_800A7664();
void fn_800FE02C(void);
void fn_800FE054(s32 p0, s32 p1);
void fn_800FE080(s32 p0, s32 p1);
void fn_800FE0AC(s32 p0, s32 p1);
void fn_800FE0D8(void);
void fn_800FE100(s32 p0, s32 p1, s32 p2);
void fn_800FE138(s32 p0, s32 p1);
void fn_800FE164(s32 p0, s32 p1);

s32 fn_800F9BF8(void) {
    return 5;
}

s32 fn_800F9C00(void) {
    if ((Player_IsHoled(0) != 0) || (Player_IsHoled(1) != 0)) {
        return 1;
    }
    return 0;
}

s32 fn_800FA2C8(void) {
    return 0;
}

void fn_800FA994(void) {
}

void fn_800FD6A0(void) {
}

void fn_800FDC0C(u8* p0, u8* p1, u8* p2) {
    *(s32*)p0 = *(s32*)(lbl_80192C00 + (*(s32*)(((u8*)gpGame) + 0x14) * 12));
    *(s32*)p1 = *(s32*)((lbl_80192C00 + (*(s32*)(((u8*)gpGame) + 0x14) * 12)) + 0x4);
    *(s32*)p2 = *(s32*)((lbl_80192C00 + (*(s32*)(((u8*)gpGame) + 0x14) * 12)) + 0x8);
}

void fn_800FDF38(void) {
    fn_800FCCF0();
}

s32 fn_800FDF58(void) {
    return 0;
}

void fn_800FDFC4(s32 p0, s32 p1, s32 p2) {
    fn_800E53F0(21, p0, p1, p2, p0);
}

void fn_800FDFFC(s32 p0, s32 p1) {
    fn_80062D38(19, p0, (p1 & 0xFF));
}

void fn_800FE02C(void) {
    fn_80062D6C(16, 1);
}

void fn_800FE054(s32 p0, s32 p1) {
    fn_80062D38(44, p0, p1);
}

void fn_800FE080(s32 p0, s32 p1) {
    fn_80062D38(41, p0, p1);
}

void fn_800FE0AC(s32 p0, s32 p1) {
    fn_80062D38(37, p0, p1);
}

void fn_800FE0D8(void) {
    fn_80062D6C(16, 2);
}

void fn_800FE100(s32 p0, s32 p1, s32 p2) {
    fn_800E53F0(23, p0, p1, p2, p0);
}

void fn_800FE138(s32 p0, s32 p1) {
    fn_80062D38(22, p0, p1);
}

void fn_800FE164(s32 p0, s32 p1) {
    fn_800A7664(4, p0, p1);
}

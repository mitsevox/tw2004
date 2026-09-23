#include "game_types.h"

void fn_80100128();
void fn_801000E8(void);
void fn_80100108(void);
s32 Game_GetMode();
u8 fn_80100294(void);
extern u8 lbl_802823F0;
extern s32 lbl_80282424;
extern s32 lbl_80282428;
void fn_801002C0(void);
extern u8 lbl_802823E2;
void fn_80100308(void);
extern s32 lbl_802823FC;
s32 fn_80100AF8(void);
s32 fn_80100C00(void);
s32 fn_80101738(void);
s32 fn_80101C9C(void);
s32 fn_80101CC4(void);
void fn_80125854();
void fn_80101CD8(void);
extern u8 lbl_802823E3;
extern u8 lbl_802823E4;
void fn_800A6EC8();
void fn_80101CFC(void);
void fn_80101D24(void);
s32 fn_80101DF4(void);
extern u8 lbl_802823F8;
void fn_80101EDC(void);
extern u8 gPlayers[];
void fn_80100160();
u8* fn_80101EE8(void);
void fn_80062D6C();
void fn_80101F18(s32 p0);
void fn_80062D38();
void fn_80101F40(s32 p0, s32 p1);
void fn_800E58B4();
void fn_80101F70(void);
void fn_800A7664();
void fn_80101F94(s32 p0, s32 p1);
void fn_80101FC0(s32 p0, s32 p1);

void fn_801000E8(void) {
    fn_80100128();
}

void fn_80100108(void) {
    fn_80100128();
}

u8 fn_80100294(void) {
    return Game_GetMode() == 11;
}

void fn_801002C0(void) {
    if (fn_80100294() != 0) {
        if ((u8) lbl_802823F0 != 0) {
            lbl_802823F0 = 0;
            lbl_80282428 = lbl_80282424;
        }
        fn_80100308();
    }
}

void fn_80100308(void) {
    if ((u8) lbl_802823E2 != 0) {
        lbl_802823E2 = 0;
        lbl_80282428 = lbl_80282424;
    }
}

s32 fn_80100AF8(void) {
    if ((fn_80100294() != 0) && ((s32) lbl_802823FC == 5)) {
        return 1;
    }
    return 0;
}

s32 fn_80100C00(void) {
    return 0;
}

s32 fn_80101738(void) {
    if ((Game_GetMode() == 0xB) && ((s32) lbl_80282428 == 5) && (((s32) lbl_802823FC == 1) || ((s32) lbl_802823FC == 0xA) || ((s32) lbl_802823FC == 8) || ((s32) lbl_802823FC == 9))) {
        return 0;
    }
    return 1;
}

s32 fn_80101C9C(void) {
    if (((s32) lbl_802823FC == 0xC) && ((s32) lbl_80282428 != 0x13)) {
        return 1;
    }
    return 0;
}

s32 fn_80101CC4(void) {
    return (((u32)__cntlzw((12 - lbl_802823FC)) >> 5) & 0xFF);
}

void fn_80101CD8(void) {
    fn_80125854(1);
}

void fn_80101CFC(void) {
    lbl_802823E4 = 1;
    fn_800A6EC8();
}

void fn_80101D24(void) {
    lbl_802823E3 = 1;
    fn_800A6EC8();
}

s32 fn_80101DF4(void) {
    if ((Game_GetMode() == 0xB) && ((s32) lbl_802823FC != 0xB)) {
        return 0;
    }
    return 1;
}

void fn_80101EDC(void) {
    lbl_802823F8 = 1;
}

u8* fn_80101EE8(void) {
    fn_80100160();
    *(s32*)(gPlayers + 0xA08) = 0;
    return gPlayers;
}

void fn_80101F18(s32 p0) {
    fn_80062D6C(43, p0);
}

void fn_80101F40(s32 p0, s32 p1) {
    fn_80062D38(15, (p0 & 0xFF), p1);
}

void fn_80101F70(void) {
    fn_800E58B4(40);
}

void fn_80101F94(s32 p0, s32 p1) {
    fn_80062D38(38, p0, p1);
}

void fn_80101FC0(s32 p0, s32 p1) {
    fn_800A7664(9, p0, p1);
}

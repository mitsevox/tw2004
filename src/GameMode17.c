#include "game_types.h"

s32 fn_800F5D10(void);
void fn_800F19D4();
void fn_800F673C(void);
extern u8 gPlayers[];
void AI_DefaultTarget();
void fn_800F1EE4();
void fn_800F6760(void);
u8* fn_800F6788(void);
void fn_800F5F58();
s32 fn_800F6820(void);
void fn_800F6828(void);
extern s32 lbl_802823A8;
s32 fn_800F6A00(void);
extern s32 lbl_802823A4;
void fn_80125910();
s32 fn_800F6A34(void);
void fn_800F6A3C(void);

s32 fn_800F5D10(void) {
    return 0;
}

void fn_800F673C(void) {
    fn_800F19D4();
    fn_800F6788();
}

void fn_800F6760(void) {
    fn_800F6788();
    AI_DefaultTarget(0);
}

u8* fn_800F6788(void) {
    fn_800F1EE4();
    *(u8*)(gPlayers + 0xE9C) = 0;
    *(s32*)(gPlayers + 0xDC0) = 5;
    *(u8*)(gPlayers + 0x1D94) = 0;
    *(s32*)(gPlayers + 0x1CB8) = 5;
    *(u8*)(gPlayers + 0x2C8C) = 0;
    *(s32*)(gPlayers + 0x2BB0) = 5;
    *(u8*)(gPlayers + 0x3B84) = 0;
    *(s32*)(gPlayers + 0x3AA8) = 5;
    *(u8*)(gPlayers + 0x4A7C) = 0;
    *(s32*)(gPlayers + 0x49A0) = 5;
    return gPlayers;
}

s32 fn_800F6820(void) {
    return 1;
}

void fn_800F6828(void) {
    fn_800F5F58();
}

s32 fn_800F6A00(void) {
    return lbl_802823A8;
}

s32 fn_800F6A34(void) {
    return lbl_802823A4;
}

void fn_800F6A3C(void) {
    fn_80125910(1);
}

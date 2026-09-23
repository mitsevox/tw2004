// GameMode22.c (our name): game mode 22 (GameRound.c starts it with fn_80125E68), and before it the
// trophy case's text (Rookie of the Year, Player of the Year, the money and scoring leaders; "Earned
// on %s"). One file: both halves share its .data, .sdata and .sbss blocks. Not yet decompiled; the
// sweep code below is the matched small functions.

#include "game/modes/mode22.h"

void fn_801260B8(void);
void fn_801260BC(void);
void GameModeStroke_SetupNextGolfer();
void fn_80126130(void);
s32 fn_8012632C(void);
void fn_80126698();
void fn_80126FB0();
void fn_80126418(void);
void fn_8012643C(void);
void fn_80126EC0();
void fn_80126E68(void);
void fn_80126E88(void);
void fn_80126F7C(void);
void fn_80126F80(void);
void fn_80126F84(s32 p0);
s32 fn_80126FA0(void);
s32 Game_GetMode();
s32 fn_80126FD8(void);
s32 fn_80127098(s32 arg0);

void fn_801260B8(void) {
}

void fn_801260BC(void) {
}

void fn_80126130(void) {
    GameModeStroke_SetupNextGolfer();
}

s32 fn_8012632C(void) {
    return 0;
}

void fn_80126418(void) {
    fn_80126FB0(0);
}

void fn_8012643C(void) {
    fn_80126698();
}

void fn_80126E68(void) {
    fn_80126EC0();
}

void fn_80126E88(void) {
    fn_80126EC0();
    lbl_80195498.n8 = 5;
    lbl_80195498.bC = 0;
}

void fn_80126F7C(void) {
}

void fn_80126F80(void) {
}

void fn_80126F84(s32 p0) {
    lbl_80195498.n4 = p0;
}

s32 fn_80126FA0(void) {
    return lbl_80195498.n0;
}

s32 fn_80126FD8(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((22 - t0)) >> 5) & 0xFF);
}

s32 fn_80127098(s32 arg0) {
    switch (arg0) {
    case 6:
        return 0;
    case 7:
        return 1;
    case 5:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    default:
        return 0;
    }
}

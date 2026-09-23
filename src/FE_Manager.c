// FE_Manager.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80189560[];
extern s32 lbl_80281EC8;
void Mem_cpy();
s32 fn_80009B34();
void fn_80009E70();
void fn_80076F80(u8* p0);
s32 fn_80013400();
u32 fn_800136DC(s32);
s32 fn_80076FDC(void);
extern u8 lbl_801D7148[];
s32 fn_80077148(void);
void fn_8008B704();
void fn_8008B754();
void fn_8008B7D0();
void fn_8008DAEC();
void fn_8008DBE8();
void fn_80092198();
void fn_800772E0(void);
void fn_8008D8F4();
void fn_8009220C();
void fn_8007731C(void);
void fn_80077340(void);
void fn_80077344(void);
void fn_80077348(void);
extern u8 gReplayData[];
void Session_SetGolfer();
void fn_80010284();
void fn_8007744C();
void fn_80079DAC();
void fn_80079EA8();
void fn_8008D8CC();
void fn_800E0B38();
void fn_800E14E0();
void fn_8007739C(s32 p0);
void fn_800773F8(void);
void Golfer_LoadCreatedFromSave();
void fn_80079D30();
void fn_80077428(void);
s32 fn_80077B18(u32 arg0);
void fn_80077C1C();
void fn_80077B78(void);
s32 fn_80077BDC(s32 arg0);
s32 Rand_Next(s32);
s32 fn_8008E6D4(s32);
s32 FE_CrAP_TurnOnPart(s32, s32, s32);
s32 fn_80078E34(s32);
void fn_8007975C(s32 arg0, s32 arg1, s32 arg2);
s32 fn_800797E0(s32, s32, s32, s32);
void fn_80079664(s32 arg0);
s32 fn_80077ACC();
s32 fn_80103B74(s32);
extern void* lbl_80281ED4;
void fn_80079974(void);

void fn_80076F80(u8* p0) {
    s32 t0;
    t0 = fn_80009B34(*(s32*)(p0 + 0x24), 2, 32, lbl_80189560, 285);
    lbl_80281EC8 = t0;
    Mem_cpy(t0, *(s32*)p0, *(s32*)(p0 + 0x24));
    fn_80009E70(p0);
}

s32 fn_80076FDC(void) {
    s32 var_r31;

    fn_80013400();
    var_r31 = 0;
loop_1:
    if (fn_800136DC(var_r31) != 0U) {
        return 1;
    }
    var_r31 += 1;
    if (var_r31 >= 4) {
        return 0;
    }
    goto loop_1;
}

s32 fn_80077148(void) {
    return (((u32)__cntlzw((*(s32*)(lbl_801D7148 + 0x20) - *(s32*)(lbl_801D7148 + 0x24))) >> 5) & 0xFF);
}

void fn_800772E0(void) {
    fn_8008B754(1);
    fn_8008B704();
    fn_8008B7D0(1);
    fn_8008DAEC();
    fn_8008DBE8();
    fn_80092198();
}

void fn_8007731C(void) {
    fn_8009220C();
    fn_8008D8F4();
}

void fn_80077340(void) {
}

void fn_80077344(void) {
}

void fn_80077348(void) {
}

void fn_8007739C(s32 p0) {
    Mem_cpy(gReplayData, p0, 3880);
    fn_800E0B38(10);
    Session_SetGolfer(*(u8*)(gReplayData + 0x10), 0);
    fn_800E14E0(*(s32*)(gReplayData + 0xF00));
}

void fn_800773F8(void) {
    fn_80010284();
    fn_80079EA8();
    fn_8007744C();
    fn_8008D8CC();
    fn_80079DAC();
}

void fn_80077428(void) {
    Golfer_LoadCreatedFromSave();
    fn_80079D30();
}

s32 fn_80077B18(u32 arg0) {
    if ((arg0 <= 1U) || ((u32) (arg0 - 3) <= 2U) || ((s32) arg0 == 7) || ((u32) (arg0 - 0xA) <= 1U) || ((u32) (arg0 - 0xD) <= 1U) || ((s32) arg0 == 0x12) || ((u32) (arg0 - 0x15) <= 3U) || ((s32) arg0 == 0x1C)) {
        return 1;
    }
    return 0;
}

void fn_80077B78(void) {
    fn_80077C1C(-1, 0);
    fn_80077C1C(-1, 1);
    fn_80077C1C(-2, 0);
    fn_80077C1C(-2, 1);
    fn_80077C1C(-3, 0);
    fn_80077C1C(-3, 1);
}

s32 fn_80077BDC(s32 arg0) {
    switch (arg0) {
    case -1:
        return 0;
    case -2:
        return 1;
    case -3:
        return 2;
    default:
        return 0;
    }
}

void fn_80079664(s32 arg0) {
    FE_CrAP_TurnOnPart(0xC, 1, 0);
    FE_CrAP_TurnOnPart(0xC, 2, 0);
    FE_CrAP_TurnOnPart(0xC, 3, 0);
    FE_CrAP_TurnOnPart(0xC, 5, 0);
    FE_CrAP_TurnOnPart(0xC, 6, 0);
    FE_CrAP_TurnOnPart(0xC, 7, 0);
    FE_CrAP_TurnOnPart(0xC, 0, 0);
    fn_8008E6D4(0);
    fn_8007975C(arg0, 1, 0);
    fn_8007975C(arg0, 2, 0);
    fn_8007975C(arg0, 7, 0);
    fn_800797E0(arg0, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_80078E34(arg0);
}

void fn_80079974(void) {
    s32 temp_r31;
    s32 var_r31;

    temp_r31 = fn_80077ACC();
    if ((u8) (*(u8*)((u8*)(lbl_80281ED4) + 0x1063F)) != 0) {
        fn_80103B74(0);
        fn_8007975C(temp_r31, 9, 0);
        FE_CrAP_TurnOnPart(3, 0, 0);
        FE_CrAP_TurnOnPart(0xE, 0, 0);
        FE_CrAP_TurnOnPart(0xF, 0, 0);
        FE_CrAP_TurnOnPart(0x10, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 1, 0);
        FE_CrAP_TurnOnPart(0xC, 2, 0);
        FE_CrAP_TurnOnPart(0xC, 3, 0);
        FE_CrAP_TurnOnPart(0xC, 5, 0);
        FE_CrAP_TurnOnPart(0xC, 6, 0);
        FE_CrAP_TurnOnPart(0xC, 7, 0);
        FE_CrAP_TurnOnPart(0xC, 4, 0);
        var_r31 = 0;
        do {
            FE_CrAP_TurnOnPart(0xD, 0, var_r31);
            FE_CrAP_TurnOnPart(0xD, 1, var_r31);
            var_r31 += 1;
        } while (var_r31 < 8);
        FE_CrAP_TurnOnPart(0xD, 2, 0);
        fn_80103B74(1);
    }
}

// ---- end of sweep code ----

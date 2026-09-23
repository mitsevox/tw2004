#include "game_types.h"

void fn_800D2994();
s32 fn_80116540(void);
void fn_80116548(void);
s32 fn_8011654C(void);
s32 fn_80116554(void);
s32 fn_8011655C(void);
void fn_80116564(void);
void fn_80116568(void);
s32 fn_8011656C(void);
void fn_80116574(void);
void fn_80116578(void);
extern s32 lbl_802817F8;
s32* fn_80116598(void);
void fn_801165A0(void);
s32 fn_801165A4(void);
void fn_801165AC(void);
void fn_80116DD4(void);
void fn_80117264();
void fn_80116F80(s32 p0, s32 p1, s32 p2);
void fn_801170EC(void);
void Session_SetGolfer();
void fn_800E0B38();
void fn_800F0E3C();
void fn_8011714C(void);
s32 fn_80117180(void);
s32 fn_801174B8(u32 arg0, u32 arg1);
s32 fn_801174E4(u32 arg0, u32 arg1);

s32 fn_80116540(void) {
    return 0;
}

void fn_80116548(void) {
}

s32 fn_8011654C(void) {
    return 0;
}

s32 fn_80116554(void) {
    return 0;
}

s32 fn_8011655C(void) {
    return -1;
}

void fn_80116564(void) {
}

void fn_80116568(void) {
}

s32 fn_8011656C(void) {
    return -1;
}

void fn_80116574(void) {
}

void fn_80116578(void) {
    fn_800D2994();
}

s32* fn_80116598(void) {
    return &lbl_802817F8;
}

void fn_801165A0(void) {
}

s32 fn_801165A4(void) {
    return 0;
}

void fn_801165AC(void) {
}

void fn_80116DD4(void) {
}

void fn_80116F80(s32 p0, s32 p1, s32 p2) {
    fn_80117264(p0, p2);
}

void fn_801170EC(void) {
    fn_800D2994();
}

void fn_8011714C(void) {
    Session_SetGolfer(30, 0);
    fn_800E0B38(24);
    fn_800F0E3C();
}

s32 fn_80117180(void) {
    return 0;
}

s32 fn_801174B8(u32 arg0, u32 arg1) {
    u8 var_r5;

    var_r5 = 0;
    if (((u32) (arg0 + 1) == arg1) || ((arg0 == 0xCU) && (arg1 == 1U))) {
        var_r5 = 1;
    }
    return var_r5;
}

s32 fn_801174E4(u32 arg0, u32 arg1) {
    u8 var_r5;

    var_r5 = 0;
    if (((u32) (arg0 - 1) == arg1) || ((arg0 == 1U) && (arg1 == 0xCU))) {
        var_r5 = 1;
    }
    return var_r5;
}

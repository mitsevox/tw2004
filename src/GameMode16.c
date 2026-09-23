#include "game_types.h"

s32 fn_800F4DB4(void);
void fn_800F19D4();
void fn_800F57C8();
void fn_800F577C(void);
extern u8 gPlayers[];
void AI_DefaultTarget();
void fn_800F1EE4();
void fn_800F57A0(void);
void fn_800F5014();
s32 fn_800F5848(void);
void fn_800F5850(void);
extern s32 lbl_80282394;
extern s32 lbl_80282398;
s32 fn_800F59CC(void);
s32 fn_800F59D4(void);
void fn_80125910();
void fn_800F5A88(void);

s32 fn_800F4DB4(void) {
    return 0;
}

void fn_800F577C(void) {
    fn_800F19D4();
    fn_800F57C8();
}

void fn_800F57A0(void) {
    fn_800F57C8();
    AI_DefaultTarget(0);
}

s32 fn_800F5848(void) {
    return 1;
}

void fn_800F5850(void) {
    fn_800F5014();
}

s32 fn_800F59CC(void) {
    return lbl_80282398;
}

s32 fn_800F59D4(void) {
    return lbl_80282394;
}

void fn_800F5A88(void) {
    fn_80125910(1);
}

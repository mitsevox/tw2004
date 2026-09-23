#include "game_types.h"

s32 fn_800F2C2C(void);
void fn_800F19D4();
void fn_800F3358();
void fn_800F330C(void);
void AI_DefaultTarget();
void fn_800F3330(void);
void fn_800F2E08();
s32 fn_800F3410(void);
void fn_800F3418(void);
extern u8 lbl_80211FB8[];
s32 fn_800F3654(s32 p0);
extern s32 lbl_80282374;
s32 fn_800F37F8(void);
extern s32 lbl_80282278;
extern u8 lbl_80282370;
void GOLFERSTATE_Switch();
void fn_800A6278();
void fn_800E3D90();
void fn_800F3800(void);
void fn_800F3828(void);
void fn_80062D6C();
void fn_800F39CC(s32 p0);

s32 fn_800F2C2C(void) {
    return 0;
}

void fn_800F330C(void) {
    fn_800F19D4();
    fn_800F3358();
}

void fn_800F3330(void) {
    fn_800F3358();
    AI_DefaultTarget(0);
}

s32 fn_800F3410(void) {
    return 1;
}

void fn_800F3418(void) {
    fn_800F2E08();
}

s32 fn_800F3654(s32 p0) {
    return *(s32*)(lbl_80211FB8 + (p0 << 3));
}

s32 fn_800F37F8(void) {
    return lbl_80282374;
}

void fn_800F3800(void) {
    fn_800F39CC(-1);
    fn_800A6278();
}

void fn_800F3828(void) {
    GOLFERSTATE_Switch(12, lbl_80282278);   // GS_SIMULATE
    fn_800E3D90();
    fn_800A6278();
    lbl_80282370 = 1;
}

void fn_800F39CC(s32 p0) {
    fn_80062D6C(55, p0);
}

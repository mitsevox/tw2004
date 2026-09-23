#include "game_types.h"

s32 fn_800F3C74(void);
void fn_800F19D4();
void fn_800F4698(void);
extern u8 lbl_8028237C;
extern u8 lbl_80282384;
extern s32 lbl_80282388;
extern s32 lbl_8028238C;
void AI_DefaultTarget();
void fn_800F1EE4();
void fn_800F46BC(void);
void fn_800F46E4(void);
void fn_800F3EBC();
s32 fn_800F475C(void);
void fn_800F4764(void);
s8 fn_800F4878(void);
void fn_800A6278();
void fn_800F39CC();
void fn_800F4894(void);
s32 fn_800F4B00(void);

s32 fn_800F3C74(void) {
    return 0;
}

void fn_800F4698(void) {
    fn_800F19D4();
    fn_800F46E4();
}

void fn_800F46BC(void) {
    fn_800F46E4();
    AI_DefaultTarget(0);
}

void fn_800F46E4(void) {
    fn_800F1EE4();
    lbl_8028238C = 5;
    lbl_80282388 = 5;
    lbl_80282384 = 0;
    lbl_8028237C = 0;
}

s32 fn_800F475C(void) {
    return 1;
}

void fn_800F4764(void) {
    fn_800F3EBC();
}

s8 fn_800F4878(void) {
    if ((s32) lbl_8028238C != 5) {
        return (s8) lbl_80282388;
    }
    return -1;
}

void fn_800F4894(void) {
    fn_800F39CC(-1);
    fn_800A6278();
    lbl_8028237C = 0;
}

s32 fn_800F4B00(void) {
    return ((u32)((-lbl_8028237C) | lbl_8028237C) >> 31);
}

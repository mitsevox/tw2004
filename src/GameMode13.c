#include "game_types.h"

s32 fn_800F6D0C(void);
void fn_800F19D4();
void fn_800F7CD4();
void fn_800F7C00(void);
void AI_DefaultTarget();
void fn_800A6278();
void fn_800F7CA0(void);
extern s32 lbl_802823B8;
extern s32 lbl_802823BC;
s32 fn_800F7D8C(void);
s32 fn_800F7D94(void);
s32 fn_800F7D9C(void);
s32 Rand_Next(s32);
s32 fn_800A624C();
s32 fn_800F263C(s32);
void fn_800F7DA4(void);
void fn_800F6ED4();
void fn_800F7ED8(void);
extern s32 lbl_802823B4;
s32 fn_800F80A0(void);
void fn_800E3C0C();
void fn_80125910();
void fn_800F80A8(void);
void fn_80062D6C();
void fn_800F80D4(s32 p0);

s32 fn_800F6D0C(void) {
    return 0;
}

void fn_800F7C00(void) {
    fn_800F19D4();
    fn_800F7CD4();
}

void fn_800F7CA0(void) {
    fn_800F7CD4();
    AI_DefaultTarget(0);
    fn_800F80D4(-1);
    fn_800A6278();
}

s32 fn_800F7D8C(void) {
    return 1;
}

s32 fn_800F7D94(void) {
    return lbl_802823BC;
}

s32 fn_800F7D9C(void) {
    return lbl_802823B8;
}

void fn_800F7DA4(void) {
    fn_800A624C();
    if (!(Rand_Next(0) & 1)) {
        fn_800F263C(0x15);
        return;
    }
    fn_800F263C(0x25);
}

void fn_800F7ED8(void) {
    fn_800F6ED4();
}

s32 fn_800F80A0(void) {
    return lbl_802823B4;
}

void fn_800F80A8(void) {
    fn_80125910(1);
    fn_800E3C0C(0);
}

void fn_800F80D4(s32 p0) {
    fn_80062D6C(17, p0);
}

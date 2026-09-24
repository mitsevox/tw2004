// Code8006F438.c (our name): made by fold.py from 6 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8* lbl_802811E8;
void fn_8006F5FC(void);
void fn_8006F4B4(void);
void fn_8006F4E0(void);
void fn_800676AC();
void fn_8006A89C();
void fn_8006F600(void);
void fn_8006F4F0(void);
void AI_TargetsHook();
void fn_80019648();
void fn_80037E50();
void fn_80046288();
void fn_8006F604(void);
void fn_8006F650();
void fn_80091708();
void fn_8009B894();
void fn_800A2E68();
void fn_800B26DC();
void fn_800C72F0();
void fn_800C8134();
void GM_InitForHole();
void fn_8006F518(void);
s32 fn_8000B68C(s32);
s32 fn_80010608(s32);
s32 fn_8003467C();
s32 fn_80046664();
s32 fn_80048EF4();
s32 fn_80058E40();
s32 fn_80064EA4();
s32 fn_8006FBF8();
s32 fn_80098C28();
s32 fn_800A2B34(s32);
s32 fn_800C830C();
s32 fn_800C9CA0();
s32 fn_8011E3B4();
void fn_8006F568(void);
void fn_8006F5F8(void);

void fn_8006F4B4(void) {
    fn_8006F5FC();
    lbl_802811E8[1] = 1;
}

void fn_8006F4E0(void) {
    lbl_802811E8[1] = 0;
}

void fn_8006F4F0(void) {
    fn_8006F600();
    fn_800676AC();
    fn_8006A89C();
}

void fn_8006F518(void) {
    fn_8009B894();
    fn_800C8134();
    AI_TargetsHook();
    fn_80019648();
    fn_80046288();
    fn_8006F650();
    GM_InitForHole();
    fn_800A2E68();
    fn_80091708();
    fn_80037E50();
    fn_800B26DC();
    fn_800C72F0();
    fn_8006F604();
}

void fn_8006F568(void) {
    s32 var_r31;

    fn_8006FBF8();
    fn_800C830C();
    fn_80058E40();
    fn_800C9CA0();
    if ((u8) *lbl_802811E8 != 0) {
        fn_8011E3B4();
        fn_8003467C();
        fn_80046664();
        fn_80064EA4();
        fn_80098C28();
        var_r31 = 0;
        do {
            fn_800A2B34(var_r31);
            var_r31 += 1;
        } while (var_r31 < 4);
        fn_80048EF4();
        fn_80010608(1);
        fn_8000B68C(1);
        *lbl_802811E8 = 0;
    }
}

void fn_8006F5F8(void) {
}

void fn_8006F5FC(void) {
}

void fn_8006F600(void) {
}

void fn_8006F604(void) {
}

// ---- end of sweep code ----

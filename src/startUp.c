// startUp.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_800AFB48(void);
void fn_800B0430(void);
void fn_800B0434(void);
s32 fn_800B0438(void);
s32 fn_800B0440(void);
void fn_800B0448(void);
void fn_800B6728();
void fn_800B04CC(void);
extern u8 lbl_80282110;
void fn_800B055C(void);
s32 fn_8009527C(s32);
extern s32 lbl_80282104;
s32 fn_800B0624(void);
void fn_800B0660(void);
s32 fn_800B5D34(s32, s32, s32);
extern s32 lbl_802820F8;
void fn_800B0698(s32 arg0);
void fn_800B5E88();
void fn_800B06CC(s32 p0);
s32 fn_800B0790(void);
s32 fn_800B0798(void);
extern u8 lbl_802814A0;
void fn_800B0954(void);
extern s32 lbl_80281498;
extern s32 lbl_8028149C;
void fn_800B13FC();
void fn_800B14E4(void);
void fn_800B1608(void);
void UStream_UnregisterHandler();
void fn_800B1644(void);

s32 fn_800AFB48(void) {
    return 1;
}

void fn_800B0430(void) {
}

void fn_800B0434(void) {
}

s32 fn_800B0438(void) {
    return 1;
}

s32 fn_800B0440(void) {
    return 1;
}

void fn_800B0448(void) {
}

void fn_800B04CC(void) {
    fn_800B6728();
}

void fn_800B055C(void) {
    lbl_80282110 = 1;
}

s32 fn_800B0624(void) {
    if ((u8) lbl_80282110 != 0) {
        lbl_80282110 = 0;
        fn_8009527C(lbl_80282104);
    }
    return 1;
}

void fn_800B0660(void) {
    if ((u8) lbl_80282110 != 0) {
        lbl_80282110 = 0;
        fn_8009527C(lbl_80282104);
    }
}

void fn_800B0698(s32 arg0) {
    fn_800B5D34(lbl_802820F8, (arg0 + 0x1F) & 0xFFFFFFE0, 0x20);
}

void fn_800B06CC(s32 p0) {
    fn_800B5E88(lbl_802820F8, p0);
}

s32 fn_800B0790(void) {
    return 17408;
}

s32 fn_800B0798(void) {
    return 1;
}

void fn_800B0954(void) {
    lbl_802814A0 = 0;
}

void fn_800B14E4(void) {
    lbl_80281498 = -1;
    lbl_8028149C = -1;
    fn_800B13FC();
}

void fn_800B1608(void) {
}

void fn_800B1644(void) {
    UStream_UnregisterHandler(1279608652);
}

// ---- end of sweep code ----

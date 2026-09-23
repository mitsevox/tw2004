// DiscError.c (our name): the disc-error screens ("The Disc Cover is open.", "Please insert the
// ... Game Disc", "An error has occurred.") shown from fn_800B7490 when the drive reports a
// problem, and the text printer that draws them. Not yet decompiled; the code below is the
// sweep's matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80282194;
extern s32 lbl_80282198;
void DCStoreRange();
void fn_800B6924(void);
void fn_800B6C0C(s32 v);
extern u16 lbl_80282190;
void fn_800B6CD0(u16 v);
extern u8 lbl_802814D0;
extern u8 lbl_802814D1;
extern u8 lbl_802814D2;
void fn_800B7684(u8 v);
void fn_800B768C(u8 v);
void fn_800B7694(u8 v);

void fn_800B6924(void) {
    DCStoreRange(lbl_80282194, lbl_80282198);
}

void fn_800B6C0C(s32 v) {
    lbl_80282194 = v;
}

void fn_800B6CD0(u16 v) {
    lbl_80282190 = v;
}

void fn_800B7684(u8 v) {
    lbl_802814D0 = v;
}

void fn_800B768C(u8 v) {
    lbl_802814D1 = v;
}

void fn_800B7694(u8 v) {
    lbl_802814D2 = v;
}

// ---- end of sweep code ----

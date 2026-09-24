// AudReverb.c (our name): made by fold.py from 4 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_800AF140(void);
void fn_800AF2D8(void);
void AXRegisterAuxACallback(void (*callback)(void*, void*), void* context);
extern void (*lbl_802820DC)(void*, void*);
extern void* lbl_802820E0;
void fn_800AF2DC(u8 arg0);
void fn_800AF31C(void);
void fn_800AF320(void);

void fn_800AF140(void) {
}

void fn_800AF2D8(void) {
}

void fn_800AF2DC(u8 arg0) {
    if (arg0 != 0) {
        AXRegisterAuxACallback(NULL, NULL);
        return;
    }
    AXRegisterAuxACallback(lbl_802820DC, lbl_802820E0);
}

void fn_800AF31C(void) {
}

void fn_800AF320(void) {
}

// ---- end of sweep code ----

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void AXRegisterAuxACallback(void (*callback)(void*, void*), void* context);
extern void (*lbl_802820DC)(void*, void*);
extern void* lbl_802820E0;

// Nonzero turns the AX aux A effect off; zero puts back the saved callback and its context.
void fn_800AF2DC(u8 arg0);
void fn_800AF2DC(u8 arg0) {
    if (arg0 != 0) {
        AXRegisterAuxACallback(NULL, NULL);
        return;
    }
    AXRegisterAuxACallback(lbl_802820DC, lbl_802820E0);
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game.h"

void fn_800D2A30(void);
void fn_800D2A64(UStreamObject* pObject);
void fn_800D2A90(UStreamObject* pObject);
void fn_800D2A30(void) {
    UStream_UnregisterHandler('CRI ');
    UStream_UnregisterHandler('CMPS');
}

void fn_800D2A64(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_801FA2F4), lbl_801FA2F4);
}

void fn_800D2A90(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_801FA1F8), lbl_801FA1F8);
}

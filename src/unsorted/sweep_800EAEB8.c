// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80203554[];
void UStream_UnregisterHandler();
void fn_8000E790();

void fn_800EAEB8(void);
void fn_800EAEEC(s32 p0);
void fn_800EAEB8(void) {
    UStream_UnregisterHandler(1347180832);
    UStream_UnregisterHandler(1347180915);
}

void fn_800EAEEC(s32 p0) {
    fn_8000E790(p0, 10624, lbl_80203554);
}

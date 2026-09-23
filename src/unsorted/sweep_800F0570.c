// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8020CF90[];
void UStream_UnregisterHandler();
void fn_8000E790();

void fn_800F0570(void);
void fn_800F05B0(s32 p0);
void fn_800F05DC(s32 p0);
void fn_800F0570(void) {
    UStream_UnregisterHandler(1381254499);
    UStream_UnregisterHandler(1381254515);
    UStream_UnregisterHandler(1381254510);
}

void fn_800F05B0(s32 p0) {
    fn_8000E790(p0, 5664, lbl_8020CF90);
}

void fn_800F05DC(s32 p0) {
    fn_8000E790(p0, 14208, (lbl_8020CF90 + 0x1620));
}

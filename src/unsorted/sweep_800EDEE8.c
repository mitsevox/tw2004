// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80205F3C[];
void UStream_UnregisterHandler();
void fn_8000E790();

void fn_800EDEE8(void);
void fn_800EDF34(s32 p0);
void fn_800EDF60(s32 p0);
void fn_800EDF90(s32 p0);
void fn_800EDEE8(void) {
    UStream_UnregisterHandler(1346847075);
    UStream_UnregisterHandler(1346847092);
    UStream_UnregisterHandler(1346847088);
    UStream_UnregisterHandler(1346847086);
}

void fn_800EDF34(s32 p0) {
    fn_8000E790(p0, 3100, lbl_80205F3C);
}

void fn_800EDF60(s32 p0) {
    fn_8000E790(p0, 2604, (lbl_80205F3C + 0xC1C));
}

void fn_800EDF90(s32 p0) {
    fn_8000E790(p0, 132, (lbl_80205F3C + 0x6FC8));
}

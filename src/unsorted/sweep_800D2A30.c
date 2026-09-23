// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801FA1F8[];
extern u8 lbl_801FA2F4[];
void UStream_UnregisterHandler();
void fn_8000E790();

void fn_800D2A30(void);
void fn_800D2A64(s32 p0);
void fn_800D2A90(s32 p0);
void fn_800D2A30(void) {
    UStream_UnregisterHandler(1129466144);
    UStream_UnregisterHandler(1129140307);
}

void fn_800D2A64(s32 p0) {
    fn_8000E790(p0, 22512, lbl_801FA2F4);
}

void fn_800D2A90(s32 p0) {
    fn_8000E790(p0, 252, lbl_801FA1F8);
}

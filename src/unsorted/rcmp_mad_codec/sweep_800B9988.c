// Small functions found by the sweep (sweep.py). Original file: rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern s32 lbl_802821D4;
void UStream_UnregisterHandler();
void fn_80009E70();
s32 fn_8000FB88();

void fn_800B9988(void);
void fn_800B99BC(s32 p0);
void fn_800B9988(void) {
    UStream_UnregisterHandler(1413828384);
    UStream_UnregisterHandler(1111575622);
}

void fn_800B99BC(s32 p0) {
    s32 t0;
    t0 = fn_8000FB88(p0, 0, 0);
    lbl_802821D4 = t0;
    fn_80009E70(p0);
}

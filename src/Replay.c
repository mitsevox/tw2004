// Replay.c (EA's name, from its asserts; TW06): not yet decompiled.
#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801888C0[];
extern s32 lbl_80281E48;
s32 fn_80009B34();
void fn_80009E70();
s32 fn_8006BED4(void);
void fn_8006BF20(void);
extern u8 gReplayData[];
u8* fn_8006BF4C(void);

s32 fn_8006BED4(void) {
    s32 t0;
    t0 = fn_80009B34(86624, 2, 0, lbl_801888C0, 77);
    lbl_80281E48 = t0;
    *(u8*)(((u8*)(t0 + 0x10000)) + 0x525C) = 0;
    return (t0 + 0x10000);
}

void fn_8006BF20(void) {
    fn_80009E70(lbl_80281E48);
    lbl_80281E48 = 0;
}

u8* fn_8006BF4C(void) {
    *(u8*)(gReplayData + 0xF10) = 0;
    return gReplayData;
}

// ---- end of sweep code ----

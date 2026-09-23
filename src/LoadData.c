// LoadData.c (our name): made by fold.py from 5 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80281C04;
extern s32 lbl_80281C08;
extern s32 lbl_80281C0C;
s32 fn_800951A0();
void fn_8000B984(void);
void UStream_UnregisterHandler();
void fn_8000BA14(void);
void fn_8000BA6C(void);
extern s32 lbl_80280DF8;
void fn_8000BEC8(void);
extern u8* gpGame;
s32 Game_GetMode(void);

void fn_8000B984(void) {
    s32 t0;
    s32 t1;
    s32 t2;
    t0 = fn_800951A0(147700, 16, 1);
    lbl_80281C04 = t0;
    t1 = fn_800951A0(52, 16, 1);
    lbl_80281C0C = t1;
    t2 = fn_800951A0(32768, 16, 1);
    lbl_80281C08 = t2;
}

void fn_8000BA14(void) {
    UStream_UnregisterHandler(1954047538);
}

void fn_8000BA6C(void) {
    UStream_UnregisterHandler(1819238756);
}

void fn_8000BEC8(void) {
    *(s32*)(((u8*)lbl_80280DF8) + 0x508) = 0;
}

s32 Game_GetMode(void) {
    return *(s32*)(gpGame + 0x0);
}

// ---- end of sweep code ----

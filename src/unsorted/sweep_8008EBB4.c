// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DF8;
void UStream_Close();
s32 UStream_Open();

void fn_8008EBB4(void);
void fn_8008EBE4(void);
void fn_8008EBB4(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 1932));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_8008EBE4(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

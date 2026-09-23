// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DF8;
void UStream_Close();
s32 UStream_Open();

void fn_800150B8(void);
void fn_800150E0(void);
void fn_8001510C(void);
void fn_80015134(void);
void fn_80015164(void);
void fn_8001518C(void);
void fn_800151BC(void);
void fn_800151E4(void);
void fn_80015214(void);
void fn_80015244(void);
void fn_8001526C(void);
void fn_800150B8(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_800150E0(void) {
    s32 t0;
    t0 = UStream_Open(lbl_80280DF8);
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_8001510C(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_80015134(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 644));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015164(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_8001518C(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 1288));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_800151BC(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_800151E4(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 3220));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015214(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 2576));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015244(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_8001526C(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 3864));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

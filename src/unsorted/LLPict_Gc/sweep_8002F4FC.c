// Small functions found by the sweep (sweep.py). Original file: LLPict_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_801876C8[];
extern s32 lbl_802810C0;
s32 fn_80009B34();
void fn_80009E70();
void fn_80056204();
void fn_80056208();

void fn_8002F4FC(void);
void fn_8002F540(void);
void fn_8002F4FC(void) {
    s32 t1;
    fn_80056204();
    t1 = fn_80009B34(2048, 2, 32, lbl_801876C8, 68);
    *(s32*)((u8*)lbl_802810C0) = t1;
}

void fn_8002F540(void) {
    fn_80056208();
    fn_80009E70(*(s32*)((u8*)lbl_802810C0));
}

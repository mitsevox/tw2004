// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpGame;
extern u8 lbl_80192C00[];

void fn_800FDC0C(u8* p0, u8* p1, u8* p2);
void fn_800FDC0C(u8* p0, u8* p1, u8* p2) {
    *(s32*)p0 = *(s32*)(lbl_80192C00 + (*(s32*)(((u8*)gpGame) + 0x14) * 12));
    *(s32*)p1 = *(s32*)((lbl_80192C00 + (*(s32*)(((u8*)gpGame) + 0x14) * 12)) + 0x4);
    *(s32*)p2 = *(s32*)((lbl_80192C00 + (*(s32*)(((u8*)gpGame) + 0x14) * 12)) + 0x8);
}

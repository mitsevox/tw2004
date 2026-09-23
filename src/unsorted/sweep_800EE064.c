// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpGame;
extern s32 gpSaveData;
extern u8 lbl_80205F3C[];

void fn_800EE064(void);
void fn_800EE064(void) {
    *(s32*)(((u8*)gpGame) + 0xE0) = *(s32*)((lbl_80205F3C + ((*(s32*)((lbl_80205F3C + (*(s32*)(((u8*)(gpSaveData + 0x10000)) - 0x49C8) * 100)) + 0x4) - 1) * 84)) + 0xC1C);
}

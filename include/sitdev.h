// sitdev.h (our name): the data SitDevFile.c shares with the code around 0x80067560..0x80067B80
// (which frees its scripts and sets its values) and 0x800BCD68.

#ifndef SITDEV_H
#define SITDEV_H

#include "engine.h"

#define SITDEV_NUM_VALUES 96

// The block lbl_802811B8 points at (lbl_801D5AB0, 0x140 bytes).
typedef struct SitDevData {
    u16   aValue[SITDEV_NUM_VALUES];            // 0x000  set through fn_80067B1C
    u32   aSetBits[SITDEV_NUM_VALUES / 32];     // 0x0C0  bit n: aValue[n] has been set
    void* pCC;                                  // 0x0CC  SitDev_LoadScripts' argument; freed by fn_8006765C
    void* pD0;                                  // 0x0D0  freed by fn_8006765C when set
    void* pD4;                                  // 0x0D4  allocated by SitDev_LoadScripts; freed by fn_8006765C
    u8    unkD8[0x140 - 0xD8];
} SitDevData;
LAYOUT_ASSERT(SitDevData, 0x140);

extern SitDevData* lbl_802811B8;    // 0x802811B8 (.sdata): &lbl_801D5AB0
extern s32 lbl_801FA198[5];         // per player; cleared by fn_800BB1C0
extern s32 lbl_801FA1AC[5];         // per player; 1: fn_800BB1F8 is true

// Store uValue in pValues[nIndex] and set bit nIndex of pSetBits.
void fn_80067B1C(u16* pValues, int nIndex, u16 uValue, u32* pSetBits);

#endif

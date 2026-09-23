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
    u8*   pD4;                                  // 0x0D4  allocated by SitDev_LoadScripts, one byte per
                                                //        SitDevScripts.n10; freed by fn_8006765C
    u8    unkD8[0x140 - 0xD8];
} SitDevData;
LAYOUT_ASSERT(SitDevData, 0x140);

extern SitDevData* lbl_802811B8;    // 0x802811B8 (.sdata): &lbl_801D5AB0

// The situation scripts' header (lbl_80282208): the block whose address is the first word of
// SitDev_LoadScripts' argument. fn_800BB4B4 turns the offsets at 0x14..0x20 into pointers.
typedef struct SitDevScripts {
    s32   nEntries;             // 0x00  entries of 0x30 bytes at p14 (0x80067710)
    u8    unk4[0x10 - 0x4];
    u32   n10;                  // 0x10  bytes in SitDevData.pD4 (fn_800BD74C clears them)
    u8*   p14;                  // 0x14
    u8*   p18;                  // 0x18  entries of 0x68 bytes (fn_800BCD68)
    u8*   p1C;                  // 0x1C
    u8*   p20;                  // 0x20
} SitDevScripts;

extern SitDevScripts* lbl_80282208; // 0x80282208 (.sbss), NULL until the scripts are loaded

void fn_800BD74C(void);             // clear SitDevData.pD4
extern s32 lbl_801FA198[5];         // per player; cleared by fn_800BB1C0
extern s32 lbl_801FA1AC[5];         // per player; 1: fn_800BB1F8 is true

// Store uValue in pValues[nIndex] and set bit nIndex of pSetBits.
void fn_80067B1C(u16* pValues, int nIndex, u16 uValue, u32* pSetBits);

#endif

// SitDevFile.c (EA's name, from its asserts; TW06): a watcher that follows the ball after a shot
// (an event 48 frames in, a call when it reaches surface 105), the loading of the situation
// scripts (lbl_80282208; their state is in the block lbl_802811B8 points at, sitdev.h), the values
// the scripts test, and running the scripts' actions (commentary lines, sounds, music).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

SwapField lbl_80191168[9] = {
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
};
SwapField lbl_801911B0[7] = {
    {1, 1}, {1, 1}, {2, 2}, {12, 4}, {8, 1}, {16, 2}, {8, 2},
};
SwapField lbl_801911E8[5] = {
    {1, 1}, {1, 1}, {1, 1}, {1, 1}, {100, 2},
};
SwapField lbl_80191210[4] = {
    {1, 1}, {1, 1}, {2, 2}, {4, 4},
};

SitDevScripts* lbl_80282208;

void fn_800BB4B0(void);

// ---- scripts -------------------------------------------------------------------------------

void fn_800BB4E8(void);
void fn_800BB4B4(SitDevScripts* pScripts);
void fn_800BB52C(void);

// Take the loaded scripts (the first time, byte-swap them and fix up their pointers) and allocate
// their per-entry bytes.
void SitDev_LoadScripts(SitDevScripts** ppScripts) {
    fn_800BB4B0();
    lbl_802811B8->pCC = ppScripts;
    if (lbl_80282208 == NULL) {
        lbl_80282208 = *ppScripts;
        fn_800BB4E8();
        fn_800BB4B4(lbl_80282208);
        fn_800BB52C();
    }
    lbl_802811B8->pD4 = fn_80009B34(lbl_80282208->n10, 2, 16, "SitDevFile.c", 105);
    fn_800BD74C();
}

void fn_800BB4B0(void) {
}

// The header's table offsets are from its start.
void fn_800BB4B4(SitDevScripts* pScripts) {
    pScripts->p14 = (SitDevEntry*)((u8*)pScripts->p14 + (uptr)pScripts);
    pScripts->p18 = (SitDevAction*)((u8*)pScripts->p18 + (uptr)pScripts);
    pScripts->p1C = (SitDevEntry8*)((u8*)pScripts->p1C + (uptr)pScripts);
    pScripts->p20 = pScripts->p20 + (uptr)pScripts;
}

// Byte-swap the header in place.
void fn_800BB4E8(void) {
    void* pSrc = lbl_80282208;
    void* pDst = lbl_80282208;
    fn_8001F08C(&pSrc, &pDst, lbl_80191168, 9, 1);
}

// Byte-swap the tables in place, then put the bit-fields of the p14 and p1C entries in order.
void fn_800BB52C(void) {
    void* pSrc;
    void* pDst;
    SitDevEntry* pEntry;
    u32 i;
    SitDevEntry8* pEntry8;
    u16 uRaw;
    if (lbl_80282208->nEntries != 0) {
        pSrc = lbl_80282208->p14;
        pDst = lbl_80282208->p14;
        fn_8001F08C(&pSrc, &pDst, lbl_801911B0, 7, lbl_80282208->nEntries);
    }
    if (lbl_80282208->n04 != 0) {
        pSrc = lbl_80282208->p18;
        pDst = lbl_80282208->p18;
        fn_8001F08C(&pSrc, &pDst, lbl_801911E8, 5, lbl_80282208->n04);
    }
    if (lbl_80282208->n08 != 0) {
        // fake match: pEntry8 carries the source pointer here (permuter find): it gives the i / pEntry8
        // registers of the second loop below
        pEntry8 = lbl_80282208->p1C;
        pSrc = pEntry8;
        pDst = lbl_80282208->p1C;
        fn_8001F08C(&pSrc, &pDst, lbl_80191210, 4, lbl_80282208->n08);
    }
    if (lbl_80282208->n0C != 0) {
        pSrc = lbl_80282208->p20;
        pDst = lbl_80282208->p20;
        // EA bug: the byte count and the value width are swapped, and the address of pDst is
        // passed for pDst (the call is shaped like fn_8001F08C's)
        BYTESWAP_SWAPDATA((u8**)&pSrc, (u8*)&pDst, 4, lbl_80282208->n0C * 4);
    }
    for (i = 0; i < lbl_80282208->nEntries; i++) {
        pEntry = &lbl_80282208->p14[i];
        uRaw = pEntry->b2.uRaw;
        pEntry->b2.s.n11 = uRaw & 0x7FF;
        lbl_80282208->p14[i].b2.s.n5 = (uRaw >> 11) & 0x1F;
    }
    for (i = 0; i < lbl_80282208->n08; i++) {
        pEntry8 = &lbl_80282208->p1C[i];
        uRaw = pEntry8->b2.uRaw;
        pEntry8->b2.s.n11 = uRaw & 0x7FF;
        lbl_80282208->p1C[i].b2.s.n5 = (uRaw >> 11) & 0x1F;
    }
}

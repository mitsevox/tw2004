// SitDevFile.c (EA's name, from its asserts; TW06's and TW07's SitDevFile.c): loading the
// situation scripts. SitDev_LoadScripts takes the hole stream's 'sscr' chunk: a header
// (SitDevScripts, gpSitDevScripts) and its tables, the situations (p14; TW07's Situation), their
// actions (p18; Action), the responses (p1C; Response) and 16-byte names (p20). The chunk is
// little-endian: the first load byte-swaps it in place (the layouts below) and turns the header's
// offsets into pointers. The scripts' run-time state is the block gpSitDevData points at
// (SitDev.c).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

// The byte-swap layouts (ByteSwap_Records) of the header, a situation, an action and a response.
SwapField gSitDevHeaderSwap[9] = {
    {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
};
SwapField gSitDevSituationSwap[7] = {
    {1, 1}, {1, 1}, {2, 2}, {12, 4}, {8, 1}, {16, 2}, {8, 2},
};
SwapField gSitDevActionSwap[5] = {
    {1, 1}, {1, 1}, {1, 1}, {1, 1}, {100, 2},
};
SwapField gSitDevResponseSwap[4] = {
    {1, 1}, {1, 1}, {2, 2}, {4, 4},
};

SitDevScripts* gpSitDevScripts;    // the loaded scripts' header; NULL until the first load and
                                   // again after SitDev_vCloseModule

void SitDev_BeginLoadScripts(void);

// ---- scripts -------------------------------------------------------------------------------

void SitDev_SwapHeader(void);
void SitDev_BindHeader(SitDevScripts* pScripts);
void SitDev_SwapTables(void);

// The hole stream's 'sscr' chunk handler (SitDev_vRegisterStreamClients). Keeps the chunk
// (SitDevData.pCC, freed at round end); the first time, takes its first word as the scripts' header
// (gpSitDevScripts), byte-swaps the header (SitDev_SwapHeader), turns its offsets into pointers
// (SitDev_BindHeader) and byte-swaps the tables (SitDev_SwapTables). Then allocates the group flags
// (pD4, one byte per group, header n10) and clears them (SitDev_ClearGroupFlags).
void SitDev_LoadScripts(SitDevScripts** ppScripts) {
    SitDev_BeginLoadScripts();
    gpSitDevData->pCC = ppScripts;
    if (gpSitDevScripts == NULL) {
        gpSitDevScripts = *ppScripts;
        SitDev_SwapHeader();
        SitDev_BindHeader(gpSitDevScripts);
        SitDev_SwapTables();
    }
    gpSitDevData->pD4 = StaticMem_Alloc(gpSitDevScripts->n10, 2, 16, "SitDevFile.c", 105);
    SitDev_ClearGroupFlags();
}

// Empty in this build; SitDev_LoadScripts calls it first.
void SitDev_BeginLoadScripts(void) {
}

// Turns the header's four table offsets (from the header's start) into pointers: the situations
// (p14), the actions (p18), the responses (p1C) and the names (p20, 16 bytes each, state value 86).
void SitDev_BindHeader(SitDevScripts* pScripts) {
    pScripts->p14 = (SitDevEntry*)((u8*)pScripts->p14 + (uptr)pScripts);
    pScripts->p18 = (SitDevAction*)((u8*)pScripts->p18 + (uptr)pScripts);
    pScripts->p1C = (SitDevEntry8*)((u8*)pScripts->p1C + (uptr)pScripts);
    pScripts->p20 = pScripts->p20 + (uptr)pScripts;
}

// Byte-swaps the scripts' header (nine words, layout gSitDevHeaderSwap) in place.
void SitDev_SwapHeader(void) {
    void* pSrc = gpSitDevScripts;
    void* pDst = gpSitDevScripts;
    ByteSwap_Records(&pSrc, &pDst, gSitDevHeaderSwap, 9, 1);
}

// Byte-swaps the scripts' tables in place: the situations (p14), actions (p18) and responses (p1C)
// by their layouts (gSitDevSituationSwap, gSitDevActionSwap, gSitDevResponseSwap), and the p20
// block. Then stores each situation's and response's b2 halfword back as its two bit-fields (the
// low 11 bits and the top 5).
void SitDev_SwapTables(void) {
    void* pSrc;
    void* pDst;
    SitDevEntry* pEntry;
    u32 i;
    SitDevEntry8* pEntry8;
    u16 uRaw;
    if (gpSitDevScripts->nEntries != 0) {
        pSrc = gpSitDevScripts->p14;
        pDst = gpSitDevScripts->p14;
        ByteSwap_Records(&pSrc, &pDst, gSitDevSituationSwap, 7, gpSitDevScripts->nEntries);
    }
    if (gpSitDevScripts->n04 != 0) {
        pSrc = gpSitDevScripts->p18;
        pDst = gpSitDevScripts->p18;
        ByteSwap_Records(&pSrc, &pDst, gSitDevActionSwap, 5, gpSitDevScripts->n04);
    }
    if (gpSitDevScripts->n08 != 0) {
        // fake match: pEntry8 carries the source pointer here (permuter find): it gives the i / pEntry8
        // registers of the second loop below
        pEntry8 = gpSitDevScripts->p1C;
        pSrc = pEntry8;
        pDst = gpSitDevScripts->p1C;
        ByteSwap_Records(&pSrc, &pDst, gSitDevResponseSwap, 4, gpSitDevScripts->n08);
    }
    if (gpSitDevScripts->n0C != 0) {
        pSrc = gpSitDevScripts->p20;
        pDst = gpSitDevScripts->p20;
        // EA bug: the byte count and the value width are swapped, and the address of pDst is
        // passed for pDst (the call is shaped like ByteSwap_Records's)
        BYTESWAP_SWAPDATA((u8**)&pSrc, (u8*)&pDst, 4, gpSitDevScripts->n0C * 4);
    }
    for (i = 0; i < gpSitDevScripts->nEntries; i++) {
        pEntry = &gpSitDevScripts->p14[i];
        uRaw = pEntry->b2.uRaw;
        pEntry->b2.s.n11 = uRaw & 0x7FF;
        gpSitDevScripts->p14[i].b2.s.n5 = (uRaw >> 11) & 0x1F;
    }
    for (i = 0; i < gpSitDevScripts->n08; i++) {
        pEntry8 = &gpSitDevScripts->p1C[i];
        uRaw = pEntry8->b2.uRaw;
        pEntry8->b2.s.n11 = uRaw & 0x7FF;
        gpSitDevScripts->p1C[i].b2.s.n5 = (uRaw >> 11) & 0x1F;
    }
}

// SitDevFile.c (EA's name, from its asserts; TW06's and TW07's SitDevFile.c): loading the
// situation scripts. SitDev_LoadScripts takes the hole stream's 'sscr' chunk: a header
// (SitDevHeader, gpSitDevScripts) and its tables, the situations (pSituations; TW07's Situation),
// their actions (pActions; Action), the responses (pResponses; Response) and 16-byte names
// (pNames). The chunk is little-endian: the first load byte-swaps it in place (the layouts below)
// and turns the header's offsets into pointers. The scripts' run-time state is the block
// gpSitDevData points at (SitDev.c).

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

SitDevHeader* gpSitDevScripts;    // the loaded scripts' header; NULL until the first load and
                                   // again after SitDev_vCloseModule

void SitDev_BeginLoadScripts(void);

// ---- scripts -------------------------------------------------------------------------------

void SitDev_SwapHeader(void);
void SitDev_BindHeader(SitDevHeader* pScripts);
void SitDev_SwapTables(void);

// The hole stream's 'sscr' chunk handler (SitDev_vRegisterStreamClients). Keeps the chunk
// (SitDevData.pCC, freed at round end); the first time, takes its first word as the scripts' header
// (gpSitDevScripts), byte-swaps the header (SitDev_SwapHeader), turns its offsets into pointers
// (SitDev_BindHeader) and byte-swaps the tables (SitDev_SwapTables). Then allocates the group flags
// (pGroupFlags, one byte per group, header nGroups) and clears them (SitDev_ClearGroupFlags).
void SitDev_LoadScripts(SitDevHeader** ppScripts) {
    SitDev_BeginLoadScripts();
    gpSitDevData->pCC = ppScripts;
    if (gpSitDevScripts == NULL) {
        gpSitDevScripts = *ppScripts;
        SitDev_SwapHeader();
        SitDev_BindHeader(gpSitDevScripts);
        SitDev_SwapTables();
    }
    gpSitDevData->pGroupFlags = StaticMem_Alloc(gpSitDevScripts->nGroups, 2, 16, "SitDevFile.c", 105);
    SitDev_ClearGroupFlags();
}

// Empty in this build; SitDev_LoadScripts calls it first.
void SitDev_BeginLoadScripts(void) {
}

// Turns the header's four table offsets (from the header's start) into pointers: the situations
// (pSituations), the actions (pActions), the responses (pResponses) and the names (pNames, 16 bytes
// each, state value 86).
void SitDev_BindHeader(SitDevHeader* pScripts) {
    pScripts->pSituations = (SitDevSituation*)((u8*)pScripts->pSituations + (uptr)pScripts);
    pScripts->pActions = (SitDevAction*)((u8*)pScripts->pActions + (uptr)pScripts);
    pScripts->pResponses = (SitDevResponse*)((u8*)pScripts->pResponses + (uptr)pScripts);
    pScripts->pNames = pScripts->pNames + (uptr)pScripts;
}

// Byte-swaps the scripts' header (nine words, layout gSitDevHeaderSwap) in place.
void SitDev_SwapHeader(void) {
    void* pSrc = gpSitDevScripts;
    void* pDst = gpSitDevScripts;
    ByteSwap_Records(&pSrc, &pDst, gSitDevHeaderSwap, 9, 1);
}

// Byte-swaps the scripts' tables in place: the situations (pSituations), actions (pActions) and
// responses (pResponses) by their layouts (gSitDevSituationSwap, gSitDevActionSwap,
// gSitDevResponseSwap), and the pNames block. Then stores each situation's and response's b2
// halfword back as its two bit-fields (the low 11 bits and the top 5).
void SitDev_SwapTables(void) {
    void* pSrc;
    void* pDst;
    SitDevSituation* pEntry;
    u32 i;
    SitDevResponse* pEntry8;
    u16 uRaw;
    if (gpSitDevScripts->nSituations != 0) {
        pSrc = gpSitDevScripts->pSituations;
        pDst = gpSitDevScripts->pSituations;
        ByteSwap_Records(&pSrc, &pDst, gSitDevSituationSwap, 7, gpSitDevScripts->nSituations);
    }
    if (gpSitDevScripts->nActions != 0) {
        pSrc = gpSitDevScripts->pActions;
        pDst = gpSitDevScripts->pActions;
        ByteSwap_Records(&pSrc, &pDst, gSitDevActionSwap, 5, gpSitDevScripts->nActions);
    }
    if (gpSitDevScripts->nResponses != 0) {
        // fake match: pEntry8 carries the source pointer here (permuter find): it gives the i / pEntry8
        // registers of the second loop below
        pEntry8 = gpSitDevScripts->pResponses;
        pSrc = pEntry8;
        pDst = gpSitDevScripts->pResponses;
        ByteSwap_Records(&pSrc, &pDst, gSitDevResponseSwap, 4, gpSitDevScripts->nResponses);
    }
    if (gpSitDevScripts->nNameWords != 0) {
        pSrc = gpSitDevScripts->pNames;
        pDst = gpSitDevScripts->pNames;
        // EA bug: the byte count and the value width are swapped, and the address of pDst is
        // passed for pDst (the call is shaped like ByteSwap_Records's)
        BYTESWAP_SWAPDATA((u8**)&pSrc, (u8*)&pDst, 4, gpSitDevScripts->nNameWords * 4);
    }
    for (i = 0; i < gpSitDevScripts->nSituations; i++) {
        pEntry = &gpSitDevScripts->pSituations[i];
        uRaw = pEntry->b2.uRaw;
        pEntry->b2.s.n11 = uRaw & 0x7FF;
        gpSitDevScripts->pSituations[i].b2.s.nFileIndex = (uRaw >> 11) & 0x1F;
    }
    for (i = 0; i < gpSitDevScripts->nResponses; i++) {
        pEntry8 = &gpSitDevScripts->pResponses[i];
        uRaw = pEntry8->b2.uRaw;
        pEntry8->b2.s.n11 = uRaw & 0x7FF;
        gpSitDevScripts->pResponses[i].b2.s.nFileIndex = (uRaw >> 11) & 0x1F;
    }
}

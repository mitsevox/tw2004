// SitDevFile.c (EA's name, from its asserts; TW06): a watcher that follows the ball after a shot
// (an event 48 frames in, a call when it reaches surface 105), the loading of the situation
// scripts into the block lbl_802811B8 points at (sitdev.h), and the values the scripts test.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8    lbl_80282200;     // 0x80282200  the watched ball has reached surface 105
Ball* lbl_802821FC;     // 0x802821FC  the watched ball, NULL for none
u32   lbl_802821F8;     // 0x802821F8  gSession.nFrameCount when it started

void fn_80067710(int nPlayer, int a, int b);   // also declared in Swing.c; belongs in a header
void fn_800BB4B0(void);

// ---- the watched ball ----------------------------------------------------------------------

void fn_800BB0DC(void);

void fn_800BB0A8(void) {
    fn_800BB0DC();
}

// Stop watching the ball.
void fn_800BB0C8(void) {
    lbl_802821FC = NULL;
}

// Whether the ball has reached surface 105 (see fn_800BB0E8).
u8 fn_800BB0D4(void) {
    return lbl_80282200;
}

void fn_800BB0DC(void) {
    lbl_80282200 = 0;
}

// Per frame while a ball is watched: 48 frames after it was hit, while it is flying or rolling,
// trigger event 0x4B for its player; the first time it lands on surface 105, call
// fn_80067710(player, 0, 0x22).
void fn_800BB0E8(void) {
    if (lbl_802821FC == NULL) return;
    switch (lbl_802821FC->nState) {
    case 2:
    case 3:
    case 4:
        if (48.0f == (f32)(gSession.nFrameCount - lbl_802821F8)) {
            EVENT_Trigger(lbl_802821FC->nPlayer, 0x4B, NULL, -1);
        }
        break;
    }
    if (lbl_802821FC->nSurface == 105 && !lbl_80282200) {
        fn_80067710(lbl_802821FC->nPlayer, 0, 0x22);
        lbl_80282200 = 1;
    }
}

// Start watching a ball: remember it and the frame it was hit on.
void fn_800BB1A8(Ball* pBall) {
    lbl_802821FC = pBall;
    lbl_802821F8 = gSession.nFrameCount;
}

void fn_800BB1C0(void) {
    int i;
    for (i = 0; i < 5; i++) {
        lbl_801FA198[i] = 0;
        lbl_801FA1AC[i] = 0;
    }
}

u8 fn_800BB1F8(int nPlayer) {
    return lbl_801FA1AC[nPlayer] == 1;
}

// ---- picking without repeats ---------------------------------------------------------------

// A list of nCount u16 values used as a deck: the top bit marks one already drawn, 0xFFF0 is an
// empty slot.

// The slots in use.
int fn_800BB218(u16* pList, int nCount) {
    int i;
    int nUsed = 0;
    for (i = 0; i < nCount; i++) {
        if (pList[i] != 0xFFF0) {
            nUsed++;
        }
    }
    return nUsed;
}

// The values not drawn yet; when every one is drawn, put them all back and return nCount.
int fn_800BB248(u16* pList, int nCount) {
    int nLeft = 0;
    int i;
    u16* p = pList;
    for (i = 0; i < nCount; i++) {
        if ((*pList & 0x8000) != 0x8000) {
            nLeft++;
        }
        pList++;
    }
    if (nLeft == 0) {
        i = nCount;
        while (i-- > 0) {
            *p &= 0x7FFF;
            p++;
        }
        return nCount;
    }
    return nLeft;
}

// Draw the nPick'th value not drawn yet (nLeft: fn_800BB248's count) and mark it drawn.
u32 fn_800BB334(u16* pList, int nCount, int nLeft, u32 nPick) {
    int i;
    u32 n = 0;
    for (i = 0; i < nCount; i++) {
        if (!(*pList & 0x8000)) {
            if (nPick == n) {
                nPick = *pList;
                *pList |= 0x8000;
                break;
            }
            n++;
        }
        pList++;
    }
    return nPick;
}

// The player's place in the PGA Tour event (1 outside mode 23) as a band: 0 for the top 3,
// 1 for the top 10, 2 for the top 25, 3 below.
int fn_800BB37C(int nPlayer) {
    int nRank;
    if (Game_GetMode() == 23) {
        nRank = fn_801190D8(nPlayer, 0);
    } else {
        nRank = 1;
    }
    if (nRank <= 3) return 0;
    if (nRank <= 10) return 1;
    return nRank <= 25 ? 2 : 3;
}

// The game mode's bit (lbl_801910F8), 0 for a mode without one.
u16 fn_800BB3F8(int nMode) {
    s32 nBit = lbl_801910F8[nMode];
    return nBit == -1 ? 0 : 1 << nBit;
}

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
    u32 i;
    void* pSrc;
    void* pDst;
    SitDevEntry* pEntry;
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
        pSrc = lbl_80282208->p1C;
        pDst = lbl_80282208->p1C;
        fn_8001F08C(&pSrc, &pDst, lbl_80191210, 4, lbl_80282208->n08);
    }
    if (lbl_80282208->n0C != 0) {
        pSrc = lbl_80282208->p20;
        pDst = lbl_80282208->p20;
        // EA bug: the byte count and the value width are swapped, and the address of pDst is
        // passed for pDst (the call is shaped like fn_8001F08C's)
        fn_80076158((u8**)&pSrc, (u8*)&pDst, 4, lbl_80282208->n0C * 4);
    }
    for (i = 0; i < lbl_80282208->nEntries; i++) {
        pEntry = &lbl_80282208->p14[i];
        uRaw = pEntry->b2.uRaw;
        pEntry->b2.s.n11 = uRaw;
        lbl_80282208->p14[i].b2.s.n5 = uRaw >> 11;
    }
    for (i = 0; i < lbl_80282208->n08; i++) {
        pEntry8 = &lbl_80282208->p1C[i];
        uRaw = pEntry8->b2.uRaw;
        pEntry8->b2.s.n11 = uRaw;
        lbl_80282208->p1C[i].b2.s.n5 = uRaw >> 11;
    }
}

// Course loader for chunk 5 of a hole: one more situation zone.
void fn_800BB6DC(u8* pChunk) {
    lbl_801FA1C0[lbl_80282210] = (SitDevZone*)pChunk;
    lbl_80282210++;
}

// The bits of every zone the point is in (0 when the hole has none).
u32 fn_800BB6FC(f32* pPos) {
    int i;
    u32 uBits = 0;
    if (lbl_80282210 == 0) return 0;
    for (i = 0; i < lbl_80282210; i++) {
        if (fn_8000C140(pPos, &lbl_801FA1C0[i]->net, lbl_801FA1C0[i]->net.nNumNodes)) {
            uBits |= *(u32*)lbl_801FA1C0[i]->aNodes[lbl_801FA1C0[i]->net.nNumNodes];
        }
    }
    return uBits;
}

u8 fn_800BB8A8(SitDevEntry* pEntry, int nTest, SitDevData* pData, int nValue);

// Whether every condition of the entry holds for the player. Value 86 is special: its argument
// picks one of the 16-byte names at p20, to compare with the golfer's.
u8 fn_800BB7AC(SitDevEntry* pEntry, SitDevData* pData, int nPlayer) {
    int nWord;
    int nBit;
    int nTest = 0;
    int nValue = 0;
    u8 bTrue;
    for (nWord = 0; nWord < 3; nWord++) {
        for (nBit = 0; nBit < 32; nBit++, nValue++) {
            if (pEntry->auTests[nWord] & (1 << nBit)) {
                if (nValue == 86) {
                    bTrue = strcmp(gPlayers[nPlayer].pChar->sz1614,
                                   (char*)lbl_80282208->p20 + pEntry->aArg[nTest++] * 16) == 0;
                } else {
                    bTrue = fn_800BB8A8(pEntry, nTest++, pData, nValue);
                }
                if (!bTrue) return 0;
            }
        }
    }
    return 1;
}

// Test nTest of the entry against value nValue (signed when lbl_80193188 says so).
u8 fn_800BB8A8(SitDevEntry* pEntry, int nTest, SitDevData* pData, int nValue) {
    if (lbl_80193188[nValue]) {
        switch (pEntry->aOp[nTest]) {
        case 0:
            return 1;
        case 1:
            return pEntry->aArg[nTest] == pData->aValue[nValue];
        case 2:
            return pEntry->aArg[nTest] != pData->aValue[nValue];
        case 3:
            return (s16)pData->aValue[nValue] > (s16)pEntry->aArg[nTest];
        case 4:
            return (s16)pData->aValue[nValue] < (s16)pEntry->aArg[nTest];
        case 5:
            return (pEntry->aArg[nTest] & pData->aValue[nValue]) != 0;
        }
    } else {
        switch (pEntry->aOp[nTest]) {
        case 0:
            return 1;
        case 1:
            return pEntry->aArg[nTest] == pData->aValue[nValue];
        case 2:
            return pEntry->aArg[nTest] != pData->aValue[nValue];
        case 3:
            return pData->aValue[nValue] > pEntry->aArg[nTest];
        case 4:
            return pData->aValue[nValue] < pEntry->aArg[nTest];
        case 5:
            return (pEntry->aArg[nTest] & pData->aValue[nValue]) != 0;
        }
    }
    return 0;
}

// Set value 5 of the shared block.
void fn_800BBADC(int nValue) {
    fn_80067B1C(lbl_802811B8->aValue, 5, (u16)nValue, lbl_802811B8->aSetBits);
}

// ---- the values the scripts test -----------------------------------------------------------

void fn_800BCA60(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer);
void fn_800BCB74(s32* pClass, int nSurface);
int  fn_800BCB88(void);
u8   fn_800BCBE0(void);
u8   fn_800BCC38(void);
u8   fn_800BCC48(void);
s32  fn_800BCCA0(int nPlayer);
s32  fn_800BCCCC(int nPlayer);
s32  fn_800BCCF8(int nPlayer);
u8   fn_800BCD24(int nPlayer);
u8   fn_800BCD50(void);
s32  fn_800BCD5C(void);
void fn_800BD77C(u16 uSound);
void fn_800BD7D0(u8 nMusic);
void fn_800BD7E8(u16 uSound);
void fn_800BD868(int nSound, int a);

// Correct the surface class of where a ball lies (SurfaceType.nClass) for the scripts: outside the
// course outline, or on ground a ball may not stay on, is 19 (not playable) unless it is water; a
// ball that must be dropped counts as water (7); surface 151 is 21; class 18 (green) reads as 12.
void fn_800BCA60(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer) {
    u8 bWater;
    u8 bNoLie;
    bWater = nSurface >= 0 && nSurface < NUM_SURFACE_TYPES &&
             (gSurfaceTypes[nSurface].nClass == 7 || gSurfaceTypes[nSurface].nClass == 16);
    bNoLie = nSurface >= 0 && nSurface < NUM_SURFACE_TYPES && !(gSurfaceTypes[nSurface].u34 & 1) &&
             (gSurfaceTypes[nSurface].u34 & 2);
    if ((!Ter_PointInOOBNetwork(pBall->vPos) && !bWater) || (bNoLie && !bWater)) {
        *pClass = 19;
    }
    if (pPlayer->b30E) {
        *pClass = 7;
    }
    if (nSurface == 151) {
        *pClass = 21;
    }
    if (*pClass == 18) {
        *pClass = 12;
    }
}

// Surface 151 is class 21 (the same test as in fn_800BCA60).
void fn_800BCB74(s32* pClass, int nSurface) {
    if (nSurface == 151) {
        *pClass = 21;
    }
}

// 1, 3 or 2 by which of lbl_802811F0's flags are set (fn_800BCC48, fn_800BCBE0, flag 0x2 alone),
// otherwise 0.
int fn_800BCB88(void) {
    int nResult;
    if (fn_800BCC48()) {
        nResult = 1;
    } else if (fn_800BCBE0()) {
        nResult = 3;
    } else if (fn_80035574()) {
        nResult = 2;
    } else {
        nResult = 0;
    }
    return nResult;
}

// Neither flag 0x2 nor b14 is set, and u04's flag 0x2 is.
u8 fn_800BCBE0(void) {
    int bResult = 0;
    if (!fn_80035574() && !lbl_802811F0->b14 && fn_800BCC38()) {
        bResult = 1;
    }
    return bResult;
}

u8 fn_800BCC38(void) {
    return lbl_802811F0->u04 & 2;
}

// Flag 0x2 is set, and u04's flag 0x2 is clear or b14 is set.
u8 fn_800BCC48(void) {
    int bResult = 0;
    if (fn_80035574() && (!fn_800BCC38() || lbl_802811F0->b14)) {
        bResult = 1;
    }
    return bResult;
}

// The game mode's answers for the scripts (GameState's callbacks).
s32 fn_800BCCA0(int nPlayer) {
    return gpGame->pfn204(nPlayer);
}

s32 fn_800BCCCC(int nPlayer) {
    return gpGame->pfn208(nPlayer);
}

s32 fn_800BCCF8(int nPlayer) {
    return gpGame->pfn200(nPlayer);
}

u8 fn_800BCD24(int nPlayer) {
    return gpGame->pfn1FC(nPlayer);
}

u8 fn_800BCD50(void) {
    return gpGame->bD4;
}

s32 fn_800BCD5C(void) {
    return gpGame->nDC;
}

// ---- running a script's actions ------------------------------------------------------------

u8   fn_800BCE70(SitDevAction* pAction, u8 nEvent);
u8   fn_800BCF84(SitDevAction* pAction, int nPlayer, u8 nEvent);
u8   fn_800BD3F8(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent);
void fn_800BD580(SitDevEntry8* pDo, int nPlayer, u8 nEvent);

// Try the entry's actions in order: each fires by its chance unless fn_800BD3F8 holds it back.
void fn_800BCD68(SitDevEntry* pEntry, int nSit, int nPlayer, u8 nEvent) {
    int i;
    SitDevAction* pAction;
    u8 bPlayed = 0;
    for (i = 0; i < 4; i++) {
        if (pEntry->aActions[i] == 0xFFF0) break;
        pAction = &lbl_80282208->p18[pEntry->aActions[i]];
        if (pAction->nChance > Rand_Next(1) % 100 && !fn_800BD3F8(pAction, nSit, nPlayer, nEvent)) {
            if (pAction->bSound) {
                bPlayed = fn_800BCE70(pAction, nEvent);
            } else {
                bPlayed = fn_800BCF84(pAction, nPlayer, nEvent);
            }
        }
    }
    if (lbl_80281E29 && !bPlayed) {
        lbl_80281E28 = 0;
    }
    lbl_80281E29 = 0;
}

// Play a sound drawn from the action's deck, once per kind; not while the GameBreaker holds the
// commentary back (then, for event 8, hand it to GameEffects for later).
u8 fn_800BCE70(SitDevAction* pAction, u8 nEvent) {
    int nCount;
    u32 nLeft;
    u32 nSound;
    int bNot30;
    if (lbl_802811B8->abPlayed[pAction->nKind]) return 0;
    nCount = fn_800BB218(pAction->aList, 50);
    nLeft = fn_800BB248(pAction->aList, nCount);
    nSound = fn_800BB334(pAction->aList, nCount, nLeft, Rand_Next(1) % nLeft);
    bNot30 = nEvent != 30;
    if (!fn_800DC784()) {
        fn_800BD83C((u16)nSound, bNot30);
        lbl_802811B8->abPlayed[pAction->nKind] = 1;
        return 1;
    }
    if (nEvent == 8) {
        fn_800BD77C(nSound);
    }
    return 0;
}

// Whether an action is held back: in modes with odd holes, or for a ball in the cup, at situations
// 21 and 22; for events 20 and 31 while the GameBreaker is up; and commentary (kinds 1 and 2)
// during a replay, in modes 6..8 and where fn_800E39F0 says so, and everywhere but mode 11.
u8 fn_800BD3F8(SitDevAction* pAction, int nSit, int nPlayer, u8 nEvent) {
    int nMode = Game_GetMode();
    if (nSit == 22 || nSit == 21) {
        if (gpGame->b136 || gpGame->b137 || gpGame->b138 || gpGame->b139) return 1;
        if (fn_800EC550()) {
            if (nSit == 21) return 1;
            if (nSit == 22 && gPlayers[nPlayer].ball.nLie != 0) return 1;
        }
    }
    if (lbl_80202898.bGameBreaker && (nEvent == 20 || (nEvent == 31 && nSit != 2))) return 1;
    if (pAction->nKind != 1 && pAction->nKind != 2) return 0;
    if (gSession.bReplay || (u32)(nMode - 6) <= 2 || fn_800E39F0()) return 1;
    if (!fn_800EC550() && nMode != 11) return 0;
    if (nMode == 11) return 1;
    return 0;
}

// Do one thing: 1 a commentary line (remembered in pE8), 11 and 10 sounds, 7 music, 4 a
// GameBreaker for a human player, 12 and 13 set the player's emotion results.
void fn_800BD580(SitDevEntry8* pDo, int nPlayer, u8 nEvent) {
    int bNot30;
    int nArg;
    switch (pDo->nKind) {
    case 1:
        if (gSession.options.a0[4]) {
            bNot30 = nEvent != 30;
            if (!fn_800DC784()) {
                fn_800BD83C((u16)pDo->n4, bNot30);
            }
            lbl_802811B8->pE8 = pDo;
        }
        break;
    case 11:
        if (gSession.options.a0[4] && !fn_800DC784()) {
            nArg = 2;
            if (nPlayer == 0) {
                nArg = 0;
            }
            fn_800BD868((u16)pDo->n4, nArg);
        }
        break;
    case 10:
        if (gSession.options.a0[4]) {
            fn_800BD7E8(pDo->n4);
        }
        break;
    case 7:
        if (fn_800DC784()) {
            fn_800BD7D0(pDo->n4);
        } else {
            fn_800A6DCC(pDo->n4, nEvent != 5);
        }
        break;
    case 4:
        if (Player_IsNotCPU(nPlayer)) {
            if (pDo->n4 == 0) {
                fn_800DBA50(nPlayer);
            } else {
                fn_800DB30C(nPlayer, pDo->n4);
            }
        }
        break;
    case 12:
        fn_8006AAB4(nPlayer, pDo->n4);
        lbl_801FA198[nPlayer] = 1;
        break;
    case 13:
        fn_8006ACE0(nPlayer, pDo->n4);
        lbl_801FA1AC[nPlayer] = 1;
        break;
    }
}

// Clear the scripts' per-entry bytes.
void fn_800BD74C(void) {
    u32 i;
    for (i = 0; i < lbl_80282208->n10; i++) {
        lbl_802811B8->pD4[i] = 0;
    }
}

// ---- sounds and music ----------------------------------------------------------------------

// Hand GameEffects a sound to stop later (u48), unless one is waiting already; not in mode 11.
void fn_800BD77C(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b47) {
        lbl_80202898.u48 = uSound;
        lbl_80202898.b47 = 1;
    }
}

// Tell GameEffects which music to go back to.
void fn_800BD7D0(u8 nMusic) {
    lbl_80202898.b4E = 1;
    lbl_80202898.n4F = nMusic;
}

// The same as fn_800BD77C with GameEffects' second slot (u4C).
void fn_800BD7E8(u16 uSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b4A) {
        lbl_80202898.u4C = uSound;
        lbl_80202898.b4A = 1;
    }
}

void fn_800BD83C(int nSound, int a) {
    fn_800A7664(0, nSound, a);
}

void fn_800BD868(int nSound, int a) {
    fn_800A7664(2, nSound, a);
}

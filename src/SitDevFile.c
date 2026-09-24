// SitDevFile.c (EA's name, from its asserts; TW06): a watcher that follows the ball after a shot
// (an event 48 frames in, a call when it reaches surface 105), the loading of the situation
// scripts into the block lbl_802811B8 points at (sitdev.h), and the values the scripts test.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8    lbl_80282200;     // 0x80282200  the watched ball has reached surface 105
Ball* lbl_802821FC;     // 0x802821FC  the watched ball, NULL for none
u32   lbl_802821F8;     // 0x802821F8  gSession.nFrameCount when it started

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
            uBits |= *(u32*)&lbl_801FA1C0[i]->net.aNodes[lbl_801FA1C0[i]->net.nNumNodes];
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
u8   fn_800BCD24(int nPlayer);
s32  fn_800BCD5C(void);
void fn_800BD77C(int nSound);
void fn_800BD7D0(u8 nMusic);
void fn_800BD7E8(u16 uSound);
void fn_800BD868(int nSound, int a);

// Whether n is a row of gSurfaceTypes (our name; EA's code has it inlined).
static inline int SurfaceType_IsValid(int n) {
    int bValid = 0;
    if (n >= 0 && n < NUM_SURFACE_TYPES) {
        bValid = 1;
    }
    return bValid;
}

// Fill in the values the scripts test for the player, for a situation of kind nKind: each kind
// sets its own values and falls through to the ones every later group needs (the round and the
// golfer, then the shot so far, then the ball and the hole).
void FE_GolferAttributes(int nPlayer, u8 nKind) {
    Player* pPlayer = &gPlayers[nPlayer];
    Ball* pBall = &pPlayer->ball;
    Ball* pBefore = &pPlayer->ballBefore;
    int nSurface = pBall->nSurface;
    int nBeforeSurface = pBefore->nSurface;
    u16* pValues = lbl_802811B8->aValue;
    u32* pSetBits = lbl_802811B8->aSetBits;
    u16 nRound = fn_800BCD5C();
    s32 nValue;
    u16 nMode;
    u16 nHole;
    int nDeg;
    f32 fAngle;

    lbl_802811B8->aSetBits[0] = 0;
    lbl_802811B8->aSetBits[1] = 0;
    lbl_802811B8->aSetBits[2] = 0;
    lbl_802811B8->abPlayed[0] = 0;
    lbl_802811B8->abPlayed[1] = 0;
    lbl_802811B8->abPlayed[2] = 0;
    lbl_802811B8->abPlayed[3] = 0;
    lbl_802811B8->abPlayed[4] = 0;
    lbl_802811B8->abPlayed[5] = 0;
    lbl_802811B8->abPlayed[6] = 0;
    lbl_802811B8->abPlayed[7] = 0;
    lbl_802811B8->abPlayed[8] = 0;
    lbl_802811B8->abPlayed[9] = 0;
    lbl_802811B8->abPlayed[10] = 0;
    lbl_802811B8->abPlayed[11] = 0;
    lbl_802811B8->abPlayed[12] = 0;
    lbl_802811B8->abPlayed[13] = 0;
    if (nKind == 25) {
        fn_80067B1C(pValues, 5, pPlayer->nClub, pSetBits);
    }
    switch (nKind) {
    case 0:
    case 1:
    case 12:
    case 13:
    case 22:
    case 23:
    case 24:
    case 26:
    case 30:
        nMode = Game_GetMode();
        fn_80067B1C(pValues, 1, nMode, pSetBits);
        fn_80067B1C(pValues, 54, fn_800BB3F8(nMode), pSetBits);
        fn_80067B1C(pValues, 60, fn_800E1734(), pSetBits);
        fn_80067B1C(pValues, 0, fn_80015464() + 1, pSetBits);
        fn_80067B1C(pValues, 30, Game_GetCourse(), pSetBits);
        fn_80067B1C(pValues, 83, GameModeDriverPGATour_GetCurrentEventID() + 1, pSetBits);
        fn_80067B1C(pValues, 62, nRound + 1, pSetBits);
        fn_80067B1C(pValues, 7, fn_800D2B08(), pSetBits);
        fn_80067B1C(pValues, 29, fn_800BCD50(), pSetBits);
        fn_80067B1C(pValues, 28, fn_8008AB4C(), pSetBits);
        fn_80067B1C(pValues, 31, fn_800D0AF4(), pSetBits);
        fn_80067B1C(pValues, 65, Game_CurrentPinSet(), pSetBits);
        fn_80067B1C(pValues, 85, 0, pSetBits);
        fn_80067B1C(pValues, 88, gSession.options.n20, pSetBits);
        fn_80067B1C(pValues, 89, gSession.options.n18, pSetBits);
        fn_80067B1C(pValues, 90, gSession.options.n1C, pSetBits);
        fn_80067B1C(pValues, 91, gSession.nTeeSet[nPlayer], pSetBits);
        fn_80067B1C(pValues, 92, gSession.options.nWind, pSetBits);
    case 2:
        nValue = nRound != 0 ? pPlayer->nRoundScore[nRound - 1] : 0;
        fn_80067B1C(pValues, 14, nValue, pSetBits);
        fn_80067B1C(pValues, 33, fn_800D07D8(nPlayer, 0), pSetBits);
        fn_80067B1C(pValues, 34, fn_800D089C(nPlayer, 0), pSetBits);
        fn_80067B1C(pValues, 35, fn_800D10B0(nPlayer), pSetBits);
        fn_80067B1C(pValues, 36, fn_800D1250(nPlayer), pSetBits);
        fn_80067B1C(pValues, 39, fn_800D0620(nPlayer, 0, 0), pSetBits);
        fn_80067B1C(pValues, 40, fn_800D06FC(nPlayer, 0, 0), pSetBits);
        fn_80067B1C(pValues, 41, fn_800D0FBC(nPlayer), pSetBits);
        fn_80067B1C(pValues, 42, fn_800D1170(nPlayer, 0), pSetBits);
        fn_80067B1C(pValues, 6, pPlayer->pChar->nSlot, pSetBits);
        fn_80067B1C(pValues, 13, Controller_IsNotCPU(pPlayer->nController), pSetBits);
        fn_80067B1C(pValues, 84, gSession.nGolfer[nPlayer] >= 30, pSetBits);
        fn_80067B1C(pValues, 61, gSession.nGolfer[nPlayer], pSetBits);
        if (gSession.nNumPlayers == 2) {
            // the other golfer
            if (nPlayer == 0) {
                fn_80067B1C(pValues, 45, gSession.nGolfer[1], pSetBits);
            } else if (nPlayer == 1) {
                fn_80067B1C(pValues, 45, gSession.nGolfer[0], pSetBits);
            }
        }
        // the attributes as signed bytes (fake match: the (s16) gives the original's extsb + clrlwi;
        // with (s8) alone CW drops the mask)
        fn_80067B1C(pValues, 68, (s16)(s8)Golfer_GetAttribute(pPlayer, 0, 2), pSetBits);
        fn_80067B1C(pValues, 69, (s16)(s8)Golfer_GetAttribute(pPlayer, 1, 2), pSetBits);
        fn_80067B1C(pValues, 70, (s16)(s8)Golfer_GetAttribute(pPlayer, 3, 2), pSetBits);
        fn_80067B1C(pValues, 71, (s16)(s8)Golfer_GetAttribute(pPlayer, 4, 2), pSetBits);
        fn_80067B1C(pValues, 72, (s16)(s8)Golfer_GetAttribute(pPlayer, 5, 2), pSetBits);
        fn_80067B1C(pValues, 73, (s16)(s8)Golfer_GetAttribute(pPlayer, 6, 2), pSetBits);
        fn_80067B1C(pValues, 74, (s16)(s8)Golfer_GetAttribute(pPlayer, 7, 2), pSetBits);
        fn_80067B1C(pValues, 77, (s16)(s8)Golfer_GetAttribute(pPlayer, 10, 2), pSetBits);
        fn_80067B1C(pValues, 78, (s16)(s8)Golfer_GetAttribute(pPlayer, 11, 2), pSetBits);
        fn_80067B1C(pValues, 93, fn_800E81A0(nPlayer), pSetBits);
    case 3:
    case 4:
    case 6:
    case 8:
    case 9:
    case 18:
    case 19:
    case 20:
    case 25:
    case 28:
    case 32:
    case 33:
    case 34:
        fn_80067B1C(pValues, 95, fn_800BB0D4(), pSetBits);
        fn_80067B1C(pValues, 43, pPlayer->b310 && !pPlayer->bMulliganUsed, pSetBits);
        fn_80067B1C(pValues, 44, pPlayer->b311 && !pPlayer->bMulliganUsed, pSetBits);
        fn_80067B1C(pValues, 49, fn_800BCD24(nPlayer), pSetBits);
        fn_80067B1C(pValues, 38, fn_800BCCF8(nPlayer), pSetBits);
        fn_80067B1C(pValues, 51, fn_800CF848(nPlayer), pSetBits);
        fn_80067B1C(pValues, 52, fn_800CF77C(nPlayer), pSetBits);
        fn_80067B1C(pValues, 37, fn_800E184C(nPlayer, 0), pSetBits);
        fn_80067B1C(pValues, 55, fn_800E1904(nPlayer, 0), pSetBits);
        // radians to degrees
        nDeg = 180.0f * fn_800D0960(nPlayer) / PI;
        fn_80067B1C(pValues, 17, nDeg, pSetBits);
        fn_80067B1C(pValues, 57, nDeg, pSetBits);
        // yards to inches
        fn_80067B1C(pValues, 16, (s32)(36.0f * (pPlayer->vTarget[1] - pBall->vStart[1])), pSetBits);
        fn_80067B1C(pValues, 66, fn_800D13F4(nPlayer), pSetBits);
        fn_80067B1C(pValues, 67, fn_800D1530(nPlayer), pSetBits);
        fn_80067B1C(pValues, 80, (f32)pPlayer->n2E0 >= 10.0f / 3.0f, pSetBits);
        fn_80067B1C(pValues, 81, pPlayer->n308 & 2, pSetBits);
        fn_80067B1C(pValues, 82, pPlayer->n308 & 1, pSetBits);
        fn_80067B1C(pValues, 47, fn_800CFD58(nPlayer), pSetBits);
        fn_80067B1C(pValues, 94, fn_800BB37C(nPlayer), pSetBits);
    case 5:
    case 7:
    case 15:
    case 16:
    case 21:
    case 27:
    case 29:
    case 31:
        nHole = Game_CurHoleIndex();
        fn_80067B1C(pValues, 46, fn_800CF904(nPlayer), pSetBits);
        fn_80067B1C(pValues, 48, fn_800BCCCC(nPlayer), pSetBits);
        fn_80067B1C(pValues, 50, fn_800BCCA0(nPlayer), pSetBits);
        fn_80067B1C(pValues, 53, Hole_ScoreAfterTapIn(nPlayer), pSetBits);
        fn_80067B1C(pValues, 10, pPlayer->nShotKind, pSetBits);
        if (nKind == 33) {
            lbl_802811B8->aValue[53]--;
        }
        fn_80067B1C(pValues, 56, fn_800CFFE4(nPlayer), pSetBits);
        fn_80067B1C(pValues, 8, pPlayer->nStrokes[nHole], pSetBits);
        fn_80067B1C(pValues, 9, pPlayer->nPutts[nHole], pSetBits);
        fn_80067B1C(pValues, 32, fn_800BCB88(), pSetBits);
        fn_80067B1C(pValues, 11, gSession.options.nC, pSetBits);
        fn_80067B1C(pValues, 12, (s32)Wind_Get(NULL), pSetBits);

        // the class of where the shot started
        nValue = SurfaceType_IsValid(pBall->nStartSurface) ?
                 gSurfaceTypes[pBall->nStartSurface].nClass : -1;
        fn_800BCB74(&nValue, pBall->nStartSurface);
        fn_80067B1C(pValues, 2, nValue, pSetBits);
        nValue = 36.0f * pPlayer->fA64;
        fn_80067B1C(pValues, 3, nValue, pSetBits);
        fn_80067B1C(pValues, 4, nValue, pSetBits);
        // the class of the ground aimed at
        nValue = SurfaceType_IsValid(pPlayer->nSurface) ? gSurfaceTypes[pPlayer->nSurface].nClass : -1;
        fn_80067B1C(pValues, 15, nValue, pSetBits);
        nValue = 36.0f * fn_800D0550(nPlayer);
        fn_80067B1C(pValues, 18, nValue, pSetBits);
        fn_80067B1C(pValues, 19, nValue, pSetBits);
        nValue = 36.0f * fn_800D0478(nPlayer);
        fn_80067B1C(pValues, 20, nValue, pSetBits);
        fn_80067B1C(pValues, 21, nValue, pSetBits);
        // the class of where the ball lies
        nValue = SurfaceType_IsValid(nSurface) ? gSurfaceTypes[pBall->nSurface].nClass : -1;
        fn_800BCA60(&nValue, pBall->nSurface, pBall, pPlayer);
        fn_80067B1C(pValues, 22, nValue, pSetBits);
        fn_80067B1C(pValues, 87, lbl_80281E28 && lbl_80282218 != nValue, pSetBits);
        fn_80067B1C(pValues, 25, (s32)(36.0f * pBall->fClosest), pSetBits);
        fn_80067B1C(pValues, 26, (s32)(36.0f * fn_800D04E0(nPlayer)), pSetBits);
        nValue = fn_800D0514(nPlayer);
        fn_800BCA60(&nValue, nBeforeSurface, pBefore, pPlayer);
        fn_80067B1C(pValues, 27, nValue, pSetBits);
        // the lie, in percent (fn_800510EC inlined)
        nValue = SurfaceType_IsValid(pBall->nStartSurface) ?
                 (u32)(100.0f * (pBall->f70 + gSurfaceTypes[pBall->nStartSurface].f00)) : 100;
        fn_80067B1C(pValues, 58, nValue, pSetBits);
        fn_80067B1C(pValues, 59, nValue, pSetBits);
        nValue = pBall->pHitSurface == NULL ? 0 : pBall->pHitSurface->nClass;
        fn_80067B1C(pValues, 63, nValue, pSetBits);
        nValue = pBefore->pHitSurface == NULL ? 0 : pBefore->pHitSurface->nClass;
        fn_80067B1C(pValues, 64, nValue, pSetBits);
        if (nKind == 29) {
            lbl_80282218 = nValue;
        }
        fn_80067B1C(pValues, 79, fn_800BB6FC(pBefore->vPos), pSetBits);
        fn_80067B1C(pValues, 75, fn_8005CB48(nPlayer), pSetBits);
        fn_80067B1C(pValues, 76, fn_8005CB60(nPlayer), pSetBits);
        fn_80067B1C(pValues, 23, (s32)(100.0f * fn_8005C1EC(nPlayer)), pSetBits);
        fAngle = 100.0f * fabsf(fn_8005C268(nPlayer));
        fn_80067B1C(pValues, 24, (s32)(fAngle / PI), pSetBits);
        break;
    }
}

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
    u8 bFlag;
    if (fn_800BCC48()) {
        nResult = 1;
    } else if (fn_800BCBE0()) {
        nResult = 3;
    } else {
        bFlag = fn_80035574();
        nResult = 0;
        if (bFlag) {
            nResult = 2;
        }
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
        if (pAction->nChance > Misc_RandFunc(1) % 100 && !fn_800BD3F8(pAction, nSit, nPlayer, nEvent)) {
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
    u32 nLeft;
    int nCount;
    u32 nSound;
    int bNot30;
    if (lbl_802811B8->abPlayed[pAction->nKind]) return 0;
    nCount = fn_800BB218(pAction->aList, 50);
    nLeft = fn_800BB248(pAction->aList, nCount);
    nSound = fn_800BB334(pAction->aList, nCount, nLeft, Misc_RandFunc(1) % nLeft);
    bNot30 = nEvent != 30;
    if (!fn_800DC784()) {
        fn_800BD83C(nSound, bNot30);
        lbl_802811B8->abPlayed[pAction->nKind] = 1;
        return 1;
    }
    if (nEvent == 8) {
        fn_800BD77C(nSound);
    }
    return 0;
}

// Run the action's p1C entries: one of each kind not played yet. A lone entry runs unless it is
// the last line played (except for event 30). Otherwise each kind draws one of its entries at
// random, skipping those already drawn (bit 15 of the list entry; when all are, the marks are
// cleared) and the last line played.
u8 fn_800BCF84(SitDevAction* pAction, int nPlayer, u8 nEvent) {
    s32 anCount[14];
    s32 aaIndex[14][50];
    SitDevEntry8* pDo;
    int nKind;
    int nPick;
    int nStart;
    int nEntry;
    int i;
    u8 bPlayed = 0;

    if (pAction->aList[1] == 0xFFF0) {
        pDo = &lbl_80282208->p1C[pAction->aList[0]];
        if (pDo == lbl_802811B8->pE8 && nEvent != 30) return 0;
        if (lbl_802811B8->abPlayed[pDo->nKind]) return 0;
        fn_800BD580(pDo, nPlayer, nEvent);
        lbl_802811B8->abPlayed[pDo->nKind] = 1;
        return 1;
    }
    for (i = 0; i < 14; i++) {
        anCount[i] = 0;
    }
    // Sort the entries not played yet by kind.
    for (i = 0; i < 50; i++) {
        if (pAction->aList[i] != 0xFFF0) {
            nKind = lbl_80282208->p1C[pAction->aList[i] & 0x7FFF].nKind;
            if (!lbl_802811B8->abPlayed[nKind]) {
                aaIndex[nKind][anCount[nKind]] = i;
                anCount[nKind]++;
            }
        }
    }
    for (nKind = 0; nKind < 14; nKind++) {
        if (anCount[nKind] == 1) {
            if (&lbl_80282208->p1C[pAction->aList[aaIndex[nKind][0]]] != lbl_802811B8->pE8) {
                fn_800BD580(&lbl_80282208->p1C[pAction->aList[aaIndex[nKind][0]]], nPlayer, nEvent);
                bPlayed = 1;
                lbl_802811B8->abPlayed[nKind] = 1;
            }
        } else if (anCount[nKind] > 1) {
            nPick = Misc_RandFunc(1) % anCount[nKind];
            nEntry = aaIndex[nKind][nPick];
            nStart = nPick;
            while (pAction->aList[nEntry] & 0x8000) {
                nPick++;
                if (nPick == anCount[nKind]) {
                    nPick = 0;
                }
                nEntry = aaIndex[nKind][nPick];
                if (nPick == nStart) {
                    // Every entry has been drawn: start the deck over.
                    for (i = 0; i < anCount[nKind]; i++) {
                        pAction->aList[aaIndex[nKind][i]] &= 0x7FFF;
                    }
                    nPick = nStart;
                    break;
                }
            }
            pDo = &lbl_80282208->p1C[pAction->aList[nEntry]];
            if (pDo == lbl_802811B8->pE8) {
                nPick++;
                if (nPick == anCount[nKind]) {
                    nPick = 0;
                }
                nEntry = aaIndex[nKind][nPick];
                pDo =&lbl_80282208->p1C[pAction->aList[nEntry] & 0x7FFF];
            }
            fn_800BD580(pDo, nPlayer, nEvent);
            lbl_802811B8->abPlayed[nKind] = 1;
            pAction->aList[nEntry] |= 0x8000;
            bPlayed = 1;
        }
    }
    return bPlayed;
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
    if (lbl_80202898.bGameBreaker && (nEvent == 20 || nEvent == 31) && nSit != 2) return 1;
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
void fn_800BD77C(int nSound) {
    if (Game_GetMode() != 11 && !lbl_80202898.b47) {
        lbl_80202898.u48 = nSound;
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

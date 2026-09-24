// SitDevStateVector.c (EA file, TW06/TW07): own unit, its .sbss starts 8-aligned at 0x80282218

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

u32 lbl_80282218;

u8  fn_800BB0D4(void);
int fn_800BB37C(int nPlayer);
u16 fn_800BB3F8(int nMode);
u32 SitDev_GetCommentaryZones(f32* pPos);

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

void modifyFinalLie(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer);
void fn_800BCB74(s32* pClass, int nSurface);
int  fn_800BCB88(void);
u8   fn_800BCBE0(void);
u8   fn_800BCC38(void);
u8   fn_800BCC48(void);
s32  fn_800BCCA0(int nPlayer);
u8   fn_800BCD24(int nPlayer);
s32  fn_800BCD5C(void);

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
void SitDev_SetupStateVector(int nPlayer, u8 nKind) {
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
    int i;

    for (i = 0; i < 3; i++) {
        lbl_802811B8->aSetBits[i] = 0;
    }
    for (i = 0; i < 14; i++) {
        lbl_802811B8->abPlayed[i] = 0;
    }
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
        fn_80067B1C(pValues, 1, (int)nMode, pSetBits);                  // fake match: (int) re-masks
        fn_80067B1C(pValues, 54, (int)fn_800BB3F8(nMode), pSetBits);    // fake match: (int) re-masks
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
        fn_80067B1C(pValues, 13, fn_8002E8E4(pPlayer->nController), pSetBits);
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
        // fake match: the original reloads the surface for the index (volatile at that one use)
        nValue = SurfaceType_IsValid(pBall->nStartSurface) ?
                 gSurfaceTypes[*(volatile s32*)&pBall->nStartSurface].nClass : -1;
        fn_800BCB74(&nValue, pBall->nStartSurface);
        fn_80067B1C(pValues, 2, nValue, pSetBits);
        nValue = 36.0f * pPlayer->fA64;
        fn_80067B1C(pValues, 3, nValue, pSetBits);
        fn_80067B1C(pValues, 4, nValue, pSetBits);
        // the class of the ground aimed at
        nValue = SurfaceType_IsValid(pPlayer->nSurface) ?
                 gSurfaceTypes[*(volatile s32*)&pPlayer->nSurface].nClass : -1;   // fake match: reload
        fn_80067B1C(pValues, 15, nValue, pSetBits);
        nValue = 36.0f * fn_800D0550(nPlayer);
        fn_80067B1C(pValues, 18, nValue, pSetBits);
        fn_80067B1C(pValues, 19, nValue, pSetBits);
        nValue = 36.0f * fn_800D0478(nPlayer);
        fn_80067B1C(pValues, 20, nValue, pSetBits);
        fn_80067B1C(pValues, 21, nValue, pSetBits);
        // the class of where the ball lies
        nValue = SurfaceType_IsValid(nSurface) ? gSurfaceTypes[pBall->nSurface].nClass : -1;
        modifyFinalLie(&nValue, pBall->nSurface, pBall, pPlayer);
        fn_80067B1C(pValues, 22, nValue, pSetBits);
        fn_80067B1C(pValues, 87, lbl_80281E28 && lbl_80282218 != nValue, pSetBits);
        fn_80067B1C(pValues, 25, (s32)(36.0f * pBall->fClosest), pSetBits);
        fn_80067B1C(pValues, 26, (s32)(36.0f * fn_800D04E0(nPlayer)), pSetBits);
        nValue = fn_800D0514(nPlayer);
        modifyFinalLie(&nValue, nBeforeSurface, pBefore, pPlayer);
        fn_80067B1C(pValues, 27, nValue, pSetBits);
        // the lie, in percent (Physics_GetLiePowerPercentage inlined)
        nValue = SurfaceType_IsValid(pBall->nStartSurface) ?
                 (u32)(100.0f * (pBall->f70 + gSurfaceTypes[*(volatile s32*)&pBall->nStartSurface].f00)) :
                 100;   // fake match: reload
        fn_80067B1C(pValues, 58, nValue, pSetBits);
        fn_80067B1C(pValues, 59, nValue, pSetBits);
        nValue = pBall->pHitSurface == NULL ? 0 : pBall->pHitSurface->nClass;
        fn_80067B1C(pValues, 63, nValue, pSetBits);
        nValue = pBefore->pHitSurface == NULL ? 0 : pBefore->pHitSurface->nClass;
        fn_80067B1C(pValues, 64, nValue, pSetBits);
        if (nKind == 29) {
            lbl_80282218 = nValue;
        }
        fn_80067B1C(pValues, 79, SitDev_GetCommentaryZones(pBefore->vPos), pSetBits);
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
void modifyFinalLie(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer) {
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

// Surface 151 is class 21 (the same test as in modifyFinalLie).
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

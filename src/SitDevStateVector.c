// SitDevStateVector.c (EA's name: TW06's and TW07's SitDevStateVector.c; TW07 has
// SitDev_ConditionsMatch, SitDev_SetupClubCondition, SitDev_SetupStateVector, modifyFinalLie and
// modifyInitialLie in this order): the state vector, the values the scripts test (the round, the
// golfer, the shot, the ball, the hole), filled in at each frame's first situation event, and the
// test of a situation's conditions against it. Its tail: the weather-change value and the game
// mode's answers the vector asks for (GM_ functions TW07 has as inlines). Own unit; its .sbss
// starts 8-aligned at 0x80282218.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "sitdev.h"
#include "game.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

u32 gSitDevPredictedHitClass;   // at situation event 29 (the look-ahead ball's first bounce): the
                                // class of what that ball hit (state value 64)

u8  SitDev_GetCupBevelFlag(void);
int SitDev_GetPGARank(int nPlayer);
u16 SitDev_TranslateGameMode(int nMode);
u32 SitDev_GetCommentaryZones(f32* pPos);

u8 SitDev_CompareConditions(SitDevSituation* pEntry, int nTest, SitDevData* pData, int nValue);

// Whether every condition of a situation holds for the player: for each state value n whose bit is
// set in auTests, in order, the situation's next test compares value n (SitDev_CompareConditions).
// Value 86 is special: its argument picks one of the 16-byte names at pNames, which must be the
// golfer's last animation clip (Character.szLastClip).
u8 SitDev_ConditionsMatch(SitDevSituation* pEntry, SitDevData* pData, int nPlayer) {
    int nWord;
    int nBit;
    int nTest = 0;
    int nValue = 0;
    u8 bTrue;
    for (nWord = 0; nWord < 3; nWord++) {
        for (nBit = 0; nBit < 32; nBit++, nValue++) {
            if (pEntry->auTests[nWord] & (1 << nBit)) {
                if (nValue == 86) {
                    bTrue = strcmp(gPlayers[nPlayer].pChar->szLastClip,
                                   (char*)gpSitDevScripts->pNames + pEntry->aArg[nTest++] * 16) == 0;
                } else {
                    bTrue = SitDev_CompareConditions(pEntry, nTest++, pData, nValue);
                }
                if (!bTrue) return 0;
            }
        }
    }
    return 1;
}

// Test nTest of the situation against state value nValue, by the test's operator (aOp): 0 always
// true, 1 equal to the argument, 2 not equal, 3 the value greater, 4 the value less, 5 any bit in
// common. Values lbl_80193188 marks are compared as signed 16-bit. An unknown operator fails.
u8 SitDev_CompareConditions(SitDevSituation* pEntry, int nTest, SitDevData* pData, int nValue) {
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

// Sets state value 5, the club, as a golfer's club is chosen (Character_SelectGameClub).
void SitDev_SetupClubCondition(int nValue) {
    _SetStateVecAndCondition(gpSitDevData->aValue, 5, (u16)nValue, gpSitDevData->aSetBits);
}

// ---- the values the scripts test -----------------------------------------------------------

void modifyFinalLie(s32* pClass, int nSurface, Ball* pBall, Player* pPlayer);
void modifyInitialLie(s32* pClass, int nSurface);
int  SitDev_GetWeatherChangeCondition(void);
u8   SitDev_WeatherEffectEnded(void);
u8   SitDev_WeatherEffectWasOn(void);
u8   SitDev_WeatherEffectBegan(void);
s32  GM_GetPotentialEventLead(int nPlayer);
u8   GM_IsPuttForWin(int nPlayer);
s32  GM_GetCurrentRound(void);

// Whether n is a row of gSurfaceTypes (our name; EA's code has it inlined).
static inline int SurfaceType_IsValid(int n) {
    int bValid = 0;
    if (n >= 0 && n < NUM_SURFACE_TYPES) {
        bValid = 1;
    }
    return bValid;
}

// Fills in the state values the scripts test, for the player at situation event nKind (the first
// event SitDev_QueueEvent queues in a frame). Every value's set bit and the per-kind played bytes
// (abPlayed) are cleared first. Each group of events sets its own values and falls through to the
// ones every later group needs: the round and the golfer, then the shot so far, then the ball and
// the hole.
void SitDev_SetupStateVector(int nPlayer, u8 nKind) {
    Player* pPlayer = &gPlayers[nPlayer];
    Ball* pBall = &pPlayer->ball;
    Ball* pBefore = &pPlayer->ballBefore;
    int nSurface = pBall->nSurface;
    int nBeforeSurface = pBefore->nSurface;
    u16* pValues = gpSitDevData->aValue;
    u32* pSetBits = gpSitDevData->aSetBits;
    u16 nRound = GM_GetCurrentRound();
    s32 nValue;
    u16 nMode;
    u16 nHole;
    int nDeg;
    f32 fAngle;
    int i;

    for (i = 0; i < 3; i++) {
        gpSitDevData->aSetBits[i] = 0;
    }
    for (i = 0; i < 14; i++) {
        gpSitDevData->abPlayed[i] = 0;
    }
    if (nKind == 25) {
        _SetStateVecAndCondition(pValues, 5, pPlayer->nClub, pSetBits);
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
        // fake match: the (int) casts of the next two values re-mask them
        _SetStateVecAndCondition(pValues, 1, (int)nMode, pSetBits);
        _SetStateVecAndCondition(pValues, 54, (int)SitDev_TranslateGameMode(nMode), pSetBits);
        _SetStateVecAndCondition(pValues, 60, GM_OnFirstSelectedHole(), pSetBits);
        _SetStateVecAndCondition(pValues, 0, Game_GetCurHoleNum() + 1, pSetBits);
        _SetStateVecAndCondition(pValues, 30, Game_GetCourse(), pSetBits);
        _SetStateVecAndCondition(pValues, 83, GameModeDriverPGATour_GetCurrentEventID() + 1, pSetBits);
        _SetStateVecAndCondition(pValues, 62, nRound + 1, pSetBits);
        _SetStateVecAndCondition(pValues, 7, GM_GetCurrentHolePar(), pSetBits);
        _SetStateVecAndCondition(pValues, 29, GM_IsPlayoff(), pSetBits);
        _SetStateVecAndCondition(pValues, 28, GM_GetNumHolesInRound(), pSetBits);
        _SetStateVecAndCondition(pValues, 31, fn_800D0AF4(), pSetBits);
        _SetStateVecAndCondition(pValues, 65, Game_CurrentPinSet(), pSetBits);
        _SetStateVecAndCondition(pValues, 85, 0, pSetBits);
        _SetStateVecAndCondition(pValues, 88, gSession.options.nFairwaySpeed, pSetBits);
        _SetStateVecAndCondition(pValues, 89, gSession.options.nGreenSpeed, pSetBits);
        _SetStateVecAndCondition(pValues, 90, gSession.options.nRough, pSetBits);
        _SetStateVecAndCondition(pValues, 91, gSession.nTeeSet[nPlayer], pSetBits);
        _SetStateVecAndCondition(pValues, 92, gSession.options.nWind, pSetBits);
    case 2:
        nValue = nRound != 0 ? pPlayer->nRoundScore[nRound - 1] : 0;
        _SetStateVecAndCondition(pValues, 14, nValue, pSetBits);
        _SetStateVecAndCondition(pValues, 33, fn_800D07D8(nPlayer, 0), pSetBits);
        _SetStateVecAndCondition(pValues, 34, fn_800D089C(nPlayer, 0), pSetBits);
        _SetStateVecAndCondition(pValues, 35, fn_800D10B0(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 36, fn_800D1250(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 39, fn_800D0620(nPlayer, 0, 0), pSetBits);
        _SetStateVecAndCondition(pValues, 40, fn_800D06FC(nPlayer, 0, 0), pSetBits);
        _SetStateVecAndCondition(pValues, 41, fn_800D0FBC(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 42, fn_800D1170(nPlayer, 0), pSetBits);
        _SetStateVecAndCondition(pValues, 6, pPlayer->pChar->nSlot, pSetBits);
        _SetStateVecAndCondition(pValues, 13, fn_8002E8E4(pPlayer->nController), pSetBits);
        _SetStateVecAndCondition(pValues, 84, gSession.nGolfer[nPlayer] >= 30, pSetBits);
        _SetStateVecAndCondition(pValues, 61, gSession.nGolfer[nPlayer], pSetBits);
        if (gSession.nNumPlayers == 2) {
            // the other golfer
            if (nPlayer == 0) {
                _SetStateVecAndCondition(pValues, 45, gSession.nGolfer[1], pSetBits);
            } else if (nPlayer == 1) {
                _SetStateVecAndCondition(pValues, 45, gSession.nGolfer[0], pSetBits);
            }
        }
        // the attributes as signed bytes (fake match: the (s16) gives the original's extsb + clrlwi;
        // with (s8) alone CW drops the mask)
        _SetStateVecAndCondition(pValues, 68, (s16)(s8)Golfer_GetAttribute(pPlayer, 0, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 69, (s16)(s8)Golfer_GetAttribute(pPlayer, 1, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 70, (s16)(s8)Golfer_GetAttribute(pPlayer, 3, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 71, (s16)(s8)Golfer_GetAttribute(pPlayer, 4, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 72, (s16)(s8)Golfer_GetAttribute(pPlayer, 5, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 73, (s16)(s8)Golfer_GetAttribute(pPlayer, 6, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 74, (s16)(s8)Golfer_GetAttribute(pPlayer, 7, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 77, (s16)(s8)Golfer_GetAttribute(pPlayer, 10, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 78, (s16)(s8)Golfer_GetAttribute(pPlayer, 11, 2), pSetBits);
        _SetStateVecAndCondition(pValues, 93, GameModeBattle_GetClubLostOnLastHole(nPlayer), pSetBits);
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
        _SetStateVecAndCondition(pValues, 95, SitDev_GetCupBevelFlag(), pSetBits);
        _SetStateVecAndCondition(pValues, 43, pPlayer->bBunkerThisHole && !pPlayer->bMulliganUsed, pSetBits);
        _SetStateVecAndCondition(pValues, 44, pPlayer->b311 && !pPlayer->bMulliganUsed, pSetBits);
        _SetStateVecAndCondition(pValues, 49, GM_IsPuttForWin(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 38, GM_GetCurrentEventLead(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 51, fn_800CF848(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 52, fn_800CF77C(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 37, GM_GetGolferRelativeCurrentScore(nPlayer, 0), pSetBits);
        _SetStateVecAndCondition(pValues, 55, GM_GetGolferRelativeCumulativeScore(nPlayer, 0), pSetBits);
        // radians to degrees
        nDeg = 180.0f * fn_800D0960(nPlayer) / PI;
        _SetStateVecAndCondition(pValues, 17, nDeg, pSetBits);
        _SetStateVecAndCondition(pValues, 57, nDeg, pSetBits);
        // yards to inches
        _SetStateVecAndCondition(pValues, 16, (s32)(36.0f * (pPlayer->vTarget[1] - pBall->vStart[1])),
                                 pSetBits);
        _SetStateVecAndCondition(pValues, 66, fn_800D13F4(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 67, fn_800D1530(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 80, (f32)pPlayer->nLongestPutt >= 10.0f / 3.0f, pSetBits);
        _SetStateVecAndCondition(pValues, 81, pPlayer->n308 & 2, pSetBits);
        _SetStateVecAndCondition(pValues, 82, pPlayer->n308 & 1, pSetBits);
        _SetStateVecAndCondition(pValues, 47, fn_800CFD58(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 94, SitDev_GetPGARank(nPlayer), pSetBits);
    case 5:
    case 7:
    case 15:
    case 16:
    case 21:
    case 27:
    case 29:
    case 31:
        nHole = Game_CurHoleIndex();
        _SetStateVecAndCondition(pValues, 46, fn_800CF904(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 48, GM_GetPotentialHoleResult(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 50, GM_GetPotentialEventLead(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 53, Hole_ScoreAfterTapIn(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 10, pPlayer->nShotKind, pSetBits);
        if (nKind == 33) {
            gpSitDevData->aValue[53]--;
        }
        _SetStateVecAndCondition(pValues, 56, fn_800CFFE4(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 8, pPlayer->nStrokes[nHole], pSetBits);
        _SetStateVecAndCondition(pValues, 9, pPlayer->nPutts[nHole], pSetBits);
        _SetStateVecAndCondition(pValues, 32, SitDev_GetWeatherChangeCondition(), pSetBits);
        _SetStateVecAndCondition(pValues, 11, gSession.options.nWeather, pSetBits);
        _SetStateVecAndCondition(pValues, 12, (s32)Wind_GetPhysicsWindVelocity(NULL), pSetBits);

        // the class of where the shot started
        // fake match: the original reloads the surface for the index (volatile at that one use)
        nValue = SurfaceType_IsValid(pBall->nStartSurface) ?
                 gSurfaceTypes[*(volatile s32*)&pBall->nStartSurface].nClass : -1;
        modifyInitialLie(&nValue, pBall->nStartSurface);
        _SetStateVecAndCondition(pValues, 2, nValue, pSetBits);
        nValue = 36.0f * pPlayer->fA64;
        _SetStateVecAndCondition(pValues, 3, nValue, pSetBits);
        _SetStateVecAndCondition(pValues, 4, nValue, pSetBits);
        // the class of the ground aimed at
        nValue = SurfaceType_IsValid(pPlayer->nSurface) ?
                 gSurfaceTypes[*(volatile s32*)&pPlayer->nSurface].nClass : -1;   // fake match: reload
        _SetStateVecAndCondition(pValues, 15, nValue, pSetBits);
        nValue = 36.0f * fn_800D0550(nPlayer);
        _SetStateVecAndCondition(pValues, 18, nValue, pSetBits);
        _SetStateVecAndCondition(pValues, 19, nValue, pSetBits);
        nValue = 36.0f * fn_800D0478(nPlayer);
        _SetStateVecAndCondition(pValues, 20, nValue, pSetBits);
        _SetStateVecAndCondition(pValues, 21, nValue, pSetBits);
        // the class of where the ball lies
        nValue = SurfaceType_IsValid(nSurface) ? gSurfaceTypes[pBall->nSurface].nClass : -1;
        modifyFinalLie(&nValue, pBall->nSurface, pBall, pPlayer);
        _SetStateVecAndCondition(pValues, 22, nValue, pSetBits);
        _SetStateVecAndCondition(pValues, 87, gSitDevPredictionVoiced && gSitDevPredictedHitClass != nValue,
                                 pSetBits);
        _SetStateVecAndCondition(pValues, 25, (s32)(36.0f * pBall->fClosest), pSetBits);
        _SetStateVecAndCondition(pValues, 26, (s32)(36.0f * fn_800D04E0(nPlayer)), pSetBits);
        nValue = fn_800D0514(nPlayer);
        modifyFinalLie(&nValue, nBeforeSurface, pBefore, pPlayer);
        _SetStateVecAndCondition(pValues, 27, nValue, pSetBits);
        // the lie, in percent (Physics_GetLiePowerPercentage inlined)
        nValue = SurfaceType_IsValid(pBall->nStartSurface) ?
                 (u32)(100.0f * (pBall->f70 + gSurfaceTypes[*(volatile s32*)&pBall->nStartSurface].f00)) :
                 100;   // fake match: reload
        _SetStateVecAndCondition(pValues, 58, nValue, pSetBits);
        _SetStateVecAndCondition(pValues, 59, nValue, pSetBits);
        nValue = pBall->pHitSurface == NULL ? 0 : pBall->pHitSurface->nClass;
        _SetStateVecAndCondition(pValues, 63, nValue, pSetBits);
        nValue = pBefore->pHitSurface == NULL ? 0 : pBefore->pHitSurface->nClass;
        _SetStateVecAndCondition(pValues, 64, nValue, pSetBits);
        if (nKind == 29) {
            gSitDevPredictedHitClass = nValue;
        }
        _SetStateVecAndCondition(pValues, 79, SitDev_GetCommentaryZones(pBefore->vPos), pSetBits);
        _SetStateVecAndCondition(pValues, 75, SW_fGetBoostMagnitude(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 76, SW_fGetSpinMagnitude(nPlayer), pSetBits);
        _SetStateVecAndCondition(pValues, 23, (s32)(100.0f * SW_vGetHookSlice(nPlayer)), pSetBits);
        fAngle = 100.0f * fabsf(SW_vGetMishitAngle(nPlayer));
        _SetStateVecAndCondition(pValues, 24, (s32)(fAngle / PI), pSetBits);
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

// The start-of-shot counterpart of modifyFinalLie: surface 151 is class 21.
void modifyInitialLie(s32* pClass, int nSurface) {
    if (nSurface == 151) {
        *pClass = 21;
    }
}

// State value 32: how the hole's weather effect bit 1 (lbl_802811F0 flag 0x2, fn_80035574; rolled
// per hole by fn_8006F650) changed from the last hole: 1 it began (SitDev_WeatherEffectBegan), 3 it
// ended (SitDev_WeatherEffectEnded), 2 it goes on, 0 it is off.
int SitDev_GetWeatherChangeCondition(void) {
    int nResult;
    u8 bFlag;
    if (SitDev_WeatherEffectBegan()) {
        nResult = 1;
    } else if (SitDev_WeatherEffectEnded()) {
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

// Whether weather effect bit 1 ended with this hole: off now, on for the last hole
// (SitDev_WeatherEffectWasOn), and b14 clear (set at round start, cleared by the first normal
// weather pick).
u8 SitDev_WeatherEffectEnded(void) {
    int bResult = 0;
    if (!fn_80035574() && !lbl_802811F0->b14 && SitDev_WeatherEffectWasOn()) {
        bResult = 1;
    }
    return bResult;
}

// Whether weather effect bit 1 was on for the last hole (u04, the flags fn_8006FBF8 kept).
u8 SitDev_WeatherEffectWasOn(void) {
    return lbl_802811F0->u04 & 2;
}

// Whether weather effect bit 1 began with this hole: on now, and off for the last hole
// (SitDev_WeatherEffectWasOn) or b14 still set (no normal weather pick since the round began).
u8 SitDev_WeatherEffectBegan(void) {
    int bResult = 0;
    if (fn_80035574() && (!SitDev_WeatherEffectWasOn() || lbl_802811F0->b14)) {
        bResult = 1;
    }
    return bResult;
}

// The mode's lead for the player if this ball drops (gpGame->pfnGetPotentialLead): state value 50.
s32 GM_GetPotentialEventLead(int nPlayer) {
    return gpGame->pfnGetPotentialLead(nPlayer);
}

// How the hole ends for the player if this ball drops (gpGame->pfnGetPotentialHoleResult): state
// value 48; HoleScore.c asks it too.
s32 GM_GetPotentialHoleResult(int nPlayer) {
    return gpGame->pfnGetPotentialHoleResult(nPlayer);
}

// The mode's lead for the player so far (gpGame->pfnGetCurrentLead): state value 38; HoleScore.c
// asks it too.
s32 GM_GetCurrentEventLead(int nPlayer) {
    return gpGame->pfnGetCurrentLead(nPlayer);
}

// Whether holing this ball wins (gpGame->pfnIsPuttForWin): state value 49, and a GameBreaker test
// in GameEffects.c.
u8 GM_IsPuttForWin(int nPlayer) {
    return gpGame->pfnIsPuttForWin(nPlayer);
}

// Whether a playoff is being played (gpGame->bInPlayoff): state value 29; HoleScore.c asks it too.
u8 GM_IsPlayoff(void) {
    return gpGame->bInPlayoff;
}

// The tour event's current round, from 0 (gpGame->nDC): SitDev_SetupStateVector sets value 62 to it
// plus 1 and value 14 to the score of the round before.
s32 GM_GetCurrentRound(void) {
    return gpGame->nDC;
}

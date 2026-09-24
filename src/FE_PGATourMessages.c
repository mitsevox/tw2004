// FE_PGATourMessages.c (EA's name, from its asserts): the PGA TOUR mode's menu message handlers:
// the tournament leaderboard, the season schedule, the season wrap-up, the sponsor offers and
// the player's wins. The first functions (to fn_8010E4DC) blend a character model's sliders; they
// sit in this unit until the file boundary there is proven.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "character.h"
#include "charstate.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"

void fn_8011CADC(Skin* pSkin, int nMorph, f32 fWeight);    // SkinMorph.c
void fn_800736D8(u8* pNode, int n);     // animblender.c
u8   fn_80118F60(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat);   // PGATourSimulation.c

s32* lbl_802824B0;                      // the tournaments on the schedule (fn_8010EA24)
s32 lbl_802824B4;                       // and how many there are

// The sponsor offers' asset kinds (11), picked at random.
s16 lbl_80193CFC[11] = { 0, 1, 2, 5, 6, 9, 10, 11, 13, 14, 15 };

// The index of the slider whose id is nId, or -1.
int fn_8010DC94(CharSliderDefs* pDefs, s32 nId) {
    int i;

    if (pDefs == NULL) {
        return -1;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        if (nId == pDefs->pDefs[i].nId) {
            return i;
        }
    }
    return -1;
}

// Every slider back to the range 0..1 at 0.
void fn_8010DCE8(CharSliderDefs* pDefs) {
    int i;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pDefs->pValues[i].fLow = 0.0f;
            pDefs->pValues[i].fHigh = 1.0f;
            pDefs->pValues[i].fValue = 0.0f;
            pDefs->pValues[i].bFixed = 0;
        }
    }
}

// Set the sliders with ids 0..nSliders-1 from percentages.
void fn_8010DD4C(CharSliderDefs* pDefs, int nSliders, u8* aValues) {
    int i;
    int n;

    for (i = 0; i < nSliders; i++) {
        n = fn_8010DC94(pDefs, i);
        if (n >= 0) {
            pDefs->pValues[n].fValue = (s8)aValues[i] / 100.0f;
        }
    }
}

// Keep every value in 0..0.99999.
void fn_8010DE04(CharSliderDefs* pDefs) {
    int i;
    CharSliderValue* pValue;

    for (i = 0; i < pDefs->nSliders; i++) {
        pValue = &pDefs->pValues[i];
        if (pValue->fValue <= 0.0f) {
            pValue->fValue = 0.0f;
        }
        if (pValue->fValue >= 0.99999f) {
            pValue->fValue = 0.99999f;
        }
    }
}

// Cut each pair of sliders that share a length back to it.
void fn_8010DE60(CharSliderDefs* pDefs) {
    CharSliderValue* pValue;
    CharSliderDef* pDef;
    CharSliderLimit* pLimit;
    CharSliderValue* pOther;
    f32 fLength;
    int i;
    int j;
    int n;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pValue = &pDefs->pValues[i];
            pDef = &pDefs->pDefs[i];
            if (pValue->bFixed != 1 && pDef->nLimits > 0) {
                for (j = 0; j < pDef->nLimits; j++) {
                    pLimit = &pDef->pLimits[j];
                    n = fn_8010DC94(pDefs, pLimit->nSlider);
                    if (n >= 0) {
                        pOther = &pDefs->pValues[n];
                        if (pOther->bFixed == 0) {
                            fLength = fn_80009680(pValue->fValue * pValue->fValue
                                                  + pOther->fValue * pOther->fValue);
                            if (fLength > pLimit->fLength && fLength != 0.0f) {
                                pValue->fValue = pLimit->fLength * (pValue->fValue / fLength);
                                pOther->fValue = pLimit->fLength * (pOther->fValue / fLength);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Let each slider move the ranges of the sliders it links to, keeping their values at the same
// place in their ranges.
void fn_8010DF8C(CharSliderDefs* pDefs) {
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderLink* pLink;
    CharSliderValue* pOther;
    f32 fT;
    f32 fSpan;
    f32 fPlace;
    u8 bMove;
    int i;
    int j;
    int n;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pDef = &pDefs->pDefs[i];
            pValue = &pDefs->pValues[i];
            if (pDef->nLinks > 0) {
                for (j = 0; j < pDef->nLinks; j++) {
                    pLink = &pDef->pLinks[j];
                    n = fn_8010DC94(pDefs, pLink->nSlider);
                    if (n >= 0) {
                        fT = 0.0f;
                        bMove = 0;
                        fSpan = fT;
                        if (pLink->fFrom < pLink->fTo) {
                            if (pValue->fValue <= pLink->fTo && pValue->fValue >= pLink->fFrom) {
                                fSpan = pLink->fTo - pLink->fFrom;
                                bMove = 1;
                                fT = (pValue->fValue - pLink->fFrom) / fSpan;
                            }
                        } else if (pLink->fFrom > pLink->fTo) {
                            if (pValue->fValue <= pLink->fFrom && pValue->fValue >= pLink->fTo) {
                                fSpan = pLink->fFrom - pLink->fTo;
                                bMove = 1;
                                fT = 1.0f - (pValue->fValue - pLink->fTo) / fSpan;
                            }
                        }
                        if (bMove) {
                            pOther = &pDefs->pValues[n];
                            if (pOther->fHigh != pOther->fLow) {
                                fPlace = (pOther->fValue - pOther->fLow) / (pOther->fHigh - pOther->fLow);
                            } else {
                                fPlace = pOther->fValue;
                            }
                            if (pLink->uFlags & 1) {
                                pOther->fLow = fT * fSpan + pOther->fLow;
                                pOther->fLow = pOther->fLow < 0.0f ? 0.0f
                                             : pOther->fLow > 1.0f ? 1.0f : pOther->fLow;
                            } else if (pLink->uFlags & 2) {
                                pOther->fHigh = fT * fSpan + pOther->fHigh;
                                pOther->fHigh = pOther->fHigh < 0.0f ? 0.0f
                                              : pOther->fHigh > 1.0f ? 1.0f : pOther->fHigh;
                            }
                            pOther->fValue = fPlace * (pOther->fHigh - pOther->fLow) + pOther->fLow;
                        }
                    }
                }
            }
        }
    }
}

// fX's place between fFrom and fTo (0..1, either way round), as a blend of fA to fB.
f32 fn_8010E194(f32 fFrom, f32 fTo, f32 fX, f32 fA, f32 fB) {
    f32 fT;

    if (fFrom == fTo) {
        return 0.0f;
    }
    if (fTo > fFrom) {
        fT = (fX - fFrom) / (fTo - fFrom);
        fT = fT < 0.0f ? 0.0f : fT > 1.0f ? 1.0f : fT;
    } else {
        fT = 1.0f - (fX - fFrom) / (fFrom - fTo);
        fT = fT < 0.0f ? 0.0f : fT > 1.0f ? 1.0f : fT;
    }
    return fT * (fB - fA) + fA;
}

// Scale the model's bones by the sliders.
void fn_8010E224(CharSliderDefs* pDefs, CharModel* pModel) {
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderRange* pRange;
    CharSliderBone* pBone;
    f32 fScale;
    int nBone;
    int i;
    int j;
    int k;

    if (pDefs == NULL || pModel == NULL) {
        return;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        pDef = &pDefs->pDefs[i];
        pValue = &pDefs->pValues[i];
        for (j = 0; j < pDef->nBoneRanges; j++) {
            pRange = &pDef->pBoneRanges[j];
            if (pValue->fValue >= pRange->fStart && pValue->fValue < pRange->fEnd) {
                for (k = 0; k < pRange->nItems; k++) {
                    pBone = &pRange->items.pBones[k];
                    fScale = fn_8010E194(pRange->fStart, pRange->fEnd, pValue->fValue,
                                         pBone->fFrom, pBone->fTo);
                    nBone = fn_800298F4(pModel, pBone->uId);
                    if (nBone >= 0) {
                        fn_80028A70(pModel, nBone, pBone->uAxes, fScale);
                    }
                }
            }
        }
    }
}

// Weight the skin's morph targets by the sliders (and mark the first 20 in the blend node).
void fn_8010E35C(CharSliderDefs* pDefs, Skin* pSkin, u8* pNode) {
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderRange* pRange;
    CharSliderMorph* pMorph;
    f32 fWeight;
    int i;
    int j;
    int k;
    int m;

    if (pDefs == NULL || pSkin == NULL) {
        return;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        pDef = &pDefs->pDefs[i];
        pValue = &pDefs->pValues[i];
        for (j = 0; j < pDef->nMorphRanges; j++) {
            pRange = &pDef->pMorphRanges[j];
            if (pValue->fValue >= pRange->fStart && pValue->fValue < pRange->fEnd) {
                for (k = 0; k < pRange->nItems; k++) {
                    pMorph = &pRange->items.pMorphs[k];
                    fWeight = fn_8010E194(pRange->fStart, pRange->fEnd, pValue->fValue,
                                          pMorph->fFrom, pMorph->fTo);
                    for (m = 0; m < pDefs->nMorphs; m++) {
                        if (pMorph->uId == pDefs->aMorphIds[m]) {
                            fn_8011CADC(pSkin, m, fWeight);
                            if (m < 20) {
                                fn_800736D8(pNode, m);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Apply nSliders slider values (percentages) to a character's model and skin.
void fn_8010E4DC(CharSliderDefs* pDefs, CharModel* pModel, Skin* pSkin, int nSliders, u8* aValues,
                 u8* pNode) {
    if (pModel == NULL || pSkin == NULL || aValues == NULL || pDefs == NULL) {
        return;
    }
    fn_80028A3C(pModel);
    fn_8010DCE8(pDefs);
    fn_8010DD4C(pDefs, nSliders, aValues);
    fn_8010DE60(pDefs);
    fn_8010DF8C(pDefs);
    fn_8010DE04(pDefs);
    fn_8010E224(pDefs, pModel);
    fn_8010E35C(pDefs, pSkin, pNode);
}

// One leaderboard row: the place ("CUT", "T3" for a tie, "3"), the name, the score, the round
// scores and the money won (empty when none).
void fn_8010E58C(char* szPlace, char* szName, char* szScore, char* szRounds, char* szMoney,
                 int nEntrant) {
    char szAmount[128];                 // sizes unknown
    char szRound[4];
    int nPlayer = fn_80077B08();
    s32 nGolfer = fn_80119118(nPlayer, nEntrant);
    s32 nRoundScore;
    s32 nMoney;
    int i;

    if (fn_801197A4(nPlayer, nEntrant)) {
        sprintf(szPlace, "CUT");
    } else if (fn_80119808(nPlayer, nEntrant)) {
        sprintf(szPlace, "T%d", fn_801190D8(nPlayer, nEntrant));
    } else {
        sprintf(szPlace, "%d", fn_801190D8(nPlayer, nEntrant));
    }
    sprintf(szName, "%s", fn_80118E30(nPlayer, nGolfer));
    sprintf(szScore, "%d", fn_801191D0(nPlayer, nEntrant, 1));
    szRounds[0] = '\0';
    for (i = 0; i < 4; i++) {
        nRoundScore = fn_80119638(nPlayer, nEntrant, i);
        if (nRoundScore != 0) {
            if (i > 0) {
                strcat(szRounds, " ");
            }
            sprintf(szRound, "%d", nRoundScore);
            strcat(szRounds, szRound);
        }
    }
    nMoney = fn_80119A2C(nPlayer, nEntrant);
    if (nMoney) {
        fn_800907AC(nMoney, szAmount);
        sprintf(szMoney, "$%s", szAmount);
        return;
    }
    strcpy(szMoney, "");
}

// Leaderboard row pArgs[0] (-1: the first entrant); all empty past the last row.
void fn_8010E748(MsgArg* pArgs, MsgArg* pResult) {
    int nRow = pArgs[0].i;
    char* szPlace = ((MsgString*)pArgs[1].p)->pStr;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szScore = ((MsgString*)pArgs[3].p)->pStr;
    char* szRounds = ((MsgString*)pArgs[4].p)->pStr;
    char* szMoney = ((MsgString*)pArgs[5].p)->pStr;
    u8 bShow = 0;
    int nPlayer = fn_80077B08();
    s32 nEntrant;

    if (nRow == -1) {
        if (fn_80118664(nPlayer) > 0) {
            nEntrant = 0;
            bShow = 1;
        }
    } else if (nRow < fn_80118664(nPlayer)) {
        bShow = 1;
        nEntrant = fn_801197CC(nPlayer, nRow);
    }
    if (bShow) {
        fn_8010E58C(szPlace, szName, szScore, szRounds, szMoney, nEntrant);
        return;
    }
    sprintf(szPlace, "");
    sprintf(szName, "");
    sprintf(szScore, "");
    sprintf(szRounds, "");
    sprintf(szMoney, "");
}

// The number of leaderboard rows.
void fn_8010E85C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80118664(fn_80077B08());
}

// Schedule line pArgs[0]: the tournament's dates, name, courses (one when all its rounds are on
// the same course) and defending champion.
void fn_8010E890(MsgArg* pArgs, MsgArg* pResult) {
    s32 aCourses[4];
    char szStart[8];
    char szEnd[8];
    s32 nEvent = lbl_802824B0[pArgs[0].i];
    char* szDates = ((MsgString*)pArgs[1].p)->pStr;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szCourses = ((MsgString*)pArgs[3].p)->pStr;
    char* szChamp = ((MsgString*)pArgs[4].p)->pStr;
    u16 nStart = fn_800EFD38(nEvent);
    u16 nEnd = GameModeDriverPGATour_GetEndDate(nEvent);
    Tournament* pTournament;
    s32 nCourses;
    u8 bOneCourse;
    int i;

    fn_800D293C(nStart, szStart);
    fn_800D293C(nEnd, szEnd);
    sprintf(szDates, "%s\nthru\n%s", szStart, szEnd);
    pTournament = fn_800EFA70(nEvent);
    sprintf(szName, "%s", GameModeDriverPGATour_GetName(nEvent));
    nCourses = GameModeDriverPGATour_GetCourses(pTournament, aCourses);
    szCourses[0] = '\0';
    bOneCourse = 1;
    for (i = 1; i < nCourses; i++) {
        if (aCourses[0] != aCourses[i]) {
            bOneCourse = 0;
            break;
        }
    }
    strcpy(szCourses, lbl_80191990[aCourses[0]]);
    if (!bOneCourse) {
        for (i = 1; i < nCourses; i++) {
            strcat(szCourses, "\n");
            strcat(szCourses, lbl_80191990[aCourses[i]]);
        }
    }
    GameModeDriverPGATour_GetChamp(nEvent, szChamp);
}

// Build the schedule: the tournaments that have a start date. Gives how many.
void fn_8010EA24(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvents = fn_800EF834();
    s32 nCount = 0;
    s32 i;

    if (lbl_802824B0 == NULL) {
        lbl_802824B0 = fn_80009B34(nEvents * 4, 1, 16, "FE_PGATourMessages.c", 263);
    }
    for (i = 0; i < nEvents; i++) {
        if (fn_800EFD38(i)) {
            lbl_802824B0[nCount++] = i;
        }
    }
    lbl_802824B4 = nCount;
    pResult->i = nCount;
}

// Line 1: the last tournament played (its name; empty before the first); line 2 is empty.
void fn_8010EAC4(MsgArg* pArgs, MsgArg* pResult) {
    int nLine = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    int nPlayer = fn_80077B08();
    s32 nEvent;

    switch (nLine) {
    case 1:
        if (fn_80118664(nPlayer) > 0) {
            nEvent = GameModeDriverPGATour_GetCurrentEventID();
            if (nEvent == -1) {
                nEvent = GameModeDriverPGATour_GetFinalEventOfSeason();
            } else if (gpSaveData[nPlayer].tour.nRound == 0) {
                nEvent--;
            }
            strcpy(szOut, GameModeDriverPGATour_GetName(nEvent));
            return;
        }
        strcpy(szOut, "");
        return;
    case 2:
        strcpy(szOut, "");
        return;
    }
}

// Whether no tournament is selected.
void fn_8010EB9C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRound;

    pResult->i = GameModeDriverPGATour_GetSelectedEvent(&nRound) == -1;
}

// Season wrap-up line pArgs[0]: the title, the player of the year, the money leader, the scoring
// leader (the player only after 15 events), the player, the player's wins, top-10 finishes,
// money and all-around rank.
void fn_8010EBDC(MsgArg* pArgs, MsgArg* pResult) {
    int nLine = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    int nPlayer = fn_80077B08();
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[PGA_USER_GOLFER];
    s32 nGolfer;
    SeasonEvent* pEvent;
    s32 nCount;
    int i;

    sprintf(szOut, "", nLine);
    switch (nLine) {
    case -1:
        sprintf(szOut, "%d Season Wrap-up", fn_800EFBAC());
        return;
    case 0:
        if (fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PLAYER_OF_YEAR_POINTS)) {
            nGolfer = PGA_USER_GOLFER;
        } else {
            nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_PLAYER_OF_YEAR_POINTS, 0);
        }
        strcpy(szOut, fn_80118E30(nPlayer, nGolfer));
        return;
    case 1:
        if (fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SEASON_WINNINGS)) {
            nGolfer = PGA_USER_GOLFER;
        } else {
            nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_SEASON_WINNINGS, 0);
        }
        strcpy(szOut, fn_80118E30(nPlayer, nGolfer));
        return;
    case 2:
        nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_SCORING, 0);
        if (nGolfer == PGA_USER_GOLFER && pStats->nEvents < 15) {
            nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_SCORING, 1);
        }
        strcpy(szOut, fn_80118E30(nPlayer, nGolfer));
        return;
    case 3:
        strcpy(szOut, fn_80118E30(nPlayer, PGA_USER_GOLFER));
        return;
    case 4:
        sprintf(szOut, "%d", pStats->nSeasonWins);
        return;
    case 5:
        nCount = 0;
        pEvent = gpSaveData[nPlayer].tour.aEvent;
        for (i = 0; i < 31; i++) {
            if (pEvent->nUserRankType == 2 && pEvent->nUserRank == 1 && fn_800EFA70(i)->nC != 0) {
                nCount++;
            }
            pEvent++;
        }
        sprintf(szOut, "%d", nCount);
        return;
    case 6:
        nCount = 0;
        pEvent = gpSaveData[nPlayer].tour.aEvent;
        for (i = 0; i < 31; i++) {
            if (pEvent->nUserRankType == 2 && pEvent->nUserRank <= 10) {
                nCount++;
            }
            pEvent++;
        }
        sprintf(szOut, "%d", nCount);
        return;
    case 7:
        sprintf(szOut, "$%d", pStats->nSeasonWinnings);
        return;
    case 8:
        sprintf(szOut, "%d",
                GM_PgaTourSim_GetStatRankFromGolferID(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_CAREER_WINNINGS));
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

// Start the next season.
void fn_8010EEA8(void) {
    lbl_80223C48.bSeasonOver = 0;
    fn_800EFAD0();
    fn_80117188();
    fn_80077808(0);
}

// ---- end of sweep code ----

// A sponsor offer's lines for pArgs[0].
void fn_8010EEE4(MsgArg* pArgs, MsgArg* pResult) {
    s32 n = pArgs[0].i;
    char* szB = ((MsgString*)pArgs[2].p)->pStr;
    char* szC = ((MsgString*)pArgs[3].p)->pStr;
    s32* pOut = (s32*)pArgs[4].p;

    sprintf(((MsgString*)pArgs[1].p)->pStr, "S %d", n);
    sprintf(szB, "I %d", n);
    sprintf(szC, "$ %d00,000", n);
    *pOut = n;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010EF80(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 25;
}

// ---- end of sweep code ----

// A new sponsor offer: the first of the 11 lock entries not set whose progress the profile has
// reached gets a kind picked at random (not one an earlier entry holds), and pays its n4.
// Gives whether there was one.
void fn_8010EF8C(MsgArg* pArgs, MsgArg* pResult) {
    s32 n8;
    s32 n4;
    int i;
    SaveProfile* pProfile = fn_80077ACC();
    s32* p0 = (s32*)pArgs[0].p;
    s32* pKind = (s32*)pArgs[1].p;
    s32* p8 = (s32*)pArgs[2].p;
    s32* p4 = (s32*)pArgs[3].p;
    u8 bFound;
    s32 nProgress;
    s16 nKind;
    int j;

    i = 0;
    bFound = 0;

    do {
        nProgress = fn_800F02D4(i);
        n4 = fn_800F02EC(i);
        n8 = fn_800F0304(i);
        if (!pProfile->a1054C[i].b && nProgress <= (s32)GM_GetGameProgress(pProfile)) {
        retry:
            nKind = lbl_80193CFC[Rand_Next(0) % 11];
            for (j = 0; j < i; j++) {
                if (pProfile->a1054C[j].n == nKind && pProfile->a1054C[j].b) {
                    // fake match: EA jumps straight back (a do-while adds a test)
                    goto retry;
                }
            }
            pProfile->a1054C[i].n = nKind;
            bFound = 1;
            pProfile->a1054C[i].b = 1;
            break;
        }
        i++;
    } while (i < 11);
    if (bFound) {
        *p0 = 0;
        *p8 = n8;
        *p4 = n4;
        *pKind = pProfile->a1054C[i].n;
        pProfile->n6C += n4;
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Lock entry pArgs[0]'s payout: its n8 for each of the profile's assets of its kind. Gives
// whether there is one.
void fn_8010F10C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 i = pArgs[0].i;
    s32* pValue = (s32*)pArgs[1].p;
    s32* pKind = (s32*)pArgs[2].p;
    int nCount = 0;
    s32 n8 = fn_800F0304(i);

    if (pProfile->a1054C[i].b) {
        nCount = fn_80106E48(pProfile->a1054C[i].n);
    }
    if (nCount) {
        *pValue = nCount * n8;
        *pKind = pProfile->a1054C[i].n;
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Pick a kind at random for lbl_80281DF0, and give it with lock entry 0's values.
void fn_8010F1B4(MsgArg* pArgs, MsgArg* pResult) {
    s32* pKind = (s32*)pArgs[0].p;
    s32* p4 = (s32*)pArgs[1].p;
    s32* p8 = (s32*)pArgs[2].p;

    fn_800588D4(lbl_80193CFC[Rand_Next(0) % 11]);
    *pKind = fn_800588E8();
    *p4 = fn_800F02EC(0);
    *p8 = fn_800F0304(0);
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010F248(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801070F4();
}

// ---- end of sweep code ----

// Record pArgs[0] of lbl_80282470.
void fn_8010F278(MsgArg* pArgs, MsgArg* pResult) {
    s16 n0;
    char* szName = ((MsgString*)pArgs[3].p)->pStr;
    s32* pN4 = (s32*)pArgs[2].p;
    int n = pArgs[0].i;
    s32* pN0 = (s32*)pArgs[1].p;

    n0 = 0;
    fn_80107244(n, &n0, pN4, szName);
    *pN0 = n0;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010F2CC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80107294(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// ---- end of sweep code ----

// A tournament the player of slot pArgs[1] may have won: its name, fn_800EFE3C, and the day it
// was won. Gives whether it was.
void fn_8010F2FC(MsgArg* pArgs, MsgArg* pResult) {
    int nPlayer = pArgs[1].i;
    s32 nEvent = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    char* szName = ((MsgString*)pArgs[4].p)->pStr;
    s32* pOut = (s32*)pArgs[5].p;
    u8 bWon;

    strcpy(szName, GameModeDriverPGATour_GetName(nEvent));
    *pOut = fn_800EFE3C(nEvent);
    bWon = gpSaveData[nPlayer].aC8[nEvent].award.bWon;
    if (bWon) {
        fn_800D28DC(gpSaveData[nPlayer].aC8[nEvent].award.nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

// Player of the Month award pArgs[2] of slot pArgs[1]: the day it was won. Gives whether it was.
void fn_8010F3A4(MsgArg* pArgs, MsgArg* pResult) {
    int nPlayer = pArgs[1].i;
    s32 n = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    s32* pOut = (s32*)pArgs[5].p;
    u8 bWon;

    strcpy(((MsgString*)pArgs[4].p)->pStr, "Player of the Month");
    *pOut = 0;
    bWon = gpSaveData[nPlayer].a1C0[n].b;
    if (bWon) {
        fn_800D28DC(gpSaveData[nPlayer].a1C0[n].nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

// A tournament the player won: the day, the player's name, the winner's earnings, the money won
// and the score.
void fn_8010F440(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvent = pArgs[0].i;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szEarnings = ((MsgString*)pArgs[3].p)->pStr;
    char* szMoney = ((MsgString*)pArgs[4].p)->pStr;
    s32* pScore = (s32*)pArgs[5].p;

    fn_800D28DC(fn_80077ACC()->aC8[nEvent].award.nDate, ((MsgString*)pArgs[1].p)->pStr);
    strcpy(szName, fn_80077ACC()->szName);
    GameModeDriverPGATour_GetWinnerEarningsString(nEvent, szEarnings);
    fn_800907AC(fn_80077ACC()->aC8[nEvent].n6 * 1000, szMoney);
    *pScore = fn_80077ACC()->aC8[nEvent].nScore;
}

void fn_8010F4EC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800EEF88(0);
    if (!fn_80077148()) {
        lbl_801D87C0.fFade = 1.0f;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010F52C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8011C058(1);
}

void fn_8010F550(MsgArg* pArgs, MsgArg* pResult) {
    fn_8011C060(1);
}

void fn_8010F574(MsgArg* pArgs, MsgArg* pResult) {
    // port: EA passes an argument fn_80117DE0 ignores
    pResult->i = ((u8 (*)(int))fn_80117DE0)(0);
}

// ---- end of sweep code ----

// Lock entry pArgs[0]: its kind (-1 when not set) and its n8.
void fn_8010F5AC(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 i = pArgs[0].i;
    s32* pKind = (s32*)pArgs[1].p;
    s32* p8 = (s32*)pArgs[2].p;
    s32* p3 = (s32*)pArgs[3].p;

    if (pProfile->a1054C[i].b) {
        *pKind = pProfile->a1054C[i].n;
        *p8 = fn_800F0304(i);
        *p3 = 0;
        return;
    }
    *pKind = -1;
    *p8 = 0;
    *p3 = 0;
}

// The n4 of every record of lbl_80282470 together.
void fn_8010F63C(MsgArg* pArgs, MsgArg* pResult) {
    char sz[0x24];                      // a CrAPRecord's name
    s32 n4;
    s16 n0;
    s32 nRecords = fn_801070F4();
    s32 nTotal;
    int i;

    n0 = 0;
    nTotal = 0;
    n4 = 0;
    for (i = 0; i < nRecords; i++) {
        fn_80107244(i, &n0, &n4, sz);
        nTotal += n4;
    }
    pResult->i = nTotal;
}

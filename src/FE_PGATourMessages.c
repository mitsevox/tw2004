// FE_PGATourMessages.c (EA's name, from its asserts): the PGA TOUR mode's front-end message
// handlers (registered in FE_MessageTable.c): the tournament leaderboard, the season schedule, the
// season wrap-up, starting the next season (PGADriver_ShowCalendar_AdvanceSeason, TW07's name), the
// sponsorships (the profile's 11 sponsorship slots, SaveProfile.aSponsor: a sponsor signed as the
// profile progresses, paying its start cash once and its bonus cash for every worn item of its
// brand), the trophy room's tournament wins and Player of the Month awards, and the details of a
// tournament won. The file starts at PGALeaderboard_FormatRow; the slider blending before it is
// CharSliders.c's.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "character.h"
#include "charstate.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"


// .sbss is laid out last-defined-first, so these are in reverse address order.
s32 gPgaScheduleCount;                  // how many tournaments gPgaScheduleEvents holds
s32* gPgaScheduleEvents;                // the tournaments held this season, in order
                                        // (PGASchedule_Build; static memory, never freed)

// Fills one leaderboard row of the current profile's tournament (FE_GetCurrUserID's slot) for entrant
// nEntrant: szPlace "CUT" when the entrant missed the cut, "T3" when another entrant holds the same
// place (GM_PgaTourSim_IsEntrantTied), else "3"; szName the golfer's name; szScore the total score
// (GM_PgaTourSim_GetTotalScoreFromEntrantID with flag 1); szRounds the round scores so far
// separated by spaces (a round scored 0 is left out); szMoney "$" and the money won as text
// (UI_GetMoneyString), empty when none.
void PGALeaderboard_FormatRow(char* szPlace, char* szName, char* szScore, char* szRounds,
                              char* szMoney, int nEntrant) {
    int i;
    s32 nRoundScore;
    char szAmount[128];                 // sizes unknown
    char szRound[4];
    int nPlayer = FE_GetCurrUserID();
    s32 nGolfer = GM_PgaTourSim_GetGolferIDFromEntrantID(nPlayer, nEntrant);
    s32 nMoney;

    if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrant)) {
        sprintf(szPlace, "CUT");
    } else if (GM_PgaTourSim_IsEntrantTied(nPlayer, nEntrant)) {
        sprintf(szPlace, "T%d", GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, nEntrant));
    } else {
        sprintf(szPlace, "%d", GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, nEntrant));
    }
    sprintf(szName, "%s", GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolfer));
    sprintf(szScore, "%d", GM_PgaTourSim_GetTotalScoreFromEntrantID(nPlayer, nEntrant, 1));
    szRounds[0] = '\0';
    for (i = 0; i < 4; i++) {
        nRoundScore = GM_PgaTourSim_GetRoundScoreFromEntrantID(nPlayer, nEntrant, i);
        if (nRoundScore != 0) {
            if (i > 0) {
                strcat(szRounds, " ");
            }
            sprintf(szRound, "%d", nRoundScore);
            strcat(szRounds, szRound);
        }
    }
    nMoney = GM_PgaTourSim_GetLeaderboardWinningsFromEntrantID(nPlayer, nEntrant);
    if (nMoney) {
        UI_GetMoneyString(nMoney, szAmount);
        sprintf(szMoney, "$%s", szAmount);
        return;
    }
    strcpy(szMoney, "");
}

// FE message 458: leaderboard row pArgs[0] into the texts pArgs[1..5] (place, name, score, round
// scores, money; PGALeaderboard_FormatRow). The row goes through
// GM_PgaTourSim_GetEntrantIDFromScoreRow, except -1, which gives entrant 0 itself; a row past the
// field, or -1 with an empty field, leaves all five texts empty.
void PGALeaderboard_GetRow(MsgArg* pArgs, MsgArg* pResult) {
    int nRow = pArgs[0].i;
    char* szPlace = ((MsgString*)pArgs[1].p)->pStr;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szScore = ((MsgString*)pArgs[3].p)->pStr;
    char* szRounds = ((MsgString*)pArgs[4].p)->pStr;
    char* szMoney = ((MsgString*)pArgs[5].p)->pStr;
    u8 bShow = 0;
    int nPlayer = FE_GetCurrUserID();
    s32 nEntrant;

    if (nRow == -1) {
        if (GM_PgaTourSim_GetNumEntrants(nPlayer) > 0) {
            nEntrant = 0;
            bShow = 1;
        }
    } else if (nRow < GM_PgaTourSim_GetNumEntrants(nPlayer)) {
        nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(nPlayer, nRow);
        bShow = 1;
    }
    if (bShow) {
        PGALeaderboard_FormatRow(szPlace, szName, szScore, szRounds, szMoney, nEntrant);
        return;
    }
    sprintf(szPlace, "");
    sprintf(szName, "");
    sprintf(szScore, "");
    sprintf(szRounds, "");
    sprintf(szMoney, "");
}

// FE message 459: how many entrants the current profile's tournament field has (the leaderboard's
// rows).
void PGALeaderboard_GetNumRows(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_PgaTourSim_GetNumEntrants(FE_GetCurrUserID());
}

// FE message 466: schedule row pArgs[0] (an index into gPgaScheduleEvents, PGASchedule_Build) into
// the texts pArgs[1..4]: the start and end days ("<start>\nthru\n<end>", CalDate_ToStringMD), the
// tournament's name, its courses (one name when every round is on the same course, else one per
// round, a line each) and its latest champion (GameModeDriverPGATour_GetChamp).
void PGASchedule_GetRow(MsgArg* pArgs, MsgArg* pResult) {
    s32 aCourses[4];
    char szStart[8];
    char szEnd[8];
    char* szDates = ((MsgString*)pArgs[1].p)->pStr;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szCourses = ((MsgString*)pArgs[3].p)->pStr;
    char* szChamp = ((MsgString*)pArgs[4].p)->pStr;
    s32 nEvent = gPgaScheduleEvents[pArgs[0].i];
    u16 nStart = GameModeDriverPGATour_GetStartDate(nEvent);
    u16 nEnd = GameModeDriverPGATour_GetEndDate(nEvent);
    Tournament* pTournament;
    s32 nCourses;
    u8 bOneCourse;
    int i;

    CalDate_ToStringMD(nStart, szStart);
    CalDate_ToStringMD(nEnd, szEnd);
    sprintf(szDates, "%s\nthru\n%s", szStart, szEnd);
    pTournament = GameModeDriverPGATour_GetEventInfo(nEvent);
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

// FE message 467: builds the season schedule: gPgaScheduleEvents gets, in order, every tournament
// held this season (a nonzero GameModeDriverPGATour_GetStartDate). The array is allocated from
// static memory on first use (room for all GM_PgaTourMode_GetNEvents tournaments) and never freed.
// Gives the count, also kept in gPgaScheduleCount.
void PGASchedule_Build(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvents = GM_PgaTourMode_GetNEvents();
    s32 nCount = 0;
    s32 i;

    if (gPgaScheduleEvents == NULL) {
        gPgaScheduleEvents = StaticMem_Alloc(nEvents * 4, 1, 16, "FE_PGATourMessages.c", 263);
    }
    for (i = 0; i < nEvents; i++) {
        if (GameModeDriverPGATour_GetStartDate(i)) {
            gPgaScheduleEvents[nCount++] = i;
        }
    }
    gPgaScheduleCount = nCount;
    pResult->i = nCount;
}

// FE message 520: text line pArgs[0] into pArgs[1]. Line 1 is the name of the tournament the
// current profile's field last played: the current tournament, the one before it when the current
// one has not started (tour.nRound 0), or the season's final tournament once no tournament is
// current; empty while the field has no entrants. Line 2 is empty; other lines leave the text as it
// was.
void PGATourMsg_GetLastEventLine(MsgArg* pArgs, MsgArg* pResult) {
    int nLine = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    int nPlayer = FE_GetCurrUserID();
    s32 nEvent;

    switch (nLine) {
    case 1:
        if (GM_PgaTourSim_GetNumEntrants(nPlayer) > 0) {
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

// FE message 551: 1 when profile 0's tour has no current tournament
// (GameModeDriverPGATour_GetSelectedEvent gives -1: the season is over), else 0.
void PGATourMsg_IsSeasonOver(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRound;

    pResult->i = GameModeDriverPGATour_GetSelectedEvent(&nRound) == -1;
}

// FE message 552: season wrap-up line pArgs[0] into the text pArgs[1] (empty for other lines): -1
// the title ("2004 Season Wrap-up"); 0 the Player of the Year (the player when no golfer beats
// their points); 1 the money leader (likewise); 2 the scoring leader (the next golfer when the
// leader is the player with fewer than 15 tournaments started); 3 the player's name; 4 the player's
// season wins; 5 the majors the player won (Tournament.bIsAMajor, placed first); 6 the player's
// top-10 finishes; 7 the season's money ("$%d"); 8 the player's rank in career money.
void PGASeasonWrapUp_GetLine(MsgArg* pArgs, MsgArg* pResult) {
    int nLine = pArgs[0].i;
    char* szOut = ((MsgString*)pArgs[1].p)->pStr;
    int nPlayer = FE_GetCurrUserID();
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[PGA_USER_GOLFER];
    s32 nGolfer;
    int i;
    s32 nCount;
    SeasonEvent* pEvent;

    sprintf(szOut, "", nLine);
    switch (nLine) {
    case -1:
        sprintf(szOut, "%d Season Wrap-up", GameModeDriverPGATour_GetCurrentSeasonYear());
        return;
    case 0:
        if (GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PLAYER_OF_YEAR_POINTS)) {
            nGolfer = PGA_USER_GOLFER;
        } else {
            nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_PLAYER_OF_YEAR_POINTS, 0);
        }
        strcpy(szOut, GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolfer));
        return;
    case 1:
        if (GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SEASON_WINNINGS)) {
            nGolfer = PGA_USER_GOLFER;
        } else {
            nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_SEASON_WINNINGS, 0);
        }
        strcpy(szOut, GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolfer));
        return;
    case 2:
        nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_SCORING, 0);
        if (nGolfer == PGA_USER_GOLFER && pStats->nEvents < 15) {
            nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, GM_PGA_STAT_SCORING, 1);
        }
        strcpy(szOut, GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolfer));
        return;
    case 3:
        strcpy(szOut, GM_PgaTourSim_GetNameFromGolferID(nPlayer, PGA_USER_GOLFER));
        return;
    case 4:
        sprintf(szOut, "%d", pStats->nSeasonWins);
        return;
    case 5:
        nCount = 0;
        pEvent = gpSaveData[nPlayer].tour.aEvent;
        for (i = 0; i < 31; i++) {
            if (pEvent[i].nUserRankType == 2 && pEvent[i].nUserRank == 1
                && GameModeDriverPGATour_GetEventInfo(i)->bIsAMajor != 0) {
                nCount++;
            }
        }
        sprintf(szOut, "%d", nCount);
        return;
    case 6:
        nCount = 0;
        pEvent = gpSaveData[nPlayer].tour.aEvent;
        for (i = 0; i < 31; i++) {
            if (pEvent[i].nUserRankType == 2 && pEvent[i].nUserRank <= 10) {
                nCount++;
            }
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

// FE message 553: starts the next PGA TOUR season (GameModeDriverPGATour_AdvanceSeason; its result,
// 0 after the tenth season, is not checked), resets the calendar to it and backs up profile slot 0
// (FE_BackupProfileClaimRow). Defined with no parameters though the message table calls it with two.
void PGADriver_ShowCalendar_AdvanceSeason(void) {
    CalendarState.bSeasonOver = 0;
    GameModeDriverPGATour_AdvanceSeason();
    ResetCalendarState();
    FE_BackupProfileClaimRow(0);
}

// FE message 559: placeholder texts for n = pArgs[0]: "S n", "I n" and "$ n00,000" into
// pArgs[1..3], and n itself into *pArgs[4].
void PGATourMsg_GetTestText(MsgArg* pArgs, MsgArg* pResult) {
    s32 n = pArgs[0].i;
    char* szB = ((MsgString*)pArgs[2].p)->pStr;
    char* szC = ((MsgString*)pArgs[3].p)->pStr;
    s32* pOut = (s32*)pArgs[4].p;

    sprintf(((MsgString*)pArgs[1].p)->pStr, "S %d", n);
    sprintf(szB, "I %d", n);
    sprintf(szC, "$ %d00,000", n);
    *pOut = n;
}

// The 11 sponsors (indexes of FE_CrAP_GetSponsorName's brands) PGASponsor_SignNext and
// PGASponsor_PickStartingSponsor draw from at random.
s16 gPgaSponsorChoices[11] = { 0, 1, 2, 5, 6, 9, 10, 11, 13, 14, 15 };

// FE message 560: always gives 25. What the menus count with it is not known; its message number
// follows the placeholder texts' (PGATourMsg_GetTestText, 559).
void GM_vFEMessage560_Return25(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 25;
}

// FE message 563: signs the current profile's next sponsorship: the first of the 11 sponsorship
// slots (SaveProfile.aSponsor) not yet signed whose required progress
// (GameModeDriverPGATour_GetSponsorshipProgress) the profile has reached (GM_GetGameProgress). Its
// sponsor is drawn at random from gPgaSponsorChoices, again until no earlier signed slot has it,
// and its start cash is paid into the profile's money (n6C). Then *pArgs[0] = 0, *pArgs[1] the
// sponsor, *pArgs[2] its bonus cash, *pArgs[3] its start cash, and it gives 1; 0 when there is none
// to sign.
void PGASponsor_SignNext(MsgArg* pArgs, MsgArg* pResult) {
    s32 nBonusCash;
    s32 nStartCash;
    int i;
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32* pZero = (s32*)pArgs[0].p;
    s32* pSponsor = (s32*)pArgs[1].p;
    s32* pBonusCash = (s32*)pArgs[2].p;
    s32* pStartCash = (s32*)pArgs[3].p;
    u8 bFound;
    s32 nProgress;
    s16 nSponsor;
    int j;

    i = 0;
    bFound = 0;

    do {
        nProgress = GameModeDriverPGATour_GetSponsorshipProgress(i);
        nStartCash = GameModeDriverPGATour_GetSponsorshipStartCash(i);
        nBonusCash = GameModeDriverPGATour_GetSponsorshipBonusCash(i);
        if (!pProfile->aSponsor[i].bSigned && nProgress <= (s32)GM_GetGameProgress(pProfile)) {
        retry:
            nSponsor = gPgaSponsorChoices[Misc_RandFunc(0) % 11];
            for (j = 0; j < i; j++) {
                if (pProfile->aSponsor[j].nSponsor == nSponsor && pProfile->aSponsor[j].bSigned) {
                    // fake match: EA jumps straight back (a do-while adds a test)
                    goto retry;
                }
            }
            pProfile->aSponsor[i].nSponsor = nSponsor;
            bFound = 1;
            pProfile->aSponsor[i].bSigned = 1;
            break;
        }
        i++;
    } while (i < 11);
    if (bFound) {
        *pZero = 0;
        *pBonusCash = nBonusCash;
        *pStartCash = nStartCash;
        *pSponsor = pProfile->aSponsor[i].nSponsor;
        pProfile->nCurrentCash += nStartCash;
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// FE message 683: what sponsorship slot pArgs[0] pays for the items the player wears: its bonus
// cash (GameModeDriverPGATour_GetSponsorshipBonusCash) times the equipped items carrying its
// sponsor (FE_CrAP_GetNumEquippedItemsWithSponsor) into *pArgs[1], its sponsor into *pArgs[2], and
// gives 1; gives 0 (outputs untouched) when the slot is not signed or no worn item carries the
// sponsor.
void PGASponsor_GetItemBonus(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 i = pArgs[0].i;
    s32* pPay = (s32*)pArgs[1].p;
    s32* pSponsor = (s32*)pArgs[2].p;
    int nCount = 0;
    s32 nBonusCash = GameModeDriverPGATour_GetSponsorshipBonusCash(i);

    if (pProfile->aSponsor[i].bSigned) {
        nCount = FE_CrAP_GetNumEquippedItemsWithSponsor(pProfile->aSponsor[i].nSponsor);
    }
    if (nCount) {
        *pPay = nCount * nBonusCash;
        *pSponsor = pProfile->aSponsor[i].nSponsor;
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// FE message 697: draws the sponsor a new profile starts with from gPgaSponsorChoices and switches
// lbl_80281DF0 on with it (FE_SetStartingSponsor; a new profile's first sponsorship slot is copied from it
// and its start cash paid, PasswordManager.c). Gives the sponsor in *pArgs[0], and sponsorship slot
// 0's start cash in *pArgs[1] and bonus cash in *pArgs[2].
void PGASponsor_PickStartingSponsor(MsgArg* pArgs, MsgArg* pResult) {
    s32* pSponsor = (s32*)pArgs[0].p;
    s32* pStartCash = (s32*)pArgs[1].p;
    s32* pBonusCash = (s32*)pArgs[2].p;

    FE_SetStartingSponsor(gPgaSponsorChoices[Misc_RandFunc(0) % 11]);
    *pSponsor = FE_GetStartingSponsor();
    *pStartCash = GameModeDriverPGATour_GetSponsorshipStartCash(0);
    *pBonusCash = GameModeDriverPGATour_GetSponsorshipBonusCash(0);
}

// FE message 698: fills the sponsorship item records (FE_CrAP_CollectSponsorshipItems: one per worn
// item of a signed sponsor) and gives how many there are.
void PGASponsor_CollectItems(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_CollectSponsorshipItems();
}

// FE message 699: sponsorship item record pArgs[0] (of PGASponsor_CollectItems' list): its sponsor
// into *pArgs[1], its bonus cash into *pArgs[2] and the item's name into the text pArgs[3].
void PGASponsor_GetItem(MsgArg* pArgs, MsgArg* pResult) {
    s16 nSponsor;
    char* szName = ((MsgString*)pArgs[3].p)->pStr;
    s32* pBonusCash = (s32*)pArgs[2].p;
    int n = pArgs[0].i;
    s32* pSponsor = (s32*)pArgs[1].p;

    nSponsor = 0;
    FE_CrAP_GetSponsorshipItemInfo(n, &nSponsor, pBonusCash, szName);
    *pSponsor = nSponsor;
}

// FE message 700: sponsor pArgs[0]'s brand name ("adidas", "Callaway Golf"...;
// FE_CrAP_GetSponsorName) into the text pArgs[1].
void PGASponsor_GetName(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_GetSponsorName(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// Trophy room, kind 0 of GM_vTrophyRoomGetStatus's award messages: tournament pArgs[2] of profile slot
// pArgs[1]: its name into the text pArgs[4], its icon (GameModeDriverPGATour_GetTextureID) into
// *pArgs[5] and, when the profile has won it (aC8[].award), the day won (month/day/year) into the
// text pArgs[3], else an empty text. Gives whether it was won.
void TrophyRoom_GetTourWinStatus(MsgArg* pArgs, MsgArg* pResult) {
    int nPlayer = pArgs[1].i;
    s32 nEvent = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    char* szName = ((MsgString*)pArgs[4].p)->pStr;
    s32* pOut = (s32*)pArgs[5].p;
    u8 bWon;

    strcpy(szName, GameModeDriverPGATour_GetName(nEvent));
    *pOut = GameModeDriverPGATour_GetTextureID(nEvent);
    bWon = gpSaveData[nPlayer].aC8[nEvent].award.bWon;
    if (bWon) {
        CalDate_ToString(gpSaveData[nPlayer].aC8[nEvent].award.nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

// Trophy room, kind 1 of GM_vTrophyRoomGetStatus's award messages: Player of the Month award
// pArgs[2] (a month, SaveProfile.aTourAward) of profile slot pArgs[1]: "Player of the Month" into
// the text pArgs[4], icon 0 into *pArgs[5] and, when won, the day won into the text pArgs[3] (else
// empty). Gives whether it was won.
void TrophyRoom_GetPlayerOfMonthStatus(MsgArg* pArgs, MsgArg* pResult) {
    int nPlayer = pArgs[1].i;
    s32 n = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    s32* pOut = (s32*)pArgs[5].p;
    u8 bWon;

    strcpy(((MsgString*)pArgs[4].p)->pStr, "Player of the Month");
    *pOut = 0;
    bWon = gpSaveData[nPlayer].aTourAward[n].bWon;
    if (bWon) {
        CalDate_ToString(gpSaveData[nPlayer].aTourAward[n].nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

// FE message 701: a tournament pArgs[0] the current profile won: the day won into the text
// pArgs[1], the profile's name into pArgs[2], the tournament's purse
// (GameModeDriverPGATour_GetPurseString) into pArgs[3], the prize won (TourWin.n6 thousands of
// dollars, as money text) into pArgs[4] and the profile's score there into *pArgs[5].
void PGATourWins_GetDetails(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEvent = pArgs[0].i;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szEarnings = ((MsgString*)pArgs[3].p)->pStr;
    char* szMoney = ((MsgString*)pArgs[4].p)->pStr;
    s32* pScore = (s32*)pArgs[5].p;

    CalDate_ToString(FE_GetCurrentProfile()->aC8[nEvent].award.nDate, ((MsgString*)pArgs[1].p)->pStr);
    strcpy(szName, FE_GetCurrentProfile()->szName);
    GameModeDriverPGATour_GetPurseString(nEvent, szEarnings);
    UI_GetMoneyString(FE_GetCurrentProfile()->aC8[nEvent].n6 * 1000, szMoney);
    *pScore = FE_GetCurrentProfile()->aC8[nEvent].nScore;
}

// FE message 718: after a tour round, profile 0's tournament is moved on
// (GameModeDriverPGATour_CheckAdvanceTournament); when a movie is queued (FE_movieIsQueueEmpty false), the
// front end's fade to black (gUIState.fFade) is set to full at once.
void PGATourMsg_CheckAdvanceTournament(MsgArg* pArgs, MsgArg* pResult) {
    GameModeDriverPGATour_CheckAdvanceTournament(0);
    if (!FE_movieIsQueueEmpty()) {
        gUIState.fFade = 1.0f;
    }
}

// FE message 742: marks the tour statistics as needing an update (GM_PgaTourSim_SetStatsDirty(1); TW06:
// GM_PgaTourSim_SetStatsDirty).
void PGATourMsg_SetStatsDirty(MsgArg* pArgs, MsgArg* pResult) {
    GM_PgaTourSim_SetStatsDirty(1);
}

// FE message 743: marks the tour scores as needing an update (GM_PgaTourSim_SetScoresDirty(1); TW06:
// GM_PgaTourSim_SetScoresDirty).
void PGATourMsg_SetScoresDirty(MsgArg* pArgs, MsgArg* pResult) {
    GM_PgaTourSim_SetScoresDirty(1);
}

// FE message 749: whether the player quit the tour round (the tour simulation's user-quit flag,
// GM_PgaTourSim_DidUserQuit).
void PGATourMsg_DidUserQuit(MsgArg* pArgs, MsgArg* pResult) {
    // port: EA passes an argument GM_PgaTourSim_DidUserQuit ignores
    pResult->i = ((u8 (*)(int))GM_PgaTourSim_DidUserQuit)(0);
}

// FE message 754: sponsorship slot pArgs[0] of the current profile: its sponsor into *pArgs[1] and
// its bonus cash into *pArgs[2], or -1 and 0 when the slot is not signed; *pArgs[3] is always set
// to 0.
void PGASponsor_GetSlot(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 i = pArgs[0].i;
    s32* pSponsor = (s32*)pArgs[1].p;
    s32* pBonusCash = (s32*)pArgs[2].p;
    s32* pZero = (s32*)pArgs[3].p;

    if (pProfile->aSponsor[i].bSigned) {
        *pSponsor = pProfile->aSponsor[i].nSponsor;
        *pBonusCash = GameModeDriverPGATour_GetSponsorshipBonusCash(i);
        *pZero = 0;
        return;
    }
    *pSponsor = -1;
    *pBonusCash = 0;
    *pZero = 0;
}

// FE message 755: the bonus cash of every worn item of a signed sponsor added up (the records of
// FE_CrAP_CollectSponsorshipItems, which it fills first).
void PGASponsor_GetTotalItemBonus(MsgArg* pArgs, MsgArg* pResult) {
    char szName[0x34];                  // a CrAPRecord's name (0x24); size unknown, the frame fits 0x34
    s32 nBonusCash;
    s16 nSponsor;
    s32 nRecords = FE_CrAP_CollectSponsorshipItems();
    int i;
    s32 nTotal;

    nSponsor = 0;
    nTotal = 0;
    nBonusCash = 0;
    for (i = 0; i < nRecords; i++) {
        FE_CrAP_GetSponsorshipItemInfo(i, &nSponsor, &nBonusCash, szName);
        nTotal += nBonusCash;
    }
    pResult->i = nTotal;
}

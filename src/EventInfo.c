// EventInfo.c (our name; EA's FE_CalendarPopups.c in TW07): the rows of the calendar's day popup
// about an event, one row's title and text a call (rows 3 to 8; GameModeDriver.c's
// PGATour_GetPopupRow and RealTime_GetPopupRow fill the rest): for a PGA TOUR event under way,
// played or to come the purse, round, course, leader, scores, winner and defending champion; for a
// real-time event its purse, rewards and status. TW07's file has the seven in the same order with
// the same locals, after three OnlinePopup_GetRow_* ones stripped here. Three front-end messages
// follow that TW07's file does not have: the next real-time event, the clock's date and an award.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/pgatour.h"
#include "game/modes/rte.h"

// Up to three reward names for a real-time event (nKind 0x11); how many there are.
int  FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(int nKind, s32 nId, char* szFirst, char* szSecond, char* szThird);
void Gaud_PlayUISound(s32 n);
void PGATourPopup_GetRow_EventUpcoming(int nRow, char* szTitle, char* szText);

// fake match: stands in for the functions the original linker stripped. The file's strings start
// with "", "Purse:" and "Status:" (.sdata 0x802818A8) and "Rewards:" (.data 0x801944F8), before
// PGATourPopup_GetRow_EventInProgress's; TW07's FE_CalendarPopups.c starts with three
// OnlinePopup_GetRow_* functions (today's event, results, upcoming) whose rows these likely are.
// Their bodies are unknown.
static void EventInfo_StrippedFn(char* szTitle, char* szText) {
    strcpy(szText, "");
    strcpy(szTitle, "Purse:");
    strcpy(szTitle, "Status:");
    strcpy(szTitle, "Rewards:");
}

// A row of the PGA TOUR calendar's day popup for a tournament under way (popup type 0,
// PGATour_GetPopupRow): 3 the purse, 4 the round (from 1), 5 the round's course (the first round's
// when the round is out of range), 6 the leader, 7 the leader's score and 8 the player's, to par
// ("E" for even). Other rows are left as they were.
void PGATourPopup_GetRow_EventInProgress(int nRow, char* szTitle, char* szText) {
    char sz[128];
    s32 aCourses[4];
    s32 nId;
    s32 nRound;
    s32 nRounds;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Purse:");
        GameModeDriverPGATour_GetPurseString(nId, sz);
        sprintf(szText, "$%s", sz);
        break;
    case 4:
        strcpy(szTitle, "Round:");
        sprintf(szText, "%d", nRound + 1);
        break;
    case 5:
        strcpy(szTitle, "Course:");
        GameModeDriverPGATour_GetCourses(GameModeDriverPGATour_GetEventInfo(nId), aCourses);
        nRounds = GameModeDriverPGATour_GetRounds(nId);
        if (nRound >= 0 && nRound < nRounds) {
            strcpy(szText, lbl_80191990[aCourses[nRound]]);
        } else {
            strcpy(szText, lbl_80191990[aCourses[0]]);
        }
        break;
    case 6:
        strcpy(szTitle, "Leader:");
        GameModeDriverPGATour_GetCurrentEventLeader(sz);
        strcpy(szText, sz);
        break;
    case 7: {
        // Each score case has its own block-scoped local (a shared one allocates differently).
        int nScore = GameModeDriverPGATour_GetCurrentLeaderScore();
        strcpy(szTitle, "Score:");
        if (nScore == 0) {
            sprintf(szText, "E");
        } else if (nScore > 0) {
            sprintf(szText, "+%d", nScore);
        } else {
            sprintf(szText, "%d", nScore);
        }
        break;
    }
    case 8: {
        int nScore = GameModeDriverPGATour_GetUserScore(nId);
        strcpy(szTitle, "Your Score:");
        if (nScore == 0) {
            sprintf(szText, "E");
        } else if (nScore > 0) {
            sprintf(szText, "+%d", nScore);
        } else {
            sprintf(szText, "%d", nScore);
        }
        break;
    }
    }
}

// A row of the PGA TOUR calendar's day popup for a tournament already played (popup type 1): rows 3
// and 8 blank, 4 the winner, 5 the winning score, 6 the winner's earnings, 7 the player's finish.
void PGATourPopup_GetRow_EventResults(int nRow, char* szTitle, char* szText) {
    char sz[128];
    s32 nId;
    s32 nRound;
    s32 nScore;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    case 4:
        strcpy(szTitle, "Winner:");
        GameModeDriverPGATour_GetChamp(nId, sz);
        strcpy(szText, sz);
        break;
    case 5:
        nScore = GameModeDriverPGATour_GetChampScore(nId);
        strcpy(szTitle, "Score:");
        sprintf(szText, "%d", nScore);
        break;
    case 6:
        strcpy(szTitle, "Earnings:");
        GameModeDriverPGATour_GetWinnerEarningsString(nId, sz);
        sprintf(szText, "$%s", sz);
        break;
    case 7:
        strcpy(szTitle, "Your Finish:");
        GameModeDriverPGATour_GetUserFinishString(nId, sz);
        strcpy(szText, sz);
        break;
    case 8:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    }
}

// A row of the PGA TOUR calendar's day popup for a tournament to come (popup type 2): rows 3 and 4
// "Defending " / "Champion:" and the defending champion, 5 and 6 "Winning " / "Score:" and the
// champion's score ("N/A" unless above 0), 7 the purse, 8 the round's course (the first round's
// when the round is out of range).
void PGATourPopup_GetRow_EventUpcoming(int nRow, char* szTitle, char* szText) {
    char sz[128];
    s32 aCourses[4];
    s32 nId;
    s32 nRound;
    Tournament* pTournament;
    s32 nRounds;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Defending ");
        strcpy(szText, "");
        break;
    case 4:
        strcpy(szTitle, "Champion:");
        GameModeDriverPGATour_GetChamp(nId, sz);
        strcpy(szText, sz);
        break;
    case 5:
        strcpy(szTitle, "Winning ");
        strcpy(szText, "");
        break;
    case 6:
        strcpy(szTitle, "Score:");
        if (GameModeDriverPGATour_GetChampScore(nId) > 0) {
            sprintf(szText, "%d", GameModeDriverPGATour_GetChampScore(nId));
        } else {
            strcpy(szText, "N/A");
        }
        break;
    case 7:
        strcpy(szTitle, "Purse:");
        GameModeDriverPGATour_GetPurseString(nId, sz);
        sprintf(szText, "$%s", sz);
        break;
    case 8:
        strcpy(szTitle, "Course:");
        pTournament = GameModeDriverPGATour_GetEventInfo(nId);
        nRounds = GameModeDriverPGATour_GetRounds(nId);
        GameModeDriverPGATour_GetCourses(pTournament, aCourses);
        if (nRound >= 0 && nRound < nRounds) {
            strcpy(szText, lbl_80191990[aCourses[nRound]]);
        } else {
            strcpy(szText, lbl_80191990[aCourses[0]]);
        }
        break;
    }
}

// A row of the PGA TOUR calendar's day popup before the tournament starts (popup type 3): the same
// rows as PGATourPopup_GetRow_EventUpcoming. TW07's is empty.
void PGATourPopup_GetRow_EventNextEvent(int nRow, char* szTitle, char* szText) {
    PGATourPopup_GetRow_EventUpcoming(nRow, szTitle, szText);
}

// A row of the real-time events calendar's day popup for today's event (popup type 4,
// RealTime_GetPopupRow): 3 the purse, 4 to 6 up to three rewards (the items the event's trophy
// unlocks; "N/A" in row 4 when there are none), 7 and 8 blank.
void RealtimePopup_GetRow_TodaysEvent(int nRow, char* szTitle, char* szText) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nId;
    s32 nRound;
    int nRewards;

    GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    nRewards = FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(0x11, GM_RealtimeMode_GetTrophyID(nId),
                                                            szReward1, szReward2, szReward3);
    fn_800907AC(GameModeDriverRTE_GetPurse(nId), szMoney);
    sprintf(szPurse, "$%s", szMoney);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Purse:");
        strcpy(szText, szPurse);
        break;
    case 4:
        strcpy(szTitle, "Rewards:");
        if (nRewards >= 1) {
            strcpy(szText, szReward1);
        } else {
            strcpy(szText, "N/A");
        }
        break;
    case 5:
        strcpy(szTitle, "");
        if (nRewards >= 2) {
            strcpy(szText, szReward2);
        } else {
            strcpy(szText, "");
        }
        break;
    case 6:
        strcpy(szTitle, "");
        if (nRewards >= 3) {
            strcpy(szText, szReward3);
        } else {
            strcpy(szText, "");
        }
        break;
    case 7:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    case 8:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    }
}

// A row of the real-time events calendar's day popup for an event already played (popup type 5: an
// earlier day's, or today's once GM_RealtimeMode_TodaysEventCompleted): 3 "COMPLETE" or
// "INCOMPLETE" for the current profile, 4 the purse, 5 to 7 up to three rewards ("N/A" in row 5
// when there are none), 8 blank.
void RealtimePopup_GetRow_EventResults(int nRow, char* szTitle, char* szText) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nId;
    s32 nRound;
    int nRewards;
    u8 bComplete;

    GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    bComplete = GameModeDriverRTE_IsEventComplete(lbl_80281ED4->nSlot, nId);
    nRewards = FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(0x11, GM_RealtimeMode_GetTrophyID(nId),
                                                            szReward1, szReward2, szReward3);
    fn_800907AC(GameModeDriverRTE_GetPurse(nId), szMoney);
    sprintf(szPurse, "$%s", szMoney);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Status:");
        if (bComplete) {
            strcpy(szText, "COMPLETE");
        } else {
            strcpy(szText, "INCOMPLETE");
        }
        break;
    case 4:
        strcpy(szTitle, "Purse:");
        strcpy(szText, szPurse);
        break;
    case 5:
        strcpy(szTitle, "Rewards:");
        if (nRewards >= 1) {
            strcpy(szText, szReward1);
        } else {
            strcpy(szText, "N/A");
        }
        break;
    case 6:
        strcpy(szTitle, "");
        if (nRewards >= 2) {
            strcpy(szText, szReward2);
        } else {
            strcpy(szText, "");
        }
        break;
    case 7:
        strcpy(szTitle, "");
        if (nRewards >= 3) {
            strcpy(szText, szReward3);
        } else {
            strcpy(szText, "");
        }
        break;
    case 8:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    }
}

// A row of the real-time events calendar's day popup for an event on a later day (popup type 6):
// the same rows as RealtimePopup_GetRow_TodaysEvent (3 the purse, 4 to 6 up to three rewards, 7 and
// 8 blank).
void RealtimePopup_GetRow_EventUpcoming(int nRow, char* szTitle, char* szText) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nId;
    s32 nRound;
    int nRewards;

    GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    nRewards = FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(0x11, GM_RealtimeMode_GetTrophyID(nId),
                                                            szReward1, szReward2, szReward3);
    fn_800907AC(GameModeDriverRTE_GetPurse(nId), szMoney);
    sprintf(szPurse, "$%s", szMoney);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Purse:");
        strcpy(szText, szPurse);
        break;
    case 4:
        strcpy(szTitle, "Rewards:");
        if (nRewards >= 1) {
            strcpy(szText, szReward1);
        } else {
            strcpy(szText, "N/A");
        }
        break;
    case 5:
        strcpy(szTitle, "");
        if (nRewards >= 2) {
            strcpy(szText, szReward2);
        } else {
            strcpy(szText, "");
        }
        break;
    case 6:
        strcpy(szTitle, "");
        if (nRewards >= 3) {
            strcpy(szText, szReward3);
        } else {
            strcpy(szText, "");
        }
        break;
    case 7:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    case 8:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    }
}

// FE message 536: the next real-time event (GameModeDriverRTE_GetNextEvent) into three strings: its
// name, the placeholder "Status (?)" and its start date. When it starts less than two days after
// today and a profile is loaded in slot 0, UI sound 0x19 plays. Gives whether it played; 0, with
// the strings untouched, when there is no next event.
void FE_GetNextRealtimeEventInfo(MsgArg* pArgs, MsgArg* pResult) {
    char* szName = ((MsgString*)pArgs[0].p)->pStr;
    char* szStatus = ((MsgString*)pArgs[1].p)->pStr;
    char* szDate = ((MsgString*)pArgs[2].p)->pStr;
    u8 bSoon;
    s32 nEvent = GameModeDriverRTE_GetNextEvent();
    u16 nDate;
    int bNear;

    if (nEvent == -1) {
        pResult->i = 0;
        return;
    }
    GameModeDriverRTE_GetCalData(nEvent);
    nDate = GM_RealtimeMode_GetStartDate(nEvent);
    strcpy(szName, GameModeDriverRTE_GetName(nEvent));
    strcpy(szStatus, "Status (?)");
    strcpy(szDate, "Start Date");
    CalDate_ToString(nDate, szDate);
    bNear = 0;
    if (nDate - CalDate_GetToday() < 2 && lbl_801D7148.aLoaded[0]) {
        bNear = 1;
    }
    // fake match: worked out as an int and kept as a u8 (one clrlwi for the test and the result);
    // the original likely returns it from an inlined u8 helper
    bSoon = bNear;
    if (bSoon) {
        Gaud_PlayUISound(0x19);
    }
    pResult->i = bSoon;
}

// FE message 544: the console clock's date and time as text ("M/D/YYYY H:MM AM",
// RTClock_GetDateTimeString) when its year is before 2003 and its month before October; otherwise
// an empty string. Gives 1 when it wrote the date. As written, October to December of an earlier
// year give the empty string too (see the EA bug).
void FE_GetDateTimeIfClockEarly(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMonth;
    s32 nYear;
    s32 nUnused;
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;
    int bShow;

    fn_8011E020(&nMonth, &nUnused, &nYear, &nUnused, &nUnused, &nUnused, &nUnused);
    bShow = 0;
    // EA bug: && makes this no date cutoff: a clock in January to September of a year before 2003
    // counts as early, one in October to December of the same year does not
    if (nYear < 2003 && nMonth < 10) {
        bShow = 1;
    }
    if (bShow) {
        RTClock_GetDateTimeString(szOut);
        pResult->i = 1;
    } else {
        szOut[0] = '\0';
        pResult->i = 0;
    }
}

// FE message 696 with pArgs[0] 3 (fn_80084B88 passes it on): real-time event award pArgs[2]. Its
// name goes into the string pArgs[4] (GM_RealtimeMode_GetNameByTrophyGroup), its icon into the int
// pArgs[5] points to (GM_RealtimeMode_GetIconIDByTrophyGroup) and the day the current profile won
// it into the string pArgs[3] (empty when not won). pArgs[1], the player slot for the other awards,
// is not read. Gives whether it is won.
void TrophyRoom_GetRTEAwardStatus(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nId = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    s32* pIcon = (s32*)pArgs[5].p;
    u8 bWon;

    GM_RealtimeMode_GetNameByTrophyGroup(nId, ((MsgString*)pArgs[4].p)->pStr);
    *pIcon = GM_RealtimeMode_GetIconIDByTrophyGroup(nId);
    bWon = pProfile->aRTEAward[nId].bWon;
    if (bWon) {
        CalDate_ToString(pProfile->aRTEAward[nId].nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

// FE_CalendarPopups.c: rows 3 to 8 of the career calendar's day-details popup for a PGA TOUR or a
// real-time event (purse, round, course, leader, scores, champion, rewards), and three front-end
// messages: the next real-time event, the console clock's date and a real-time event award.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/pgatour.h"
#include "game/modes/rte.h"
#include "core/gameaudio.h"

// fake match: never called, puts the file's first strings in order
static void CalendarPopups_OrderStrings(char* szTitle, char* szText) {
    strcpy(szText, "");
    strcpy(szTitle, "Purse:");
    strcpy(szTitle, "Status:");
    strcpy(szTitle, "Rewards:");
}

// A row of the PGA TOUR calendar's day popup for a tournament under way
// (CALENDAR_POPUP_PGA_IN_PROGRESS): 3 the purse, 4 the selected day's round (from 1), 5 that
// round's course, 6 the leader, 7 the leader's score and 8 the player's, to par ("E" for even).
// Other rows are left as they were.
void PGATourPopup_GetRow_EventInProgress(int nRow, char* szTitle, char* szText) {
    char szValue[128];
    s32 aCourses[4];
    s32 nEvent;
    s32 nRound;
    s32 nRounds;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nEvent, &nRound);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Purse:");
        GameModeDriverPGATour_GetPurseString(nEvent, szValue);
        sprintf(szText, "$%s", szValue);
        break;
    case 4:
        strcpy(szTitle, "Round:");
        sprintf(szText, "%d", nRound + 1);
        break;
    case 5:
        strcpy(szTitle, "Course:");
        GameModeDriverPGATour_GetCourses(GameModeDriverPGATour_GetEventInfo(nEvent), aCourses);
        nRounds = GameModeDriverPGATour_GetRounds(nEvent);
        if (nRound >= 0 && nRound < nRounds) {
            strcpy(szText, gCourseNames[aCourses[nRound]]);
        } else {
            strcpy(szText, gCourseNames[aCourses[0]]);
        }
        break;
    case 6:
        strcpy(szTitle, "Leader:");
        GameModeDriverPGATour_GetCurrentEventLeader(szValue);
        strcpy(szText, szValue);
        break;
    case 7: {
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
        int nScore = GameModeDriverPGATour_GetUserScore(nEvent);
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

// A row of the PGA TOUR calendar's day popup for a tournament already played
// (CALENDAR_POPUP_PGA_RESULTS): rows 3 and 8 blank, 4 the winner, 5 the winning score, 6 the
// winner's earnings, 7 the player's finish.
void PGATourPopup_GetRow_EventResults(int nRow, char* szTitle, char* szText) {
    char szValue[128];
    s32 nEvent;
    s32 nRound;
    s32 nScore;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nEvent, &nRound);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    case 4:
        strcpy(szTitle, "Winner:");
        GameModeDriverPGATour_GetChamp(nEvent, szValue);
        strcpy(szText, szValue);
        break;
    case 5:
        nScore = GameModeDriverPGATour_GetChampScore(nEvent);
        strcpy(szTitle, "Score:");
        sprintf(szText, "%d", nScore);
        break;
    case 6:
        strcpy(szTitle, "Earnings:");
        GameModeDriverPGATour_GetWinnerEarningsString(nEvent, szValue);
        sprintf(szText, "$%s", szValue);
        break;
    case 7:
        strcpy(szTitle, "Your Finish:");
        GameModeDriverPGATour_GetUserFinishString(nEvent, szValue);
        strcpy(szText, szValue);
        break;
    case 8:
        strcpy(szTitle, "");
        strcpy(szText, "");
        break;
    }
}

// A row of the PGA TOUR calendar's day popup for a tournament to come
// (CALENDAR_POPUP_PGA_UPCOMING): rows 3 and 4 "Defending " / "Champion:" and the defending
// champion, 5 and 6 "Winning " / "Score:" and the champion's score ("N/A" unless above 0), 7 the
// purse, 8 the course of the selected day's round.
void PGATourPopup_GetRow_EventUpcoming(int nRow, char* szTitle, char* szText) {
    char szValue[128];
    s32 aCourses[4];
    s32 nEvent;
    s32 nRound;
    Tournament* pTournament;
    s32 nRounds;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nEvent, &nRound);
    switch (nRow) {
    case 3:
        strcpy(szTitle, "Defending ");
        strcpy(szText, "");
        break;
    case 4:
        strcpy(szTitle, "Champion:");
        GameModeDriverPGATour_GetChamp(nEvent, szValue);
        strcpy(szText, szValue);
        break;
    case 5:
        strcpy(szTitle, "Winning ");
        strcpy(szText, "");
        break;
    case 6:
        strcpy(szTitle, "Score:");
        if (GameModeDriverPGATour_GetChampScore(nEvent) > 0) {
            sprintf(szText, "%d", GameModeDriverPGATour_GetChampScore(nEvent));
        } else {
            strcpy(szText, "N/A");
        }
        break;
    case 7:
        strcpy(szTitle, "Purse:");
        GameModeDriverPGATour_GetPurseString(nEvent, szValue);
        sprintf(szText, "$%s", szValue);
        break;
    case 8:
        strcpy(szTitle, "Course:");
        pTournament = GameModeDriverPGATour_GetEventInfo(nEvent);
        nRounds = GameModeDriverPGATour_GetRounds(nEvent);
        GameModeDriverPGATour_GetCourses(pTournament, aCourses);
        if (nRound >= 0 && nRound < nRounds) {
            strcpy(szText, gCourseNames[aCourses[nRound]]);
        } else {
            strcpy(szText, gCourseNames[aCourses[0]]);
        }
        break;
    }
}

// A row of the PGA TOUR calendar's day popup for the career's current tournament while it is on its
// first round (CALENDAR_POPUP_PGA_NEXT_EVENT): the same rows as for a tournament to come.
void PGATourPopup_GetRow_EventNextEvent(int nRow, char* szTitle, char* szText) {
    PGATourPopup_GetRow_EventUpcoming(nRow, szTitle, szText);
}

// A row of the real-time events calendar's day popup for today's event (CALENDAR_POPUP_RTE_TODAY):
// 3 the purse, 4 to 6 up to three rewards (the Create-A-Player items the event's award unlocks;
// "N/A" in row 4 when there are none), 7 and 8 blank.
void RealtimePopup_GetRow_TodaysEvent(int nRow, char* szTitle, char* szText) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nEvent;
    s32 nDay;
    int nRewards;

    GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nEvent, &nDay);
    nRewards = FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(CRAP_LOCK_RTE_AWARD,
                                                            GM_RealtimeMode_GetTrophyID(nEvent),
                                                            szReward1, szReward2, szReward3);
    UI_GetMoneyString(GameModeDriverRTE_GetPurse(nEvent), szMoney);
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

// A row of the real-time events calendar's day popup for an event on a past day
// (CALENDAR_POPUP_RTE_RESULTS): 3 "COMPLETE" or "INCOMPLETE" for the profile being worked on, 4 the
// purse, 5 to 7 up to three rewards ("N/A" in row 5 when there are none), 8 blank.
void RealtimePopup_GetRow_EventResults(int nRow, char* szTitle, char* szText) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nEvent;
    s32 nDay;
    int nRewards;
    u8 bComplete;

    GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nEvent, &nDay);
    bComplete = GameModeDriverRTE_IsEventComplete(gpFEProfile->nSlot, nEvent);
    nRewards = FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(CRAP_LOCK_RTE_AWARD,
                                                            GM_RealtimeMode_GetTrophyID(nEvent),
                                                            szReward1, szReward2, szReward3);
    UI_GetMoneyString(GameModeDriverRTE_GetPurse(nEvent), szMoney);
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

// A row of the real-time events calendar's day popup for an event on a later day
// (CALENDAR_POPUP_RTE_UPCOMING): the same rows as for today's event (3 the purse, 4 to 6 up to
// three rewards, 7 and 8 blank).
void RealtimePopup_GetRow_EventUpcoming(int nRow, char* szTitle, char* szText) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nEvent;
    s32 nDay;
    int nRewards;

    GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nEvent, &nDay);
    nRewards = FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(CRAP_LOCK_RTE_AWARD,
                                                            GM_RealtimeMode_GetTrophyID(nEvent),
                                                            szReward1, szReward2, szReward3);
    UI_GetMoneyString(GameModeDriverRTE_GetPurse(nEvent), szMoney);
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

// Front-end message 536: the next real-time event's name, the placeholder "Status (?)" and its
// start date, into the three strings pArgs[0..2]. When it starts less than two days after today
// and a profile is loaded in player slot 0, menu sound 25 plays. Gives whether it played; 0, with
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
    if (nDate - CalDate_GetToday() < 2 && gFEState.aLoaded[0]) {
        bNear = 1;
    }
    bSoon = bNear;
    if (bSoon) {
        Gaud_PlayUISound(25);
    }
    pResult->i = bSoon;
}

// Front-end message 544: the console clock's date and time as text ("M/D/YYYY H:MM AM") into the
// string pArgs[0] when the clock's year is before 2003 and its month before October; otherwise an
// empty string. Gives 1 when it wrote the date.
void FE_GetDateTimeIfClockEarly(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMonth;
    s32 nYear;
    s32 nUnused;
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;
    int bShow;

    RTClock_GetCurrentTime(&nMonth, &nUnused, &nYear, &nUnused, &nUnused, &nUnused, &nUnused);
    bShow = 0;
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

// Front-end message 696 with pArgs[0] 3: real-time event award pArgs[2]. Its event's name goes into
// the string pArgs[4], its icon into the int pArgs[5] points to and the day the profile being
// worked on won it into the string pArgs[3] (empty when not won). pArgs[1], the player slot for the
// other awards, is not read. Gives whether it is won.
void TrophyRoom_GetRTEAwardStatus(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nAward = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    s32* pIcon = (s32*)pArgs[5].p;
    u8 bWon;

    GM_RealtimeMode_GetNameByTrophyGroup(nAward, ((MsgString*)pArgs[4].p)->pStr);
    *pIcon = GM_RealtimeMode_GetIconIDByTrophyGroup(nAward);
    bWon = pProfile->aRTEAward[nAward].bWon;
    if (bWon) {
        CalDate_ToString(pProfile->aRTEAward[nAward].nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

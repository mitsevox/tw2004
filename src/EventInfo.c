// EventInfo.c (our name): the front end's panel of details about a calendar day's event: for a
// PGA TOUR event the purse, round, course, leader, score and the defending champion, for a
// real-time event its rewards, status and dates. The panel functions fill one line's label and
// value each (lines 3 to 8 of the panel); three front-end messages give the next real-time event,
// the clock's date and an award.
// TW06 keeps the like in fe_calendarpopups.c, but nothing here proves the pairing.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/pgatour.h"
#include "game/modes/rte.h"

// Up to three reward names for a real-time event (nKind 0x11); how many there are.
int  fn_80106F68(int nKind, s32 nId, char* szFirst, char* szSecond, char* szThird);
void fn_800A73F0(s32 n);
void fn_8011D658(int nLine, char* szLabel, char* szValue);

// fake match: stands in for a function the original linker stripped. The file's strings start
// with "", "Purse:" and "Status:" (.sdata 0x802818A8) and "Rewards:" (.data 0x801944F8), before
// fn_8011D280's; the body is unknown.
static void EventInfo_StrippedFn(char* szLabel, char* szValue) {
    strcpy(szValue, "");
    strcpy(szLabel, "Purse:");
    strcpy(szLabel, "Status:");
    strcpy(szLabel, "Rewards:");
}

// A PGA TOUR event under way: purse, round, course, leader, the leader's score, the player's score.
void fn_8011D280(int nLine, char* szLabel, char* szValue) {
    char sz[128];
    s32 aCourses[4];
    s32 nId;
    s32 nRound;
    s32 nRounds;

    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    switch (nLine) {
    case 3:
        strcpy(szLabel, "Purse:");
        GameModeDriverPGATour_GetPurseString(nId, sz);
        sprintf(szValue, "$%s", sz);
        break;
    case 4:
        strcpy(szLabel, "Round:");
        sprintf(szValue, "%d", nRound + 1);
        break;
    case 5:
        strcpy(szLabel, "Course:");
        GameModeDriverPGATour_GetCourses(fn_800EFA70(nId), aCourses);
        nRounds = GameModeDriverPGATour_GetRounds(nId);
        if (nRound >= 0 && nRound < nRounds) {
            strcpy(szValue, lbl_80191990[aCourses[nRound]]);
        } else {
            strcpy(szValue, lbl_80191990[aCourses[0]]);
        }
        break;
    case 6:
        strcpy(szLabel, "Leader:");
        GameModeDriverPGATour_GetCurrentEventLeader(sz);
        strcpy(szValue, sz);
        break;
    case 7: {
        // Each score case has its own block-scoped local (a shared one allocates differently).
        int nScore = fn_800F009C();
        strcpy(szLabel, "Score:");
        if (nScore == 0) {
            sprintf(szValue, "E");
        } else if (nScore > 0) {
            sprintf(szValue, "+%d", nScore);
        } else {
            sprintf(szValue, "%d", nScore);
        }
        break;
    }
    case 8: {
        int nScore = fn_800F018C(nId);
        strcpy(szLabel, "Your Score:");
        if (nScore == 0) {
            sprintf(szValue, "E");
        } else if (nScore > 0) {
            sprintf(szValue, "+%d", nScore);
        } else {
            sprintf(szValue, "%d", nScore);
        }
        break;
    }
    }
}

// A PGA TOUR event played: the winner, the winning score, the winner's earnings, your finish.
void fn_8011D4DC(int nLine, char* szLabel, char* szValue) {
    char sz[128];
    s32 nId;
    s32 nRound;
    s32 nScore;

    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    switch (nLine) {
    case 3:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    case 4:
        strcpy(szLabel, "Winner:");
        GameModeDriverPGATour_GetChamp(nId, sz);
        strcpy(szValue, sz);
        break;
    case 5:
        nScore = GameModeDriverPGATour_GetChampScore(nId);
        strcpy(szLabel, "Score:");
        sprintf(szValue, "%d", nScore);
        break;
    case 6:
        strcpy(szLabel, "Earnings:");
        GameModeDriverPGATour_GetWinnerEarningsString(nId, sz);
        sprintf(szValue, "$%s", sz);
        break;
    case 7:
        strcpy(szLabel, "Your Finish:");
        GameModeDriverPGATour_GetUserFinishString(nId, sz);
        strcpy(szValue, sz);
        break;
    case 8:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    }
}

// A PGA TOUR event to come: the defending champion and winning score, the purse, the course.
void fn_8011D658(int nLine, char* szLabel, char* szValue) {
    char sz[128];
    s32 aCourses[4];
    s32 nId;
    s32 nRound;
    Tournament* pTournament;
    s32 nRounds;

    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    switch (nLine) {
    case 3:
        strcpy(szLabel, "Defending ");
        strcpy(szValue, "");
        break;
    case 4:
        strcpy(szLabel, "Champion:");
        GameModeDriverPGATour_GetChamp(nId, sz);
        strcpy(szValue, sz);
        break;
    case 5:
        strcpy(szLabel, "Winning ");
        strcpy(szValue, "");
        break;
    case 6:
        strcpy(szLabel, "Score:");
        if (GameModeDriverPGATour_GetChampScore(nId) > 0) {
            sprintf(szValue, "%d", GameModeDriverPGATour_GetChampScore(nId));
        } else {
            strcpy(szValue, "N/A");
        }
        break;
    case 7:
        strcpy(szLabel, "Purse:");
        GameModeDriverPGATour_GetPurseString(nId, sz);
        sprintf(szValue, "$%s", sz);
        break;
    case 8:
        strcpy(szLabel, "Course:");
        pTournament = fn_800EFA70(nId);
        nRounds = GameModeDriverPGATour_GetRounds(nId);
        GameModeDriverPGATour_GetCourses(pTournament, aCourses);
        if (nRound >= 0 && nRound < nRounds) {
            strcpy(szValue, lbl_80191990[aCourses[nRound]]);
        } else {
            strcpy(szValue, lbl_80191990[aCourses[0]]);
        }
        break;
    }
}

void fn_8011D858(int nLine, char* szLabel, char* szValue) {
    fn_8011D658(nLine, szLabel, szValue);
}

// A real-time event (day panel 4: today's): the purse and up to three rewards.
void fn_8011D878(int nLine, char* szLabel, char* szValue) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nId;
    s32 nRound;
    int nRewards;

    GameModeDriverRTE_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    nRewards = fn_80106F68(0x11, fn_800F120C(nId), szReward1, szReward2, szReward3);
    fn_800907AC(fn_800F0F30(nId), szMoney);
    sprintf(szPurse, "$%s", szMoney);
    switch (nLine) {
    case 3:
        strcpy(szLabel, "Purse:");
        strcpy(szValue, szPurse);
        break;
    case 4:
        strcpy(szLabel, "Rewards:");
        if (nRewards >= 1) {
            strcpy(szValue, szReward1);
        } else {
            strcpy(szValue, "N/A");
        }
        break;
    case 5:
        strcpy(szLabel, "");
        if (nRewards >= 2) {
            strcpy(szValue, szReward2);
        } else {
            strcpy(szValue, "");
        }
        break;
    case 6:
        strcpy(szLabel, "");
        if (nRewards >= 3) {
            strcpy(szValue, szReward3);
        } else {
            strcpy(szValue, "");
        }
        break;
    case 7:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    case 8:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    }
}

// A real-time event (day panel 5: an earlier day's, or today's when fn_800F102C): whether the
// player completed it, then the purse and rewards.
void fn_8011DA44(int nLine, char* szLabel, char* szValue) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nId;
    s32 nRound;
    int nRewards;
    u8 bComplete;

    GameModeDriverRTE_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    bComplete = GameModeDriverRTE_IsEventComplete(lbl_80281ED4->nSlot, nId);
    nRewards = fn_80106F68(0x11, fn_800F120C(nId), szReward1, szReward2, szReward3);
    fn_800907AC(fn_800F0F30(nId), szMoney);
    sprintf(szPurse, "$%s", szMoney);
    switch (nLine) {
    case 3:
        strcpy(szLabel, "Status:");
        if (bComplete) {
            strcpy(szValue, "COMPLETE");
        } else {
            strcpy(szValue, "INCOMPLETE");
        }
        break;
    case 4:
        strcpy(szLabel, "Purse:");
        strcpy(szValue, szPurse);
        break;
    case 5:
        strcpy(szLabel, "Rewards:");
        if (nRewards >= 1) {
            strcpy(szValue, szReward1);
        } else {
            strcpy(szValue, "N/A");
        }
        break;
    case 6:
        strcpy(szLabel, "");
        if (nRewards >= 2) {
            strcpy(szValue, szReward2);
        } else {
            strcpy(szValue, "");
        }
        break;
    case 7:
        strcpy(szLabel, "");
        if (nRewards >= 3) {
            strcpy(szValue, szReward3);
        } else {
            strcpy(szValue, "");
        }
        break;
    case 8:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    }
}

// As fn_8011D878 (the purse and up to three rewards), for a real-time event on a later day (day
// panel 6).
void fn_8011DC30(int nLine, char* szLabel, char* szValue) {
    char szReward1[36];
    char szReward2[36];
    char szReward3[36];
    char szMoney[32];
    char szPurse[32];
    s32 nId;
    s32 nRound;
    int nRewards;

    GameModeDriverRTE_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    nRewards = fn_80106F68(0x11, fn_800F120C(nId), szReward1, szReward2, szReward3);
    fn_800907AC(fn_800F0F30(nId), szMoney);
    sprintf(szPurse, "$%s", szMoney);
    switch (nLine) {
    case 3:
        strcpy(szLabel, "Purse:");
        strcpy(szValue, szPurse);
        break;
    case 4:
        strcpy(szLabel, "Rewards:");
        if (nRewards >= 1) {
            strcpy(szValue, szReward1);
        } else {
            strcpy(szValue, "N/A");
        }
        break;
    case 5:
        strcpy(szLabel, "");
        if (nRewards >= 2) {
            strcpy(szValue, szReward2);
        } else {
            strcpy(szValue, "");
        }
        break;
    case 6:
        strcpy(szLabel, "");
        if (nRewards >= 3) {
            strcpy(szValue, szReward3);
        } else {
            strcpy(szValue, "");
        }
        break;
    case 7:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    case 8:
        strcpy(szLabel, "");
        strcpy(szValue, "");
        break;
    }
}

// The next real-time event: name, "Status (?)", its start date; when it is a day or less away and
// a profile is loaded, calls GameAudio's fn_800A73F0(0x19). Gives whether it did.
void fn_8011DDFC(MsgArg* pArgs, MsgArg* pResult) {
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
    nDate = fn_800F0FBC(nEvent);
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
        fn_800A73F0(0x19);
    }
    pResult->i = bSoon;
}

// The clock's date and time as text while the clock's year is before 2003 and its month before
// October; else an empty string. Gives which.
void fn_8011DEF0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMonth;
    s32 nYear;
    s32 nUnused;
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;
    int bShow;

    fn_8011E020(&nMonth, &nUnused, &nYear, &nUnused, &nUnused, &nUnused, &nUnused);
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

// A real-time event's award: its name and prize (fn_800F11A0, fn_800F1154) and the day the player
// won it. Gives whether it is won.
void fn_8011DF90(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 nId = pArgs[2].i;
    char* szDate = ((MsgString*)pArgs[3].p)->pStr;
    s32* pPrize = (s32*)pArgs[5].p;
    u8 bWon;

    fn_800F11A0(nId, ((MsgString*)pArgs[4].p)->pStr);
    *pPrize = fn_800F1154(nId);
    bWon = pProfile->aRTEAward[nId].bWon;
    if (bWon) {
        CalDate_ToString(pProfile->aRTEAward[nId].nDate, szDate);
    } else {
        szDate[0] = '\0';
    }
    pResult->i = bWon;
}

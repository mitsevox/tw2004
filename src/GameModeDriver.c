// GameModeDriver.c (our name): the career calendar's per-mode driver. The front end's calendar
// screen calls through tables of three functions (lbl_80193E70..lbl_80193F00), indexed by
// CareerCalendar.nDriver: no career (the stubs first), the PGA TOUR season (GameModeDriverPGATour.c)
// and the real-time events (GameModeDriverRTE.c). The file ends with the calendar grid itself: the
// month shown in 35 day cells (lbl_80223C48).

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/modes/pgatour.h"
#include "game/modes/rte.h"
#include "game/modes/pgatoursim.h"
#include "frontend/fe.h"

// The front end's day-details panels (0x8011D280..).
void fn_8011D280(int nKind, char* szTitle, char* szText);
void fn_8011D4DC(int nKind, char* szTitle, char* szText);
void fn_8011D658(int nKind, char* szTitle, char* szText);
void fn_8011D858(int nKind, char* szTitle, char* szText);
void fn_8011D878(int nKind, char* szTitle, char* szText);
void fn_8011DA44(int nKind, char* szTitle, char* szText);
void fn_8011DC30(int nKind, char* szTitle, char* szText);

void GetRankText(int nPlace, char* sz);
void Calendar_GetEventNameLine(u16 nDate, char* sz);
u8   IsAMonthAhead(u32 nMonth, u32 nOther);
u8   IsAMonthBehind(u32 nMonth, u32 nOther);

// ---- no career: nothing to show -------------------------------------------------------------

// The online calendar driver's entry in the event-on-a-date table: always NULL, no day has an event
// (the online driver is a stub in this build). The PGA TOUR and real-time drivers' entries return
// the tournament or event on the date.
void* Online_GetEventInfoByDate(u16 nDate) {
    return NULL;
}

// Sets the online calendar driver up when the calendar screen switches to it: empty (the online
// driver is a stub in this build).
void Online_Init(void) {
}

// Whether the online calendar is at its earliest month (then it cannot go back one): never, 0
// (stub).
u8 Online_AtEarliest(void) {
    return 0;
}

// Whether the online calendar is at its latest month (then it cannot go on one): never, 0 (stub).
u8 Online_AtLatest(void) {
    return 0;
}

// The online calendar's day cell: leaves the text, color and state as they are and returns -1, no
// icon (stub).
s32 Online_FillCell(char* sz, u16 nDate, s32* pLook, s32* pButton) {
    return -1;
}

// The online calendar's header lines: leaves the text as it is (stub).
void Online_GetLine(int nLine, char* sz) {
}

// The online calendar's line under "Selected Day:": leaves the text as it is (stub).
void Online_GetBottomLine(u16 nDate, int n, char* sz) {
}

// The online calendar's day-details popup for a date: -1, none (stub).
s32 Online_GetPopupType(u16 nDate) {
    return -1;
}

// A row of the online calendar's day-details popup: leaves the title and text as they are (stub).
void Online_GetPopupRow(int nKind, char* szTitle, char* szText) {
}

// The online calendar's current day: today's date from the clock (CalDate_GetToday).
u16 Online_GetCurrentDay(void) {
    return CalDate_GetToday();
}

// The name of the online event on a date: always "" (stub).
char* Online_GetEventName(u16 nDate) {
    return "";
}

// The online calendar's play button: empty (stub).
void Online_Play(void) {
}

// Whether events must be simulated before the online calendar's selected day can be played: never,
// 0 (stub).
u8 Online_IsSimulationNecessary(void) {
    return 0;
}

// ---- the PGA TOUR season ----------------------------------------------------------------------

// Sets the PGA TOUR calendar driver up when the calendar screen switches to it: empty in this
// build.
void PGATour_Init(void) {
}

// Whether the PGA TOUR calendar shows January of the year of the career's current day
// (lbl_80223C48.nToday): the season's first month, the calendar cannot go back from it. 1 there,
// else 0.
u8 PGATour_AtEarliest(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    CalDate_GetMDY(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 1 && lbl_80223C48.nYear == nYear) {
        b = 1;
    }
    return b;
}

// Whether the PGA TOUR calendar shows December of the year of the career's current day: the
// season's last month, the calendar cannot go on from it. 1 there, else 0.
u8 PGATour_AtLatest(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    CalDate_GetMDY(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 12 && lbl_80223C48.nYear == nYear) {
        b = 1;
    }
    return b;
}

void  PGATour_GetLine(int nLine, char* sz);
void  PGATour_GetBottomLine(u16 nDate, int n, char* sz);
s32   PGATour_FillCell(char* sz, u16 nDate, s32* pLook, s32* pButton);
s32   PGATour_GetPopupType(u16 nDate);
void  PGATour_GetPopupRow(int nKind, char* szTitle, char* szText);
u16   PGATour_GetCurrentDay(void);
char* PGATour_GetEventName(u16 nDate);
void  PGATour_Play(void);
u8    PGATour_IsSimulationNecessary(void);
void  RealTime_Init(void);
u8    RealTime_AtEarliest(void);
u8    RealTime_AtLatest(void);
s32   RealTime_FillCell(char* sz, u16 nDate, s32* pLook, s32* pButton);
void  RealTime_GetLine(int nLine, char* sz);
void  RealTime_GetBottomLine(u16 nDate, int n, char* sz);
s32   RealTime_GetPopupType(u16 nDate);
void  RealTime_GetPopupRow(int nKind, char* szTitle, char* szText);
u16   RealTime_GetCurrentDay(void);
char* RealTime_GetEventName(u16 nDate);
void  RealTime_Play(void);
u8    RealTime_IsSimulationNecessary(void);

// The calendar screen's tables, indexed by CareerCalendar.nDriver: no career, the PGA TOUR season,
// the real-time events. (Defined here, before the strings below, to keep EA's data order.)
u8 (*lbl_80193E70[3])(void) = { Online_AtEarliest, PGATour_AtEarliest, RealTime_AtEarliest };
u8 (*lbl_80193E7C[3])(void) = { Online_AtLatest, PGATour_AtLatest, RealTime_AtLatest };
s32 (*lbl_80193E88[3])(char* sz, u16 nDate, s32* pLook, s32* pButton) = {
    Online_FillCell, PGATour_FillCell, RealTime_FillCell
};
void (*lbl_80193E94[3])(int nLine, char* sz) = { Online_GetLine, PGATour_GetLine, RealTime_GetLine };
void (*lbl_80193EA0[3])(u16 nDate, int n,
                        char* sz) = { Online_GetBottomLine, PGATour_GetBottomLine, RealTime_GetBottomLine };
s32 (*lbl_80193EAC[3])(u16 nDate) = { Online_GetPopupType, PGATour_GetPopupType, RealTime_GetPopupType };
void (*lbl_80193EB8[3])(int nKind, char* szTitle, char* szText) = {
    Online_GetPopupRow, PGATour_GetPopupRow, RealTime_GetPopupRow
};
u16 (*lbl_80193EC4[3])(void) = { Online_GetCurrentDay, PGATour_GetCurrentDay, RealTime_GetCurrentDay };
void (*lbl_80193ED0[3])(void) = { Online_Init, PGATour_Init, RealTime_Init };
char* (*lbl_80193EDC[3])(u16 nDate) = { Online_GetEventName, PGATour_GetEventName, RealTime_GetEventName };
// port: the PGA TOUR and real-time drivers return their own event types through this void* entry
void* (*lbl_80193EE8[3])(u16 nDate) = {
    Online_GetEventInfoByDate, (void* (*)(u16))fn_800EFC80, (void* (*)(u16))GM_RealtimeMode_GetEventInfoByDate
};
void (*lbl_80193EF4[3])(void) = { Online_Play, PGATour_Play, RealTime_Play };
u8 (*lbl_80193F00[3])(void) = { Online_IsSimulationNecessary, PGATour_IsSimulationNecessary, RealTime_IsSimulationNecessary };

// The calendar grid.
CareerCalendar lbl_80223C48;

// The PGA TOUR calendar's header lines: line 1 today's tournament and round (Calendar_GetEventNameLine), or
// "Season Complete" once the season has no tournament left; line 2 empty. Other lines leave the
// text as it is.
void PGATour_GetLine(int nLine, char* sz) {
    switch (nLine) {
    case 1:
        if (lbl_80223C48.bSeasonOver) {
            strcpy(sz, "Season Complete");
            return;
        }
        Calendar_GetEventNameLine(lbl_80223C48.nToday, sz);
        return;
    case 2:
        sz[0] = 0;
        return;
    }
}

// The PGA TOUR calendar's line under "Selected Day:" for a date: "Event: <name>, Round <n>" (the
// round the tournament plays that day), or "No Event Scheduled". The line number is not used.
void PGATour_GetBottomLine(u16 nDate, int n, char* sz) {
    char* szName = lbl_80193EDC[lbl_80223C48.nDriver](nDate);
    s32 nId;
    s32 nRound;

    GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound);
    if (szName[0] == 0) {
        strcpy(sz, "No Event Scheduled");
        return;
    }
    sprintf(sz, "Event: %s, Round %d", szName, nRound + 1);
}

// A finishing place as text: "1st", "2nd", "3rd", "4th"..., by its last digit; 0 gives "".
// EA bug: 11, 12 and 13 come out as "11st", "12nd" and "13rd".
void GetRankText(int nPlace, char* sz) {
    if (nPlace == 0) {
        sz[0] = 0;
        return;
    }
    switch (nPlace % 10) {
    case 1:
        sprintf(sz, "%dst", nPlace);
        return;
    case 2:
        sprintf(sz, "%dnd", nPlace);
        return;
    case 3:
        sprintf(sz, "%drd", nPlace);
        return;
    default:
        sprintf(sz, "%dth", nPlace);
        return;
    }
}

// The PGA TOUR calendar's day cell for a date. On a tournament day: the text is START (today, its
// first round), CONTINUE (today, a later round; both on a second line) or empty, and *pCellState is
// 1 START, 2 CONTINUE, 3 the first day of a later tournament than the current one, 4 no button (any
// other day, and today once the season is over); on the day before today in the current tournament
// the text becomes the player's place in it (GetRankText of
// GM_PgaTourSim_GetScoreRankFromEntrantID). *pCellColor: 4 the current tournament, 5 a past
// tournament the profile won, 2 other past days (today too once the season is over), 3 days to
// come. Returns the tournament's calendar icon (fn_800EFE3C, its n10) on its last day, else -1. A
// day without a tournament gets empty text, *pCellState 0 and -1.
s32 PGATour_FillCell(char* sz, u16 nDate, s32* pLook, s32* pButton) {
    s32 nId;
    s32 nRound;
    s32 nSelRound;
    s32 nRound2;
    u8 bSelected;

    GameModeDriverPGATour_GetSelectedEvent(&nSelRound);
    if (GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound)) {
        bSelected = nId == GameModeDriverPGATour_GetSelectedEvent(&nRound2);
        fn_800EFA70(nId);
        if (nDate == lbl_80223C48.nToday) {
            if (lbl_80223C48.bSeasonOver) {
                strcpy(sz, "");
                *pButton = 4;
            } else if (nSelRound == 0) {
                strcpy(sz, "\nSTART");
                *pButton = 1;
            } else {
                strcpy(sz, "\nCONTINUE");
                *pButton = 2;
            }
        } else if (nRound == 0 && bSelected == 0 && nDate > lbl_80223C48.nToday) {
            *pButton = 3;
            strcpy(sz, "");
        } else {
            *pButton = 4;
            strcpy(sz, "");
        }
        if (nDate == lbl_80223C48.nToday - 1 && bSelected) {
            GetRankText(GM_PgaTourSim_GetScoreRankFromEntrantID(fn_80077B08(), 0), sz);
        }
        if (bSelected) {
            *pLook = 4;
        } else if (FE_GetCurrentProfile()->aC8[nId].award.bWon &&
                   (nDate < lbl_80223C48.nToday ||
                    (lbl_80223C48.bSeasonOver && nDate == lbl_80223C48.nToday))) {
            *pLook = 5;
        } else if (nDate < lbl_80223C48.nToday ||
                   (lbl_80223C48.bSeasonOver && nDate == lbl_80223C48.nToday)) {
            *pLook = 2;
        } else {
            *pLook = 3;
        }
        if (nRound == GameModeDriverPGATour_GetRounds(nId) - 1) {
            return fn_800EFE3C(nId);
        }
        return -1;
    }
    *pButton = 0;
    sz[0] = 0;
    return -1;
}

// Which day-details popup the PGA TOUR calendar shows for a date's tournament (the calendar screen
// keeps it in lbl_80223C48.n1C for PGATour_GetPopupRow), by its start against the current
// tournament's: 1 results (an earlier tournament, or any while player 1 has no current tournament,
// fn_800F0428(0) == -1), 3 the current one before it starts (today is its first round), 0 the
// current one in progress, 2 upcoming (a later one).
s32 PGATour_GetPopupType(u16 nDate) {
    s32 nId;
    s32 nTodayId;
    s32 nTodayRound;
    s32 nRound;
    u16 nTodayStart;
    u16 nStart;
    s32 nResult;

    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nToday, &nTodayId, &nTodayRound);
    GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound);
    nTodayStart = fn_800EFD38(nTodayId);
    nStart = fn_800EFD38(nId);
    if (fn_800F0428(0) == -1) {
        return 1;
    }
    if (nStart < nTodayStart) {
        return 1;
    }
    if (nStart == nTodayStart) {
        nResult = 0;
        if (nTodayRound == 0) {
            nResult = 3;
        }
        return nResult;
    }
    return 2;
}

// A row of the PGA TOUR calendar's day-details popup (row 0, "Event:", is the calendar screen's):
// row 1 blank, row 2 "Dates:" and the selected day's tournament's first and last days ("<start> -
// <end>"); the other rows come from the popup lbl_80223C48.n1C names (0 in progress fn_8011D280, 1
// results fn_8011D4DC, 2 upcoming fn_8011D658, 3 before it starts fn_8011D858).
void PGATour_GetPopupRow(int nKind, char* szTitle, char* szText) {
    s32 nId;
    s32 nRound;
    char szStart[12];
    char szEnd[12];
    u16 nStart;
    u16 nEnd;

    switch (nKind) {
    case 1:
        szTitle[0] = 0;
        szText[0] = 0;
        return;
    case 2:
        GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
        nStart = fn_800EFD38(nId);
        nEnd = GameModeDriverPGATour_GetEndDate(nId);
        CalDate_ToString(nStart, szStart);
        CalDate_ToString(nEnd, szEnd);
        strcpy(szTitle, "Dates:");
        sprintf(szText, "%s - %s", szStart, szEnd);
        return;
    default:
        switch (lbl_80223C48.n1C) {
        case 0:
            fn_8011D280(nKind, szTitle, szText);
            return;
        case 1:
            fn_8011D4DC(nKind, szTitle, szText);
            return;
        case 2:
            fn_8011D658(nKind, szTitle, szText);
            return;
        case 3:
            fn_8011D858(nKind, szTitle, szText);
            return;
        }
        break;
    }
}

// The PGA TOUR career's current day: the first day of player 1's current tournament plus the round
// they are on. 0xFFFF once the season has no tournament left (fn_800EFD38 of none), which
// ResetCalendarState checks for.
u16 PGATour_GetCurrentDay(void) {
    s32 nRound;
    u16 nDate;

    nDate = fn_800EFD38(GameModeDriverPGATour_GetSelectedEvent(&nRound));
    CalDate_AddDays(&nDate, nRound);
    return nDate;
}

// The name of the PGA TOUR tournament on a date, or "" when none is played that day.
char* PGATour_GetEventName(u16 nDate) {
    s32 nId;
    s32 nRound;

    if (GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound)) {
        return GameModeDriverPGATour_GetName(nId);
    }
    return "";
}

// The PGA TOUR calendar's play button: player 0 plays the first created golfer (30), the game mode
// becomes 23 (the PGA TOUR season), and the season skips ahead to the selected day's tournament
// (fn_800EF9D0: the tournaments before it are played out).
void PGATour_Play(void) {
    s32 nId;
    s32 nRound;

    Session_SetGolfer(30, 0);
    GM_SetModeType(23);
    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    fn_800EF9D0(nId);
}

// Whether tournaments must be simulated before the PGA TOUR calendar's selected day can be played:
// 1 when the selected day's tournament is not player 1's current one (PGATour_Play then plays out
// the ones before it).
u8 PGATour_IsSimulationNecessary(void) {
    s32 nId;
    s32 nSel;
    s32 nRound;

    nSel =GameModeDriverPGATour_GetSelectedEvent(&nRound);
    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    return nSel != nId;
}

// ---- the real-time events ---------------------------------------------------------------------

// Sets the real-time events calendar driver up when the calendar screen switches to it: empty in
// this build.
void RealTime_Init(void) {
}

// Whether the real-time events calendar shows January of the year of the current day
// (lbl_80223C48.nToday): its first month, the calendar cannot go back from it. 1 there, else 0.
u8 RealTime_AtEarliest(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    CalDate_GetMDY(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 1 && lbl_80223C48.nYear == nYear) {
        b = 1;
    }
    return b;
}

// Whether the real-time events calendar shows January of the year after the current day's: its last
// month, the calendar cannot go on from it. 1 there, else 0.
u8 RealTime_AtLatest(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    CalDate_GetMDY(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 1 && lbl_80223C48.nYear == nYear + 1) {
        b = 1;
    }
    return b;
}

// The real-time events calendar's day cell: never any text (color and state are left as they are);
// returns the event's calendar icon (fn_800F1008, its n14), or -1 on a day without an event. The
// event's GameModeDriverRTE_GetCalData is fetched and not used.
s32 RealTime_FillCell(char* sz, u16 nDate, s32* pLook, s32* pButton) {
    s32 nId;
    s32 nRound;

    if (GameModeDriverRTE_GetEventByDate(nDate, &nId, &nRound)) {
        GameModeDriverRTE_GetCalData(nId);
        sz[0] = 0;
        return GameModeDriverRTE_UI_GetEventIconIndexOnCal(nId);
    }
    sz[0] = 0;
    return -1;
}

// The real-time events calendar's header lines: line 1 "Today: " and the clock's date and time
// (RTClock_GetDateTimeString), line 2 today's event (Calendar_GetEventNameLine). Other lines leave
// the text as it is.
void RealTime_GetLine(int nLine, char* sz) {
    char szDate[32];

    switch (nLine) {
    case 1:
        RTClock_GetDateTimeString(szDate);
        sprintf(sz, "Today: %s", szDate);
        return;
    case 2:
        Calendar_GetEventNameLine(lbl_80223C48.nToday, sz);
        return;
    }
}

// The real-time events calendar's line under "Selected Day:": the date's event
// (Calendar_GetEventNameLine). The line number is not used.
void RealTime_GetBottomLine(u16 nDate, int n, char* sz) {
    Calendar_GetEventNameLine(nDate, sz);
}

// Which day-details popup the real-time events calendar shows for a date (the calendar screen keeps
// it in lbl_80223C48.n1C for RealTime_GetPopupRow): 5 results for a past day; 4 today's event, or 5
// once it is completed (fn_800F102C, always 0 in this build); 6 upcoming for a day to come.
s32 RealTime_GetPopupType(u16 nDate) {
    if (nDate < lbl_80223C48.nToday) {
        return 5;
    }
    if (nDate == lbl_80223C48.nToday) {
        return (GM_RealtimeMode_TodaysEventCompleted() != 0) + 4;
    }
    return 6;
}

// A row of the real-time events calendar's day-details popup (row 0, "Event:", is the calendar
// screen's): row 1 "Date:" and the selected day, row 2 blank; the other rows come from the popup
// lbl_80223C48.n1C names (4 today's event fn_8011D878, 5 results fn_8011DA44, 6 upcoming
// fn_8011DC30).
void RealTime_GetPopupRow(int nKind, char* szTitle, char* szText) {
    char szDate[12];

    switch (nKind) {
    case 1:
        CalDate_ToString(lbl_80223C48.nSelected, szDate);
        strcpy(szTitle, "Date:");
        strcpy(szText, szDate);
        return;
    case 2:
        szTitle[0] = 0;
        szText[0] = 0;
        return;
    default:
        switch (lbl_80223C48.n1C) {
        case 4:
            fn_8011D878(nKind, szTitle, szText);
            return;
        case 5:
            fn_8011DA44(nKind, szTitle, szText);
            return;
        case 6:
            fn_8011DC30(nKind, szTitle, szText);
            return;
        }
        break;
    }
}

// The real-time events calendar's current day: today's date from the clock (CalDate_GetToday).
u16 RealTime_GetCurrentDay(void) {
    return CalDate_GetToday();
}

// The name of the real-time event on a date, or "" when there is none that day.
char* RealTime_GetEventName(u16 nDate) {
    s32 nId;
    s32 nRound;

    if (GameModeDriverRTE_GetEventByDate(nDate, &nId, &nRound)) {
        return GameModeDriverRTE_GetName(nId);
    }
    return "";
}

// The real-time events calendar's play button: player 0 plays the first created golfer (30), the
// game mode becomes 24 (the real-time events), and today's event by the clock becomes the current
// one (fn_800F0E3C).
void RealTime_Play(void) {
    Session_SetGolfer(30, 0);
    GM_SetModeType(24);
    GM_RealtimeMode_SelectEventToday();
}

// Whether events must be simulated before the real-time events calendar's selected day can be
// played: never, 0.
u8 RealTime_IsSimulationNecessary(void) {
    return 0;
}

// ---- the calendar -----------------------------------------------------------------------------

// Opens the calendar on the driver's current day (lbl_80223C48.nToday; once the PGA TOUR season has
// no tournament left, the last tournament's last day, with bSeasonOver set) and shows the month it
// falls in; when that day's cell would lie past the grid's 35 cells the month after is shown
// instead (not from December). Called when the calendar screen switches driver and when a new
// season starts.
void ResetCalendarState(void) {
    u16 nDate;
    s32 nMonth;
    s32 nDay;
    s32 nYear;

    lbl_80223C48.bSeasonOver = 0;
    nDate = lbl_80193EC4[lbl_80223C48.nDriver]();
    if (nDate == 0xFFFF) {
        nDate = GameModeDriverPGATour_GetEndDate(GameModeDriverPGATour_GetFinalEventOfSeason());
        lbl_80223C48.bSeasonOver = 1;
    }
    lbl_80223C48.nToday = nDate;
    CalDate_GetMDY(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    lbl_80223C48.nMonth = nMonth;
    lbl_80223C48.nYear = nYear;
    UpdateCalendarState();
    if ((u32)(lbl_80223C48.nFirstCell + nDay) > 35 && lbl_80223C48.nMonth != 12) {
        lbl_80223C48.nMonth++;
        UpdateCalendarState();
    }
}

// A day's event as text: "No Event Scheduled", on the PGA TOUR "Active Event: <name>, Round <n>"
// (the round of the event on nToday, not nDate), else "Event: <name>".
void Calendar_GetEventNameLine(u16 nDate, char* sz) {
    char* szName = lbl_80193EDC[lbl_80223C48.nDriver](nDate);
    s32 nId;
    s32 nRound;

    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nToday, &nId, &nRound);
    if (szName[0] == 0) {
        strcpy(sz, "No Event Scheduled");
        return;
    }
    if (lbl_80223C48.nDriver == 1) {
        sprintf(sz, "Active Event: %s, Round %d", szName, nRound + 1);
        return;
    }
    sprintf(sz, "Event: %s", szName);
}

// Lays the month shown (lbl_80223C48.nMonth and nYear) out on the 35-cell grid: the cell of its
// first day (its day of the week, counted from 0), the cell after its last day, and the number of
// days in the month before. Called whenever the month shown changes.
void UpdateCalendarState(void) {
    int nDays;
    u16 nDate;
    s32 nMonth;
    s32 nYear;

    nDays = DaysInMonth(lbl_80223C48.nMonth, lbl_80223C48.nYear);
    CalDate_SetMDY(&nDate, lbl_80223C48.nMonth, 1, lbl_80223C48.nYear);
    lbl_80223C48.nFirstCell = CalDate_GetDayOfWeek(&nDate);
    lbl_80223C48.nFirstCell = lbl_80223C48.nFirstCell - 1;
    lbl_80223C48.nEndCell = lbl_80223C48.nFirstCell + nDays;
    CalDate_GetPrevMonth(lbl_80223C48.nMonth, lbl_80223C48.nYear, &nMonth, &nYear);
    lbl_80223C48.nPrevMonthDays = DaysInMonth(nMonth, nYear);
}

// The date in a cell of the grid (cells before day 1 belong to the month before, those after the
// last day to the month after).
u16 GetDateFromCellIndex(u32 nCell) {
    u16 nDate;
    s32 nMonth;
    s32 nYear;
    int nDay;

    nMonth = 0;
    nYear = 0;
    if (nCell < lbl_80223C48.nFirstCell) {
        nDay = 1 + (lbl_80223C48.nPrevMonthDays - lbl_80223C48.nFirstCell) + nCell;
        CalDate_GetPrevMonth(lbl_80223C48.nMonth, lbl_80223C48.nYear, &nMonth, &nYear);
    } else if (nCell >= lbl_80223C48.nEndCell) {
        nDay = nCell - (lbl_80223C48.nEndCell - 1);
        CalDate_GetNextMonth(lbl_80223C48.nMonth, lbl_80223C48.nYear, &nMonth, &nYear);
    } else {
        nDay = (nCell - lbl_80223C48.nFirstCell) + 1;
        nMonth = lbl_80223C48.nMonth;
        nYear = lbl_80223C48.nYear;
    }
    CalDate_SetMDY(&nDate, nMonth, nDay, nYear);
    return nDate;
}

// nOther is the month after nMonth (December to January included).
u8 IsAMonthAhead(u32 nMonth, u32 nOther) {
    int b = 0;
    if (nMonth + 1 == nOther || (nMonth == 12 && nOther == 1)) {
        b = 1;
    }
    return b;
}

// nOther is the month before nMonth (January to December included).
u8 IsAMonthBehind(u32 nMonth, u32 nOther) {
    int b = 0;
    if (nMonth - 1 == nOther || (nMonth == 1 && nOther == 12)) {
        b = 1;
    }
    return b;
}

// The grid cell a date falls in, or -1 when it is not shown.
s32 GetCellIndexFromDate(u16 nDate) {
    u16 nCopy;
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    u32 nPrevShown;

    nCopy = nDate;
    nPrevShown = lbl_80223C48.nPrevMonthDays - lbl_80223C48.nFirstCell;
    CalDate_GetMDY(&nCopy, &nMonth, &nDay, &nYear);
    // fake match: the original compares the months unsigned (cmplw); both are 1..12.
    if ((u32)lbl_80223C48.nMonth == nMonth) {
        return nDay + (s32)lbl_80223C48.nFirstCell - 1;
    }
    if (IsAMonthAhead(lbl_80223C48.nMonth, nMonth) && nDay < 35 - lbl_80223C48.nEndCell) {
        return nDay + (s32)lbl_80223C48.nEndCell - 1;
    }
    if (IsAMonthBehind(lbl_80223C48.nMonth, nMonth) && nDay > nPrevShown) {
        return (nDay - nPrevShown) - 1;
    }
    return -1;
}

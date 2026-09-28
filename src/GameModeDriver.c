// GameModeDriver.c (our name; EA's FE_Calendar.c, TW07 PS3 has every function here in the same
// order): the front end's calendar, the month grid a career is played from. The calendar screen
// (CalendarScreen.c) asks the driver CalendarState.nDriver for everything it shows, through the
// per-driver tables gCalendarAtEarliest..gCalendarIsSimulationNecessary: 0 the online tournaments
// (Online_: stubs in this build), 1 the PGA TOUR season (PGATour_, game mode 23,
// GameModeDriverPGATour.c) and 2 the real-time events (RealTime_, game mode 24,
// GameModeDriverRTE.c). Each driver says whether the month can move (AtEarliest / AtLatest), fills a
// day cell (FillCell), the header lines and the line under the selected day, picks and fills the
// day-details popup (GetPopupType / GetPopupRow; the rows themselves are EventInfo.c's), gives the
// current day and a day's event, and starts play (Play; IsSimulationNecessary when events before
// the selected day must be played out first). The file ends with the grid itself: the month shown
// laid out in 35 day cells (CalendarState, ResetCalendarState / UpdateCalendarState, and the
// conversions between a cell and its date).

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/modes/pgatour.h"
#include "game/modes/rte.h"
#include "game/modes/pgatoursim.h"
#include "frontend/fe.h"

// The rows of the day-details popups (EventInfo.c): PGA TOUR in progress, results, upcoming,
// before it starts; real-time today's event, results, upcoming.
void fn_8011D280(int nRow, char* szTitle, char* szText);
void fn_8011D4DC(int nRow, char* szTitle, char* szText);
void fn_8011D658(int nRow, char* szTitle, char* szText);
void fn_8011D858(int nRow, char* szTitle, char* szText);
void fn_8011D878(int nRow, char* szTitle, char* szText);
void fn_8011DA44(int nRow, char* szTitle, char* szText);
void fn_8011DC30(int nRow, char* szTitle, char* szText);

void GetRankText(int nRank, char* sz);
void Calendar_GetEventNameLine(u16 nDate, char* sz);
u8   IsAMonthAhead(u32 nMonth, u32 nOther);
u8   IsAMonthBehind(u32 nMonth, u32 nOther);

// ---- the online tournaments: stubs in this build ----------------------------------------------

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
s32 Online_FillCell(char* sz, u16 nDate, s32* pCellColor, s32* pCellState) {
    return -1;
}

// The online calendar's header lines: leaves the text as it is (stub).
void Online_GetLine(int nLine, char* sz) {
}

// The online calendar's line under "Selected Day:": leaves the text as it is (stub).
void Online_GetBottomLine(u16 nDate, int nLine, char* sz) {
}

// The online calendar's day-details popup for a date: -1, none (stub).
s32 Online_GetPopupType(u16 nDate) {
    return -1;
}

// A row of the online calendar's day-details popup: leaves the title and text as they are (stub).
void Online_GetPopupRow(int nRow, char* szTitle, char* szText) {
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
// (CalendarState.nToday): the season's first month, the calendar cannot go back from it. 1 there,
// else 0.
u8 PGATour_AtEarliest(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    CalDate_GetMDY(&CalendarState.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (CalendarState.nMonth == 1 && CalendarState.nYear == nYear) {
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

    CalDate_GetMDY(&CalendarState.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (CalendarState.nMonth == 12 && CalendarState.nYear == nYear) {
        b = 1;
    }
    return b;
}

void  PGATour_GetLine(int nLine, char* sz);
void  PGATour_GetBottomLine(u16 nDate, int nLine, char* sz);
s32   PGATour_FillCell(char* sz, u16 nDate, s32* pCellColor, s32* pCellState);
s32   PGATour_GetPopupType(u16 nDate);
void  PGATour_GetPopupRow(int nRow, char* szTitle, char* szText);
u16   PGATour_GetCurrentDay(void);
char* PGATour_GetEventName(u16 nDate);
void  PGATour_Play(void);
u8    PGATour_IsSimulationNecessary(void);
void  RealTime_Init(void);
u8    RealTime_AtEarliest(void);
u8    RealTime_AtLatest(void);
s32   RealTime_FillCell(char* sz, u16 nDate, s32* pCellColor, s32* pCellState);
void  RealTime_GetLine(int nLine, char* sz);
void  RealTime_GetBottomLine(u16 nDate, int nLine, char* sz);
s32   RealTime_GetPopupType(u16 nDate);
void  RealTime_GetPopupRow(int nRow, char* szTitle, char* szText);
u16   RealTime_GetCurrentDay(void);
char* RealTime_GetEventName(u16 nDate);
void  RealTime_Play(void);
u8    RealTime_IsSimulationNecessary(void);

// The calendar screen's per-driver tables, indexed by CalendarState.nDriver: 0 online, 1 the PGA
// TOUR season, 2 the real-time events. (Defined here, before the strings below, to keep EA's data
// order.)
u8 (*gCalendarAtEarliest[3])(void) = { Online_AtEarliest, PGATour_AtEarliest, RealTime_AtEarliest };
u8 (*gCalendarAtLatest[3])(void) = { Online_AtLatest, PGATour_AtLatest, RealTime_AtLatest };
s32 (*gCalendarFillCell[3])(char* sz, u16 nDate, s32* pCellColor, s32* pCellState) = {
    Online_FillCell, PGATour_FillCell, RealTime_FillCell
};
void (*gCalendarGetLine[3])(int nLine, char* sz) = {
    Online_GetLine, PGATour_GetLine, RealTime_GetLine
};
void (*gCalendarGetBottomLine[3])(u16 nDate, int nLine, char* sz) = {
    Online_GetBottomLine, PGATour_GetBottomLine, RealTime_GetBottomLine
};
s32 (*gCalendarGetPopupType[3])(u16 nDate) = {
    Online_GetPopupType, PGATour_GetPopupType, RealTime_GetPopupType
};
void (*gCalendarGetPopupRow[3])(int nRow, char* szTitle, char* szText) = {
    Online_GetPopupRow, PGATour_GetPopupRow, RealTime_GetPopupRow
};
u16 (*gCalendarGetCurrentDay[3])(void) = {
    Online_GetCurrentDay, PGATour_GetCurrentDay, RealTime_GetCurrentDay
};
void (*gCalendarInit[3])(void) = { Online_Init, PGATour_Init, RealTime_Init };
char* (*gCalendarGetEventName[3])(u16 nDate) = {
    Online_GetEventName, PGATour_GetEventName, RealTime_GetEventName
};
// port: the PGA TOUR and real-time drivers return their own event types through this void* entry
void* (*gCalendarGetEventInfoByDate[3])(u16 nDate) = {
    Online_GetEventInfoByDate, (void* (*)(u16))fn_800EFC80, (void* (*)(u16))GM_RealtimeMode_GetEventInfoByDate
};
void (*gCalendarPlay[3])(void) = { Online_Play, PGATour_Play, RealTime_Play };
u8 (*gCalendarIsSimulationNecessary[3])(void) = {
    Online_IsSimulationNecessary, PGATour_IsSimulationNecessary, RealTime_IsSimulationNecessary
};

// The calendar: its driver, the current and selected days, the month shown and its layout on the
// 35-cell grid, the day-details popup shown (EA's CalendarState, TW07).
CareerCalendar CalendarState;

// The PGA TOUR calendar's header lines: line 1 today's tournament and round
// (Calendar_GetEventNameLine), or "Season Complete" once the season has no tournament left; line 2
// empty. Other lines leave the text as it is.
void PGATour_GetLine(int nLine, char* sz) {
    switch (nLine) {
    case 1:
        if (CalendarState.bSeasonOver) {
            strcpy(sz, "Season Complete");
            return;
        }
        Calendar_GetEventNameLine(CalendarState.nToday, sz);
        return;
    case 2:
        sz[0] = 0;
        return;
    }
}

// The PGA TOUR calendar's line under "Selected Day:" for a date: "Event: <name>, Round <n>" (the
// round the tournament plays that day), or "No Event Scheduled". The line number is not used.
void PGATour_GetBottomLine(u16 nDate, int nLine, char* sz) {
    char* szName = gCalendarGetEventName[CalendarState.nDriver](nDate);
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
void GetRankText(int nRank, char* sz) {
    if (nRank == 0) {
        sz[0] = 0;
        return;
    }
    switch (nRank % 10) {
    case 1:
        sprintf(sz, "%dst", nRank);
        return;
    case 2:
        sprintf(sz, "%dnd", nRank);
        return;
    case 3:
        sprintf(sz, "%drd", nRank);
        return;
    default:
        sprintf(sz, "%dth", nRank);
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
s32 PGATour_FillCell(char* sz, u16 nDate, s32* pCellColor, s32* pCellState) {
    s32 nId;
    s32 nRound;
    s32 nSelRound;
    s32 nRound2;
    u8 bSelected;

    GameModeDriverPGATour_GetSelectedEvent(&nSelRound);
    if (GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound)) {
        bSelected = nId == GameModeDriverPGATour_GetSelectedEvent(&nRound2);
        fn_800EFA70(nId);
        if (nDate == CalendarState.nToday) {
            if (CalendarState.bSeasonOver) {
                strcpy(sz, "");
                *pCellState = 4;
            } else if (nSelRound == 0) {
                strcpy(sz, "\nSTART");
                *pCellState = 1;
            } else {
                strcpy(sz, "\nCONTINUE");
                *pCellState = 2;
            }
        } else if (nRound == 0 && bSelected == 0 && nDate > CalendarState.nToday) {
            *pCellState = 3;
            strcpy(sz, "");
        } else {
            *pCellState = 4;
            strcpy(sz, "");
        }
        if (nDate == CalendarState.nToday - 1 && bSelected) {
            GetRankText(GM_PgaTourSim_GetScoreRankFromEntrantID(fn_80077B08(), 0), sz);
        }
        if (bSelected) {
            *pCellColor = 4;
        } else if (FE_GetCurrentProfile()->aC8[nId].award.bWon &&
                   (nDate < CalendarState.nToday ||
                    (CalendarState.bSeasonOver && nDate == CalendarState.nToday))) {
            *pCellColor = 5;
        } else if (nDate < CalendarState.nToday ||
                   (CalendarState.bSeasonOver && nDate == CalendarState.nToday)) {
            *pCellColor = 2;
        } else {
            *pCellColor = 3;
        }
        if (nRound == GameModeDriverPGATour_GetRounds(nId) - 1) {
            return fn_800EFE3C(nId);
        }
        return -1;
    }
    *pCellState = 0;
    sz[0] = 0;
    return -1;
}

// Which day-details popup the PGA TOUR calendar shows for a date's tournament (the calendar screen
// keeps it in CalendarState.n1C for PGATour_GetPopupRow), by its start against the current
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

    GameModeDriverPGATour_GetEventByDate(CalendarState.nToday, &nTodayId, &nTodayRound);
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
// <end>"); the other rows come from the popup CalendarState.n1C names (0 in progress fn_8011D280, 1
// results fn_8011D4DC, 2 upcoming fn_8011D658, 3 before it starts fn_8011D858).
void PGATour_GetPopupRow(int nRow, char* szTitle, char* szText) {
    s32 nId;
    s32 nRound;
    char szStart[12];
    char szEnd[12];
    u16 nStart;
    u16 nEnd;

    switch (nRow) {
    case 1:
        szTitle[0] = 0;
        szText[0] = 0;
        return;
    case 2:
        GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
        nStart = fn_800EFD38(nId);
        nEnd = GameModeDriverPGATour_GetEndDate(nId);
        CalDate_ToString(nStart, szStart);
        CalDate_ToString(nEnd, szEnd);
        strcpy(szTitle, "Dates:");
        sprintf(szText, "%s - %s", szStart, szEnd);
        return;
    default:
        switch (CalendarState.n1C) {
        case 0:
            fn_8011D280(nRow, szTitle, szText);
            return;
        case 1:
            fn_8011D4DC(nRow, szTitle, szText);
            return;
        case 2:
            fn_8011D658(nRow, szTitle, szText);
            return;
        case 3:
            fn_8011D858(nRow, szTitle, szText);
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
    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
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
    GameModeDriverPGATour_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
    return nSel != nId;
}

// ---- the real-time events ---------------------------------------------------------------------

// Sets the real-time events calendar driver up when the calendar screen switches to it: empty in
// this build.
void RealTime_Init(void) {
}

// Whether the real-time events calendar shows January of the year of the current day
// (CalendarState.nToday): its first month, the calendar cannot go back from it. 1 there, else 0.
u8 RealTime_AtEarliest(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    CalDate_GetMDY(&CalendarState.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (CalendarState.nMonth == 1 && CalendarState.nYear == nYear) {
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

    CalDate_GetMDY(&CalendarState.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (CalendarState.nMonth == 1 && CalendarState.nYear == nYear + 1) {
        b = 1;
    }
    return b;
}

// The real-time events calendar's day cell: never any text (color and state are left as they are);
// returns the event's calendar icon (fn_800F1008, its n14), or -1 on a day without an event. The
// event's GameModeDriverRTE_GetCalData is fetched and not used.
s32 RealTime_FillCell(char* sz, u16 nDate, s32* pCellColor, s32* pCellState) {
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
        Calendar_GetEventNameLine(CalendarState.nToday, sz);
        return;
    }
}

// The real-time events calendar's line under "Selected Day:": the date's event
// (Calendar_GetEventNameLine). The line number is not used.
void RealTime_GetBottomLine(u16 nDate, int nLine, char* sz) {
    Calendar_GetEventNameLine(nDate, sz);
}

// Which day-details popup the real-time events calendar shows for a date (the calendar screen keeps
// it in CalendarState.n1C for RealTime_GetPopupRow): 5 results for a past day; 4 today's event, or 5
// once it is completed (fn_800F102C, always 0 in this build); 6 upcoming for a day to come.
s32 RealTime_GetPopupType(u16 nDate) {
    if (nDate < CalendarState.nToday) {
        return 5;
    }
    if (nDate == CalendarState.nToday) {
        return (GM_RealtimeMode_TodaysEventCompleted() != 0) + 4;
    }
    return 6;
}

// A row of the real-time events calendar's day-details popup (row 0, "Event:", is the calendar
// screen's): row 1 "Date:" and the selected day, row 2 blank; the other rows come from the popup
// CalendarState.n1C names (4 today's event fn_8011D878, 5 results fn_8011DA44, 6 upcoming
// fn_8011DC30).
void RealTime_GetPopupRow(int nRow, char* szTitle, char* szText) {
    char szDate[12];

    switch (nRow) {
    case 1:
        CalDate_ToString(CalendarState.nSelected, szDate);
        strcpy(szTitle, "Date:");
        strcpy(szText, szDate);
        return;
    case 2:
        szTitle[0] = 0;
        szText[0] = 0;
        return;
    default:
        switch (CalendarState.n1C) {
        case 4:
            fn_8011D878(nRow, szTitle, szText);
            return;
        case 5:
            fn_8011DA44(nRow, szTitle, szText);
            return;
        case 6:
            fn_8011DC30(nRow, szTitle, szText);
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

// Opens the calendar on the driver's current day (CalendarState.nToday; once the PGA TOUR season has
// no tournament left, the last tournament's last day, with bSeasonOver set) and shows the month it
// falls in; when that day's cell would lie past the grid's 35 cells the month after is shown
// instead (not from December). Called when the calendar screen switches driver and when a new
// season starts.
void ResetCalendarState(void) {
    u16 nDate;
    s32 nMonth;
    s32 nDay;
    s32 nYear;

    CalendarState.bSeasonOver = 0;
    nDate = gCalendarGetCurrentDay[CalendarState.nDriver]();
    if (nDate == 0xFFFF) {
        nDate = GameModeDriverPGATour_GetEndDate(GameModeDriverPGATour_GetFinalEventOfSeason());
        CalendarState.bSeasonOver = 1;
    }
    CalendarState.nToday = nDate;
    CalDate_GetMDY(&CalendarState.nToday, &nMonth, &nDay, &nYear);
    CalendarState.nMonth = nMonth;
    CalendarState.nYear = nYear;
    UpdateCalendarState();
    if ((u32)(CalendarState.nFirstCell + nDay) > 35 && CalendarState.nMonth != 12) {
        CalendarState.nMonth++;
        UpdateCalendarState();
    }
}

// A day's event as text: "No Event Scheduled", on the PGA TOUR "Active Event: <name>, Round <n>"
// (the round of the event on nToday, not nDate), else "Event: <name>".
void Calendar_GetEventNameLine(u16 nDate, char* sz) {
    char* szName = gCalendarGetEventName[CalendarState.nDriver](nDate);
    s32 nId;
    s32 nRound;

    GameModeDriverPGATour_GetEventByDate(CalendarState.nToday, &nId, &nRound);
    if (szName[0] == 0) {
        strcpy(sz, "No Event Scheduled");
        return;
    }
    if (CalendarState.nDriver == 1) {
        sprintf(sz, "Active Event: %s, Round %d", szName, nRound + 1);
        return;
    }
    sprintf(sz, "Event: %s", szName);
}

// Lays the month shown (CalendarState.nMonth and nYear) out on the 35-cell grid: the cell of its
// first day (its day of the week, counted from 0), the cell after its last day, and the number of
// days in the month before. Called whenever the month shown changes.
void UpdateCalendarState(void) {
    int nDays;
    u16 nDate;
    s32 nMonth;
    s32 nYear;

    nDays = DaysInMonth(CalendarState.nMonth, CalendarState.nYear);
    CalDate_SetMDY(&nDate, CalendarState.nMonth, 1, CalendarState.nYear);
    CalendarState.nFirstCell = CalDate_GetDayOfWeek(&nDate);
    CalendarState.nFirstCell = CalendarState.nFirstCell - 1;
    CalendarState.nEndCell = CalendarState.nFirstCell + nDays;
    CalDate_GetPrevMonth(CalendarState.nMonth, CalendarState.nYear, &nMonth, &nYear);
    CalendarState.nPrevMonthDays = DaysInMonth(nMonth, nYear);
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
    if (nCell < CalendarState.nFirstCell) {
        nDay = 1 + (CalendarState.nPrevMonthDays - CalendarState.nFirstCell) + nCell;
        CalDate_GetPrevMonth(CalendarState.nMonth, CalendarState.nYear, &nMonth, &nYear);
    } else if (nCell >= CalendarState.nEndCell) {
        nDay = nCell - (CalendarState.nEndCell - 1);
        CalDate_GetNextMonth(CalendarState.nMonth, CalendarState.nYear, &nMonth, &nYear);
    } else {
        nDay = (nCell - CalendarState.nFirstCell) + 1;
        nMonth = CalendarState.nMonth;
        nYear = CalendarState.nYear;
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
    nPrevShown = CalendarState.nPrevMonthDays - CalendarState.nFirstCell;
    CalDate_GetMDY(&nCopy, &nMonth, &nDay, &nYear);
    // fake match: the original compares the months unsigned (cmplw); both are 1..12.
    if ((u32)CalendarState.nMonth == nMonth) {
        return nDay + (s32)CalendarState.nFirstCell - 1;
    }
    if (IsAMonthAhead(CalendarState.nMonth, nMonth) && nDay < 35 - CalendarState.nEndCell) {
        return nDay + (s32)CalendarState.nEndCell - 1;
    }
    if (IsAMonthBehind(CalendarState.nMonth, nMonth) && nDay > nPrevShown) {
        return (nDay - nPrevShown) - 1;
    }
    return -1;
}

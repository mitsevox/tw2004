// CalendarScreen.c (our name): the front end's career calendar screen. Its message handlers,
// registered in the front end's message table (FE_MessageTable.c), fill the month grid, the lines
// around it and the day-details popup, move between months and play the selected day, each through
// the current career driver's entry in the calendar tables (gCalendar*, indexed by
// CalendarState.nDriver: GameModeDriver.c, which is EA's FE_Calendar.c in TW07). Its last messages
// are not the calendar's: a square root (FE_Sqrt), whether the "SHERWOOD TARGET" code was entered,
// and GameMode5's Play Now calendar flag.

#include "engine.h"
#include "game.h"
#include "game/frontend.h"
#include "game/modes/rte.h"

u8 PasswordManager_IsPasswordEntered(int n);                  // PasswordManager.c

// FE message 468: day cell pArgs[0] of the month grid: its day number into the text pArgs[1], and
// the calendar driver's cell (gCalendarFillCell: its text into pArgs[2], color into *pArgs[4],
// button state into *pArgs[5]) with the driver's icon (-1 none) into *pArgs[3]. The color starts at
// 0 and is 1 for a day of the month before or after the one shown.
void Calendar_FillCell(MsgArg* pArgs, MsgArg* pResult) {
    char* szDay = ((MsgString*)pArgs[1].p)->pStr;
    char* szText = ((MsgString*)pArgs[2].p)->pStr;
    s32* pIcon = (s32*)pArgs[3].p;
    s32* pCellColor = (s32*)pArgs[4].p;
    s32* pCellState = (s32*)pArgs[5].p;
    u16 nDate = GetDateFromCellIndex(pArgs[0].i);

    sprintf(szDay, "%d", CalDate_GetDay(nDate));
    *pCellColor = 0;
    *pIcon = gCalendarFillCell[CalendarState.nDriver](szText, nDate, pCellColor, pCellState);
    if (CalendarState.nMonth != CalDate_GetMonth(nDate)) {
        *pCellColor = 1;
    }
}

// FE message 469: the calendar driver's header line pArgs[0] (gCalendarGetLine; for the PGA TOUR,
// line 1 today's tournament and round) into the text pArgs[1].
void Calendar_GetLine(MsgArg* pArgs, MsgArg* pResult) {
    gCalendarGetLine[CalendarState.nDriver](pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// FE message 483: line pArgs[1] under the grid about day cell pArgs[0] into the text pArgs[2]: 1
// "Selected Day: month/day/year", 2 the calendar driver's line for that day
// (gCalendarGetBottomLine; for the PGA TOUR its tournament and round). Other lines leave the text
// as it is.
void Calendar_GetBottomLine(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    u16 nDate;
    int nLine = pArgs[1].i;
    char* sz = ((MsgString*)pArgs[2].p)->pStr;

    nDate = GetDateFromCellIndex(pArgs[0].i);
    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    switch (nLine) {
    case 1:
        sprintf(sz, "Selected Day: %d/%d/%d", nMonth, nDay, nYear);
        return;
    case 2:
        gCalendarGetBottomLine[CalendarState.nDriver](nDate, nLine, sz);
        return;
    }
}

// FE message 484: the month (1..12) and year the calendar shows into *pArgs[0] and *pArgs[1].
void Calendar_GetMonthShown(MsgArg* pArgs, MsgArg* pResult) {
    s32* pMonth = (s32*)pArgs[0].p;
    s32* pYear = (s32*)pArgs[1].p;

    *pMonth = CalendarState.nMonth;
    *pYear = CalendarState.nYear;
}

// FE message 485: the calendar shows the month before (December of the year before after January),
// unless the driver says it is at its earliest month (gCalendarAtEarliest); the grid layout is
// worked out again either way (UpdateCalendarState).
void Calendar_PrevMonth(MsgArg* pArgs, MsgArg* pResult) {
    if (!gCalendarAtEarliest[CalendarState.nDriver]()) {
        if (CalendarState.nMonth == 1) {
            CalendarState.nMonth = 12;
            CalendarState.nYear--;
        } else {
            CalendarState.nMonth--;
        }
    }
    UpdateCalendarState();
}

// FE message 486: the calendar shows the month after (January of the next year after December),
// unless the driver says it is at its latest month (gCalendarAtLatest); the grid layout is worked
// out again either way (UpdateCalendarState).
void Calendar_NextMonth(MsgArg* pArgs, MsgArg* pResult) {
    if (!gCalendarAtLatest[CalendarState.nDriver]()) {
        if (CalendarState.nMonth == 12) {
            CalendarState.nMonth = 1;
            CalendarState.nYear++;
        } else {
            CalendarState.nMonth++;
        }
    }
    UpdateCalendarState();
}

// FE message 487: the grid cell of the career's current day (CalendarState.nToday;
// GetCellIndexFromDate).
void Calendar_GetTodayCell(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GetCellIndexFromDate(CalendarState.nToday);
}

// FE message 488: switches the calendar to driver pArgs[0] (0 none, 1 the PGA TOUR season, 2 the
// real-time events), sets the driver up (gCalendarInit) and resets the calendar to the career's
// current day (ResetCalendarState).
void Calendar_SetDriver(MsgArg* pArgs, MsgArg* pResult) {
    s32 nDriver = pArgs[0].i;

    CalendarState.nDriver = nDriver;
    gCalendarInit[nDriver]();
    ResetCalendarState();
}

// FE message 496: selects day cell pArgs[0] (CalendarState.nSelected) and gives the day-details
// popup its event shows (the driver's gCalendarGetPopupType; -1 when the day has no event), also
// kept in CalendarState.nPopupType for the popup's rows.
void Calendar_SelectCell(MsgArg* pArgs, MsgArg* pResult) {
    u16 nDate = GetDateFromCellIndex(pArgs[0].i);
    s32 nPanel;

    CalendarState.nSelected = nDate;
    if (gCalendarGetEventInfoByDate[CalendarState.nDriver](nDate)) {
        nPanel = gCalendarGetPopupType[CalendarState.nDriver](nDate);
    } else {
        nPanel = -1;
    }
    CalendarState.nPopupType = nPanel;
    pResult->i = nPanel;
}

// FE message 497: row pArgs[0] of the day-details popup into the texts pArgs[1] (title) and
// pArgs[2]: row 0 is "Event:" and the name of the selected day's event (gCalendarGetEventName),
// every other row the driver's (gCalendarGetPopupRow).
void Calendar_GetPopupRow(MsgArg* pArgs, MsgArg* pResult) {
    int nKind = pArgs[0].i;
    char* szTitle = ((MsgString*)pArgs[1].p)->pStr;
    char* szText = ((MsgString*)pArgs[2].p)->pStr;

    switch (nKind) {
    case 0:
        strcpy(szTitle, "Event:");
        strcpy(szText, gCalendarGetEventName[CalendarState.nDriver](CalendarState.nSelected));
        break;
    default:
        gCalendarGetPopupRow[CalendarState.nDriver](nKind, szTitle, szText);
        break;
    }
}

// FE message 511: does nothing (empty in this build). It sits with the calendar screen's messages,
// between the popup row (497) and play (519).
void Calendar_DoNothing(MsgArg* pArgs, MsgArg* pResult) {
}

// FE message 519: plays the selected day's event through the calendar driver (gCalendarPlay; for
// the PGA TOUR, PGATour_Play).
void Calendar_Play(MsgArg* pArgs, MsgArg* pResult) {
    gCalendarPlay[CalendarState.nDriver]();
}

// FE message 549: whether events before the selected day must be simulated before it can be played
// (the driver's gCalendarIsSimulationNecessary).
void Calendar_IsSimulationNecessary(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gCalendarIsSimulationNecessary[CalendarState.nDriver]();
}

// FE message 708: into the text pArgs[0], the description of the selected day's real-time event
// (GameModeDriverRTE_GetDescription) while the day-details popup is type 4
// (CalendarState.nPopupType), else a single space.
void Calendar_GetRTEDescription(MsgArg* pArgs, MsgArg* pResult) {
    s32 nId;
    s32 nRound;
    char* sz = ((MsgString*)pArgs[0].p)->pStr;

    switch (CalendarState.nPopupType) {
    case 4:
        GameModeDriverRTE_GetEventByDate(CalendarState.nSelected, &nId, &nRound);
        strcpy(sz, GameModeDriverRTE_GetDescription(nId));
        break;
    default:
        strcpy(sz, " ");
        break;
    }
}

// FE message 568: the square root of the float pArgs[0] (Math_Sqrt), as a float.
void FE_Sqrt(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = Math_Sqrt(pArgs[0].f);
}

// FE message 693: whether the cheat code "SHERWOOD TARGET" has been entered
// (PasswordManager_IsPasswordEntered(6)).
void Calendar_IsSherwoodTargetEntered(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PasswordManager_IsPasswordEntered(6);
}

// FE message 694: sets GameMode5's calendar flag to pArgs[0] (PlayNow_SetCalendarFlag, which lists
// what the flag changes).
void Calendar_SetPlayNowFlag(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_SetCalendarFlag(pArgs[0].i);
}

// FE message 695: GameMode5's calendar flag (PlayNow_GetCalendarFlag).
void Calendar_GetPlayNowFlag(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetCalendarFlag();
}

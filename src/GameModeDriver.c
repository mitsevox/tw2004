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

// The front end's day-details panels (0x8011D280..), and the clock's date text.
void fn_8011D280(int nKind, char* szTitle, char* szText);
void fn_8011D4DC(int nKind, char* szTitle, char* szText);
void fn_8011D658(int nKind, char* szTitle, char* szText);
void fn_8011D858(int nKind, char* szTitle, char* szText);
void fn_8011D878(int nKind, char* szTitle, char* szText);
void fn_8011DA44(int nKind, char* szTitle, char* szText);
void fn_8011DC30(int nKind, char* szTitle, char* szText);

void fn_80116798(int nPlace, char* sz);
void fn_80117264(u16 nDate, char* sz);
u8   fn_801174B8(u32 nMonth, u32 nOther);
u8   fn_801174E4(u32 nMonth, u32 nOther);

// ---- no career: nothing to show -------------------------------------------------------------

void* fn_80116540(u16 nDate) {
    return NULL;
}

void fn_80116548(void) {
}

u8 fn_8011654C(void) {
    return 0;
}

u8 fn_80116554(void) {
    return 0;
}

s32 fn_8011655C(char* sz, u16 nDate, s32* pLook, s32* pButton) {
    return -1;
}

void fn_80116564(int nLine, char* sz) {
}

void fn_80116568(u16 nDate, int n, char* sz) {
}

s32 fn_8011656C(u16 nDate) {
    return -1;
}

void fn_80116574(int nKind, char* szTitle, char* szText) {
}

u16 fn_80116578(void) {
    return fn_800D2994();
}

char* fn_80116598(u16 nDate) {
    return "";
}

void fn_801165A0(void) {
}

u8 fn_801165A4(void) {
    return 0;
}

// ---- the PGA TOUR season ----------------------------------------------------------------------

void fn_801165AC(void) {
}

// The month shown is January of the current year: the calendar cannot go back.
u8 fn_801165B0(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    fn_800D2714(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 1 && lbl_80223C48.nYear == nYear) {
        b = 1;
    }
    return b;
}

// The month shown is December of the current year: the calendar cannot go on.
u8 fn_80116614(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    fn_800D2714(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 12 && lbl_80223C48.nYear == nYear) {
        b = 1;
    }
    return b;
}

// The calendar's header lines: 1 the current event (or "Season Complete"), 2 nothing.
void fn_80116678(int nLine, char* sz) {
    switch (nLine) {
    case 1:
        if (lbl_80223C48.bSeasonOver) {
            strcpy(sz, "Season Complete");
            return;
        }
        fn_80117264(lbl_80223C48.nToday, sz);
        return;
    case 2:
        sz[0] = 0;
        return;
    }
}

// The event on a day and its round, for the day's cell.
void fn_801166E8(u16 nDate, int n, char* sz) {
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

// A finishing place as text: "1st", "2nd", "3rd", "4th"...
void fn_80116798(int nPlace, char* sz) {
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

// A day's cell: the button text (START on today's event, CONTINUE on a round in progress, the
// finishing place on the day after a finished event), how the cell looks (*pLook) and the button's
// state (*pButton). Returns the event's field size on its last round, else -1.
s32 fn_80116858(char* sz, u16 nDate, s32* pLook, s32* pButton) {
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
            fn_80116798(fn_801190D8(fn_80077B08(), 0), sz);
        }
        if (bSelected) {
            *pLook = 4;
        } else if (fn_80077ACC()->aC8[nId].award.bWon &&
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

// Whether a day may be picked: 1 an earlier event (or no season), 3 today's event on its first
// round, 0 on a later round, 2 a later event.
s32 fn_80116AA0(u16 nDate) {
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

// The day-details panel: 1 empty, 2 the selected event's dates, else the panel lbl_80223C48.n1C
// names.
void fn_80116B60(int nKind, char* szTitle, char* szText) {
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
        fn_800D28DC(nStart, szStart);
        fn_800D28DC(nEnd, szEnd);
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

// The career's current day: the selected event's start, plus its round.
u16 fn_80116CAC(void) {
    s32 nRound;
    u16 nDate;

    nDate = fn_800EFD38(GameModeDriverPGATour_GetSelectedEvent(&nRound));
    fn_800D27CC(&nDate, nRound);
    return nDate;
}

// The name of the tournament on a day.
char* fn_80116CE8(u16 nDate) {
    s32 nId;
    s32 nRound;

    if (GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound)) {
        return GameModeDriverPGATour_GetName(nId);
    }
    return "";
}

// Start the selected day's tournament: golfer 30 in slot 0, game mode 23.
void fn_80116D28(void) {
    s32 nId;
    s32 nRound;

    Session_SetGolfer(30, 0);
    fn_800E0B38(23);
    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    fn_800EF9D0(nId);
}

// The selected day is not the tournament in progress.
u8 fn_80116D78(void) {
    s32 nId;
    s32 nSel;
    s32 nRound;

    nSel =GameModeDriverPGATour_GetSelectedEvent(&nRound);
    GameModeDriverPGATour_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
    return nSel != nId;
}

// ---- the real-time events ---------------------------------------------------------------------

void fn_80116DD4(void) {
}

// The month shown is January of the current year: the calendar cannot go back.
u8 fn_80116DD8(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    fn_800D2714(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 1 && lbl_80223C48.nYear == nYear) {
        b = 1;
    }
    return b;
}

// The month shown is January of next year: the calendar cannot go on.
u8 fn_80116E3C(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    int b;

    fn_800D2714(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    b = 0;
    if (lbl_80223C48.nMonth == 1 && lbl_80223C48.nYear == nYear + 1) {
        b = 1;
    }
    return b;
}

// A day's cell: no button text; the event's award id, or -1.
s32 fn_80116EA4(char* sz, u16 nDate, s32* pLook, s32* pButton) {
    s32 nId;
    s32 nRound;

    if (GameModeDriverRTE_GetEventByDate(nDate, &nId, &nRound)) {
        GameModeDriverRTE_GetCalData(nId);
        sz[0] = 0;
        return fn_800F1008(nId);
    }
    sz[0] = 0;
    return -1;
}

// The calendar's header lines: 1 "Today: <date and time>", 2 the event on the current day.
void fn_80116F0C(int nLine, char* sz) {
    char szDate[32];

    switch (nLine) {
    case 1:
        RTClock_GetDateTimeString(szDate);
        sprintf(sz, "Today: %s", szDate);
        return;
    case 2:
        fn_80117264(lbl_80223C48.nToday, sz);
        return;
    }
}

void fn_80116F80(u16 nDate, int n, char* sz) {
    fn_80117264(nDate, sz);
}

// How a day's cell looks: 5 past, 4 or 5 today (5 when fn_800F102C), 6 to come.
s32 fn_80116FA4(u16 nDate) {
    if (nDate < lbl_80223C48.nToday) {
        return 5;
    }
    if (nDate == lbl_80223C48.nToday) {
        return (fn_800F102C() != 0) + 4;
    }
    return 6;
}

// The day-details panel: 1 the selected day's date, 2 empty, else the panel n1C names.
void fn_80117004(int nKind, char* szTitle, char* szText) {
    char szDate[12];

    switch (nKind) {
    case 1:
        fn_800D28DC(lbl_80223C48.nSelected, szDate);
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

u16 fn_801170EC(void) {
    return fn_800D2994();
}

// The name of the event on a day.
char* fn_8011710C(u16 nDate) {
    s32 nId;
    s32 nRound;

    if (GameModeDriverRTE_GetEventByDate(nDate, &nId, &nRound)) {
        return GameModeDriverRTE_GetName(nId);
    }
    return "";
}

// Start the selected day's event: golfer 30 in slot 0, game mode 24.
void fn_8011714C(void) {
    Session_SetGolfer(30, 0);
    fn_800E0B38(24);
    fn_800F0E3C();
}

u8 fn_80117180(void) {
    return 0;
}

// ---- the calendar -----------------------------------------------------------------------------

// Opens the calendar on the career's current day (the season's last day once it is over), on the
// month it falls in; a day in the last row but one moves the view on a month.
void fn_80117188(void) {
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
    fn_800D2714(&lbl_80223C48.nToday, &nMonth, &nDay, &nYear);
    lbl_80223C48.nMonth = nMonth;
    lbl_80223C48.nYear = nYear;
    fn_80117348();
    if ((u32)(lbl_80223C48.nFirstCell + nDay) > 35 && lbl_80223C48.nMonth != 12) {
        lbl_80223C48.nMonth++;
        fn_80117348();
    }
}

// The header's event line: the current event and round, or "No Event Scheduled".
void fn_80117264(u16 nDate, char* sz) {
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

// Lays out the month shown: where day 1 falls, where the month ends, and the month before's length.
void fn_80117348(void) {
    int nDays;
    u16 nDate;
    s32 nMonth;
    s32 nYear;

    nDays = fn_800D2814(lbl_80223C48.nMonth, lbl_80223C48.nYear);
    fn_800D2678(&nDate, lbl_80223C48.nMonth, 1, lbl_80223C48.nYear);
    lbl_80223C48.nFirstCell = fn_800D27E0(&nDate);
    lbl_80223C48.nFirstCell = lbl_80223C48.nFirstCell - 1;
    lbl_80223C48.nEndCell = lbl_80223C48.nFirstCell + nDays;
    fn_800D2884(lbl_80223C48.nMonth, lbl_80223C48.nYear, &nMonth, &nYear);
    lbl_80223C48.nPrevMonthDays = fn_800D2814(nMonth, nYear);
}

// The date in a cell of the grid (cells before day 1 belong to the month before, those after the
// last day to the month after).
u16 fn_801173F0(u32 nCell) {
    u16 nDate;
    s32 nMonth;
    s32 nYear;
    int nDay;

    nMonth = 0;
    nYear = 0;
    if (nCell < lbl_80223C48.nFirstCell) {
        nDay = 1 + (lbl_80223C48.nPrevMonthDays - lbl_80223C48.nFirstCell) + nCell;
        fn_800D2884(lbl_80223C48.nMonth, lbl_80223C48.nYear, &nMonth, &nYear);
    } else if (nCell >= lbl_80223C48.nEndCell) {
        nDay = nCell - (lbl_80223C48.nEndCell - 1);
        fn_800D28B0(lbl_80223C48.nMonth, lbl_80223C48.nYear, &nMonth, &nYear);
    } else {
        nDay = (nCell - lbl_80223C48.nFirstCell) + 1;
        nMonth = lbl_80223C48.nMonth;
        nYear = lbl_80223C48.nYear;
    }
    fn_800D2678(&nDate, nMonth, nDay, nYear);
    return nDate;
}

// Month b follows month a (December to January included).
u8 fn_801174B8(u32 nMonth, u32 nOther) {
    int b = 0;
    if (nMonth + 1 == nOther || (nMonth == 12 && nOther == 1)) {
        b = 1;
    }
    return b;
}

// Month b comes before month a.
u8 fn_801174E4(u32 nMonth, u32 nOther) {
    int b = 0;
    if (nMonth - 1 == nOther || (nMonth == 1 && nOther == 12)) {
        b = 1;
    }
    return b;
}

// The grid cell a date falls in, or -1 when it is not shown.
s32 fn_80117510(u16 nDate) {
    u16 nCopy;
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    u32 nPrevShown;

    nCopy = nDate;
    nPrevShown = lbl_80223C48.nPrevMonthDays - lbl_80223C48.nFirstCell;
    fn_800D2714(&nCopy, &nMonth, &nDay, &nYear);
    // fake match: the original compares the months unsigned (cmplw); both are 1..12.
    if ((u32)lbl_80223C48.nMonth == nMonth) {
        return nDay + (s32)lbl_80223C48.nFirstCell - 1;
    }
    if (fn_801174B8(lbl_80223C48.nMonth, nMonth) && nDay < 35 - lbl_80223C48.nEndCell) {
        return nDay + (s32)lbl_80223C48.nEndCell - 1;
    }
    if (fn_801174E4(lbl_80223C48.nMonth, nMonth) && nDay > nPrevShown) {
        return (nDay - nPrevShown) - 1;
    }
    return -1;
}

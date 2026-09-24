// CalendarScreen.c (our name): the front end's career calendar screen. Its callbacks, registered
// in the front end's message table, fill the month grid and the day-details panel through the
// career calendar's driver tables (GameModeDriver.c, CareerCalendar lbl_80223C48).

#include "engine.h"
#include "game.h"
#include "game/frontend.h"
#include "game/modes/rte.h"

u8 fn_80056480(int n);                  // PasswordManager.c

// Grid cell pArgs[0]: its day number, the driver's text, look and button for it, and whether it
// lies outside the month shown.
void fn_80111F58(MsgArg* pArgs, MsgArg* pResult) {
    char* szDay = ((MsgString*)pArgs[1].p)->pStr;
    char* szText = ((MsgString*)pArgs[2].p)->pStr;
    s32* pCell = (s32*)pArgs[3].p;
    s32* pLook = (s32*)pArgs[4].p;
    s32* pButton = (s32*)pArgs[5].p;
    u16 nDate = fn_801173F0(pArgs[0].i);

    sprintf(szDay, "%d", fn_800D2608(nDate));
    *pLook = 0;
    *pCell = lbl_80193E88[lbl_80223C48.nDriver](szText, nDate, pLook, pButton);
    if (lbl_80223C48.nMonth != fn_800D2640(nDate)) {
        *pLook = 1;
    }
}

void fn_80112020(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80193E94[lbl_80223C48.nDriver](pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// Line pArgs[1] about grid cell pArgs[0]: 1 its date, 2 the driver's text.
void fn_8011206C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    u16 nDate;
    int nLine = pArgs[1].i;
    char* sz = ((MsgString*)pArgs[2].p)->pStr;

    nDate = fn_801173F0(pArgs[0].i);
    fn_800D2714(&nDate, &nMonth, &nDay, &nYear);
    switch (nLine) {
    case 1:
        sprintf(sz, "Selected Day: %d/%d/%d", nMonth, nDay, nYear);
        return;
    case 2:
        lbl_80193EA0[lbl_80223C48.nDriver](nDate, nLine, sz);
        return;
    }
}

// The month shown.
void fn_80112130(MsgArg* pArgs, MsgArg* pResult) {
    s32* pMonth = (s32*)pArgs[0].p;
    s32* pYear = (s32*)pArgs[1].p;

    *pMonth = lbl_80223C48.nMonth;
    *pYear = lbl_80223C48.nYear;
}

// Show the month before (unless the driver keeps it).
void fn_80112154(MsgArg* pArgs, MsgArg* pResult) {
    if (!lbl_80193E70[lbl_80223C48.nDriver]()) {
        if (lbl_80223C48.nMonth == 1) {
            lbl_80223C48.nMonth = 12;
            lbl_80223C48.nYear--;
        } else {
            lbl_80223C48.nMonth--;
        }
    }
    fn_80117348();
}

// Show the month after (unless the driver keeps it).
void fn_801121D4(MsgArg* pArgs, MsgArg* pResult) {
    if (!lbl_80193E7C[lbl_80223C48.nDriver]()) {
        if (lbl_80223C48.nMonth == 12) {
            lbl_80223C48.nMonth = 1;
            lbl_80223C48.nYear++;
        } else {
            lbl_80223C48.nMonth++;
        }
    }
    fn_80117348();
}

void fn_80112254(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80117510(lbl_80223C48.nToday);
}

// Switch the calendar to driver pArgs[0].
void fn_80112290(MsgArg* pArgs, MsgArg* pResult) {
    s32 nDriver = pArgs[0].i;

    lbl_80223C48.nDriver = nDriver;
    lbl_80193ED0[nDriver]();
    fn_80117188();
}

// Select grid cell pArgs[0]; gives the panel its day's details show (-1: no event).
void fn_801122D8(MsgArg* pArgs, MsgArg* pResult) {
    u16 nDate = fn_801173F0(pArgs[0].i);
    s32 nPanel;

    lbl_80223C48.nSelected = nDate;
    if (lbl_80193EE8[lbl_80223C48.nDriver](nDate)) {
        nPanel = lbl_80193EAC[lbl_80223C48.nDriver](nDate);
    } else {
        nPanel = -1;
    }
    lbl_80223C48.n1C = nPanel;
    pResult->i = nPanel;
}

// The details panel's title and text: the driver's for kind pArgs[0], else "Event:" and the
// selected day's event.
void fn_80112384(MsgArg* pArgs, MsgArg* pResult) {
    int nKind = pArgs[0].i;
    char* szTitle = ((MsgString*)pArgs[1].p)->pStr;
    char* szText = ((MsgString*)pArgs[2].p)->pStr;

    switch (nKind) {
    case 0:
        strcpy(szTitle, "Event:");
        strcpy(szText, lbl_80193EDC[lbl_80223C48.nDriver](lbl_80223C48.nSelected));
        break;
    default:
        lbl_80193EB8[lbl_80223C48.nDriver](nKind, szTitle, szText);
        break;
    }
}

void fn_80112438(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8011243C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80193EF4[lbl_80223C48.nDriver]();
}

void fn_8011247C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80193F00[lbl_80223C48.nDriver]();
}

// The selected real-time event's description (panel 4), else a blank.
void fn_801124D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nId;
    s32 nRound;
    char* sz = ((MsgString*)pArgs[0].p)->pStr;

    switch (lbl_80223C48.n1C) {
    case 4:
        GameModeDriverRTE_GetEventByDate(lbl_80223C48.nSelected, &nId, &nRound);
        strcpy(sz, GameModeDriverRTE_GetDescription(nId));
        break;
    default:
        strcpy(sz, " ");
        break;
    }
}

// A square root.
void fn_80112548(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_80009680(pArgs[0].f);
}

void fn_80112580(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80056480(6);
}

void fn_801125B8(MsgArg* pArgs, MsgArg* pResult) {
    fn_800ED6E8(pArgs[0].i);
}

void fn_801125E0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800ED6F0();
}

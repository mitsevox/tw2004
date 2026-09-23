// llrtclock.c (TW06's legacy/ll/llrtclock.c): the real-time clock, as a date and as text for the
// front end.

#include "engine.h"

int fn_8011E020(s32* pnMonth, s32* pnDay, s32* pnYear, s32* pnHour, s32* pnMinute, s32* pnSecond,
                s32* pnMsec) {
    struct OSCalendarTime time;

    OSTicksToCalendarTime(OSGetTime(), &time);
    *pnMonth = time.nMon + 1;
    *pnDay = time.nMday;
    *pnYear = time.nYear;
    *pnHour = time.nHour;
    *pnMinute = time.nMin;
    *pnSecond = time.nSec;
    *pnMsec = time.nMsec;
    return 1;
}

// "M/D/YYYY H:MM AM". EA bug: noon shows as AM and midnight as PM.
void RTClock_GetDateTimeString(char* szOut) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    int bAM;
    char* szAMPM;

    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nSecond);
    bAM = 0;
    if (nHour >= 1 && nHour <= 12) {
        bAM = 1;
    }
    szAMPM = bAM ? "AM" : "PM";
    if (nHour == 0) {
        nHour = 12;
    } else if (nHour > 12) {
        nHour -= 12;
    }
    sprintf(szOut, "%d/%d/%d %d:%02d %s", nMonth, nDay, nYear, nHour, nMinute, szAMPM);
}

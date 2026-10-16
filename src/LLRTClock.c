// LLRTClock.c: the console clock's date and time as one line of text, for the front end: the
// real-time events calendar's "Today:" line, and front-end message 544 when the clock's year is
// before 2003 and its month before October.

#include "engine.h"

// Writes the clock's date and time into szOut as "M/D/YYYY H:MM AM": the hour on a 12-hour dial,
// then "AM" for clock hours 1 to 12 and "PM" for the rest.
void RTClock_GetDateTimeString(char* szOut) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nUnused;
    int bAM;
    char* szAMPM;

    RTClock_GetCurrentTime(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nUnused, &nUnused);
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

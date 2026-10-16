// LLRTClock.c: the console clock's date and time as one line of text, for the front end: the
// real-time events calendar's "Today:" line, and front-end message 544 when the clock's year is
// before 2003 and its month before October.

#include "engine.h"

// Writes the clock's date and time into s as "M/D/YYYY H:MM AM": the hour on a 12-hour dial,
// then "AM" for clock hours 1 to 12 and "PM" for the rest.
void RTClock_GetDateTimeString(char* s) {
    s32 month;
    s32 day;
    s32 year;
    s32 hour;
    s32 minute;
    s32 dummy;
    int bAM;
    char* AMPM;

    RTClock_GetCurrentTime(&month, &day, &year, &hour, &minute, &dummy, &dummy);
    bAM = 0;
    if (hour >= 1 && hour <= 12) {
        bAM = 1;
    }
    AMPM = bAM ? "AM" : "PM";
    if (hour == 0) {
        hour = 12;
    } else if (hour > 12) {
        hour -= 12;
    }
    sprintf(s, "%d/%d/%d %d:%02d %s", month, day, year, hour, minute, AMPM);
}

// LLRTClock_Gc.c: the GameCube side of the real-time clock: the console's date and time, read from
// the OS clock as numbers.

#include "engine.h"

int RTClock_GetCurrentTime(s32* pnMonth, s32* pnDay, s32* pnYear, s32* pnHour, s32* pnMinute, s32* pnSecond,
                s32* pnMsec) {
    OSCalendarTime time;

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

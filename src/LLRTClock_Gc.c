// LLRTClock_Gc.c: the GameCube side of the real-time clock: the console's date and time, read from
// the OS clock as numbers.

#include "engine.h"

int RTClock_GetCurrentTime(s32* pMonth, s32* pDay, s32* pYear, s32* pHour, s32* pMinute, s32* pSecond,
                s32* pMillisecond) {
    OSCalendarTime time;

    OSTicksToCalendarTime(OSGetTime(), &time);
    *pMonth = time.nMon + 1;
    *pDay = time.nMday;
    *pYear = time.nYear;
    *pHour = time.nHour;
    *pMinute = time.nMin;
    *pSecond = time.nSec;
    *pMillisecond = time.nMsec;
    return 1;
}

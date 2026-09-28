// Calendar.c (EA's name: TW07's golf/gamemode/Calendar.c, TW06's calendar.c; every function here
// has TW07's name): dates as day numbers (a u16, CalendarDate in TW07; day 1 is 1 January 1900,
// which counts as a leap year, see IsLeapYear): turned into month, day and year and back, the
// weekday, month steps, today's date from the console clock (CalDate_GetToday) and the
// "month/day/year" strings the menus print. The tour season, the calendar screen, the awards and
// the Play Now medals keep their dates this way. TW07's CalDate_GetYear, CalDate_SetMDYHMS,
// CalDate_AddMonths and CalDate_AddYears are not in this build.

#include "game.h"

u8  IsLeapYear(u32 nYear);
u32 DaysInYear(u32 nYear);

// The days of January..December; February's 28 (DaysInMonth gives 29 in a leap year).
u8 gMonthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Whether a year is a leap year. EA bug: 1900 counts as one (it was not), which the day numbers
// rely on (36525 days from 1900 to 2000).
u8 IsLeapYear(u32 nYear) {
    u8 bLeap;

    if ((nYear & 3) == 0) {
        if (nYear % 100 == 0) {
            if (nYear == 1900) {
                bLeap = 1;
            } else if (nYear % 400 == 0) {
                bLeap = 1;
            } else {
                bLeap = 0;
            }
        } else {
            bLeap = 1;
        }
    } else {
        bLeap = 0;
    }
    return bLeap;
}

// The days in year nYear: 366 in a leap year (IsLeapYear, 1900 included), else 365.
u32 DaysInYear(u32 nYear) {
    return IsLeapYear(nYear) ? 366 : 365;
}

// The day of the month (1..31) of day number nDate.
s32 CalDate_GetDay(u16 nDate) {
    s32 nMonth;
    s32 nDay;

    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nMonth);
    return nDay;
}

// The month (1 January .. 12 December) of day number nDate.
u32 CalDate_GetMonth(u16 nDate) {
    s32 nOther;
    s32 nMonth;

    CalDate_GetMDY(&nDate, &nMonth, &nOther, &nOther);
    return nMonth;
}

// Sets *pDate to the day number of nMonth (1..12), nDay, nYear: 1 January 1900 is day 1 and 1900
// counts as a leap year (IsLeapYear), so 1 January 2000 is day 36526; years from 2000 on are
// counted from there. The u16 runs out in 2079.
void CalDate_SetMDY(u16* pDate, s32 nMonth, s32 nDay, u32 nYear) {
    u32 i;
    u32 nYearAt;
    s32 nDays;

    if (nYear >= 2000) {
        nYearAt = 2000;
        nDays = 36525;
    } else {
        nYearAt = 1900;
        nDays = 0;
    }
    for (; nYearAt < nYear; nYearAt++) {
        nDays += DaysInYear(nYearAt);
    }
    for (i = 1; i < nMonth; i++) {
        nDays += DaysInMonth(i, nYearAt);
    }
    nDays += nDay;
    *pDate = nDays;
}

// Splits day number *pDate into its month (1..12), day of the month and year: the inverse of
// CalDate_SetMDY, counting from 2000 for numbers from 36525 on.
void CalDate_GetMDY(u16* pDate, s32* pMonth, s32* pDay, s32* pYear) {
    u32 nMonth;
    u32 nYear;
    u32 nDays;
    u32 n;
    s32 nInMonth;

    nDays = *pDate;
    // EA bug: day 36525 (31 December 1999) comes out as 0 January 2000
    if (nDays >= 36525) {
        nYear = 2000;
        nDays -= 36525;
    } else {
        nYear = 1900;
    }
    n = DaysInYear(nYear);
    while (nDays > n) {
        nYear++;
        nDays -= n;
        n = DaysInYear(nYear);
    }
    nMonth = 1;
    nInMonth = DaysInMonth(1, nYear);
    while (nDays > nInMonth) {
        nMonth++;
        nDays -= nInMonth;
        nInMonth = DaysInMonth(nMonth, nYear);
    }
    *pMonth = nMonth;
    *pDay = nDays;
    *pYear = nYear;
}

// Moves a date on by nDays.
void CalDate_AddDays(u16* pDate, s32 nDays) {
    *pDate += (u16)nDays;
}

// The weekday of *pDate, 1..7: (day number - 1) % 7 + 1. With 1900's extra leap day this is 1 for
// Sunday to 7 for Saturday from 1 March 1900 on (1 January 2000, a Saturday, is day 36526: 7). The
// calendar screen's weeks start with 1 (UpdateCalendarState).
s32 CalDate_GetDayOfWeek(u16* pDate) {
    return (*pDate - 1) % 7 + 1;
}

// The days in month nMonth (1..12) of year nYear: 29 for a leap February (IsLeapYear), else
// gMonthDays.
s32 DaysInMonth(u32 nMonth, u32 nYear) {
    s32 bLeap;

    bLeap = 0;
    if (nMonth == 2 && IsLeapYear(nYear)) {
        bLeap = 1;
    }
    if (bLeap) {
        return 29;
    }
    return gMonthDays[nMonth - 1];
}

// The month before nMonth of nYear into *pMonth and *pYear (December of the year before, from
// January).
void CalDate_GetPrevMonth(s32 nMonth, s32 nYear, s32* pMonth, s32* pYear) {
    if (nMonth == 1) {
        *pMonth = 12;
        *pYear = nYear - 1;
        return;
    }
    *pMonth = nMonth - 1;
    *pYear = nYear;
}

// The month after nMonth of nYear into *pMonth and *pYear (January of the next year, from
// December).
void CalDate_GetNextMonth(s32 nMonth, s32 nYear, s32* pMonth, s32* pYear) {
    if (nMonth == 12) {
        *pMonth = 1;
        *pYear = nYear + 1;
        return;
    }
    *pMonth = nMonth + 1;
    *pYear = nYear;
}

// Prints day number nDate into pBuf as month/day/year ("12/25/2003"). No buffer size (TW07's takes
// one).
void CalDate_ToString(u16 nDate, char* pBuf) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;

    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    sprintf(pBuf, "%d/%d/%d", nMonth, nDay, nYear);
}

// Prints day number nDate into pBuf as month/day ("12/25"). No buffer size (TW07's takes one).
void CalDate_ToStringMD(u16 nDate, char* pBuf) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;

    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    sprintf(pBuf, "%d/%d", nMonth, nDay);
}

// Today's date as a day number: the month, day and year of the console's clock (fn_8011E020,
// llrtclock.c) through CalDate_SetMDY.
u16 CalDate_GetToday(void) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nUnused;
    u16 nDate;

    fn_8011E020(&nMonth, &nDay, &nYear, &nUnused, &nUnused, &nUnused, &nUnused);
    CalDate_SetMDY(&nDate, nMonth, nDay, nYear);
    return nDate;
}

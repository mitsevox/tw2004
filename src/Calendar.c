// Calendar.c (our name): dates for the tour season: a day number (u16, day 1 is 1 January 1900)
// turned into weekday, day, month and year and back, month steps, today's date (CalDate_GetToday) and
// the date strings the menus print.

#include "game.h"

u8  IsLeapYear(u32 nYear);
u32 DaysInYear(u32 nYear);

u8 lbl_80191798[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    // days in each month

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

// The days in a year.
u32 DaysInYear(u32 nYear) {
    return IsLeapYear(nYear) ? 366 : 365;
}

// A date's day of the month.
s32 CalDate_GetDay(u16 nDate) {
    s32 nMonth;
    s32 nDay;

    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nMonth);
    return nDay;
}

// A date's month.
u32 CalDate_GetMonth(u16 nDate) {
    s32 nOther;
    s32 nMonth;

    CalDate_GetMDY(&nDate, &nMonth, &nOther, &nOther);
    return nMonth;
}

// The day number of a date.
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

// A day number's month, day and year.
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

// A date's weekday, 1..7.
s32 CalDate_GetDayOfWeek(u16* pDate) {
    return (*pDate - 1) % 7 + 1;
}

// The days in a month.
s32 DaysInMonth(u32 nMonth, u32 nYear) {
    s32 bLeap;

    bLeap = 0;
    if (nMonth == 2 && IsLeapYear(nYear)) {
        bLeap = 1;
    }
    if (bLeap) {
        return 29;
    }
    return lbl_80191798[nMonth - 1];
}

// The month before.
void CalDate_GetPrevMonth(s32 nMonth, s32 nYear, s32* pMonth, s32* pYear) {
    if (nMonth == 1) {
        *pMonth = 12;
        *pYear = nYear - 1;
        return;
    }
    *pMonth = nMonth - 1;
    *pYear = nYear;
}

// The month after.
void CalDate_GetNextMonth(s32 nMonth, s32 nYear, s32* pMonth, s32* pYear) {
    if (nMonth == 12) {
        *pMonth = 1;
        *pYear = nYear + 1;
        return;
    }
    *pMonth = nMonth + 1;
    *pYear = nYear;
}

// Prints a date as month/day/year.
void CalDate_ToString(u16 nDate, char* pBuf) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;

    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    sprintf(pBuf, "%d/%d/%d", nMonth, nDay, nYear);
}

// Prints a date as month/day.
void CalDate_ToStringMD(u16 nDate, char* pBuf) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;

    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    sprintf(pBuf, "%d/%d", nMonth, nDay);
}

// Today's date.
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

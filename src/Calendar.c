// Calendar.c: dates as day numbers (CalendarDate, day 1 is 1 January 1900): made from and split
// into month, day and year, the weekday, the month before and after, today's date from the console
// clock, and the date text ("12/25/2003", "12/25") the menus show for tour events, awards and medals.

#include "game.h"

// The days of January..December; February's 28 (DaysInMonth gives 29 in a leap year).
u8 gMonthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Whether year is a leap year: every fourth year, but of the century years only those divisible by
// 400, and 1900. Counting 1900 makes the years 1900..1999 36525 days (CalDate_SetMDY's 2000 start).
u8 IsLeapYear(u32 year) {
    u8 result;

    if ((year & 3) == 0) {
        if (year % 100 == 0) {
            if (year == 1900) {
                result = 1;
            } else if (year % 400 == 0) {
                result = 1;
            } else {
                result = 0;
            }
        } else {
            result = 1;
        }
    } else {
        result = 0;
    }
    return result;
}

// The days in year: 366 in a leap year (IsLeapYear, 1900 included), else 365.
static u32 DaysInYear(u32 year) {
    return IsLeapYear(year) ? 366 : 365;
}

// The day of the month (1..31) of date.
s32 CalDate_GetDay(CalendarDate date) {
    s32 dummy;
    s32 day;

    CalDate_GetMDY(&date, &dummy, &day, &dummy);
    return day;
}

// The month (1 January .. 12 December) of date.
u32 CalDate_GetMonth(CalendarDate date) {
    s32 dummy;
    s32 month;

    CalDate_GetMDY(&date, &month, &dummy, &dummy);
    return month;
}

// Sets *pDate to the day number of month (1..12), day and year: 1 January 1900 is day 1, and with
// 1900 counted as a leap year (IsLeapYear) 1 January 2000 is day 36526, from which later years are
// counted. A year before 1900 counts as 1900. The last date the u16 holds is 4 June 2079 (65535).
void CalDate_SetMDY(CalendarDate* pDate, s32 month, s32 day, u32 year) {
    u32 currMonth;
    u32 currYear;
    s32 dayCount;

    if (year >= 2000) {
        currYear = 2000;
        dayCount = 36525;
    } else {
        currYear = 1900;
        dayCount = 0;
    }
    for (; currYear < year; currYear++) {
        dayCount += DaysInYear(currYear);
    }
    for (currMonth = 1; currMonth < month; currMonth++) {
        dayCount += DaysInMonth(currMonth, currYear);
    }
    dayCount += day;
    *pDate = dayCount;
}

// Splits *pDate into its month (1..12), day of the month and year, the inverse of CalDate_SetMDY:
// day numbers from 36525 on are counted from 2000, so day 36525 (31 December 1999) comes out as
// day 0 of January 2000.
void CalDate_GetMDY(CalendarDate* pDate, s32* pMonth, s32* pDay, s32* pYear) {
    u32 currMonth;
    u32 currYear;
    u32 dayNum;
    u32 daysInYear;
    s32 daysInMonth;

    dayNum = *pDate;
    if (dayNum >= 36525) {
        currYear = 2000;
        dayNum -= 36525;
    } else {
        currYear = 1900;
    }
    daysInYear = DaysInYear(currYear);
    while (dayNum > daysInYear) {
        currYear++;
        dayNum -= daysInYear;
        daysInYear = DaysInYear(currYear);
    }
    currMonth = 1;
    daysInMonth = DaysInMonth(1, currYear);
    while (dayNum > daysInMonth) {
        currMonth++;
        dayNum -= daysInMonth;
        daysInMonth = DaysInMonth(currMonth, currYear);
    }
    *pMonth = currMonth;
    *pDay = dayNum;
    *pYear = currYear;
}

// Moves *pDate on by days (back when negative).
void CalDate_AddDays(CalendarDate* pDate, s32 days) {
    *pDate += (u16)days;
}

// The weekday of *pDate, 1 Sunday .. 7 Saturday. It is right from 1 March 1900 on; the days before
// come out one early, as IsLeapYear counts a 29 February 1900. The calendar screen's grid starts
// its weeks on Sunday (UpdateCalendarState).
s32 CalDate_GetDayOfWeek(CalendarDate* pDate) {
    return (*pDate - 1) % 7 + 1;
}

// The days in month (1..12) of year: 29 for February of a leap year (IsLeapYear), else gMonthDays.
s32 DaysInMonth(u32 month, u32 year) {
    s32 bLeap;

    bLeap = 0;
    if (month == 2 && IsLeapYear(year)) {
        bLeap = 1;
    }
    if (bLeap) {
        return 29;
    }
    return gMonthDays[month - 1];
}

// The month before curMonth of curYear into *prevMonth and *prevYear (December of the year before,
// from January).
void CalDate_GetPrevMonth(s32 curMonth, s32 curYear, s32* prevMonth, s32* prevYear) {
    if (curMonth == 1) {
        *prevMonth = 12;
        *prevYear = curYear - 1;
        return;
    }
    *prevMonth = curMonth - 1;
    *prevYear = curYear;
}

// The month after curMonth of curYear into *nextMonth and *nextYear (January of the next year, from
// December).
void CalDate_GetNextMonth(s32 curMonth, s32 curYear, s32* nextMonth, s32* nextYear) {
    if (curMonth == 12) {
        *nextMonth = 1;
        *nextYear = curYear + 1;
        return;
    }
    *nextMonth = curMonth + 1;
    *nextYear = curYear;
}

// Prints date into s as month/day/year ("12/25/2003"), with no bound on s.
void CalDate_ToString(CalendarDate date, char* s) {
    s32 month;
    s32 day;
    s32 year;

    CalDate_GetMDY(&date, &month, &day, &year);
    sprintf(s, "%d/%d/%d", month, day, year);
}

// Prints date into s as month/day ("12/25"), with no bound on s.
void CalDate_ToStringMD(CalendarDate date, char* s) {
    s32 month;
    s32 day;
    s32 year;

    CalDate_GetMDY(&date, &month, &day, &year);
    sprintf(s, "%d/%d", month, day);
}

// Today's date: the month, day and year of the console's clock (RTClock_GetCurrentTime) as a day
// number.
CalendarDate CalDate_GetToday(void) {
    s32 month;
    s32 day;
    s32 year;
    s32 dummy;
    CalendarDate curDay;

    RTClock_GetCurrentTime(&month, &day, &year, &dummy, &dummy, &dummy, &dummy);
    CalDate_SetMDY(&curDay, month, day, year);
    return curDay;
}

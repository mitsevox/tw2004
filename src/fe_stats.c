// fe_stats.c (TW06's fe_stats.c, TW07's ui_core/frontend/FE_Stats.c): the front end's PGA TOUR
// statistics screen, FE messages 521 to 526 (the UIStatsRankings_ functions). It fills the menus'
// text for the player's own line in each of the 29 categories and for the leader board of the
// chosen category (gStatsActiveCategory), from the statistics PGATourSimulation.c keeps. A
// category is a screen row; gStatsCategoryStat maps it to the statistic behind it (the longest
// putt and the Player of the Year points have no row).

#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/pgatoursim.h"

// Per category: the title on the screen.
char* gStatsCategoryTitle[FE_STATS_NUM_CATEGORIES] = {
    "Season Money Leaders", "Career Money Leaders", "All-Around Ranking", "Total Rounds",
    "Scoring Average", "Total Driving", "Longest Drive", "Driving Distance", "Driving Accuracy",
    "Ball Striking", "Greens In Regulation (GIR)", "Putts Per Round", "Putting Average",
    "Sand Save %", "Scrambling", "Bounce Back", "Consecutive Cuts Made", "Total Eagles",
    "Holes Per Eagle", "Total Birdies", "Birdie Average", "Par 3 Birdie Leaders",
    "Par 4 Birdie Leaders", "Par 5 Birdie Leaders", "Birdie Conversion", "Par Breakers",
    "Par 3 Performance", "Par 4 Performance", "Par 5 Performance",
};
// Per category: the statistic it shows (GM_Pga_StatTypes_t; -1 would mean none, which no category
// has in this build).
s32 gStatsCategoryStat[FE_STATS_NUM_CATEGORIES] = {
    24, 25, 28, 26, 14, 29, 19, 0, 1, 30, 2, 3, 4, 5, 6, 7, 23, 21, 8, 22, 9, 10, 11, 12, 13, 15,
    16, 17, 18,
};
// Per category: the units its value is printed with (PrintStatsWithUnits).
StatsUnits gStatsCategoryUnits[FE_STATS_NUM_CATEGORIES] = {
    UNITS_MONEY, UNITS_MONEY, UNITS_NONE, UNITS_NONE, UNITS_NONE, UNITS_NONE, UNITS_YARDS,
    UNITS_YARDS, UNITS_PERCENT, UNITS_NONE, UNITS_PERCENT, UNITS_NONE, UNITS_NONE, UNITS_PERCENT,
    UNITS_PERCENT, UNITS_PERCENT, UNITS_NONE, UNITS_NONE, UNITS_NONE, UNITS_NONE, UNITS_NONE,
    UNITS_NONE, UNITS_NONE, UNITS_NONE, UNITS_PERCENT, UNITS_PERCENT, UNITS_NONE, UNITS_NONE,
    UNITS_NONE,
};

// The category the leader board shows (UIStatsRankings_SetActiveStat).
s32 gStatsActiveCategory;

// Prints a statistic's value text with its units into szOut: as it is, as money with thousands
// commas ("$1,234,567", fn_800907AC), with " yds", or with "%".
void PrintStatsWithUnits(const char* szValue, StatsUnits eUnits, char* szOut) {
    char szMoney[128];

    switch (eUnits) {
    case UNITS_NONE:
        strcpy(szOut, szValue);
        break;
    case UNITS_MONEY:
        fn_800907AC(atoi(szValue), szMoney);
        sprintf(szOut, "$%s", szMoney);
        break;
    case UNITS_YARDS:
        sprintf(szOut, "%s yds", szValue);
        break;
    case UNITS_PERCENT:
        sprintf(szOut, "%s%%", szValue);
        break;
    }
}

// FE message 521: the player's line in a category of the statistics screen (arguments: the
// category, then the title, value and place strings to fill): the category's title, the player's
// value with its units and the player's place in the statistic. A category with no statistic behind
// it (-1) would show "TODO"; every category has one in this build.
void UIStatsRankings_GetRow(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = pArgs[0].i;
    char* szTitle = ((MsgString*)pArgs[1].p)->pStr;
    char* szValue = ((MsgString*)pArgs[2].p)->pStr;
    char* szRank = ((MsgString*)pArgs[3].p)->pStr;
    GM_Pga_StatTypes_t nStat = gStatsCategoryStat[nCategory];
    StatsUnits eUnits = gStatsCategoryUnits[nCategory];
    int nPlayer = fn_80077B08();
    char szStat[32];

    strcpy(szTitle, gStatsCategoryTitle[nCategory]);
    if (nStat != -1) {
        GM_PgaTourSim_GetStatValString(
            nStat, GM_PgaTourSim_GetStatValueFromGolferID(nPlayer, PGA_USER_GOLFER, nStat), szStat);
        PrintStatsWithUnits(szStat, eUnits, szValue);
        sprintf(szRank, "%d", GM_PgaTourSim_GetStatRankFromGolferID(nPlayer, PGA_USER_GOLFER, nStat));
    } else {
        sprintf(szValue, "TODO");
        sprintf(szRank, "TODO");
    }
}

// FE message 522: copies the name of the profile being worked on (FE_GetCurrentProfile) into the
// string argument, for the statistics screen.
void UIStatsRankings_GetProfileName(MsgArg* pArgs, MsgArg* pResult) {
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;

    strcpy(szOut, FE_GetCurrentProfile()->szName);
}

// FE message 523: picks the category (a row of the statistics screen, not a statistic number) that
// the leader board shows.
void UIStatsRankings_SetActiveStat(MsgArg* pArgs, MsgArg* pResult) {
    gStatsActiveCategory = pArgs[0].i;
}

// FE message 524: a line of the chosen category's leader board (arguments: the row, -1 for the
// player's own line, then the place, name, played and value strings to fill): the golfer's place,
// name, how much the golfer played (tournaments or rounds as GM_PgaTourSim_GetStatView says: 0
// tournaments, else rounds; blank for career money) and value with its units, percentages without
// the sign. A category with no statistic (-1) would show "TODO" in every column; every category has
// one in this build.
void UIStatsRankings_GetIndStatsRow(MsgArg* pArgs, MsgArg* pResult) {
    StatsUnits eUnits;
    GM_Pga_StatTypes_t nStat;
    int nPlayer;
    int nGolfer = pArgs[0].i;      // the row, until it is turned into the golfer in it
    char* szRank = ((MsgString*)pArgs[1].p)->pStr;
    char* szName = ((MsgString*)pArgs[2].p)->pStr;
    char* szPlayed = ((MsgString*)pArgs[3].p)->pStr;
    char* szValue = ((MsgString*)pArgs[4].p)->pStr;
    char szStat[16];

    nPlayer = fn_80077B08();
    nStat = gStatsCategoryStat[gStatsActiveCategory];
    if (nGolfer == -1) {
        nGolfer = PGA_USER_GOLFER;
    } else {
        nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, nStat, nGolfer);
    }
    if (nStat != -1) {
        sprintf(szRank, "%d", GM_PgaTourSim_GetStatRankFromGolferID(nPlayer, nGolfer, nStat));
        strcpy(szName, GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolfer));
        if (gStatsActiveCategory == 1) {
            szPlayed[0] = '\0';
        } else if (GM_PgaTourSim_GetStatView(nStat) == 0) {
            sprintf(szPlayed, "%d", GM_PgaTourSim_GetNEventsFromGolferID(nPlayer, nGolfer));
        } else {
            sprintf(szPlayed, "%d", GM_PgaTourSim_GetNRoundsFromGolferID(nPlayer, nGolfer));
        }
        GM_PgaTourSim_GetStatValString(nStat, GM_PgaTourSim_GetStatValueFromGolferID(nPlayer, nGolfer, nStat),
                                       szStat);
        eUnits = gStatsCategoryUnits[gStatsActiveCategory];
        if (eUnits == UNITS_PERCENT) {
            eUnits = UNITS_NONE;
        }
        PrintStatsWithUnits(szStat, eUnits, szValue);
    } else {
        sprintf(szRank, "TODO");
        sprintf(szName, "TODO");
        sprintf(szPlayed, "TODO");
        sprintf(szValue, "TODO");
    }
}

// FE message 525: the leader board has a line for every tour golfer and the player
// (PGA_NUM_GOLFERS).
void UIStatsRankings_GetIndStatsNumRows(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PGA_NUM_GOLFERS;
}

// FE message 526: the statistics screen has a line for every category (FE_STATS_NUM_CATEGORIES,
// 29).
void UIStatsRankings_GetNumRows(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_STATS_NUM_CATEGORIES;
}

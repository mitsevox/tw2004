// fe_stats.c (TW06's fe_stats.c): the front end's PGA TOUR statistics screen. It fills the
// menus' text for the player's own line in each of the 29 categories and for the leader board of
// the chosen category (lbl_80282508), from the statistics PGATourSimulation.c keeps.

#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/pgatoursim.h"

// Prints a statistic's value text with its units: "$1,234,567", "301.2 yds", "65.2%".
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

// The player's line in a category: its title, the player's value and the player's place. A
// category with no statistic behind it shows "TODO".
void fn_8011CF64(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = pArgs[0].i;
    char* szTitle = ((MsgString*)pArgs[1].p)->pStr;
    char* szValue = ((MsgString*)pArgs[2].p)->pStr;
    char* szRank = ((MsgString*)pArgs[3].p)->pStr;
    GM_Pga_StatTypes_t nStat = lbl_80194410[nCategory];
    StatsUnits eUnits = lbl_80194484[nCategory];
    int nPlayer = fn_80077B08();
    char szStat[32];

    strcpy(szTitle, lbl_8019439C[nCategory]);
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

void fn_8011D05C(MsgArg* pArgs, MsgArg* pResult) {
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;

    strcpy(szOut, fn_80077ACC()->szName);
}

// Picks the category the leader board shows.
void fn_8011D09C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80282508 = pArgs[0].i;
}

// A leader board line of the chosen category: place, golfer, tournaments played (rounds for the
// statistics counted per round, nothing for career money) and value, percentages without the
// sign. Row -1 is the player's own line.
void fn_8011D0A8(MsgArg* pArgs, MsgArg* pResult) {
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
    nStat = lbl_80194410[lbl_80282508];
    if (nGolfer == -1) {
        nGolfer = PGA_USER_GOLFER;
    } else {
        nGolfer = GM_PgaTourSim_GetGolferIDFromStatRow(nPlayer, nStat, nGolfer);
    }
    if (nStat != -1) {
        sprintf(szRank, "%d", GM_PgaTourSim_GetStatRankFromGolferID(nPlayer, nGolfer, nStat));
        strcpy(szName, fn_80118E30(nPlayer, nGolfer));
        if (lbl_80282508 == 1) {
            szPlayed[0] = '\0';
        } else if (GM_PgaTourSim_GetStatView(nStat) == 0) {
            sprintf(szPlayed, "%d", fn_8011903C(nPlayer, nGolfer));
        } else {
            sprintf(szPlayed, "%d", fn_80119064(nPlayer, nGolfer));
        }
        GM_PgaTourSim_GetStatValString(nStat, GM_PgaTourSim_GetStatValueFromGolferID(nPlayer, nGolfer, nStat),
                                       szStat);
        eUnits = lbl_80194484[lbl_80282508];
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

// The leader board has a line for every tour golfer.
void fn_8011D268(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PGA_NUM_GOLFERS;
}

// The screen has a line for every category.
void fn_8011D274(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_STATS_NUM_CATEGORIES;
}

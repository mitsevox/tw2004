// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game/frontend.h"
#include "game/modes/pgatoursim.h"

void fn_8011D268(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PGA_NUM_GOLFERS;
}

void fn_8011D274(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_STATS_NUM_CATEGORIES;
}

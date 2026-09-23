// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game.h"
#include "game/frontend.h"

// Whether a year is a leap year.
void fn_8010A3C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800D256C(pArgs[0].i);
}

// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "frontend/fe.h"
#include "game/frontend.h"

// How many choices a part has.
void fn_801077EC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801048EC(pArgs[0].i, pArgs[1].i);
}

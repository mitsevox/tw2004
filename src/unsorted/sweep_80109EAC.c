// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "frontend/fe.h"
#include "game/frontend.h"

// Count a part's offered assets into four results.
void fn_80109EAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_801072CC(pArgs[0].i, pArgs[3].p, pArgs[2].p, pArgs[4].p, pArgs[1].p);
}

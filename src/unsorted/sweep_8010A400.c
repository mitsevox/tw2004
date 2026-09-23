// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "frontend/fe.h"
#include "game/frontend.h"

void fn_8010A400(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801074D4(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i));
}

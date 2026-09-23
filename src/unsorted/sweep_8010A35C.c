// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "frontend/fe.h"
#include "game/frontend.h"

// A part's choice's n2E.
void fn_8010A35C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80104F68(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i))->n2E;
}

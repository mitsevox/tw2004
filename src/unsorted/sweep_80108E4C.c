// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "charstate.h"
#include "frontend/fe.h"
#include "game/frontend.h"

void fn_80108E4C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    pResult->i = fn_8001E9CC(pProfile->a10548, pArgs[0].i);
}

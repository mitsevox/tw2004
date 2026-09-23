// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "charstate.h"
#include "frontend/fe.h"
#include "game/frontend.h"

// A part's choice has its aB1CC bit set.
void fn_80108398(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = fn_8001E9CC(pProfile->aB1CC, nAsset);
}

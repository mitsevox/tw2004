// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "charstate.h"
#include "frontend/fe.h"
#include "game/frontend.h"

// A part's choice with its aB344 bit set gets its aB4BC bit set too.
void fn_80108B84(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    if (fn_8001E9CC(pProfile->aB344, nAsset)) {
        fn_8001EA34(pProfile->aB4BC, nAsset);
    }
}

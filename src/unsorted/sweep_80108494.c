// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "frontend/fe.h"
#include "game/frontend.h"

void fn_80108494(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077ACC();
    if (fn_80106244(pArgs[0].i) >= 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

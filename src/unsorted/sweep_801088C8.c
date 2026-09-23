// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "frontend/fe.h"
#include "game/frontend.h"

void fn_801088C8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        fn_8010F7E4(LOGO_SQUARE);
        return;
    }
    fn_8010F7E4(LOGO_RECT);
}

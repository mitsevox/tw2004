// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "game/frontend.h"

s32 fn_800A7A14(s32);

void fn_800B2150(MsgArg* pArgs);
void fn_800B2150(MsgArg* pArgs) {
    if (pArgs[0].i == 2) {
        fn_800A7A14(0);
        return;
    }
    fn_800A7A14(1);
}

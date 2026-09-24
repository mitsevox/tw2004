// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "game/frontend.h"

void fn_800B1510(s32 a, s32 b);

void fn_800B208C(MsgArg* pArgs);
void fn_800B208C(MsgArg* pArgs) {
    fn_800B1510(pArgs[0].i, pArgs[1].i);
}

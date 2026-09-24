// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "game/frontend.h"
#include "core/memcard.h"

s32 fn_800B120C(s32* pnPort, s32* pnSlot);
s32 fn_800B12FC(s32* pnPort, s32* pnSlot);

void fn_800B20B8(MsgArg* pArgs, MsgArg* pResult);
void fn_800B2104(MsgArg* pArgs, MsgArg* pResult);
void fn_800B20B8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD10();
    pResult->i = fn_800B12FC(pArgs[0].p, pArgs[1].p);
    fn_8009CD7C();
}

void fn_800B2104(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD10();
    pResult->i = fn_800B120C(pArgs[0].p, pArgs[1].p);
    fn_8009CD7C();
}

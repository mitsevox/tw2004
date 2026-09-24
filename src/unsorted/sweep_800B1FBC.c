// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "game/frontend.h"

int fn_800B13FC(s32* pnPort, s32* pnSlot);
int fn_800B14E4(s32* pnPort, s32* pnSlot);

void fn_800B1FBC(MsgArg* pArgs, MsgArg* pResult);
void fn_800B1FFC(MsgArg* pArgs, MsgArg* pResult);
void fn_800B1FBC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)fn_800B14E4(pArgs[0].p, pArgs[1].p);
}

void fn_800B1FFC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)fn_800B13FC(pArgs[0].p, pArgs[1].p);
}

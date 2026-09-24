// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "game/frontend.h"
#include "core/memcard.h"

void fn_800B21F0(MsgArg* pArgs, MsgArg* pResult);
void fn_800B21F0(MsgArg* pArgs, MsgArg* pResult) {
    fn_8009CD10();
    pResult->i = fn_8009D3DC(pArgs[0].i, pArgs[1].i);
    fn_8009CD7C();
}

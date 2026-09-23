#include "game/frontend.h"
#include "frontend/fe.h"
#include "game/modes/pgatoursim.h"



void fn_8011D05C(MsgArg* pArgs, MsgArg* pResult) {
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;

    strcpy(szOut, fn_80077ACC()->szName);
}

void fn_8011D09C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80282508 = pArgs[0].i;
}

void fn_8011D268(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PGA_NUM_GOLFERS;
}

void fn_8011D274(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_STATS_NUM_CATEGORIES;
}

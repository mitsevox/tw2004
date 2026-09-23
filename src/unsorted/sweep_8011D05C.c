// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game/frontend.h"
#include "frontend/fe.h"

void fn_8011D05C(MsgArg* pArgs, MsgArg* pResult) {
    char* szOut = ((MsgString*)pArgs[0].p)->pStr;

    strcpy(szOut, fn_80077ACC()->szName);
}

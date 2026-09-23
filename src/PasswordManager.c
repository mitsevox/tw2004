#include "game/save.h"



void fn_80057ED0(SaveProfile* pProfile, const char* pName) {
    strcpy(pProfile->createdGolfer.szLast, pName);
    strcpy(pProfile->szName, pName);
}

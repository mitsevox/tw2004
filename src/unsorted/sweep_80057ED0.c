// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game/save.h"

// Names the profile: its name and its created golfer's last name.
void fn_80057ED0(SaveProfile* pProfile, const char* pName) {
    strcpy(pProfile->createdGolfer.szLast, pName);
    strcpy(pProfile->szName, pName);
}

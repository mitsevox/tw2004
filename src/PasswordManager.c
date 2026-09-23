// PasswordManager.c (TW06's golf/earnings/passwordmanager.cpp; our spelling): the cheat codes
// typed in the options menu (PasswordManager_TestPassword) and the unlocks they set, which hold for
// every profile (lbl_80281DF4); and the setup of a new save profile. Not decompiled yet beyond the
// functions below.

#include "game/save.h"

// Names the profile: its name and its created golfer's last name.
void fn_80057ED0(SaveProfile* pProfile, const char* pName) {
    strcpy(pProfile->createdGolfer.szLast, pName);
    strcpy(pProfile->szName, pName);
}

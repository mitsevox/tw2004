// fe_craputils.c (TW06's golf/ui core/frontend/fe_craputils.c; TW07's FE_CrAPUtils.c): a save
// profile's Create-A-Player (CrAP) data and its unlocks. FE_CrAP_InitCrAPInfo resets the created
// golfer (choices, sliders, asset bits, logos); the FE_CrAP_*CustomAnimation functions keep its
// custom animation lists. The UserInfo_ functions unlock and test golfers, courses and rewards,
// keep the profile's flag bits and par-5 eagle records and count its ladder wins: TW07 has them
// as methods of its profile class, UserInfo (UserInfo.cpp), under these names. The starting
// sponsor of a new profile (FE_SetStartingSponsor) is kept here too.

#include "golfer.h"
#include "engine.h"
#include "game/save.h"
#include "charstate.h"
#include "frontend/fe.h"

char gszNoLogoName[] = "NoLogoName";  // a user logo's name until the player names it

void UserInfo_InitCrAPItemBitArrays(SaveProfile* pProfile);
void FE_CrAP_ResetSliders(SaveProfile* pProfile);

// Resets a profile's Create-A-Player data: clears 0x5500..0xB634 (the choices, the date, the asset
// slots and the four asset bit arrays), sets every part and set choice to -1 (none) and the 26
// sliders to 50 (FE_CrAP_ResetSliders), refreshes the asset bits, logos and slots from the database
// when it is loaded (UserInfo_InitCrAPItemBitArrays) and names the five user logos "NoLogoName".
// Called when a profile is loaded (MC_LoadUser, MC_LoadLastUser) or set up.
void FE_CrAP_InitCrAPInfo(SaveProfile* pProfile) {
    int i;

    // everything from the choices up to the tour (0x5500..0xB634): the choices, the date, the asset
    // slots and the four asset bit arrays
    memset(&pProfile->choices, 0,
           sizeof(pProfile->choices) + sizeof(pProfile->nDateMonth) + sizeof(pProfile->nDateDay) +
               sizeof(pProfile->nDateYear) + sizeof(pProfile->aAF80) + 4 * sizeof(pProfile->aAssetLocked));
    memset(pProfile->choices.aParts, -1, sizeof(pProfile->choices.aParts));
    memset(pProfile->choices.aSets, -1, sizeof(pProfile->choices.aSets));
    FE_CrAP_ResetSliders(pProfile);
    UserInfo_InitCrAPItemBitArrays(pProfile);
    for (i = 0; i < 5; i++) {
        strcpy(pProfile->choices.aLogo[i].szName, gszNoLogoName);
    }
}

// Only while the Create-A-Player database is loaded: for every asset, aAssetOwned is set for a
// level-0 asset (FE_CrAP_GetPartLevelFromAssetIndex) and cleared otherwise, aAssetNew and
// aAssetMarkedNew are cleared, and aAssetLocked is set as FE_CrAP_IsItemLocked answers with the asset's own
// gender made current (the current gender is put back after). Then the five user logos' bSaved are
// cleared and the 53 asset slots (aAF80) emptied (-1).
void UserInfo_InitCrAPItemBitArrays(SaveProfile* pProfile) {
    s8 nOffered;
    s32 nAssets;
    s32 i;

    if (FE_CrAP_IsCrAPDBLoaded()) {
        nOffered = FE_CrAP_GetCurrentGender();
        nAssets = FE_CrAP_GetNumEntriesInCrAPDB();
        for (i = 0; i < nAssets; i++) {
            if (FE_CrAP_GetPartLevelFromAssetIndex(i) == 0) {
                BitArray_SetBit(pProfile->aAssetOwned, i);
            } else {
                BitArray_ClearBit(pProfile->aAssetOwned, i);
            }
            BitArray_ClearBit(pProfile->aAssetNew, i);
            BitArray_ClearBit(pProfile->aAssetMarkedNew, i);
            FE_CrAP_SetCurrentGender(FE_CrAP_GetAssetGender(i));
            if (FE_CrAP_IsItemLocked(i, pProfile)) {
                BitArray_SetBit(pProfile->aAssetLocked, i);
            } else {
                BitArray_ClearBit(pProfile->aAssetLocked, i);
            }
        }
        FE_CrAP_SetCurrentGender(nOffered);
        for (i = 0; i < 5; i++) {
            pProfile->choices.aLogo[i].bSaved = 0;
        }
        for (i = 0; i < 53; i++) {
            pProfile->aAF80[i] = -1;
        }
    }
}

// Puts the created golfer's 26 sliders (choices.a9B4) at 50, the middle. EA sets them one by one,
// 25 before 24.
void FE_CrAP_ResetSliders(SaveProfile* pProfile) {
    pProfile->choices.a9B4[0] = 50;
    pProfile->choices.a9B4[1] = 50;
    pProfile->choices.a9B4[2] = 50;
    pProfile->choices.a9B4[3] = 50;
    pProfile->choices.a9B4[4] = 50;
    pProfile->choices.a9B4[5] = 50;
    pProfile->choices.a9B4[6] = 50;
    pProfile->choices.a9B4[7] = 50;
    pProfile->choices.a9B4[8] = 50;
    pProfile->choices.a9B4[9] = 50;
    pProfile->choices.a9B4[10] = 50;
    pProfile->choices.a9B4[11] = 50;
    pProfile->choices.a9B4[12] = 50;
    pProfile->choices.a9B4[13] = 50;
    pProfile->choices.a9B4[14] = 50;
    pProfile->choices.a9B4[15] = 50;
    pProfile->choices.a9B4[16] = 50;
    pProfile->choices.a9B4[17] = 50;
    pProfile->choices.a9B4[18] = 50;
    pProfile->choices.a9B4[19] = 50;
    pProfile->choices.a9B4[20] = 50;
    pProfile->choices.a9B4[21] = 50;
    pProfile->choices.a9B4[22] = 50;
    pProfile->choices.a9B4[23] = 50;
    pProfile->choices.a9B4[25] = 50;
    pProfile->choices.a9B4[24] = 50;
}

// Unlocks golfer nGolfer (0..29) for save profile nProfile (aGolferUnlocked) and triggers event
// 0x42 (EVENT_UnlockedNewCharacter). The ladder unlocks the opponent it beat (GameMode4_WinEvent).
void UserInfo_UnlockGolfer(int nProfile, int nGolfer) {
    gpSaveData[nProfile].aGolferUnlocked[nGolfer] = 1;
    EVENT_Trigger(nProfile, 0x42, NULL, -1);
}

// Sets (bSet) or clears flag bit nBit of the profile's aUserFlags. Bit 1 records that the Game Boy
// Advance link's unlocks were given (GM_vGbaGrantUnlocks).
void UserInfo_SetUserFlag(SaveProfile* pProfile, int nBit, u8 bSet) {
    if (bSet) {
        BitArray_SetBit(pProfile->aUserFlags, nBit);
    } else {
        BitArray_ClearBit(pProfile->aUserFlags, nBit);
    }
}

u8 UserInfo_GetUserFlag(SaveProfile* pProfile, int nBit) {
    return BitArray_TestBit(pProfile->aUserFlags, nBit);
}

// 1 when golfer nGolfer can be played with save profile nProfile: one of the first 30 that the
// profile has unlocked (aGolferUnlocked), or any golfer whose gGolferTable entry has bAvailable 1.
u8 UserInfo_IsGolferAvailable(int nProfile, int nGolfer) {
    if (nGolfer < 30) {
        if (gpSaveData[nProfile].aGolferUnlocked[nGolfer]) {
            return 1;
        }
        return (s8)gGolferTable[nGolfer].bAvailable == 1;
    }
    return (s8)gGolferTable[nGolfer].bAvailable == 1;
}

// Unlocks course nCourse (0..20) for save profile nProfile (aCourseUnlocked) and triggers event
// 0x43 (EVENT_UnlockedNewCourse). GM_Earnings_CheckUnlockCourses calls it when the profile's money
// reaches the course's price.
void UserInfo_UnlockCourse(int nProfile, int nCourse) {
    gpSaveData[nProfile].aCourseUnlocked[nCourse] = 1;
    EVENT_Trigger(nProfile, 0x43, NULL, -1);
}

u8 UserInfo_IsCourseUnlocked(int nProfile, int nCourse) {
    return gpSaveData[nProfile].aCourseUnlocked[nCourse] != 0;
}

// Unlocks reward nReward for save profile nProfile (aRewardUnlocked); no event. The ladder gives
// its event's reward (GameMode4_WinEvent).
void UserInfo_UnlockReward(int nProfile, int nReward) {
    gpSaveData[nProfile].aRewardUnlocked[nReward] = 1;
}

// Unlocks course slot 21 for save profile nProfile (aCourseUnlocked[21]; the course picker's choice
// 2, GM_vIsCourseChoiceUnlocked), with no event. GM_Earnings_CheckUnlockCourses buys it with price
// 23.
void UserInfo_UnlockCourseSlot21(int nProfile) {
    gpSaveData[nProfile].aCourseUnlocked[21] = 1;
}

u8 UserInfo_IsCourseSlot21Unlocked(int nProfile) {
    return gpSaveData[nProfile].aCourseUnlocked[21] != 0;
}

// Unlocks course slot 22 for save profile nProfile (aCourseUnlocked[22]; the course picker's choice
// 3, GM_vIsCourseChoiceUnlocked), with no event. GM_Earnings_CheckUnlockCourses buys it with price
// 21.
void UserInfo_UnlockCourseSlot22(int nProfile) {
    gpSaveData[nProfile].aCourseUnlocked[22] = 1;
}

u8 UserInfo_IsCourseSlot22Unlocked(int nProfile) {
    return gpSaveData[nProfile].aCourseUnlocked[22] != 0;
}

// The number of the 25 ladder events save profile nProfile has won (aLadderAward[].bWon).
// GM_Earnings_RateGolfer uses it as the profile's earnings rating.
int UserInfo_GetNumLadderEventsWon(int nProfile) {
    int nWon = 0;
    int i;

    for (i = 0; i < 25; i++) {
        if (gpSaveData[nProfile].aLadderAward[i].bWon) {
            nWon++;
        }
    }
    return nWon;
}

// Adds animation pName to the created golfer's custom animation list nKind (the entry of part 13 it
// belongs to): lists 0 and 1 (choices.aszCustomAnims0, aszCustomAnims1) take up to 8 names and
// ignore more; list 2 holds one name (szCustomAnim2), which this replaces and switches on
// (nCustomAnims2 = 1). Called when a part 13 asset is turned on (sTurnOnAnimation).
void FE_CrAP_AddCustomAnimation(SaveProfile* pProfile, int nKind, char* pName) {
    switch (nKind) {
    case 2:
        strcpy(pProfile->choices.szCustomAnim2, pName);
        pProfile->choices.nCustomAnims2 = 1;
        break;
    case 0:
        if (pProfile->choices.nCustomAnims0 < 8) {
            strcpy(pProfile->choices.aszCustomAnims0[pProfile->choices.nCustomAnims0], pName);
            pProfile->choices.nCustomAnims0++;
        }
        break;
    case 1:
        if (pProfile->choices.nCustomAnims1 < 8) {
            strcpy(pProfile->choices.aszCustomAnims1[pProfile->choices.nCustomAnims1], pName);
            pProfile->choices.nCustomAnims1++;
        }
        break;
    }
}

// Takes animation pName out of the created golfer's custom animation list nKind: in lists 0 and 1
// the names after it move up one; list 2 is simply switched off (nCustomAnims2 = 0) whatever pName
// is. A name not in the list changes nothing.
void FE_CrAP_RemoveCustomAnimation(SaveProfile* pProfile, int nKind, char* pName) {
    int nFound;
    int i;

    switch (nKind) {
    case 2:
        pProfile->choices.nCustomAnims2 = 0;
        break;
    case 0:
        nFound = -1;
        for (i = 0; i < pProfile->choices.nCustomAnims0; i++) {
            if (strcmp(pName, pProfile->choices.aszCustomAnims0[i]) == 0) {
                nFound = i;
                break;
            }
        }
        if (nFound >= 0) {
            for (; nFound < pProfile->choices.nCustomAnims0 - 1; nFound++) {
                strcpy(pProfile->choices.aszCustomAnims0[nFound],
                       pProfile->choices.aszCustomAnims0[nFound + 1]);
            }
            pProfile->choices.nCustomAnims0--;
        }
        break;
    case 1:
        nFound = -1;
        for (i = 0; i < pProfile->choices.nCustomAnims1; i++) {
            if (strcmp(pName, pProfile->choices.aszCustomAnims1[i]) == 0) {
                nFound = i;
                break;
            }
        }
        if (nFound >= 0) {
            for (; nFound < pProfile->choices.nCustomAnims1 - 1; nFound++) {
                strcpy(pProfile->choices.aszCustomAnims1[nFound],
                       pProfile->choices.aszCustomAnims1[nFound + 1]);
            }
            pProfile->choices.nCustomAnims1--;
        }
        break;
    }
}

// 1 when animation pName is in the created golfer's custom animation list nKind (0..2; list 2
// counts only while it is switched on). Any other nKind: 0.
u8 FE_CrAP_IsCustomAnimationSelected(SaveProfile* pProfile, int nKind, char* pName) {
    int i;

    switch (nKind) {
    case 0:
        for (i = 0; i < pProfile->choices.nCustomAnims0; i++) {
            if (strcmp(pProfile->choices.aszCustomAnims0[i], pName) == 0) {
                return 1;
            }
        }
        break;
    case 1:
        for (i = 0; i < pProfile->choices.nCustomAnims1; i++) {
            if (strcmp(pProfile->choices.aszCustomAnims1[i], pName) == 0) {
                return 1;
            }
        }
        break;
    case 2:
        // the list's one name, compared once per entry counted
        for (i = 0; i < pProfile->choices.nCustomAnims2; i++) {
            if (strcmp(pProfile->choices.szCustomAnim2, pName) == 0) {
                return 1;
            }
        }
        break;
    }
    return 0;
}

// Signs sponsor n (a Create-A-Player asset's n2C) into lbl_80281DF0, the sponsorship a new profile
// starts with; the profile setup (PasswordManager.c) copies it into the profile's first slot.
// Called by PGASponsor_PickStartingSponsor.
void FE_SetStartingSponsor(s16 n) {
    lbl_80281DF0.bSigned = 1;
    lbl_80281DF0.nSponsor = n;
}

int FE_GetStartingSponsor(void) {
    return lbl_80281DF0.nSponsor;
}

// Par-5 eagle record i (GM_ConvertCourseAndHoleToPar5EagleIndex, 0..74) of the profile: kind 0
// whether the hole has been eagled, kind 1 the date of that eagle (packed by FE_DateToInt); -1 for
// another kind. Records 0..70 are kept in a5004/a504C, 71..74 in a10578/a1057C.
int UserInfo_GetPar5EagleStat(SaveProfile* pProfile, int nKind, int i) {
    u8 bFirst = i < 71;

    switch (nKind) {
    case 0:
        if (bFirst) {
            return pProfile->a5004[i];
        }
        return pProfile->a10578[i - 71];
    case 1:
        if (bFirst) {
            return pProfile->a504C[i];
        }
        return pProfile->a1057C[i - 71];
    }
    return -1;
}

// Sets par-5 eagle record i of the profile to nValue: kind 0 the eagled flag (stored as a byte),
// kind 1 the eagle's date; another kind is ignored. Records 0..70 go to a5004/a504C, 71..74 to
// a10578/a1057C.
void UserInfo_SetPar5EagleStat(SaveProfile* pProfile, int nKind, int i, int nValue) {
    u8 bFirst = i < 71;

    switch (nKind) {
    case 0:
        if (bFirst) {
            pProfile->a5004[i] = nValue;
        } else {
            pProfile->a10578[i - 71] = nValue;
        }
        break;
    case 1:
        if (bFirst) {
            pProfile->a504C[i] = nValue;
        } else {
            pProfile->a1057C[i - 71] = nValue;
        }
        break;
    }
}

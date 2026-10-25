// PasswordManager.c (TW06's and TW07's golf/earnings/passwordmanager.cpp, a C++ class there; our
// spelling): the cheat codes typed in the menus (PasswordManager_TestPassword) and the unlocks they
// set, which hold for every profile (gpPasswordUnlocks, and the bit arrays gPasswordEnteredBits and
// gSponsorPasswordBits): IsPasswordEntered, IsSponsorshipPasswordEntered, TestPassword and
// SetDefaults, which TW07's class also has (TestPassword last there). Then, with no TW07
// counterpart in this file, the set-up of a new save profile (SaveProfile_*).

#include "game/save.h"
#include "frontend/fe.h"
#include "charstate.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"

u32 gPasswordEnteredBits[8];    // the cheats entered, one bit each (bits 0..6 used)
u32 gSponsorPasswordBits[16];   // the sponsors whose code was entered, bit n for sponsor n (0..15)

void SaveProfile_InitNew(SaveProfile* pProfile);
void SaveProfile_InitCreatedGolfer(SaveProfile* pProfile);

// Sponsor n's code sets bit n of gSponsorPasswordBits. "lsfkajfd" (3, 4, 8) and "CXCbr883" (12, 14)
// are listed more than once, and TestPassword stops at the first match, so bits 4, 8 and 14 have no
// code that sets them.
char* gSponsorPasswords[16] = {
    "91treSTR", "cgTR78qw", "CL45etUB", "lsfkajfd", "lsfkajfd", "FDGH597i", "YJHk342B", "Uit45TW6",
    "lsfkajfd", "kjnMR3qv", "R453DrTe", "BRi3498Z", "CXCbr883", "cDsa2fgY", "CXCbr883", "TS345329",
};

// Whether the cheat code for bit n of gPasswordEnteredBits was entered.
// PasswordManager_TestPassword sets bits 1..5 (the codes "A".."E") and 6 (SHERWOOD TARGET); no code
// sets bit 0, which a new profile's billion in cash and every Create-A-Player item unlocked test.
u8 PasswordManager_IsPasswordEntered(int n) {
    return BitArray_TestBit(gPasswordEnteredBits, n);
}

// Whether sponsor n's code (gSponsorPasswords[n]) was entered: bit n of gSponsorPasswordBits.
u8 PasswordManager_IsSponsorshipPasswordEntered(int n) {
    return BitArray_TestBit(gSponsorPasswordBits, n);
}

// Tests a code typed in the menus (menu message GM_vTestPassword); if it is a cheat, sets what it
// unlocks, which holds for every profile (gpPasswordUnlocks), and returns 1 (0: not a cheat).
// THEKITCHENSINK unlocks every golfer, course and reward and TOUR card level 1 (and clears
// gFEState.bTourCardWithheld); ALLTHETRACKS the courses and rewards; CANYOUPICKONE the golfers;
// fourteen codes one golfer each; SHERWOOD TARGET sets cheat bit 6. The 16 codes of
// gSponsorPasswords set their sponsor's bit (gSponsorPasswordBits), and "A".."E" cheat bits 1..5
// (gPasswordEnteredBits). ALLOFITSFREE is tested but does nothing.
u8 PasswordManager_TestPassword(char* szCode) {
    s32 aBit[5] = {1, 2, 3, 4, 5};
    char aszCode[5][32] = {"A", "B", "C", "D", "E"};
    int i;

    if (strcmp(szCode, "THEKITCHENSINK") == 0) {
        for (i = 0; i < 30; i++) {
            gpPasswordUnlocks->aGolferUnlocked[i] = 1;
        }
        for (i = 0; i < 21; i++) {
            gpPasswordUnlocks->aCourseUnlocked[i] = 1;
        }
        gpPasswordUnlocks->aCourseUnlocked[21] = 1;
        gpPasswordUnlocks->aCourseUnlocked[COURSE_TIGERS_DREAM_18] = 1;
        for (i = 0; i < 18; i++) {
            gpPasswordUnlocks->aRewardUnlocked[i] = 1;
        }
        gpPasswordUnlocks->nTourCardLevel = 1;
        gFEState.bTourCardWithheld = 0;
        return 1;
    }
    if (strcmp(szCode, "ALLTHETRACKS") == 0) {
        for (i = 0; i < 21; i++) {
            gpPasswordUnlocks->aCourseUnlocked[i] = 1;
        }
        gpPasswordUnlocks->aCourseUnlocked[21] = 1;
        gpPasswordUnlocks->aCourseUnlocked[COURSE_TIGERS_DREAM_18] = 1;
        for (i = 0; i < 18; i++) {
            gpPasswordUnlocks->aRewardUnlocked[i] = 1;
        }
        return 1;
    }
    if (strcmp(szCode, "CANYOUPICKONE") == 0) {
        for (i = 0; i < 30; i++) {
            gpPasswordUnlocks->aGolferUnlocked[i] = 1;
        }
        return 1;
    }
    // One golfer each.
    if (strcmp(szCode, "4REDSHIRTS") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[1] = 1;
        return 1;
    }
    if (strcmp(szCode, "ACEINTHEHOLE") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[2] = 1;
        return 1;
    }
    if (strcmp(szCode, "DISCOKING") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[8] = 1;
        return 1;
    }
    if (strcmp(szCode, "SHORTGAME") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[15] = 1;
        return 1;
    }
    if (strcmp(szCode, "DWILBY") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[17] = 1;
        return 1;
    }
    if (strcmp(szCode, "EMERALDCHAMP") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[19] = 1;
        return 1;
    }
    if (strcmp(szCode, "TRAVELER") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[20] = 1;
        return 1;
    }
    if (strcmp(szCode, "BEVERLYHILLS") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[25] = 1;
        return 1;
    }
    if (strcmp(szCode, "THENEWLEFTY") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[27] = 1;
        return 1;
    }
    if (strcmp(szCode, "CEDDYBEAR") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[9] = 1;
        return 1;
    }
    if (strcmp(szCode, "DTBROWN") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[6] = 1;
        return 1;
    }
    if (strcmp(szCode, "EDDIE") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[16] = 1;
        return 1;
    }
    if (strcmp(szCode, "ERUPTION") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[26] = 1;
        return 1;
    }
    if (strcmp(szCode, "ICYONE") == 0) {
        gpPasswordUnlocks->aGolferUnlocked[29] = 1;
        return 1;
    }
    if (stricmp(szCode, "SHERWOOD TARGET") == 0) {
        BitArray_SetBit(gPasswordEnteredBits, 6);
        return 1;
    }
    // This code does nothing: the test is made and its result is not used.
    if (strcmp(szCode, "ALLOFITSFREE") == 0) {
    }
    for (i = 0; i < 16; i++) {
        if (strcmp(szCode, gSponsorPasswords[i]) == 0) {
            BitArray_SetBit(gSponsorPasswordBits, i);
            return 1;
        }
    }
    for (i = 0; i < 5; i++) {
        if (strcmp(szCode, aszCode[i]) == 0) {
            BitArray_SetBit(gPasswordEnteredBits, aBit[i]);
            return 1;
        }
    }
    return 0;
}

// Sets the unlocks that hold for every profile (gpPasswordUnlocks) to their defaults at start-up (user.c
// UserManager_OpenOnce): a new profile's (SaveProfile_InitNew), not active, with the starting golfers and
// courses, only reward 0, no money, stats, awards or medals, no TOUR card, and every cheat bit
// cleared (gPasswordEnteredBits 0..6, gSponsorPasswordBits).
void PasswordManager_SetDefaults(void) {
    int i;

    SaveProfile_InitNew(gpPasswordUnlocks);
    BitArray_ClearArray(gPasswordEnteredBits, 7);
    BitArray_ClearArray(gSponsorPasswordBits, 16);
    gpPasswordUnlocks->bActive = 0;
    for (i = 0; i < 16; i++) {
        gpPasswordUnlocks->aGolferUnlocked[gStartUnlockedGolfers[i]] = 1;
    }
    for (i = 0; i < 21; i++) {
        gpPasswordUnlocks->aCourseUnlocked[i] = 1;
    }
    gpPasswordUnlocks->aCourseUnlocked[21] = 1;
    gpPasswordUnlocks->aCourseUnlocked[COURSE_TIGERS_DREAM_18] = 1;
    for (i = 0; i < 6; i++) {
        gpPasswordUnlocks->aCourseUnlocked[gStartLockedCourses[i]] = 0;
    }
    gpPasswordUnlocks->aCourseUnlocked[21] = 1;
    gpPasswordUnlocks->aCourseUnlocked[COURSE_TIGERS_DREAM_18] = 1;
    for (i = 0; i < 18; i++) {
        gpPasswordUnlocks->aRewardUnlocked[i] = 0;
    }
    gpPasswordUnlocks->aRewardUnlocked[0] = 1;

    gpPasswordUnlocks->nTotalCash = 0;
    gpPasswordUnlocks->nCurrentCash = 0;
    gpPasswordUnlocks->nStrokeRounds = 0;
    gpPasswordUnlocks->nStrokeRoundStrokes = 0;
    gpPasswordUnlocks->nRounds = 0;
    gpPasswordUnlocks->nPuttHoles = 0;
    gpPasswordUnlocks->nPutts = 0;
    gpPasswordUnlocks->nDrives = 0;
    gpPasswordUnlocks->nDriveDistance = 0;
    gpPasswordUnlocks->nFairways = 0;
    gpPasswordUnlocks->nFairwaysHit = 0;
    gpPasswordUnlocks->nHoles = 0;
    gpPasswordUnlocks->nGreensHit = 0;
    gpPasswordUnlocks->nLongestDrive = 0;
    gpPasswordUnlocks->nLongestPutt = 0;
    gpPasswordUnlocks->nBestRound = 0;
    gpPasswordUnlocks->bChanged = 0;
    for (i = 0; i < 25; i++) {
        gpPasswordUnlocks->aLadderAward[i].bWon = 0;
    }
    for (i = 0; i < 31; i++) {
        gpPasswordUnlocks->aC8[i].award.bWon = 0;
    }
    for (i = 0; i < 16; i++) {
        gpPasswordUnlocks->aTourAward[i].bWon = 0;
    }
    for (i = 0; i < 3; i++) {
        gpPasswordUnlocks->aMoneyListAward[i].bWon = 0;
    }
    // EA bug: runs past the 75 awards, as in SaveProfile_InitNew.
    for (i = 0; i < 118; i++) {
        gpPasswordUnlocks->aRTEAward[i].bWon = 0;
    }
    for (i = 0; i < 75; i++) {
        UserInfo_SetPar5EagleStat(gpPasswordUnlocks, 0, i, 0);
    }
    for (i = 0; i < 29; i++) {
        gpPasswordUnlocks->aMedal[i] = 3;
    }
    gpPasswordUnlocks->n5168 = 3;

    gpPasswordUnlocks->nHolesInOne = 0;
    gpPasswordUnlocks->nAlbatrosses = 0;
    gpPasswordUnlocks->nEagles = 0;
    gpPasswordUnlocks->nBirdies = 0;
    gpPasswordUnlocks->nPars = 0;
    gpPasswordUnlocks->nBogeys = 0;
    gpPasswordUnlocks->nDoubleBogeys = 0;
    for (i = 0; i < 39; i++) {
        gpPasswordUnlocks->aAward[i].bWon = 0;
    }
    for (i = 0; i < 15; i++) {
        gpPasswordUnlocks->aTipSeen[i] = 0;
    }
    gpPasswordUnlocks->bCaddieTipsOff = 0;
    gpPasswordUnlocks->nTourCardLevel = 0;
}

// Sets save slot nSlot up as a new profile (SaveProfile_InitNew) named "USER<nSlot + 1>", not
// loaded in the menus (gFEState.aLoaded). Called for all five slots at start-up (user.c) and by
// GM_SetupDefaultProfile.
void SaveProfile_InitSlot(int nSlot) {
    char szName[16];

    gFEState.aLoaded[nSlot] = 0;
    SaveProfile_InitNew(&gpSaveData[nSlot]);
    sprintf(szName, "USER%d", nSlot + 1);
    strcpy(gpSaveData[nSlot].szName, szName);
}

// Gives player slot 0 a profile named "Dummy", active and loaded, if none is loaded (at boot,
// GoEntry.c).
void SaveProfile_SetupDummy(void) {
    SaveProfile* pProfile = gpSaveData;

    if (gFEState.aLoaded[0] == 0) {
        strcpy(pProfile->szName, "Dummy");
        pProfile->bActive = 1;
        gFEState.aLoaded[0] = 1;
    }
}

// Sets a save profile up as a new one: cleared and not active; named "User <n>" after the slot the
// menus work on in game type 3 ("NoName" otherwise); the starting golfers (gStartUnlockedGolfers)
// and courses (all but gStartLockedCourses) unlocked, reward 0 only; 25000 in cash plus
// gFEState.nMCRewardMoney (plus the first sponsor's start cash when one is signed; a billion with cheat bit
// 0); no stats, awards, medals (aMedal 3) or TOUR card; three empty saved rounds, the default
// created golfer (SaveProfile_InitCreatedGolfer), no PGA TOUR seasons, the Create-A-Player defaults
// (FE_CrAP_InitCrAPInfo) and the first sponsor (gStartingSponsor) signed when there is one.
void SaveProfile_InitNew(SaveProfile* pProfile) {
    int i;
    int j;

    memset(pProfile, 0, sizeof(SaveProfile));
    pProfile->bActive = 0;
    if (gSession.nGameType == GAMESTATE_FE_RUN) {
        sprintf(pProfile->szName, "User %d", gpFEProfile->nSlot + 1);
    } else {
        strcpy(pProfile->szName, "NoName");
    }
    pProfile->szName[10] = 0;

    for (i = 0; i < 30; i++) {
        pProfile->aGolferUnlocked[i] = 0;
    }
    for (i = 0; i < 11; i++) {
        pProfile->aSponsor[i].bSigned = 0;
        pProfile->aSponsor[i].nSponsor = 0;
    }
    for (i = 0; i < 16; i++) {
        pProfile->aGolferUnlocked[gStartUnlockedGolfers[i]] = 1;
    }
    for (i = 0; i < 21; i++) {
        pProfile->aCourseUnlocked[i] = 1;
    }
    pProfile->aCourseUnlocked[21] = 1;
    pProfile->aCourseUnlocked[COURSE_TIGERS_DREAM_18] = 1;
    for (i = 0; i < 6; i++) {
        pProfile->aCourseUnlocked[gStartLockedCourses[i]] = 0;
    }
    pProfile->aCourseUnlocked[21] = 1;
    pProfile->aCourseUnlocked[COURSE_TIGERS_DREAM_18] = 1;
    for (i = 0; i < 18; i++) {
        pProfile->aRewardUnlocked[i] = 0;
    }
    pProfile->aRewardUnlocked[0] = 1;

    pProfile->nTotalCash = 0;
    pProfile->n68 = 0;
    if (PasswordManager_IsPasswordEntered(0)) {
        pProfile->nCurrentCash = 1000000000;
    } else {
        pProfile->nCurrentCash = gFEState.nMCRewardMoney + 25000;
        if (gStartingSponsor.bSigned) {
            pProfile->nCurrentCash += GameModeDriverPGATour_GetSponsorshipStartCash(0);
        }
    }

    pProfile->nStrokeRounds = 0;
    pProfile->nStrokeRoundStrokes = 0;
    pProfile->nRounds = 0;
    pProfile->nPuttHoles = 0;
    pProfile->nPutts = 0;
    pProfile->nDrives = 0;
    pProfile->nDriveDistance = 0;
    pProfile->nFairways = 0;
    pProfile->nFairwaysHit = 0;
    pProfile->nHoles = 0;
    pProfile->nGreensHit = 0;
    pProfile->nLongestDrive = 0;
    pProfile->nLongestPutt = 0;
    pProfile->nBestRound = 0;
    pProfile->bChanged = 0;
    for (i = 0; i < 25; i++) {
        pProfile->aLadderAward[i].bWon = 0;
    }
    for (i = 0; i < 31; i++) {
        pProfile->aC8[i].award.bWon = 0;
    }
    for (i = 0; i < 16; i++) {
        pProfile->aTourAward[i].bWon = 0;
    }
    for (i = 0; i < 3; i++) {
        pProfile->aMoneyListAward[i].bWon = 0;
    }
    // EA bug: 118 (the real-time event count) runs past the 75 awards into aLadderAward and
    // aAward, which are cleared anyway.
    for (i = 0; i < 118; i++) {
        pProfile->aRTEAward[i].bWon = 0;
    }
    for (i = 0; i < 75; i++) {
        UserInfo_SetPar5EagleStat(pProfile, 0, i, 0);
    }
    for (i = 0; i < 29; i++) {
        pProfile->aMedal[i] = 3;
    }
    pProfile->n5168 = 3;

    pProfile->nHolesInOne = 0;
    pProfile->nAlbatrosses = 0;
    pProfile->nEagles = 0;
    pProfile->nBirdies = 0;
    pProfile->nPars = 0;
    pProfile->nBogeys = 0;
    pProfile->nDoubleBogeys = 0;
    for (i = 0; i < 39; i++) {
        pProfile->aAward[i].bWon = 0;
    }
    for (i = 0; i < 15; i++) {
        pProfile->aTipSeen[i] = 0;
    }
    pProfile->bCaddieTipsOff = 0;
    pProfile->nTourCardLevel = 0;

    for (i = 0; i < NUM_SAVED_ROUNDS; i++) {
        pProfile->aSavedRound[i].bInUse = 0;
        pProfile->aSavedRound[i].n15 = 1;
        for (j = 0; j < 18; j++) {
            pProfile->aSavedRound[i].nHoleNum[j] = -1;
            pProfile->aSavedRound[i].nCourse[j] = 0;
        }
    }

    SaveProfile_InitCreatedGolfer(pProfile);
    pProfile->unk54C0[0] = 0;
    pProfile->unk54C0[1] = 0;
    pProfile->nGolferGlove = 0;
    for (i = 0; i < 6; i++) {
        SKA_PackName(&pProfile->aGolferNames[i], "");
    }
    pProfile->nGolferBallType = 0;
    pProfile->nGolferOutfit = -1;
    GM_PgaTourSim_ClearAllSeasons(&pProfile->tour);
    BitArray_ClearArray(pProfile->aUserFlags, 2);
    FE_CrAP_InitCrAPInfo(pProfile);
    if (gStartingSponsor.bSigned) {
        pProfile->aSponsor[0].bSigned = 1;
        pProfile->aSponsor[0].nSponsor = gStartingSponsor.nSponsor;
    }
}

// The created golfer of a new profile: available, model 0, last name "NoName", nickname "NA", the
// bag 0x02A7FC44 and every attribute 10 (105 with bit 0x4000 of the session's flags).
void SaveProfile_InitCreatedGolfer(SaveProfile* pProfile) {
    int i;

    memset(&pProfile->createdGolfer, 0, sizeof(GolferRecord));
    pProfile->createdGolfer.bAvailable = 1;
    pProfile->createdGolfer.nModelID = 0;
    strcpy(pProfile->createdGolfer.szLast, "NoName");
    strcpy(pProfile->createdGolfer.szFirst, "");
    strcpy(pProfile->createdGolfer.szNick, "NA");
    pProfile->createdGolfer.uBagMask = 0x02A7FC44;
    for (i = 0; i < NUM_ATTRS; i++) {
        pProfile->createdGolfer.attr[i] = 10;
        if (gSession.uFlags & 0x4000) {
            pProfile->createdGolfer.attr[i] = 105;
        }
    }
}

// Names the profile: its name and its created golfer's last name.
void SaveProfile_SetName(SaveProfile* pProfile, const char* pName) {
    strcpy(pProfile->createdGolfer.szLast, pName);
    strcpy(pProfile->szName, pName);
}

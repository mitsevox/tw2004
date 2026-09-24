// PasswordManager.c (TW06's golf/earnings/passwordmanager.cpp; our spelling): the cheat codes
// typed in the options menu (PasswordManager_TestPassword) and the unlocks they set, which hold for
// every profile (lbl_80281DF4, and the bit arrays lbl_801D5948 and lbl_801D5908); and the setup of
// a new save profile. Not decompiled yet beyond the functions below.

#include "game/save.h"
#include "frontend/fe.h"
#include "charstate.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"

void fn_80057438(SaveProfile* pProfile);
void fn_80057DA4(SaveProfile* pProfile);

// The codes that set a bit of lbl_801D5908, one each (some bits have two codes).
char* lbl_80188024[16] = {
    "91treSTR", "cgTR78qw", "CL45etUB", "lsfkajfd", "lsfkajfd", "FDGH597i", "YJHk342B", "Uit45TW6",
    "lsfkajfd", "kjnMR3qv", "R453DrTe", "BRi3498Z", "CXCbr883", "cDsa2fgY", "CXCbr883", "TS345329",
};

// Bit n of lbl_801D5948: the cheat that sets it has been entered.
u8 fn_80056480(int n) {
    return fn_8001E9CC(lbl_801D5948, n);
}

// Bit n of lbl_801D5908.
u8 fn_800564AC(int n) {
    return fn_8001E9CC(lbl_801D5908, n);
}

// Tests a typed code; if it is a cheat, sets what it unlocks and returns 1.
u8 PasswordManager_TestPassword(char* szCode) {
    s32 aBit[5] = {1, 2, 3, 4, 5};
    char aszCode[5][32] = {"A", "B", "C", "D", "E"};
    int i;

    if (strcmp(szCode, "THEKITCHENSINK") == 0) {
        for (i = 0; i < 30; i++) {
            lbl_80281DF4->aGolferUnlocked[i] = 1;
        }
        for (i = 0; i < 21; i++) {
            lbl_80281DF4->aCourseUnlocked[i] = 1;
        }
        lbl_80281DF4->aCourseUnlocked[21] = 1;
        lbl_80281DF4->aCourseUnlocked[22] = 1;
        for (i = 0; i < 18; i++) {
            lbl_80281DF4->aRewardUnlocked[i] = 1;
        }
        lbl_80281DF4->nTourCardLevel = 1;
        lbl_801D7148.b18 = 0;
        return 1;
    }
    if (strcmp(szCode, "ALLTHETRACKS") == 0) {
        for (i = 0; i < 21; i++) {
            lbl_80281DF4->aCourseUnlocked[i] = 1;
        }
        lbl_80281DF4->aCourseUnlocked[21] = 1;
        lbl_80281DF4->aCourseUnlocked[22] = 1;
        for (i = 0; i < 18; i++) {
            lbl_80281DF4->aRewardUnlocked[i] = 1;
        }
        return 1;
    }
    if (strcmp(szCode, "CANYOUPICKONE") == 0) {
        for (i = 0; i < 30; i++) {
            lbl_80281DF4->aGolferUnlocked[i] = 1;
        }
        return 1;
    }
    // One golfer each.
    if (strcmp(szCode, "4REDSHIRTS") == 0) {
        lbl_80281DF4->aGolferUnlocked[1] = 1;
        return 1;
    }
    if (strcmp(szCode, "ACEINTHEHOLE") == 0) {
        lbl_80281DF4->aGolferUnlocked[2] = 1;
        return 1;
    }
    if (strcmp(szCode, "DISCOKING") == 0) {
        lbl_80281DF4->aGolferUnlocked[8] = 1;
        return 1;
    }
    if (strcmp(szCode, "SHORTGAME") == 0) {
        lbl_80281DF4->aGolferUnlocked[15] = 1;
        return 1;
    }
    if (strcmp(szCode, "DWILBY") == 0) {
        lbl_80281DF4->aGolferUnlocked[17] = 1;
        return 1;
    }
    if (strcmp(szCode, "EMERALDCHAMP") == 0) {
        lbl_80281DF4->aGolferUnlocked[19] = 1;
        return 1;
    }
    if (strcmp(szCode, "TRAVELER") == 0) {
        lbl_80281DF4->aGolferUnlocked[20] = 1;
        return 1;
    }
    if (strcmp(szCode, "BEVERLYHILLS") == 0) {
        lbl_80281DF4->aGolferUnlocked[25] = 1;
        return 1;
    }
    if (strcmp(szCode, "THENEWLEFTY") == 0) {
        lbl_80281DF4->aGolferUnlocked[27] = 1;
        return 1;
    }
    if (strcmp(szCode, "CEDDYBEAR") == 0) {
        lbl_80281DF4->aGolferUnlocked[9] = 1;
        return 1;
    }
    if (strcmp(szCode, "DTBROWN") == 0) {
        lbl_80281DF4->aGolferUnlocked[6] = 1;
        return 1;
    }
    if (strcmp(szCode, "EDDIE") == 0) {
        lbl_80281DF4->aGolferUnlocked[16] = 1;
        return 1;
    }
    if (strcmp(szCode, "ERUPTION") == 0) {
        lbl_80281DF4->aGolferUnlocked[26] = 1;
        return 1;
    }
    if (strcmp(szCode, "ICYONE") == 0) {
        lbl_80281DF4->aGolferUnlocked[29] = 1;
        return 1;
    }
    if (stricmp(szCode, "SHERWOOD TARGET") == 0) {
        fn_8001EA34(lbl_801D5948, 6);
        return 1;
    }
    // This code does nothing: the test is made and its result is not used.
    if (strcmp(szCode, "ALLOFITSFREE") == 0) {
    }
    for (i = 0; i < 16; i++) {
        if (strcmp(szCode, lbl_80188024[i]) == 0) {
            fn_8001EA34(lbl_801D5908, i);
            return 1;
        }
    }
    for (i = 0; i < 5; i++) {
        if (strcmp(szCode, aszCode[i]) == 0) {
            fn_8001EA34(lbl_801D5948, aBit[i]);
            return 1;
        }
    }
    return 0;
}

// Sets up save profile nSlot as a new one named "USER<n>", not loaded.
void fn_80057364(int nSlot) {
    char szName[16];

    lbl_801D7148.aLoaded[nSlot] = 0;
    fn_80057438(&gpSaveData[nSlot]);
    sprintf(szName, "USER%d", nSlot + 1);
    strcpy(gpSaveData[nSlot].szName, szName);
}

// Gives player slot 0 a profile named "Dummy" if none is loaded.
void fn_800573E4(void) {
    SaveProfile* pProfile = gpSaveData;

    if (lbl_801D7148.aLoaded[0] == 0) {
        strcpy(pProfile->szName, "Dummy");
        pProfile->bActive = 1;
        lbl_801D7148.aLoaded[0] = 1;
    }
}

// Sets up a new save profile: named "User <slot>" in game type 3 ("NoName" otherwise), the
// starting golfers and courses unlocked, the starting money, no stats, awards or medals, three
// empty saved rounds and the default created golfer.
void fn_80057438(SaveProfile* pProfile) {
    int i;
    int j;

    memset(pProfile, 0, sizeof(SaveProfile));
    pProfile->bActive = 0;
    if (gSession.nGameType == 3) {
        sprintf(pProfile->szName, "User %d", lbl_80281ED4->nSlot + 1);
    } else {
        strcpy(pProfile->szName, "NoName");
    }
    pProfile->szName[10] = 0;

    for (i = 0; i < 30; i++) {
        pProfile->aGolferUnlocked[i] = 0;
    }
    for (i = 0; i < 11; i++) {
        pProfile->a1054C[i].b = 0;
        pProfile->a1054C[i].n = 0;
    }
    for (i = 0; i < 16; i++) {
        pProfile->aGolferUnlocked[lbl_801894E8[i]] = 1;
    }
    for (i = 0; i < 21; i++) {
        pProfile->aCourseUnlocked[i] = 1;
    }
    pProfile->aCourseUnlocked[21] = 1;
    pProfile->aCourseUnlocked[22] = 1;
    for (i = 0; i < 6; i++) {
        pProfile->aCourseUnlocked[lbl_801894D0[i]] = 0;
    }
    pProfile->aCourseUnlocked[21] = 1;
    pProfile->aCourseUnlocked[22] = 1;
    for (i = 0; i < 18; i++) {
        pProfile->aRewardUnlocked[i] = 0;
    }
    pProfile->aRewardUnlocked[0] = 1;

    pProfile->n64 = 0;
    pProfile->n68 = 0;
    if (fn_80056480(0)) {
        pProfile->n6C = 1000000000;
    } else {
        pProfile->n6C = lbl_801D7148.n1C + 25000;
        if (lbl_80281DF0.b) {
            pProfile->n6C += fn_800F02EC(0);
        }
    }

    pProfile->n74 = 0;
    pProfile->n78 = 0;
    pProfile->n7C = 0;
    pProfile->n80 = 0;
    pProfile->n84 = 0;
    pProfile->n88 = 0;
    pProfile->n8C = 0;
    pProfile->n90 = 0;
    pProfile->n94 = 0;
    pProfile->n98 = 0;
    pProfile->n9C = 0;
    pProfile->nA0 = 0;
    pProfile->nA4 = 0;
    pProfile->nA8 = 0;
    pProfile->b70 = 0;
    for (i = 0; i < 25; i++) {
        pProfile->aLadderAward[i].bWon = 0;
    }
    for (i = 0; i < 31; i++) {
        pProfile->aC8[i].award.bWon = 0;
    }
    for (i = 0; i < 16; i++) {
        pProfile->a1C0[i].bWon = 0;
    }
    for (i = 0; i < 3; i++) {
        pProfile->a200[i].bWon = 0;
    }
    // EA bug: 118 (the real-time event count) runs past the 75 awards into aLadderAward and
    // aAward, which are cleared anyway.
    for (i = 0; i < 118; i++) {
        pProfile->aRTEAward[i].bWon = 0;
    }
    for (i = 0; i < 75; i++) {
        fn_8005897C(pProfile, 0, i, 0);
    }
    for (i = 0; i < 29; i++) {
        pProfile->aMedal[i] = 3;
    }
    pProfile->n5168 = 3;

    pProfile->nAC = 0;
    pProfile->nB0 = 0;
    pProfile->nB4 = 0;
    pProfile->nB8 = 0;
    pProfile->nBC = 0;
    pProfile->nC0 = 0;
    pProfile->nC4 = 0;
    for (i = 0; i < 39; i++) {
        pProfile->aAward[i].bWon = 0;
    }
    for (i = 0; i < 15; i++) {
        pProfile->aTipSeen[i] = 0;
    }
    pProfile->b522F = 0;
    pProfile->nTourCardLevel = 0;

    for (i = 0; i < NUM_SAVED_ROUNDS; i++) {
        pProfile->aSavedRound[i].n0 = 0;
        pProfile->aSavedRound[i].n15 = 1;
        for (j = 0; j < 18; j++) {
            pProfile->aSavedRound[i].nHoleNum[j] = -1;
            pProfile->aSavedRound[i].nCourse[j] = 0;
        }
    }

    fn_80057DA4(pProfile);
    pProfile->unk54C0[0] = 0;
    pProfile->unk54C0[1] = 0;
    pProfile->n54C2 = 0;
    for (i = 0; i < 6; i++) {
        fn_800CB700(&pProfile->aGolferNames[i], "");
    }
    pProfile->nGolferBallType = 0;
    pProfile->nGolferOutfit = -1;
    fn_801176C0(&pProfile->tour);
    fn_8001E938(pProfile->a10548, 2);
    FE_CrAP_InitCrAPInfo(pProfile);
    if (lbl_80281DF0.b) {
        pProfile->a1054C[0].b = 1;
        pProfile->a1054C[0].n = lbl_80281DF0.n;
    }
}

// The created golfer of a new profile: "NoName", every attribute 10 (105 with bit 0x4000 of the
// session's flags) and a set bag.
void fn_80057DA4(SaveProfile* pProfile) {
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
void fn_80057ED0(SaveProfile* pProfile, const char* pName) {
    strcpy(pProfile->createdGolfer.szLast, pName);
    strcpy(pProfile->szName, pName);
}

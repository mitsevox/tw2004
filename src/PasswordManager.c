// PasswordManager.c (TW06's golf/earnings/passwordmanager.cpp; our spelling): the cheat codes
// typed in the options menu (PasswordManager_TestPassword) and the unlocks they set, which hold for
// every profile (lbl_80281DF4, and the bit arrays lbl_801D5948 and lbl_801D5908); and the setup of
// a new save profile. Not decompiled yet beyond the functions below.

#include "game/save.h"
#include "frontend/fe.h"
#include "charstate.h"

void fn_80057438(SaveProfile* pProfile);

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
int PasswordManager_TestPassword(char* szCode) {
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

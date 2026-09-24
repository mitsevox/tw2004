// fe_craputils.c (TW06's golf/ui core/frontend/fe_craputils.c): the Create-A-Player (CrAP) data in
// a save profile. FE_CrAP_InitCrAPInfo resets it; the rest unlock golfers, courses and rewards,
// rate the profile, and keep the created golfer's name lists.

#include "golfer.h"
#include "engine.h"
#include "game/save.h"
#include "charstate.h"
#include "frontend/fe.h"

void fn_80057FBC(SaveProfile* pProfile);
void fn_80058208(SaveProfile* pProfile);

// Resets the Create-A-Player data: nothing chosen, the sliders at 50, the asset bits refreshed and
// the logos renamed.
void FE_CrAP_InitCrAPInfo(SaveProfile* pProfile) {
    int i;

    // everything from the choices up to the tour (0x5500..0xB634): the choices, the date, the asset
    // slots and the four asset bit arrays
    memset(&pProfile->choices, 0,
           sizeof(pProfile->choices) + sizeof(pProfile->nDateMonth) + sizeof(pProfile->nDateDay) +
               sizeof(pProfile->nDateYear) + sizeof(pProfile->aAF80) + 4 * sizeof(pProfile->aAssetLocked));
    memset(pProfile->choices.aParts, -1, sizeof(pProfile->choices.aParts));
    memset(pProfile->choices.aSets, -1, sizeof(pProfile->choices.aSets));
    fn_80058208(pProfile);
    fn_80057FBC(pProfile);
    for (i = 0; i < 5; i++) {
        strcpy(pProfile->choices.aLogo[i].szName, lbl_80188138);
    }
}

// Refreshes the four asset bit arrays from the Create-A-Player database (when it is loaded), then
// clears the logos' b1020 and empties the asset slots.
void fn_80057FBC(SaveProfile* pProfile) {
    s8 nOffered;
    s32 nAssets;
    s32 i;

    if (fn_80105C30()) {
        nOffered = fn_80103BB4();
        nAssets = fn_80105C00();
        for (i = 0; i < nAssets; i++) {
            if (fn_80105C0C(i) == 0) {
                fn_8001EA34(pProfile->aB1CC, i);
            } else {
                fn_8001EB6C(pProfile->aB1CC, i);
            }
            fn_8001EB6C(pProfile->aB344, i);
            fn_8001EB6C(pProfile->aB4BC, i);
            fn_80103B8C(fn_80103BC0(i));
            if (fn_80078008(i, pProfile)) {
                fn_8001EA34(pProfile->aAssetLocked, i);
            } else {
                fn_8001EB6C(pProfile->aAssetLocked, i);
            }
        }
        fn_80103B8C(nOffered);
        for (i = 0; i < 5; i++) {
            pProfile->choices.aLogo[i].b1020 = 0;
        }
        for (i = 0; i < 53; i++) {
            pProfile->aAF80[i] = -1;
        }
    }
}

// The 26 bytes at 0x5EB4 start at 50, set one by one (the last two in EA's order: 25, then 24).
void fn_80058208(SaveProfile* pProfile) {
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

// Unlock golfer nGolfer for profile nProfile, and tell the event table (event 0x42).
void fn_80058278(int nProfile, int nGolfer) {
    gpSaveData[nProfile].aGolferUnlocked[nGolfer] = 1;
    EVENT_Trigger(nProfile, 0x42, NULL, -1);
}

void fn_800582C4(SaveProfile* pProfile, int nBit, u8 bSet) {
    if (bSet) {
        fn_8001EA34(pProfile->a10548, nBit);
    } else {
        fn_8001EB6C(pProfile->a10548, nBit);
    }
}

u8 fn_80058304(SaveProfile* pProfile, int nBit) {
    return fn_8001E9CC(pProfile->a10548, nBit);
}

// Golfer nGolfer is unlocked for profile nProfile, or available to everyone.
u8 fn_8005832C(int nProfile, int nGolfer) {
    if (nGolfer < 30) {
        if (gpSaveData[nProfile].aGolferUnlocked[nGolfer]) {
            return 1;
        }
        return (s8)gGolferTable[nGolfer].bAvailable == 1;
    }
    return (s8)gGolferTable[nGolfer].bAvailable == 1;
}

// Unlock course nCourse for profile nProfile, and tell the event table (event 0x43).
void fn_800583B0(int nProfile, int nCourse) {
    gpSaveData[nProfile].aCourseUnlocked[nCourse] = 1;
    EVENT_Trigger(nProfile, 0x43, NULL, -1);
}

u8 fn_800583FC(int nProfile, int nCourse) {
    return gpSaveData[nProfile].aCourseUnlocked[nCourse] != 0;
}

void fn_80058428(int nProfile, int nReward) {
    gpSaveData[nProfile].aRewardUnlocked[nReward] = 1;
}

void fn_8005844C(int nProfile) {
    gpSaveData[nProfile].aCourseUnlocked[21] = 1;
}

u8 fn_8005846C(int nProfile) {
    return gpSaveData[nProfile].aCourseUnlocked[21] != 0;
}

void fn_80058494(int nProfile) {
    gpSaveData[nProfile].aCourseUnlocked[22] = 1;
}

u8 fn_800584B4(int nProfile) {
    return gpSaveData[nProfile].aCourseUnlocked[22] != 0;
}

// The number of ladder events profile nProfile has won.
int fn_800584DC(int nProfile) {
    int nWon = 0;
    int i;

    for (i = 0; i < 25; i++) {
        if (gpSaveData[nProfile].aLadderAward[i].bWon) {
            nWon++;
        }
    }
    return nWon;
}

void fn_80058560(SaveProfile* pProfile, int nKind, char* pName) {
    switch (nKind) {
    case 2:
        strcpy(pProfile->choices.sz103, pName);
        pProfile->choices.n102 = 1;
        break;
    case 0:
        if (pProfile->choices.n0 < 8) {
            strcpy(pProfile->choices.a1[pProfile->choices.n0], pName);
            pProfile->choices.n0++;
        }
        break;
    case 1:
        if (pProfile->choices.n81 < 8) {
            strcpy(pProfile->choices.a82[pProfile->choices.n81], pName);
            pProfile->choices.n81++;
        }
        break;
    }
}

void fn_80058624(SaveProfile* pProfile, int nKind, char* pName) {
    int nFound;
    int i;

    switch (nKind) {
    case 2:
        pProfile->choices.n102 = 0;
        break;
    case 0:
        nFound = -1;
        for (i = 0; i < pProfile->choices.n0; i++) {
            if (strcmp(pName, pProfile->choices.a1[i]) == 0) {
                nFound = i;
                break;
            }
        }
        if (nFound >= 0) {
            for (; nFound < pProfile->choices.n0 - 1; nFound++) {
                strcpy(pProfile->choices.a1[nFound], pProfile->choices.a1[nFound + 1]);
            }
            pProfile->choices.n0--;
        }
        break;
    case 1:
        nFound = -1;
        for (i = 0; i < pProfile->choices.n81; i++) {
            if (strcmp(pName, pProfile->choices.a82[i]) == 0) {
                nFound = i;
                break;
            }
        }
        if (nFound >= 0) {
            for (; nFound < pProfile->choices.n81 - 1; nFound++) {
                strcpy(pProfile->choices.a82[nFound], pProfile->choices.a82[nFound + 1]);
            }
            pProfile->choices.n81--;
        }
        break;
    }
}

u8 fn_800587A8(SaveProfile* pProfile, int nKind, char* pName) {
    int i;

    switch (nKind) {
    case 0:
        for (i = 0; i < pProfile->choices.n0; i++) {
            if (strcmp(pProfile->choices.a1[i], pName) == 0) {
                return 1;
            }
        }
        break;
    case 1:
        for (i = 0; i < pProfile->choices.n81; i++) {
            if (strcmp(pProfile->choices.a82[i], pName) == 0) {
                return 1;
            }
        }
        break;
    case 2:
        // the list's one name, compared once per entry counted
        for (i = 0; i < pProfile->choices.n102; i++) {
            if (strcmp(pProfile->choices.sz103, pName) == 0) {
                return 1;
            }
        }
        break;
    }
    return 0;
}

void fn_800588D4(s16 n) {
    lbl_80281DF0.b = 1;
    lbl_80281DF0.n = n;
}

int fn_800588E8(void) {
    return lbl_80281DF0.n;
}

// Marked holes 0..70 are kept in a5004/a504C, 71..74 in a10578/a1057C.
int fn_800588F4(SaveProfile* pProfile, int nKind, int i) {
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

void fn_8005897C(SaveProfile* pProfile, int nKind, int i, int nValue) {
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

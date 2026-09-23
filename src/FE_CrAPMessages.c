// FE_CrAPMessages.c (our name, after EA's FE_PGATourMessages.c): the Create-A-Player screens'
// message handlers, registered in the front end's message table (FE_MessageTable.c). They read
// and set the golfer being created through the Create-A-Player database (FE_CrAPDB.c), the logo
// editor (FE_LogoDesign.c) and the menu golfer (FEgolferanim.c).

#include "engine.h"
#include "camera.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "charstate.h"
#include "game.h"

// FE_Manager.c
void fn_8007873C(SaveProfile* pProfile);
void fn_80078E34(SaveProfile* pProfile);
void fn_80079664(SaveProfile* pProfile);
void fn_8007975C(SaveProfile* pProfile, s16 nPart, int nChance);
int  fn_800797E0(SaveProfile* pProfile, s16 nPart, int b, int nChance);

void fn_80103B74(u8 b);                 // FE_CrAPDB.c: set the database's b14
void fn_801073DC(s16 nSlot);            // FE_CrAPDB.c
void fn_800A73F0(s32 n);
u8   fn_800D256C(u32 nYear);            // Calendar.c: a leap year (1900 counts as one)

// ---- sweep code (not yet cleaned up) ----

void fn_801077EC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801048EC(pArgs[0].i, pArgs[1].i);
}

// ---- end of sweep code ----

// Value n of a part's choice i (0 when there is no such choice).
void fn_80107828(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = fn_801048EC(nPart, b);
    int i = pArgs[2].i;
    int n = pArgs[3].i;

    if (i < 0 || i >= nChoices) {
        pResult->i = 0;
    } else {
        pResult->i = fn_80105644(nPart, b, i, n);
    }
}

// Colour n of a part's choice i, as four components (opaque red when there is no such choice).
void fn_801078B8(MsgArg* pArgs, MsgArg* pResult) {
    u8 aColor[4];
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = fn_801048EC(nPart, b);
    int i = pArgs[2].i;
    int n = pArgs[3].i;

    if (i < 0 || i >= nChoices || nChoices == 0) {
        *(s32*)pArgs[4].p = 0xFF;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
        *(s32*)pArgs[7].p = 0xFF;
    } else {
        fn_8010568C(nPart, b, i, n, aColor);
        *(s32*)pArgs[4].p = aColor[0];
        *(s32*)pArgs[5].p = aColor[1];
        *(s32*)pArgs[6].p = aColor[2];
        *(s32*)pArgs[7].p = aColor[3];
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80107994(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80107998(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80107BA4(MsgArg* pArgs, MsgArg* pResult) {
}

// ---- end of sweep code ----

// Slider n of the created golfer (the profile's a5EB4, read signed).
void fn_80107BA8(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    pResult->i = (s8)pProfile->a5EB4[pArgs[0].i];
}

// ---- sweep code (not yet cleaned up) ----

void fn_80107BF4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80105C44(pArgs[0].i, pArgs[1].i);
}

void fn_80107C30(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// ---- end of sweep code ----

// A part's choice i: its name and four values ("Coming Soon" in a demo session, "No Entry Found"
// otherwise, when there is no such choice).
void fn_80107C3C(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = fn_801048EC(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        if (gSession.uFlags & 0x4000) {
            strcpy(((MsgString*)pArgs[3].p)->pStr, "Coming Soon");
        } else {
            strcpy(((MsgString*)pArgs[3].p)->pStr, "No Entry Found");
        }
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
        *(s32*)pArgs[7].p = 0;
        *(s32*)pArgs[8].p = 0;
    } else {
        strcpy(((MsgString*)pArgs[3].p)->pStr, fn_80105264(nPart, b, i));
        *(s32*)pArgs[4].p = fn_80105298(nPart, b, i);
        *(s32*)pArgs[5].p = fn_801052CC(nPart, b, i);
        *(s32*)pArgs[6].p = fn_80105300(nPart, b, i);
        *(s32*)pArgs[7].p = fn_80105334(nPart, b, i);
        *(s32*)pArgs[8].p = fn_801053D0(nPart, b, i);
    }
}

// Four more values of a part's choice i (0 when there is no such choice).
void fn_80107DBC(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = fn_801048EC(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        *(s32*)pArgs[3].p = 0;
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
    } else {
        *(s32*)pArgs[3].p = fn_80105404(nPart, b, i);
        *(s32*)pArgs[4].p = fn_80105428(nPart, b, i);
        *(s32*)pArgs[5].p = fn_8010544C(nPart, b, i);
        *(s32*)pArgs[6].p = fn_80105470(nPart, b, i);
    }
}

// Four more values of a part's choice i (0 when there is no such choice).
void fn_80107EB0(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = fn_801048EC(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        *(s32*)pArgs[3].p = 0;
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
    } else {
        *(s32*)pArgs[3].p = fn_80105368(nPart, b, i);
        *(s32*)pArgs[4].p = fn_8010539C(nPart, b, i);
        *(s32*)pArgs[5].p = fn_80105574(nPart, b, i);
        *(s32*)pArgs[6].p = fn_801055A8(nPart, b, i);
    }
}

// Put a part's choice i on the created golfer, and save the profile. Part 17 is always turned on
// directly; for the others fn_80103F94 does it when fn_80103C98 and fn_801074D4 allow.
void fn_80107FAC(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    int nAsset;

    if (nPart == 17) {
        FE_CrAP_TurnOnPart(nPart, b, i);
        return;
    }
    nAsset = fn_80104FA8(nPart, b, i);
    if (nAsset != -1) {
        if (fn_80103C98(fn_80104F68(nAsset)) && fn_801074D4(nAsset)) {
            fn_80103F94(nPart, b, i);
        } else {
            FE_CrAP_TurnOnPart(nPart, b, i);
        }
        fn_8007873C(pProfile);
    }
}

// fn_80107FAC without saving the profile.
void fn_80108070(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart;
    int b;
    int i;
    int nAsset;

    fn_80077ACC();
    nPart = pArgs[0].i;
    b = pArgs[1].i;
    i = pArgs[2].i;
    if (nPart == 17) {
        FE_CrAP_TurnOnPart(nPart, b, i);
        return;
    }
    nAsset = fn_80104FA8(nPart, b, i);
    if (nAsset != -1) {
        if (fn_80103C98(fn_80104F68(nAsset)) && fn_801074D4(nAsset)) {
            fn_80103F94(nPart, b, i);
            return;
        }
        FE_CrAP_TurnOnPart(nPart, b, i);
    }
}

void fn_80108140(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801049C8(pArgs[0].i);
}

void fn_80108300(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->bCopy = pArgs[0].i;
}

// ---- end of sweep code ----

// A part's choice i is locked (never in a demo session).
void fn_80108314(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;

    if (gSession.uFlags & 0x4000) {
        pResult->i = 0;
    } else {
        pResult->i = fn_80078008(fn_80104FA8(nPart, b, i), pProfile);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108398(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = fn_8001E9CC(pProfile->aB1CC, nAsset);
}

// ---- end of sweep code ----

// Buy a part's choice i: take its price from the money, mark it bought (aB1CC), put it on the
// created golfer and save the profile.
void fn_8010840C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    s32 nPrice = pArgs[3].i;
    int nAsset = fn_80104FA8(nPart, b, i);

    pProfile->n6C -= nPrice;
    fn_8001EA34(pProfile->aB1CC, nAsset);
    FE_CrAP_TurnOnPart(nPart, b, i);
    fn_8007873C(pProfile);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108494(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077ACC();
    if (fn_80106244(pArgs[0].i) >= 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// ---- end of sweep code ----

// A part's choice i is in the profile's list b: for part 13 by its name, else by fn_80106374.
void fn_801084F4(MsgArg* pArgs, MsgArg* pResult) {
    char szName[16];                    // the size is unknown (the frame allows up to 20)
    SaveProfile* pProfile = fn_80077ACC();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;

    fn_80104AF4(nPart, b);
    if (nPart == 13) {
        fn_80105B4C(nPart, b, i, szName);
        pResult->i = fn_800587A8(pProfile, b, szName);
    } else {
        pResult->i = fn_80106374(nPart, b, i);
    }
}

// Today's random asset kind of category pArgs[0] (-1, -2, -3) for the database's n4, and the
// choices of its five random assets (fn_80077C1C).
void fn_80108594(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = fn_80103BB4();

    *(s32*)pArgs[1].p = lbl_80281ED4->aKind[b][nCategory];
    *(s32*)pArgs[2].p = lbl_80281ED4->aChoice[b][nCategory][0];
    *(s32*)pArgs[3].p = lbl_80281ED4->aChoice[b][nCategory][1];
    *(s32*)pArgs[4].p = lbl_80281ED4->aChoice[b][nCategory][2];
    *(s32*)pArgs[5].p = lbl_80281ED4->aChoice[b][nCategory][3];
    *(s32*)pArgs[6].p = lbl_80281ED4->aChoice[b][nCategory][4];
}

// The parts of the five random assets of category pArgs[0].
void fn_80108690(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = fn_80103BB4();

    *(s32*)pArgs[1].p = lbl_80281ED4->aPart[b][nCategory][0];
    *(s32*)pArgs[2].p = lbl_80281ED4->aPart[b][nCategory][1];
    *(s32*)pArgs[3].p = lbl_80281ED4->aPart[b][nCategory][2];
    *(s32*)pArgs[4].p = lbl_80281ED4->aPart[b][nCategory][3];
    *(s32*)pArgs[5].p = lbl_80281ED4->aPart[b][nCategory][4];
}

// ---- sweep code (not yet cleaned up) ----

void fn_801088A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7C0(pArgs[0].i);
}

void fn_801088C8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        fn_8010F7E4(LOGO_SQUARE);
        return;
    }
    fn_8010F7E4(LOGO_RECT);
}

void fn_80108904(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7FC(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p, pArgs[4].p);
}

void fn_801089BC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F880();
}

void fn_801089DC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F90C(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// ---- end of sweep code ----

// Check the assets' locks again; count the assets that were locked and are now unlocked and
// offered, and set their aB344 bits.
void fn_80108A0C(MsgArg* pArgs, MsgArg* pResult) {
    u32 aWasLocked[94];                 // the size is unknown (the frame allows up to 97 words)
    int nUnlocked = 0;
    SaveProfile* pProfile = fn_80077ACC();
    s32 nAssets = fn_80105C00();
    int i;

    fn_8001E938(aWasLocked, 3000);
    for (i = 0; i < nAssets; i++) {
        if (fn_8001E9CC(pProfile->aAssetLocked, i)) {
            fn_8001EA34(aWasLocked, i);
        } else {
            fn_8001EB6C(aWasLocked, i);
        }
    }
    fn_80078680(pProfile);
    for (i = 0; i < nAssets; i++) {
        if (!fn_8001E9CC(pProfile->aAssetLocked, i) && fn_8001E9CC(aWasLocked, i)) {
            if (fn_801061C8(fn_80103BC0(i))) {
                nUnlocked++;
                fn_8001EA34(pProfile->aB344, i);
            }
        }
    }
    pResult->i = nUnlocked;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108B10(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = fn_8001E9CC(pProfile->aB344, nAsset);
}

void fn_80108B84(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nAsset = fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    if (fn_8001E9CC(pProfile->aB344, nAsset)) {
        fn_8001EA34(pProfile->aB4BC, nAsset);
    }
}

// ---- end of sweep code ----

// Clear every asset's aB344 and aB4BC bits where both are set.
void fn_80108C00(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 nAssets = fn_80105C00();
    u32 i;

    for (i = 0; i < nAssets; i++) {
        if (fn_8001E9CC(pProfile->aB344, i) && fn_8001E9CC(pProfile->aB4BC, i)) {
            fn_8001EB6C(pProfile->aB344, i);
            fn_8001EB6C(pProfile->aB4BC, i);
        }
    }
}

// Whether a controller in any of the four ports holds button bit 24 (held).
void fn_80108CA8(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 4; i++) {
        if (fn_80013070(i) && (fn_800136DC(i) & 0x1000000)) {
            pResult->i = 1;
            return;
        }
    }
    pResult->i = 0;
}

// Four buttons of controller pArgs[0] (bits 19, 18, 16 and 17 of what it holds), when one is
// plugged in.
void fn_80108D1C(MsgArg* pArgs, MsgArg* pResult) {
    int nChan = pArgs[0].i;
    s32* pA = pArgs[1].p;
    s32* pB = pArgs[2].p;
    s32* pC = pArgs[3].p;
    s32* pD = pArgs[4].p;

    if (fn_80013070(nChan)) {
        if (fn_800136DC(nChan) & 0x80000) {
            *pA = 1;
        } else {
            *pA = 0;
        }
        if (fn_800136DC(nChan) & 0x40000) {
            *pB = 1;
        } else {
            *pB = 0;
        }
        if (fn_800136DC(nChan) & 0x10000) {
            *pC = 1;
        } else {
            *pC = 0;
        }
        if (fn_800136DC(nChan) & 0x20000) {
            *pD = 1;
        } else {
            *pD = 0;
        }
    }
}

// Set or clear bit pArgs[0] of the profile's a10548.
void fn_80108DF4(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 bSet = pArgs[1].i;
    s32 nBit = pArgs[0].i;

    if (bSet) {
        fn_8001EA34(pProfile->a10548, nBit);
    } else {
        fn_8001EB6C(pProfile->a10548, nBit);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108E4C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    pResult->i = fn_8001E9CC(pProfile->a10548, pArgs[0].i);
}

// ---- end of sweep code ----

// Today's date: month, day and year.
void fn_801090B4(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    s32 nMsec;

    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
    *(s32*)pArgs[0].p = nMonth;
    *(s32*)pArgs[1].p = nDay;
    *(s32*)pArgs[2].p = nYear;
}

// A random whole number 1..99, as a float.
void fn_80109120(MsgArg* pArgs, MsgArg* pResult) {
    u32 n = 0;

    while (n == 0) {
        n = (u32)(100.0f * fn_8000B318(0)) % 100;
    }
    pResult->f = n;
}

// The logo's name.
void fn_801091B8(MsgArg* pArgs, MsgArg* pResult) {
    LogoRecord* pLogo = fn_8010FB70();
    strcpy(((MsgString*)pArgs[1].p)->pStr, pLogo->szName);
    pResult->i = pLogo->b1020;
}

// Name the logo.
void fn_8010920C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(fn_8010FB70()->szName, ((MsgString*)pArgs[1].p)->pStr);
}

// The profile's logo n's b1020.
void fn_80109248(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    pResult->i = pProfile->aLogos[pArgs[0].i].b1020;
}

// Keep the edited logo: copy it into the profile's logo fn_8010F7D8.
void fn_80109294(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 nLogo = fn_8010F7D8();

    lbl_80281ED4->logo106E0.b1020 = 1;
    Mem_cpy(&pProfile->aLogos[nLogo], &lbl_80281ED4->logo106E0, sizeof(LogoRecord));
    lbl_80281ED4->b10640 = 0;
}

// Golfer pArgs[0]'s equipment tier for attribute pArgs[1].
void fn_80109304(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord = fn_80077A80(pArgs[0].i);
    pResult->i = pRecord->tier[pArgs[1].i];
}

void fn_80109354(MsgArg* pArgs, MsgArg* pResult) {
    fn_80104DB8(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr);
}

void fn_80109388(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010645C(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// ---- end of sweep code ----

// A part's choice i: its unlock text (fn_8010651C), when there is such a choice.
void fn_801093B4(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = fn_801048EC(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        return;
    }
    fn_8010651C(nPart, b, i, ((MsgString*)pArgs[3].p)->pStr);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80109430(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109434(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E364(pArgs[0].i);
}

void fn_80109458(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)fn_8008E420();
}

void fn_8010948C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)fn_8008E44C();
}

void fn_801094C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008B990() == 1;
}

void fn_801094FC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109500(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->bCopy;
}

// Start (pArgs[0] set) or stop editing the profile's logo fn_8010F7D8: a logo not made yet
// starts blank (colour 0x1C), named "MyLogo <n>".
void fn_80109514(MsgArg* pArgs, MsgArg* pResult) {
    char szName[32];                    // the size is unknown (the frame allows up to 0x20)
    SaveProfile* pProfile = fn_80077ACC();
    s32 bStart = pArgs[0].i;
    s32 nLogo = fn_8010F7D8();

    if (bStart == 0) {
        lbl_80281ED4->b10640 = 0;
        return;
    }
    if (pProfile->aLogos[nLogo].b1020) {
        Mem_cpy(&lbl_80281ED4->logo106E0, &pProfile->aLogos[nLogo], sizeof(LogoRecord));
    } else {
        sprintf(szName, "MyLogo %d", nLogo + 1);
        strcpy(lbl_80281ED4->logo106E0.szName, szName);
        lbl_80281ED4->logo106E0.b1020 = 0;
        lbl_80281ED4->logo106E0.nShape = 0;
        memset(lbl_80281ED4->logo106E0.aPixels, 0x1C, sizeof(lbl_80281ED4->logo106E0.aPixels));
    }
    lbl_80281ED4->b10640 = 1;
}

// A palette colour's components.
void fn_80109618(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7FC(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p, pArgs[4].p);
}

// A pixel's colour index and components.
void fn_80109650(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8010FBCC(pArgs[0].i, pArgs[1].i, pArgs[2].p, pArgs[3].p, pArgs[4].p, pArgs[5].p);
}

// Set the logo's shape.
void fn_8010969C(MsgArg* pArgs, MsgArg* pResult) {
    LogoRecord* pLogo = fn_8010FB70();
    s32 nShape = pArgs[1].i;
    switch (nShape) {
    case LOGO_SQUARE:
        pLogo->nShape = LOGO_SQUARE;
        break;
    case LOGO_RECT:
        pLogo->nShape = LOGO_RECT;
        break;
    }
    fn_8010F7E4(nShape);
}

// The logo's shape.
void fn_80109700(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8010FB70()->nShape;
}

void fn_80109734(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109738(MsgArg* pArgs, MsgArg* pResult) {
    fn_80103B74(pArgs[0].i);
}

void fn_80109760(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E824();
}

// fn_8008E860 for part 12's entry n: 1 for entries 0..4, 2 for entry 6, else 0.
void fn_80109780(MsgArg* pArgs, MsgArg* pResult) {
    s32 nPart = pArgs[0].i;
    s32 n = pArgs[1].i;

    if (nPart == 12) {
        switch (n) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            fn_8008E860(1);
            break;
        case 6:
            fn_8008E860(2);
            break;
        default:
            fn_8008E860(0);
            break;
        }
    } else {
        fn_8008E860(0);
    }
}

void fn_801097FC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        fn_8008E6D4(0);
        return;
    case 1:
        fn_8008E6D4(1);
        return;
    case 2:
        fn_8008E6D4(4);
        return;
    case 3:
        fn_8008E6D4(5);
        return;
    case 4:
        fn_8008E6D4(2);
        return;
    default:
        fn_8008E6D4(0);
        return;
    }
}

void fn_8010988C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008DAEC();
}

void fn_801098AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109BA4(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    fn_80103B74(0);
    fn_801073DC(2);
    fn_801073DC(5);
    fn_801073DC(6);
    fn_801073DC(7);
    fn_801073DC(8);
    fn_801073DC(11);
    fn_801073DC(12);
    fn_801073DC(13);
    fn_801073DC(14);
    fn_8007975C(pProfile, 0, 80);
    fn_8007975C(pProfile, 1, 0);
    fn_8007975C(pProfile, 2, 0);
    fn_8007975C(pProfile, 7, 0);
    fn_8007975C(pProfile, 19, 70);
    fn_8007975C(pProfile, 20, 70);
    fn_8007975C(pProfile, 8, 70);
    fn_800797E0(pProfile, 7, (Rand_Next(0) & 1) + 1, 0);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Idle" shot.
void fn_80109CBC(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    View* pView = fn_80017028(fn_80016D10());
    fn_800A73F0((Rand_Next(0) & 7) + 11);
    fn_8008E244();
    fn_8008E364(0);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Idle", lbl_80281EE0->n4, 0, 0, 0);
    fn_80103B74(0);
    fn_80079664(pProfile);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Face" shot.
void fn_80109D5C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    View* pView = fn_80017028(fn_80016D10());
    fn_8008E244();
    fn_8008E364(1);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Face", lbl_80281EE0->n4, 0, 0, 0);
    fn_80103B74(0);
    fn_80078E34(pProfile);
}

void fn_80109DDC(MsgArg* pArgs, MsgArg* pResult) {
}

// ---- sweep code (not yet cleaned up) ----

void fn_80109EAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_801072CC(pArgs[0].i, pArgs[3].p, pArgs[2].p, pArgs[4].p, pArgs[1].p);
}

void fn_8010A208(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E724(NULL, NULL, 0, 0);
}

void fn_8010A35C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80104F68(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i))->n2E;
}

void fn_8010A3A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80104804();
}

void fn_8010A3C4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8010A3C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800D256C(pArgs[0].i);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8010A400(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801074D4(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i));
}

// ---- end of sweep code ----

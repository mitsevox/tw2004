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

void fn_80103B74(u8 b);                 // FE_CrAPDB.c: set the database's b14
void fn_801073DC(s16 nSlot);            // FE_CrAPDB.c
void fn_800A73F0(s32 n);
u8   IsLeapYear(u32 nYear);            // Calendar.c: a leap year (1900 counts as one)

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

// ---- end of sweep code ----

// Set slider pArgs[0] of the created golfer to pArgs[1]; the menu golfer, when there is one, takes
// the new sliders and part 18 is turned on with the slider's number.
void fn_8010799C(MsgArg* pArgs, MsgArg* pResult) {
    SkinChoices* pChoices = &fn_80077ACC()->choices;
    int n = pArgs[0].i;
    Character* pChar;

    pChoices->a9B4[n] = pArgs[1].i;
    if (lbl_80281EE0->pB4 == NULL) {
        return;
    }
    pChar = lbl_80281EE0->pB4->pChar;
    if (pChar == NULL) {
        return;
    }
    fn_8010E4DC(pChar->p17AC, pChar->pModel, pChar->pSkin, 26, pChoices->a9B4,
                &pChar->node3E0);
    FE_CrAP_TurnOnPart(18, 0, n);
}

// The place, in its part's list, of the asset in the profile's slot for part pArgs[0] (part 12:
// for its entry pArgs[1]); 0 when the part has no slot or the slot is empty.
void fn_80107A2C(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    s32 nEntry = pArgs[1].i;
    s32 n;
    s16 nSlot;

    switch (nPart) {
    case 3:
        nSlot = 16;
        break;
    case 4:
        nSlot = 19;
        break;
    case 5:
        nSlot = 18;
        break;
    case 6:
        nSlot = 17;
        break;
    case 9:
        nSlot = 24;
        break;
    case 10:
        nSlot = 25;
        break;
    case 14:
        nSlot = 22;
        break;
    case 15:
        nSlot = 23;
        break;
    case 16:
        nSlot = 20;
        break;
    case 21:
        nSlot = 21;
        break;
    case 11:
        nSlot = 26;
        break;
    case 22:
        nSlot = 15;
        break;
    default:
        nSlot = -1;
        break;
    }
    if (nPart == 12) {
        switch (nEntry) {
        case 0:
            nSlot = 44;
            break;
        case 1:
            nSlot = 45;
            break;
        case 2:
            nSlot = 46;
            break;
        case 3:
            nSlot = 47;
            break;
        case 4:
            nSlot = 48;
            break;
        case 5:
            nSlot = 49;
            break;
        case 6:
            nSlot = 51;
            break;
        case 7:
            nSlot = 50;
            break;
        }
    }
    if (nSlot == -1) {
        pResult->i = 0;
    } else {
        n = fn_80103D14(nSlot);
        if (n < 0) {
            n = 0;
        } else {
            fn_80105FF8(n, &nPart, &nEntry, &n);
        }
        pResult->i = n;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80107BA4(MsgArg* pArgs, MsgArg* pResult) {
}

// ---- end of sweep code ----

// Slider n of the created golfer (the profile's choices.a9B4, read signed).
void fn_80107BA8(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    pResult->i = (s8)pProfile->choices.a9B4[pArgs[0].i];
}

// ---- sweep code (not yet cleaned up) ----

void fn_80107BF4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80105C44(pArgs[0].i, pArgs[1].i);
}

void fn_80107C30(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// ---- end of sweep code ----

// A part's choice i: its name and five values ("Coming Soon" in the session's 0x4000 mode,
// "No Entry Found" otherwise, when there is no such choice).
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

// Put a part's choice i on the created golfer and recompute its equipment tiers (fn_8007873C).
// Part 17 is only turned on directly; for the others fn_80103F94 does it when fn_80103C98 and
// fn_801074D4 allow.
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

// fn_80107FAC without recomputing the equipment tiers.
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

// ---- end of sweep code ----

// Set one of the profile's settings: 0 its name (pArgs[1]), 1 choices.n5A7A, 2 the date (packed as
// fn_80078604 packs it), 3 choices.n113 (pArgs[2]).
void fn_80108178(MsgArg* pArgs, MsgArg* pResult) {
    int nMonth;
    int nDay;
    int nYear;
    SaveProfile* pProfile = fn_80077ACC();

    switch (pArgs[0].i) {
    case 0:
        fn_80057ED0(pProfile, ((MsgString*)pArgs[1].p)->pStr);
        break;
    case 1:
        pProfile->choices.n5A7A = pArgs[2].i;
        fn_80103B8C(pProfile->choices.n5A7A);
        break;
    case 2:
        fn_80078620(pArgs[2].i, &nMonth, &nDay, &nYear);
        pProfile->nDateDay = nDay;
        pProfile->nDateMonth = nMonth;
        pProfile->nDateYear = nYear;
        break;
    case 3:
        pProfile->choices.n113 = pArgs[2].i;
        break;
    }
}

// Read one of the profile's settings (see fn_80108178).
void fn_80108244(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();

    switch (pArgs[0].i) {
    case 0:
        strcpy(((MsgString*)pArgs[1].p)->pStr, pProfile->szName);
        break;
    case 1:
        *(s32*)pArgs[2].p = (s8)pProfile->choices.n5A7A;
        break;
    case 2:
        *(s32*)pArgs[2].p = fn_80078604(pProfile->nDateMonth, pProfile->nDateDay, pProfile->nDateYear);
        break;
    case 3:
        *(s32*)pArgs[2].p = pProfile->choices.n113;
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108300(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->bCopy = pArgs[0].i;
}

// ---- end of sweep code ----

// A part's choice i is locked (never in the session's 0x4000 mode).
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

// Buy a part's choice i: take the price pArgs[3] from the money, mark it bought (aB1CC), put it on
// the created golfer and recompute its equipment tiers (fn_8007873C).
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

// Part 13: choice i's name is in the profile's list b (fn_800587A8); other parts: choice i is the
// asset in its slot of the profile (fn_80106374).
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

// The entries (aPart, which fn_80104FA8 takes as b) of the five random assets of category pArgs[0].
void fn_80108690(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = fn_80103BB4();

    *(s32*)pArgs[1].p = lbl_80281ED4->aPart[b][nCategory][0];
    *(s32*)pArgs[2].p = lbl_80281ED4->aPart[b][nCategory][1];
    *(s32*)pArgs[3].p = lbl_80281ED4->aPart[b][nCategory][2];
    *(s32*)pArgs[4].p = lbl_80281ED4->aPart[b][nCategory][3];
    *(s32*)pArgs[5].p = lbl_80281ED4->aPart[b][nCategory][4];
}

// A part's choice i is one of today's random assets: parts 0, 1, 2 and 7 are category -1, parts
// 8, 19 and 20 category -2, part 12 category -3 (other parts have none).
void fn_80108768(MsgArg* pArgs, MsgArg* pResult) {
    int j;
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    s8 nDb = fn_80103BB4();
    int nAsset = fn_80104FA8(nPart, b, i);
    int nKind;
    int nCategory;

    switch (nPart) {
    case 0:
    case 1:
    case 2:
    case 7:
        nKind = -1;
        break;
    case 8:
    case 19:
    case 20:
        nKind = -2;
        break;
    case 12:
        nKind = -3;
        break;
    default:
        pResult->i = 0;
        return;
    }
    nCategory = fn_80077BDC(nKind);
    if (lbl_80281ED4->aKind[nDb][nCategory] != nPart) {
        pResult->i = 0;
        return;
    }
    for (j = 0; j < 5; j++) {
        if (nAsset == fn_80104FA8(lbl_80281ED4->aKind[nDb][nCategory], lbl_80281ED4->aPart[nDb][nCategory][j],
                                  lbl_80281ED4->aChoice[nDb][nCategory][j])) {
            pResult->i = 1;
            return;
        }
    }
    pResult->i = 0;
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

// ---- end of sweep code ----

// Load the logo from the texture named pArgs[0].
void fn_8010893C(MsgArg* pArgs, MsgArg* pResult) {
    char szName[32] = "";

    strcpy(szName, ((MsgString*)pArgs[0].p)->pStr);
    fn_8010F890(szName);
}

// ---- sweep code (not yet cleaned up) ----

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

// The newly unlocked assets (aB344) for the five strings pArgs[0..4]: their names when there are
// at most five, else their categories (the fifth line "And more..." when there are more than five).
void fn_80108E9C(MsgArg* pArgs, MsgArg* pResult) {
    char aNames[5][64];
    char aCategories[5][64];
    char szCategory[64];
    SaveProfile* pProfile = fn_80077ACC();
    s32 nAssets = fn_80105C00();
    int nNames = 0;
    int nCategories = 0;
    int i;
    int j;
    u8 bFound;

    for (i = 0; i < 5; i++) {
        strcpy(aNames[i], " ");
        strcpy(aCategories[i], " ");
    }
    for (i = 0; i < nAssets; i++) {
        if (fn_8001E9CC(pProfile->aB344, i) && fn_801061C8(fn_80103BC0(i))) {
            fn_8010745C(i, szCategory);
            if (nCategories < 5) {
                bFound = 0;
                for (j = 0; j < nCategories; j++) {
                    if (strcmp(szCategory, aCategories[j]) == 0) {
                        bFound = 1;
                    }
                }
                if (!bFound) {
                    strcpy(aCategories[nCategories], szCategory);
                    nCategories++;
                }
            } else if (nCategories == 5) {
                bFound = 0;
                for (j = 0; j < nCategories; j++) {
                    if (strcmp(szCategory, aCategories[j]) == 0) {
                        bFound = 1;
                    }
                }
                if (!bFound) {
                    strcpy(aCategories[4], "And more...");
                    nCategories++;
                }
            }
            if (nNames < 5) {
                fn_8010749C(i, aNames[nNames]);
            }
            nNames++;
        }
    }
    if (nNames <= 5) {
        for (i = 0; i < 5; i++) {
            strcpy(((MsgString*)pArgs[i].p)->pStr, aNames[i]);
        }
    } else {
        for (i = 0; i < 5; i++) {
            strcpy(((MsgString*)pArgs[i].p)->pStr, aCategories[i]);
        }
    }
}

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
        n = (u32)(100.0f * Misc_RandFuncg(0)) % 100;
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
    pResult->i = pProfile->choices.aLogo[pArgs[0].i].b1020;
}

// Keep the edited logo: copy it into the profile's logo fn_8010F7D8.
void fn_80109294(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s32 nLogo = fn_8010F7D8();

    lbl_80281ED4->logo106E0.b1020 = 1;
    Mem_cpy(&pProfile->choices.aLogo[nLogo], &lbl_80281ED4->logo106E0, sizeof(LogoRecord));
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
    if (pProfile->choices.aLogo[nLogo].b1020) {
        Mem_cpy(&lbl_80281ED4->logo106E0, &pProfile->choices.aLogo[nLogo], sizeof(LogoRecord));
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

// Make a random created golfer: take the assets in slots 2, 5..8 and 11..14 off, parts 4, 5 and 6
// at their first choice, random choices for parts 0 to 8, 14, 16, 19 and 20 (no pick with a chance
// of 80% for 0 and 4, 90% for 5 and 6, 70% for 8, 19 and 20), parts 15, 9 and 10 as fn_80109FB4
// does, part 7 at b 1 or 2, then its equipment tiers again (fn_8007873C).
void fn_801098B0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int nRoll = Misc_RandFunc(0) % 100;
    int nChoice;

    Misc_RandFunc(0);                       // drawn, not used
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
    FE_CrAP_TurnOnPart(5, 0, 0);
    FE_CrAP_TurnOnPart(6, 0, 0);
    FE_CrAP_TurnOnPart(4, 0, 0);
    fn_8007975C(pProfile, 0, 80);
    fn_8007975C(pProfile, 3, 0);
    fn_8007975C(pProfile, 4, 80);
    fn_8007975C(pProfile, 15, 0);
    fn_8007975C(pProfile, 5, 90);
    fn_8007975C(pProfile, 6, 90);
    fn_8007975C(pProfile, 1, 0);
    fn_8007975C(pProfile, 2, 0);
    fn_8007975C(pProfile, 7, 0);
    fn_8007975C(pProfile, 16, 0);
    fn_8007975C(pProfile, 19, 70);
    fn_8007975C(pProfile, 20, 70);
    fn_8007975C(pProfile, 8, 70);
    nChoice = fn_8007975C(pProfile, 14, 0);
    if (Misc_RandFunc(0) % 100 < 95) {
        FE_CrAP_TurnOnPart(15, 0, nChoice);
    } else {
        fn_8007975C(pProfile, 15, 0);
    }
    if (nRoll < 75) {
        FE_CrAP_TurnOnPart(9, 0, Misc_RandFunc(0) % 3);
    } else if (nRoll >= 75 && nRoll < 85) {
        Misc_RandFunc(0);                   // drawn, not used
        FE_CrAP_TurnOnPart(9, 0, 6);
    } else if (nRoll >= 85 && nRoll < 95) {
        Misc_RandFunc(0);                   // drawn, not used
        FE_CrAP_TurnOnPart(9, 0, 4);
    } else {
        Misc_RandFunc(0);                   // drawn, not used
        FE_CrAP_TurnOnPart(9, 0, 8);
    }
    fn_80078A2C(10, 95);
    fn_800797E0(pProfile, 7, (Misc_RandFunc(0) & 1) + 1, 0);
    fn_8007873C(pProfile);
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
    fn_800797E0(pProfile, 7, (Misc_RandFunc(0) & 1) + 1, 0);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Idle" shot; then a random created golfer (fn_80079664) and its
// equipment tiers again (fn_8007873C).
void fn_80109CBC(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    View* pView = fn_80017028(fn_80016D10());
    fn_800A73F0((Misc_RandFunc(0) & 7) + 11);
    fn_8008E244();
    fn_8008E364(0);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Idle", lbl_80281EE0->n4, 0, 0, 0);
    fn_80103B74(0);
    fn_80079664(pProfile);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Face" shot; then fn_80078E34 dresses the created golfer at random.
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

// Sell a part's choice i back: a quarter of its price goes back into the money, and its bought
// bit (aB1CC) is cleared.
void fn_80109DE0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    int nAsset = fn_80104FA8(nPart, b, i);

    pProfile->n6C += (s32)(0.25f * fn_80105368(nPart, b, i));
    fn_8001EB6C(pProfile->aB1CC, nAsset);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80109EAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_801072CC(pArgs[0].i, pArgs[3].p, pArgs[2].p, pArgs[4].p, pArgs[1].p);
}

// ---- end of sweep code ----

// How many of the five random assets of category pArgs[0] have been bought (aB1CC).
void fn_80109EE8(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = fn_80077ACC();
    int i;
    s32* pnBought = pArgs[1].p;
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = fn_80103BB4();
    int nAsset;

    *pnBought = 0;
    for (i = 0; i < 5; i++) {
        nAsset = fn_80104FA8(lbl_80281ED4->aKind[b][nCategory], lbl_80281ED4->aPart[b][nCategory][i],
                             lbl_80281ED4->aChoice[b][nCategory][i]);
        if (fn_8001E9CC(pProfile->aB1CC, nAsset)) {
            (*pnBought)++;
        }
    }
}

// Dress the created golfer at random: parts 4, 5, 6 and 22 at their first choice, then random
// choices (with the chance in percent of the first one); part 15 follows part 14's choice 95% of
// the time, and part 9 is one of choices 0..2 (75%), 6 (10%), 4 (10%) or 8 (5%).
void fn_80109FB4(MsgArg* pArgs, MsgArg* pResult) {
    int nRoll = Misc_RandFunc(0) % 100;
    int nChoice;
    SaveProfile* pProfile;

    Misc_RandFunc(0);                       // drawn, not used
    pProfile = fn_80077ACC();
    fn_80103B74(0);
    FE_CrAP_TurnOnPart(5, 0, 0);
    FE_CrAP_TurnOnPart(6, 0, 0);
    FE_CrAP_TurnOnPart(4, 0, 0);
    FE_CrAP_TurnOnPart(22, 0, 0);
    fn_8007975C(pProfile, 3, 0);
    fn_8007975C(pProfile, 4, 80);
    fn_8007975C(pProfile, 15, 0);
    fn_8007975C(pProfile, 5, 90);
    fn_8007975C(pProfile, 6, 90);
    fn_8007975C(pProfile, 16, 0);
    fn_8007975C(pProfile, 22, 80);
    fn_8007975C(pProfile, 21, 0);
    fn_8007975C(pProfile, 11, 0);
    nChoice = fn_8007975C(pProfile, 14, 0);
    if (Misc_RandFunc(0) % 100 < 95) {
        FE_CrAP_TurnOnPart(15, 0, nChoice);
    } else {
        fn_8007975C(pProfile, 15, 0);
    }
    if (nRoll < 75) {
        FE_CrAP_TurnOnPart(9, 0, Misc_RandFunc(0) % 3);
    } else if (nRoll >= 75 && nRoll < 85) {
        Misc_RandFunc(0);                   // drawn, not used
        FE_CrAP_TurnOnPart(9, 0, 6);
    } else if (nRoll >= 85 && nRoll < 95) {
        Misc_RandFunc(0);                   // drawn, not used
        FE_CrAP_TurnOnPart(9, 0, 4);
    } else {
        Misc_RandFunc(0);                   // drawn, not used
        FE_CrAP_TurnOnPart(9, 0, 8);
    }
    fn_80078A2C(10, 95);
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010A208(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008E724(NULL, NULL, 0, 0);
}

// ---- end of sweep code ----

// The place of the part's first slotted asset that fits its entry n, in the list of the part's
// offered assets (0 when there is none).
void fn_8010A238(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart;
    int n;
    s32 nResult = 0;
    s32 nPlace;
    int nAsset;

    nPart = pArgs[0].i;
    n = pArgs[1].i;
    nPlace = 0;
    if (nPart >= 0 && nPart < 24 && (nAsset = fn_801062C8(nPart, n)) != -1) {
        fn_801060F0(nAsset, nPart, n, &nPlace);
        nResult = nPlace;
    }
    pResult->i = nResult;
}

// Part 13's choice i has its animation in the shown golfer's library (always 1 for other parts,
// or with no golfer shown).
void fn_8010A2C8(MsgArg* pArgs, MsgArg* pResult) {
    char szName[64];                    // the size is unknown (the frame allows up to 0x40)
    u8 bFound = 1;
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;

    if (lbl_80281EE0->pB4 != NULL && nPart == 13) {
        fn_80105B4C(nPart, b, i, szName);
        if (AnimLib_FindByName(lbl_80281EE0->pB4->pChar->pLib, szName) == NULL) {
            bFound = 0;
        }
    }
    pResult->i = bFound;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010A35C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80104F68(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i))->n2E;
}

void fn_8010A3A4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80104804();
}

void fn_8010A3C4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8010A3C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = IsLeapYear(pArgs[0].i);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8010A400(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801074D4(fn_80104FA8(pArgs[0].i, pArgs[1].i, pArgs[2].i));
}

// ---- end of sweep code ----

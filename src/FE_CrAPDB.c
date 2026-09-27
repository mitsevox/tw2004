// FE_CrAPDB.c (EA's name, from its asserts; TW06): the Create-A-Player database, every asset a
// created golfer can wear or carry (hair, faces, shirts, hats, clubs...). The assets arrive in the
// 'CR_A' stream object and the names they use in 'CR_S'; each asset belongs to one of the
// CRAP_NUM_PARTS parts, has a category and a lock kind, and raises up to two attributes.

#include "game_types.h"
#include "charstate.h"
#include "endian.h"
#include "dynobj.h"
#include "game/modes/pgatour.h"
#include "frontend/fe.h"
#include "game/frontend.h"

s32  SkinPart_GetNumSets(Skin* pSkin);          // SkinPart.c: how many choices aSets[3] holds

// .data, in address order (0x80193228..)
SwapField lbl_80193228[20] = {
    { 4, 4 },
    { 36, 1 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 4, 4 },
    { 4, 4 },
    { 4, 4 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 2, 2 },
    { 6, 1 },
};
char lbl_801932C8[CRAP_NUM_PARTS][32] = {
    "All Headwear",
    "All Shirts",
    "All Pants/Shorts",
    "",
    "",
    "",
    "",
    "All Shoes",
    "All Eyewear",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "All Watches and Jewelry",
    "All Miscellaneous",
    "",
    "",
    "",
};
char lbl_801935C8[16][32] = {
    "adidas\xAE",
    "Callaway Golf\xAE",
    "Cleveland Golf\xAE",
    "EA SPORTS\xA6",
    "GENERIC",
    "Maxfli\xAE",
    "Nike",
    "Nike TW Collection",
    "None",
    "Odyssey Golf\xAE",
    "PING\xAE",
    "Precept\xAE",
    "Rossa\xAE",
    "TAG Heuer\xA6",
    "TaylorMade\xAE",
    "TourStage\xAE",
};
char lbl_801937C8[11][32] = {
    "ushirtlogof",
    "ushirtlogoh",
    "ushirtlogoa",
    "uhatlogof",
    "uhatlogoh",
    "uglovelogol",
    "uglovelogor",
    "uarmtattool",
    "uarmtattoor",
    "ulegtattool",
    "ulegtattoor",
};

// .sdata
s32 lbl_802816E8 = -1;
s32 lbl_802816EC = -1;

// .sbss, defined in reverse address order (CodeWarrior lays it out last-defined-first)
s32* lbl_80282480;
s32* lbl_8028247C;
s32* lbl_80282478;
s32* lbl_80282474;
CrAPRecord* lbl_80282470;
s32 lbl_8028246C;
UStreamObject* lbl_80282468;
UStreamObject* lbl_80282464;
CrAPDB* lbl_80282460;

// This file, in address order.
void FE_CrAP_ResetLastCategoryTables(void);
void FE_CrAP_TurnOffAsset(CrAPAsset* pAsset);
void sTurnOnLogo(s16 nPart, int b, int i);
void FE_CrAP_TurnOnAsset(CrAPAsset* pAsset);
u8   fn_801048B0(int nPart);
int  FE_CrAP_GetAssetIndexFromAsset(CrAPAsset* pAsset);
int  FE_CrAP_GetFirstCategoryAssetID(s16 nPart);
void fn_80105188(UStreamObject* pObject);
void fn_801051F4(UStreamObject* pObject);
void fn_80105240(void);
void fn_80105B80(CrAPAsset* pAsset, char* pName);
void fn_80105DAC(void);
void fn_80105EFC(void);
void FE_CheckSpecialCaseConnections(CrAPAsset* pAsset);
u8   FE_IsMatchingSubCategory(s16 nPart, int nCategory, int nWanted);
u8   fn_80106658(CrAPAsset* pAsset);
int  fn_80106750(CrAPAsset* pAsset, Skin** apSkins);
u8   FE_CrAP_TryBallSwappingAsset(CrAPAsset* pAsset);
void fn_80106A64(CrAPAsset* pAsset, Skin* pSkin);
void fn_80106B04(CrAPAsset* pAsset, Skin* pSkin);
void fn_80106BF8(CrAPAsset* pAsset, Skin* pSkin);
void fn_80106D24(CrAPAsset* pAsset, Skin* pSkin);
void fn_80106DA0(CrAPAsset* pAsset, Skin* pSkin);
int  fn_8010766C(MsgArg* pArg, char* sz);

// UISScreen.c's sender, with the front end's view of its arguments (as GameMessages.c declares it;
// uistudio.h has UIStudio* and const s32*, and game/frontend.h cannot be included with it).
void UISProcessHint(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);

// Allocate the database, empty, and its tables.
void FE_CrAP_InitModule(void) {
    lbl_80282460 = StaticMem_Alloc(sizeof(CrAPDB), 2, 0, "FE_CrAPDB.c", 211);
    lbl_80282460->pAssets = NULL;
    lbl_80282460->pStrings = NULL;
    lbl_80282460->nAssets = 0;
    lbl_80282460->uStringsSize = 0;
    lbl_80282460->n4 = 0;
    lbl_80282460->b14 = 1;
    lbl_80282480 = StaticMem_Alloc(CRAP_NUM_PARTS * sizeof(s32), 2, 0, "FE_CrAPDB.c", 219);
    lbl_8028247C = StaticMem_Alloc(CRAP_NUM_PARTS * 64 * sizeof(s32), 2, 0, "FE_CrAPDB.c", 220);
    lbl_80282478 = StaticMem_Alloc(CRAP_NUM_PARTS * 64 * sizeof(s32), 2, 0, "FE_CrAPDB.c", 221);
    lbl_80282474 = StaticMem_Alloc(CRAP_NUM_PARTS * sizeof(s32), 2, 0, "FE_CrAPDB.c", 222);
    lbl_80282470 = StaticMem_Alloc(64 * sizeof(CrAPRecord), 2, 0, "FE_CrAPDB.c", 224);
    lbl_8028246C = 0;
    FE_CrAP_ResetLastCategoryTables();
    lbl_80282464 = NULL;
    lbl_80282468 = NULL;
}

// Set every part's entries in the tables to -1 (none).
void FE_CrAP_ResetLastCategoryTables(void) {
    int nPart;
    s32 i;

    for (nPart = 0; nPart < CRAP_NUM_PARTS; nPart++) {
        lbl_80282480[nPart] = -1;
        // EA bug: a part's row is 24 entries long but 64 are cleared, into the next rows (the
        // tables hold 64 per part, so nothing past the end is touched)
        for (i = 0; i < 64; i++) {
            lbl_8028247C[nPart * CRAP_NUM_PARTS + i] = -1;
            lbl_80282478[nPart * CRAP_NUM_PARTS + i] = -1;
        }
    }
}

// Free the database: its stream objects, the database and its tables.
void FE_CrAP_CloseModule(void) {
    if (lbl_80282464 != NULL) {
        StaticMem_Free(lbl_80282464);
    }
    if (lbl_80282468 != NULL) {
        StaticMem_Free(lbl_80282468);
    }
    lbl_80282468 = NULL;
    lbl_80282464 = NULL;
    if (lbl_80282460 != NULL) {
        StaticMem_Free(lbl_80282460);
    }
    if (lbl_80282480 != NULL) {
        StaticMem_Free(lbl_80282480);
    }
    if (lbl_8028247C != NULL) {
        StaticMem_Free(lbl_8028247C);
    }
    if (lbl_80282478 != NULL) {
        StaticMem_Free(lbl_80282478);
    }
    if (lbl_80282474 != NULL) {
        StaticMem_Free(lbl_80282474);
    }
    if (lbl_80282470 != NULL) {
        StaticMem_Free(lbl_80282470);
    }
    lbl_80282480 = NULL;
    lbl_8028247C = NULL;
    lbl_80282478 = NULL;
    lbl_80282474 = NULL;
    lbl_80282470 = NULL;
    lbl_80282460 = NULL;
}

// The asset an asset takes its attributes from: for lock kind 28 the asset its nLock names,
// otherwise itself.
int sGetLinkedAssetID(int nAsset) {
    CrAPAsset* pAsset = &lbl_80282460->pAssets[nAsset];

    if (pAsset->nLockKind == 28) {
        return pAsset->nLock;
    }
    return nAsset;
}

// The asset an asset takes its attributes from.
CrAPAsset* sGetLinkedAsset(CrAPAsset* pAsset) {
    return FE_CrAP_GetAssetFromAssetIndex(sGetLinkedAssetID(FE_CrAP_GetAssetIndexFromAsset(pAsset)));
}

// Let the menu golfer's animation and camera calls (FEgolferanim.c) run (1) or do nothing (0);
// callers turn it off while putting on many parts at once.
void FE_CrAP_SetTriggerAnims(u8 b) {
    lbl_80282460->b14 = b;
}

u8 FE_CrAP_GetTriggerAnims(void) {
    return lbl_80282460->b14;
}

// Set the gender of the golfer being created, which picks the assets offered, and clear the cached
// list tables (FE_CrAP_ResetLastCategoryTables).
void FE_CrAP_SetCurrentGender(s8 n) {
    lbl_80282460->n4 = n;
    FE_CrAP_ResetLastCategoryTables();
}

s8 FE_CrAP_GetCurrentGender(void) {
    return lbl_80282460->n4;
}

// An asset's gender, as FE_IsValidCurrentGender tests it (2: suits either).
s8 FE_CrAP_GetAssetGender(int nAsset) {
    return lbl_80282460->pAssets[nAsset].n40;
}

// Empty the profile's slot of the asset.
void FE_CrAP_ClearEquippedAsset(CrAPAsset* pAsset) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nSlot = pAsset->n2E;

    if (nSlot >= 0 && nSlot < 53) {
        pProfile->aAF80[nSlot] = -1;
    }
}

// Put the asset in its slot of the profile.
void FE_CrAP_EquipAsset(CrAPAsset* pAsset) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nSlot = pAsset->n2E;

    if (nSlot >= 0 && nSlot < 53) {
        pProfile->aAF80[nSlot] = FE_CrAP_GetAssetIndexFromAsset(pAsset);
    }
}

// The asset (the one it takes its attributes from) is the one in its slot of the profile.
u8 FE_CrAP_IsAssetEquipped(CrAPAsset* pAsset) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    CrAPAsset* pBase = sGetLinkedAsset(pAsset);
    s16 nSlot = pBase->n2E;

    if (nSlot >= 0 && nSlot < 53) {
        return pProfile->aAF80[nSlot] == FE_CrAP_GetAssetIndexFromAsset(pBase);
    }
    return 0;
}

// The asset index in the profile's slot nSlot (-1: empty, or nSlot is not 0..52).
int FE_CrAP_GetEquippedAsset(s16 nSlot) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

    if (nSlot >= 0 && nSlot < 53) {
        return pProfile->aAF80[nSlot];
    }
    return -1;
}

// Save the created golfer's body skin entries in the profile.
void fn_80103D6C(void) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    Skin* pSkin = lbl_80281EE0->pB4->pChar->pSkin;

    Mem_cpy(pProfile->choices.aParts, pSkin->aParts[3], SkinPart_GetNumParts(pSkin) * sizeof(SkinChoice));
    Mem_cpy(pProfile->choices.aSets, pSkin->aSets[3], SkinPart_GetNumSets(pSkin) * sizeof(SkinChoice));
}

// And the entries of its six other skins.
void fn_80103DE0(void) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    Skin* pSkin;
    int i;

    for (i = 0; i < 6; i++) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[i];
        Mem_cpy(pProfile->choices.aSkinParts[i], pSkin->aParts[3], SkinPart_GetNumParts(pSkin)
                * sizeof(SkinChoice));
        Mem_cpy(pProfile->choices.aSkinSets[i], pSkin->aSets[3], SkinPart_GetNumSets(pSkin)
                * sizeof(SkinChoice));
    }
}

// Take the asset's name out of the profile's list b when it is there.
void sTurnOffAnimation(CrAPAsset* pAsset, int b) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    char szName[16];

    fn_80105B80(pAsset, szName);
    if (fn_800587A8(pProfile, b, szName)) {
        fn_80058624(pProfile, b, szName);
    }
}

// Take the asset (the one it takes its attributes from) off the golfer being edited and out of its
// slot of the profile.
void FE_CrAP_TurnOffAsset(CrAPAsset* pAsset) {
    Skin* pSkin;
    CrAPAsset* pBase;

    FE_GetCurrentProfile();
    pBase = sGetLinkedAsset(pAsset);
    if (pBase->n2E != -1) {
        pSkin = lbl_80281EE0->pB4->pChar->pSkin;
        fn_80106D24(pBase, pSkin);
        fn_80106DA0(pBase, pSkin);
        fn_8008E944(0, 0.0f);
        fn_8001D624(lbl_80281EE0->pB4->n10);
        fn_80103D6C();
        FE_CrAP_ClearEquippedAsset(pBase);
    }
}

// Take a part's choice i off the golfer being edited (part 13 by its name, from the list b).
void FE_CrAP_TurnOffPart(s16 nPart, int b, int i) {
    CrAPAsset* pAsset;

    FE_GetCurrentProfile();
    if (lbl_80281EE0->pB4->pChar != NULL
        && (pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i)) != NULL) {
        if (nPart == 13) {
            sTurnOffAnimation(pAsset, b);
        } else {
            FE_CrAP_TurnOffAsset(pAsset);
        }
    }
}

// The asset may be picked: it was not locked when last checked, and its aB1CC bit is set.
u8 FE_CrAP_IsAssetAvailableForUser(int nAsset) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    if (!BitArray_Test(pProfile->aAssetLocked, nAsset) && BitArray_Test(pProfile->aB1CC, nAsset)) {
        return 1;
    }
    return 0;
}

// Switch the asset's name in the profile's list b: take it out when it is there, otherwise add it
// and have the menu golfer play it (unless it already does).
void sTurnOnAnimation(CrAPAsset* pAsset, int b) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    char szName[24];

    fn_80105B80(pAsset, szName);
    if (fn_800587A8(pProfile, b, szName)) {
        fn_80058624(pProfile, b, szName);
    } else {
        fn_80058560(pProfile, b, szName);
        if (fn_8008E6BC() == NULL || strcmp(fn_8008E6BC(), szName) != 0) {
            fn_8008E724(szName, FE_CrAP_GetStringFromTable(pAsset->n114), 1, 0);
        }
    }
    fn_8008E944(0, 0.0f);
}

// Part 17: set the logo place b (a set of lbl_801937C8) to variant i; for 1..5 that is the
// profile's user logo i - 1, whose set gets the variant for its shape.
void sTurnOnLogo(s16 nPart, int b, int i) {
    char szLogo[32];
    char szShape[32];
    u64 uSetId;
    u64 uVariantId;
    SaveProfile* pProfile = FE_GetCurrentProfile();
    Skin* pSkin;
    int nLogo = i - 1;
    s32 nSet;
    s32 nVariant;

    fn_8008EA38(1);
    pSkin = lbl_80281EE0->pB4->pChar->pSkin;
    fn_8008E944(0, 0.0f);
    SKA_PackName(&uSetId, lbl_801937C8[b]);
    nSet = SkinPart_FindSet(pSkin, uSetId);
    SkinPart_ChooseSet(pSkin, nSet, i, 0);
    if (i > 0 && i <= 5) {
        sprintf(szLogo, "%s%d", "userlogo", nLogo);
        if (pProfile->choices.aLogo[nLogo].nShape == LOGO_SQUARE) {
            strcpy(szShape, "square");
        } else {
            strcpy(szShape, "wide");
        }
        SKA_PackName(&uSetId, szLogo);
        SKA_PackName(&uVariantId, szShape);
        nSet = SkinPart_FindSet(pSkin, uSetId);
        nVariant = SkinPart_FindSetVariant(pSkin, nSet, uVariantId);
        SkinPart_ChooseSet(pSkin, nSet, nVariant, 0);
    }
    fn_8001D624(lbl_80281EE0->pB4->n10);
    fn_80103D6C();
}

// A part 18 asset: set the menu golfer's n0 from it and have the golfer play its animation (unless
// it already does).
void fn_801042D0(CrAPAsset* pAsset) {
    fn_8008E944(0, 0.0f);
    if (pAsset->n0 != fn_8008EAEC()) {
        fn_8008E2F8(1, 0.0f);
    }
    fn_8008EAE0(pAsset->n0);
    if (fn_8008E6BC() == NULL || strcmp(fn_8008E6BC(), FE_CrAP_GetStringFromTable(pAsset->n112)) != 0) {
        if (fn_8008E468(FE_CrAP_GetStringFromTable(pAsset->n112), FE_CrAP_GetStringFromTable(pAsset->n114),
                        1)) {
            fn_8008E818(1);
        }
    } else {
        fn_8008E818(1);
    }
}

// Put the asset (the one it takes its attributes from) on the golfer being edited, in place of the
// one in its slot of the profile, and have the golfer show it off: club assets on the club skins
// (the golfer takes up the club), balls, or the body skin; then play the asset's animation.
void FE_CrAP_TurnOnAsset(CrAPAsset* pAsset) {
    int nPart;
    u8 bLoop;
    s8 nPlay;
    Skin* pSkin;
    f32 fAngle;
    int nOld;

    FE_GetCurrentProfile();
    fAngle = 0.0f;
    bLoop = 0;
    nPlay = 1;
    pSkin = lbl_80281EE0->pB4->pChar->pSkin;
    pAsset = sGetLinkedAsset(pAsset);
    lbl_802816E8 = FE_CrAP_GetEquippedAsset(pAsset->n2E);
    lbl_802816EC = FE_CrAP_GetAssetIndexFromAsset(pAsset);
    if (pAsset->n2E == -1) {
        return;
    }
    nPart = pAsset->nPart;
    fn_8008EA38(1);
    if (fn_80106658(pAsset)) {
        fn_8008EABC(0);
        fn_8001D624(lbl_80281EE0->pB4->n10);
        if (stricmp(FE_CrAP_GetStringFromTable(pAsset->n112), "gdlcrp07") == 0 ||
            stricmp(FE_CrAP_GetStringFromTable(pAsset->n112), "fdlcrp07") == 0) {
            fAngle = 4.0f;
            fn_8008E860(0);
        } else if (fn_8008E9A8() != 1) {
            fn_8008E8D0(1);
            if (stricmp(FE_CrAP_GetStringFromTable(pAsset->nCategory), "Fairway Woods") == 0) {
                fn_8008E718(1);
            } else if (stricmp(FE_CrAP_GetStringFromTable(pAsset->nCategory), "Iron Sets") == 0) {
                fn_8008E718(3);
            } else if (stricmp(FE_CrAP_GetStringFromTable(pAsset->nCategory), "Wedge Sets") == 0) {
                fn_8008E718(5);
            } else if (stricmp(FE_CrAP_GetStringFromTable(pAsset->nCategory), "Putters") == 0) {
                fn_8008E718(2);
            }
        } else {
            fn_8008E9B4();
            bLoop = 1;
            nPlay = 0;
        }
        fn_80103DE0();
        FE_CrAP_ClearEquippedAsset(pAsset);
    } else if (FE_CrAP_TryBallSwappingAsset(pAsset)) {
        if (fn_8008E9A8() != 2) {
            fn_8008E8D0(2);
        } else {
            nPlay = 0;
        }
        FE_CrAP_ClearEquippedAsset(pAsset);
    } else {
        nOld = FE_CrAP_GetEquippedAsset(pAsset->n2E);
        if (nOld >= 0) {
            FE_CrAP_TurnOffAsset(FE_CrAP_GetAssetFromAssetIndex(nOld));
        }
        fn_80106A64(pAsset, pSkin);
        fn_80106B04(pAsset, pSkin);
        fn_8001D624(lbl_80281EE0->pB4->n10);
        fn_80103D6C();
        if (fn_8008E9A8() != 0) {
            fn_8008E8D0(0);
        }
    }
    if (strcmp(FE_CrAP_GetStringFromTable(pAsset->n112), "") == 0
        || strcmp(FE_CrAP_GetStringFromTable(pAsset->n112), "0") == 0 ||
        !fn_801048B0(nPart)) {
        fn_8008E944(0, 0.0f);
        if (fn_8008E6BC() == NULL || strcmp(fn_8008E6BC(), FE_CrAP_GetStringFromTable(pAsset->n112)) != 0) {
            fn_8008E468(FE_CrAP_GetStringFromTable(pAsset->n112), FE_CrAP_GetStringFromTable(pAsset->n114),
                        1);
        } else {
            fn_8008E818(1);
        }
        if (nPart != fn_8008EB04()) {
            fn_8008E2F8(1, 0.0f);
        }
        fn_8008EAF8(nPart);
    } else {
        fn_8008E944(1, fAngle);
        fn_8008E724(FE_CrAP_GetStringFromTable(pAsset->n112), FE_CrAP_GetStringFromTable(pAsset->n114),
                    nPlay, bLoop);
    }
    FE_CrAP_EquipAsset(pAsset);
    FE_CheckSpecialCaseConnections(pAsset);
}

// Put a part's choice i (from the list b) on the golfer being edited: part 13 by its name, part 17
// the logo, part 18 an animation, the others as an asset.
void FE_CrAP_TurnOnPart(s16 nPart, int b, int i) {
    int nAsset;
    CrAPAsset* pAsset = NULL;

    FE_GetCurrentProfile();
    if (lbl_80281EE0->pB4->pChar == NULL) {
        return;
    }
    if (nPart != 17) {
        FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
        nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
        pAsset = FE_CrAP_GetAssetFromAssetIndex(nAsset);
    }
    if (pAsset == NULL && nPart != 17) {
        return;
    }
    fn_8008E818(0);
    if (nPart == 13) {
        lbl_802816EC = nAsset;
        lbl_802816E8 = -1;
        sTurnOnAnimation(pAsset, b);
    } else if (nPart == 17) {
        sTurnOnLogo(nPart, b, i);
    } else if (nPart == 18) {
        fn_801042D0(pAsset);
    } else if (pAsset != NULL) {
        FE_CrAP_TurnOnAsset(pAsset);
    }
}

// Put on the asset waiting in lbl_802816E8, or else take off the one in lbl_802816EC; then clear
// both.
void FE_CrAP_RestoreLastRemovedAsset(void) {
    CrAPAsset* pAsset;
    s16 nKind;
    s32 nPart;
    s32 nChoice;

    FE_GetCurrentProfile();
    if (lbl_802816E8 != -1) {
        FE_CrAP_SetTriggerAnims(0);
        FE_CrAP_TurnOnAsset(FE_CrAP_GetAssetFromAssetIndex(lbl_802816E8));
        FE_CrAP_SetTriggerAnims(1);
    } else if (lbl_802816EC != -1) {
        pAsset = FE_CrAP_GetAssetFromAssetIndex(lbl_802816EC);
        if (pAsset->nPart == 13) {
            FE_CrAP_GetCategorySubcategoryAndEntryNumFromAssetID(lbl_802816EC, &nKind, &nPart, &nChoice);
            sTurnOffAnimation(pAsset, nPart);
        } else {
            FE_CrAP_TurnOffAsset(pAsset);
        }
    }
    fn_8008EB70();
    lbl_802816E8 = -1;
    lbl_802816EC = -1;
}

// The parts whose choices are grouped by category, with an "All ..." entry: headwear, shirts,
// pants and shorts, shoes, eyewear, watches and jewelry, miscellaneous; and part 12.
u8 fn_801048B0(int nPart) {
    if ((u32)nPart <= 2 || (u32)(nPart - 7) <= 1 || nPart == 12 || nPart == 19 || nPart == 20) {
        return 1;
    }
    return 0;
}

// How many offered assets of the part fit its entry b (kept in lbl_8028247C).
int FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(s16 nPart, int b) {
    int nAsset;
    int nCount;
    int nWanted;
    int nFirst;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(nPart);
    nCount = 0;
    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    for (nAsset = nFirst; nAsset < lbl_80282460->nAssets; nAsset++) {
        if (nPart == lbl_80282460->pAssets[nAsset].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[nAsset].n40) &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[nAsset].nCategory, nWanted)) {
            nCount++;
        }
    }
    lbl_8028247C[b + nPart * CRAP_NUM_PARTS] = nCount;
    return nCount;
}

// How many entries a part's list has: one per category among its offered assets, plus its "All ..."
// entry when it has one (kept in lbl_80282480).
int FE_CrAP_GetNumberOfSubcategoryIndicesForCategory(s16 nPart) {
    int nCount = 0;
    int i;
    int j;
    u8 bLater;

    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            // a category is counted at its last asset
            bLater = 0;
            for (j = i + 1; j < lbl_80282460->nAssets; j++) {
                if (nPart == lbl_80282460->pAssets[j].nPart
                    && FE_IsValidCurrentGender(lbl_80282460->pAssets[j].n40) &&
                    lbl_80282460->pAssets[j].nCategory == lbl_80282460->pAssets[i].nCategory) {
                    bLater = 1;
                    break;
                }
            }
            if (!bLater) {
                nCount++;
            }
        }
    }
    if (lbl_801932C8[nPart][0] != '\0') {
        nCount++;
    }
    lbl_80282480[nPart] = nCount;
    return nCount;
}

// The category of a part's entry n: its categories in the order its offered assets list them,
// after the "All ..." entry when the part has one (-1 for that entry, 0x40: none). Kept in
// lbl_80282478.
int FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(s16 nPart, int n) {
    s32 aCategories[64];
    int i;
    int nFound = 0;
    int j;
    int bKnown;
    int nFirst;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(nPart);
    if (lbl_801932C8[nPart][0] != '\0') {
        if (n == 0) {
            return -1;
        }
        n--;
    }
    for (i = nFirst; i < lbl_80282460->nAssets; i++) {
        if (nPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            bKnown = 0;
            for (j = 0; j < nFound; j++) {
                if (aCategories[j] == lbl_80282460->pAssets[i].nCategory) {
                    bKnown = 1;
                    break;
                }
            }
            if (!bKnown) {
                aCategories[nFound++] = lbl_80282460->pAssets[i].nCategory;
            }
            if (nFound == 64) {
                return 0x40;
            }
        }
    }
    if (n >= 0 && n < nFound) {
        lbl_80282478[n + nPart * CRAP_NUM_PARTS] = aCategories[n];
        return aCategories[n];
    }
    return 0x40;
}

// The entry of a part's list that shows a category (-1: none); see FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex.
int FE_CrAP_GetSubCategoryIndexForCategoryAndSubcategoryID(s16 nPart, int nCategory) {
    s32 aCategories[64];
    int i;
    int nFound = 0;
    int j;
    int bKnown;
    int nFirst;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(nPart);
    for (i = nFirst; i < lbl_80282460->nAssets; i++) {
        if (nPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            bKnown = 0;
            for (j = 0; j < nFound; j++) {
                if (aCategories[j] == lbl_80282460->pAssets[i].nCategory) {
                    bKnown = 1;
                    break;
                }
            }
            if (!bKnown) {
                aCategories[nFound++] = lbl_80282460->pAssets[i].nCategory;
            }
            if (nFound == 64) {
                return -1;
            }
        }
    }
    for (j = 0; j < nFound; j++) {
        if (nCategory == aCategories[j]) {
            if (lbl_801932C8[nPart][0] != '\0') {
                return (j > 0) ? j + 1 : 0;
            }
            return j;
        }
    }
    return -1;
}

u8 FE_CrAP_GetSubCategoryNameForCategoryAndSubcategoryIndex(s16 nPart, int n, char* pDst) {
    int nCategory;

    if (lbl_80282460->pStrings == NULL) {
        return 0;
    }
    if (pDst == NULL) {
        return 0;
    }
    if (lbl_801932C8[nPart][0] != '\0' && n == 0) {
        strcpy(pDst, lbl_801932C8[nPart]);
        return 1;
    }
    nCategory = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, n);
    if (nCategory == 0x40) {
        return 0;
    }
    if (nCategory == -1) {
        return 0;
    }
    strcpy(pDst, lbl_80282460->pStrings + nCategory);
    return 1;
}

// A part's choice i under its list entry b: the i-th asset offered for the current gender that fits
// the entry (NULL: none).
CrAPAsset* FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(s16 nPart, int b, int i) {
    int nAsset;
    int n;
    int nWanted;
    int nFirst;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(nPart);
    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    n = 0;
    for (nAsset = nFirst; nAsset < lbl_80282460->nAssets; nAsset++) {
        if (nPart == lbl_80282460->pAssets[nAsset].nPart &&
            FE_IsValidCurrentGender(lbl_80282460->pAssets[nAsset].n40) &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[nAsset].nCategory, nWanted)) {
            if (n == i) {
                return &lbl_80282460->pAssets[nAsset];
            }
            n++;
        }
    }
    return NULL;
}

CrAPAsset* FE_CrAP_GetAssetFromAssetIndex(int nAsset) {
    return &lbl_80282460->pAssets[nAsset];
}

int FE_CrAP_GetAssetIndexFromAsset(CrAPAsset* pAsset) {
    return pAsset - lbl_80282460->pAssets;
}

// The asset index of a part's choice i under its list entry b: the i-th asset offered for the
// current gender that fits the entry (-1: none).
int FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(s16 nPart, int b, int i) {
    int nAsset;
    int n;
    int nWanted;
    int nFirst;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(nPart);
    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    n = 0;
    for (nAsset = nFirst; nAsset < lbl_80282460->nAssets; nAsset++) {
        if (nPart == lbl_80282460->pAssets[nAsset].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[nAsset].n40) &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[nAsset].nCategory, nWanted)) {
            if (n == i) {
                return nAsset;
            }
            n++;
        }
    }
    // The original tests this flag here although both ways end the same.
    if (gSession.uFlags & 0x4000) {
        return -1;
    }
    return -1;
}

// Take the database's stream objects as they load.
void FE_CrAP_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CR_A', fn_80105188);
    Stream_RegisterLoadChunkCallback('CR_S', fn_801051F4);
}

// Find each part's first asset (the assets are sorted by part; 0 when a part has none).
void FE_CrAP_SetupFirstAssetIDs(void) {
    int nPart;
    int i;
    for (nPart = 0; nPart < CRAP_NUM_PARTS; nPart++) {
        lbl_80282474[nPart] = 0;
        for (i = 0; i < lbl_80282460->nAssets; i++) {
            if (nPart == lbl_80282460->pAssets[i].nPart) {
                lbl_80282474[nPart] = i;
                break;
            }
        }
    }
}

// The index of a part's first asset, as FE_CrAP_SetupFirstAssetIDs found it (0 when the part has none).
int FE_CrAP_GetFirstCategoryAssetID(s16 nPart) {
    return lbl_80282474[nPart];
}

// Stop taking the database's stream objects ('CR_A' and 'CR_S').
void FE_CrAP_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('CR_A');
    Stream_UnregisterLoadChunkCallback('CR_S');
}

// The 'CR_A' handler: the assets.
void fn_80105188(UStreamObject* pObject) {
    if (pObject != NULL) {
        lbl_80282460->pAssets = (CrAPAsset*)pObject->pData;
        lbl_80282460->nAssets = pObject->uSize / sizeof(CrAPAsset);
        fn_80105DAC();
        lbl_80282464 = pObject;
        fn_80105240();
    }
}

// The 'CR_S' handler: the names.
void fn_801051F4(UStreamObject* pObject) {
    if (pObject != NULL) {
        lbl_80282460->pStrings = (char*)pObject->pData;
        lbl_80282460->uStringsSize = pObject->uSize;
        fn_80105EFC();
        lbl_80282468 = pObject;
    }
}

// With the assets loaded: index the parts and pick the day's random assets.
void fn_80105240(void) {
    FE_CrAP_SetupFirstAssetIDs();
    fn_80077B78();
}

// A part's choice i: its name, and the fields below (0 or -1 when there is no such choice).
char* FE_CrAP_GetPartName(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return NULL;
    }
    return pAsset->szName;
}

s16 FE_CrAP_GetPartColor1(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n44;
}

s16 FE_CrAP_GetPartColor2(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n46;
}

s16 FE_CrAP_GetPartColor3(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n48;
}

// A part's choice i: its sponsor, an index into the brand names (lbl_801935C8); -1 when there is no
// such choice.
s16 FE_CrAP_GetPartSponsor(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n2C;
}

s32 FE_CrAP_GetPartRetailPrice(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n30;
}

s32 FE_CrAP_GetPartSalePrice(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n34;
}

s32 FE_CrAP_GetPartLevel(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n38;
}

// A part's choice i: the attributes it raises and the tiers (see FE_CrAP_GetPartAttributeUpgrade1ByAssetID).
int FE_CrAP_GetPartAttributeUpgrade1(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeUpgrade1ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

int FE_CrAP_GetPartAttributeModifier1(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeModifier1ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

int FE_CrAP_GetPartAttributeUpgrade2(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeUpgrade2ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

int FE_CrAP_GetPartAttributeModifier2(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeModifier2ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

// The attributes an asset raises (-1: none) and the tier it raises each to; an asset of lock kind
// 28 has those of the asset it names.
int FE_CrAP_GetPartAttributeUpgrade1ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nAttrA;
}

int FE_CrAP_GetPartAttributeModifier1ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nTierA;
}

int FE_CrAP_GetPartAttributeUpgrade2ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nAttrB;
}

int FE_CrAP_GetPartAttributeModifier2ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nTierB;
}

// A part's choice i: its lock kind and number (-1: no such choice).
s8 FE_CrAP_GetPartGMLockID(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->nLockKind;
}

s16 FE_CrAP_GetPartGMLockVal(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->nLock;
}

s8 FE_CrAP_GetPartGMLockIDByAssetNum(int nAsset) {
    if (nAsset < 0 || nAsset >= lbl_80282460->nAssets) {
        return -1;
    }
    return lbl_80282460->pAssets[nAsset].nLockKind;
}

s16 FE_CrAP_GetPartGMLockValByAssetNum(int nAsset) {
    if (nAsset < 0 || nAsset >= lbl_80282460->nAssets) {
        return -1;
    }
    return lbl_80282460->pAssets[nAsset].nLock;
}

int fn_80105644(s16 nPart, int b, int i, int n) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->a4A[n];
}

// Colour n of a part's choice i (from the list b), as RGBA bytes: the asset's own aColor[n], or for
// colour kinds 0..2 that colour of the skin option the asset picks, on the club skin its category
// uses (shafts and grips: the drivers' for the "fwd_" sets, else the irons') or the body skin.
void fn_8010568C(s16 nPart, int b, int i, int n, u8* pColor) {
    char szSet[16];                     // the size is unknown (the frame allows up to 16)
    CrAPAsset* pAsset;
    char* szCategory;
    s32 nSet;
    s32 nVariant;
    s32 nOption;
    f32* pOption;
    Skin* pSkin;

    pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        pColor[0] = 0;
        pColor[1] = 0;
        pColor[2] = 0;
        pColor[3] = 0xFF;
        return;
    }
    if (pAsset->aColorKind[n] == -1 || pAsset->aColorKind[n] > 2) {
        pColor[0] = pAsset->aColor[n][0];
        pColor[1] = pAsset->aColor[n][1];
        pColor[2] = pAsset->aColor[n][2];
        pColor[3] = pAsset->aColor[n][3];
        return;
    }
    if (lbl_80281EE0->pB4->pChar == NULL) {
        pColor[0] = pAsset->aColor[n][0];
        pColor[1] = pAsset->aColor[n][1];
        pColor[2] = pAsset->aColor[n][2];
        pColor[3] = pAsset->aColor[n][3];
        return;
    }
    szCategory = FE_CrAP_GetStringFromTable(pAsset->nCategory);
    if (stricmp(szCategory, "drivers") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[0];
    } else if (stricmp(szCategory, "Fairway Woods") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[1];
    } else if (stricmp(szCategory, "Iron Sets") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[3];
    } else if (stricmp(szCategory, "Wedge Sets") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[5];
    } else if (stricmp(szCategory, "Putters") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[2];
    } else if (stricmp(szCategory, "shafts") == 0) {
        fn_800CB8F0(&pAsset->aSet[0], szSet);
        if (stricmp(szSet, "fwd_shaft") == 0) {
            pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[0];
        } else {
            pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[3];
        }
    } else if (stricmp(szCategory, "grips") == 0) {
        fn_800CB8F0(&pAsset->aSet[0], szSet);
        if (stricmp(szSet, "fwd_grip") == 0) {
            pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[0];
        } else {
            pSkin = lbl_80281EE0->pB4->pChar->p16D8->apSkins[3];
        }
    } else {
        pSkin = lbl_80281EE0->pB4->pChar->pSkin;
    }
    nSet = SkinPart_FindSet(pSkin, pAsset->aSet[0]);
    nVariant = SkinPart_FindSetVariant(pSkin, nSet, pAsset->aSetVariant[0]);
    if (nSet >= 0 && nVariant >= 0) {
        nOption = SkinPart_FindSetOption(pSkin, nSet, nVariant, pAsset->aSetOption[0]);
        if (nOption >= 0) {
            // the option's data is three RGB colours, 0..1 each
            pOption = (f32*)fn_800CD248(pSkin, nSet, nVariant, nOption);
            if (pOption == NULL) {
                pColor[0] = pAsset->aColor[n][0];
                pColor[1] = pAsset->aColor[n][1];
                pColor[2] = pAsset->aColor[n][2];
                pColor[3] = pAsset->aColor[n][3];
                return;
            }
            if (pAsset->aColorKind[n] == 0) {
                pColor[0] = 255.0f * pOption[6];
                pColor[1] = 255.0f * pOption[7];
                pColor[2] = 255.0f * pOption[8];
                pColor[3] = 0xFF;
                return;
            }
            if (pAsset->aColorKind[n] == 1) {
                pColor[0] = 255.0f * pOption[3];
                pColor[1] = 255.0f * pOption[4];
                pColor[2] = 255.0f * pOption[5];
                pColor[3] = 0xFF;
                return;
            }
            pColor[0] = 255.0f * pOption[0];
            pColor[1] = 255.0f * pOption[1];
            pColor[2] = 255.0f * pOption[2];
            pColor[3] = 0xFF;
            return;
        }
        pColor[0] = pAsset->aColor[n][0];
        pColor[1] = pAsset->aColor[n][1];
        pColor[2] = pAsset->aColor[n][2];
        pColor[3] = pAsset->aColor[n][3];
        return;
    }
    pColor[0] = pAsset->aColor[n][0];
    pColor[1] = pAsset->aColor[n][1];
    pColor[2] = pAsset->aColor[n][2];
    pColor[3] = pAsset->aColor[n][3];
}

void fn_80105B4C(s16 nPart, int b, int i, char* pName) {
    fn_80105B80(FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i), pName);
}

// Copy the name of the asset's first variant id. The ids are byte-swapped for the lookup and swapped
// back after.
void fn_80105B80(CrAPAsset* pAsset, char* pName) {
    u64 nId;
    u8* pSrc;

    pSrc = (u8*)pAsset->aVariant;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)pAsset->aVariant, sizeof(pAsset->aVariant), sizeof(u64));
    nId = pAsset->aVariant[0];
    fn_800CB868(&nId, pName);
    pSrc = (u8*)pAsset->aVariant;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)pAsset->aVariant, sizeof(pAsset->aVariant), sizeof(u64));
}

s32 FE_CrAP_GetNumEntriesInCrAPDB(void) {
    return lbl_80282460->nAssets;
}

// An asset's level (n38, as FE_CrAP_GetPartLevel); fe_craputils makes the assets of level 0
// available from the start.
s32 FE_CrAP_GetPartLevelFromAssetIndex(int nAsset) {
    CrAPAsset* pAsset = &lbl_80282460->pAssets[nAsset];
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n38;
}

u8 FE_CrAP_IsCrAPDBLoaded(void) {
    return lbl_80282460 != NULL;
}

// How many different choices fit a part's entry b: assets of the same category and the same first
// colour count once.
int fn_80105C44(s16 nPart, int b) {
    int nCount = 0;
    int i;
    int j;
    int nWanted;
    u8 bEarlier;

    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40) &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[i].nCategory, nWanted)) {
            bEarlier = 0;
            for (j = i - 1; j >= 0; j--) {
                if (nPart == lbl_80282460->pAssets[j].nPart
                    && FE_IsValidCurrentGender(lbl_80282460->pAssets[j].n40) &&
                    lbl_80282460->pAssets[j].nCategory == lbl_80282460->pAssets[i].nCategory &&
                    lbl_80282460->pAssets[j].aColor[0][0] == lbl_80282460->pAssets[i].aColor[0][0] &&
                    lbl_80282460->pAssets[j].aColor[0][1] == lbl_80282460->pAssets[i].aColor[0][1] &&
                    lbl_80282460->pAssets[j].aColor[0][2] == lbl_80282460->pAssets[i].aColor[0][2] &&
                    lbl_80282460->pAssets[j].aColor[0][3] == lbl_80282460->pAssets[i].aColor[0][3]) {
                    bEarlier = 1;
                    break;
                }
            }
            if (!bEarlier) {
                nCount++;
            }
        }
    }
    return nCount;
}

// Swap every asset from the disc's byte order.
void fn_80105DAC(void) {
    u8* pSrc;
    u8* pDst;
    u32 i;

    for (i = 0; i < lbl_80282460->nAssets; i++) {
        pSrc = (u8*)&lbl_80282460->pAssets[i];
        pDst = (u8*)&lbl_80282460->pAssets[i];
        ByteSwap_Records((void**)&pSrc, (void**)&pDst, lbl_80193228, 20, 1);
        pSrc = (u8*)&lbl_80282460->pAssets[i].n110;
        pDst = (u8*)&lbl_80282460->pAssets[i].n110;
        BYTESWAP_SWAPDATA(&pSrc, pDst, sizeof(s16), sizeof(s16));
        pSrc = (u8*)&lbl_80282460->pAssets[i].n112;
        pDst = (u8*)&lbl_80282460->pAssets[i].n112;
        BYTESWAP_SWAPDATA(&pSrc, pDst, sizeof(s16), sizeof(s16));
        pSrc = (u8*)&lbl_80282460->pAssets[i].n114;
        pDst = (u8*)&lbl_80282460->pAssets[i].n114;
        BYTESWAP_SWAPDATA(&pSrc, pDst, sizeof(s16), sizeof(s16));
        pSrc = (u8*)&lbl_80282460->pAssets[i].n116;
        pDst = (u8*)&lbl_80282460->pAssets[i].n116;
        BYTESWAP_SWAPDATA(&pSrc, pDst, sizeof(s16), sizeof(s16));
    }
}

// Called when the names arrive; empty in this build.
void fn_80105EFC(void) {
}

// After putting on a part 9 or part 1 asset, switch the body skin's set "wire" (part 9) or "hands"
// (part 1) to its "nowire" variant.
void FE_CheckSpecialCaseConnections(CrAPAsset* pAsset) {
    s16 nPart = pAsset->nPart;
    s32 nSet;
    Skin* pSkin;
    s32 nVariant;

    if (lbl_80281EE0->pB4->pChar != NULL) {
        pSkin = lbl_80281EE0->pB4->pChar->pSkin;
        if (nPart == 9) {
            nSet = fn_800CDCA0(pSkin, "wire");
            nVariant = SkinPart_FindSetVariantByName(pSkin, nSet, "nowire");
            if (nSet >= 0 && nVariant >= 0) {
                fn_800CC9D8(lbl_80281EE0->pB4->pChar, nSet, nVariant, 0);
            }
        } else if (nPart == 1) {
            nSet = fn_800CDCA0(pSkin, "hands");
            nVariant = SkinPart_FindSetVariantByName(pSkin, nSet, "nowire");
            if (nSet >= 0 && nVariant >= 0) {
                fn_800CC9D8(lbl_80281EE0->pB4->pChar, nSet, nVariant, 0);
            }
        }
    }
}

// Where an asset sits in the menus: its part, the part's list entry for its category, and its place
// among that entry's offered assets.
void FE_CrAP_GetCategorySubcategoryAndEntryNumFromAssetID(int nAsset, s16* pnPart, s32* pnEntry,
                                                          s32* pnPlace) {
    int i;
    int nCount = 0;
    int nFirst;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(lbl_80282460->pAssets[nAsset].nPart);
    *pnPart = lbl_80282460->pAssets[nAsset].nPart;
    *pnEntry = FE_CrAP_GetSubCategoryIndexForCategoryAndSubcategoryID(*pnPart, lbl_80282460->pAssets[nAsset].nCategory);
    for (i = nFirst; i < nAsset; i++) {
        if (*pnPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            if (FE_IsMatchingSubCategory(*pnPart, lbl_80282460->pAssets[i].nCategory,
                            lbl_80282460->pAssets[nAsset].nCategory)) {
                nCount++;
            }
        }
    }
    *pnPlace = nCount;
}

// How many offered assets of the part that fit its entry n come before the asset in the part's
// list: the asset's place in that list.
void FE_CrAP_GetEntryNumFromAssetIDCategorySubcategory(int nAsset, s16 nPart, int n, s32* pnPlace) {
    int i;
    int nCount = 0;
    int nFirst;
    int nWanted;

    nFirst = FE_CrAP_GetFirstCategoryAssetID(lbl_80282460->pAssets[nAsset].nPart);
    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, n);

    for (i = nFirst; i < nAsset; i++) {
        if (nPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40) &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[i].nCategory, nWanted)) {
            nCount++;
        }
    }
    *pnPlace = nCount;
}

// An asset with this n40 is offered: it matches the database's n4, or 2 (any).
u8 FE_IsValidCurrentGender(s8 n) {
    if (n == lbl_80282460->n4 || n == 2) {
        return 1;
    }
    return 0;
}

// An asset of category nCategory fits the category a part's list shows (nWanted); a part with an
// "All ..." entry shows every category for -1.
u8 FE_IsMatchingSubCategory(s16 nPart, int nCategory, int nWanted) {
    if (lbl_801932C8[nPart][0] != '\0') {
        return nWanted == -1 || nCategory == nWanted;
    }
    return nCategory == nWanted;
}

// The asset in the first of the profile's slots whose asset is of the part (-1: none).
int FE_CrAP_GetFirstEquippedIndexForCategory(s16 nPart) {
    s16 i;
    int nAsset;

    FE_GetCurrentProfile();
    for (i = 0; i < 53; i++) {
        nAsset = FE_CrAP_GetEquippedAsset(i);
        if (nAsset >= 0 && nPart == lbl_80282460->pAssets[nAsset].nPart) {
            return nAsset;
        }
    }
    return -1;
}

// The asset in the first of the profile's slots whose asset is of the part and fits the part's
// entry n (-1: none).
int FE_CrAP_GetFirstEquippedIndexForCategoryAndSubcategory(s16 nPart, int n) {
    s16 i;
    int nAsset;
    int nWanted;
    CrAPAsset* pAsset;

    FE_GetCurrentProfile();
    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, n);
    for (i = 0; i < 53; i++) {
        nAsset = FE_CrAP_GetEquippedAsset(i);
        if (nAsset >= 0) {
            pAsset = &lbl_80282460->pAssets[nAsset];
            if (nPart == pAsset->nPart && FE_IsMatchingSubCategory(nPart, pAsset->nCategory, nWanted)) {
                return nAsset;
            }
        }
    }
    return -1;
}

// A part's choice i is the asset in its slot of the profile.
u8 FE_CrAP_IsItemEquipped(s16 nPart, int b, int i) {
    int nWanted;
    int nAsset;
    int n;
    int nFirst;

    FE_GetCurrentProfile();
    nFirst = FE_CrAP_GetFirstCategoryAssetID(nPart);
    nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    n = -1;
    for (nAsset = nFirst; nAsset < lbl_80282460->nAssets; nAsset++) {
        if (nPart == lbl_80282460->pAssets[nAsset].nPart &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[nAsset].nCategory, nWanted) &&
            FE_IsValidCurrentGender(lbl_80282460->pAssets[nAsset].n40)) {
            n++;
            if (n == i) {
                return FE_CrAP_IsAssetEquipped(FE_CrAP_GetAssetFromAssetIndex(nAsset));
            }
        }
    }
    return 0;
}

// Copy the name at nOffset in the 'CR_S' strings into pDst ("" for "NONE").
u8 FE_CrAP_GetColorNameFromID(int nOffset, char* pDst) {
    char* pStrings = lbl_80282460->pStrings;
    if (pStrings == NULL) {
        return 0;
    }
    if (pDst == NULL) {
        return 0;
    }
    if (nOffset == -1) {
        return 0;
    }
    strcpy(pDst, pStrings + nOffset);
    if (stricmp(pDst, "NONE") == 0) {
        *pDst = '\0';
    }
    return 1;
}

// The string at offset id in the 'CR_S' strings (the names the assets use: categories, animations,
// camera shots); NULL when they are not loaded or id is -1.
char* FE_CrAP_GetStringFromTable(int nCategory) {
    if (lbl_80282460->pStrings == NULL) {
        return NULL;
    }
    if (nCategory == -1) {
        return NULL;
    }
    return lbl_80282460->pStrings + nCategory;
}

// Copy how a part's choice i is unlocked into pDst: the Game Boy Advance link for lock kind 2,
// otherwise its text in 'CR_S' (pDst is left as it is when it has none).
u8 FE_CrAP_GetUnlockMessageFrom(s16 nPart, int b, int i, char* pDst) {
    int nAsset;
    int nWanted = FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    int n;
    CrAPAsset* pAsset;

    if (lbl_80282460->pStrings == NULL) {
        return 0;
    }
    if (pDst == NULL) {
        return 0;
    }
    n = 0;
    for (nAsset = 0; nAsset < lbl_80282460->nAssets; nAsset++) {
        if (nPart == lbl_80282460->pAssets[nAsset].nPart &&
            FE_IsValidCurrentGender(lbl_80282460->pAssets[nAsset].n40) &&
            FE_IsMatchingSubCategory(nPart, lbl_80282460->pAssets[nAsset].nCategory, nWanted)) {
            if (n == i) {
                CrAPDB* pDB = lbl_80282460;
                pAsset = &pDB->pAssets[nAsset];
                if (pAsset->nLockKind == 2) {
                    strcpy(pDst, "Game Boy\xAE Advance Link Required");
                    return 1;
                }
                if (pAsset->n110 != -1) {
                    strcpy(pDst, pDB->pStrings + pAsset->n110);
                }
                return 1;
            }
            n++;
        }
    }
    return 0;
}

// A club asset (a category fn_80106750 knows): put it on all six club skins (see the EA bug
// below), not only the ones its category uses. 0 when it is not a club asset or the golfer has no
// club skins.
u8 fn_80106658(CrAPAsset* pAsset) {
    Skin* apSkins[6];
    int nSkins;
    int i;

    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL ||
        lbl_80281EE0->pB4->pChar->p16D8 == NULL) {
        return 0;
    }
    nSkins = fn_80106750(pAsset, apSkins);
    if (nSkins <= 0) {
        return 0;
    }
    for (i = 0; i < nSkins; i++) {
        // EA bug: the inner loop reuses i, so every club skin gets the asset once, whatever nSkins
        // is, and apSkins is never read
        for (i = 0; i < 6; i++) {
            fn_80106A64(pAsset, lbl_80281EE0->pB4->pChar->p16D8->apSkins[i]);
            fn_80106BF8(pAsset, lbl_80281EE0->pB4->pChar->p16D8->apSkins[i]);
        }
    }
    return 1;
}

// The club skins an asset's category goes on, into apSkins; how many (0: not a club category).
// The six skins are the drivers, fairway woods, putters, two of irons and the wedges.
int fn_80106750(CrAPAsset* pAsset, Skin** apSkins) {
    char* szCategory = FE_CrAP_GetStringFromTable(pAsset->nCategory);

    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL ||
        lbl_80281EE0->pB4->pChar->p16D8 == NULL || apSkins == NULL) {
        return 0;
    }
    if (stricmp(szCategory, "shafts") == 0 || stricmp(szCategory, "grips") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[0];
        apSkins[1] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[1];
        apSkins[2] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[2];
        apSkins[3] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[3];
        apSkins[4] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[4];
        apSkins[5] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[5];
        return 6;
    }
    if (stricmp(szCategory, "drivers") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[0];
        return 1;
    }
    if (stricmp(szCategory, "Fairway Woods") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[1];
        return 1;
    }
    if (stricmp(szCategory, "Iron Sets") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[3];
        apSkins[1] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[4];
        return 2;
    }
    if (stricmp(szCategory, "Wedge Sets") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[5];
        return 1;
    }
    if (stricmp(szCategory, "Putters") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->p16D8->apSkins[2];
        return 1;
    }
    return 0;
}

// A part 12 asset of the category "balls": make its ball the profile's (nGolferOutfit) and show it.
u8 FE_CrAP_TryBallSwappingAsset(CrAPAsset* pAsset) {
    char szName[16];                    // the size is unknown (the frame allows up to 20)
    char* szCategory;
    s8 nBall;

    if (pAsset->nPart != 12) {
        return 0;
    }
    szCategory = FE_CrAP_GetStringFromTable(pAsset->nCategory);
    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL ||
        lbl_80281EE0->pB4->pChar->p16D8 == NULL) {
        return 0;
    }
    if (stricmp(szCategory, "balls") == 0) {
        fn_800CB8F0(&pAsset->aSetVariant[0], szName);
        fn_8008E960(szName);
        nBall = fn_800484F4(szName);
        FE_GetCurrentProfile()->nGolferOutfit = nBall;
        return 1;
    }
    return 0;
}

// Put the asset on a skin: each of its parts the skin has gets the asset's variant.
void fn_80106A64(CrAPAsset* pAsset, Skin* pSkin) {
    int i;
    s32 nPart;
    s32 nVariant;

    for (i = 0; i < 4; i++) {
        nPart = SkinPart_FindPart(pSkin, pAsset->aPart[i]);
        nVariant = fn_800CDBB0(pSkin, nPart, pAsset->aVariant[i]);
        if (nPart >= 0 && nVariant >= 0) {
            fn_800CCB08(pSkin, nPart, nVariant);
        }
    }
}

// And each of its sets the skin has gets the asset's variant and option.
void fn_80106B04(CrAPAsset* pAsset, Skin* pSkin) {
    s32 nSet;
    s32 nVariant;
    s32 nOption;
    int i;

    for (i = 0; i < 4; i++) {
        nSet =SkinPart_FindSet(pSkin, pAsset->aSet[i]);
        nVariant = SkinPart_FindSetVariant(pSkin, nSet, pAsset->aSetVariant[i]);
        if (nSet >= 0 && nVariant >= 0) {
            nOption = SkinPart_FindSetOption(pSkin, nSet, nVariant, pAsset->aSetOption[i]);
            SkinPart_ChooseSet(pSkin, nSet, nVariant, nOption);
            if (SkinPart_GetChangeAllCopies() && nOption >= 0) {
                fn_800CECE0(pSkin, nSet, nVariant, nOption,
                            lbl_80281EE0->pB4->pChar->a64[lbl_80281EE0->pB4->pChar->n74]);
            }
        }
    }
}

// The same for a club skin, except that a set's "DefaultL" variant is used instead when it has one
// and the profile's choices.n113 is 1.
void fn_80106BF8(CrAPAsset* pAsset, Skin* pSkin) {
    s32 nSet;
    s32 nVariant;
    s32 nDefaultL;
    s32 nOption;
    int i;

    for (i = 0; i < 4; i++) {
        nSet = SkinPart_FindSet(pSkin, pAsset->aSet[i]);
        nVariant = SkinPart_FindSetVariant(pSkin, nSet, pAsset->aSetVariant[i]);
        nDefaultL = SkinPart_FindSetVariantByName(pSkin, nSet, "DefaultL");
        if (nDefaultL >= 0 && FE_GetCurrentProfile()->choices.n113 == 1) {
            nVariant = nDefaultL;
        }
        if (nSet >= 0 && nVariant >= 0) {
            nOption = SkinPart_FindSetOption(pSkin, nSet, nVariant, pAsset->aSetOption[i]);
            SkinPart_ChooseSet(pSkin, nSet, nVariant, nOption);
            if (SkinPart_GetChangeAllCopies() && nOption >= 0) {
                fn_800CECE0(pSkin, nSet, nVariant, nOption,
                            lbl_80281EE0->pB4->pChar->a64[lbl_80281EE0->pB4->pChar->n74]);
            }
        }
    }
}

// Take the asset's parts off a skin: each goes back to variant 0.
void fn_80106D24(CrAPAsset* pAsset, Skin* pSkin) {
    int i;
    s32 nPart;

    if (pAsset != NULL) {
        for (i = 0; i < 4; i++) {
            nPart = SkinPart_FindPart(pSkin, pAsset->aPart[i]);
            if (nPart >= 0) {
                fn_800CCB08(pSkin, nPart, 0);
            }
        }
    }
}

// And its sets: each goes back to its "Defaults" variant (or 0).
void fn_80106DA0(CrAPAsset* pAsset, Skin* pSkin) {
    int i;
    int nSet;
    s32 nVariant;

    if (pAsset != NULL) {
        for (i = 0; i < 4; i++) {
            nSet = SkinPart_FindSet(pSkin, pAsset->aSet[i]);
            if (nSet >= 0) {
                nVariant = SkinPart_FindSetVariantByName(pSkin, nSet, "Defaults");
                if (nVariant < 0) {
                    nVariant = 0;
                }
                SkinPart_ChooseSet(pSkin, nSet, nVariant, 0);
            }
        }
    }
}

// How many of the profile's slots hold an asset whose n2C is n.
int FE_CrAP_GetNumEquippedItemsWithSponsor(s16 n) {
    s16 i;
    int nAsset;
    int nCount = 0;

    FE_GetCurrentProfile();
    for (i = 0; i < 53; i++) {
        nAsset = FE_CrAP_GetEquippedAsset(i);
        if (nAsset >= 0 && n == lbl_80282460->pAssets[nAsset].n2C) {
            nCount++;
        }
    }
    return nCount;
}

// How many offered assets have lock kind nKind and lock number nLock.
s32 FE_CrAP_GetNumItemsWithLockModeAndLockVal(s32 nKind, s32 nLock) {
    int i;
    int nCount = 0;

    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nKind == lbl_80282460->pAssets[i].nLockKind && nLock == lbl_80282460->pAssets[i].nLock &&
            FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            nCount++;
        }
    }
    return nCount;
}

// Copy the names of the first three of those assets; how many there are, up to 3.
s32 FE_CrAP_GetFirstThreeItemsWithLockModeAndVal(s32 nKind, s32 nLock, char* szFirst, char* szSecond, char* szThird) {
    int i;
    int nCount = 0;

    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nKind == lbl_80282460->pAssets[i].nLockKind && nLock == lbl_80282460->pAssets[i].nLock &&
            FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            nCount++;
            switch (nCount) {
            case 1:
                strcpy(szFirst, lbl_80282460->pAssets[i].szName);
                break;
            case 2:
                strcpy(szSecond, lbl_80282460->pAssets[i].szName);
                break;
            case 3:
                strcpy(szThird, lbl_80282460->pAssets[i].szName);
                return nCount;
            }
        }
    }
    return nCount;
}

// The lowest nLock above nAfter among the assets of lock kind nKind (-1: none).
s32 FE_CrAP_GetNextUnlockVal(s32 nKind, s32 nAfter) {
    int i;
    int nBest = 999999999;

    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nKind == lbl_80282460->pAssets[i].nLockKind) {
            if (lbl_80282460->pAssets[i].nLock > nAfter && lbl_80282460->pAssets[i].nLock < nBest) {
                nBest = lbl_80282460->pAssets[i].nLock;
            }
        }
    }
    if (nBest == 999999999) {
        return -1;
    }
    return nBest;
}

// Fill lbl_80282470: a record for each of the profile's set a1054C entries and each asset in the
// profile's slots whose n2C is that entry's n (with fn_800F0304 of the entry and the asset's name).
// How many records there are.
s32 fn_801070F4(void) {
    s32 aAssets[64];
    s16 nSlot;
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32* pBase = aAssets;
    int nAsset;
    int i;
    int j;
    int nAssets = 0;

    for (nSlot = 0; nSlot < 53; nSlot++) {
        nAsset = FE_CrAP_GetEquippedAsset(nSlot);
        if (nAsset >= 0 && nAssets < 64) {
            pBase[nAssets] = nAsset;
            nAssets++;
        }
    }
    lbl_8028246C = 0;
    for (i = 0; i < 11; i++) {
        if (pProfile->a1054C[i].b) {
            for (j = 0; j < nAssets; j++) {
                if (pProfile->a1054C[i].n == lbl_80282460->pAssets[pBase[j]].n2C) {
                    lbl_80282470[lbl_8028246C].n0 = pProfile->a1054C[i].n;
                    lbl_80282470[lbl_8028246C].n4 = fn_800F0304(i);
                    strcpy(lbl_80282470[lbl_8028246C].sz8, lbl_80282460->pAssets[pBase[j]].szName);
                    lbl_8028246C++;
                }
            }
        }
    }
    return lbl_8028246C;
}

// Copy record n out of lbl_80282470.
void fn_80107244(int n, s16* pN0, s32* pN4, char* pDst) {
    *pN0 = lbl_80282470[n].n0;
    *pN4 = lbl_80282470[n].n4;
    strcpy(pDst, lbl_80282470[n].sz8);
}

// Copy sponsor n's brand name ("adidas", "Callaway Golf"...) into pDst.
void FE_CrAP_GetSponsorName(s16 n, char* pDst) {
    strcpy(pDst, lbl_801935C8[n]);
}

void FE_CrAP_GetCategoryInfo(s16 nPart, s32* pLocked, s32* pB1CC, s32* pB344, s32* pAll) {
    int i;
    SaveProfile* pProfile = FE_GetCurrentProfile();

    *pLocked = 0;
    *pB1CC = 0;
    *pB344 = 0;
    *pAll = 0;
    for (i = 0; i < lbl_80282460->nAssets; i++) {
        if (nPart == lbl_80282460->pAssets[i].nPart
            && FE_IsValidCurrentGender(lbl_80282460->pAssets[i].n40)) {
            if (BitArray_Test(pProfile->aB1CC, i)) {
                *pB1CC += 1;
            }
            if (BitArray_Test(pProfile->aB344, i)) {
                *pB344 += 1;
            }
            if (fn_80078008(i, pProfile)) {
                *pLocked += 1;
            }
            *pAll += 1;
        }
    }
}

// Take the asset in the profile's slot nSlot off the golfer being edited.
void FE_CrAP_UnequipSlot(s16 nSlot) {
    int nAsset;

    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL) {
        return;
    }
    nAsset = FE_CrAP_GetEquippedAsset(nSlot);
    if (nAsset >= 0) {
        FE_CrAP_TurnOffAsset(FE_CrAP_GetAssetFromAssetIndex(nAsset));
    }
}

// The part an asset is a choice for.
s16 FE_CrAP_GetCategoryFromAssetID(int nAsset) {
    return lbl_80282460->pAssets[nAsset].nPart;
}

int FE_CrAP_GetLevelFromAssetID(int nAsset) {
    return lbl_80282460->pAssets[nAsset].n38;
}

// Copy the name of an asset's category into pDst.
void FE_CrAP_GetSubcategoryNameFromAssetID(int nAsset, char* pDst) {
    strcpy(pDst, lbl_80282460->pStrings + lbl_80282460->pAssets[nAsset].nCategory);
}

// Copy an asset's name into pDst.
void fn_8010749C(int nAsset, char* pDst) {
    strcpy(pDst, lbl_80282460->pAssets[nAsset].szName);
}

u8 fn_801074D4(int nAsset) {
    s16 n2E = FE_CrAP_GetAssetFromAssetIndex(nAsset)->n2E;

    if (FE_CrAP_GetCategoryFromAssetID(nAsset) == 13) {
        return 1;
    }
    switch (n2E) {
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return 1;
    }
    return 0;
}

// Senders the EA Sports Bio screens (EASportsBio.c) use: message nMsg with its values to the front
// end's handler, when there is a front end. A string value goes as a MsgString.

void fn_80107554(int nMsg, s32 nA) {
    MsgArg arg;

    if (lbl_80281F1C != NULL) {
        arg.i = nA;
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, &arg);
    }
}

void fn_80107594(int nMsg, s32 nA, char* szB) {
    MsgString str;
    MsgArg args[2];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].p = &str;
        fn_8010766C(&args[1], szB);
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 2, args);
    }
}

void fn_801075F8(int nMsg, s32 nA, char* szB, s32 nC) {
    MsgArg args[3];
    MsgString str;

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].p = &str;
        fn_8010766C(&args[1], szB);
        args[2].i = nC;
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 3, args);
    }
}

// Point the string value pArg holds at sz.
int fn_8010766C(MsgArg* pArg, char* sz) {
    ((MsgString*)pArg->p)->pStr = sz;
    ((MsgString*)pArg->p)->nLen = strlen(sz);
    return 0;
}

int fn_801076B0(char* sz, int nMsg) {
    MsgArg arg;
    MsgString str;

    if (lbl_80281F1C == NULL) {
        return -1;
    }
    arg.p = &str;
    fn_8010766C(&arg, sz);
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, &arg);
    return 0;
}

void fn_8010771C(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, f32 fG) {
    MsgArg args[7];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].i = nB;
        args[2].i = nC;
        args[3].i = nD;
        args[4].i = nE;
        args[5].i = nF;
        args[6].f = fG;
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 7, args);
    }
}

void fn_80107774(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, s32 nG, s32 nH, s32 nI,
                 s32 nJ) {
    MsgArg args[10];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].i = nB;
        args[2].i = nC;
        args[3].i = nD;
        args[4].i = nE;
        args[5].i = nF;
        args[6].i = nG;
        args[7].i = nH;
        args[8].i = nI;
        args[9].i = nJ;
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 10, args);
    }
}

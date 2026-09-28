// FE_CrAPDB.c (EA's name, from its asserts; TW06): the Create-A-Player database, every asset a
// created golfer can wear or carry (hair, faces, shirts, hats, clubs...). The assets arrive in the
// 'CR_A' stream object and the names they use in 'CR_S'; each asset belongs to one of the
// CRAP_NUM_PARTS parts, has a category and a lock kind, and raises up to two attributes. The part
// picker shows each part as a list of entries (an "All ..." entry for some parts, then one per
// category), each entry holding its choices; EA's names call the part the category, the entry the
// subcategory index and the choice the entry number.

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
u8   FE_CrAP_IsFadeOutCategory(int nPart);
int  FE_CrAP_GetAssetIndexFromAsset(CrAPAsset* pAsset);
int  FE_CrAP_GetFirstCategoryAssetID(s16 nPart);
void FE_CrAP_LoadAssetsFromStream(UStreamObject* pObject);
void FE_CrAP_LoadStringsFromStream(UStreamObject* pObject);
void FE_CrAP_PostAssetsLoad(void);
void FE_CrAP_GetAssetVariantName(CrAPAsset* pAsset, char* pName);
void CrAPAssetsByteSwap(void);
void FE_CrAP_PostStringsLoad(void);
void FE_CheckSpecialCaseConnections(CrAPAsset* pAsset);
u8   FE_IsMatchingSubCategory(s16 nPart, int nCategory, int nWanted);
u8   FE_CrAP_TryClubSwappingAsset(CrAPAsset* pAsset);
int  FE_CrAP_GetClubSkinsForAsset(CrAPAsset* pAsset, Skin** apSkins);
u8   FE_CrAP_TryBallSwappingAsset(CrAPAsset* pAsset);
void FE_CrAP_ApplyAssetParts(CrAPAsset* pAsset, Skin* pSkin);
void FE_CrAP_ApplyAssetSets(CrAPAsset* pAsset, Skin* pSkin);
void FE_CrAP_ApplyAssetSetsToClubSkin(CrAPAsset* pAsset, Skin* pSkin);
void FE_CrAP_RemoveAssetParts(CrAPAsset* pAsset, Skin* pSkin);
void FE_CrAP_RemoveAssetSets(CrAPAsset* pAsset, Skin* pSkin);
int  FE_SetHintString(MsgArg* pArg, char* sz);

// UISScreen.c's sender, with the front end's view of its arguments (as GameMessages.c declares it;
// uistudio.h has UIStudio* and const s32*, and game/frontend.h cannot be included with it).
void UISProcessHint(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);

// Allocate the Create-A-Player database (empty until the 'CR_A' and 'CR_S' objects load), its
// per-part first-asset table, the list caches and the 64 sponsorship records (lbl_80282470), and
// clear the caches.
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

// Forget the cached list sizes and categories: lbl_80282480, lbl_8028247C and lbl_80282478 go to -1
// (FE_CrAP_SetCurrentGender calls it, since the lists change with the gender).
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

// Whether putting on an asset runs the menu golfer's animation and camera calls (1) or not (0); see
// FE_CrAP_SetTriggerAnims.
u8 FE_CrAP_GetTriggerAnims(void) {
    return lbl_80282460->b14;
}

// Set the gender of the golfer being created, which picks the assets offered, and clear the cached
// list tables (FE_CrAP_ResetLastCategoryTables).
void FE_CrAP_SetCurrentGender(s8 n) {
    lbl_80282460->n4 = n;
    FE_CrAP_ResetLastCategoryTables();
}

// The gender of the golfer being created (FE_CrAP_SetCurrentGender), which picks the assets offered
// (FE_IsValidCurrentGender).
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
void FE_CrAP_SaveBodySkinChoices(void) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    Skin* pSkin = lbl_80281EE0->pB4->pChar->pSkin;

    Mem_cpy(pProfile->choices.aParts, pSkin->aParts[3], SkinPart_GetNumParts(pSkin) * sizeof(SkinChoice));
    Mem_cpy(pProfile->choices.aSets, pSkin->aSets[3], SkinPart_GetNumSets(pSkin) * sizeof(SkinChoice));
}

// Save the six club skins' part and set choices of the golfer being edited in the profile
// (choices.aSkinParts, aSkinSets), as FE_CrAP_SaveBodySkinChoices does for the body.
void FE_CrAP_SaveClubSkinChoices(void) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    Skin* pSkin;
    int i;

    for (i = 0; i < 6; i++) {
        pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[i];
        Mem_cpy(pProfile->choices.aSkinParts[i], pSkin->aParts[3], SkinPart_GetNumParts(pSkin)
                * sizeof(SkinChoice));
        Mem_cpy(pProfile->choices.aSkinSets[i], pSkin->aSets[3], SkinPart_GetNumSets(pSkin)
                * sizeof(SkinChoice));
    }
}

// Part 13 (custom animations): take the asset's animation (its first variant's name) out of the
// profile's animation list b, when it is there.
void sTurnOffAnimation(CrAPAsset* pAsset, int b) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    char szName[16];

    FE_CrAP_GetAssetVariantName(pAsset, szName);
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
        FE_CrAP_RemoveAssetParts(pBase, pSkin);
        FE_CrAP_RemoveAssetSets(pBase, pSkin);
        fn_8008E944(0, 0.0f);
        Character_RequestClothesUpdateFE(lbl_80281EE0->pB4->n10);
        FE_CrAP_SaveBodySkinChoices();
        FE_CrAP_ClearEquippedAsset(pBase);
    }
}

// Take choice i under entry b of a part's list off the golfer being edited: a part 13 animation
// comes out of the profile's list b, any other asset off the body skin and out of its slot. Nothing
// without a menu golfer or such a choice.
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

// The player may pick the asset: it was not locked when last checked (aAssetLocked) and the profile
// owns it (aB1CC: set for the level 0 assets, cleared when one is sold).
u8 FE_CrAP_IsAssetAvailableForUser(int nAsset) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    if (!BitArray_TestBit(pProfile->aAssetLocked, nAsset) && BitArray_TestBit(pProfile->aB1CC, nAsset)) {
        return 1;
    }
    return 0;
}

// Part 13 (custom animations): take the asset's animation out of the profile's list b when it is
// there; otherwise add it, and the menu golfer plays it with the asset's camera shot n114 unless it
// already plays it.
void sTurnOnAnimation(CrAPAsset* pAsset, int b) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    char szName[24];

    FE_CrAP_GetAssetVariantName(pAsset, szName);
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

// Part 17 (logos and tattoos): give logo place b (lbl_801937C8: the shirt, hat and glove logos, the
// arm and leg tattoos) variant i. Variants 1..5 are the profile's user logos 0..4, whose own set
// "userlogo<n>" then gets its "square" or "wide" variant from the logo's shape. Saves the body skin
// choices.
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
    Character_RequestClothesUpdateFE(lbl_80281EE0->pB4->n10);
    FE_CrAP_SaveBodySkinChoices();
}

// Part 18 (the body sliders): show the slider asset on the menu golfer. The golfer turns to the
// front when the asset's n0 differs from the last one shown (kept by fn_8008EAE0), then plays the
// asset's animation n112 with its camera shot n114 unless that animation already plays.
void sApplySlider(CrAPAsset* pAsset) {
    fn_8008E944(0, 0.0f);
    if (pAsset->n0 != fn_8008EAEC()) {
        FE_SetCrapRotation(1, 0.0f);
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

// Put the asset (the one it takes its attributes from) on the golfer being edited and in its slot
// of the profile, and show it off. Club assets go on the club skins (the golfer takes up that
// club), balls on the ball; any other replaces its slot's asset on the body skin. Then its
// animation n112 plays with its camera shot n114: for the fade-out parts
// (FE_CrAP_IsFadeOutCategory) with the texture swap delayed (by 4 for the "gdlcrp07" and "fdlcrp07"
// animations), otherwise at once, the golfer turning to the front when the part changes. The
// replaced asset and this one are kept for FE_CrAP_RestoreLastRemovedAsset. Nothing for an asset
// without a slot (n2E -1).
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
    if (FE_CrAP_TryClubSwappingAsset(pAsset)) {
        fn_8008EABC(0);
        Character_RequestClothesUpdateFE(lbl_80281EE0->pB4->n10);
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
        FE_CrAP_SaveClubSkinChoices();
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
        FE_CrAP_ApplyAssetParts(pAsset, pSkin);
        FE_CrAP_ApplyAssetSets(pAsset, pSkin);
        Character_RequestClothesUpdateFE(lbl_80281EE0->pB4->n10);
        FE_CrAP_SaveBodySkinChoices();
        if (fn_8008E9A8() != 0) {
            fn_8008E8D0(0);
        }
    }
    if (strcmp(FE_CrAP_GetStringFromTable(pAsset->n112), "") == 0
        || strcmp(FE_CrAP_GetStringFromTable(pAsset->n112), "0") == 0 ||
        !FE_CrAP_IsFadeOutCategory(nPart)) {
        fn_8008E944(0, 0.0f);
        if (fn_8008E6BC() == NULL || strcmp(fn_8008E6BC(), FE_CrAP_GetStringFromTable(pAsset->n112)) != 0) {
            fn_8008E468(FE_CrAP_GetStringFromTable(pAsset->n112), FE_CrAP_GetStringFromTable(pAsset->n114),
                        1);
        } else {
            fn_8008E818(1);
        }
        if (nPart != fn_8008EB04()) {
            FE_SetCrapRotation(1, 0.0f);
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

// Put choice i under entry b of a part's list on the golfer being edited: part 13 toggles a custom
// animation (and is noted for FE_CrAP_RestoreLastRemovedAsset), part 17 a logo (sTurnOnLogo), part
// 18 a slider (sApplySlider), the others go through FE_CrAP_TurnOnAsset. Nothing without a menu
// golfer.
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
        sApplySlider(pAsset);
    } else if (pAsset != NULL) {
        FE_CrAP_TurnOnAsset(pAsset);
    }
}

// Undo the last FE_CrAP_TurnOnAsset or part 13 FE_CrAP_TurnOnPart: put back the asset it replaced
// (lbl_802816E8, with the golfer's animations off), or when it replaced none take the new one
// (lbl_802816EC) off again, a part 13 animation coming out of its list. Clears both.
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

// The parts whose new asset FE_CrAP_TurnOnAsset shows under its animation with the texture swap
// delayed (fn_8008E944): headwear, shirts, pants and shorts (0..2), shoes and eyewear (7, 8), part
// 12, watches and jewelry, miscellaneous (19, 20). TW07's version also takes the asset.
u8 FE_CrAP_IsFadeOutCategory(int nPart) {
    if ((u32)nPart <= 2 || (u32)(nPart - 7) <= 1 || nPart == 12 || nPart == 19 || nPart == 20) {
        return 1;
    }
    return 0;
}

// How many choices entry b of a part's list has: the part's assets offered for the current gender
// that fit the entry's category (all of them for an "All ..." entry). Also written to lbl_8028247C,
// which nothing reads.
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

// The category of entry n of a part's list, as the offset of its name in 'CR_S': the part's
// categories in the order its assets offered for the current gender list them, after the "All ..."
// entry when the part has one (-1 for that entry; 0x40: no such entry, or 64 categories found).
// Also written to lbl_80282478, which nothing reads.
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

// The entry of a part's list that shows category nCategory (-1: none), the reverse of
// FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex. For a part with an "All ..." entry,
// category j > 0 is entry j + 1 but the first category gives 0, the "All ..." entry.
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

// Copy the name of entry n of a part's list into pDst: the "All ..." name for entry 0 of a part
// that has one, else its category's name from 'CR_S'. 0 when the names are not loaded, pDst is NULL
// or there is no such entry.
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

// The database's asset number nAsset (no range check); FE_CrAP_GetAssetIndexFromAsset is the
// reverse.
CrAPAsset* FE_CrAP_GetAssetFromAssetIndex(int nAsset) {
    return &lbl_80282460->pAssets[nAsset];
}

// The number of pAsset, an entry of the database's asset array (the reverse of
// FE_CrAP_GetAssetFromAssetIndex).
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

// Have the stream loader hand the 'CR_A' (assets) and 'CR_S' (names) objects to
// FE_CrAP_LoadAssetsFromStream and FE_CrAP_LoadStringsFromStream as they load.
void FE_CrAP_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('CR_A', FE_CrAP_LoadAssetsFromStream);
    Stream_RegisterLoadChunkCallback('CR_S', FE_CrAP_LoadStringsFromStream);
}

// Note each part's first asset in lbl_80282474 (0 when the part has none); the list functions start
// their scans there (FE_CrAP_GetFirstCategoryAssetID).
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

// The 'CR_A' stream handler: the object's data becomes the asset array (0x118 bytes each),
// byte-swapped; the object is kept for FE_CrAP_CloseModule to free, and FE_CrAP_PostAssetsLoad
// runs.
void FE_CrAP_LoadAssetsFromStream(UStreamObject* pObject) {
    if (pObject != NULL) {
        lbl_80282460->pAssets = (CrAPAsset*)pObject->pData;
        lbl_80282460->nAssets = pObject->uSize / sizeof(CrAPAsset);
        CrAPAssetsByteSwap();
        lbl_80282464 = pObject;
        FE_CrAP_PostAssetsLoad();
    }
}

// The 'CR_S' stream handler: the object's data becomes the names the assets use (categories,
// colours, animations, camera shots, unlock texts); the object is kept for FE_CrAP_CloseModule to
// free.
void FE_CrAP_LoadStringsFromStream(UStreamObject* pObject) {
    if (pObject != NULL) {
        lbl_80282460->pStrings = (char*)pObject->pData;
        lbl_80282460->uStringsSize = pObject->uSize;
        FE_CrAP_PostStringsLoad();
        lbl_80282468 = pObject;
    }
}

// With the assets loaded: index the parts and pick the day's random assets.
void FE_CrAP_PostAssetsLoad(void) {
    FE_CrAP_SetupFirstAssetIDs();
    fn_80077B78();
}

// The name of choice i under entry b of a part's list, as the part picker shows it (NULL: no such
// choice).
char* FE_CrAP_GetPartName(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return NULL;
    }
    return pAsset->szName;
}

// The first of the three colour ids of choice i under entry b of a part's list (n44; TW06 color1);
// 0 when there is no such choice.
s16 FE_CrAP_GetPartColor1(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n44;
}

// The second colour id of choice i under entry b of a part's list (n46); 0 when there is no such
// choice.
s16 FE_CrAP_GetPartColor2(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return 0;
    }
    return pAsset->n46;
}

// The third colour id of choice i under entry b of a part's list (n48); 0 when there is no such
// choice.
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

// The price of choice i under entry b of a part's list (n30; -1: no such choice). Selling it back
// returns a quarter of it.
s32 FE_CrAP_GetPartRetailPrice(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n30;
}

// The sale price of choice i under entry b of a part's list (n34; -1: no such choice).
s32 FE_CrAP_GetPartSalePrice(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n34;
}

// The level of choice i under entry b of a part's list (n38; -1: no such choice); the assets of
// level 0 are owned from the start.
s32 FE_CrAP_GetPartLevel(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->n38;
}

// The first attribute choice i under entry b of a part's list raises (-1: none;
// FE_CrAP_GetPartAttributeUpgrade1ByAssetID). The choice must exist: for a missing one the index -1
// is used unchecked (FE_CrAPMessages.c checks first).
int FE_CrAP_GetPartAttributeUpgrade1(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeUpgrade1ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

// The tier choice i under entry b of a part's list raises its first attribute to; the choice must
// exist, as for FE_CrAP_GetPartAttributeUpgrade1.
int FE_CrAP_GetPartAttributeModifier1(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeModifier1ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

// The second attribute choice i under entry b of a part's list raises (-1: none); the choice must
// exist, as for FE_CrAP_GetPartAttributeUpgrade1.
int FE_CrAP_GetPartAttributeUpgrade2(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeUpgrade2ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

// The tier choice i under entry b of a part's list raises its second attribute to; the choice must
// exist, as for FE_CrAP_GetPartAttributeUpgrade1.
int FE_CrAP_GetPartAttributeModifier2(s16 nPart, int b, int i) {
    return FE_CrAP_GetPartAttributeModifier2ByAssetID(
            FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i));
}

// The first attribute an asset raises (-1: none); an asset of lock kind 28 gives that of the asset
// its nLock names (sGetLinkedAssetID).
int FE_CrAP_GetPartAttributeUpgrade1ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nAttrA;
}

// The tier an asset raises its first attribute to; lock kind 28 as in
// FE_CrAP_GetPartAttributeUpgrade1ByAssetID.
int FE_CrAP_GetPartAttributeModifier1ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nTierA;
}

// The second attribute an asset raises (-1: none); lock kind 28 as in
// FE_CrAP_GetPartAttributeUpgrade1ByAssetID.
int FE_CrAP_GetPartAttributeUpgrade2ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nAttrB;
}

// The tier an asset raises its second attribute to; lock kind 28 as in
// FE_CrAP_GetPartAttributeUpgrade1ByAssetID.
int FE_CrAP_GetPartAttributeModifier2ByAssetID(int nAsset) {
    nAsset = sGetLinkedAssetID(nAsset);
    return lbl_80282460->pAssets[nAsset].nTierB;
}

// How choice i under entry b of a part's list is unlocked: its lock kind (2: a Game Boy Advance
// link, 12: an EA Sports Bio level, 17: an event's reward; 28: it shares another asset's attributes
// instead), -1 when there is no such choice. fn_80078008 tests the kinds.
s8 FE_CrAP_GetPartGMLockID(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->nLockKind;
}

// The number that goes with the lock kind of choice i under entry b of a part's list (the bio
// level, the event, or for kind 28 the linked asset); -1 when there is no such choice.
s16 FE_CrAP_GetPartGMLockVal(s16 nPart, int b, int i) {
    CrAPAsset* pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (pAsset == NULL) {
        return -1;
    }
    return pAsset->nLock;
}

// An asset's lock kind (see FE_CrAP_GetPartGMLockID); -1 when nAsset is not an asset.
s8 FE_CrAP_GetPartGMLockIDByAssetNum(int nAsset) {
    if (nAsset < 0 || nAsset >= lbl_80282460->nAssets) {
        return -1;
    }
    return lbl_80282460->pAssets[nAsset].nLockKind;
}

// The number that goes with an asset's lock kind (see FE_CrAP_GetPartGMLockVal); -1 when nAsset is
// not an asset.
s16 FE_CrAP_GetPartGMLockValByAssetNum(int nAsset) {
    if (nAsset < 0 || nAsset >= lbl_80282460->nAssets) {
        return -1;
    }
    return lbl_80282460->pAssets[nAsset].nLock;
}

// Value n (0..5) of a part's choice i under its list entry b: the asset's a4A[n]
// (FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum), or -1 when there is no such choice.
// The UI reads it through FE_CrAPMessages.c fn_80107828. What a4A holds is not known; it has six
// entries, like the asset's six colours.
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
void FE_CrAP_GetPartColorRGBA(s16 nPart, int b, int i, int n, u8* pColor) {
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
        pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[0];
    } else if (stricmp(szCategory, "Fairway Woods") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[1];
    } else if (stricmp(szCategory, "Iron Sets") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[3];
    } else if (stricmp(szCategory, "Wedge Sets") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[5];
    } else if (stricmp(szCategory, "Putters") == 0) {
        pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[2];
    } else if (stricmp(szCategory, "shafts") == 0) {
        fn_800CB8F0(&pAsset->aSet[0], szSet);
        if (stricmp(szSet, "fwd_shaft") == 0) {
            pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[0];
        } else {
            pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[3];
        }
    } else if (stricmp(szCategory, "grips") == 0) {
        fn_800CB8F0(&pAsset->aSet[0], szSet);
        if (stricmp(szSet, "fwd_grip") == 0) {
            pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[0];
        } else {
            pSkin = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[3];
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
            pOption = (f32*)SkinPart_GetSetOptionData(pSkin, nSet, nVariant, nOption);
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

// Copy the name of a part's choice i's first variant (its animation name, for part 13) into pName.
void FE_CrAP_GetPartVariantName(s16 nPart, int b, int i, char* pName) {
    FE_CrAP_GetAssetVariantName(FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i),
                                pName);
}

// Copy the name of the asset's first variant id. The ids are byte-swapped for the lookup and swapped
// back after.
void FE_CrAP_GetAssetVariantName(CrAPAsset* pAsset, char* pName) {
    u64 nId;
    u8* pSrc;

    pSrc = (u8*)pAsset->aVariant;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)pAsset->aVariant, sizeof(pAsset->aVariant), sizeof(u64));
    nId = pAsset->aVariant[0];
    fn_800CB868(&nId, pName);
    pSrc = (u8*)pAsset->aVariant;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)pAsset->aVariant, sizeof(pAsset->aVariant), sizeof(u64));
}

// The number of assets in the database (nAssets): asset numbers run from 0 to one less.
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

// The database is allocated (FE_CrAP_InitModule has run); its assets may still be on their way.
u8 FE_CrAP_IsCrAPDBLoaded(void) {
    return lbl_80282460 != NULL;
}

// How many different choices fit a part's entry b: assets of the same category and the same first
// colour count once.
int FE_CrAP_GetNumberUniqueGeometries(s16 nPart, int b) {
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
void CrAPAssetsByteSwap(void) {
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

// Run when the 'CR_S' names have loaded, as FE_CrAP_PostAssetsLoad is for the assets; empty in this
// build.
void FE_CrAP_PostStringsLoad(void) {
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
            nSet = SkinPart_FindSetByName(pSkin, "wire");
            nVariant = SkinPart_FindSetVariantByName(pSkin, nSet, "nowire");
            if (nSet >= 0 && nVariant >= 0) {
                SkinPart_ChooseBodySet(lbl_80281EE0->pB4->pChar, nSet, nVariant, 0);
            }
        } else if (nPart == 1) {
            nSet = SkinPart_FindSetByName(pSkin, "hands");
            nVariant = SkinPart_FindSetVariantByName(pSkin, nSet, "nowire");
            if (nSet >= 0 && nVariant >= 0) {
                SkinPart_ChooseBodySet(lbl_80281EE0->pB4->pChar, nSet, nVariant, 0);
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

// An asset of gender n is offered for the golfer being created: n is the current gender
// (FE_CrAP_SetCurrentGender) or 2, either.
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

// Copy the colour name at offset nOffset of the 'CR_S' names into pDst ("NONE" gives ""). 0 when
// the names are not loaded, pDst is NULL or nOffset is -1.
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

// The string at offset nCategory of the 'CR_S' names (any of the names the assets use: categories,
// colours, animations, camera shots, unlock texts); NULL when they are not loaded or the offset is
// -1.
char* FE_CrAP_GetStringFromTable(int nCategory) {
    if (lbl_80282460->pStrings == NULL) {
        return NULL;
    }
    if (nCategory == -1) {
        return NULL;
    }
    return lbl_80282460->pStrings + nCategory;
}

// Copy how choice i under entry b of a part's list is unlocked into pDst: "Game Boy Advance Link
// Required" for lock kind 2, otherwise its unlock text from 'CR_S' (n110; pDst is left as it is
// when it has none). 0 when the names are not loaded, pDst is NULL or there is no such choice.
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

// A club asset (a category FE_CrAP_GetClubSkinsForAsset knows): put it on all six club skins (see the EA bug
// below), not only the ones its category uses. 0 when it is not a club asset or the golfer has no
// club skins.
u8 FE_CrAP_TryClubSwappingAsset(CrAPAsset* pAsset) {
    Skin* apSkins[6];
    int nSkins;
    int i;

    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL ||
        lbl_80281EE0->pB4->pChar->pClubSet == NULL) {
        return 0;
    }
    nSkins = FE_CrAP_GetClubSkinsForAsset(pAsset, apSkins);
    if (nSkins <= 0) {
        return 0;
    }
    for (i = 0; i < nSkins; i++) {
        // EA bug: the inner loop reuses i, so every club skin gets the asset once, whatever nSkins
        // is, and apSkins is never read
        for (i = 0; i < 6; i++) {
            FE_CrAP_ApplyAssetParts(pAsset, lbl_80281EE0->pB4->pChar->pClubSet->apSkins[i]);
            FE_CrAP_ApplyAssetSetsToClubSkin(pAsset, lbl_80281EE0->pB4->pChar->pClubSet->apSkins[i]);
        }
    }
    return 1;
}

// Put the club skins an asset's category goes on into apSkins; how many (0: not a club category, or
// no menu golfer with club skins). Of the six skins (0 drivers, 1 fairway woods, 2 putters, 3 and 4
// irons, 5 wedges), shafts and grips go on all six.
int FE_CrAP_GetClubSkinsForAsset(CrAPAsset* pAsset, Skin** apSkins) {
    char* szCategory = FE_CrAP_GetStringFromTable(pAsset->nCategory);

    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL ||
        lbl_80281EE0->pB4->pChar->pClubSet == NULL || apSkins == NULL) {
        return 0;
    }
    if (stricmp(szCategory, "shafts") == 0 || stricmp(szCategory, "grips") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[0];
        apSkins[1] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[1];
        apSkins[2] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[2];
        apSkins[3] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[3];
        apSkins[4] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[4];
        apSkins[5] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[5];
        return 6;
    }
    if (stricmp(szCategory, "drivers") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[0];
        return 1;
    }
    if (stricmp(szCategory, "Fairway Woods") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[1];
        return 1;
    }
    if (stricmp(szCategory, "Iron Sets") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[3];
        apSkins[1] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[4];
        return 2;
    }
    if (stricmp(szCategory, "Wedge Sets") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[5];
        return 1;
    }
    if (stricmp(szCategory, "Putters") == 0) {
        apSkins[0] = lbl_80281EE0->pB4->pChar->pClubSet->apSkins[2];
        return 1;
    }
    return 0;
}

// A part 12 asset of the category "balls": pass its ball's name (its first set variant) to
// fn_8008E960 and store the ball's index (fn_800484F4) in the profile's nGolferOutfit; 1 when done.
// 0 for any other asset, or when there is no menu golfer with club skins.
u8 FE_CrAP_TryBallSwappingAsset(CrAPAsset* pAsset) {
    char szName[16];                    // the size is unknown (the frame allows up to 20)
    char* szCategory;
    s8 nBall;

    if (pAsset->nPart != 12) {
        return 0;
    }
    szCategory = FE_CrAP_GetStringFromTable(pAsset->nCategory);
    if (lbl_80281EE0->pB4 == NULL || lbl_80281EE0->pB4->pChar == NULL ||
        lbl_80281EE0->pB4->pChar->pClubSet == NULL) {
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
void FE_CrAP_ApplyAssetParts(CrAPAsset* pAsset, Skin* pSkin) {
    int i;
    s32 nPart;
    s32 nVariant;

    for (i = 0; i < 4; i++) {
        nPart = SkinPart_FindPart(pSkin, pAsset->aPart[i]);
        nVariant = SkinPart_FindPartVariant(pSkin, nPart, pAsset->aVariant[i]);
        if (nPart >= 0 && nVariant >= 0) {
            SkinPart_ChoosePartVariant(pSkin, nPart, nVariant);
        }
    }
}

// Put the asset's sets on a skin: each of its four sets the skin has gets the asset's variant and
// option; SkinPart_DropSetVariantTextures gets them too when SkinPart_GetChangeAllCopies is set.
void FE_CrAP_ApplyAssetSets(CrAPAsset* pAsset, Skin* pSkin) {
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
                SkinPart_DropSetVariantTextures(pSkin, nSet, nVariant, nOption,
                            lbl_80281EE0->pB4->pChar->apDynTex[lbl_80281EE0->pB4->pChar->nCurDynTex]);
            }
        }
    }
}

// FE_CrAP_ApplyAssetSets for a club skin: a set that has a "DefaultL" variant gets that one instead
// when the profile's choices.n113 is 1.
void FE_CrAP_ApplyAssetSetsToClubSkin(CrAPAsset* pAsset, Skin* pSkin) {
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
                SkinPart_DropSetVariantTextures(pSkin, nSet, nVariant, nOption,
                            lbl_80281EE0->pB4->pChar->apDynTex[lbl_80281EE0->pB4->pChar->nCurDynTex]);
            }
        }
    }
}

// Take the asset's parts off a skin: each goes back to variant 0.
void FE_CrAP_RemoveAssetParts(CrAPAsset* pAsset, Skin* pSkin) {
    int i;
    s32 nPart;

    if (pAsset != NULL) {
        for (i = 0; i < 4; i++) {
            nPart = SkinPart_FindPart(pSkin, pAsset->aPart[i]);
            if (nPart >= 0) {
                SkinPart_ChoosePartVariant(pSkin, nPart, 0);
            }
        }
    }
}

// Take the asset's sets off a skin: each goes back to its "Defaults" variant (or variant 0 when it
// has none). Nothing for a NULL asset.
void FE_CrAP_RemoveAssetSets(CrAPAsset* pAsset, Skin* pSkin) {
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

// How many of the assets in the profile's 53 slots carry sponsor n's brand (n2C; lbl_801935C8 names
// the brands).
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

// How many assets offered for the current gender unlock with lock kind nKind and number nLock:
// EASportsBio.c asks how many a bio level (kind 12) unlocks.
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

// Copy the names of the first three assets offered for the current gender with lock kind nKind and
// number nLock into szFirst, szSecond, szThird; how many there are, up to 3. EventInfo.c lists an
// event's rewards (kind 17) this way.
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

// The lowest lock number above nAfter among the assets of lock kind nKind, whatever their gender
// (-1: none): EASportsBio.c's next bio level that unlocks something.
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

// Fill the sponsorship records (lbl_80282470, their count in lbl_8028246C): for each of the
// profile's 11 a1054C entries that is on (b), one record per worn asset of its sponsor (n), holding
// the sponsor, fn_800F0304 of the entry (TW07 calls it the cash bonus) and the asset's name.
// Returns how many records there are.
s32 FE_CrAP_CollectSponsorshipItems(void) {
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

// Copy sponsorship record n (FE_CrAP_CollectSponsorshipItems): its sponsor, its bonus and the
// asset's name.
void FE_CrAP_GetSponsorshipItemInfo(int n, s16* pN0, s32* pN4, char* pDst) {
    *pN0 = lbl_80282470[n].n0;
    *pN4 = lbl_80282470[n].n4;
    strcpy(pDst, lbl_80282470[n].sz8);
}

// Copy sponsor n's brand name ("adidas", "Callaway Golf"...) into pDst.
void FE_CrAP_GetSponsorName(s16 n, char* pDst) {
    strcpy(pDst, lbl_801935C8[n]);
}

// Count a part's assets offered for the current gender: the locked ones (fn_80078008), the owned
// ones (aB1CC), those with their aB344 bit set (TW07 counts new ones here) and all of them.
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
            if (BitArray_TestBit(pProfile->aB1CC, i)) {
                *pB1CC += 1;
            }
            if (BitArray_TestBit(pProfile->aB344, i)) {
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

// An asset's level (its n38, as FE_CrAP_GetPartLevelFromAssetIndex returns it); level 0 assets are
// owned from the start. No range check on nAsset.
int FE_CrAP_GetLevelFromAssetID(int nAsset) {
    return lbl_80282460->pAssets[nAsset].n38;
}

// Copy the name of an asset's category into pDst.
void FE_CrAP_GetSubcategoryNameFromAssetID(int nAsset, char* pDst) {
    strcpy(pDst, lbl_80282460->pStrings + lbl_80282460->pAssets[nAsset].nCategory);
}

// Copy an asset's name into pDst.
void FE_CrAP_GetAssetNameFromAssetID(int nAsset, char* pDst) {
    strcpy(pDst, lbl_80282460->pAssets[nAsset].szName);
}

// Whether picking the equipped asset again takes it off (FE_CrAPMessages): yes for animations (part
// 13) and the assets of slots 2 and 4..14, no for the other slots.
u8 FE_CrAP_IsAssetRemovable(int nAsset) {
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

// Send message nMsg with the value nA to the front end's handler (lbl_80281F1C), when there is a
// front end. The EA Sports Bio screens (EASportsBio.c) use this file's FE_SendHint senders; a
// string value goes as a MsgString (FE_SetHintString).
void FE_SendHintInt(int nMsg, s32 nA) {
    MsgArg arg;

    if (lbl_80281F1C != NULL) {
        arg.i = nA;
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, &arg);
    }
}

// Send message nMsg with the value nA and the string szB to the front end's handler (lbl_80281F1C),
// when there is a front end.
void FE_SendHintIntString(int nMsg, s32 nA, char* szB) {
    MsgString str;
    MsgArg args[2];

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].p = &str;
        FE_SetHintString(&args[1], szB);
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 2, args);
    }
}

// Send message nMsg with the value nA, the string szB and the value nC to the front end's handler
// (lbl_80281F1C), when there is a front end.
void FE_SendHintIntStringInt(int nMsg, s32 nA, char* szB, s32 nC) {
    MsgArg args[3];
    MsgString str;

    if (lbl_80281F1C != NULL) {
        args[0].i = nA;
        args[1].p = &str;
        FE_SetHintString(&args[1], szB);
        args[2].i = nC;
        UISProcessHint(lbl_80281F1C->pHandler, nMsg, 3, args);
    }
}

// Point the string value pArg holds at sz.
int FE_SetHintString(MsgArg* pArg, char* sz) {
    ((MsgString*)pArg->p)->pStr = sz;
    ((MsgString*)pArg->p)->nLen = strlen(sz);
    return 0;
}

// Send message nMsg with the string sz to the front end's handler (note the string comes first); -1
// when there is no front end, else 0.
int FE_SendHintString(char* sz, int nMsg) {
    MsgArg arg;
    MsgString str;

    if (lbl_80281F1C == NULL) {
        return -1;
    }
    arg.p = &str;
    FE_SetHintString(&arg, sz);
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, &arg);
    return 0;
}

// Send message nMsg with the six values nA..nF and the float fG to the front end's handler
// (lbl_80281F1C), when there is a front end.
void FE_SendHint7Args(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, f32 fG) {
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

// Send message nMsg with the ten values nA..nJ to the front end's handler (lbl_80281F1C), when
// there is a front end.
void FE_SendHint10Args(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, s32 nG, s32 nH, s32 nI,
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

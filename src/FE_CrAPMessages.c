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

void FE_CrAP_SetTriggerAnims(u8 b);                 // FE_CrAPDB.c: set the database's b14
void FE_CrAP_UnequipSlot(s16 nSlot);            // FE_CrAPDB.c
void Gaud_PlayUISound(s32 n);
u8   IsLeapYear(u32 nYear);            // Calendar.c: a leap year (1900 counts as one)

// ---- sweep code (not yet cleaned up) ----

// Menu message 381: how many choices entry pArgs[1] of part pArgs[0] offers
// (FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex).
void GM_vGetNumCrAPItems(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(pArgs[0].i, pArgs[1].i);
}

// ---- end of sweep code ----

// Menu message 382: value pArgs[3] (the asset's a4A[n], FE_CrAP_GetPartValue) of choice pArgs[2]
// under entry pArgs[1] of part pArgs[0]; 0 when there is no such choice.
void GM_vGetCrAPItemValue(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
    int i = pArgs[2].i;
    int n = pArgs[3].i;

    if (i < 0 || i >= nChoices) {
        pResult->i = 0;
    } else {
        pResult->i = FE_CrAP_GetPartValue(nPart, b, i, n);
    }
}

// Menu message 383: colour pArgs[3] of choice pArgs[2] under entry pArgs[1] of part pArgs[0]
// (FE_CrAP_GetPartColorRGBA) as red, green, blue and alpha 0..255 into *pArgs[4..7]; opaque red
// when there is no such choice.
void GM_vGetCrAPItemColor(MsgArg* pArgs, MsgArg* pResult) {
    u8 aColor[4];
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
    int i = pArgs[2].i;
    int n = pArgs[3].i;

    if (i < 0 || i >= nChoices || nChoices == 0) {
        *(s32*)pArgs[4].p = 0xFF;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
        *(s32*)pArgs[7].p = 0xFF;
    } else {
        FE_CrAP_GetPartColorRGBA(nPart, b, i, n, aColor);
        *(s32*)pArgs[4].p = aColor[0];
        *(s32*)pArgs[5].p = aColor[1];
        *(s32*)pArgs[6].p = aColor[2];
        *(s32*)pArgs[7].p = aColor[3];
    }
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 404: does nothing in this build.
void GM_vCrAPMessage404_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 405: does nothing in this build.
void GM_vCrAPMessage405_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// ---- end of sweep code ----

// Menu message 406: set slider pArgs[0] (0..25) of the created golfer to pArgs[1]. When the menu
// golfer has a character, its body is reshaped from all 26 sliders and the slider's asset (part
// 18's choice pArgs[0]) is shown on it.
void GM_vSetCRAPSlider(MsgArg* pArgs, MsgArg* pResult) {
    SkinChoices* pChoices = &FE_GetCurrentProfile()->choices;
    int n = pArgs[0].i;
    Character* pChar;

    pChoices->a9B4[n] = pArgs[1].i;
    if (gpCrAPState->pB4 == NULL) {
        return;
    }
    pChar = gpCrAPState->pB4->pChar;
    if (pChar == NULL) {
        return;
    }
    CharSlider_UpdateCharacterBasedOnSliderValues(pChar->pSliderDefs, pChar->pModel, pChar->pSkin, 26,
                                                  pChoices->a9B4,
                &pChar->morphBlend);
    FE_CrAP_TurnOnPart(18, 0, n);
}

// Menu message 407: which choice the created golfer wears for part pArgs[0]. The part names its
// asset slot (part 12 names one per entry pArgs[1]: 0..7 to slots 44..51, with 6 and 7 swapped);
// the result is the slot's asset's place in its entry's list
// (FE_CrAP_GetCategorySubcategoryAndEntryNumFromAssetID), 0 when the part has no slot here or the
// slot is empty.
void GM_vGetCRAPItem(MsgArg* pArgs, MsgArg* pResult) {
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
        n = FE_CrAP_GetEquippedAsset(nSlot);
        if (n < 0) {
            n = 0;
        } else {
            FE_CrAP_GetCategorySubcategoryAndEntryNumFromAssetID(n, &nPart, &nEntry, &n);
        }
        pResult->i = n;
    }
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 408: does nothing in this build.
void GM_vCrAPMessage408_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// ---- end of sweep code ----

// Menu message 409: slider pArgs[0] of the created golfer (the profile's choices.a9B4, read
// signed).
void GM_vGetCRAPSlider(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    pResult->i = (s8)pProfile->choices.a9B4[pArgs[0].i];
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 410: how many different choices entry pArgs[1] of part pArgs[0] offers, assets of
// the same category and first colour counted once (FE_CrAP_GetNumberUniqueGeometries).
void GM_vGetNumCrAPGeometries(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetNumberUniqueGeometries(pArgs[0].i, pArgs[1].i);
}

// Menu message 411: always answers 0 in this build.
void GM_vCrAPMessage411_Zero(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// ---- end of sweep code ----

// Menu message 412: choice pArgs[2] under entry pArgs[1] of part pArgs[0]: its name into the string
// pArgs[3], and its three colour ids, sponsor and level into *pArgs[4..8]. With no such choice the
// name is "Coming Soon" in the session's 0x4000 mode, else "No Entry Found", and the five numbers
// are 0.
void GM_vGetCrAPItemInfo(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
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
        strcpy(((MsgString*)pArgs[3].p)->pStr, FE_CrAP_GetPartName(nPart, b, i));
        *(s32*)pArgs[4].p = FE_CrAP_GetPartColor1(nPart, b, i);
        *(s32*)pArgs[5].p = FE_CrAP_GetPartColor2(nPart, b, i);
        *(s32*)pArgs[6].p = FE_CrAP_GetPartColor3(nPart, b, i);
        *(s32*)pArgs[7].p = FE_CrAP_GetPartSponsor(nPart, b, i);
        *(s32*)pArgs[8].p = FE_CrAP_GetPartLevel(nPart, b, i);
    }
}

// Menu message 451: the two attributes choice pArgs[2] under entry pArgs[1] of part pArgs[0] raises
// and the tier it raises each to: *pArgs[3] and *pArgs[4] the first, *pArgs[5] and *pArgs[6] the
// second (-1 attribute: none); all 0 when there is no such choice.
void GM_vGetCrAPItemAttributes(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        *(s32*)pArgs[3].p = 0;
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
    } else {
        *(s32*)pArgs[3].p = FE_CrAP_GetPartAttributeUpgrade1(nPart, b, i);
        *(s32*)pArgs[4].p = FE_CrAP_GetPartAttributeModifier1(nPart, b, i);
        *(s32*)pArgs[5].p = FE_CrAP_GetPartAttributeUpgrade2(nPart, b, i);
        *(s32*)pArgs[6].p = FE_CrAP_GetPartAttributeModifier2(nPart, b, i);
    }
}

// Menu message 452: choice pArgs[2] under entry pArgs[1] of part pArgs[0]: its price, its sale
// price, its lock kind and the number that goes with it into *pArgs[3..6]; all 0 when there is no
// such choice.
void GM_vGetCrAPItemPriceAndLock(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        *(s32*)pArgs[3].p = 0;
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
    } else {
        *(s32*)pArgs[3].p = FE_CrAP_GetPartRetailPrice(nPart, b, i);
        *(s32*)pArgs[4].p = FE_CrAP_GetPartSalePrice(nPart, b, i);
        *(s32*)pArgs[5].p = FE_CrAP_GetPartGMLockID(nPart, b, i);
        *(s32*)pArgs[6].p = FE_CrAP_GetPartGMLockVal(nPart, b, i);
    }
}

// Menu message 413: pick choice pArgs[2] under entry pArgs[1] of part pArgs[0] on the created
// golfer: a worn asset that can come off (FE_CrAP_IsAssetEquipped, FE_CrAP_IsAssetRemovable) is
// taken off, anything else is put on (part 17's logos are only put on), then the golfer's equipment
// tiers are worked out again (fn_8007873C).
void GM_vCRAPTryOnItem(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    int nAsset;

    if (nPart == 17) {
        FE_CrAP_TurnOnPart(nPart, b, i);
        return;
    }
    nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (nAsset != -1) {
        if (FE_CrAP_IsAssetEquipped(FE_CrAP_GetAssetFromAssetIndex(nAsset))
            && FE_CrAP_IsAssetRemovable(nAsset)) {
            FE_CrAP_TurnOffPart(nPart, b, i);
        } else {
            FE_CrAP_TurnOnPart(nPart, b, i);
        }
        fn_8007873C(pProfile);
    }
}

// Menu message 760: GM_vCRAPTryOnItem without working out the equipment tiers again: choice
// pArgs[2] under entry pArgs[1] of part pArgs[0] is taken off when worn and removable, else put on.
void GM_vPreviewItem(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart;
    int b;
    int i;
    int nAsset;

    FE_GetCurrentProfile();
    nPart = pArgs[0].i;
    b = pArgs[1].i;
    i = pArgs[2].i;
    if (nPart == 17) {
        FE_CrAP_TurnOnPart(nPart, b, i);
        return;
    }
    nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
    if (nAsset != -1) {
        if (FE_CrAP_IsAssetEquipped(FE_CrAP_GetAssetFromAssetIndex(nAsset))
            && FE_CrAP_IsAssetRemovable(nAsset)) {
            FE_CrAP_TurnOffPart(nPart, b, i);
            return;
        }
        FE_CrAP_TurnOnPart(nPart, b, i);
    }
}

// Menu message 441: how many entries part pArgs[0]'s list has
// (FE_CrAP_GetNumberOfSubcategoryIndicesForCategory).
void GM_vGetNumCrAPSubcategories(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetNumberOfSubcategoryIndicesForCategory(pArgs[0].i);
}

// ---- end of sweep code ----

// Menu message 453: set one of the created golfer's details, chosen by pArgs[0]: 0 the profile's
// name (the string pArgs[1]); 1 its gender (choices.n5A7A, also made the database's current
// gender); 2 its date, pArgs[2] packed as month * 10000 + day * 1000000 + year (see fn_80078604); 3
// left-handed (choices.bLeftHanded). Values come in pArgs[2].
void GM_vSetCrAPGolferInfo(MsgArg* pArgs, MsgArg* pResult) {
    int nMonth;
    int nDay;
    int nYear;
    SaveProfile* pProfile = FE_GetCurrentProfile();

    switch (pArgs[0].i) {
    case 0:
        fn_80057ED0(pProfile, ((MsgString*)pArgs[1].p)->pStr);
        break;
    case 1:
        pProfile->choices.n5A7A = pArgs[2].i;
        FE_CrAP_SetCurrentGender(pProfile->choices.n5A7A);
        break;
    case 2:
        fn_80078620(pArgs[2].i, &nMonth, &nDay, &nYear);
        pProfile->nDateDay = nDay;
        pProfile->nDateMonth = nMonth;
        pProfile->nDateYear = nYear;
        break;
    case 3:
        pProfile->choices.bLeftHanded = pArgs[2].i;
        break;
    }
}

// Menu message 454: read back one of the details GM_vSetCrAPGolferInfo sets, chosen by pArgs[0]: 0
// the name into the string pArgs[1]; 1 the gender, 2 the packed date, 3 left-handed into *pArgs[2].
void GM_vGetCrAPGolferInfo(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

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
        *(s32*)pArgs[2].p = pProfile->choices.bLeftHanded;
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 457: from now on the menus work on their own copy of the profile (pArgs[0] 1) or on
// the player slot's saved profile (0); FEProfile.bCopy, read by FE_GetCurrentProfile.
void GM_vSetUseProfileCopy(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->bCopy = pArgs[0].i;
}

// ---- end of sweep code ----

// Menu message 460: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is still locked for the
// profile (fn_80078008); never in the session's 0x4000 mode.
void GM_vIsCrAPItemLocked(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;

    if (gSession.uFlags & 0x4000) {
        pResult->i = 0;
    } else {
        pResult->i = fn_80078008(FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b,
                i), pProfile);
    }
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 461: the profile owns choice pArgs[2] under entry pArgs[1] of part pArgs[0]: its bit
// in aB1CC, set when it is bought (GM_vPurchaseCrAPItem) and from the start for level 0 assets.
void GM_vIsCrAPItemOwned(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = BitArray_TestBit(pProfile->aB1CC, nAsset);
}

// ---- end of sweep code ----

// Menu message 462: buy choice pArgs[2] under entry pArgs[1] of part pArgs[0] for pArgs[3]: the
// price comes off the profile's money (no check that it is there), the asset's owned bit (aB1CC) is
// set, the created golfer puts it on and its equipment tiers are worked out again (fn_8007873C).
void GM_vPurchaseCrAPItem(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    s32 nPrice = pArgs[3].i;
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);

    pProfile->nCurrentCash -= nPrice;
    BitArray_SetBit(pProfile->aB1CC, nAsset);
    FE_CrAP_TurnOnPart(nPart, b, i);
    fn_8007873C(pProfile);
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 470: 1 when one of the profile's asset slots holds an asset of part pArgs[0]
// (FE_CrAP_GetFirstEquippedIndexForCategory), else 0.
void GM_vIsCrAPCategoryWorn(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetCurrentProfile();
    if (FE_CrAP_GetFirstEquippedIndexForCategory(pArgs[0].i) >= 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// ---- end of sweep code ----

// Menu message 474: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is in use: for part 13
// (custom animations) its animation is in the profile's list pArgs[1] (fn_800587A8), for other
// parts it is the asset in its slot (FE_CrAP_IsItemEquipped).
void GM_vIsCrAPItemEquipped(MsgArg* pArgs, MsgArg* pResult) {
    char szName[16];                    // the size is unknown (the frame allows up to 20)
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;

    FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, b);
    if (nPart == 13) {
        FE_CrAP_GetPartVariantName(nPart, b, i, szName);
        pResult->i = fn_800587A8(pProfile, b, szName);
    } else {
        pResult->i = FE_CrAP_IsItemEquipped(nPart, b, i);
    }
}

// Menu message 475: the day's sale items in sale category pArgs[0] (-1 clothes, -2 accessories, -3
// clubs and balls; see fn_80077C1C) for the current gender: the part they are of into *pArgs[1],
// their five choice numbers into *pArgs[2..6] (-1: none). GM_vGetCrAPSaleSubcategories gives their
// entries.
void GM_vGetCrAPSaleItems(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = FE_CrAP_GetCurrentGender();

    *(s32*)pArgs[1].p = lbl_80281ED4->aKind[b][nCategory];
    *(s32*)pArgs[2].p = lbl_80281ED4->aChoice[b][nCategory][0];
    *(s32*)pArgs[3].p = lbl_80281ED4->aChoice[b][nCategory][1];
    *(s32*)pArgs[4].p = lbl_80281ED4->aChoice[b][nCategory][2];
    *(s32*)pArgs[5].p = lbl_80281ED4->aChoice[b][nCategory][3];
    *(s32*)pArgs[6].p = lbl_80281ED4->aChoice[b][nCategory][4];
}

// Menu message 476: the list entries (EA's subcategories) of the five sale items
// GM_vGetCrAPSaleItems gives for sale category pArgs[0], into *pArgs[1..5] (-1: none).
void GM_vGetCrAPSaleSubcategories(MsgArg* pArgs, MsgArg* pResult) {
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = FE_CrAP_GetCurrentGender();

    *(s32*)pArgs[1].p = lbl_80281ED4->aPart[b][nCategory][0];
    *(s32*)pArgs[2].p = lbl_80281ED4->aPart[b][nCategory][1];
    *(s32*)pArgs[3].p = lbl_80281ED4->aPart[b][nCategory][2];
    *(s32*)pArgs[4].p = lbl_80281ED4->aPart[b][nCategory][3];
    *(s32*)pArgs[5].p = lbl_80281ED4->aPart[b][nCategory][4];
}

// Menu message 477: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is one of the day's sale
// items. Parts 0, 1, 2 and 7 (headwear, shirts, pants, shoes) are sale category -1, parts 8, 19 and
// 20 (eyewear, watches and jewelry, miscellaneous) -2, part 12 -3; other parts are never on sale.
void GM_vIsCrAPItemOnSale(MsgArg* pArgs, MsgArg* pResult) {
    int j;
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    s8 nDb = FE_CrAP_GetCurrentGender();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);
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
        if (nAsset
            == FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(
                    lbl_80281ED4->aKind[nDb][nCategory], lbl_80281ED4->aPart[nDb][nCategory][j],
                                  lbl_80281ED4->aChoice[nDb][nCategory][j])) {
            pResult->i = 1;
            return;
        }
    }
    pResult->i = 0;
}

// ---- sweep code (not yet cleaned up) ----

// Menu message 489: the logo editor works on the profile's logo pArgs[0] (0..4) from now on.
void GM_vSelectLogo(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7C0(pArgs[0].i);
}

// Menu message 490: the logo editor edits a square logo (pArgs[0] 0) or a rectangular one (anything
// else). Only the editor's shape; GM_vSetLogoShape also sets the logo's.
void GM_vSetLogoEditorShape(MsgArg* pArgs, MsgArg* pResult) {
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
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nAssets = FE_CrAP_GetNumEntriesInCrAPDB();
    int i;

    BitArray_ClearArray(aWasLocked, 3000);
    for (i = 0; i < nAssets; i++) {
        if (BitArray_TestBit(pProfile->aAssetLocked, i)) {
            BitArray_SetBit(aWasLocked, i);
        } else {
            BitArray_ClearBit(aWasLocked, i);
        }
    }
    fn_80078680(pProfile);
    for (i = 0; i < nAssets; i++) {
        if (!BitArray_TestBit(pProfile->aAssetLocked, i) && BitArray_TestBit(aWasLocked, i)) {
            if (FE_IsValidCurrentGender(FE_CrAP_GetAssetGender(i))) {
                nUnlocked++;
                BitArray_SetBit(pProfile->aB344, i);
            }
        }
    }
    pResult->i = nUnlocked;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108B10(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = BitArray_TestBit(pProfile->aB344, nAsset);
}

void fn_80108B84(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    if (BitArray_TestBit(pProfile->aB344, nAsset)) {
        BitArray_SetBit(pProfile->aB4BC, nAsset);
    }
}

// ---- end of sweep code ----

// Clear every asset's aB344 and aB4BC bits where both are set.
void fn_80108C00(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nAssets = FE_CrAP_GetNumEntriesInCrAPDB();
    u32 i;

    for (i = 0; i < nAssets; i++) {
        if (BitArray_TestBit(pProfile->aB344, i) && BitArray_TestBit(pProfile->aB4BC, i)) {
            BitArray_ClearBit(pProfile->aB344, i);
            BitArray_ClearBit(pProfile->aB4BC, i);
        }
    }
}

// Whether a controller in any of the four ports holds button bit 24 (held).
void fn_80108CA8(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 4; i++) {
        if (Input_bDoesPadExist(i) && (Input_ReadControlPad(i) & 0x1000000)) {
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

    if (Input_bDoesPadExist(nChan)) {
        if (Input_ReadControlPad(nChan) & 0x80000) {
            *pA = 1;
        } else {
            *pA = 0;
        }
        if (Input_ReadControlPad(nChan) & 0x40000) {
            *pB = 1;
        } else {
            *pB = 0;
        }
        if (Input_ReadControlPad(nChan) & 0x10000) {
            *pC = 1;
        } else {
            *pC = 0;
        }
        if (Input_ReadControlPad(nChan) & 0x20000) {
            *pD = 1;
        } else {
            *pD = 0;
        }
    }
}

// Set or clear bit pArgs[0] of the profile's a10548.
void fn_80108DF4(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 bSet = pArgs[1].i;
    s32 nBit = pArgs[0].i;

    if (bSet) {
        BitArray_SetBit(pProfile->a10548, nBit);
    } else {
        BitArray_ClearBit(pProfile->a10548, nBit);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80108E4C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    pResult->i = BitArray_TestBit(pProfile->a10548, pArgs[0].i);
}

// ---- end of sweep code ----

// The newly unlocked assets (aB344) for the five strings pArgs[0..4]: their names when there are
// at most five, else their categories (the fifth line "And more..." when there are more than five).
void fn_80108E9C(MsgArg* pArgs, MsgArg* pResult) {
    char aNames[5][64];
    char aCategories[5][64];
    char szCategory[64];
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nAssets = FE_CrAP_GetNumEntriesInCrAPDB();
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
        if (BitArray_TestBit(pProfile->aB344, i) && FE_IsValidCurrentGender(FE_CrAP_GetAssetGender(i))) {
            FE_CrAP_GetSubcategoryNameFromAssetID(i, szCategory);
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
                FE_CrAP_GetAssetNameFromAssetID(i, aNames[nNames]);
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
    SaveProfile* pProfile = FE_GetCurrentProfile();
    pResult->i = pProfile->choices.aLogo[pArgs[0].i].b1020;
}

// Keep the edited logo: copy it into the profile's logo fn_8010F7D8.
void fn_80109294(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
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
    FE_CrAP_GetSubCategoryNameForCategoryAndSubcategoryIndex(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr);
}

void fn_80109388(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_GetColorNameFromID(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// ---- end of sweep code ----

// A part's choice i: its unlock text (FE_CrAP_GetUnlockMessageFrom), when there is such a choice.
void fn_801093B4(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
    int i = pArgs[2].i;

    if (i < 0 || i >= nChoices) {
        return;
    }
    FE_CrAP_GetUnlockMessageFrom(nPart, b, i, ((MsgString*)pArgs[3].p)->pStr);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80109430(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80109434(MsgArg* pArgs, MsgArg* pResult) {
    FE_SetCrAPCameraIdleState(pArgs[0].i);
}

void fn_80109458(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)FE_HasGolferCharacter();
}

void fn_8010948C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)FE_IsGolferReady();
}

void fn_801094C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_StreamGetCurrentState() == 1;
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
    SaveProfile* pProfile = FE_GetCurrentProfile();
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
    FE_CrAP_SetTriggerAnims(pArgs[0].i);
}

void fn_80109760(MsgArg* pArgs, MsgArg* pResult) {
    FE_RestartCrAPAnim();
}

// FE_SetCrapRenderState for part 12's entry n: 1 for entries 0..4, 2 for entry 6, else 0.
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
            FE_SetCrapRenderState(1);
            break;
        case 6:
            FE_SetCrapRenderState(2);
            break;
        default:
            FE_SetCrapRenderState(0);
            break;
        }
    } else {
        FE_SetCrapRenderState(0);
    }
}

void fn_801097FC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        FE_SetCrapClub(0);
        return;
    case 1:
        FE_SetCrapClub(1);
        return;
    case 2:
        FE_SetCrapClub(4);
        return;
    case 3:
        FE_SetCrapClub(5);
        return;
    case 4:
        FE_SetCrapClub(2);
        return;
    default:
        FE_SetCrapClub(0);
        return;
    }
}

void fn_8010988C(MsgArg* pArgs, MsgArg* pResult) {
    FE_vClearGolferCache();
}

void fn_801098AC(MsgArg* pArgs, MsgArg* pResult) {
}

// Make a random created golfer: take the assets in slots 2, 5..8 and 11..14 off, parts 4, 5 and 6
// at their first choice, random choices for parts 0 to 8, 14, 16, 19 and 20 (no pick with a chance
// of 80% for 0 and 4, 90% for 5 and 6, 70% for 8, 19 and 20), parts 15, 9 and 10 as fn_80109FB4
// does, part 7 at b 1 or 2, then its equipment tiers again (fn_8007873C).
void fn_801098B0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nRoll = Misc_RandFunc(0) % 100;
    int nChoice;

    Misc_RandFunc(0);                       // drawn, not used
    FE_CrAP_SetTriggerAnims(0);
    FE_CrAP_UnequipSlot(2);
    FE_CrAP_UnequipSlot(5);
    FE_CrAP_UnequipSlot(6);
    FE_CrAP_UnequipSlot(7);
    FE_CrAP_UnequipSlot(8);
    FE_CrAP_UnequipSlot(11);
    FE_CrAP_UnequipSlot(12);
    FE_CrAP_UnequipSlot(13);
    FE_CrAP_UnequipSlot(14);
    FE_CrAP_TurnOnPart(5, 0, 0);
    FE_CrAP_TurnOnPart(6, 0, 0);
    FE_CrAP_TurnOnPart(4, 0, 0);
    FE_CrAP_RandomizePart(pProfile, 0, 80);
    FE_CrAP_RandomizePart(pProfile, 3, 0);
    FE_CrAP_RandomizePart(pProfile, 4, 80);
    FE_CrAP_RandomizePart(pProfile, 15, 0);
    FE_CrAP_RandomizePart(pProfile, 5, 90);
    FE_CrAP_RandomizePart(pProfile, 6, 90);
    FE_CrAP_RandomizePart(pProfile, 1, 0);
    FE_CrAP_RandomizePart(pProfile, 2, 0);
    FE_CrAP_RandomizePart(pProfile, 7, 0);
    FE_CrAP_RandomizePart(pProfile, 16, 0);
    FE_CrAP_RandomizePart(pProfile, 19, 70);
    FE_CrAP_RandomizePart(pProfile, 20, 70);
    FE_CrAP_RandomizePart(pProfile, 8, 70);
    nChoice = FE_CrAP_RandomizePart(pProfile, 14, 0);
    if (Misc_RandFunc(0) % 100 < 95) {
        FE_CrAP_TurnOnPart(15, 0, nChoice);
    } else {
        FE_CrAP_RandomizePart(pProfile, 15, 0);
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
    SaveProfile* pProfile = FE_GetCurrentProfile();
    FE_CrAP_SetTriggerAnims(0);
    FE_CrAP_UnequipSlot(2);
    FE_CrAP_UnequipSlot(5);
    FE_CrAP_UnequipSlot(6);
    FE_CrAP_UnequipSlot(7);
    FE_CrAP_UnequipSlot(8);
    FE_CrAP_UnequipSlot(11);
    FE_CrAP_UnequipSlot(12);
    FE_CrAP_UnequipSlot(13);
    FE_CrAP_UnequipSlot(14);
    FE_CrAP_RandomizePart(pProfile, 0, 80);
    FE_CrAP_RandomizePart(pProfile, 1, 0);
    FE_CrAP_RandomizePart(pProfile, 2, 0);
    FE_CrAP_RandomizePart(pProfile, 7, 0);
    FE_CrAP_RandomizePart(pProfile, 19, 70);
    FE_CrAP_RandomizePart(pProfile, 20, 70);
    FE_CrAP_RandomizePart(pProfile, 8, 70);
    fn_800797E0(pProfile, 7, (Misc_RandFunc(0) & 1) + 1, 0);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Idle" shot; then a random created golfer (fn_80079664) and its
// equipment tiers again (fn_8007873C).
void fn_80109CBC(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    View* pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    Gaud_PlayUISound((Misc_RandFunc(0) & 7) + 11);
    FE_ResetCrAPZoom();
    FE_SetCrAPCameraIdleState(0);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Idle", gpCrAPState->nCamIdleState, 0, 0, 0);
    FE_CrAP_SetTriggerAnims(0);
    fn_80079664(pProfile);
    fn_8007873C(pProfile);
}

// The CrAP camera to the "Crap Face" shot; then fn_80078E34 dresses the created golfer at random.
void fn_80109D5C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    View* pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    FE_ResetCrAPZoom();
    FE_SetCrAPCameraIdleState(1);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Face", gpCrAPState->nCamIdleState, 0, 0, 0);
    FE_CrAP_SetTriggerAnims(0);
    fn_80078E34(pProfile);
}

void fn_80109DDC(MsgArg* pArgs, MsgArg* pResult) {
}

// Sell a part's choice i back: a quarter of its price goes back into the money, and its bought
// bit (aB1CC) is cleared.
void fn_80109DE0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int b = pArgs[1].i;
    int i = pArgs[2].i;
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i);

    pProfile->nCurrentCash += (s32)(0.25f * FE_CrAP_GetPartRetailPrice(nPart, b, i));
    BitArray_ClearBit(pProfile->aB1CC, nAsset);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80109EAC(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_GetCategoryInfo(pArgs[0].i, pArgs[3].p, pArgs[2].p, pArgs[4].p, pArgs[1].p);
}

// ---- end of sweep code ----

// How many of the five random assets of category pArgs[0] have been bought (aB1CC).
void fn_80109EE8(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int i;
    s32* pnBought = pArgs[1].p;
    int nCategory = fn_80077BDC(pArgs[0].i);
    s8 b = FE_CrAP_GetCurrentGender();
    int nAsset;

    *pnBought = 0;
    for (i = 0; i < 5; i++) {
        nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(
                lbl_80281ED4->aKind[b][nCategory], lbl_80281ED4->aPart[b][nCategory][i],
                             lbl_80281ED4->aChoice[b][nCategory][i]);
        if (BitArray_TestBit(pProfile->aB1CC, nAsset)) {
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
    pProfile = FE_GetCurrentProfile();
    FE_CrAP_SetTriggerAnims(0);
    FE_CrAP_TurnOnPart(5, 0, 0);
    FE_CrAP_TurnOnPart(6, 0, 0);
    FE_CrAP_TurnOnPart(4, 0, 0);
    FE_CrAP_TurnOnPart(22, 0, 0);
    FE_CrAP_RandomizePart(pProfile, 3, 0);
    FE_CrAP_RandomizePart(pProfile, 4, 80);
    FE_CrAP_RandomizePart(pProfile, 15, 0);
    FE_CrAP_RandomizePart(pProfile, 5, 90);
    FE_CrAP_RandomizePart(pProfile, 6, 90);
    FE_CrAP_RandomizePart(pProfile, 16, 0);
    FE_CrAP_RandomizePart(pProfile, 22, 80);
    FE_CrAP_RandomizePart(pProfile, 21, 0);
    FE_CrAP_RandomizePart(pProfile, 11, 0);
    nChoice = FE_CrAP_RandomizePart(pProfile, 14, 0);
    if (Misc_RandFunc(0) % 100 < 95) {
        FE_CrAP_TurnOnPart(15, 0, nChoice);
    } else {
        FE_CrAP_RandomizePart(pProfile, 15, 0);
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
    FE_QueueCrAPAnim(NULL, NULL, 0, 0);
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
    if (nPart >= 0 && nPart < 24
        && (nAsset = FE_CrAP_GetFirstEquippedIndexForCategoryAndSubcategory(nPart, n)) != -1) {
        FE_CrAP_GetEntryNumFromAssetIDCategorySubcategory(nAsset, nPart, n, &nPlace);
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

    if (gpCrAPState->pB4 != NULL && nPart == 13) {
        FE_CrAP_GetPartVariantName(nPart, b, i, szName);
        if (AnimLib_FindByName(gpCrAPState->pB4->pChar->pLib, szName) == NULL) {
            bFound = 0;
        }
    }
    pResult->i = bFound;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8010A35C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetAssetFromAssetIndex(FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i))->n2E;
}

void fn_8010A3A4(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_RestoreLastRemovedAsset();
}

void fn_8010A3C4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8010A3C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = IsLeapYear(pArgs[0].i);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8010A400(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_IsAssetRemovable(FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i));
}

// ---- end of sweep code ----

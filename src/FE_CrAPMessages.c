// FE_CrAPMessages.c (EA's name in TW06 and TW07, whose GameMessages/FE_CrAPMessages.c holds the
// same handlers: GM_vSetCRAPSlider, GM_vGetCRAPItem, GM_vIsCrAPItemLocked, GM_vIsCrAPItemOnSale,
// GM_vIsCrAPItemOwned, GM_vPurchaseCrAPItem, GM_vCRAPCreatingLogo...): the Create-A-Player menus'
// message handlers. The menu UI sends each one by its number through the front end's message table
// (FE_MessageTable.c; every comment here says which), with its arguments and results as MsgArgs.
// They read and change the golfer being created through the Create-A-Player database (FE_CrAPDB.c:
// a part, the entries of its list and each entry's choices are EA's category, subcategory and
// entry number), buy and sell its assets, list the day's sale items and the newly unlocked ones,
// drive the logo editor (FE_LogoDesign.c) and the menu golfer's camera, animations and loader
// (FEgolferanim.c), and make random golfers.

#include "engine.h"
#include "camera.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "charstate.h"
#include "game.h"

void FE_CrAP_SetTriggerAnims(u8 b);     // FE_CrAPDB.c: let the menu golfer's animation and camera
                                        // calls run (1) or not (0)
void FE_CrAP_UnequipSlot(s16 nSlot);    // FE_CrAPDB.c: take the asset in the profile's slot off
void Gaud_PlayUISound(s32 n);
u8   IsLeapYear(u32 nYear);            // Calendar.c: a leap year (1900 counts as one)

// Menu message 381: how many choices entry pArgs[1] of part pArgs[0] offers
// (FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex).
void GM_vGetNumCrAPItems(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(pArgs[0].i, pArgs[1].i);
}

// Menu message 382: value pArgs[3] (the asset's a4A[n], FE_CrAP_GetPartValue) of choice pArgs[2]
// under entry pArgs[1] of part pArgs[0]; 0 when there is no such choice.
void GM_vGetCrAPItemValue(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, nEntry);
    int nChoice = pArgs[2].i;
    int nValue = pArgs[3].i;

    if (nChoice < 0 || nChoice >= nChoices) {
        pResult->i = 0;
    } else {
        pResult->i = FE_CrAP_GetPartValue(nPart, nEntry, nChoice, nValue);
    }
}

// Menu message 383: colour pArgs[3] of choice pArgs[2] under entry pArgs[1] of part pArgs[0]
// (FE_CrAP_GetPartColorRGBA) as red, green, blue and alpha 0..255 into *pArgs[4..7]; opaque red
// when there is no such choice.
void GM_vGetCrAPItemColor(MsgArg* pArgs, MsgArg* pResult) {
    u8 aColor[4];
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, nEntry);
    int nChoice = pArgs[2].i;
    int nColor = pArgs[3].i;

    if (nChoice < 0 || nChoice >= nChoices || nChoices == 0) {
        *(s32*)pArgs[4].p = 0xFF;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
        *(s32*)pArgs[7].p = 0xFF;
    } else {
        FE_CrAP_GetPartColorRGBA(nPart, nEntry, nChoice, nColor, aColor);
        *(s32*)pArgs[4].p = aColor[0];
        *(s32*)pArgs[5].p = aColor[1];
        *(s32*)pArgs[6].p = aColor[2];
        *(s32*)pArgs[7].p = aColor[3];
    }
}

// Menu message 404: does nothing in this build.
void GM_vFEMessage404_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 405: does nothing in this build.
void GM_vFEMessage405_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 406: set slider pArgs[0] (0..25) of the created golfer to pArgs[1]. When the menu
// golfer has a character, its body is reshaped from all 26 sliders and the slider's asset (part
// 18's choice pArgs[0]) is shown on it.
void GM_vSetCRAPSlider(MsgArg* pArgs, MsgArg* pResult) {
    SkinChoices* pChoices = &FE_GetCurrentProfile()->choices;
    int nSlider = pArgs[0].i;
    Character* pChar;

    pChoices->a9B4[nSlider] = pArgs[1].i;
    if (gpCrAPState->pB4 == NULL) {
        return;
    }
    pChar = gpCrAPState->pB4->pChar;
    if (pChar == NULL) {
        return;
    }
    CharSlider_UpdateCharacterBasedOnSliderValues(pChar->pSliderDefs, pChar->pModel, pChar->pSkin, 26,
                                                  pChoices->a9B4, &pChar->morphBlend);
    FE_CrAP_TurnOnPart(18, 0, nSlider);
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

// Menu message 408: does nothing in this build.
void GM_vFEMessage408_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 409: slider pArgs[0] of the created golfer (the profile's choices.a9B4, read
// signed).
void GM_vGetCRAPSlider(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    pResult->i = (s8)pProfile->choices.a9B4[pArgs[0].i];
}

// Menu message 410: how many different choices entry pArgs[1] of part pArgs[0] offers, assets of
// the same category and first colour counted once (FE_CrAP_GetNumberUniqueGeometries).
void GM_vGetNumCrAPGeometries(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetNumberUniqueGeometries(pArgs[0].i, pArgs[1].i);
}

// Menu message 411: always answers 0 in this build.
void GM_vFEMessage411_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Menu message 412: choice pArgs[2] under entry pArgs[1] of part pArgs[0]: its name into the string
// pArgs[3], and its three colour ids, sponsor and level into *pArgs[4..8]. With no such choice the
// name is "Coming Soon" in the session's 0x4000 mode, else "No Entry Found", and the five numbers
// are 0.
void GM_vGetCrAPItemInfo(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, nEntry);
    int nChoice = pArgs[2].i;

    if (nChoice < 0 || nChoice >= nChoices) {
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
        strcpy(((MsgString*)pArgs[3].p)->pStr, FE_CrAP_GetPartName(nPart, nEntry, nChoice));
        *(s32*)pArgs[4].p = FE_CrAP_GetPartColor1(nPart, nEntry, nChoice);
        *(s32*)pArgs[5].p = FE_CrAP_GetPartColor2(nPart, nEntry, nChoice);
        *(s32*)pArgs[6].p = FE_CrAP_GetPartColor3(nPart, nEntry, nChoice);
        *(s32*)pArgs[7].p = FE_CrAP_GetPartSponsor(nPart, nEntry, nChoice);
        *(s32*)pArgs[8].p = FE_CrAP_GetPartLevel(nPart, nEntry, nChoice);
    }
}

// Menu message 451: the two attributes choice pArgs[2] under entry pArgs[1] of part pArgs[0] raises
// and the tier it raises each to: *pArgs[3] and *pArgs[4] the first, *pArgs[5] and *pArgs[6] the
// second (-1 attribute: none); all 0 when there is no such choice.
void GM_vGetCrAPItemAttributes(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, nEntry);
    int nChoice = pArgs[2].i;

    if (nChoice < 0 || nChoice >= nChoices) {
        *(s32*)pArgs[3].p = 0;
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
    } else {
        *(s32*)pArgs[3].p = FE_CrAP_GetPartAttributeUpgrade1(nPart, nEntry, nChoice);
        *(s32*)pArgs[4].p = FE_CrAP_GetPartAttributeModifier1(nPart, nEntry, nChoice);
        *(s32*)pArgs[5].p = FE_CrAP_GetPartAttributeUpgrade2(nPart, nEntry, nChoice);
        *(s32*)pArgs[6].p = FE_CrAP_GetPartAttributeModifier2(nPart, nEntry, nChoice);
    }
}

// Menu message 452: choice pArgs[2] under entry pArgs[1] of part pArgs[0]: its price, its sale
// price, its lock kind and the number that goes with it into *pArgs[3..6]; all 0 when there is no
// such choice.
void GM_vGetCrAPItemPriceAndLock(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, nEntry);
    int nChoice = pArgs[2].i;

    if (nChoice < 0 || nChoice >= nChoices) {
        *(s32*)pArgs[3].p = 0;
        *(s32*)pArgs[4].p = 0;
        *(s32*)pArgs[5].p = 0;
        *(s32*)pArgs[6].p = 0;
    } else {
        *(s32*)pArgs[3].p = FE_CrAP_GetPartRetailPrice(nPart, nEntry, nChoice);
        *(s32*)pArgs[4].p = FE_CrAP_GetPartSalePrice(nPart, nEntry, nChoice);
        *(s32*)pArgs[5].p = FE_CrAP_GetPartGMLockID(nPart, nEntry, nChoice);
        *(s32*)pArgs[6].p = FE_CrAP_GetPartGMLockVal(nPart, nEntry, nChoice);
    }
}

// Menu message 413: pick choice pArgs[2] under entry pArgs[1] of part pArgs[0] on the created
// golfer: a worn asset that can come off (FE_CrAP_IsAssetEquipped, FE_CrAP_IsAssetRemovable) is
// taken off, anything else is put on (part 17's logos are only put on), then the golfer's equipment
// tiers are worked out again (FE_CrAP_UpdateUserAttributeMods).
void GM_vCRAPTryOnItem(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;
    int nAsset;

    if (nPart == 17) {
        FE_CrAP_TurnOnPart(nPart, nEntry, nChoice);
        return;
    }
    nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, nEntry, nChoice);
    if (nAsset != -1) {
        if (FE_CrAP_IsAssetEquipped(FE_CrAP_GetAssetFromAssetIndex(nAsset))
            && FE_CrAP_IsAssetRemovable(nAsset)) {
            FE_CrAP_TurnOffPart(nPart, nEntry, nChoice);
        } else {
            FE_CrAP_TurnOnPart(nPart, nEntry, nChoice);
        }
        FE_CrAP_UpdateUserAttributeMods(pProfile);
    }
}

// Menu message 760: GM_vCRAPTryOnItem without working out the equipment tiers again: choice
// pArgs[2] under entry pArgs[1] of part pArgs[0] is taken off when worn and removable, else put on.
void GM_vPreviewItem(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart;
    int nEntry;
    int nChoice;
    int nAsset;

    FE_GetCurrentProfile();
    nPart = pArgs[0].i;
    nEntry = pArgs[1].i;
    nChoice = pArgs[2].i;
    if (nPart == 17) {
        FE_CrAP_TurnOnPart(nPart, nEntry, nChoice);
        return;
    }
    nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, nEntry, nChoice);
    if (nAsset != -1) {
        if (FE_CrAP_IsAssetEquipped(FE_CrAP_GetAssetFromAssetIndex(nAsset))
            && FE_CrAP_IsAssetRemovable(nAsset)) {
            FE_CrAP_TurnOffPart(nPart, nEntry, nChoice);
            return;
        }
        FE_CrAP_TurnOnPart(nPart, nEntry, nChoice);
    }
}

// Menu message 441: how many entries part pArgs[0]'s list has
// (FE_CrAP_GetNumberOfSubcategoryIndicesForCategory).
void GM_vGetNumCrAPSubcategories(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetNumberOfSubcategoryIndicesForCategory(pArgs[0].i);
}

// Menu message 453: set one of the created golfer's details, chosen by pArgs[0]: 0 the profile's
// name (the string pArgs[1]); 1 its gender (choices.nGender, also made the database's current
// gender); 2 its date, pArgs[2] packed as month * 10000 + day * 1000000 + year (see FE_DateToInt); 3
// left-handed (choices.bLeftHanded). Values come in pArgs[2].
void GM_vSetCrAPGolferInfo(MsgArg* pArgs, MsgArg* pResult) {
    int nMonth;
    int nDay;
    int nYear;
    SaveProfile* pProfile = FE_GetCurrentProfile();

    switch (pArgs[0].i) {
    case 0:
        SaveProfile_SetName(pProfile, ((MsgString*)pArgs[1].p)->pStr);
        break;
    case 1:
        pProfile->choices.nGender = pArgs[2].i;
        FE_CrAP_SetCurrentGender(pProfile->choices.nGender);
        break;
    case 2:
        FE_IntToDate(pArgs[2].i, &nMonth, &nDay, &nYear);
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
        *(s32*)pArgs[2].p = (s8)pProfile->choices.nGender;
        break;
    case 2:
        *(s32*)pArgs[2].p = FE_DateToInt(pProfile->nDateMonth, pProfile->nDateDay, pProfile->nDateYear);
        break;
    case 3:
        *(s32*)pArgs[2].p = pProfile->choices.bLeftHanded;
        break;
    }
}

// Menu message 457: from now on the menus work on their own copy of the profile (pArgs[0] 1) or on
// the player slot's saved profile (0); FEProfile.bCopy, read by FE_GetCurrentProfile.
void GM_vSetUseProfileCopy(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->bCopy = pArgs[0].i;
}

// Menu message 460: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is still locked for the
// profile (FE_CrAP_IsItemLocked); never in the session's 0x4000 mode.
void GM_vIsCrAPItemLocked(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;

    if (gSession.uFlags & 0x4000) {
        pResult->i = 0;
    } else {
        pResult->i = FE_CrAP_IsItemLocked(
                FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, nEntry, nChoice),
                pProfile);
    }
}

// Menu message 461: the profile owns choice pArgs[2] under entry pArgs[1] of part pArgs[0]: its bit
// in aAssetOwned, set when it is bought (GM_vPurchaseCrAPItem) and from the start for level 0
// assets.
void GM_vIsCrAPItemOwned(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = BitArray_TestBit(pProfile->aAssetOwned, nAsset);
}

// Menu message 462: buy choice pArgs[2] under entry pArgs[1] of part pArgs[0] for pArgs[3]: the
// price comes off the profile's money (no check that it is there), the asset's owned bit
// (aAssetOwned) is set, the created golfer puts it on and its equipment tiers are worked out again
// (FE_CrAP_UpdateUserAttributeMods).
void GM_vPurchaseCrAPItem(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;
    s32 nPrice = pArgs[3].i;
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, nEntry, nChoice);

    pProfile->nCurrentCash -= nPrice;
    BitArray_SetBit(pProfile->aAssetOwned, nAsset);
    FE_CrAP_TurnOnPart(nPart, nEntry, nChoice);
    FE_CrAP_UpdateUserAttributeMods(pProfile);
}

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

// Menu message 474: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is in use: for part 13
// (custom animations) its animation is in the profile's list pArgs[1]
// (FE_CrAP_IsCustomAnimationSelected), for other parts it is the asset in its slot (FE_CrAP_IsItemEquipped).
void GM_vIsCrAPItemEquipped(MsgArg* pArgs, MsgArg* pResult) {
    char szName[16];                    // the size is unknown (the frame allows up to 20)
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;

    FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(nPart, nEntry);
    if (nPart == 13) {
        FE_CrAP_GetPartVariantName(nPart, nEntry, nChoice, szName);
        pResult->i = FE_CrAP_IsCustomAnimationSelected(pProfile, nEntry, szName);
    } else {
        pResult->i = FE_CrAP_IsItemEquipped(nPart, nEntry, nChoice);
    }
}

// Menu message 475: the day's sale items in sale category pArgs[0] (-1 clothes, -2 accessories, -3
// clubs and balls; see FE_CrAP_UpdateSaleInfo) for the current gender: the part they are of into *pArgs[1],
// their five choice numbers into *pArgs[2..6] (-1: none). GM_vGetCrAPSaleSubcategories gives their
// entries.
void GM_vGetCrAPSaleItems(MsgArg* pArgs, MsgArg* pResult) {
    int nSale = FE_GetSaleIDFromSaleCategory(pArgs[0].i);
    s8 nGender = FE_CrAP_GetCurrentGender();

    *(s32*)pArgs[1].p = gpFEProfile->aSalePart[nGender][nSale];
    *(s32*)pArgs[2].p = gpFEProfile->aSaleChoice[nGender][nSale][0];
    *(s32*)pArgs[3].p = gpFEProfile->aSaleChoice[nGender][nSale][1];
    *(s32*)pArgs[4].p = gpFEProfile->aSaleChoice[nGender][nSale][2];
    *(s32*)pArgs[5].p = gpFEProfile->aSaleChoice[nGender][nSale][3];
    *(s32*)pArgs[6].p = gpFEProfile->aSaleChoice[nGender][nSale][4];
}

// Menu message 476: the list entries (EA's subcategories) of the five sale items
// GM_vGetCrAPSaleItems gives for sale category pArgs[0], into *pArgs[1..5] (-1: none).
void GM_vGetCrAPSaleSubcategories(MsgArg* pArgs, MsgArg* pResult) {
    int nSale = FE_GetSaleIDFromSaleCategory(pArgs[0].i);
    s8 nGender = FE_CrAP_GetCurrentGender();

    *(s32*)pArgs[1].p = gpFEProfile->aSaleEntry[nGender][nSale][0];
    *(s32*)pArgs[2].p = gpFEProfile->aSaleEntry[nGender][nSale][1];
    *(s32*)pArgs[3].p = gpFEProfile->aSaleEntry[nGender][nSale][2];
    *(s32*)pArgs[4].p = gpFEProfile->aSaleEntry[nGender][nSale][3];
    *(s32*)pArgs[5].p = gpFEProfile->aSaleEntry[nGender][nSale][4];
}

// Menu message 477: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is one of the day's sale
// items. Parts 0, 1, 2 and 7 (headwear, shirts, pants, shoes) are sale category -1, parts 8, 19 and
// 20 (eyewear, watches and jewelry, miscellaneous) -2, part 12 -3; other parts are never on sale.
void GM_vIsCrAPItemOnSale(MsgArg* pArgs, MsgArg* pResult) {
    int j;
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;
    s8 nGender = FE_CrAP_GetCurrentGender();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, nEntry, nChoice);
    int nSaleCategory;
    int nSale;

    switch (nPart) {
    case 0:
    case 1:
    case 2:
    case 7:
        nSaleCategory = -1;
        break;
    case 8:
    case 19:
    case 20:
        nSaleCategory = -2;
        break;
    case 12:
        nSaleCategory = -3;
        break;
    default:
        pResult->i = 0;
        return;
    }
    nSale = FE_GetSaleIDFromSaleCategory(nSaleCategory);
    if (gpFEProfile->aSalePart[nGender][nSale] != nPart) {
        pResult->i = 0;
        return;
    }
    for (j = 0; j < 5; j++) {
        if (nAsset
            == FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(
                    gpFEProfile->aSalePart[nGender][nSale], gpFEProfile->aSaleEntry[nGender][nSale][j],
                    gpFEProfile->aSaleChoice[nGender][nSale][j])) {
            pResult->i = 1;
            return;
        }
    }
    pResult->i = 0;
}

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

// Menu message 491: logo palette colour pArgs[0] (0..255) as red, green, blue and alpha into
// *pArgs[1..4], 0..255 each (alpha 0 or 255).
void GM_vGetLogoPaletteColor(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7FC(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p, pArgs[4].p);
}

// Menu message 492: fill the logo being edited with the pixels of the texture named by the string
// pArgs[0] (copied into a 32-byte buffer first; fn_8010F890), at the editor's shape's size; nothing
// when there is no such texture.
void GM_vLoadLogoFromTexture(MsgArg* pArgs, MsgArg* pResult) {
    char szName[32] = "";

    strcpy(szName, ((MsgString*)pArgs[0].p)->pStr);
    fn_8010F890(szName);
}

// Menu message 493: mark the logo being edited changed, so it is copied into its texture on the
// next frame.
void GM_vMarkLogoChanged(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F880();
}

// Menu message 494: set pixel pArgs[0], pArgs[1] (x, y) of the logo being edited to palette colour
// pArgs[2]. A pixel off the logo writes the byte before it (fn_8010F90C's EA bug).
void GM_vSetLogoPixel(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F90C(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// Menu message 498: check every asset's lock again (FE_CrAP_SetupLockedAssets). Each asset that was
// locked before and is not now, and is offered for the current gender, gets its new bit (aAssetNew) set; the
// result is how many there are.
void GM_vCheckCrAPUnlocks(MsgArg* pArgs, MsgArg* pResult) {
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
    FE_CrAP_SetupLockedAssets(pProfile);
    for (i = 0; i < nAssets; i++) {
        if (!BitArray_TestBit(pProfile->aAssetLocked, i) && BitArray_TestBit(aWasLocked, i)) {
            if (FE_IsValidCurrentGender(FE_CrAP_GetAssetGender(i))) {
                nUnlocked++;
                BitArray_SetBit(pProfile->aAssetNew, i);
            }
        }
    }
    pResult->i = nUnlocked;
}

// Menu message 499: choice pArgs[2] under entry pArgs[1] of part pArgs[0] is newly unlocked (its
// aAssetNew bit, set by GM_vCheckCrAPUnlocks).
void GM_vIsCrAPItemNew(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    pResult->i = BitArray_TestBit(pProfile->aAssetNew, nAsset);
}

// Menu message 500: when choice pArgs[2] under entry pArgs[1] of part pArgs[0] is newly unlocked
// (aAssetNew), set its aAssetMarkedNew bit too; GM_vClearMarkedNewCrAPItems later clears both.
void GM_vMarkNewCrAPItem(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    if (BitArray_TestBit(pProfile->aAssetNew, nAsset)) {
        BitArray_SetBit(pProfile->aAssetMarkedNew, nAsset);
    }
}

// Menu message 501: every asset marked by GM_vMarkNewCrAPItem (both its aAssetNew and
// aAssetMarkedNew bits set) stops being new: both bits are cleared.
void GM_vClearMarkedNewCrAPItems(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nAssets = FE_CrAP_GetNumEntriesInCrAPDB();
    u32 i;

    for (i = 0; i < nAssets; i++) {
        if (BitArray_TestBit(pProfile->aAssetNew, i) && BitArray_TestBit(pProfile->aAssetMarkedNew, i)) {
            BitArray_ClearBit(pProfile->aAssetNew, i);
            BitArray_ClearBit(pProfile->aAssetMarkedNew, i);
        }
    }
}

// Menu message 502: 1 when a controller in any of the four ports holds the A button (PAD_BUTTON_A
// in the held half of Input_ReadControlPad), else 0.
void GM_vIsAnyPadHoldingA(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 4; i++) {
        if (Input_bDoesPadExist(i) && (Input_ReadControlPad(i) & 0x1000000)) {
            pResult->i = 1;
            return;
        }
    }
    pResult->i = 0;
}

// Menu message 557: the D-pad directions controller pArgs[0] holds (the held half of
// Input_ReadControlPad; the stick counts too while gControllers.bStickAsDpad is set): up, down,
// left and right into *pArgs[1..4], 1 or 0 each. Nothing is written when there is no controller in
// that port.
void GM_vGetDPadHeld(MsgArg* pArgs, MsgArg* pResult) {
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

// Menu message 503: set (pArgs[1] nonzero) or clear bit pArgs[0] of the profile's a10548 flags.
void GM_vSetProfileFlag(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 bSet = pArgs[1].i;
    s32 nBit = pArgs[0].i;

    if (bSet) {
        BitArray_SetBit(pProfile->a10548, nBit);
    } else {
        BitArray_ClearBit(pProfile->a10548, nBit);
    }
}

// Menu message 504: bit pArgs[0] of the profile's a10548 flags (GM_vSetProfileFlag sets them).
void GM_vGetProfileFlag(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    pResult->i = BitArray_TestBit(pProfile->a10548, pArgs[0].i);
}

// Menu message 507: five lines naming the newly unlocked assets (aAssetNew, offered for the current
// gender) into the strings pArgs[0..4]. With five or fewer, their names; with more, the names of
// the list entries they belong to, the fifth line "And more..." when there are more than five
// entries. Unused lines are " ".
void GM_vGetNewCrAPItemsText(MsgArg* pArgs, MsgArg* pResult) {
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
        if (BitArray_TestBit(pProfile->aAssetNew, i) && FE_IsValidCurrentGender(FE_CrAP_GetAssetGender(i))) {
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

// Menu message 509: today's date from the console clock: month, day and year into *pArgs[0..2].
void GM_vGetTodaysDate(MsgArg* pArgs, MsgArg* pResult) {
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

// Menu message 512: a random whole number from 1 to 99 (a 0 is drawn again), handed back as a
// float.
void GM_vGetRandom1To99(MsgArg* pArgs, MsgArg* pResult) {
    u32 n = 0;

    while (n == 0) {
        n = (u32)(100.0f * Misc_RandFuncg(0)) % 100;
    }
    pResult->f = n;
}

// Menu message 513: the name of the logo being edited into the string pArgs[1]; the result is its
// bSaved (1 once it has been kept by GM_vSaveLogo).
void GM_vGetLogoName(MsgArg* pArgs, MsgArg* pResult) {
    LogoRecord* pLogo = fn_8010FB70();
    strcpy(((MsgString*)pArgs[1].p)->pStr, pLogo->szName);
    pResult->i = pLogo->bSaved;
}

// Menu message 514: name the logo being edited after the string pArgs[1].
void GM_vSetLogoName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(fn_8010FB70()->szName, ((MsgString*)pArgs[1].p)->pStr);
}

// Menu message 621: the profile's logo pArgs[0] (0..4) has been made and kept (its bSaved;
// GM_vSaveLogo sets it).
void GM_vIsProfileLogoMade(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    pResult->i = pProfile->choices.aLogo[pArgs[0].i].bSaved;
}

// Menu message 622: keep the logo just edited: the menus' copy (FEProfile.logoCopy) is marked made
// (bSaved) and copied over the profile's logo the editor works on (fn_8010F7D8), and the editor
// goes back to working on the profile's logo (bEditingCopy cleared).
void GM_vSaveLogo(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 nLogo = fn_8010F7D8();

    gpFEProfile->logoCopy.bSaved = 1;
    Mem_cpy(&pProfile->choices.aLogo[nLogo], &gpFEProfile->logoCopy, sizeof(LogoRecord));
    gpFEProfile->bEditingCopy = 0;
}

// Menu message 515: golfer pArgs[0]'s equipment tier for attribute pArgs[1] (a created golfer's
// comes from what it wears, FE_CrAP_UpdateUserAttributeMods).
void GM_vGetGolferAttributeTier(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord = FE_spGetGolfer(pArgs[0].i);
    pResult->i = pRecord->tier[pArgs[1].i];
}

// Menu message 527: the name of entry pArgs[1] of part pArgs[0]'s list into the string pArgs[2]
// (for entry 0 the part's "All ..." entry when it has one).
void GM_vGetCrAPSubcategoryName(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_GetSubCategoryNameForCategoryAndSubcategoryIndex(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr);
}

// Menu message 528: the name of colour id pArgs[0] (an offset into the 'CR_S' names, as
// GM_vGetCrAPItemInfo gives them) into the string pArgs[1]; "" for "NONE".
void GM_vGetCrAPColorName(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_GetColorNameFromID(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// Menu message 529: how choice pArgs[2] under entry pArgs[1] of part pArgs[0] is unlocked, into the
// string pArgs[3] (FE_CrAP_GetUnlockMessageFrom); the string is left alone when there is no such
// choice.
void GM_vGetCrAPItemUnlockText(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoices = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, nEntry);
    int nChoice = pArgs[2].i;

    if (nChoice < 0 || nChoice >= nChoices) {
        return;
    }
    FE_CrAP_GetUnlockMessageFrom(nPart, nEntry, nChoice, ((MsgString*)pArgs[3].p)->pStr);
}

// Menu message 534: does nothing in this build.
void GM_vFEMessage534_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 535: the Create-A-Player camera's idle state (FE_SetCrAPCameraIdleState): 0 the
// "Crap Idle" shot, 1 the "Crap Face" shot.
void GM_vSetCrAPCameraIdleState(MsgArg* pArgs, MsgArg* pResult) {
    FE_SetCrAPCameraIdleState(pArgs[0].i);
}

// Menu message 538: 1 when the menu golfer's character has been made (FE_HasGolferCharacter), else
// 0.
void GM_vHasMenuGolfer(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)FE_HasGolferCharacter();
}

// Menu message 562: 1 when the menu golfer is loaded and ready to show (FE_IsGolferReady), else 0.
void GM_vIsMenuGolferReady(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)FE_IsGolferReady();
}

// Menu message 687: 1 when the menu golfer loader is idle (FE_StreamGetCurrentState is 1), else 0.
void GM_vIsGolferLoaderIdle(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_StreamGetCurrentState() == 1;
}

// Menu message 539: does nothing in this build.
void GM_vFEMessage539_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 542: 1 while the menus work on their own copy of the profile, 0 while on the player
// slot's saved one (FEProfile.bCopy; GM_vSetUseProfileCopy sets it).
void GM_vGetUseProfileCopy(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->bCopy;
}

// Menu message 543: start (pArgs[0] nonzero) or stop creating the profile's logo the editor works
// on (fn_8010F7D8). Starting copies that logo into the menus' copy (FEProfile.logoCopy), or for a
// logo not made yet (bSaved clear) makes a blank one there: every pixel palette colour 0x1C,
// square, named "MyLogo <n>" with n counted from 1. The editor then works on the copy (bEditingCopy
// set) until GM_vSaveLogo keeps it; stopping goes back to the profile's logo and drops the copy.
void GM_vCRAPCreatingLogo(MsgArg* pArgs, MsgArg* pResult) {
    char szName[32];                    // TW07's logoName is char[32] too
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s32 bStart = pArgs[0].i;
    s32 nLogo = fn_8010F7D8();

    if (bStart == 0) {
        gpFEProfile->bEditingCopy = 0;
        return;
    }
    if (pProfile->choices.aLogo[nLogo].bSaved) {
        Mem_cpy(&gpFEProfile->logoCopy, &pProfile->choices.aLogo[nLogo], sizeof(LogoRecord));
    } else {
        sprintf(szName, "MyLogo %d", nLogo + 1);
        strcpy(gpFEProfile->logoCopy.szName, szName);
        gpFEProfile->logoCopy.bSaved = 0;
        gpFEProfile->logoCopy.nShape = 0;
        memset(gpFEProfile->logoCopy.aPixels, 0x1C, sizeof(gpFEProfile->logoCopy.aPixels));
    }
    gpFEProfile->bEditingCopy = 1;
}

// Menu message 583: the same as GM_vGetLogoPaletteColor (message 491): logo palette colour pArgs[0]
// as red, green, blue and alpha into *pArgs[1..4], 0..255 each (alpha 0 or 255).
void GM_vGetLogoPaletteEntry(MsgArg* pArgs, MsgArg* pResult) {
    fn_8010F7FC(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p, pArgs[4].p);
}

// Menu message 584: pixel pArgs[0], pArgs[1] (x, y) of the logo being edited: the result is its
// palette colour index, and its red, green, blue and alpha go into *pArgs[2..5] (fn_8010FBCC). A
// pixel off the logo reads the byte before it.
void GM_vGetLogoPixel(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8010FBCC(pArgs[0].i, pArgs[1].i, pArgs[2].p, pArgs[3].p, pArgs[4].p, pArgs[5].p);
}

// Menu message 587: the logo being edited becomes square or rectangular (pArgs[1], LOGO_SQUARE or
// LOGO_RECT; another value leaves the logo's own shape alone) and the editor takes the shape too
// (fn_8010F7E4).
void GM_vSetLogoShape(MsgArg* pArgs, MsgArg* pResult) {
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

// Menu message 588: the shape of the logo being edited (LOGO_SQUARE or LOGO_RECT).
void GM_vGetLogoShape(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8010FB70()->nShape;
}

// Menu message 609: does nothing in this build.
void GM_vFEMessage609_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 657: let the menu golfer's animation and camera calls run (pArgs[0] 1) or do nothing
// (0) (FE_CrAP_SetTriggerAnims).
void GM_vSetCrAPTriggerAnims(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_SetTriggerAnims(pArgs[0].i);
}

// Menu message 663: start the menu golfer's next animation at once and drop the repeats and queued
// animation (FE_RestartCrAPAnim).
void GM_vRestartCrAPAnim(MsgArg* pArgs, MsgArg* pResult) {
    FE_RestartCrAPAnim();
}

// Menu message 664: what the Create-A-Player screen shows for entry pArgs[1] of part pArgs[0]
// (FE_SetCrapRenderState): part 12's entries 0..4 show the clubs, its entry 6 the ball; anything
// else the golfer.
void GM_vSetCrAPRenderStateForSubcategory(MsgArg* pArgs, MsgArg* pResult) {
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

// Menu message 665: the club the menu golfer holds (FE_SetCrapClub): pArgs[0] 0 a driver, 1 a
// fairway wood, 2 a 7-iron, 3 a wedge, 4 the putter; anything else a driver.
void GM_vSetCrAPClub(MsgArg* pArgs, MsgArg* pResult) {
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

// Menu message 685: drop the golfers loaded for the menus (FE_vClearGolferCache).
void GM_vClearGolferCache(MsgArg* pArgs, MsgArg* pResult) {
    FE_vClearGolferCache();
}

// Menu message 598: does nothing in this build.
void GM_vFEMessage598_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 689: dress and shape the created golfer at random. The assets in slots 2, 5..8 and
// 11..14 come off and parts 4, 5 and 6 go to their first choice. Then each part gets a random owned
// choice (FE_CrAP_RandomizeCrAPCategoryInOneSubcategory), except that it is left alone with the
// chance given: headwear (0) and part 4 80%, parts 5 and 6 90%, eyewear (8), watches and jewelry
// (19) and miscellaneous (20) 70%; hair (3), parts 15 and 16, shirts (1), pants (2), shoes (7) and
// part 14 always change. Part 15 then takes part 14's choice 95% of the time, part 9 is one of
// choices 0..2 (75%), 6 (10%), 4 (10%) or 8 (5%), part 10 a desirable choice 95% of the time
// (FE_CrAP_RandomizeCategoryWithUndesirableTest), the shoes come from entry 1 or 2, and the
// equipment tiers are worked out again (FE_CrAP_UpdateUserAttributeMods).
void GM_vRandomizeCrAPGolfer(MsgArg* pArgs, MsgArg* pResult) {
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
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 0, 80);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 3, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 4, 80);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 15, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 5, 90);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 6, 90);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 1, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 2, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 7, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 16, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 19, 70);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 20, 70);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 8, 70);
    nChoice = FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 14, 0);
    if (Misc_RandFunc(0) % 100 < 95) {
        FE_CrAP_TurnOnPart(15, 0, nChoice);
    } else {
        FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 15, 0);
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
    FE_CrAP_RandomizeCategoryWithUndesirableTest(10, 95);
    FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem(pProfile, 7, (Misc_RandFunc(0) & 1) + 1, 0);
    FE_CrAP_UpdateUserAttributeMods(pProfile);
}

// Menu message 690: dress the created golfer at random: the assets in slots 2, 5..8 and 11..14 come
// off, then random owned choices go on (FE_CrAP_RandomizeCrAPCategoryInOneSubcategory) for shirts
// (1), pants (2) and shoes (7), and with a chance of 20% for headwear (0) and 30% each for watches
// and jewelry (19), miscellaneous (20) and eyewear (8); the shoes come from entry 1 or 2, and the
// equipment tiers are worked out again (FE_CrAP_UpdateUserAttributeMods).
void GM_vRandomizeCrAPOutfit(MsgArg* pArgs, MsgArg* pResult) {
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
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 0, 80);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 1, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 2, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 7, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 19, 70);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 20, 70);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 8, 70);
    FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem(pProfile, 7, (Misc_RandFunc(0) & 1) + 1, 0);
    FE_CrAP_UpdateUserAttributeMods(pProfile);
}

// Menu message 547: play one of UI sounds 11..18 at random, put the Create-A-Player camera back to
// the "Crap Idle" shot (zoom reset, idle state 0), turn the menu golfer's animation calls off and
// make a random created golfer (FE_CrAP_RandomizeAll: part 12's entries 0..3 and 5..7 at their first choice,
// a driver in hand, a random shirt, pants and shoes, then a random look, FE_CrAP_RandomizeFace), then work
// out its equipment tiers again (FE_CrAP_UpdateUserAttributeMods).
void GM_vRandomizeCrAPGolferInIdleShot(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    View* pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    Gaud_PlayUISound((Misc_RandFunc(0) & 7) + 11);
    FE_ResetCrAPZoom();
    FE_SetCrAPCameraIdleState(0);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Idle", gpCrAPState->nCamIdleState, 0, 0, 0);
    FE_CrAP_SetTriggerAnims(0);
    FE_CrAP_RandomizeAll(pProfile);
    FE_CrAP_UpdateUserAttributeMods(pProfile);
}

// Menu message 554: put the Create-A-Player camera on the "Crap Face" shot (zoom reset, idle state
// 1), turn the menu golfer's animation calls off and give the created golfer a random look
// (FE_CrAP_RandomizeFace: face, hair and its colours, a hat now and then, a few accessories). The equipment
// tiers are not worked out again.
void GM_vRandomizeCrAPLookInFaceShot(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    View* pView = ViewController_GetCameraControl(ViewController_GetCurrentViewControllerID());
    FE_ResetCrAPZoom();
    FE_SetCrAPCameraIdleState(1);
    GolfCamera_SwitchCrAPCamera(pView, "Crap Face", gpCrAPState->nCamIdleState, 0, 0, 0);
    FE_CrAP_SetTriggerAnims(0);
    FE_CrAP_RandomizeFace(pProfile);
}

// Menu message 566: does nothing in this build.
void GM_vFEMessage566_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 711: sell choice pArgs[2] under entry pArgs[1] of part pArgs[0] back: a quarter of
// its full price (not its sale price) goes into the profile's money and it is no longer owned
// (aAssetOwned). It is not taken off the golfer, and nothing checks that it was owned.
void GM_vSellCrAPItem(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;
    int nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, nEntry, nChoice);

    pProfile->nCurrentCash += (s32)(0.25f * FE_CrAP_GetPartRetailPrice(nPart, nEntry, nChoice));
    BitArray_ClearBit(pProfile->aAssetOwned, nAsset);
}

// Menu message 712: count part pArgs[0]'s assets offered for the current gender
// (FE_CrAP_GetCategoryInfo): all of them into *pArgs[1], the owned ones (aAssetOwned) into
// *pArgs[2], the locked ones into *pArgs[3], the new ones (aAssetNew) into *pArgs[4].
void GM_vGetCrAPCategoryCounts(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_GetCategoryInfo(pArgs[0].i, pArgs[3].p, pArgs[2].p, pArgs[4].p, pArgs[1].p);
}

// Menu message 713: how many of the day's five sale items in sale category pArgs[0] (-1, -2, -3;
// see GM_vGetCrAPSaleItems) the profile owns (aAssetOwned), into *pArgs[1].
void GM_vGetNumCrAPSaleItemsOwned(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int i;
    s32* pnBought = pArgs[1].p;
    int nSale = FE_GetSaleIDFromSaleCategory(pArgs[0].i);
    s8 nGender = FE_CrAP_GetCurrentGender();
    int nAsset;

    *pnBought = 0;
    for (i = 0; i < 5; i++) {
        nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(
                gpFEProfile->aSalePart[nGender][nSale], gpFEProfile->aSaleEntry[nGender][nSale][i],
                gpFEProfile->aSaleChoice[nGender][nSale][i]);
        if (BitArray_TestBit(pProfile->aAssetOwned, nAsset)) {
            (*pnBought)++;
        }
    }
}

// Menu message 714: give the created golfer a random body, hair and face, clothes left alone. Parts
// 4, 5, 6 and 22 go to their first choice, then random owned choices go on
// (FE_CrAP_RandomizeCrAPCategoryInOneSubcategory): always for hair (3) and parts 11, 14, 15, 16 and
// 21, and for parts 4 and 22 (left alone 80% of the time) and 5 and 6 (left alone 90%). Part 15
// takes part 14's choice 95% of the time, part 9 is one of choices 0..2 (75%), 6 (10%), 4 (10%) or
// 8 (5%), and part 10 gets a desirable choice 95% of the time
// (FE_CrAP_RandomizeCategoryWithUndesirableTest). The equipment tiers are not worked out again.
void GM_vRandomizeCrAPBody(MsgArg* pArgs, MsgArg* pResult) {
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
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 3, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 4, 80);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 15, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 5, 90);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 6, 90);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 16, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 22, 80);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 21, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 11, 0);
    nChoice = FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 14, 0);
    if (Misc_RandFunc(0) % 100 < 95) {
        FE_CrAP_TurnOnPart(15, 0, nChoice);
    } else {
        FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 15, 0);
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
    FE_CrAP_RandomizeCategoryWithUndesirableTest(10, 95);
}

// Menu message 725: drop the menu golfer's queued animation (FE_QueueCrAPAnim with no animation).
void GM_vClearQueuedCrAPAnim(MsgArg* pArgs, MsgArg* pResult) {
    FE_QueueCrAPAnim(NULL, NULL, 0, 0);
}

// Menu message 728: which choice of entry pArgs[1] of part pArgs[0] the created golfer wears: the
// asset in the first of the profile's slots that is of the part and fits the entry, as its place in
// the entry's list (FE_CrAP_GetEntryNumFromAssetIDCategorySubcategory); 0 when none is worn or the
// part is not 0..23.
void GM_vGetEquippedCrAPItemInSubcategory(MsgArg* pArgs, MsgArg* pResult) {
    s16 nPart;
    int nEntry;
    s32 nResult = 0;
    s32 nPlace;
    int nAsset;

    nPart = pArgs[0].i;
    nEntry = pArgs[1].i;
    nPlace = 0;
    if (nPart >= 0 && nPart < 24
        && (nAsset = FE_CrAP_GetFirstEquippedIndexForCategoryAndSubcategory(nPart, nEntry)) != -1) {
        FE_CrAP_GetEntryNumFromAssetIDCategorySubcategory(nAsset, nPart, nEntry, &nPlace);
        nResult = nPlace;
    }
    pResult->i = nResult;
}

// Menu message 730: for part 13 (custom animations), whether the animation of choice pArgs[2] under
// entry pArgs[1] is in the menu golfer's animation library (AnimLib_FindByName); always 1 for other
// parts, and when there is no menu golfer slot.
void GM_vIsCrAPAnimInGolferLib(MsgArg* pArgs, MsgArg* pResult) {
    char szName[64];                    // the size is unknown (the frame allows up to 0x40)
    u8 bFound = 1;
    s16 nPart = pArgs[0].i;
    int nEntry = pArgs[1].i;
    int nChoice = pArgs[2].i;

    if (gpCrAPState->pB4 != NULL && nPart == 13) {
        FE_CrAP_GetPartVariantName(nPart, nEntry, nChoice, szName);
        if (AnimLib_FindByName(gpCrAPState->pB4->pChar->pLib, szName) == NULL) {
            bFound = 0;
        }
    }
    pResult->i = bFound;
}

// Menu message 739: the profile slot (0..52, -1: none) choice pArgs[2] under entry pArgs[1] of part
// pArgs[0] goes into when worn (the asset's n2E). No range check: a missing choice reads the record
// before the first asset.
void GM_vGetCrAPItemSlot(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_GetAssetFromAssetIndex(FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i))->n2E;
}

// Menu message 740: undo the last asset put on the created golfer
// (FE_CrAP_RestoreLastRemovedAsset): the asset it replaced goes back on, or when it replaced none
// it comes off again.
void GM_vRestoreAfterPreview(MsgArg* pArgs, MsgArg* pResult) {
    FE_CrAP_RestoreLastRemovedAsset();
}

// Menu message 741: does nothing in this build.
void GM_vFEMessage741_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Menu message 756: whether year pArgs[0] is a leap year (IsLeapYear; 1900 counts as one).
void GM_vIsLeapYear(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = IsLeapYear(pArgs[0].i);
}

// Menu message 757: whether picking choice pArgs[2] under entry pArgs[1] of part pArgs[0] again,
// once worn, takes it off (FE_CrAP_IsAssetRemovable: animations and the assets of slots 2 and
// 4..14).
void GM_vIsCrAPItemRemovable(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_CrAP_IsAssetRemovable(FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(pArgs[0].i, pArgs[1].i, pArgs[2].i));
}

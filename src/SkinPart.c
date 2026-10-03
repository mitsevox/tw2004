// SkinPart.c (EA's name, from its asserts; TW07 has no such file): the choosable pieces of the
// golfer's skinned body and club skins. A skin's parts (such as the glove) each have variants
// ("GloveOn", "GloveOff") and options; its sets (a club's shaft or grip, a logo) swap the texture
// names of their first variant for the chosen variant's and recolour them with the chosen option.
// Everything is found by name code (SKA_PackName). Each skin keeps four copies of its choices
// (Skin.aParts, Skin.aSets): a new choice goes into copy 3, or into all four at once
// (SkinPart_SetChangeAllCopies), and char.c moves it down copy by copy as the textures load; copy 2
// is what the texture lists are built from (SkinPart_DropUnusedTextures,
// SkinPart_QueueMissingTextures), copy 0 what is drawn (SkinPart_DrawPart) and burnt
// (SkinPart_BurnBodySkin). A golfer's choices are also kept outside the skins (SkinChoices, the
// save). The types are in charstate.h.

#include "charstate.h"
#include "terrain.h"
#include "camera.h"
#include "lldyntex.h"

void  fn_80112614(SkinDesc14* pEntry, SkinDesc18* pMaterial, TexBank* pBank);
void  fn_80113774(int nPart, int nVariant, int nOption);
void  fn_8011387C(void);
void  fn_8011389C(void);
void  fn_801138CC(SkinDesc* pDesc);
void  fn_801138D8(void* p);
int   fn_8001005C(TexBank* pBank, u64 uHash);   // LLTex.c: the texture's index, or 0x80000000

void  SkinPart_ChooseBodyPartVariant(Character* pChar, int nPart, int nVariant);
void  SkinPart_Init(void);
void  SkinPart_Shutdown(void);
s32   SkinPart_GetNumPartVariants(Skin* pSkin, int nPart);
s32   SkinPart_GetNumPartOptions(Skin* pSkin, int nPart, int nVariant);
void  SkinPart_ChoosePartOption(Skin* pSkin, int nPart, int nOption);
u64   SkinPart_GetPartVariantId(Skin* pSkin, int nPart, int nVariant);
s32   SkinPart_GetNumSets(Skin* pSkin);
s32   SkinPart_GetNumSetVariants(Skin* pSkin, int nSet);
s32   SkinPart_GetNumSetOptions(Skin* pSkin, int nSet, int nVariant);
s32   SkinPart_GetSetVariant(Skin* pSkin, int nSet, int nCopy);
s32   SkinPart_GetSetVariantUVIndex(Skin* pSkin, int nSet, int nVariant);
s32   SkinPart_GetSetOption(Skin* pSkin, int nSet, int nCopy);
u64   SkinPart_GetSetId(Skin* pSkin, int nSet);
u64   SkinPart_GetSetVariantId(Skin* pSkin, int nSet, int nVariant);
s32   SkinPart_GetOptionSize(SkinDesc* pDesc, int n);
s32   SkinPart_GetMaxPartOptionSize(SkinDesc* pDesc, int nPart);
s32   SkinPart_GetMeshBit(Skin* pSkin, s32 n);
void  SkinPart_MarkMeshMatrices(Skin* pSkin, SkinMesh* pMesh);
void  SkinPart_MarkOption(Skin* pSkin, int n);
void  SkinPart_MarkPart(Skin* pSkin, int nPart);
s32   SkinPart_FindSetOptionByName(Skin* pSkin, int nSet, int nVariant, const char* pName);
s32   SkinPart_GetNumVariantLinks(Skin* pSkin, int nPart, int nVariant);
void  SkinPart_ApplyVariantLink(Skin* pSkin, int nPart, int nVariant, int nLink);
void  SkinPart_AddToTexList(u64 uId, SkinListEntry* aList, s32* pnList, u8* pRecolor, s32 nMode);
void  SkinPart_ListOptionTextures(Skin* pSkin, int n, SkinListEntry* aList, s32* pnList, int nCopy);
s32   SkinPart_ListChosenTextures(Skin** apSkins, int nSkins, SkinListEntry** ppList, u64* aIds, int nIds,
                                  int nCopy);
u8    SkinPart_TexListHas(u64 uId, SkinListEntry* aList, int nList);
u64   SkinPart_TexListGetId(int i, SkinListEntry* aList, int nList);
u8*   SkinPart_TexListGetRecolor(int i, SkinListEntry* aList, int nList);
s32   SkinPart_TexListGetRecolorMode(int i, SkinListEntry* aList, int nList);
void  SkinPart_DropTexture(Skin* pSkin, DynTex* pTex, u64 uId);
void  SkinPart_CopyChoices(Skin* pSkin, int nFrom, int nTo);
void  SkinPart_SetChangeAllCopies(u8 b);
void  SkinPart_InitChangeAllCopies(void);

// The morph targets SkinPart_BurnBodySkin keeps (at weight 0); -1 ends the list.
s32   gBurnKeptMorphs[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1};
// The parts SkinPart_BurnBodySkin keeps every variant of, by name; NULL ends the list.
char* gBurnAllVariantParts[2] = {"Glove", NULL};
u8    gSkinChangeAllCopies;     // set: a new choice goes into all four copies of a skin's choices,
                                // clear: into copy 3 only (SkinPart_SetChangeAllCopies)

// Copies a golfer's look between the body's skin and pChoices: while pChoices holds no choice yet
// (every nVariant -1) the skin's copy 0 goes into it; otherwise its choices go into all four copies
// of the skin's.
void SkinPart_ApplyBodyChoices(Character* pChar, SkinChoices* pChoices) {
    u8 bEmpty;
    int i;
    int k;
    int j;

    if (pChar == NULL || pChoices == NULL || pChar->pSkin == NULL) return;
    bEmpty = 1;
    for (i = 0; i < 40; i++) {
        if (pChoices->aParts[i].nVariant != -1) {
            bEmpty = 0;
        }
    }
    for (i = 0; i < 116; i++) {
        if (pChoices->aSets[i].nVariant != -1) {
            bEmpty = 0;
        }
    }
    if (bEmpty) {
        Mem_cpy(pChoices->aParts, pChar->pSkin->aParts[0], SkinPart_GetNumParts(pChar->pSkin)
                * sizeof(SkinChoice));
    } else {
        for (j = 0; j < 4; j++) {
            Mem_cpy(pChar->pSkin->aParts[j], pChoices->aParts,
                    SkinPart_GetNumParts(pChar->pSkin) * sizeof(SkinChoice));
        }
    }
    if (bEmpty) {
        Mem_cpy(pChoices->aSets, pChar->pSkin->aSets[0], SkinPart_GetNumSets(pChar->pSkin)
                * sizeof(SkinChoice));
    } else {
        for (k = 0; k < 4; k++) {
            Mem_cpy(pChar->pSkin->aSets[k], pChoices->aSets,
                    SkinPart_GetNumSets(pChar->pSkin) * sizeof(SkinChoice));
        }
    }
}

// Gives the six club skins (pClubSet, one per club class) their choices from pChoices, in all four
// copies.
void SkinPart_ApplyClubChoices(Character* pChar, SkinChoices* pChoices) {
    int i;
    int j;

    if (pChar == NULL || pChoices == NULL || pChar->pClubSet == NULL) return;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) {
            // fake match: the (u32) on j (0-3, so the same index) keeps the array start and the offset
            // apart, as EA's code does (docs/compiler/decomp-notes.md, the (u32) index cast)
            Mem_cpy(pChar->pClubSet->apSkins[i]->aParts[(u32)j], pChoices->aSkinParts[i],
                    SkinPart_GetNumParts(pChar->pClubSet->apSkins[i]) * sizeof(SkinChoice));
            Mem_cpy(pChar->pClubSet->apSkins[i]->aSets[(u32)j], pChoices->aSkinSets[i],
                    SkinPart_GetNumSets(pChar->pClubSet->apSkins[i]) * sizeof(SkinChoice));
        }
    }
}

// Burns the body's skin down to its current look (SkinBurn SkinBurn_BurnSkin): each part keeps only its
// chosen variant and option, except the parts named in gBurnAllVariantParts (the glove), which
// keep every variant so the glove can still come off and go back on (stateFunc.c); morph targets
// 0-9 (gBurnKeptMorphs) are kept. char.c calls it once a look is applied.
void SkinPart_BurnBodySkin(Character* pChar) {
    s32 aParts[2];
    char** ppName;
    int n;
    int nPart;

    if (pChar == NULL || pChar->pSkin == NULL) return;
    n = 0;
    for (ppName = gBurnAllVariantParts; *ppName != NULL; ppName++) {
        nPart = SkinPart_FindPartByName(pChar->pSkin, *ppName);
        if (nPart >= 0) {
            aParts[n++] = nPart;
        }
    }
    aParts[n] = -1;
    SkinBurn_BurnSkin(pChar->pSkin, aParts, gBurnKeptMorphs);
}

void SkinPart_ChooseBodyPartVariant(Character* pChar, int nPart, int nVariant) {
    if (pChar == NULL || pChar->pSkin == NULL) return;
    SkinPart_ChoosePartVariant(pChar->pSkin, nPart, nVariant);
}

// Picks a variant of a part of the body's skin by their names (the glove on or off).
void SkinPart_ChooseBodyPartVariantByName(Character* pChar, char* pPart, char* pVariant) {
    u64 uId;
    int nPart;
    int nVariant;

    if (pChar == NULL || pChar->pSkin == NULL) return;
    SKA_PackName(&uId, pPart);
    nPart = SkinPart_FindPart(pChar->pSkin, uId);
    SKA_PackName(&uId, pVariant);
    nVariant = SkinPart_FindPartVariant(pChar->pSkin, nPart, uId);
    SkinPart_ChooseBodyPartVariant(pChar, nPart, nVariant);
}

// Picks a set's variant and option of the body's skin by their names (char.c: the shirt and the
// glove); pOption NULL chooses no option (-1). Does nothing while the skin has no model.
void SkinPart_ChooseBodySetByName(Character* pChar, char* pSet, char* pVariant, char* pOption) {
    int nSet;
    int nVariant;
    int nOption;

    if (pChar == NULL || pChar->pSkin == NULL) return;
    if (pChar->pSkin->pModel != NULL) {
        nSet = SkinPart_FindSetByName(pChar->pSkin, pSet);
        nVariant = SkinPart_FindSetVariantByName(pChar->pSkin, nSet, pVariant);
        nOption = SkinPart_FindSetOptionByName(pChar->pSkin, nSet, nVariant, pOption);
        SkinPart_ChooseBodySet(pChar, nSet, nVariant, nOption);
    }
}

// Picks a variant of a part of one of the six club skins (nSkin 0..5: Drivers, Fairwaywoods,
// Putters, 3Irons, 7Irons, Wedges; char.c gClubPartNames) by their name codes; a variant not found
// becomes variant 0. Does nothing while the body's skin has no model.
void SkinPart_ChooseClubPartVariant(Character* pChar, int nSkin, u64 uPart, u64 uVariant) {
    int nPart;
    int nVariant;
    Skin* pSkin;

    if (pChar == NULL || pChar->pClubSet == NULL || pChar->pClubSet->apSkins == NULL || nSkin < 0 || nSkin
        >= 6) {
        return;
    }
    if (pChar->pSkin->pModel != NULL) {
        pSkin = pChar->pClubSet->apSkins[nSkin];
        nPart = SkinPart_FindPart(pSkin, uPart);
        nVariant = SkinPart_FindPartVariant(pSkin, nPart, uVariant);
        if (nVariant < 0) {
            nVariant = 0;
        }
        SkinPart_ChoosePartVariant(pSkin, nPart, nVariant);
    }
}

// Picks a set's variant and option of one of the six club skins (nSkin 0..5, as
// SkinPart_ChooseClubPartVariant) by their name codes: the club's model, shaft or grip; an option
// not found becomes option 0. Does nothing while the body's skin has no model.
void SkinPart_ChooseClubSet(Character* pChar, int nSkin, u64 uSet, u64 uVariant, u64 uOption) {
    int nSet;
    int nVariant;
    Skin* pSkin;
    int nOption;

    if (pChar == NULL || pChar->pClubSet == NULL || pChar->pClubSet->apSkins == NULL || nSkin < 0 || nSkin
        >= 6) {
        return;
    }
    if (pChar->pSkin->pModel != NULL) {
        pSkin = pChar->pClubSet->apSkins[nSkin];
        nSet = SkinPart_FindSet(pSkin, uSet);
        nVariant = SkinPart_FindSetVariant(pSkin, nSet, uVariant);
        nOption = SkinPart_FindSetOption(pSkin, nSet, nVariant, uOption);
        if (nOption < 0) {
            nOption = 0;
        }
        SkinPart_ChooseSet(pSkin, nSet, nVariant, nOption);
    }
}

// Makes the clubs left- or right-handed (char.c passes Character_IsLeftHanded): every set of the six
// club skins that has a "DefaultL" variant gets it when bOn, else its first variant, keeping the
// option of copy 3 (copy 0 when SkinPart_GetChangeAllCopies).
void SkinPart_SetClubsLeftHanded(Character* pChar, u8 bOn) {
    int i;
    int j;
    int nSets;
    int nVariant;
    int nOption;
    int nCopy;
    u8 bAll;

    if (pChar == NULL || pChar->pClubSet == NULL || pChar->pClubSet->apSkins == NULL) return;
    for (i = 0; i < 6; i++) {
        nSets = SkinPart_GetNumSets(pChar->pClubSet->apSkins[i]);
        for (j = 0; j < nSets; j++) {
            nVariant = SkinPart_FindSetVariantByName(pChar->pClubSet->apSkins[i], j, "DefaultL");
            if (nVariant >= 0) {
                bAll = SkinPart_GetChangeAllCopies();
                nCopy = 3;
                if (bAll) {
                    nCopy = 0;
                }
                nOption = SkinPart_GetSetOption(pChar->pClubSet->apSkins[i], j, nCopy);
                if (bOn) {
                    SkinPart_ChooseSet(pChar->pClubSet->apSkins[i], j, nVariant, nOption);
                } else {
                    SkinPart_ChooseSet(pChar->pClubSet->apSkins[i], j, 0, nOption);
                }
            }
        }
    }
}

// SkinPart_ChooseSet on the character's body skin; does nothing while the skin has no model.
void SkinPart_ChooseBodySet(Character* pChar, int nSet, int nVariant, int nOption) {
    if (pChar == NULL || pChar->pSkin == NULL) return;
    if (pChar->pSkin->pModel != NULL) {
        SkinPart_ChooseSet(pChar->pSkin, nSet, nVariant, nOption);
    }
}

// The module's start-up: new choices go into all four copies at once
// (SkinPart_InitChangeAllCopies). char.c calls it from both of its start-ups, then sets the mode it
// wants with SkinPart_SetChangeAllCopies.
void SkinPart_Init(void) {
    SkinPart_InitChangeAllCopies();
}

// Empty in this build; char.c calls it from both of its shut-downs, the pairs of the start-ups that
// call SkinPart_Init.
void SkinPart_Shutdown(void) {
}

// The skin's number of parts.
s32 SkinPart_GetNumParts(Skin* pSkin) {
    if (pSkin->pModel == NULL) return 0;
    if (pSkin->pModel->pDesc != NULL) {
        return pSkin->pModel->pDesc->nParts;
    }
    return 0;
}

// A part's number of variants.
s32 SkinPart_GetNumPartVariants(Skin* pSkin, int nPart) {
    SkinDesc* pDesc;

    if (pSkin->pModel == NULL) return 0;
    if (nPart >= 0) {
        pDesc = pSkin->pModel->pDesc;
        if (pDesc != NULL && nPart < pDesc->nParts) {
            return pDesc->pParts[nPart].nVariants;
        }
    }
    return 0;
}

// A variant's number of options.
s32 SkinPart_GetNumPartOptions(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;

    if (pSkin == NULL || pSkin->pModel == NULL) return 0;
    pDesc = pSkin->pModel->pDesc;
    // fake match: the cast to the field's own type gives EA's (i * 0x18 + 8) lwzx; perhaps EA's field
    // was untyped (SkinPart_FixupDesc turns the file's offsets into pointers)
    return ((SkinVariant*)pDesc->pVariants)[nVariant + pDesc->pParts[nPart].nFirst].nOptions;
}

// Picks a part's variant (-1, none, when out of range) in copy 3, or in all four copies when
// SkinPart_GetChangeAllCopies, then also setting Skin.uFlags bit 1 so SkinPart_UpdateMarks redoes
// the drawn meshes. A chosen variant's links then set the options of the parts they name
// (SkinPart_ApplyVariantLink).
void SkinPart_ChoosePartVariant(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;
    int i;
    int nLinks;

    if (pSkin == NULL) return;
    if (pSkin->pModel != NULL && nPart >= 0) {
        pDesc = pSkin->pModel->pDesc;
        if (pDesc != NULL && nPart < pDesc->nParts) {
            if (nVariant < 0 || nVariant >= pDesc->pParts[nPart].nVariants) {
                nVariant = -1;
            }
            if (SkinPart_GetChangeAllCopies()) {
                // fake match: `!=` (`i < 4` unrolls to add + stw, EA's is stwx)
                for (i = 0; i != 4; i++) {
                    pSkin->aParts[i][nPart].nVariant = nVariant;
                }
                pSkin->uFlags |= 1;
            } else {
                pSkin->aParts[3][nPart].nVariant = nVariant;
            }
            if (nVariant >= 0) {
                nLinks = SkinPart_GetNumVariantLinks(pSkin, nPart, nVariant);
                for (i = 0; i < nLinks; i++) {
                    SkinPart_ApplyVariantLink(pSkin, nPart, nVariant, i);
                }
            }
        }
    }
}

// Picks a part's option (0 when any of the part's variants has fewer than nOption options) in copy
// 3, or in all four copies when SkinPart_GetChangeAllCopies, then also setting Skin.uFlags bit 1
// (see SkinPart_ChoosePartVariant). Does nothing for a part out of range.
void SkinPart_ChoosePartOption(Skin* pSkin, int nPart, int nOption) {
    SkinDesc* pDesc;
    int i;

    if (pSkin->pModel != NULL && nPart >= 0 && (pDesc = pSkin->pModel->pDesc) != NULL
        && nPart < pDesc->nParts) {
        for (i = 0; i < SkinPart_GetNumPartVariants(pSkin, nPart); i++) {
            pDesc = pSkin->pModel->pDesc;
            if (pDesc->pVariants[i + pDesc->pParts[nPart].nFirst].nOptions < nOption) {
                nOption = 0;
            }
        }
        if (SkinPart_GetChangeAllCopies()) {
            // fake match: `!=` (`i < 4` unrolls to add + stw 4(r3), EA's is addi + stwx)
            for (i = 0; i != 4; i++) {
                pSkin->aParts[i][nPart].nOption = nOption;
            }
            pSkin->uFlags |= 1;
            return;
        }
        pSkin->aParts[3][nPart].nOption = nOption;
    }
}

// A part's chosen variant in copy nCopy (copy 0 is the one drawn): -1 none or a part out of range,
// 0 when the skin has no model.
s32 SkinPart_GetPartVariant(Skin* pSkin, int nPart, int nCopy) {
    SkinDesc* pDesc;

    if (pSkin->pModel == NULL) return 0;
    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return -1;
    return pSkin->aParts[nCopy][nPart].nVariant;
}

// A part's chosen option in copy nCopy (copy 0 is the one drawn): -1 none or a part out of range, 0
// when the skin has no model.
s32 SkinPart_GetPartOption(Skin* pSkin, int nPart, int nCopy) {
    SkinDesc* pDesc;

    if (pSkin->pModel == NULL) return 0;
    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return -1;
    return pSkin->aParts[nCopy][nPart].nOption;
}

// A part's name code.
u64 SkinPart_GetPartId(Skin* pSkin, int nPart) {
    SkinDesc* pDesc;

    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return 0;
    return pDesc->pParts[nPart].uId;
}

// A variant's name code.
u64 SkinPart_GetPartVariantId(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;
    SkinPartDef* pPart;

    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return 0;
    if (nVariant < 0 || nVariant >= (pPart = &pDesc->pParts[nPart])->nVariants) return 0;
    return pDesc->pVariants[nVariant + pPart->nFirst].uId;
}

// The skin's number of sets.
s32 SkinPart_GetNumSets(Skin* pSkin) {
    if (pSkin->pModel == NULL) return 0;
    if (pSkin->pModel->pDesc != NULL) {
        return pSkin->pModel->pDesc->n70;
    }
    return 0;
}

// A set's number of variants.
s32 SkinPart_GetNumSetVariants(Skin* pSkin, int nSet) {
    SkinDesc* pDesc;

    if (nSet >= 0) {
        pDesc = pSkin->pModel->pDesc;
        if (pDesc != NULL && nSet < pDesc->n70) {
            return pDesc->p74[nSet].n08;
        }
    }
    return 0;
}

// A set variant's number of options.
s32 SkinPart_GetNumSetOptions(Skin* pSkin, int nSet, int nVariant) {
    SkinDesc* pDesc;
    s32 nFirst;

    if (pSkin != NULL && pSkin->pModel != NULL) {
        pDesc = pSkin->pModel->pDesc;
        if (pDesc != NULL && nSet < pDesc->n70 && nSet >= 0) {
            nFirst = pDesc->p74[nSet].n10;
            if (nFirst >= 0 && nFirst < pDesc->n78) {
                // fake match: the cast, as in SkinPart_GetNumPartOptions
                return ((SkinDesc7C*)pDesc->p7C)[nFirst + nVariant].n08;
            }
            return 0;
        }
    }
    return 0;
}

// Picks a set's variant (0 when out of range) and option (-1, none, when out of range) in copy 3,
// or in all four copies when SkinPart_GetChangeAllCopies. Unlike the parts' choosers it leaves
// Skin.uFlags alone.
void SkinPart_ChooseSet(Skin* pSkin, int nSet, int nVariant, int nOption) {
    int i;

    if (pSkin == NULL || nSet < 0 || nSet >= SkinPart_GetNumSets(pSkin)) return;
    if (nVariant < 0 || nVariant >= SkinPart_GetNumSetVariants(pSkin, nSet)) {
        nVariant = 0;
    }
    if (nOption < 0 || nOption >= SkinPart_GetNumSetOptions(pSkin, nSet, nVariant)) {
        nOption = -1;
    }
    if (SkinPart_GetChangeAllCopies()) {
        // fake match: `!=` (`i < 4` unrolls to add + stw, EA's is addi + stwx)
        for (i = 0; i != 4; i++) {
            pSkin->aSets[i][nSet].nVariant = nVariant;
            pSkin->aSets[i][nSet].nOption = nOption;
        }
        return;
    }
    pSkin->aSets[3][nSet].nVariant = nVariant;
    pSkin->aSets[3][nSet].nOption = nOption;
}

// A set's chosen variant in copy nCopy (copy 0 is the one drawn); -1 for a set out of range.
s32 SkinPart_GetSetVariant(Skin* pSkin, int nSet, int nCopy) {
    if (nSet < 0 || nSet >= SkinPart_GetNumSets(pSkin)) return -1;
    return pSkin->aSets[nCopy][nSet].nVariant;
}

// The texture scale-and-offset a set's variant picks (SkinDesc7C.nUVIndex):
// SkinPart_ApplySetsToMaterialEntry gives a patched p14 entry that entry of its SkinDesc.pB8 run.
// -1 when the set or the variant is out of range.
s32 SkinPart_GetSetVariantUVIndex(Skin* pSkin, int nSet, int nVariant) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;

    if (pSkin->pModel->pDesc == NULL || nSet < 0 || nSet >= SkinPart_GetNumSets(pSkin)) return -1;
    if (nVariant < 0 || nVariant >= (pSet = &(pDesc = pSkin->pModel->pDesc)->p74[nSet])->n08) return -1;
    nVariant += pSet->n10;
    return pDesc->p7C[nVariant].nUVIndex;
}

// A set's chosen option in copy nCopy (copy 0 is the one drawn); -1 none or a set out of range.
s32 SkinPart_GetSetOption(Skin* pSkin, int nSet, int nCopy) {
    if (nSet < 0 || nSet >= SkinPart_GetNumSets(pSkin)) return -1;
    return pSkin->aSets[nCopy][nSet].nOption;
}

// The data of a set's variant's option, or NULL.
u8* SkinPart_GetSetOptionData(Skin* pSkin, int nSet, int nVariant, int nOption) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;
    SkinDesc7C* pVariant;

    if (pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL) return NULL;
    if (nSet < 0 || nSet >= SkinPart_GetNumSets(pSkin)) return NULL;
    pSet = &pDesc->p74[nSet];
    if (nVariant < 0 || nVariant >= SkinPart_GetNumSetVariants(pSkin, nSet)) return NULL;
    pVariant = &pDesc->p7C[pSet->n10 + nVariant];
    if (nOption >= 0 && nOption < SkinPart_GetNumSetOptions(pSkin, nSet, nVariant)) {
        return pDesc->p8C[pVariant->n0C + nOption].aRecolor;
    }
    return NULL;
}

// A set's name code.
u64 SkinPart_GetSetId(Skin* pSkin, int nSet) {
    SkinDesc* pDesc;

    if (nSet < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nSet >= pDesc->n70) return 0;
    return pDesc->p74[nSet].uId;
}

// A set variant's name code.
u64 SkinPart_GetSetVariantId(Skin* pSkin, int nSet, int nVariant) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;

    if (nSet < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nSet >= pDesc->n70) return 0;
    if (nVariant < 0 || nVariant >= (pSet = &pDesc->p74[nSet])->n08) return 0;
    return pDesc->p7C[nVariant + pSet->n10].uId;
}

// Allocates the four copies of the skin's choices: parts zeroed (variant 0, option 0), sets on
// their "Defaults" variant (or the first) and option 0 (-1 when the set's first variant has none).
void SkinPart_AllocChoices(Skin* pSkin) {
    int j;
    int i;
    s32 nParts;
    s32 nSets;
    u32 nBytes;
    s32 nVariant;

    nParts = SkinPart_GetNumParts(pSkin);
    SkinPart_GetNumSets(pSkin);
    nBytes = nParts * sizeof(SkinChoice);
    for (i = 0; i < 4; i++) {
        if (nParts != 0) {
            pSkin->aParts[i] = StaticMem_Alloc(nBytes, 2, 16, "SkinPart.c", 655);
            memset(pSkin->aParts[i], 0, nBytes);
        } else {
            pSkin->aParts[i] = NULL;
        }
        nSets = SkinPart_GetNumSets(pSkin);
        if (nSets != 0) {
            // nSets * 8 is nSets SkinChoices; an int product here, so it is not shared with the
            // memset's size (sizeof makes that one unsigned)
            pSkin->aSets[i] = StaticMem_Alloc(nSets * 8, 2, 16, "SkinPart.c", 670);
            memset(pSkin->aSets[i], 0, nSets * sizeof(SkinChoice));
            for (j = 0; j < nSets; j++) {
                if (SkinPart_GetNumSetOptions(pSkin, j, 0) >= 1) {
                    pSkin->aSets[i][j].nOption = 0;
                } else {
                    pSkin->aSets[i][j].nOption = -1;
                }
                nVariant = SkinPart_FindSetVariantByName(pSkin, j, "Defaults");
                if (nVariant < 0) {
                    nVariant = 0;
                }
                pSkin->aSets[i][j].nVariant = nVariant;
            }
        } else {
            pSkin->aSets[i] = NULL;
        }
    }
}

// Frees the four copies of the skin's choices.
void SkinPart_FreeChoices(Skin* pSkin) {
    int i;

    for (i = 0; i < 4; i++) {
        if (pSkin->aParts[i] != NULL) {
            StaticMem_Free(pSkin->aParts[i]);
        }
        if (pSkin->aSets[i] != NULL) {
            StaticMem_Free(pSkin->aSets[i]);
        }
    }
}

// The bytes option n (a SkinDesc.p5C entry) needs for its meshes with flags 0x300000
// (SkinMesh.nSize added up).
s32 SkinPart_GetOptionSize(SkinDesc* pDesc, int n) {
    u8 aBuf[0x48];
    SkinIterArgs args;
    SkinIter* pIter;
    SkinMesh* pMesh;
    s32 nBytes;

    args.pDesc = pDesc;
    args.nEntry = n;
    pIter = fn_80113B34(aBuf, &args);
    nBytes = 0;
    for (; SkinIter_IsValid(pIter); SkinIter_Next(pIter)) {
        pMesh = SkinIter_GetMesh(pIter);
        if ((pMesh->uFlags & 0x300000) == 0x300000) {
            nBytes += pMesh->nSize;
        }
    }
    fn_80113BAC(pIter);
    return nBytes;
}

// The most bytes any option of any of the part's variants needs (SkinPart_GetOptionSize).
s32 SkinPart_GetMaxPartOptionSize(SkinDesc* pDesc, int nPart) {
    SkinPartDef* pPart;
    SkinVariant* pVariant;
    s32 nMax;
    int i;
    int j;
    s32 nBytes;

    nMax = 0;
    pPart = &pDesc->pParts[nPart];
    for (i = 0; i < pPart->nVariants; i++) {
        pVariant = &pDesc->pVariants[pPart->nFirst + i];
        for (j = 0; j < pVariant->nOptions; j++) {
            nBytes = SkinPart_GetOptionSize(pDesc, pVariant->nFirstOption + j);
            if (nMax < nBytes) {
                nMax = nBytes;
            }
        }
    }
    return nMax;
}

// The bytes the skin's chosen options can need at most: each part's SkinPart_GetMaxPartOptionSize
// added up, but no more than all of SkinDesc.p34's meshes with flags 0x300000 together. Skin.c
// (SKN_AllocRenderData) calls it and ignores the result.
s32 SkinPart_GetMaxOptionsSize(Skin* pSkin) {
    SkinDesc* pDesc;
    s32 nBytes;
    int i;
    s32 nAll;
    int j;

    nBytes = 0;
    pDesc = pSkin->pModel->pDesc;
    if (pDesc == NULL) return 0;
    for (i = 0; i < pDesc->nParts; i++) {
        nBytes += SkinPart_GetMaxPartOptionSize(pDesc, i);
    }
    nAll = 0;
    for (j = 0; j < pDesc->nOverrideMeshes; j++) {
        if ((pDesc->p34[j].uFlags & 0x300000) == 0x300000) {
            nAll += pDesc->p34[j].nSize;
        }
    }
    if (nBytes > nAll) {
        nBytes = nAll;
    }
    return nBytes;
}

// The aMeshBits bit for the mesh iterator's index n: n itself (pSkin is unused).
s32 SkinPart_GetMeshBit(Skin* pSkin, s32 n) {
    return n;
}

// Marks in aMtxBits the matrices the mesh uses (its n8 SkinMeshBit entries; a bit per SkinModel.p54
// blended matrix, which Skin.c computes only when marked).
void SkinPart_MarkMeshMatrices(Skin* pSkin, SkinMesh* pMesh) {
    SkinMeshBit* pBit;
    int i;

    pBit = pMesh->pBits;
    for (i = 0; i < pMesh->n8; i++) {
        BitArray_SetBit(pSkin->aMtxBits, pBit->nBit);
        pBit++;
    }
}

// Marks what option n (a SkinDesc.p5C entry) needs: its meshes with flags 0x300000 in aMeshBits (what
// Skin.c draws; skipped while the skin has no aMeshBits) and, for its meshes with flags 1 and 0x10,
// their matrices in aMtxBits. Nothing for n out of range.
void SkinPart_MarkOption(Skin* pSkin, int n) {
    u8 aBuf[0x38];              // the iterator's work space; its real size is not known
    SkinIterArgs args;
    SkinMesh* pMesh;
    SkinIter* pIter;
    SkinDesc* pDesc;

    pDesc = pSkin->pModel->pDesc;
    if (n < 0 || n >= pDesc->n58) return;
    args.nEntry = n;
    args.pDesc = pDesc;
    for (pIter = fn_80113B34(aBuf, &args); SkinIter_IsValid(pIter); SkinIter_Next(pIter)) {
        pMesh = SkinIter_GetMesh(pIter);
        if ((pMesh->uFlags & 0x300000) == 0x300000) {
            n = SkinPart_GetMeshBit(pSkin, SkinIter_GetIndex(pIter));
            if (pSkin->aMeshBits != NULL) {
                BitArray_SetBit(pSkin->aMeshBits, n);
            }
        }
        if ((pMesh->uFlags & 1) && (pMesh->uFlags & 0x10)) {
            SkinPart_MarkMeshMatrices(pSkin, pMesh);
        }
    }
    fn_80113BAC(pIter);
}

// Marks what a part's chosen option in copy 0 needs (SkinPart_MarkOption); nothing while its
// variant or option is -1.
void SkinPart_MarkPart(Skin* pSkin, int nPart) {
    SkinDesc* pDesc;
    s32 nVariant;
    s32 nOption;

    nVariant = SkinPart_GetPartVariant(pSkin, nPart, 0);
    nOption = SkinPart_GetPartOption(pSkin, nPart, 0);
    pDesc = pSkin->pModel->pDesc;
    if (nVariant < 0 || nOption < 0) return;
    SkinPart_MarkOption(pSkin,
                        nOption + pDesc->pVariants[nVariant + pDesc->pParts[nPart].nFirst].nFirstOption);
}

// Marks what every option of the skin needs (each SkinDesc.p5C entry), for SkinBurn
// SkinBurn_DropUnusedMatrices, which drops the matrices none of them uses.
void SkinPart_MarkAllOptions(Skin* pSkin) {
    SkinDesc* pDesc;
    int i;
    s32 n;

    if (pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL) return;
    n = pDesc->n58;
    for (i = 0; i < n; i++) {
        SkinPart_MarkOption(pSkin, i);
    }
}

// Once the choices changed (Skin.uFlags bit 1, which it clears): clears both bit arrays and marks
// again what each part's chosen option in copy 0 needs (SkinPart_MarkPart). Skin.c calls it before
// it computes the skin's matrices.
void SkinPart_UpdateMarks(Skin* pSkin) {
    int i;

    if (pSkin->uFlags & 1) {
        pSkin->uFlags &= ~1;
        BitArray_ClearArray(pSkin->aMtxBits, pSkin->pModel->n50);
        if (pSkin->aMeshBits != NULL) {
            BitArray_ClearArray(pSkin->aMeshBits, pSkin->pModel->n40);
        }
        for (i = 0; i < SkinPart_GetNumParts(pSkin); i++) {
            SkinPart_MarkPart(pSkin, i);
        }
    }
}

// The part with this name code, or -1.
s32 SkinPart_FindPart(Skin* pSkin, u64 uId) {
    s32 n;
    int i;

    n = SkinPart_GetNumParts(pSkin);
    for (i = 0; i < n; i++) {
        if (SkinPart_GetPartId(pSkin, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The part with this name, or -1.
s32 SkinPart_FindPartByName(Skin* pSkin, const char* pName) {
    u64 uId;

    SKA_PackName(&uId, pName);
    return SkinPart_FindPart(pSkin, uId);
}

// The part's variant with this name code, or -1.
s32 SkinPart_FindPartVariant(Skin* pSkin, int nPart, u64 uId) {
    s32 n;
    int i;

    n = SkinPart_GetNumPartVariants(pSkin, nPart);
    for (i = 0; i < n; i++) {
        if (SkinPart_GetPartVariantId(pSkin, nPart, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The set with this name code, or -1.
s32 SkinPart_FindSet(Skin* pSkin, u64 uId) {
    s32 n;
    int i;

    n = SkinPart_GetNumSets(pSkin);
    for (i = 0; i < n; i++) {
        if (SkinPart_GetSetId(pSkin, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The set with this name, or -1.
s32 SkinPart_FindSetByName(Skin* pSkin, const char* pName) {
    u64 uId;

    SKA_PackName(&uId, pName);
    return SkinPart_FindSet(pSkin, uId);
}

// The set's variant with this name code, or -1.
s32 SkinPart_FindSetVariant(Skin* pSkin, int nSet, u64 uId) {
    s32 n;
    int i;

    n = SkinPart_GetNumSetVariants(pSkin, nSet);
    for (i = 0; i < n; i++) {
        if (SkinPart_GetSetVariantId(pSkin, nSet, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The set's variant with this name, or -1.
s32 SkinPart_FindSetVariantByName(Skin* pSkin, int nSet, const char* pName) {
    u64 uId;

    SKA_PackName(&uId, pName);
    return SkinPart_FindSetVariant(pSkin, nSet, uId);
}

// The option of variant nVariant of set nSet with this name code, or -1. 0 (not -1) when the skin
// has no description or the set or the variant is out of range.
s32 SkinPart_FindSetOption(Skin* pSkin, int nSet, int nVariant, u64 uId) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;
    SkinDesc7C* pVariant;
    int i;
    s32 nFirst;

    if (nSet < 0 || pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL
        || nSet >= pDesc->n70 || nSet < 0) {
        return 0;
    }
    if (nVariant < 0 || nVariant >= (pSet = &pDesc->p74[nSet])->n08) return 0;
    pVariant = &pDesc->p7C[nVariant + pSet->n10];
    nFirst = pVariant->n0C;
    for (i = 0; i < pVariant->n08; i++) {
        if (pDesc->p8C[nFirst + i].uId == uId) {
            return i;
        }
    }
    return -1;
}

// The option of variant nVariant of set nSet with this name, or -1 (also for no name); 0 when the
// set or the variant is out of range (SkinPart_FindSetOption).
s32 SkinPart_FindSetOptionByName(Skin* pSkin, int nSet, int nVariant, const char* pName) {
    u64 uId;

    if (pName == NULL) return -1;
    SKA_PackName(&uId, pName);
    return SkinPart_FindSetOption(pSkin, nSet, nVariant, uId);
}

// The number of links (SkinLink) of variant nVariant of part nPart: the other parts' options it
// sets when chosen. 0 with no skin or a variant out of range.
s32 SkinPart_GetNumVariantLinks(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;

    if (pSkin == NULL || nVariant < 0 || nVariant >= SkinPart_GetNumPartVariants(pSkin, nPart)) return 0;
    pDesc = pSkin->pModel->pDesc;
    return pDesc->pVariants[nVariant + pDesc->pParts[nPart].nFirst].nLinks;
}

// fake match: an identity read; gives EA's operand order for add r0, r0, r6.
static inline int fn_800CDF80_Read(int n) { return n; }

// Applies link nLink of variant nVariant of part nPart: the part the link names (when the skin has
// it) gets the link's option (SkinPart_ChoosePartOption). SkinPart_ChoosePartVariant applies each
// link of the variant it picks, so choosing "GloveOff" can change another part too.
void SkinPart_ApplyVariantLink(Skin* pSkin, int nPart, int nVariant, int nLink) {
    SkinDesc* pDesc;
    SkinLink* pLink;
    s32 nOther;

    if (pSkin == NULL) return;
    pDesc = pSkin->pModel->pDesc;
    // fake match: the cast, as in SkinPart_GetNumPartOptions
    pLink = &pDesc->pLinks[((SkinVariant*)pDesc->pVariants)[nVariant + pDesc->pParts[nPart].nFirst].nFirstLink
                           + fn_800CDF80_Read(nLink)];
    nOther = SkinPart_FindPart(pSkin, pLink->uPart);
    if (nOther >= 0 && nOther < SkinPart_GetNumParts(pSkin)) {
        SkinPart_ChoosePartOption(pSkin, nOther, pLink->nOption);
    }
}

// Starts drawing the skin's parts for view nView (Skin.c: then SkinPart_DrawPart for each part and
// SkinPart_EndDraw): once the skin is loaded (uFlags & 2) and has a description, flushes the render
// state with clipping on and the camera's matrices, resets the skin renderer (hwsRender_Gc.c) and
// hands it the description and the view's mesh overrides (a10A0[nView]).
void SkinPart_BeginDraw(Skin* pSkin, int nView) {
    if (pSkin->pModel->pDesc != NULL && (pSkin->uFlags & 2)) {
        RenderState_SetClipMode(1);
        RenderState_SetCameraMatrices();
        RenderState_Flush();
        // port: EA passes an argument fn_8011387C ignores
        ((void (*)(int))fn_8011387C)(0x400);
        fn_801138CC(pSkin->pModel->pDesc);
        fn_801138D8(pSkin->apOverride[nView]);
    }
}

// Draws part nPart of a loaded skin as copy 0 (the one shown) chooses it: the meshes of its chosen
// variant's chosen option (hwsRender_Gc.c fn_80113774); nothing when it has no variant. Between
// SkinPart_BeginDraw and SkinPart_EndDraw.
void SkinPart_DrawPart(Skin* pSkin, int nPart) {
    s32 nVariant;
    s32 nOption;

    if (pSkin->uFlags & 2) {
        nVariant = SkinPart_GetPartVariant(pSkin, nPart, 0);
        nOption = SkinPart_GetPartOption(pSkin, nPart, 0);
        if (nVariant != -1) {
            fn_80113774(nPart, nVariant, nOption);
        }
    }
}

// Ends drawing a loaded skin's parts (SkinPart_BeginDraw): hwsRender_Gc.c's end step (fn_8011389C).
void SkinPart_EndDraw(Skin* pSkin) {
    if (pSkin->pModel->pDesc != NULL && (pSkin->uFlags & 2)) {
        fn_8011389C();
    }
}

// Empty. Skin.c calls it (passing the skin) as it sets up a loaded skin (SKN_AllocRenderData, when asked).
void SkinPart_InitSkin(void) {
}

// Empty. Skin.c calls it (passing the skin) as it frees what a loaded skin allocated (SKN_FreeRenderData).
void SkinPart_ShutdownSkin(void) {
}

// Empty. Skin.c calls it (passing the body skin and the view) after posing the skin for a view
// (SKN_PoseCharacter).
void SkinPart_UpdateSkin(void) {
}

// Sets up the skin's materials (SkinDesc.p18) from their entries (p14) as copy 0's chosen sets
// patch them (SkinPart_ApplySetsToMaterialEntry), with textures from the character's dynamic
// textures (pTarget is a DynTex; its p4 has TexBank's layout). char.c and FEgolferanim.c call it
// for each skin once new textures are in.
void SkinPart_SetupMaterials(Skin* pSkin, SkinTarget* pTarget) {
    SkinDesc* pDesc;
    SkinDesc14 entry;
    int i;

    if (pSkin == NULL || pSkin->pModel == NULL) return;
    pDesc = pSkin->pModel->pDesc;
    if (pDesc != NULL) {
        for (i = 0; i < pDesc->n10; i++) {
            Mem_cpy(&entry, &pDesc->p14[i], sizeof(SkinDesc14));
            SkinPart_ApplySetsToMaterialEntry(pSkin, &entry, NULL, NULL, 0);
            fn_80112614(&entry, &pDesc->p18[i], pTarget->pBank);
        }
    }
}

// fake match: keep the set loop's counter as an inlined local so its register follows the
// pointer-induction temporary; the loop body and all writes are unchanged.
static inline void fn_800CE224_Loop(Skin* pSkin, SkinDesc14* pEntry, SkinDesc* pDesc, int nCopy,
                                    int* pj, SkinDesc74** ppSet, s32* pbChanged, s32* pnUV,
                                    int* pnVariant) {
    int i;
    int k;
    SkinDesc7C* pVariant;
    s32 nOption;
    s32 n;

    // fake match: the repeated nUV zero and parallel k counter keep EA's induction zero loads.
    for (i = (*pnUV = 0), k = 0; i < pDesc->n70; k++, i++) {
        (*ppSet) = &pDesc->p74[k];
        for ((*pj) = 0; (*pj) < (*ppSet)->n0C; (*pj)++) {
            if (pEntry->uId == pDesc->p84[(*ppSet)->n14 + (*pj)]) {
                (*pnVariant) = SkinPart_GetSetVariant(pSkin, i, nCopy);
                nOption = SkinPart_GetSetOption(pSkin, i, nCopy);
                n = SkinPart_GetSetVariantUVIndex(pSkin, i, (*pnVariant));
                if (n >= 0) {
                    (*pnUV) = n;
                }
                if ((*pnVariant) >= 0 && (*pnVariant) < SkinPart_GetNumSetVariants(pSkin, i)) {
                    pVariant = &pDesc->p7C[(*ppSet)->n10 + (*pnVariant)];
                    if (nOption >= 0 && nOption < SkinPart_GetNumSetOptions(pSkin, i, (*pnVariant))) {
                        pEntry->n18 = pVariant->n0C + nOption;
                    } else {
                        pEntry->n18 = -1;
                    }
                    if ((*pnVariant) > 0) {
                        (*pbChanged) = 1;
                        pEntry->uId = pDesc->p84[(*pnVariant) * (*ppSet)->n0C + (*pj) + (*ppSet)->n14];
                    }
                }
            }
        }
    }
}

// Patches a material entry (a copy of a SkinDesc.p14 entry) for the sets as copy nCopy chooses
// them. A set lists texture name codes per variant (SkinDesc.p84, n0C per variant); an entry using
// one of the first variant's names takes the chosen variant's name in its place and the chosen
// option's SkinDesc.p8C entry (n18; -1 when the option is out of range). With u08 & 2 it also takes
// its texture scale and offset (a20) from pB8, at the variant's UV index (0 when out of range).
// With ppOut and pnOut it gives the option entry's recolouring (a08 and n2C), or NULL in *ppOut.
// Returns whether the name changed or a recolouring was given.
s32 SkinPart_ApplySetsToMaterialEntry(Skin* pSkin, SkinDesc14* pEntry, u8** ppOut, s32* pnOut, int nCopy) {
    SkinDesc* pDesc;
    int j;
    int nVariant;
    SkinDesc74* pSet;
    s32 bChanged;
    s32 nUV;
    SkinDescB8* pB8;

    bChanged = 0;
    if (pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL || pEntry == NULL) {
        return 0;
    }
    nUV = 0;
    fn_800CE224_Loop(pSkin, pEntry, pDesc, nCopy, &j, &pSet, &bChanged, &nUV, &nVariant);
    if (pEntry->u08 & 2) {
        if (pEntry->n16 > 0) {
            if (nUV < 0 || nUV >= pEntry->n16) {
                nUV = 0;
            }
            pB8 = &pDesc->pB8[pEntry->n1C + nUV];
            pEntry->a20 = *pB8;
        }
    }
    if (ppOut != NULL && pnOut != NULL) {
        if (pEntry->n18 >= 0 && pEntry->n18 < pDesc->n88) {
            bChanged = 1;
            *ppOut = pDesc->p8C[pEntry->n18].aRecolor;
            *pnOut = pDesc->p8C[pEntry->n18].nRecolorMode;
        } else {
            *ppOut = NULL;
        }
    }
    return bChanged;
}

// Adds a texture name code to a texture list, with the recolouring (a set option's
// SkinDesc8C.aRecolor and nRecolorMode) it is to be loaded with, unless the list has that name
// already.
void SkinPart_AddToTexList(u64 uId, SkinListEntry* aList, s32* pnList, u8* pRecolor, s32 nMode) {
    int i;

    for (i = 0; i < *pnList; i++) {
        if (aList[i].uId == uId) return;
    }
    aList[*pnList].uId = uId;
    aList[*pnList].pRecolor = pRecolor;
    aList[*pnList].nRecolorMode = nMode;
    (*pnList)++;
}

// fake match: SkinPart_ListOptionTextures's inner loop as an inline, everything it writes passed
// by pointer; its pIndex parameter is a frontend variable, numbered after the hoisted p5C offset
// (EA's register order). Same statements, same order.
static inline void fn_800CE52C_Loop(Skin* pSkin, SkinDesc* pDesc, s32* pIndex, s32 nIndices,
                                    SkinListEntry* aList, s32* pnList, int nCopy, SkinDesc14* pEntry,
                                    u8** ppRecolor, s32* pnMode, int* pj) {
    for (*pj = 0; *pj < nIndices; (*pj)++) {
        if (!(pDesc->p14[*pIndex].u08 & 1)) {
            Mem_cpy(pEntry, &pDesc->p14[*pIndex], sizeof(SkinDesc14));
            *ppRecolor = NULL;
            SkinPart_ApplySetsToMaterialEntry(pSkin, pEntry, ppRecolor, pnMode, nCopy);
            SkinPart_AddToTexList(pEntry->uId, aList, pnList, *ppRecolor, *pnMode);
        }
        pIndex++;
    }
}

// Adds to a texture list the textures option n (a SkinDesc.p5C entry) draws with: the material
// entries of its meshes (p6C to p44 to p20 to p14; entries with u08 & 1 skipped), each patched for
// copy nCopy's chosen sets (SkinPart_ApplySetsToMaterialEntry) and with its set option's
// recolouring.
void SkinPart_ListOptionTextures(Skin* pSkin, int n, SkinListEntry* aList, s32* pnList, int nCopy) {
    SkinDesc14 entry;
    u8* pRecolor;
    s32 nMode;
    SkinDesc44* p44;
    s32 nFirst;
    int i;
    int j;
    s32 nIndices;
    s32 n44;
    SkinDesc* pDesc;

    pDesc = pSkin->pModel->pDesc;
    nFirst = pDesc->p5C[n].n4;
    for (i = 0; i < pDesc->p5C[n].n0; i++) {
        n44 = pDesc->p6C[nFirst + i];
        if (n44 >= 0) {
            p44 = &pDesc->p44[n44];
            if (p44->n8 != 0) {
                nIndices = pDesc->p28[p44->nC].n0;
                fn_800CE52C_Loop(pSkin, pDesc, &pDesc->p20[p44->n4], nIndices, aList, pnList, nCopy,
                                 &entry, &pRecolor, &nMode, &j);
            }
        }
    }
}

// Lists the textures the skins' parts need as copy nCopy chooses them (each part's chosen variant
// and option; for a part whose name code is in aIds, every option of every variant), each once with
// its recolouring, in a new *ppList (StaticMem; the caller frees it). Returns the list's length.
s32 SkinPart_ListChosenTextures(Skin** apSkins, int nSkins, SkinListEntry** ppList, u64* aIds, int nIds,
                                int nCopy) {
    int j;
    s32 nList;
    Skin* pSkin;
    SkinDesc* pDesc;
    u8 bAll;
    int m;
    int i;
    int k;
    int v;
    s32 n;
    s32 nVariant;
    s32 nOption;

    n = 0;
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL && apSkins[i]->pModel->pDesc != NULL) {
            n += apSkins[i]->pModel->pDesc->n10;
        }
    }
    nList = 0;
    *ppList = StaticMem_Alloc(n * sizeof(SkinListEntry), 1, 0, "SkinPart.c", 1980);
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL && apSkins[i]->pModel->pDesc != NULL) {
            pSkin = apSkins[i];
            pDesc = pSkin->pModel->pDesc;
            for (j = 0; j < SkinPart_GetNumParts(pSkin); j++) {
                bAll = 0;
                nVariant = SkinPart_GetPartVariant(pSkin, j, nCopy);
                nOption = SkinPart_GetPartOption(pSkin, j, nCopy);
                if (aIds != NULL) {
                    for (k = 0; k < nIds; k++) {
                        if (aIds[k] == SkinPart_GetPartId(pSkin, j)) {
                            bAll = 1;
                            break;
                        }
                    }
                }
                if (bAll == 1) {
                    for (v = 0; v < SkinPart_GetNumPartVariants(pSkin, j); v++) {
                        for (m = 0; m < SkinPart_GetNumPartOptions(pSkin, j, v); m++) {
                            SkinPart_ListOptionTextures(
                                pSkin, m + pDesc->pVariants[v + pDesc->pParts[j].nFirst].nFirstOption,
                                *ppList, &nList, nCopy);
                        }
                    }
                } else if (nVariant >= 0 && nOption >= 0) {
                    SkinPart_ListOptionTextures(
                        pSkin, nOption + pDesc->pVariants[nVariant + pDesc->pParts[j].nFirst].nFirstOption,
                        *ppList, &nList, nCopy);
                }
            }
        }
    }
    return nList;
}

// fake match: an identity read; it gives EA's register order.
static inline SkinDesc* fn_800CE8C0_Read(SkinDesc* p) { return p; }

// Lists every texture any option of the skins' parts can use (patched for copy 0's chosen sets),
// each once with its recolouring, in a new *ppList (StaticMem; the caller frees it). Returns the
// list's length. Character_LoadTextures loads only these outside the front end.
s32 SkinPart_ListAllTextures(Skin** apSkins, int nSkins, SkinListEntry** ppList) {
    s32 nList;
    int i;
    int j;
    int k;
    int m;
    Skin* pSkin;
    SkinDesc* pDesc;
    s32 n;

    n = 0;
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL && apSkins[i]->pModel->pDesc != NULL) {
            n += apSkins[i]->pModel->pDesc->n10;
        }
    }
    *ppList = StaticMem_Alloc(n * sizeof(SkinListEntry), 1, 0, "SkinPart.c", 2068);
    nList = 0;
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL &&
            (pDesc = fn_800CE8C0_Read(apSkins[i]->pModel->pDesc)) != NULL) {
            pSkin = apSkins[i];
            for (j = 0; j < SkinPart_GetNumParts(pSkin); j++) {
                for (k = 0; k < SkinPart_GetNumPartVariants(pSkin, j); k++) {
                    n = k + pDesc->pParts[j].nFirst;
                    for (m = 0; m < pDesc->pVariants[n].nOptions; m++) {
                        SkinPart_ListOptionTextures(pSkin, m + pDesc->pVariants[n].nFirstOption, *ppList,
                                                    &nList, 0);
                    }
                }
            }
        }
    }
    return nList;
}

// Whether the texture list holds the name code.
u8 SkinPart_TexListHas(u64 uId, SkinListEntry* aList, int nList) {
    int i;

    for (i = 0; i < nList; i++) {
        if (aList[i].uId == uId) return 1;
    }
    return 0;
}

u64 SkinPart_TexListGetId(int i, SkinListEntry* aList, int nList) {
    if (nList <= 0 || aList == NULL || i < 0 || i >= nList) return 0;
    return aList[i].uId;
}

// Entry i's recolouring data (a set option's SkinDesc8C.aRecolor, which LLDynTex.c applies to the
// texture's pixels), or NULL (also for i out of range).
u8* SkinPart_TexListGetRecolor(int i, SkinListEntry* aList, int nList) {
    if (nList <= 0 || aList == NULL || i < 0 || i >= nList) return NULL;
    return aList[i].pRecolor;
}

// Entry i's recolouring mode (the set option's SkinDesc8C.nRecolorMode), or 0 (also for i out of
// range).
s32 SkinPart_TexListGetRecolorMode(int i, SkinListEntry* aList, int nList) {
    if (nList <= 0 || aList == NULL || i < 0 || i >= nList) return 0;
    return aList[i].nRecolorMode;
}

// Drops from the dynamic textures pTex every texture the skins' parts do not use as copy 2 chooses
// them (LLDynTex.c fn_8010AD50 clears its name, fn_8010ADA4 packs the rest). char.c's begin
// callbacks of a texture load call it once the newest choices are in copy 2.
void SkinPart_DropUnusedTextures(Skin** apSkins, int nSkins, DynTex* pTex) {
    int i;
    SkinListEntry* pList;
    int nList;
    s32 n;
    u64 uId;

    if (pTex == NULL) return;
    nList = SkinPart_ListChosenTextures(apSkins, nSkins, &pList, NULL, 0, 2);
    n = fn_8010AD10(pTex);
    for (i = 0; i < n; i++) {
        uId = fn_8010AD18(pTex, i);
        if (uId != 0 && !SkinPart_TexListHas(uId, pList, nList)) {
            fn_8010AD50(pTex, uId);
        }
    }
    fn_8010ADA4(pTex);
    if (pList != NULL) {
        StaticMem_Free(pList);
    }
}

// Queues for loading (LLDynTex.c fn_8010BCFC) each texture the skins' parts need as copy 2 chooses
// them that pTex does not hold yet, with its recolouring. The parts whose name codes are in aIds
// (nIds of them) get every option's textures: the in-game load passes the "Glove" part, so the
// glove can come off and go back on.
void SkinPart_QueueMissingTextures(Skin** apSkins, int nSkins, DynTex* pTex, u64* aIds, int nIds) {
    SkinListEntry* pList;
    s32 nMode;
    int i;
    int nList;
    u64 uId;
    TexBank* pBank;

    pList = NULL;
    if (pTex == NULL) return;
    nList = SkinPart_ListChosenTextures(apSkins, nSkins, &pList, aIds, nIds, 2);
    // port: a DynTexHeader has TexBank's layout (lldyntex.h); the two are not merged yet.
    pBank = (TexBank*)fn_8010A780(pTex);
    for (i = 0; i < nList; i++) {
        uId = SkinPart_TexListGetId(i, pList, nList);
        if (uId != 0 && fn_8001005C(pBank, uId) == -0x80000000) {
            nMode = SkinPart_TexListGetRecolorMode(i, pList, nList);
            fn_8010BCFC(uId, SkinPart_TexListGetRecolor(i, pList, nList), nMode);
        }
    }
    if (pList != NULL) {
        StaticMem_Free(pList);
    }
}

// Drops from the dynamic textures pTex the textures variant nVariant of set nSet names
// (SkinDesc.p84), so they load again with the new option's recolouring. Nothing when the set, the
// variant or nOption is out of range (nOption is only checked). char.c's texture swap and
// FE_CrAPDB.c call it as a set's choice changes.
void SkinPart_DropSetVariantTextures(Skin* pSkin, int nSet, int nVariant, int nOption, DynTex* pTex) {
    int i;
    SkinDesc74* pSet;
    SkinDesc* pDesc;

    if (pSkin == NULL || pSkin->pModel == NULL || nSet < 0 || nSet >= SkinPart_GetNumSets(pSkin)) return;
    if (nVariant < 0 || nVariant >= SkinPart_GetNumSetVariants(pSkin, nSet)) return;
    if (nOption < 0 || nOption >= SkinPart_GetNumSetOptions(pSkin, nSet, nVariant)) return;
    pDesc = pSkin->pModel->pDesc;
    pSet = &pDesc->p74[nSet];
    for (i = 0; i < pSet->n0C; i++) {
        SkinPart_DropTexture(pSkin, pTex, pDesc->p84[i + nVariant * pSet->n0C + pSet->n14]);
    }
}

// Drops the texture with this name code from the dynamic textures pTex (LLDynTex.c fn_8010AD50);
// pSkin is not used.
void SkinPart_DropTexture(Skin* pSkin, DynTex* pTex, u64 uId) {
    fn_8010AD50(pTex, uId);
}

// Copies one copy of the skin's choices over another.
void SkinPart_CopyChoices(Skin* pSkin, int nFrom, int nTo) {
    Mem_cpy(pSkin->aParts[nTo], pSkin->aParts[nFrom], SkinPart_GetNumParts(pSkin) * sizeof(SkinChoice));
    Mem_cpy(pSkin->aSets[nTo], pSkin->aSets[nFrom], SkinPart_GetNumSets(pSkin) * sizeof(SkinChoice));
}

// Sets whether a new choice goes into all four copies of a skin's choices at once (set) or only
// into copy 3, the newest, from which char.c passes it down copy by copy (3 to 2 to 1 to 0) as the
// textures load.
void SkinPart_SetChangeAllCopies(u8 b) {
    gSkinChangeAllCopies = b;
}

// Whether a new choice goes into all four copies of a skin's choices (see
// SkinPart_SetChangeAllCopies).
u8 SkinPart_GetChangeAllCopies(void) {
    return gSkinChangeAllCopies;
}

// Turns SkinPart_SetChangeAllCopies on, the default (SkinPart_Init).
void SkinPart_InitChangeAllCopies(void) {
    SkinPart_SetChangeAllCopies(1);
}

// Empty. char.c's Character_LoadTextures calls it (passing the body skin, its new texture bank and
// two sizes, 0xBF600 and 0xCDA) once the bank is set up.
void SkinPart_InitTextures(void) {
}

// Whether the iterator is on an entry; false once it has stepped past the last one.
u8 SkinIter_IsValid(SkinIter* pIter) {
    return pIter->bValid;
}

// Steps the iterator to its next entry through its kind's step function.
void SkinIter_Next(SkinIter* pIter) {
    (*pIter->ppfnNext)(pIter);
}

SkinMesh* SkinIter_GetMesh(SkinIter* pIter) {
    return pIter->pCur;
}

// The current entry's index in the description table the iterator walks (SkinDesc.p34 for a mesh).
s32 SkinIter_GetIndex(SkinIter* pIter) {
    return pIter->nCur;
}

// Makes a loaded description usable: its offsets become pointers (once). One of another version
// is cleared instead.
void SkinPart_FixupDesc(SkinDesc* pDesc) {
    // port: the file holds offsets from its start where the struct holds pointers
    if (pDesc->uFlags & 1) return;
    if (pDesc->nVersion != 8) {
        memset(pDesc, 0, sizeof(SkinDesc));
        pDesc->nVersion = 8;
        pDesc->n04 = 1;
        pDesc->uFlags = 1;
        return;
    }
    if (pDesc->p14 != NULL) {
        pDesc->p14 = (SkinDesc14*)((u8*)pDesc + (uptr)pDesc->p14);
    }
    if (pDesc->p18 != NULL) {
        pDesc->p18 = (SkinDesc18*)((u8*)pDesc + (uptr)pDesc->p18);
    }
    if (pDesc->p20 != NULL) {
        pDesc->p20 = (s32*)((u8*)pDesc + (uptr)pDesc->p20);
    }
    if (pDesc->p28 != NULL) {
        pDesc->p28 = (SkinDesc28*)((u8*)pDesc + (uptr)pDesc->p28);
    }
    if (pDesc->p34 != NULL) {
        pDesc->p34 = (SkinMesh*)((u8*)pDesc + (uptr)pDesc->p34);
    }
    if (pDesc->p3C != NULL) {
        pDesc->p3C = (s32*)((u8*)pDesc + (uptr)pDesc->p3C);
    }
    if (pDesc->p44 != NULL) {
        pDesc->p44 = (SkinDesc44*)((u8*)pDesc + (uptr)pDesc->p44);
    }
    if (pDesc->pParts != NULL) {
        pDesc->pParts = (SkinPartDef*)((u8*)pDesc + (uptr)pDesc->pParts);
    }
    if (pDesc->pVariants != NULL) {
        pDesc->pVariants = (SkinVariant*)((u8*)pDesc + (uptr)pDesc->pVariants);
    }
    if (pDesc->p5C != NULL) {
        pDesc->p5C = (SkinDesc5C*)((u8*)pDesc + (uptr)pDesc->p5C);
    }
    if (pDesc->pLinks != NULL) {
        pDesc->pLinks = (SkinLink*)((u8*)pDesc + (uptr)pDesc->pLinks);
    }
    if (pDesc->p6C != NULL) {
        pDesc->p6C = (s32*)((u8*)pDesc + (uptr)pDesc->p6C);
    }
    if (pDesc->p74 != NULL) {
        pDesc->p74 = (SkinDesc74*)((u8*)pDesc + (uptr)pDesc->p74);
    }
    if (pDesc->p7C != NULL) {
        pDesc->p7C = (SkinDesc7C*)((u8*)pDesc + (uptr)pDesc->p7C);
    }
    if (pDesc->p84 != NULL) {
        pDesc->p84 = (u64*)((u8*)pDesc + (uptr)pDesc->p84);
    }
    if (pDesc->p9C != NULL) {
        pDesc->p9C = (u8*)pDesc + (uptr)pDesc->p9C;
    }
    if (pDesc->p8C != NULL) {
        pDesc->p8C = (SkinDesc8C*)((u8*)pDesc + (uptr)pDesc->p8C);
    }
    if (pDesc->pA4 != NULL) {
        pDesc->pA4 = (s16*)((u8*)pDesc + (uptr)pDesc->pA4);
    }
    if (pDesc->pAC != NULL) {
        pDesc->pAC = (s16*)((u8*)pDesc + (uptr)pDesc->pAC);
    }
    if (pDesc->pB8 != NULL) {
        pDesc->pB8 = (SkinDescB8*)((u8*)pDesc + (uptr)pDesc->pB8);
    }
    pDesc->uFlags |= 1;
}

// The description's part with this name code, or -1: SkinPart_FindPart without a skin (hwsBurn.c
// resolves a link's part with it).
s32 SkinPart_FindDescPart(SkinDesc* pDesc, u64 uId) {
    int i;

    for (i = 0; i < pDesc->nParts; i++) {
        if (pDesc->pParts[i].uId == uId) {
            return i;
        }
    }
    return -1;
}

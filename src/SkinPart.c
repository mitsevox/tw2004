// SkinPart.c (EA's name, from its asserts): the parts of a character's skins, such as the glove,
// and their variants and options ("GloveOn", "GloveOff"), plus a second kind of choice, the sets.
// Each skin keeps four copies of its choices (Skin.aParts, Skin.aSets); a golfer's choices are also
// kept outside the skins (SkinChoices). The types are in charstate.h.

#include "charstate.h"
#include "terrain.h"
#include "camera.h"
#include "lldyntex.h"

void  fn_80112614(SkinDesc14* pEntry, SkinDesc18* pMaterial, s32 n);
void  fn_80113774(int nPart, int nVariant, int nOption);
void  fn_8011387C(int n);
void  fn_8011389C(void);
void  fn_801138CC(SkinDesc* pDesc);
void  fn_801138D8(void* p);
int   fn_8001005C(TexBank* pBank, u64 uHash);   // LLTex.c: the texture's index, or 0x80000000

void  fn_800CC588(Character* pChar, int nPart, int nVariant);
void  fn_800CCA1C(void);
void  fn_800CCA3C(void);
s32   fn_800CCA70(Skin* pSkin, int nPart);
s32   fn_800CCAC0(Skin* pSkin, int nPart, int nVariant);
void  fn_800CCC1C(Skin* pSkin, int nPart, int nOption);
u64   fn_800CCE24(Skin* pSkin, int nPart, int nVariant);
s32   fn_800CCEA0(Skin* pSkin);
s32   fn_800CCED0(Skin* pSkin, int nSet);
s32   fn_800CCF10(Skin* pSkin, int nSet, int nVariant);
s32   fn_800CD0B8(Skin* pSkin, int nSet, int nCopy);
s32   fn_800CD124(Skin* pSkin, int nSet, int nVariant);
s32   fn_800CD1D8(Skin* pSkin, int nSet, int nCopy);
u64   fn_800CD340(Skin* pSkin, int nSet);
u64   fn_800CD388(Skin* pSkin, int nSet, int nVariant);
s32   fn_800CD5D0(SkinDesc* pDesc, int n);
s32   fn_800CD664(SkinDesc* pDesc, int nPart);
s32   fn_800CD7CC(Skin* pSkin, s32 n);
void  fn_800CD7D4(Skin* pSkin, SkinMesh* pMesh);
void  fn_800CD844(Skin* pSkin, int n);
void  fn_800CD944(Skin* pSkin, int nPart);
s32   fn_800CDE80(Skin* pSkin, int nSet, int nVariant, const char* pName);
s32   fn_800CDEF4(Skin* pSkin, int nPart, int nVariant);
void  fn_800CDF80(Skin* pSkin, int nPart, int nVariant, int nLink);
void  fn_800CE4B8(u64 uId, SkinListEntry* aList, s32* pnList, u8* p, s32 n);
void  fn_800CE52C(Skin* pSkin, int n, SkinListEntry* aList, s32* pnList, int nCopy);
s32   fn_800CE660(Skin** apSkins, int nSkins, SkinListEntry** ppList, u64* aIds, int nIds, int nCopy);
u8    fn_800CEA30(u64 uId, SkinListEntry* aList, int nList);
u64   fn_800CEA6C(int i, SkinListEntry* aList, int nList);
u8*   fn_800CEAAC(int i, SkinListEntry* aList, int nList);
s32   fn_800CEAE4(int i, SkinListEntry* aList, int nList);
void  fn_800CEDE0(Skin* pSkin, DynTex* pTex, u64 uId);
void  fn_800CEE04(Skin* pSkin, int nFrom, int nTo);
void  fn_800CEE88(u8 b);
void  fn_800CEE98(void);

s32   lbl_80191748[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1};
char* lbl_80281540[2] = {"Glove", NULL};     // the parts fn_800CC4EC lists
u8    lbl_80282238;             // set: a choice goes to all four copies

// Copies a golfer's choices between the body's skin and pChoices: while pChoices holds none yet
// (every entry -1), the skin's go into it; otherwise its go into all four copies of the skin's.
void fn_800CC1EC(Character* pChar, SkinChoices* pChoices) {
    int i;
    u8 bEmpty;

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
        Mem_cpy(pChoices->aParts, pChar->pSkin->aParts[0], fn_800CCA40(pChar->pSkin) * sizeof(SkinChoice));
    } else {
        for (i = 0; i < 4; i++) {
            Mem_cpy(pChar->pSkin->aParts[i], pChoices->aParts,
                    fn_800CCA40(pChar->pSkin) * sizeof(SkinChoice));
        }
    }
    if (bEmpty) {
        Mem_cpy(pChoices->aSets, pChar->pSkin->aSets[0], fn_800CCEA0(pChar->pSkin) * sizeof(SkinChoice));
    } else {
        for (i = 0; i < 4; i++) {
            Mem_cpy(pChar->pSkin->aSets[i], pChoices->aSets,
                    fn_800CCEA0(pChar->pSkin) * sizeof(SkinChoice));
        }
    }
}

// Gives the six skins of p16D8 their choices from pChoices, in all four copies.
void fn_800CC408(Character* pChar, SkinChoices* pChoices) {
    int i;
    int j;

    if (pChar == NULL || pChoices == NULL || pChar->p16D8 == NULL) return;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) {
            Mem_cpy(pChar->p16D8->apSkins[i]->aParts[j], pChoices->aSkinParts[i],
                    fn_800CCA40(pChar->p16D8->apSkins[i]) * sizeof(SkinChoice));
            Mem_cpy(pChar->p16D8->apSkins[i]->aSets[j], pChoices->aSkinSets[i],
                    fn_800CCEA0(pChar->p16D8->apSkins[i]) * sizeof(SkinChoice));
        }
    }
}

// Hands fn_80127B98 the body's parts named in lbl_80281540 (the glove), ending with -1.
void fn_800CC4EC(Character* pChar) {
    s32 aParts[2];
    char** ppName;
    int n;
    int nPart;

    if (pChar == NULL || pChar->pSkin == NULL) return;
    n = 0;
    for (ppName = lbl_80281540; *ppName != NULL; ppName++) {
        nPart = fn_800CDB70(pChar->pSkin, *ppName);
        if (nPart >= 0) {
            aParts[n++] = nPart;
        }
    }
    aParts[n] = -1;
    fn_80127B98(pChar->pSkin, aParts, lbl_80191748);
}

void fn_800CC588(Character* pChar, int nPart, int nVariant) {
    if (pChar == NULL || pChar->pSkin == NULL) return;
    fn_800CCB08(pChar->pSkin, nPart, nVariant);
}

// Picks a variant of a part of the body's skin by their names (the glove on or off).
void fn_800CC5C0(Character* pChar, char* pPart, char* pVariant) {
    u64 uId;
    int nPart;

    if (pChar == NULL || pChar->pSkin == NULL) return;
    fn_800CB700(&uId, pPart);
    nPart = fn_800CDAFC(pChar->pSkin, uId);
    fn_800CB700(&uId, pVariant);
    fn_800CC588(pChar, nPart, fn_800CDBB0(pChar->pSkin, nPart, uId));
}

// Picks a set's variant and option of the body's skin by their names.
void fn_800CC658(Character* pChar, char* pSet, char* pVariant, char* pOption) {
    int nSet;
    int nVariant;

    if (pChar == NULL || pChar->pSkin == NULL) return;
    if (pChar->pSkin->pModel != NULL) {
        nSet = fn_800CDCA0(pChar->pSkin, pSet);
        nVariant = fn_800CDD5C(pChar->pSkin, nSet, pVariant);
        fn_800CC9D8(pChar, nSet, nVariant, fn_800CDE80(pChar->pSkin, nSet, nVariant, pOption));
    }
}

// Picks a variant of a part of one of the six skins by their name codes.
void fn_800CC710(Character* pChar, int nSkin, u64 uPart, u64 uVariant) {
    int nPart;
    int nVariant;
    Skin* pSkin;

    if (pChar == NULL || pChar->p16D8 == NULL || pChar->p16D8->apSkins == NULL || nSkin < 0 || nSkin >= 6) {
        return;
    }
    if (pChar->pSkin->pModel != NULL) {
        pSkin = pChar->p16D8->apSkins[nSkin];
        nPart = fn_800CDAFC(pSkin, uPart);
        nVariant = fn_800CDBB0(pSkin, nPart, uVariant);
        if (nVariant < 0) {
            nVariant = 0;
        }
        fn_800CCB08(pSkin, nPart, nVariant);
    }
}

// Picks a set's variant and option of one of the six skins by their name codes.
void fn_800CC7DC(Character* pChar, int nSkin, u64 uSet, u64 uVariant, u64 uOption) {
    Skin* pSkin;
    int nSet;
    int nVariant;
    int nOption;

    if (pChar == NULL || pChar->p16D8 == NULL || pChar->p16D8->apSkins == NULL || nSkin < 0 || nSkin >= 6) {
        return;
    }
    if (pChar->pSkin->pModel != NULL) {
        pSkin = pChar->p16D8->apSkins[nSkin];
        nSet = fn_800CDC2C(pSkin, uSet);
        nVariant = fn_800CDCE0(pSkin, nSet, uVariant);
        nOption = fn_800CDDB0(pSkin, nSet, nVariant, uOption);
        if (nOption < 0) {
            nOption = 0;
        }
        fn_800CCF90(pSkin, nSet, nVariant, nOption);
    }
}

// Sets every set of the six skins that has a "DefaultL" variant to it (bOn) or to its first
// variant, keeping the option.
void fn_800CC8BC(Character* pChar, u8 bOn) {
    int i;
    int j;
    int nSets;
    int nVariant;
    int nOption;
    int nCopy;

    if (pChar == NULL || pChar->p16D8 == NULL || pChar->p16D8->apSkins == NULL) return;
    for (i = 0; i < 6; i++) {
        nSets = fn_800CCEA0(pChar->p16D8->apSkins[i]);
        for (j = 0; j < nSets; j++) {
            nVariant = fn_800CDD5C(pChar->p16D8->apSkins[i], j, "DefaultL");
            if (nVariant >= 0) {
                nCopy = 3;
                if (fn_800CEE90()) {
                    nCopy = 0;
                }
                nOption = fn_800CD1D8(pChar->p16D8->apSkins[i], j, nCopy);
                if (bOn) {
                    fn_800CCF90(pChar->p16D8->apSkins[i], j, nVariant, nOption);
                } else {
                    fn_800CCF90(pChar->p16D8->apSkins[i], j, 0, nOption);
                }
            }
        }
    }
}

void fn_800CC9D8(Character* pChar, int nSet, int nVariant, int nOption) {
    if (pChar == NULL || pChar->pSkin == NULL) return;
    if (pChar->pSkin->pModel != NULL) {
        fn_800CCF90(pChar->pSkin, nSet, nVariant, nOption);
    }
}

void fn_800CCA1C(void) {
    fn_800CEE98();
}

void fn_800CCA3C(void) {
}

// The skin's number of parts.
s32 fn_800CCA40(Skin* pSkin) {
    if (pSkin->pModel == NULL) return 0;
    if (pSkin->pModel->pDesc != NULL) {
        return pSkin->pModel->pDesc->nParts;
    }
    return 0;
}

// A part's number of variants.
s32 fn_800CCA70(Skin* pSkin, int nPart) {
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
s32 fn_800CCAC0(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;

    if (pSkin == NULL || pSkin->pModel == NULL) return 0;
    pDesc = pSkin->pModel->pDesc;
    return pDesc->pVariants[nVariant + pDesc->pParts[nPart].nFirst].nOptions;
}

// Picks a part's variant (-1 when out of range), then applies the variant's links.
void fn_800CCB08(Skin* pSkin, int nPart, int nVariant) {
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
            if (fn_800CEE90()) {
                pSkin->aParts[0][nPart].nVariant = nVariant;
                pSkin->aParts[1][nPart].nVariant = nVariant;
                pSkin->aParts[2][nPart].nVariant = nVariant;
                pSkin->aParts[3][nPart].nVariant = nVariant;
                pSkin->u10D4 |= 1;
            } else {
                pSkin->aParts[3][nPart].nVariant = nVariant;
            }
            if (nVariant >= 0) {
                nLinks = fn_800CDEF4(pSkin, nPart, nVariant);
                for (i = 0; i < nLinks; i++) {
                    fn_800CDF80(pSkin, nPart, nVariant, i);
                }
            }
        }
    }
}

// Picks a part's option (0 when a variant has too few).
void fn_800CCC1C(Skin* pSkin, int nPart, int nOption) {
    SkinDesc* pDesc;
    int i;

    if (pSkin->pModel != NULL && nPart >= 0 && (pDesc = pSkin->pModel->pDesc) != NULL
        && nPart < pDesc->nParts) {
        for (i = 0; i < fn_800CCA70(pSkin, nPart); i++) {
            pDesc = pSkin->pModel->pDesc;
            if (pDesc->pVariants[i + pDesc->pParts[nPart].nFirst].nOptions < nOption) {
                nOption = 0;
            }
        }
        if (fn_800CEE90()) {
            pSkin->aParts[0][nPart].nOption = nOption;
            pSkin->aParts[1][nPart].nOption = nOption;
            pSkin->aParts[2][nPart].nOption = nOption;
            pSkin->aParts[3][nPart].nOption = nOption;
            pSkin->u10D4 |= 1;
            return;
        }
        pSkin->aParts[3][nPart].nOption = nOption;
    }
}

// A part's variant in one copy of the choices.
s32 fn_800CCD30(Skin* pSkin, int nPart, int nCopy) {
    SkinDesc* pDesc;

    if (pSkin->pModel == NULL) return 0;
    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return -1;
    return pSkin->aParts[nCopy][nPart].nVariant;
}

// A part's option in one copy of the choices.
s32 fn_800CCD84(Skin* pSkin, int nPart, int nCopy) {
    SkinDesc* pDesc;

    if (pSkin->pModel == NULL) return 0;
    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return -1;
    return pSkin->aParts[nCopy][nPart].nOption;
}

// A part's name code.
u64 fn_800CCDDC(Skin* pSkin, int nPart) {
    SkinDesc* pDesc;

    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return 0;
    return pDesc->pParts[nPart].uId;
}

// A variant's name code.
u64 fn_800CCE24(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;
    SkinPartDef* pPart;

    if (nPart < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nPart >= pDesc->nParts) return 0;
    if (nVariant < 0 || nVariant >= (pPart = &pDesc->pParts[nPart])->nVariants) return 0;
    return pDesc->pVariants[nVariant + pPart->nFirst].uId;
}

// The skin's number of sets.
s32 fn_800CCEA0(Skin* pSkin) {
    if (pSkin->pModel == NULL) return 0;
    if (pSkin->pModel->pDesc != NULL) {
        return pSkin->pModel->pDesc->n70;
    }
    return 0;
}

// A set's number of variants.
s32 fn_800CCED0(Skin* pSkin, int nSet) {
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
s32 fn_800CCF10(Skin* pSkin, int nSet, int nVariant) {
    SkinDesc* pDesc;
    s32 nFirst;

    if (pSkin != NULL && pSkin->pModel != NULL) {
        pDesc = pSkin->pModel->pDesc;
        if (pDesc != NULL && nSet < pDesc->n70 && nSet >= 0) {
            nFirst = pDesc->p74[nSet].n10;
            if (nFirst >= 0 && nFirst < pDesc->n78) {
                return pDesc->p7C[nFirst + nVariant].n08;
            }
            return 0;
        }
    }
    return 0;
}

// Picks a set's variant (0 when out of range) and option (-1 when out of range).
void fn_800CCF90(Skin* pSkin, int nSet, int nVariant, int nOption) {
    if (pSkin == NULL || nSet < 0) return;
    if (nSet >= fn_800CCEA0(pSkin)) return;
    if (nVariant < 0 || nVariant >= fn_800CCED0(pSkin, nSet)) {
        nVariant = 0;
    }
    if (nOption < 0 || nOption >= fn_800CCF10(pSkin, nSet, nVariant)) {
        nOption = -1;
    }
    if (fn_800CEE90()) {
        pSkin->aSets[0][nSet].nVariant = nVariant;
        pSkin->aSets[0][nSet].nOption = nOption;
        pSkin->aSets[1][nSet].nVariant = nVariant;
        pSkin->aSets[1][nSet].nOption = nOption;
        pSkin->aSets[2][nSet].nVariant = nVariant;
        pSkin->aSets[2][nSet].nOption = nOption;
        pSkin->aSets[3][nSet].nVariant = nVariant;
        pSkin->aSets[3][nSet].nOption = nOption;
        return;
    }
    pSkin->aSets[3][nSet].nVariant = nVariant;
    pSkin->aSets[3][nSet].nOption = nOption;
}

// A set's variant in one copy of the choices.
s32 fn_800CD0B8(Skin* pSkin, int nSet, int nCopy) {
    if (nSet < 0 || nSet >= fn_800CCEA0(pSkin)) return -1;
    return pSkin->aSets[nCopy][nSet].nVariant;
}

s32 fn_800CD124(Skin* pSkin, int nSet, int nVariant) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;

    if (pSkin->pModel->pDesc == NULL || nSet < 0 || nSet >= fn_800CCEA0(pSkin)) return -1;
    pDesc = pSkin->pModel->pDesc;
    if (nVariant < 0 || nVariant >= (pSet = &pDesc->p74[nSet])->n08) return -1;
    return pDesc->p7C[nVariant + pSet->n10].n10;
}

// A set's option in one copy of the choices.
s32 fn_800CD1D8(Skin* pSkin, int nSet, int nCopy) {
    if (nSet < 0 || nSet >= fn_800CCEA0(pSkin)) return -1;
    return pSkin->aSets[nCopy][nSet].nOption;
}

// The data of a set's variant's option, or NULL.
u8* fn_800CD248(Skin* pSkin, int nSet, int nVariant, int nOption) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;
    SkinDesc7C* pVariant;

    if (pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL) return NULL;
    if (nSet < 0 || nSet >= fn_800CCEA0(pSkin)) return NULL;
    pSet = &pDesc->p74[nSet];
    if (nVariant < 0 || nVariant >= fn_800CCED0(pSkin, nSet)) return NULL;
    pVariant = &pDesc->p7C[pSet->n10 + nVariant];
    if (nOption >= 0 && nOption < fn_800CCF10(pSkin, nSet, nVariant)) {
        return pDesc->p8C[pVariant->n0C + nOption].a08;
    }
    return NULL;
}

// A set's name code.
u64 fn_800CD340(Skin* pSkin, int nSet) {
    SkinDesc* pDesc;

    if (nSet < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nSet >= pDesc->n70) return 0;
    return pDesc->p74[nSet].uId;
}

// A set variant's name code.
u64 fn_800CD388(Skin* pSkin, int nSet, int nVariant) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;

    if (nSet < 0 || (pDesc = pSkin->pModel->pDesc) == NULL || nSet >= pDesc->n70) return 0;
    if (nVariant < 0 || nVariant >= (pSet = &pDesc->p74[nSet])->n08) return 0;
    return pDesc->p7C[nVariant + pSet->n10].uId;
}

// Allocates the four copies of the skin's choices: parts with nothing picked, sets on their
// "Defaults" variant (or the first) and their first option (-1 when a variant has none).
void fn_800CD404(Skin* pSkin) {
    int i;
    int j;
    s32 nParts;
    s32 nSets;
    u32 nBytes;
    s32 nVariant;

    nParts = fn_800CCA40(pSkin);
    fn_800CCEA0(pSkin);
    nBytes = nParts * sizeof(SkinChoice);
    for (i = 0; i < 4; i++) {
        if (nParts != 0) {
            pSkin->aParts[i] = fn_80009B34(nBytes, 2, 16, "SkinPart.c", 655);
            memset(pSkin->aParts[i], 0, nBytes);
        } else {
            pSkin->aParts[i] = NULL;
        }
        nSets = fn_800CCEA0(pSkin);
        if (nSets != 0) {
            pSkin->aSets[i] = fn_80009B34(nSets * sizeof(SkinChoice), 2, 16, "SkinPart.c", 670);
            memset(pSkin->aSets[i], 0, nSets * sizeof(SkinChoice));
            for (j = 0; j < nSets; j++) {
                if (fn_800CCF10(pSkin, j, 0) >= 1) {
                    pSkin->aSets[i][j].nOption = 0;
                } else {
                    pSkin->aSets[i][j].nOption = -1;
                }
                nVariant = fn_800CDD5C(pSkin, j, "Defaults");
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
void fn_800CD56C(Skin* pSkin) {
    int i;

    for (i = 0; i < 4; i++) {
        if (pSkin->aParts[i] != NULL) {
            fn_80009E70(pSkin->aParts[i]);
        }
        if (pSkin->aSets[i] != NULL) {
            fn_80009E70(pSkin->aSets[i]);
        }
    }
}

// The bytes of the counted entries fn_80113B34 walks for pDesc and n.
s32 fn_800CD5D0(SkinDesc* pDesc, int n) {
    u8 aBuf[0x48];
    SkinIterArgs args;
    SkinIter* pIter;
    SkinMesh* pMesh;
    s32 nBytes;

    args.pDesc = pDesc;
    args.n = n;
    nBytes = 0;
    for (pIter = fn_80113B34(aBuf, &args); fn_800CEEC0(pIter); fn_800CEEC8(pIter)) {
        pMesh = fn_800CEEF4(pIter);
        if ((pMesh->uFlags & 0x300000) == 0x300000) {
            nBytes += pMesh->nSize;
        }
    }
    fn_80113BAC(pIter);
    return nBytes;
}

// The most bytes any option of any of a part's variants needs.
s32 fn_800CD664(SkinDesc* pDesc, int nPart) {
    SkinPartDef* pPart;
    SkinVariant* pVariant;
    int i;
    int j;
    s32 nMax;
    s32 nBytes;

    nMax = 0;
    pPart = &pDesc->pParts[nPart];
    for (i = 0; i < pPart->nVariants; i++) {
        pVariant = &pDesc->pVariants[pPart->nFirst + i];
        for (j = 0; j < pVariant->nOptions; j++) {
            nBytes = fn_800CD5D0(pDesc, pVariant->nFirstOption + j);
            if (nMax < nBytes) {
                nMax = nBytes;
            }
        }
    }
    return nMax;
}

// The bytes the skin's parts can need at most: the sum of each part's largest, but no more than all
// the counted entries of p34 together.
s32 fn_800CD700(Skin* pSkin) {
    SkinDesc* pDesc;
    int i;
    s32 nBytes;
    s32 nAll;

    nBytes = 0;
    pDesc = pSkin->pModel->pDesc;
    if (pDesc == NULL) return 0;
    for (i = 0; i < pDesc->nParts; i++) {
        nBytes += fn_800CD664(pDesc, i);
    }
    nAll = 0;
    for (i = 0; i < pDesc->n30; i++) {
        if ((pDesc->p34[i].uFlags & 0x300000) == 0x300000) {
            nAll += pDesc->p34[i].nSize;
        }
    }
    if (nBytes > nAll) {
        nBytes = nAll;
    }
    return nBytes;
}

s32 fn_800CD7CC(Skin* pSkin, s32 n) {
    return n;
}

// Sets the bits of p10CC an entry lists.
void fn_800CD7D4(Skin* pSkin, SkinMesh* pMesh) {
    SkinMeshBit* pBit;
    int i;

    pBit = pMesh->pBits;
    for (i = 0; i < pMesh->n8; i++) {
        fn_8001EA34(pSkin->p10CC, pBit->nBit);
        pBit++;
    }
}

// Marks the bits of the entries fn_80113B34 walks for option n.
void fn_800CD844(Skin* pSkin, int n) {
    u8 aBuf[0x48];
    SkinIterArgs args;
    SkinIter* pIter;
    SkinMesh* pMesh;
    SkinDesc* pDesc;

    pDesc = pSkin->pModel->pDesc;
    if (n < 0) return;
    if (n >= pDesc->n58) return;
    args.n = n;
    args.pDesc = pDesc;
    for (pIter = fn_80113B34(aBuf, &args); fn_800CEEC0(pIter); fn_800CEEC8(pIter)) {
        pMesh = fn_800CEEF4(pIter);
        if ((pMesh->uFlags & 0x300000) == 0x300000) {
            n = fn_800CD7CC(pSkin, fn_800CEEFC(pIter));
            if (pSkin->p10D0 != NULL) {
                fn_8001EA34(pSkin->p10D0, n);
            }
        }
        if ((pMesh->uFlags & 1) && (pMesh->uFlags & 0x10)) {
            fn_800CD7D4(pSkin, pMesh);
        }
    }
    fn_80113BAC(pIter);
}

// Marks the bits of a part's chosen option.
void fn_800CD944(Skin* pSkin, int nPart) {
    SkinDesc* pDesc;
    s32 nVariant;
    s32 nOption;

    nVariant = fn_800CCD30(pSkin, nPart, 0);
    nOption = fn_800CCD84(pSkin, nPart, 0);
    pDesc = pSkin->pModel->pDesc;
    if (nVariant < 0) return;
    if (nOption < 0) return;
    fn_800CD844(pSkin, nOption + pDesc->pVariants[nVariant + pDesc->pParts[nPart].nFirst].nFirstOption);
}

// Marks the bits of every option.
void fn_800CD9EC(Skin* pSkin) {
    SkinDesc* pDesc;
    s32 n;
    int i;

    if (pSkin == NULL || pSkin->pModel == NULL) return;
    pDesc = pSkin->pModel->pDesc;
    if (pDesc == NULL) return;
    n = pDesc->n58;
    for (i = 0; i < n; i++) {
        fn_800CD844(pSkin, i);
    }
}

// After the choices changed: clears the bit arrays and marks the chosen options' bits again.
void fn_800CDA68(Skin* pSkin) {
    int i;

    if (pSkin->u10D4 & 1) {
        pSkin->u10D4 &= ~1;
        fn_8001E938(pSkin->p10CC, pSkin->pModel->n50);
        if (pSkin->p10D0 != NULL) {
            fn_8001E938(pSkin->p10D0, pSkin->pModel->n40);
        }
        for (i = 0; i < fn_800CCA40(pSkin); i++) {
            fn_800CD944(pSkin, i);
        }
    }
}

// The part with this name code, or -1.
s32 fn_800CDAFC(Skin* pSkin, u64 uId) {
    s32 n;
    int i;

    n = fn_800CCA40(pSkin);
    for (i = 0; i < n; i++) {
        if (fn_800CCDDC(pSkin, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The part with this name, or -1.
s32 fn_800CDB70(Skin* pSkin, const char* pName) {
    u64 uId;

    fn_800CB700(&uId, pName);
    return fn_800CDAFC(pSkin, uId);
}

// The part's variant with this name code, or -1.
s32 fn_800CDBB0(Skin* pSkin, int nPart, u64 uId) {
    s32 n;
    int i;

    n = fn_800CCA70(pSkin, nPart);
    for (i = 0; i < n; i++) {
        if (fn_800CCE24(pSkin, nPart, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The set with this name code, or -1.
s32 fn_800CDC2C(Skin* pSkin, u64 uId) {
    s32 n;
    int i;

    n = fn_800CCEA0(pSkin);
    for (i = 0; i < n; i++) {
        if (fn_800CD340(pSkin, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The set with this name, or -1.
s32 fn_800CDCA0(Skin* pSkin, const char* pName) {
    u64 uId;

    fn_800CB700(&uId, pName);
    return fn_800CDC2C(pSkin, uId);
}

// The set's variant with this name code, or -1.
s32 fn_800CDCE0(Skin* pSkin, int nSet, u64 uId) {
    s32 n;
    int i;

    n = fn_800CCED0(pSkin, nSet);
    for (i = 0; i < n; i++) {
        if (fn_800CD388(pSkin, nSet, i) == uId) {
            return i;
        }
    }
    return -1;
}

// The set's variant with this name, or -1.
s32 fn_800CDD5C(Skin* pSkin, int nSet, const char* pName) {
    u64 uId;

    fn_800CB700(&uId, pName);
    return fn_800CDCE0(pSkin, nSet, uId);
}

// The set variant's option with this name code, or -1.
s32 fn_800CDDB0(Skin* pSkin, int nSet, int nVariant, u64 uId) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;
    SkinDesc7C* pVariant;
    int i;

    if (nSet < 0 || pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL
        || nSet >= pDesc->n70 || nSet < 0) {
        return 0;
    }
    if (nVariant < 0 || nVariant >= (pSet = &pDesc->p74[nSet])->n08) return 0;
    pVariant = &pDesc->p7C[nVariant + pSet->n10];
    for (i = 0; i < pVariant->n08; i++) {
        if (pDesc->p8C[pVariant->n0C + i].uId == uId) {
            return i;
        }
    }
    return -1;
}

// The set variant's option with this name, or -1.
s32 fn_800CDE80(Skin* pSkin, int nSet, int nVariant, const char* pName) {
    u64 uId;

    if (pName == NULL) return -1;
    fn_800CB700(&uId, pName);
    return fn_800CDDB0(pSkin, nSet, nVariant, uId);
}

// A part variant's number of links.
s32 fn_800CDEF4(Skin* pSkin, int nPart, int nVariant) {
    SkinDesc* pDesc;

    if (pSkin == NULL || nVariant < 0 || nVariant >= fn_800CCA70(pSkin, nPart)) return 0;
    pDesc = pSkin->pModel->pDesc;
    return pDesc->pVariants[nVariant + pDesc->pParts[nPart].nFirst].nLinks;
}

// Applies a link of a part's variant: sets the option of the part it names.
void fn_800CDF80(Skin* pSkin, int nPart, int nVariant, int nLink) {
    SkinDesc* pDesc;
    SkinLink* pLink;
    s32 nOther;

    if (pSkin == NULL) return;
    pDesc = pSkin->pModel->pDesc;
    pLink = &pDesc->pLinks[pDesc->pVariants[nVariant + pDesc->pParts[nPart].nFirst].nFirstLink + nLink];
    nOther = fn_800CDAFC(pSkin, pLink->uPart);
    if (nOther >= 0 && nOther < fn_800CCA40(pSkin)) {
        fn_800CCC1C(pSkin, nOther, pLink->nOption);
    }
}

void fn_800CE02C(Skin* pSkin, int n) {
    if (pSkin->pModel->pDesc != NULL && (pSkin->u10D4 & 2)) {
        fn_80035138(1);
        fn_80016B9C();
        fn_80012EF8();
        fn_8011387C(0x400);
        fn_801138CC(pSkin->pModel->pDesc);
        fn_801138D8(pSkin->a10A0[n]);
    }
}

void fn_800CE0B0(Skin* pSkin, int nPart) {
    s32 nVariant;
    s32 nOption;

    if (pSkin->u10D4 & 2) {
        nVariant = fn_800CCD30(pSkin, nPart, 0);
        nOption = fn_800CCD84(pSkin, nPart, 0);
        if (nVariant != -1) {
            fn_80113774(nPart, nVariant, nOption);
        }
    }
}

void fn_800CE128(Skin* pSkin) {
    if (pSkin->pModel->pDesc != NULL && (pSkin->u10D4 & 2)) {
        fn_8011389C();
    }
}

void fn_800CE164(void) {
}

void fn_800CE168(void) {
}

void fn_800CE16C(void) {
}

// Hands fn_80112614 each p14 entry as the chosen sets patch it, with its p18 entry.
void fn_800CE170(Skin* pSkin, SkinTarget* pTarget) {
    SkinDesc* pDesc;
    SkinDesc14 entry;
    int i;

    if (pSkin == NULL) return;
    if (pSkin->pModel == NULL) return;
    pDesc = pSkin->pModel->pDesc;
    if (pDesc != NULL) {
        for (i = 0; i < pDesc->n10; i++) {
            Mem_cpy(&entry, &pDesc->p14[i], sizeof(SkinDesc14));
            fn_800CE224(pSkin, &entry, NULL, NULL, 0);
            fn_80112614(&entry, &pDesc->p18[i], pTarget->n4);
        }
    }
}

// Patches a p14 entry for the chosen sets: an entry whose name code a set lists takes the chosen
// variant's name code (from the second variant on) and the chosen option's p8C entry, and with
// bit 2 its a20 from pB8. Gives the p8C entry's data and n2C too when asked. Returns whether
// anything changed.
s32 fn_800CE224(Skin* pSkin, SkinDesc14* pEntry, u8** ppOut, s32* pnOut, int nCopy) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;
    SkinDesc7C* pVariant;
    SkinDescB8* pB8;
    int i;
    int j;
    s32 nVariant;
    s32 nOption;
    s32 n;
    s32 nB8;
    s32 bChanged;

    bChanged = 0;
    if (pSkin == NULL || pSkin->pModel == NULL || (pDesc = pSkin->pModel->pDesc) == NULL || pEntry == NULL) {
        return 0;
    }
    nB8 = 0;
    for (i = 0; i < pDesc->n70; i++) {
        pSet = &pDesc->p74[i];
        for (j = 0; j < pSet->n0C; j++) {
            if (pEntry->uId == pDesc->p84[pSet->n14 + j]) {
                nVariant = fn_800CD0B8(pSkin, i, nCopy);
                nOption = fn_800CD1D8(pSkin, i, nCopy);
                n = fn_800CD124(pSkin, i, nVariant);
                if (n >= 0) {
                    nB8 = n;
                }
                if (nVariant >= 0 && nVariant < fn_800CCED0(pSkin, i)) {
                    pVariant = &pDesc->p7C[pSet->n10 + nVariant];
                    if (nOption >= 0 && nOption < fn_800CCF10(pSkin, i, nVariant)) {
                        pEntry->n18 = pVariant->n0C + nOption;
                    } else {
                        pEntry->n18 = -1;
                    }
                    if (nVariant > 0) {
                        bChanged = 1;
                        pEntry->uId = pDesc->p84[j + (nVariant * pSet->n0C + pSet->n14)];
                    }
                }
            }
        }
    }
    if (pEntry->u08 & 2) {
        if (pEntry->n16 > 0) {
            if (nB8 < 0 || nB8 >= pEntry->n16) {
                nB8 = 0;
            }
            pB8 = &pDesc->pB8[pEntry->n1C + nB8];
            pEntry->a20[0] = pB8->a[0];
            pEntry->a20[1] = pB8->a[1];
            pEntry->a20[2] = pB8->a[2];
            pEntry->a20[3] = pB8->a[3];
        }
    }
    if (ppOut != NULL && pnOut != NULL) {
        if (pEntry->n18 >= 0 && pEntry->n18 < pDesc->n88) {
            bChanged = 1;
            *ppOut = pDesc->p8C[pEntry->n18].a08;
            *pnOut = pDesc->p8C[pEntry->n18].n2C;
        } else {
            *ppOut = NULL;
        }
    }
    return bChanged;
}

// Adds a name code to a list unless it is there already.
void fn_800CE4B8(u64 uId, SkinListEntry* aList, s32* pnList, u8* p, s32 n) {
    int i;

    for (i = 0; i < *pnList; i++) {
        if (aList[i].uId == uId) return;
    }
    aList[*pnList].uId = uId;
    aList[*pnList].p8 = p;
    aList[*pnList].nC = n;
    (*pnList)++;
}

// Lists the name codes of option n's p14 entries (as the chosen sets patch them).
void fn_800CE52C(Skin* pSkin, int n, SkinListEntry* aList, s32* pnList, int nCopy) {
    SkinDesc14 entry;
    u8* p;
    s32 nOut;
    SkinDesc* pDesc;
    SkinDesc44* p44;
    s32* pIndex;
    int i;
    int j;
    s32 nFirst;
    s32 nIndices;
    s32 n44;

    pDesc = pSkin->pModel->pDesc;
    nFirst = pDesc->p5C[n].n4;
    for (i = 0; i < pDesc->p5C[n].n0; i++) {
        n44 = pDesc->p6C[nFirst + i];
        if (n44 >= 0) {
            p44 = &pDesc->p44[n44];
            if (p44->n8 != 0) {
                nIndices = pDesc->p28[p44->nC].n0;
                pIndex = &pDesc->p20[p44->n4];
                for (j = 0; j < nIndices; j++) {
                    if (!(pDesc->p14[*pIndex].u08 & 1)) {
                        Mem_cpy(&entry, &pDesc->p14[*pIndex], sizeof(SkinDesc14));
                        p = NULL;
                        fn_800CE224(pSkin, &entry, &p, &nOut, nCopy);
                        fn_800CE4B8(entry.uId, aList, pnList, p, nOut);
                    }
                    pIndex++;
                }
            }
        }
    }
}

// Lists (in a new *ppList) the name codes the skins' chosen options use; for the parts named in
// aIds, those of every option. Returns the list's length.
s32 fn_800CE660(Skin** apSkins, int nSkins, SkinListEntry** ppList, u64* aIds, int nIds, int nCopy) {
    s32 nList;
    Skin* pSkin;
    SkinDesc* pDesc;
    int i;
    int j;
    int k;
    s32 n;
    s32 nVariant;
    s32 nOption;
    u8 bAll;

    n = 0;
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL && apSkins[i]->pModel->pDesc != NULL) {
            n += apSkins[i]->pModel->pDesc->n10;
        }
    }
    nList = 0;
    *ppList = fn_80009B34(n * sizeof(SkinListEntry), 1, 0, "SkinPart.c", 1980);
    for (i = 0; i < nSkins; i++) {
        pSkin = apSkins[i];
        if (pSkin != NULL && pSkin->pModel != NULL && (pDesc = pSkin->pModel->pDesc) != NULL) {
            for (j = 0; j < fn_800CCA40(pSkin); j++) {
                bAll = 0;
                nVariant = fn_800CCD30(pSkin, j, nCopy);
                nOption = fn_800CCD84(pSkin, j, nCopy);
                if (aIds != NULL) {
                    for (k = 0; k < nIds; k++) {
                        if (aIds[k] == fn_800CCDDC(pSkin, j)) {
                            bAll = 1;
                            break;
                        }
                    }
                }
                if (bAll == 1) {
                    for (k = 0; k < fn_800CCA70(pSkin, j); k++) {
                        for (n = 0; n < fn_800CCAC0(pSkin, j, k); n++) {
                            fn_800CE52C(pSkin, n + pDesc->pVariants[k + pDesc->pParts[j].nFirst].nFirstOption,
                                        *ppList, &nList, nCopy);
                        }
                    }
                } else if (nVariant >= 0 && nOption >= 0) {
                    fn_800CE52C(pSkin,
                                nOption + pDesc->pVariants[nVariant + pDesc->pParts[j].nFirst].nFirstOption,
                                *ppList, &nList, nCopy);
                }
            }
        }
    }
    return nList;
}

// Lists (in a new *ppList) the name codes every option of the skins uses. Returns the list's
// length.
s32 fn_800CE8C0(Skin** apSkins, int nSkins, SkinListEntry** ppList) {
    s32 nList;
    Skin* pSkin;
    SkinDesc* pDesc;
    int i;
    int j;
    int k;
    int m;
    s32 n;

    n = 0;
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL && apSkins[i]->pModel->pDesc != NULL) {
            n += apSkins[i]->pModel->pDesc->n10;
        }
    }
    *ppList = fn_80009B34(n * sizeof(SkinListEntry), 1, 0, "SkinPart.c", 2068);
    nList = 0;
    for (i = 0; i < nSkins; i++) {
        if (apSkins[i] != NULL && apSkins[i]->pModel != NULL && (pDesc = apSkins[i]->pModel->pDesc) != NULL) {
            pSkin = apSkins[i];
            for (j = 0; j < fn_800CCA40(pSkin); j++) {
                for (k = 0; k < fn_800CCA70(pSkin, j); k++) {
                    n = k + pDesc->pParts[j].nFirst;
                    for (m = 0; m < pDesc->pVariants[n].nOptions; m++) {
                        fn_800CE52C(pSkin, m + pDesc->pVariants[n].nFirstOption, *ppList, &nList, 0);
                    }
                }
            }
        }
    }
    return nList;
}

// Whether a list holds the name code.
u8 fn_800CEA30(u64 uId, SkinListEntry* aList, int nList) {
    int i;

    for (i = 0; i < nList; i++) {
        if (aList[i].uId == uId) return 1;
    }
    return 0;
}

u64 fn_800CEA6C(int i, SkinListEntry* aList, int nList) {
    if (nList <= 0 || aList == NULL || i < 0 || i >= nList) return 0;
    return aList[i].uId;
}

u8* fn_800CEAAC(int i, SkinListEntry* aList, int nList) {
    if (nList <= 0 || aList == NULL || i < 0 || i >= nList) return NULL;
    return aList[i].p8;
}

s32 fn_800CEAE4(int i, SkinListEntry* aList, int nList) {
    if (nList <= 0 || aList == NULL || i < 0 || i >= nList) return 0;
    return aList[i].nC;
}

// Hands fn_8010AD50 each of pTex's name codes the skins' chosen options do not use, then calls
// fn_8010ADA4.
void fn_800CEB1C(Skin** apSkins, int nSkins, DynTex* pTex) {
    SkinListEntry* pList;
    s32 nList;
    s32 n;
    int i;
    u64 uId;

    if (pTex == NULL) return;
    nList = fn_800CE660(apSkins, nSkins, &pList, NULL, 0, 2);
    n = fn_8010AD10(pTex);
    for (i = 0; i < n; i++) {
        uId = fn_8010AD18(pTex, i);
        if (uId != 0 && !fn_800CEA30(uId, pList, nList)) {
            fn_8010AD50(pTex, uId);
        }
    }
    fn_8010ADA4(pTex);
    if (pList != NULL) {
        fn_80009E70(pList);
    }
}

// Hands fn_8010BCFC each name code the skins' chosen options use that pTex has no entry for. aIds
// and nIds go on to fn_800CE660.
void fn_800CEBE8(Skin** apSkins, int nSkins, DynTex* pTex, u64* aIds, int nIds) {
    SkinListEntry* pList;
    s32 nList;
    TexBank* pBank;
    s32 nC;
    int i;
    u64 uId;

    pList = NULL;
    if (pTex == NULL) return;
    nList = fn_800CE660(apSkins, nSkins, &pList, aIds, nIds, 2);
    i = 0;
    // port: a DynTexHeader has TexBank's layout (lldyntex.h); the two are not merged yet.
    pBank = (TexBank*)fn_8010A780(pTex);
    for (; i < nList; i++) {
        uId = fn_800CEA6C(i, pList, nList);
        if (uId != 0 && fn_8001005C(pBank, uId) == -0x80000000) {
            nC = fn_800CEAE4(i, pList, nList);
            fn_8010BCFC(uId, fn_800CEAAC(i, pList, nList), nC);
        }
    }
    if (pList != NULL) {
        fn_80009E70(pList);
    }
}

// Hands fn_800CEDE0 the name codes a set's variant uses.
void fn_800CECE0(Skin* pSkin, int nSet, int nVariant, int nOption, DynTex* pTex) {
    SkinDesc* pDesc;
    SkinDesc74* pSet;
    int i;

    if (pSkin == NULL || pSkin->pModel == NULL || nSet < 0) return;
    if (nSet >= fn_800CCEA0(pSkin)) return;
    if (nVariant < 0) return;
    if (nVariant >= fn_800CCED0(pSkin, nSet)) return;
    if (nOption < 0) return;
    if (nOption >= fn_800CCF10(pSkin, nSet, nVariant)) return;
    pDesc = pSkin->pModel->pDesc;
    pSet = &pDesc->p74[nSet];
    for (i = 0; i < pSet->n0C; i++) {
        fn_800CEDE0(pSkin, pTex, pDesc->p84[i + (nVariant * pSet->n0C + pSet->n14)]);
    }
}

void fn_800CEDE0(Skin* pSkin, DynTex* pTex, u64 uId) {
    fn_8010AD50(pTex, uId);
}

// Copies one copy of the skin's choices over another.
void fn_800CEE04(Skin* pSkin, int nFrom, int nTo) {
    Mem_cpy(pSkin->aParts[nTo], pSkin->aParts[nFrom], fn_800CCA40(pSkin) * sizeof(SkinChoice));
    Mem_cpy(pSkin->aSets[nTo], pSkin->aSets[nFrom], fn_800CCEA0(pSkin) * sizeof(SkinChoice));
}

void fn_800CEE88(u8 b) {
    lbl_80282238 = b;
}

u8 fn_800CEE90(void) {
    return lbl_80282238;
}

void fn_800CEE98(void) {
    fn_800CEE88(1);
}

void fn_800CEEBC(void) {
}

u8 fn_800CEEC0(SkinIter* pIter) {
    return pIter->bValid;
}

void fn_800CEEC8(SkinIter* pIter) {
    (*pIter->ppfnNext)(pIter);
}

SkinMesh* fn_800CEEF4(SkinIter* pIter) {
    return pIter->pCur;
}

s32 fn_800CEEFC(SkinIter* pIter) {
    return pIter->nCur;
}

// Makes a loaded description usable: its offsets become pointers (once). One of another version
// is cleared instead.
void fn_800CEF04(SkinDesc* pDesc) {
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

// The part with this name code, or -1.
s32 fn_800CF104(SkinDesc* pDesc, u64 uId) {
    int i;

    for (i = 0; i < pDesc->nParts; i++) {
        if (pDesc->pParts[i].uId == uId) {
            return i;
        }
    }
    return -1;
}

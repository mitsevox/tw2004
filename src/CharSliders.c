// CharSliders.c (EA's name, from its asserts; TW06): the character's body sliders: reading their
// definitions (CharSlider_CreateDefinitionsFromMem), freeing them, and blending a model's bones
// and a skin's morph targets by the slider values (fn_8010E4DC). The file starts at fn_8010D454
// (before it is GameMode26.c) and ends at fn_8010E58C, where FE_PGATourMessages.c's leaderboard
// messages begin.

#include "engine.h"
#include "character.h"
#include "charstate.h"
#include "golfer.h"

void fn_8011CADC(Skin* pSkin, int nMorph, f32 fWeight);    // SkinMorph.c

// Free slider definitions made by CharSlider_CreateDefinitionsFromMem.
void fn_8010D454(CharSliderDefs* pDefs) {
    int i;
    int j;

    if (pDefs != NULL) {
        if (pDefs->pDefs != NULL) {
            for (i = 0; i < pDefs->nSliders; i++) {
                if (pDefs->pDefs[i].pLinks != NULL) {
                    fn_80009E70(pDefs->pDefs[i].pLinks);
                }
                if (pDefs->pDefs[i].pLimits != NULL) {
                    fn_80009E70(pDefs->pDefs[i].pLimits);
                }
                if (pDefs->pDefs[i].pBoneRanges != NULL) {
                    for (j = 0; j < pDefs->pDefs[i].nBoneRanges; j++) {
                        fn_80009E70(pDefs->pDefs[i].pBoneRanges[j].items.pBones);
                    }
                    fn_80009E70(pDefs->pDefs[i].pBoneRanges);
                }
                if (pDefs->pDefs[i].pMorphRanges != NULL) {
                    for (j = 0; j < pDefs->pDefs[i].nMorphRanges; j++) {
                        fn_80009E70(pDefs->pDefs[i].pMorphRanges[j].items.pMorphs);
                    }
                    fn_80009E70(pDefs->pDefs[i].pMorphRanges);
                }
            }
            fn_80009E70(pDefs->pDefs);
        }
        if (pDefs->pValues != NULL) {
            fn_80009E70(pDefs->pValues);
        }
        if (pDefs->aMorphIds != NULL) {
            fn_80009E70(pDefs->aMorphIds);
        }
        fn_80009E70(pDefs);
    }
}

// Reads the slider definitions at *ppData (a count of sliders and of morph targets, then each
// table in turn, byte-swapped by its layout); NULL when there are no sliders.
CharSliderDefs* CharSlider_CreateDefinitionsFromMem(u8** ppData) {
    void* pDst;
    s32 nSliders;
    s32 nMorphs;
    CharSliderDefs* pDefs;
    int i;
    int j;
    int k;

    fn_80076158(ppData, (u8*)&nSliders, 4, 4);
    fn_80076158(ppData, (u8*)&nMorphs, 4, 4);
    *ppData += 8;
    if (nSliders <= 0) {
        return NULL;
    }

    pDefs = fn_80009B34(sizeof(CharSliderDefs), 2, 0, "CharSliders.c", 1010);
    fn_80005AE8(pDefs, 0, sizeof(CharSliderDefs));
    pDefs->nSliders = nSliders;
    pDefs->pDefs = fn_80009B34(pDefs->nSliders * sizeof(CharSliderDef), 2, 0, "CharSliders.c", 1014);
    fn_80005AE8(pDefs->pDefs, 0, pDefs->nSliders * sizeof(CharSliderDef));
    pDefs->pValues = fn_80009B34(pDefs->nSliders * sizeof(CharSliderValue), 2, 0, "CharSliders.c", 1016);
    fn_80005AE8(pDefs->pValues, 0, pDefs->nSliders * sizeof(CharSliderValue));
    pDefs->nMorphs = nMorphs;
    pDefs->aMorphIds = fn_80009B34(pDefs->nMorphs * sizeof(u64), 2, 0, "CharSliders.c", 1020);
    fn_80005AE8(pDefs->aMorphIds, 0, pDefs->nMorphs * sizeof(u64));

    // The sliders; the pointers read with them are not valid yet.
    for (i = 0; i < pDefs->nSliders; i++) {
        pDst = &pDefs->pDefs[i];
        fn_8001F08C((void**)ppData, &pDst, lbl_80193C30, 10, 1);
        pDefs->pDefs[i].pBoneRanges = NULL;
        pDefs->pDefs[i].pMorphRanges = NULL;
        pDefs->pDefs[i].pLinks = NULL;
        pDefs->pDefs[i].pLimits = NULL;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nLinks > 0) {
            pDefs->pDefs[i].pLinks = fn_80009B34(pDefs->pDefs[i].nLinks * sizeof(CharSliderLink), 2, 0,
                                                 "CharSliders.c", 1039);
            for (j = 0; j < pDefs->pDefs[i].nLinks; j++) {
                pDst = &pDefs->pDefs[i].pLinks[j];
                fn_8001F08C((void**)ppData, &pDst, lbl_80193BF0, 6, 1);
            }
        }
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nLimits > 0) {
            pDefs->pDefs[i].pLimits = fn_80009B34(pDefs->pDefs[i].nLimits * sizeof(CharSliderLimit), 2, 0,
                                                  "CharSliders.c", 1053);
            for (j = 0; j < pDefs->pDefs[i].nLimits; j++) {
                pDst = &pDefs->pDefs[i].pLimits[j];
                fn_8001F08C((void**)ppData, &pDst, lbl_80193C20, 2, 1);
            }
        }
    }

    // The bone ranges, then the bones of each.
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nBoneRanges > 0) {
            pDefs->pDefs[i].pBoneRanges = fn_80009B34(pDefs->pDefs[i].nBoneRanges * sizeof(CharSliderRange),
                                                      2, 0, "CharSliders.c", 1067);
            for (j = 0; j < pDefs->pDefs[i].nBoneRanges; j++) {
                pDst = &pDefs->pDefs[i].pBoneRanges[j];
                fn_8001F08C((void**)ppData, &pDst, lbl_80193B98, 4, 1);
                pDefs->pDefs[i].pBoneRanges[j].items.pBones = NULL;
            }
        }
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        for (j = 0; j < pDefs->pDefs[i].nBoneRanges; j++) {
            if (pDefs->pDefs[i].pBoneRanges[j].nItems > 0) {
                pDefs->pDefs[i].pBoneRanges[j].items.pBones =
                    fn_80009B34(pDefs->pDefs[i].pBoneRanges[j].nItems * sizeof(CharSliderBone), 2, 0,
                                "CharSliders.c", 1084);
                for (k = 0; k < pDefs->pDefs[i].pBoneRanges[j].nItems; k++) {
                    pDst = &pDefs->pDefs[i].pBoneRanges[j].items.pBones[k];
                    fn_8001F08C((void**)ppData, &pDst, lbl_80193B70, 5, 1);
                }
            }
        }
    }

    // The morph target ranges, then the morph targets of each.
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nMorphRanges > 0) {
            pDefs->pDefs[i].pMorphRanges = fn_80009B34(pDefs->pDefs[i].nMorphRanges * sizeof(CharSliderRange),
                                                       2, 0, "CharSliders.c", 1100);
            for (j = 0; j < pDefs->pDefs[i].nMorphRanges; j++) {
                pDst = &pDefs->pDefs[i].pMorphRanges[j];
                fn_8001F08C((void**)ppData, &pDst, lbl_80193BD0, 4, 1);
                pDefs->pDefs[i].pMorphRanges[j].items.pMorphs = NULL;
            }
        }
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        for (j = 0; j < pDefs->pDefs[i].nMorphRanges; j++) {
            if (pDefs->pDefs[i].pMorphRanges[j].nItems > 0) {
                pDefs->pDefs[i].pMorphRanges[j].items.pMorphs =
                    fn_80009B34(pDefs->pDefs[i].pMorphRanges[j].nItems * sizeof(CharSliderMorph), 2, 0,
                                "CharSliders.c", 1117);
                for (k = 0; k < pDefs->pDefs[i].pMorphRanges[j].nItems; k++) {
                    pDst = &pDefs->pDefs[i].pMorphRanges[j].items.pMorphs[k];
                    fn_8001F08C((void**)ppData, &pDst, lbl_80193BB8, 3, 1);
                }
            }
        }
    }

    // The morph targets' ids.
    for (i = 0; i < pDefs->nMorphs; i++) {
        pDst = &pDefs->aMorphIds[i];
        fn_8001F08C((void**)ppData, &pDst, lbl_80281788, 1, 1);
    }
    return pDefs;
}

// The index of the slider whose id is nId, or -1.
int fn_8010DC94(CharSliderDefs* pDefs, s32 nId) {
    int i;

    if (pDefs == NULL) {
        return -1;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        if (nId == pDefs->pDefs[i].nId) {
            return i;
        }
    }
    return -1;
}

// Every slider back to the range 0..1 at 0.
void fn_8010DCE8(CharSliderDefs* pDefs) {
    int i;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pDefs->pValues[i].fLow = 0.0f;
            pDefs->pValues[i].fHigh = 1.0f;
            pDefs->pValues[i].fValue = 0.0f;
            pDefs->pValues[i].bFixed = 0;
        }
    }
}

// Set the sliders with ids 0..nSliders-1 from percentages.
void fn_8010DD4C(CharSliderDefs* pDefs, int nSliders, u8* aValues) {
    int i;
    int n;

    for (i = 0; i < nSliders; i++) {
        n = fn_8010DC94(pDefs, i);
        if (n >= 0) {
            pDefs->pValues[n].fValue = (s8)aValues[i] / 100.0f;
        }
    }
}

// Keep every value in 0..0.99999.
void fn_8010DE04(CharSliderDefs* pDefs) {
    int i;
    CharSliderValue* pValue;

    for (i = 0; i < pDefs->nSliders; i++) {
        pValue = &pDefs->pValues[i];
        if (pValue->fValue <= 0.0f) {
            pValue->fValue = 0.0f;
        }
        if (pValue->fValue >= 0.99999f) {
            pValue->fValue = 0.99999f;
        }
    }
}

// Cut each pair of sliders that share a length back to it.
void fn_8010DE60(CharSliderDefs* pDefs) {
    CharSliderValue* pValue;
    CharSliderDef* pDef;
    CharSliderLimit* pLimit;
    CharSliderValue* pOther;
    f32 fLength;
    int i;
    int j;
    int n;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pValue = &pDefs->pValues[i];
            pDef = &pDefs->pDefs[i];
            if (pValue->bFixed != 1 && pDef->nLimits > 0) {
                for (j = 0; j < pDef->nLimits; j++) {
                    pLimit = &pDef->pLimits[j];
                    n = fn_8010DC94(pDefs, pLimit->nSlider);
                    if (n >= 0) {
                        pOther = &pDefs->pValues[n];
                        if (pOther->bFixed == 0) {
                            fLength = fn_80009680(pValue->fValue * pValue->fValue
                                                  + pOther->fValue * pOther->fValue);
                            if (fLength > pLimit->fLength && fLength != 0.0f) {
                                pValue->fValue = pLimit->fLength * (pValue->fValue / fLength);
                                pOther->fValue = pLimit->fLength * (pOther->fValue / fLength);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Let each slider move the ranges of the sliders it links to, keeping their values at the same
// place in their ranges.
void fn_8010DF8C(CharSliderDefs* pDefs) {
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderLink* pLink;
    CharSliderValue* pOther;
    f32 fT;
    f32 fSpan;
    f32 fPlace;
    u8 bMove;
    int i;
    int j;
    int n;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pDef = &pDefs->pDefs[i];
            pValue = &pDefs->pValues[i];
            if (pDef->nLinks > 0) {
                for (j = 0; j < pDef->nLinks; j++) {
                    pLink = &pDef->pLinks[j];
                    n = fn_8010DC94(pDefs, pLink->nSlider);
                    if (n >= 0) {
                        fT = 0.0f;
                        bMove = 0;
                        fSpan = fT;
                        if (pLink->fFrom < pLink->fTo) {
                            if (pValue->fValue <= pLink->fTo && pValue->fValue >= pLink->fFrom) {
                                fSpan = pLink->fTo - pLink->fFrom;
                                bMove = 1;
                                fT = (pValue->fValue - pLink->fFrom) / fSpan;
                            }
                        } else if (pLink->fFrom > pLink->fTo) {
                            if (pValue->fValue <= pLink->fFrom && pValue->fValue >= pLink->fTo) {
                                fSpan = pLink->fFrom - pLink->fTo;
                                bMove = 1;
                                fT = 1.0f - (pValue->fValue - pLink->fTo) / fSpan;
                            }
                        }
                        if (bMove) {
                            pOther = &pDefs->pValues[n];
                            if (pOther->fHigh != pOther->fLow) {
                                fPlace = (pOther->fValue - pOther->fLow) / (pOther->fHigh - pOther->fLow);
                            } else {
                                fPlace = pOther->fValue;
                            }
                            if (pLink->uFlags & 1) {
                                pOther->fLow = fT * fSpan + pOther->fLow;
                                pOther->fLow = pOther->fLow < 0.0f ? 0.0f
                                             : pOther->fLow > 1.0f ? 1.0f : pOther->fLow;
                            } else if (pLink->uFlags & 2) {
                                pOther->fHigh = fT * fSpan + pOther->fHigh;
                                pOther->fHigh = pOther->fHigh < 0.0f ? 0.0f
                                              : pOther->fHigh > 1.0f ? 1.0f : pOther->fHigh;
                            }
                            pOther->fValue = fPlace * (pOther->fHigh - pOther->fLow) + pOther->fLow;
                        }
                    }
                }
            }
        }
    }
}

// fX's place between fFrom and fTo (0..1, either way round), as a blend of fA to fB.
f32 fn_8010E194(f32 fFrom, f32 fTo, f32 fX, f32 fA, f32 fB) {
    f32 fT;

    if (fFrom == fTo) {
        return 0.0f;
    }
    if (fTo > fFrom) {
        fT = (fX - fFrom) / (fTo - fFrom);
        fT = fT < 0.0f ? 0.0f : fT > 1.0f ? 1.0f : fT;
    } else {
        fT = 1.0f - (fX - fFrom) / (fFrom - fTo);
        fT = fT < 0.0f ? 0.0f : fT > 1.0f ? 1.0f : fT;
    }
    return fT * (fB - fA) + fA;
}

// Scale the model's bones by the sliders.
void fn_8010E224(CharSliderDefs* pDefs, CharModel* pModel) {
    int i;
    int j;
    int k;
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderRange* pRange;
    CharSliderBone* pBone;
    f32 fScale;
    int nBone;

    if (pDefs == NULL || pModel == NULL) {
        return;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        pDef = &pDefs->pDefs[i];
        pValue = &pDefs->pValues[i];
        for (j = 0; j < pDef->nBoneRanges; j++) {
            pRange = &pDef->pBoneRanges[j];
            if (pRange->fStart <= pValue->fValue && pRange->fEnd > pValue->fValue) {
                for (k = 0; k < pRange->nItems; k++) {
                    pBone = &pRange->items.pBones[k];
                    fScale = fn_8010E194(pRange->fStart, pRange->fEnd, pValue->fValue,
                                         pBone->fFrom, pBone->fTo);
                    nBone = fn_800298F4(pModel, pBone->uId);
                    if (nBone >= 0) {
                        fn_80028A70(pModel, nBone, pBone->uAxes, fScale);
                    }
                }
            }
        }
    }
}

// Weight the skin's morph targets by the sliders (and mark the first 20 in the blend node).
void fn_8010E35C(CharSliderDefs* pDefs, Skin* pSkin, SKABlendNode* pNode) {
    int i;
    int j;
    int k;
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderRange* pRange;
    CharSliderMorph* pMorph;
    int m;
    f32 fWeight;

    if (pDefs == NULL || pSkin == NULL) {
        return;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        pDef = &pDefs->pDefs[i];
        pValue = &pDefs->pValues[i];
        for (j = 0; j < pDef->nMorphRanges; j++) {
            pRange = &pDef->pMorphRanges[j];
            if (pRange->fStart <= pValue->fValue && pRange->fEnd > pValue->fValue) {
                for (k = 0; k < pRange->nItems; k++) {
                    pMorph = &pRange->items.pMorphs[k];
                    fWeight = fn_8010E194(pRange->fStart, pRange->fEnd, pValue->fValue,
                                          pMorph->fFrom, pMorph->fTo);
                    for (m = 0; m < pDefs->nMorphs; m++) {
                        if (pMorph->uId == pDefs->aMorphIds[m]) {
                            fn_8011CADC(pSkin, m, fWeight);
                            if (m < 20) {
                                fn_800736D8(pNode, m);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Apply nSliders slider values (percentages) to a character's model and skin.
void fn_8010E4DC(CharSliderDefs* pDefs, CharModel* pModel, Skin* pSkin, int nSliders, u8* aValues,
                 SKABlendNode* pNode) {
    if (pModel == NULL || pSkin == NULL || aValues == NULL || pDefs == NULL) {
        return;
    }
    fn_80028A3C(pModel);
    fn_8010DCE8(pDefs);
    fn_8010DD4C(pDefs, nSliders, aValues);
    fn_8010DE60(pDefs);
    fn_8010DF8C(pDefs);
    fn_8010DE04(pDefs);
    fn_8010E224(pDefs, pModel);
    fn_8010E35C(pDefs, pSkin, pNode);
}

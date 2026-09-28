// CharSliders.c (EA's name, from its asserts; TW06): the character's body sliders: reading their
// definitions (CharSlider_CreateDefinitionsFromMem), freeing them, and blending a model's bones
// and a skin's morph targets by the slider values (CharSlider_UpdateCharacterBasedOnSliderValues). The file starts at CharSlider_Free
// (before it is GameMode26.c) and ends at fn_8010E58C, where FE_PGATourMessages.c's leaderboard
// messages begin.

#include "engine.h"
#include "character.h"
#include "charstate.h"
#include "golfer.h"

// The byte-swap layouts of the records CharSlider_CreateDefinitionsFromMem reads.
SwapField lbl_80193B70[5] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };   // CharSliderBone
SwapField lbl_80193B98[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };   // CharSliderRange of bones
SwapField lbl_80193BB8[3] = { { 8, 8 }, { 4, 4 }, { 4, 4 } };             // CharSliderMorph
SwapField lbl_80193BD0[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };   // CharSliderRange of morph targets
SwapField lbl_80193BF0[6] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };  // CharSliderLink
SwapField lbl_80193C20[2] = { { 4, 4 }, { 4, 4 } };                         // CharSliderLimit
SwapField lbl_80193C30[10] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                               { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };   // CharSliderDef
SwapField lbl_80281788[1] = { { 8, 8 } };                                   // a morph target id (u64)

// Free a slider-definition set made by CharSlider_CreateDefinitionsFromMem, with every table it
// owns (Character_Free); NULL is ignored.
void CharSlider_Free(CharSliderDefs* pDefs) {
    int i;
    int j;

    if (pDefs != NULL) {
        if (pDefs->pDefs != NULL) {
            for (i = 0; i < pDefs->nSliders; i++) {
                if (pDefs->pDefs[i].pLinks != NULL) {
                    StaticMem_Free(pDefs->pDefs[i].pLinks);
                }
                if (pDefs->pDefs[i].pLimits != NULL) {
                    StaticMem_Free(pDefs->pDefs[i].pLimits);
                }
                if (pDefs->pDefs[i].pBoneRanges != NULL) {
                    for (j = 0; j < pDefs->pDefs[i].nBoneRanges; j++) {
                        StaticMem_Free(pDefs->pDefs[i].pBoneRanges[j].items.pBones);
                    }
                    StaticMem_Free(pDefs->pDefs[i].pBoneRanges);
                }
                if (pDefs->pDefs[i].pMorphRanges != NULL) {
                    for (j = 0; j < pDefs->pDefs[i].nMorphRanges; j++) {
                        StaticMem_Free(pDefs->pDefs[i].pMorphRanges[j].items.pMorphs);
                    }
                    StaticMem_Free(pDefs->pDefs[i].pMorphRanges);
                }
            }
            StaticMem_Free(pDefs->pDefs);
        }
        if (pDefs->pValues != NULL) {
            StaticMem_Free(pDefs->pValues);
        }
        if (pDefs->aMorphIds != NULL) {
            StaticMem_Free(pDefs->aMorphIds);
        }
        StaticMem_Free(pDefs);
    }
}

// Read a character's slider definitions from its file data at *ppData (Character_CreateFromMem):
// the slider and morph target counts, then the sliders, each one's links, limits, bone ranges and
// their bones, morph ranges and their morph targets, and the morph target ids, each byte-swapped by
// its layout into memory from StaticMem_Alloc; *ppData is left after them. NULL when there are no
// sliders.
CharSliderDefs* CharSlider_CreateDefinitionsFromMem(u8** ppData) {
    void* pDst;
    s32 nSliders;
    s32 nMorphs;
    CharSliderDefs* pDefs;
    int i;
    int j;
    int k;
    // fake match: these four loops count with their own variables, not j and k (register allocation)
    int nMorphRange;
    int nBone;
    int nBoneRange;
    int nLimit;

    BYTESWAP_SWAPDATA(ppData, (u8*)&nSliders, 4, 4);
    BYTESWAP_SWAPDATA(ppData, (u8*)&nMorphs, 4, 4);
    *ppData += 8;
    if (nSliders <= 0) {
        return NULL;
    }

    pDefs = StaticMem_Alloc(sizeof(CharSliderDefs), 2, 0, "CharSliders.c", 1010);
    Mem_set(pDefs, 0, sizeof(CharSliderDefs));
    pDefs->nSliders = nSliders;
    pDefs->pDefs = StaticMem_Alloc(pDefs->nSliders * sizeof(CharSliderDef), 2, 0, "CharSliders.c", 1014);
    Mem_set(pDefs->pDefs, 0, pDefs->nSliders * sizeof(CharSliderDef));
    pDefs->pValues = StaticMem_Alloc(pDefs->nSliders * sizeof(CharSliderValue), 2, 0, "CharSliders.c", 1016);
    Mem_set(pDefs->pValues, 0, pDefs->nSliders * sizeof(CharSliderValue));
    pDefs->nMorphs = nMorphs;
    pDefs->aMorphIds = StaticMem_Alloc(pDefs->nMorphs * sizeof(u64), 2, 0, "CharSliders.c", 1020);
    Mem_set(pDefs->aMorphIds, 0, pDefs->nMorphs * sizeof(u64));

    // The sliders; the pointers read with them are not valid yet.
    for (i = 0; i < pDefs->nSliders; i++) {
        pDst = &pDefs->pDefs[i];
        ByteSwap_Records((void**)ppData, &pDst, lbl_80193C30, 10, 1);
        pDefs->pDefs[i].pBoneRanges = NULL;
        pDefs->pDefs[i].pMorphRanges = NULL;
        pDefs->pDefs[i].pLinks = NULL;
        pDefs->pDefs[i].pLimits = NULL;
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nLinks > 0) {
            pDefs->pDefs[i].pLinks = StaticMem_Alloc(pDefs->pDefs[i].nLinks * sizeof(CharSliderLink), 2, 0,
                                                 "CharSliders.c", 1039);
            for (j = 0; j < pDefs->pDefs[i].nLinks; j++) {
                pDst = &pDefs->pDefs[i].pLinks[j];
                ByteSwap_Records((void**)ppData, &pDst, lbl_80193BF0, 6, 1);
            }
        }
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nLimits > 0) {
            pDefs->pDefs[i].pLimits = StaticMem_Alloc(pDefs->pDefs[i].nLimits * sizeof(CharSliderLimit), 2, 0,
                                                  "CharSliders.c", 1053);
            for (nLimit = 0; nLimit < pDefs->pDefs[i].nLimits; nLimit++) {
                pDst = &pDefs->pDefs[i].pLimits[nLimit];
                ByteSwap_Records((void**)ppData, &pDst, lbl_80193C20, 2, 1);
            }
        }
    }

    // The bone ranges, then the bones of each.
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nBoneRanges > 0) {
            pDefs->pDefs[i].pBoneRanges
                    = StaticMem_Alloc(pDefs->pDefs[i].nBoneRanges * sizeof(CharSliderRange),
                                                      2, 0, "CharSliders.c", 1067);
            for (nBoneRange = 0; nBoneRange < pDefs->pDefs[i].nBoneRanges; nBoneRange++) {
                pDst = &pDefs->pDefs[i].pBoneRanges[nBoneRange];
                ByteSwap_Records((void**)ppData, &pDst, lbl_80193B98, 4, 1);
                pDefs->pDefs[i].pBoneRanges[nBoneRange].items.pBones = NULL;
            }
        }
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        for (j = 0; j < pDefs->pDefs[i].nBoneRanges; j++) {
            if (pDefs->pDefs[i].pBoneRanges[j].nItems > 0) {
                pDefs->pDefs[i].pBoneRanges[j].items.pBones =
                    StaticMem_Alloc(pDefs->pDefs[i].pBoneRanges[j].nItems * sizeof(CharSliderBone), 2, 0,
                                "CharSliders.c", 1084);
                for (nBone = 0; nBone < pDefs->pDefs[i].pBoneRanges[j].nItems; nBone++) {
                    pDst = &pDefs->pDefs[i].pBoneRanges[j].items.pBones[nBone];
                    ByteSwap_Records((void**)ppData, &pDst, lbl_80193B70, 5, 1);
                }
            }
        }
    }

    // The morph target ranges, then the morph targets of each.
    for (i = 0; i < pDefs->nSliders; i++) {
        if (pDefs->pDefs[i].nMorphRanges > 0) {
            pDefs->pDefs[i].pMorphRanges
                    = StaticMem_Alloc(pDefs->pDefs[i].nMorphRanges * sizeof(CharSliderRange),
                                                       2, 0, "CharSliders.c", 1100);
            for (nMorphRange = 0; nMorphRange < pDefs->pDefs[i].nMorphRanges; nMorphRange++) {
                pDst = &pDefs->pDefs[i].pMorphRanges[nMorphRange];
                ByteSwap_Records((void**)ppData, &pDst, lbl_80193BD0, 4, 1);
                pDefs->pDefs[i].pMorphRanges[nMorphRange].items.pMorphs = NULL;
            }
        }
    }
    for (i = 0; i < pDefs->nSliders; i++) {
        for (j = 0; j < pDefs->pDefs[i].nMorphRanges; j++) {
            if (pDefs->pDefs[i].pMorphRanges[j].nItems > 0) {
                pDefs->pDefs[i].pMorphRanges[j].items.pMorphs =
                    StaticMem_Alloc(pDefs->pDefs[i].pMorphRanges[j].nItems * sizeof(CharSliderMorph), 2, 0,
                                "CharSliders.c", 1117);
                for (k = 0; k < pDefs->pDefs[i].pMorphRanges[j].nItems; k++) {
                    pDst = &pDefs->pDefs[i].pMorphRanges[j].items.pMorphs[k];
                    ByteSwap_Records((void**)ppData, &pDst, lbl_80193BB8, 3, 1);
                }
            }
        }
    }

    // The morph targets' ids.
    for (i = 0; i < pDefs->nMorphs; i++) {
        pDst = &pDefs->aMorphIds[i];
        ByteSwap_Records((void**)ppData, &pDst, lbl_80281788, 1, 1);
    }
    return pDefs;
}

// The index of the slider whose id is nId, or -1 (also for NULL pDefs).
int CharSlider_GetSliderIndex(CharSliderDefs* pDefs, s32 nId) {
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

// Every slider back to its defaults: range 0..1, value 0, not fixed. NULL is ignored.
void CharSlider_ResetGameSettings(CharSliderDefs* pDefs) {
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

// Set the sliders with ids 0..nSliders-1 from aValues, signed percentages (value = aValues[id] /
// 100); an id no slider has is skipped.
void CharSlider_SetInitialVirtualValues(CharSliderDefs* pDefs, int nSliders, u8* aValues) {
    int i;
    int n;

    for (i = 0; i < nSliders; i++) {
        n = CharSlider_GetSliderIndex(pDefs, i);
        if (n >= 0) {
            pDefs->pValues[n].fValue = (s8)aValues[i] / 100.0f;
        }
    }
}

// Keep every value in 0..0.99999.
void CharSlider_ClampVirtualValues(CharSliderDefs* pDefs) {
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

// Limit paired sliders: each limit of a slider that is not fixed names a partner slider; when the
// partner is not fixed either and the two values, taken as a 2-D vector, are longer than the
// limit's fLength, both are scaled down to that length, so two sliders of a pair cannot both be
// near full. NULL is ignored.
void CharSlider_NormalizePairs(CharSliderDefs* pDefs) {
    int i;
    int j;
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderValue* pOther;
    CharSliderLimit* pLimit;
    f32 fLength;
    int n;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pDef = &pDefs->pDefs[i];
            pValue = &pDefs->pValues[i];
            if (pValue->bFixed != 1 && pDef->nLimits > 0) {
                for (j = 0; j < pDef->nLimits; j++) {
                    pLimit = &pDef->pLimits[j];
                    n = CharSlider_GetSliderIndex(pDefs, pLimit->nSlider);
                    if (n >= 0) {
                        pOther = &pDefs->pValues[n];
                        if (pOther->bFixed == 0) {
                            fLength = Math_Sqrt(pValue->fValue * pValue->fValue
                                                  + pOther->fValue * pOther->fValue);
                            if (!(fLength <= pLimit->fLength) && fLength != 0.0f) {
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

// Let each slider move the ranges of the sliders it links to. While a slider's value is inside a
// link's fFrom..fTo span (either way round), its place in the span (reversed when fFrom > fTo)
// times the span's length is added to the linked slider's low end (link flag 1) or high end (flag
// 2), clamped to 0..1, and the linked slider's value is moved to keep its place in its new range.
// NULL is ignored.
void CharSlider_PropogateEffects(CharSliderDefs* pDefs) {
    int i;
    int j;
    CharSliderDef* pDef;
    CharSliderValue* pValue;
    CharSliderLink* pLink;
    CharSliderValue* pOther;
    f32 fT;
    f32 fSpan;
    f32 fPlace;
    u8 bMove;
    int n;

    if (pDefs != NULL) {
        for (i = 0; i < pDefs->nSliders; i++) {
            pDef = &pDefs->pDefs[i];
            pValue = &pDefs->pValues[i];
            if (pDef->nLinks > 0) {
                for (j = 0; j < pDef->nLinks; j++) {
                    pLink = &pDef->pLinks[j];
                    n = CharSlider_GetSliderIndex(pDefs, pLink->nSlider);
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
                                pOther->fLow += fT * fSpan;
                                pOther->fLow = pOther->fLow < 0.0f ? 0.0f
                                             : pOther->fLow > 1.0f ? 1.0f : pOther->fLow;
                            } else if (pLink->uFlags & 2) {
                                pOther->fHigh += fT * fSpan;
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

// fX's place between fFrom and fTo (0..1, clamped) as a blend from fA to fB; 0 when fFrom equals
// fTo. Gives a slider bone's scale or a morph target's weight for a slider value.
// EA bug: when fTo < fFrom the place is measured from fFrom instead of fTo, so any fX between them
// comes out 1 (fB); both callers only pass fFrom < fTo.
f32 CharSlider_CalculateActualModAmount(f32 fFrom, f32 fTo, f32 fX, f32 fA, f32 fB) {
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

// Scale the model's bones by the sliders: for each bone range a slider's value is in (fStart up to,
// not including, fEnd), each of the range's bones, found by name id, is scaled on its axes (uAxes)
// by the value's place in the range blended from the bone's fFrom to fTo
// (CharSlider_CalculateActualModAmount, SKEL_ScaleBone). NULL either way does nothing.
void CharSlider_SetBoneModifiers(CharSliderDefs* pDefs, CharModel* pModel) {
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
                    fScale = CharSlider_CalculateActualModAmount(pRange->fStart, pRange->fEnd, pValue->fValue,
                                         pBone->fFrom, pBone->fTo);
                    nBone = SKEL_GetBoneIDFromNameID(pModel, pBone->uId);
                    if (nBone >= 0) {
                        SKEL_ScaleBone(pModel, nBone, pBone->uAxes, fScale);
                    }
                }
            }
        }
    }
}

// Weight the skin's morph targets by the sliders: for each morph range a slider's value is in
// (fStart up to, not including, fEnd), each of the range's morph targets gets the value's place in
// the range blended from its fFrom to fTo (CharSlider_CalculateActualModAmount) as its weight
// (SkinMorph_SetTargetWeight, by its index in aMorphIds); the first 20 are also unmarked in the
// blend node (SKABlender_ClearMorph) so the animation no longer sets them. NULL pDefs or pSkin does
// nothing.
void CharSlider_SetMorphTargets(CharSliderDefs* pDefs, Skin* pSkin, SKABlendNode* pNode) {
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
                    fWeight = CharSlider_CalculateActualModAmount(pRange->fStart, pRange->fEnd,
                            pValue->fValue,
                                          pMorph->fFrom, pMorph->fTo);
                    for (m = 0; m < pDefs->nMorphs; m++) {
                        if (pMorph->uId == pDefs->aMorphIds[m]) {
                            SkinMorph_SetTargetWeight(pSkin, m, fWeight);
                            if (m < 20) {
                                SKABlender_ClearMorph(pNode, m);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Apply nSliders slider values (signed percentages, the created golfer's body and face settings) to
// a character's model and skin: bone scales reset, sliders reset, set from aValues, paired sliders
// limited, links applied, values clamped, then the bones scaled and the morph targets weighted.
// Nothing unless pDefs, pModel, pSkin and aValues are all set.
void CharSlider_UpdateCharacterBasedOnSliderValues(CharSliderDefs* pDefs, CharModel* pModel, Skin* pSkin, int nSliders, u8* aValues,
                 SKABlendNode* pNode) {
    if (pModel == NULL || pSkin == NULL || aValues == NULL || pDefs == NULL) {
        return;
    }
    SKEL_ResetBoneScales(pModel);
    CharSlider_ResetGameSettings(pDefs);
    CharSlider_SetInitialVirtualValues(pDefs, nSliders, aValues);
    CharSlider_NormalizePairs(pDefs);
    CharSlider_PropogateEffects(pDefs);
    CharSlider_ClampVirtualValues(pDefs);
    CharSlider_SetBoneModifiers(pDefs, pModel);
    CharSlider_SetMorphTargets(pDefs, pSkin, pNode);
}

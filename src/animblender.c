// AnimBlender.c (EA's name: TW2005's GameCube build has the path
// c:/dev/Golf_2005/Tigercode/Code/Golf/Animation/AnimBlender.c, and TW07's AnimBlender.c has these
// functions in the same order, as C++ classes): blends a character's animations. A character has
// two blend trees (SKABlendNode): the skeleton's, whose channels play skeletal clips (Clip; format
// 0 pose buffers, SkelPose), and the morphs', whose channels play morph libraries (MtaLib,
// mtalib.c; format 1 buffers, SkelPose1). In EA's words a blender (nType 1) mixes its two children
// with a blend callback (SKABlender_BlendLinear) and a channel (nType 0, SKASourceNode) plays one
// clip over a time window; nodes and pose buffers come from five pools. The animation player's
// functions (AnimPlayer, EA's TSKATime) follow: SKATime_Update moves its time through the tree's
// window, at which SKABlender_Update then poses the tree.

#include "character.h"
#include "charstate.h"
#include "golfer.h"

f32  SKA_GetTagTime(Clip* pBlend, u64 uEvent);   // an event's time (by its 64-bit id)
void MtaLib_Free(void* pItem);          // mtalib.c
void fn_800977CC(void* p);              // (sweep code) frees a clip: its Clip.pC0 memory
// Skeleton.c
void SKEL_BlendPoses(int nBone, int nCount, SkelPose* pA, SkelPose* pB, SkelPose* pOut, f32 fT);

int  SKABlender_FindOldestChannel(SKABlendNode* pNode, SKABlendNode*** pppOldest);
int  SKABlender_GetCurrentChannel(SKABlendNode* pNode, f32 fTime);
f32  SKABlender_GetStartTime(SKABlendNode* pNode);
void SKATime_Pause(u8* pAnim);
f32  SKATime_MapTime(f32 fTime, f32 fNow, f32 fStart, f32 fEnd);
int  SKABlender_NumSKAsInBlender(SKABlendNode* pNode);
f32  SKATime_CalcStep(AnimPlayer* pPlayer, f32 fT);

// The pools (AnimBlender_InitModule), defined here last address first (CodeWarrior lays out .sbss
// in reverse).
UMemPool* gSKAChannelPool;      // channels (nType 0, SKASourceNode, 0x34 bytes)
UMemPool* gSKABlenderPool;      // blenders (nType 1, SKABlendNode, 0x2C bytes)
UMemPool* gSKABlendDataPool;    // other node types (0x20 bytes: the fields before the union)
UMemPool* gSkelPosePool;        // format 0 pose buffers (SkelPose)
UMemPool* gMorphPosePool;       // format 1 pose buffers (SkelPose1: morph weights and a SkelPose)

// Creates the blend tree's pools, nNumEntries of each (10 in the front end, game types 10 and 3,
// else 50): nodes by type (channels 0x34 bytes, blenders 0x2C, other nodes 0x20), then pose buffers
// of format 0 (SkelPose) and format 1 (SkelPose1).
void AnimBlender_InitModule(void) {
    int nNumEntries;

    if (gSession.nGameType == 3 || gSession.nGameType == 10) {
        nNumEntries = 10;
    } else {
        nNumEntries = 50;
    }
    gSKAChannelPool = CreateMemPool(nNumEntries, 0x34, 2, 16);
    gSKABlenderPool = CreateMemPool(nNumEntries, 0x2C, 2, 16);
    gSKABlendDataPool = CreateMemPool(nNumEntries, 0x20, 2, 16);
    gSkelPosePool = CreateMemPool(nNumEntries, sizeof(SkelPose), 2, 16);
    gMorphPosePool = CreateMemPool(nNumEntries, 0x114C, 2, 16);
}

// Destroy the pools AnimBlender_InitModule made.
void AnimBlender_CloseModule(void) {
    if (gSKAChannelPool != NULL) {
        DeleteMemPool(gSKAChannelPool);
        gSKAChannelPool = NULL;
    }
    if (gSKABlenderPool != NULL) {
        DeleteMemPool(gSKABlenderPool);
        gSKABlenderPool = NULL;
    }
    if (gSKABlendDataPool != NULL) {
        DeleteMemPool(gSKABlendDataPool);
        gSKABlendDataPool = NULL;
    }
    if (gSkelPosePool != NULL) {
        DeleteMemPool(gSkelPosePool);
        gSkelPosePool = NULL;
    }
    if (gMorphPosePool != NULL) {
        DeleteMemPool(gMorphPosePool);
        gMorphPosePool = NULL;
    }
}

// Sets up *ppNode as an empty node of nType (0 a channel that plays one clip, 1 a blender that
// mixes two children with pfnBlend, else a plain node): taken from nType's pool when *ppNode is
// NULL (bPooled then 1), no times, half weight, bFreeASAP (free it as soon as it has ended) from
// bFreeASAP, and a fresh pose buffer of nFormat from its pool (no bones set; format 1's morph
// blocks all marked, their weights 0). Gives up quietly when a pool is empty.
void SKABlendData_Init(SKABlendNode** ppNode, int nType, int nFormat, SKABlendFn pfnBlend,
                       int bFreeASAP) {
    SKABlendNode* pNode;
    s32 i;
    s32 j;

    if (ppNode == NULL) return;
    if (*ppNode == NULL) {
        switch (nType) {
        case 0:
            *ppNode = AllocPoolMem(gSKAChannelPool);
            break;
        case 1:
            *ppNode = AllocPoolMem(gSKABlenderPool);
            break;
        default:
            *ppNode = AllocPoolMem(gSKABlendDataPool);
            break;
        }
        if (*ppNode == NULL) return;
        (*ppNode)->bPooled = 1;
    } else {
        (*ppNode)->bPooled = 0;
    }
    (*ppNode)->nType = nType;
    (*ppNode)->nFormat = nFormat;
    (*ppNode)->bFreeASAP = bFreeASAP;
    (*ppNode)->fStart = (*ppNode)->fEnd = 0.0f;
    (*ppNode)->fWeight = 0.5f;
    if ((*ppNode)->nFormat == 0) {
        (*ppNode)->pPose = AllocPoolMem(gSkelPosePool);
        if ((*ppNode)->pPose == NULL) return;
        BitArray_ClearArray((*ppNode)->pPose->a0, 128);
        BitArray_ClearArray((*ppNode)->pPose->a10, 128);
        BitArray_FillArray((*ppNode)->pPose->a20, 128);
        BitArray_FillArray((*ppNode)->pPose->a30, 128);
    } else if ((*ppNode)->nFormat == 1) {
        (*ppNode)->pPose = AllocPoolMem(gMorphPosePool);
        if ((*ppNode)->pPose == NULL) return;
        BitArray_ClearArray(((SkelPose1*)(*ppNode)->pPose)->pose.a0, 128);
        BitArray_ClearArray(((SkelPose1*)(*ppNode)->pPose)->pose.a10, 128);
        BitArray_FillArray(((SkelPose1*)(*ppNode)->pPose)->pose.a20, 128);
        BitArray_FillArray(((SkelPose1*)(*ppNode)->pPose)->pose.a30, 128);
        for (i = 0; i < 3; i++) {
            BitArray_FillArray(((SkelPose1*)(*ppNode)->pPose)->aBlocks[i].aBits, 20);
            for (j = 0; j < 20; j++) {
                ((SkelPose1*)(*ppNode)->pPose)->aBlocks[i].af8[j] = 0.0f;
            }
        }
    }
    pNode = *ppNode;
    if (pNode->nType == 0) {
        pNode->u.src.pSrc = NULL;
        ((SKASourceNode*)pNode)->n30 = 0;
        pNode->u.src.fFrom = pNode->u.src.fTo = 0.0f;
    } else if (pNode->nType == 1) {
        pNode->u.blend.pfnBlend = pfnBlend;
        pNode->u.blend.apChild[0] = NULL;
        pNode->u.blend.apChild[1] = NULL;
    }
}

// Give the tree at *ppNode back: each node's pose buffer to its pool, then its children (or, with
// bFreeSources, a source node's clip), then the node itself if it came from a pool (*ppNode is
// then NULL).
void SKABlendData_Shutdown(SKABlendNode** ppNode, u8 bFreeSources) {
    int i;

    if (ppNode == NULL) return;
    if (*ppNode == NULL) return;
    if ((*ppNode)->nFormat == 0) {
        if ((*ppNode)->pPose != NULL) {
            ReturnPoolMem(gSkelPosePool, (*ppNode)->pPose);
        }
    } else if ((*ppNode)->nFormat == 1) {
        if ((*ppNode)->pPose != NULL) {
            ReturnPoolMem(gMorphPosePool, (*ppNode)->pPose);
        }
    }
    (*ppNode)->pPose = NULL;
    if ((*ppNode)->nType == 1) {
        for (i = 0; i < 2; i++) {
            SKABlendData_Shutdown(&(*ppNode)->u.blend.apChild[i], bFreeSources);
        }
    } else if ((*ppNode)->nType == 0) {
        if ((*ppNode)->nFormat == 0) {
            if (bFreeSources) {
                fn_800977CC((*ppNode)->u.src.pSrc);
            }
        } else if ((*ppNode)->nFormat == 1) {
            if (bFreeSources) {
                MtaLib_Free((*ppNode)->u.src.pSrc);
            }
        }
    }
    if ((*ppNode)->bPooled == 1) {
        switch ((*ppNode)->nType) {
        case 0:
            ReturnPoolMem(gSKAChannelPool, *ppNode);
            break;
        case 1:
            ReturnPoolMem(gSKABlenderPool, *ppNode);
            break;
        default:
            ReturnPoolMem(gSKABlendDataPool, *ppNode);
            break;
        }
        *ppNode = NULL;
    }
}

// Adds pNew to the tree at *ppNode. Its times come from pInfo (SKABlend_CalculateBlendInfo's six
// floats: [3] and [4] its start and end, [0] and [1] a channel's clip window); with no pInfo it
// keeps its length and starts where the tree ends. A channel gets a fresh pose from pChar (when
// given). pNew takes a free child slot of *ppNode (a pNew that is not freed as soon as it ends
// keeps *ppNode from being so too); with both slots taken, *ppNode's contents move into a new
// pooled blender (bFreeASAP) and that and pNew become *ppNode's two children. *ppNode's times then
// cover its children's.
void SKABlender_AddBlenderData(Character* pChar, SKABlendNode* pNew, SKABlendNode** ppNode,
                               f32* pInfo, SKABlendFn pfnBlend, int bFreeASAP) {
    s32 i = 0;
    u8 bFree = 0;
    SKABlendNode* pNewBlend = NULL;
    s32 j;
    f32 aInfo[6];
    SkelPose* pPose;

    if (ppNode == NULL) return;
    if (*ppNode != NULL) {
        if (pNew != NULL) {
            if (pInfo == NULL) {
                pNew->fEnd -= pNew->fStart;
                pNew->fStart = SKABlender_GetEndTime(*ppNode);
                pNew->fEnd += pNew->fStart;
            } else {
                pNew->fStart = pInfo[3];
                pNew->fEnd = pInfo[4];
                if (pNew->nType == 0) {
                    pNew->u.src.fFrom = pInfo[0];
                    pNew->u.src.fTo = pInfo[1];
                }
            }
            if (pChar != NULL && pNew->nType == 0) {
                if (pNew->nFormat == 0) {
                    Character_InitBoneState(pChar, pNew->pPose);
                } else if (pNew->nFormat == 1) {
                    Character_InitBoneStateBits(pChar, &((SkelPose1*)pNew->pPose)->pose);
                    for (j = 0; j < 3; j++) {
                        memset(&((SkelPose1*)pNew->pPose)->aBlocks[j], 0, sizeof(SkelPoseBlock));
                        BitArray_FillArray(((SkelPose1*)pNew->pPose)->aBlocks[j].aBits, 20);
                    }
                }
            }
        }
        while (i < 2 && !bFree) {
            if ((*ppNode)->u.blend.apChild[i] != NULL) {
                i++;
            } else {
                bFree = 1;
            }
        }
        if (bFree) {
            (*ppNode)->u.blend.apChild[i] = pNew;
            if (pNew->bFreeASAP == 0) {
                (*ppNode)->bFreeASAP = 0;
            }
        } else {
            SKABlendData_Init(&pNewBlend, 1, (*ppNode)->nFormat, pfnBlend, bFreeASAP);
            if ((*ppNode)->nFormat == 0) {
                memcpy(pNewBlend->pPose, (*ppNode)->pPose, sizeof(SkelPose));
            } else {
                memcpy(pNewBlend->pPose, (*ppNode)->pPose, sizeof(SkelPose1));
            }
            pPose = pNewBlend->pPose;
            memcpy(pNewBlend, *ppNode, sizeof(SKABlendNode));
            pNewBlend->bPooled = 1;
            pNewBlend->pPose = pPose;
            (*ppNode)->u.blend.apChild[0] = NULL;
            (*ppNode)->u.blend.apChild[1] = NULL;
            aInfo[3] = pNewBlend->fStart;
            aInfo[4] = pNewBlend->fEnd;
            aInfo[5] = 0.0f;
            SKABlender_AddBlenderData(NULL, pNewBlend, ppNode, aInfo, pfnBlend, bFreeASAP);
            SKABlender_AddBlenderData(NULL, pNew, ppNode, pInfo, pfnBlend, pNew->bFreeASAP);
        }
    } else {
        // EA bug: *ppNode is NULL here, so this reads nFormat through NULL, sets up a node over
        // ppNode's own slot and stores pNew through NULL; no caller passes an empty slot.
        SKABlendData_Init((SKABlendNode**)&ppNode, 1, (*ppNode)->nFormat, pfnBlend, bFreeASAP);
        (*ppNode)->u.blend.apChild[0] = pNew;
    }
    (*ppNode)->fStart = SKABlender_GetStartTime(*ppNode);
    (*ppNode)->fEnd = SKABlender_GetEndTime(*ppNode);
}

// Returns how many channels (clips) the tree under pNode plays, as SKABlender_NumSKAsInBlender
// does, and points *pppOldest at the child slot of the one that ends first (left alone when it
// already points at one that ends earlier).
int SKABlender_FindOldestChannel(SKABlendNode* pNode, SKABlendNode*** pppOldest) {
    int i = 0;
    int nSources = 0;
    SKABlendNode* pChild;

    if (pNode == NULL) return 0;
    do {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                nSources += SKABlender_FindOldestChannel(pChild, pppOldest);
            } else if (pChild->nType == 0) {
                if (*pppOldest == NULL) {
                    *pppOldest = &pNode->u.blend.apChild[i];
                } else if (pChild->fEnd < (**pppOldest)->fEnd) {
                    *pppOldest = &pNode->u.blend.apChild[i];
                }
                nSources++;
            }
        }
        i++;
    } while (i < 2);
    return nSources;
}

// Makes pNew a channel that plays all of pClip (a Clip for format 0, an MtaLib for format 1) from
// time 0, at weight fWeight. When the tree at pNode already plays gMaxBlendClips clips, it is given
// back first and pNode set up again as a blender of its format (SKABlender_BlendLinear, weight 0.5,
// freed as soon as it has ended).
void SKAChannel_SetChannel(SKABlendNode* pNode, SKABlendNode* pNew, void* pClip, f32 fWeight) {
    SKABlendNode** ppOldest = NULL;

    if (pNew == NULL) return;
    if (SKABlender_FindOldestChannel(pNode, &ppOldest) >= gMaxBlendClips) {
        SKABlendData_Shutdown(&pNode, 0);
        SKABlendData_Init(&pNode, 1, pNode->nFormat, SKABlender_BlendLinear, 1);
        SKABlender_SetBlender(pNode, SKABlender_BlendLinear, 0.5f);
    }
    pNew->nType = 0;
    pNew->u.src.pSrc = pClip;
    ((SKASourceNode*)pNew)->f2C = 0.0f;
    pNew->u.src.fFrom = 0.0f;
    pNew->fStart = 0.0f;
    if (pNew->nFormat == 0) {
        pNew->fEnd = pNew->u.src.fTo = ((Clip*)pClip)->f18;
    } else if (pNew->nFormat == 1) {
        pNew->fEnd = pNew->u.src.fTo = ((MtaLib*)pClip)->f1C;
    }
    pNew->fWeight = fWeight;
}

// Makes pNode a blender that mixes its children with pfnBlend, at weight fWeight in its parent's
// blend, and takes its times from its children.
void SKABlender_SetBlender(SKABlendNode* pNode, SKABlendFn pfnBlend, f32 fWeight) {
    if (pNode != NULL) {
        pNode->nType = 1;
        pNode->fWeight = fWeight;
        pNode->u.blend.pfnBlend = pfnBlend;
        pNode->fStart = SKABlender_GetStartTime(pNode);
        pNode->fEnd = SKABlender_GetEndTime(pNode);
    }
}

// Poses the tree at pNode at fTime (its player's time): takes its times from its children; gives
// back a child marked to be freed once it has ended (bFreeASAP) when it has, while the other plays;
// poses each channel at its clip time (fTime's place between fStart and fEnd carried over to
// fFrom..fTo, clamped, kept in f2C; a format 0 channel past its clip's end keeps its last pose) and
// each blender the same way; then mixes the children with pfnBlend. In a gap between two format 0
// channels, when only the earlier one's clip holds the club in the hand (clip flag 0x10,
// Character_UpdateClubAttachment), its grip bone takes the grip held from the root (qGripFromRoot,
// vGripFromRoot), so the club does not jump.
void SKABlender_Update(Character* pChar, SKABlendNode* pNode, CharModel* pModel, f32 fTime) {
    int nChannel;
    s32 i;
    SKABlendNode* pA;
    SKABlendNode* pB;
    SKABlendNode* pFirst;
    Clip* pClipFirst;
    Clip* pClipOther;
    Clip* pSwap;
    SkelPose* pPose;
    SKABlendNode* pChild;
    int bInside;
    f32 fClip;

    if (pNode == NULL) return;
    pNode->fStart = SKABlender_GetStartTime(pNode);
    pNode->fEnd = SKABlender_GetEndTime(pNode);
    nChannel = SKABlender_GetCurrentChannel(pNode, fTime);
    if (nChannel < 0 && pNode->nFormat == 0) {
        pA = pNode->u.blend.apChild[0];
        if (pA != NULL) {
            pB = pNode->u.blend.apChild[1];
            if (pB != NULL) {
                pFirst = pA;
                if (pA->nType == 0 && pB->nType == 0) {
                    pClipFirst = pA->u.src.pSrc;
                    pClipOther = pB->u.src.pSrc;
                    if (pA->fStart > pB->fStart) {
                        pSwap = pClipFirst;
                        pClipFirst = pClipOther;
                        pClipOther = pSwap;
                        pFirst = pB;
                    }
                    if ((pClipFirst->uFlags & 0x10) && !(pClipOther->uFlags & 0x10)) {
                        pPose = pFirst->pPose;
                        LLMath_CopyVec(pChar->qGripFromRoot, pPose->aBones[pChar->nGripBone].q0);
                        LLMath_CopyVec(pChar->vGripFromRoot, pPose->aBones[pChar->nGripBone].v10);
                    }
                }
            }
        }
    }
    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL && pChild->bFreeASAP && pChild->fStart < fTime && pChild->fEnd < fTime &&
            nChannel > -1) {
            SKABlendData_Shutdown(&pNode->u.blend.apChild[i], 0);
        }
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                SKABlender_Update(pChar, pChild, pModel, fTime);
            } else {
                bInside = 1;
                // the clip time: fTime's point between fStart and fEnd, carried over to fFrom..fTo
                fClip = fTime - pChild->fStart;
                fClip = fClip * ((pChild->u.src.fTo - pChild->u.src.fFrom) /
                                 (pChild->fEnd - pChild->fStart)) +
                        pChild->u.src.fFrom;
                if (fClip >= pChild->u.src.fTo) {
                    fClip = pChild->u.src.fTo;
                    bInside = 0;
                } else if (fClip < pChild->u.src.fFrom) {
                    fClip = pChild->u.src.fFrom;
                }
                ((SKASourceNode*)pChild)->f2C = fClip;
                if (pChild->nFormat == 0) {
                    if (bInside) {
                        SKA_Update(pChar, pChild->u.src.pSrc, pChild->pPose, 0, fClip);
                    }
                } else if (pChild->nFormat == 1) {
                    MtaLib_ApplyToPose(pChar, pChild->u.src.pSrc, (SkelPose1*)pChild->pPose, fClip);
                }
            }
        }
    }
    pNode->u.blend.pfnBlend(pNode, pModel, fTime);
}

// Which of pNode's children play at fTime: -1 neither, 0 or 1 that one, 2 both.
int SKABlender_GetCurrentChannel(SKABlendNode* pNode, f32 fTime) {
    SKABlendNode* pChild;
    int i = 0;
    int nChannel = -1;

    if (pNode != NULL) {
        while (i < 2 && nChannel < 2) {
            pChild = pNode->u.blend.apChild[i];
            if (pChild != NULL && !(pChild->fStart > fTime) && !(pChild->fEnd < fTime)) {
                if (nChannel == -1) {
                    nChannel = i;
                } else {
                    nChannel = 2;
                }
            }
            i++;
        }
    }
    return nChannel;
}

// The earliest start of pNode's children (0 without children).
f32 SKABlender_GetStartTime(SKABlendNode* pNode) {
    f32 fStart = 1073741824.0f;
    int bFound = 0;
    SKABlendNode* pChild;
    int i;

    if (pNode != NULL) {
        for (i = 0; i < 2; i++) {
            pChild = pNode->u.blend.apChild[i];
            if (pChild != NULL) {
                bFound = 1;
                if (pChild->fStart < fStart) {
                    fStart = pChild->fStart;
                }
            }
        }
    }
    if (bFound == 0) return 0.0f;
    return fStart;
}

// The latest end of pNode's children (0 without children).
f32 SKABlender_GetEndTime(SKABlendNode* pNode) {
    f32 fEnd = 0.0f;
    SKABlendNode* pChild;
    int i;

    if (pNode == NULL) return fEnd;
    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL && pChild->fEnd > fEnd) {
            fEnd = pChild->fEnd;
        }
    }
    return fEnd;
}

// The weight of pA (a blender's first child) at fTime, running linearly across the overlap of pA
// and pB, from the later start to the earlier end: from 0 to 1 when pA starts later (it fades in),
// from 1 to 0 when pB does. With bOut, across the gap from the earlier end to the later start
// instead. When one node lies inside the other, the weight w is folded to abs(2w - 1).
f32 SKABlender_CalcBlendWeight(SKABlendNode* pA, SKABlendNode* pB, u8 bOut, f32 fTime) {
    f32 fStart;
    f32 fEnd;
    int nLater;
    u8 bInside;
    f32 fDir;
    f32 fWeight;

    if (pA->fStart > pB->fStart) {
        fStart = pA->fStart;
        nLater = 0;
    } else {
        nLater = 1;
        fStart = pB->fStart;
    }
    if (pA->fEnd >= pB->fEnd) {
        fEnd = pB->fEnd;
        bInside = nLater == 1;
    } else {
        fEnd = pA->fEnd;
        bInside = nLater == 0;
    }
    fDir = 2.0f * ((f32)nLater - 0.5f);
    if (!bOut) {
        f32 fLen = fEnd - fStart;

        if (fLen < 0.00001f) {
            fWeight = nLater;
        } else {
            fWeight = nLater - fDir * (fTime - fStart) / fLen;
        }
    } else {
        f32 fLen = fStart - fEnd;

        if (fLen < 0.00001f) {
            fWeight = nLater;
        } else {
            fWeight = nLater - fDir * (fTime - fEnd) / fLen;
        }
    }
    if (bInside) {
        return fabsf(2.0f * fWeight - 1.0f);
    }
    return fWeight;
}

// The blend callback: pose pNode's buffer at fTime from its children. While both play (or neither,
// between them) and fTime is inside their span, their weights come from
// SKABlender_CalcBlendWeight and the poses are blended by format; while only one plays, its pose
// is copied (a format 1 copy then has the child's three block masks cleared).
void SKABlender_BlendLinear(SKABlendNode* pNode, CharModel* pModel, f32 fTime) {
    int nChannel;
    u8 bBetween;
    f32 fWeight;
    int i;

    if (pNode == NULL) return;
    if (pNode->nType != 1) return;
    nChannel = SKABlender_GetCurrentChannel(pNode, fTime);
    if (nChannel == 2 || (nChannel == -1 && pNode->u.blend.apChild[0] != NULL &&
                          pNode->u.blend.apChild[1] != NULL)) {
        bBetween = nChannel == -1;
        if (fTime >= pNode->u.blend.apChild[0]->fEnd && fTime >= pNode->u.blend.apChild[1]->fEnd) {
            return;
        }
        if (fTime <= pNode->u.blend.apChild[0]->fStart && fTime <= pNode->u.blend.apChild[1]->fStart) {
            return;
        }
        fWeight = SKABlender_CalcBlendWeight(pNode->u.blend.apChild[0], pNode->u.blend.apChild[1], bBetween,
                                             fTime);
        pNode->u.blend.apChild[0]->fWeight = fWeight;
        // the pose blend below takes the second child's weight
        fWeight = 1.0f - fWeight;
        pNode->u.blend.apChild[1]->fWeight = fWeight;
        if (pNode->nFormat == 0) {
            SKEL_BlendPoses(1, pModel->nBones - 1, pNode->u.blend.apChild[0]->pPose,
                        pNode->u.blend.apChild[1]->pPose, pNode->pPose, fWeight);
        } else if (pNode->nFormat == 1) {
            SKN_BlendMorphWeights((SkelPose1*)pNode->u.blend.apChild[0]->pPose,
                        (SkelPose1*)pNode->u.blend.apChild[1]->pPose, (SkelPose1*)pNode->pPose, fWeight);
        }
    } else if (nChannel != -1) {
        if (pNode->u.blend.apChild[nChannel]->nFormat == 0) {
            memcpy(pNode->pPose, pNode->u.blend.apChild[nChannel]->pPose, sizeof(SkelPose));
        } else if (pNode->u.blend.apChild[nChannel]->nFormat == 1) {
            memcpy(pNode->pPose, pNode->u.blend.apChild[nChannel]->pPose, sizeof(SkelPose1));
            for (i = 0; i < 3; i++) {
                BitArray_ClearArray(((SkelPose1*)pNode->u.blend.apChild[nChannel]->pPose)->aBlocks[i].aBits,
                                  20);
            }
        }
    }
}

// The time of SKA tag (timed event) uEvent in the first format 0 channel under pNode whose clip has
// it, depth first (0 when none has, or its time is 0).
f32 SKABlender_GetTagTime(SKABlendNode* pNode, u64 uEvent) {
    f32 fTime = 0.0f;
    int i = 0;
    SKABlendNode* pChild;

    if (pNode == NULL) return fTime;
    while (fTime == 0.0f && i < 2) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                fTime = SKABlender_GetTagTime(pChild, uEvent);
            } else if (pChild->nType == 0 && pChild->nFormat == 0) {
                fTime = SKA_GetTagTime(pChild->u.src.pSrc, uEvent);
            }
        }
        i++;
    }
    return fTime;
}

// Resets an animation player (EA's TSKATime): time, flags, play count and queued transition 0, time
// scale (fTimeScale) 1, and its ten entries chained both ways from p44.
void SKATime_Init(AnimPlayer* pPlayer) {
    int i;
    AnimPlayerEntry* pPrev;

    pPlayer->fTime = 0.0f;
    pPlayer->nPlays = 0;
    pPlayer->uFlags = 0;
    pPlayer->n00 = 0;
    pPlayer->fTimeScale = 1.0f;
    pPlayer->nTransitionState = 0;
    pPlayer->fTransitionTime = 0.0f;
    pPlayer->n3C = 0;
    pPlayer->n40 = 0;
    pPlayer->p44 = &pPlayer->a48[0];
    pPrev = NULL;
    for (i = 0; i < 10; i++) {
        pPlayer->a48[i].pNext = (i < 9) ? &pPlayer->a48[i + 1] : NULL;
        pPlayer->a48[i].pPrev = pPrev;
        pPrev = &pPlayer->a48[i];
    }
}

// Advances a player by its step for fT (SKATime_CalcStep) across the times of the tree under pNode,
// unless uFlags bit 0 holds it (with bit 7, fHoldTime counts down, then clears bits 0 and 7):
// forward, or backward with bit 6. At an end nPlays counts the plays down (at 0 the player stops
// there with bit 2 set; going forward with bit 8 it rewinds to 0 and clears bits 0, 2 and 8
// instead); with bit 5 it turns round (bit 6 flips, bit 2 set), else it wraps to the other end and
// sets bit 12.
void SKATime_Update(AnimPlayer* pPlayer, SKABlendNode* pNode, f32 fT) {
    f32 fStep;
    f32 fEnd;
    f32 fStart;

    pPlayer->uFlags &= ~0x1000;
    pPlayer->fStart = SKABlender_GetStartTime(pNode);
    pPlayer->fEnd = SKABlender_GetEndTime(pNode);
    if (pPlayer->uFlags & 0x80) {
        pPlayer->fHoldTime -= fT;
        if (pPlayer->fHoldTime <= 0.0f) {
            pPlayer->fHoldTime = 0.0f;
            pPlayer->uFlags &= ~0x81;
        }
    }
    pPlayer->uFlags &= ~4;
    fStep = SKATime_CalcStep(pPlayer, fT);
    fStart = pPlayer->fStart;
    fEnd = pPlayer->fEnd;
    if (pPlayer->uFlags & 1) return;
    if (pPlayer->uFlags & 0x40) {
        pPlayer->fTime -= fStep;
        if (pPlayer->fTime < fStart) {
            if (pPlayer->nPlays != 0 && pPlayer->nPlays > 0) {
                pPlayer->nPlays--;
            }
            if (pPlayer->nPlays == 0) {
                pPlayer->uFlags |= 4;
                pPlayer->fTime = fStart;
                return;
            }
            if (pPlayer->uFlags & 0x20) {
                pPlayer->fTime = fStart;
                pPlayer->uFlags ^= 0x40;
                pPlayer->uFlags |= 4;
                return;
            }
            pPlayer->fTime = fEnd;
            pPlayer->uFlags |= 0x1000;
        }
    } else {
        pPlayer->fTime += fStep;
        if (pPlayer->fTime > fEnd) {
            if (pPlayer->nPlays != 0 && pPlayer->nPlays > 0) {
                pPlayer->nPlays--;
            }
            if (pPlayer->nPlays == 0) {
                if (pPlayer->uFlags & 0x100) {
                    pPlayer->fTime = 0.0f;
                    pPlayer->uFlags &= ~0x105;
                    pPlayer->n00 = 0;
                    pPlayer->nPlays = 1;
                    return;
                }
                pPlayer->uFlags |= 4;
                pPlayer->fTime = fEnd;
                return;
            }
            if (pPlayer->uFlags & 0x20) {
                pPlayer->fTime = fEnd;
                pPlayer->uFlags ^= 0x40;
                pPlayer->uFlags |= 4;
                return;
            }
            pPlayer->fTime = fStart;
            pPlayer->uFlags |= 0x1000;
        }
    }
}

// The idle sway of a golfer standing still: pPlayer's time moves towards fIdleCentre minus a smooth
// wave (three cosines of the fIdleClock clock, advanced by fT) from 0 to 1, scaled by 0.05, or with
// the putter by 0.3 (0.033 in clip group 9). SKATime_Update runs the player forward or backward
// (uFlags bit 6) by the distance.
void SKATime_Idle(Character* pChar, int nPlayer, AnimPlayer* pPlayer, SKABlendNode* pNode, f32 fT) {
    f32 fSmoothRand;
    f32 fDelta;

    pPlayer->fIdleClock += fT;
    fSmoothRand = 1.0f - (3.0f + (Math_Cos(pPlayer->fIdleClock / 5.0f) +
                                  (Math_Cos(5.0f * pPlayer->fIdleClock)
                                   + Math_Cos(7.0f * pPlayer->fIdleClock / 3.0f)))) /
                             6.0f;
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        if (pChar->nGroup == 9) {
            fSmoothRand *= 0.033f;
        } else {
            fSmoothRand *= 0.3f;
        }
    } else {
        fSmoothRand *= 0.05f;
    }
    fDelta = (pPlayer->fIdleCentre - fSmoothRand) - pPlayer->fTime;
    if (fDelta < 0.0f) {
        fDelta = -fDelta;
        pPlayer->uFlags |= 0x40;
    } else {
        pPlayer->uFlags &= ~0x40;
    }
    SKATime_Update(pPlayer, pNode, fDelta);
}

// Pauses the player at pAnim: sets uFlags bit 1 (0x2), which Character_UpdateAnimation turns into
// the hold bit 0 once it has posed the frame. Character.anim is still declared as bytes, so this
// and the next two take the player's address as a u8*.
void SKATime_Pause(u8* pAnim) {
    ((AnimPlayer*)pAnim)->uFlags |= 2;
}

// Lets the player at pAnim run again: clears its pause and hold bits (uFlags bits 1 and 0).
void SKATime_UnPause(u8* pAnim) {
    ((AnimPlayer*)pAnim)->uFlags &= ~3;
}

// Sets the player's time to fTime, which may be a code (SKATime_MapTime: -10000 its end, -20000
// now, -30000 its start).
void SKATime_SetTime(u8* pAnim, f32 fTime) {
    AnimPlayer* pPlayer = (AnimPlayer*)pAnim;

    pPlayer->fTime = SKATime_MapTime(fTime, pPlayer->fTime, pPlayer->fStart, pPlayer->fEnd);
}

// A time that may be a code: -10000 is the end, -20000 now, -30000 the start.
f32 SKATime_MapTime(f32 fTime, f32 fNow, f32 fStart, f32 fEnd) {
    if (-10000.0f == fTime) return fEnd;
    if (-20000.0f == fTime) return fNow;
    if (-30000.0f == fTime) return fStart;
    return fTime;
}

// Cuts the tree at pNode off at fTime (EA's ClampT1). While pPlayer's time is inside the tree (a
// child plays at it), the tree's end, the player's and each child's come down to fTime when later
// (a channel's clip end fTo in proportion), those children are marked to be freed once they have
// ended (bFreeASAP), and so is pNode. Otherwise the player's times go back to 0, the tree is given
// back and pNode set up again as an empty blender of the same format and callback.
void SKABlender_ClampT1(SKABlendNode* pNode, AnimPlayer* pPlayer, f32 fTime) {
    s32 nFormat;
    SKABlendFn pfnBlend;
    SKABlendNode* pChild;

    // fake match: the goto gives EA's layout, the start-over block between the two tests
    if (pPlayer->fTime < pNode->fStart) {
    reset:
        nFormat = pNode->nFormat;
        pfnBlend = pNode->u.blend.pfnBlend;
        pPlayer->fTime = 0.0f;
        pPlayer->fEnd = 0.0f;
        pPlayer->fStart = 0.0f;
        SKABlendData_Shutdown(&pNode, 0);
        SKABlendData_Init(&pNode, 1, nFormat, pfnBlend, 1);
        return;
    }
    if (SKABlender_GetCurrentChannel(pNode, pPlayer->fTime) == -1) {
        goto reset;     // fake match: see above
    }
    if (pNode->fEnd > fTime) {
        pNode->fEnd = fTime;
        pPlayer->fEnd = fTime;
        pChild = pNode->u.blend.apChild[0];
        if (pChild != NULL) {
            if (pChild->fEnd > fTime) {
                if (pChild->nType == 0) {
                    pChild->u.src.fTo = pChild->u.src.fFrom + (fTime - pChild->fStart) *
                        ((pChild->u.src.fTo - pChild->u.src.fFrom) / (pChild->fEnd - pChild->fStart));
                }
                pNode->u.blend.apChild[0]->fEnd = fTime;
            }
            pNode->u.blend.apChild[0]->bFreeASAP = 1;
        }
        pChild = pNode->u.blend.apChild[1];
        if (pChild != NULL) {
            if (pChild->fEnd > fTime) {
                if (pChild->nType == 0) {
                    pChild->u.src.fTo = pChild->u.src.fFrom + (fTime - pChild->fStart) *
                        ((pChild->u.src.fTo - pChild->u.src.fFrom) / (pChild->fEnd - pChild->fStart));
                }
                pNode->u.blend.apChild[1]->fEnd = fTime;
            }
            pNode->u.blend.apChild[1]->bFreeASAP = 1;
        }
    }
    pNode->bFreeASAP = 1;
}

// 1 unless the tree under pNode plays exactly one clip (so also 1 for an empty tree).
u8 SKABlender_IsNotSingleSKA(SKABlendNode* pNode) {
    return SKABlender_NumSKAsInBlender(pNode) != 1;
}

// How many channels (clips) the tree under pNode plays.
int SKABlender_NumSKAsInBlender(SKABlendNode* pNode) {
    int i = 0;
    int nNumBlends = 0;
    SKABlendNode* pChild;

    do {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                nNumBlends += SKABlender_NumSKAsInBlender(pChild);
            } else if (pChild->nType == 0) {
                nNumBlends++;
            }
        }
        i++;
    } while (i < 2);
    return nNumBlends;
}

// Whether a channel under pNode plays the clip pSrc (a format 0 channel: the format tested is its
// parent's). AnimStream.c keeps a clip while the golfer's tree plays it.
u8 SKABlender_HasClip(SKABlendNode* pNode, void* pSrc) {
    int i;
    SKABlendNode* pChild;

    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                if (SKABlender_HasClip(pChild, pSrc) == 1) return 1;
            } else if (pChild->nType == 0 && pNode->nFormat == 0 && pChild->u.src.pSrc == pSrc) {
                return 1;
            }
        }
    }
    return 0;
}

// Whether a channel under pNode plays the morph library pSrc (an MtaLib; a format 1 channel, tested
// by its parent's format). Never for a NULL pSrc.
u8 SKABlender_HasMtaLib(SKABlendNode* pNode, void* pSrc) {
    int i;
    SKABlendNode* pChild;

    if (pSrc == NULL) return 0;
    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        if (pChild != NULL) {
            if (pChild->nType == 1) {
                if (SKABlender_HasMtaLib(pChild, pSrc) == 1) return 1;
            } else if (pChild->nType == 0 && pNode->nFormat == 1 && pChild->u.src.pSrc == pSrc) {
                return 1;
            }
        }
    }
    return 0;
}

// Unmarks morph nMorph in the three blocks of pNode's format 1 pose buffer and of every channel's
// under it (a channel's only when its parent is format 1), so the tree no longer sets that morph's
// weight; CharSliders.c does this for a morph a slider sets.
void SKABlender_ClearMorph(SKABlendNode* pNode, s32 nMorph) {
    SKABlendNode* pChild;
    int i;
    int j;
    int n = nMorph;   // fake match: a copy of the parameter for the pose calls

    for (j = 0; j < 3; j++) {
        BitArray_ClearBit(((SkelPose1*)pNode->pPose)->aBlocks[j].aBits, n);
    }
    for (i = 0; i < 2; i++) {
        pChild = pNode->u.blend.apChild[i];
        // fake match: the null test reads the array again (a test of pChild takes other registers)
        if (pNode->u.blend.apChild[i] != NULL) {
            if (pChild->nType == 1) {
                SKABlender_ClearMorph(pChild, nMorph);
            } else if (pChild->nType == 0 && pNode->nFormat == 1) {
                for (j = 0; j < 3; j++) {
                    BitArray_ClearBit(((SkelPose1*)pChild->pPose)->aBlocks[j].aBits, n);
                }
            }
        }
    }
}

// The player's time step for a frame of fT: fT times its time scale (fTimeScale). While it eases in
// (uFlags bit 3) or out (bit 4), the step is also scaled by fEase / fEaseTime, fEase climbing by
// the step to fEaseTime (then bit 3 clears) or falling by it to fEaseFloor (at 0, bit 4 gives way
// to the hold bit 0).
f32 SKATime_CalcStep(AnimPlayer* pPlayer, f32 fT) {
    f32 fStep = fT * pPlayer->fTimeScale;

    if (pPlayer->uFlags & 8) {
        pPlayer->fEase += fStep;
        if (pPlayer->fEase >= pPlayer->fEaseTime) {
            pPlayer->uFlags &= ~8;
            pPlayer->fEase = 0.0f;
            return fStep;
        }
        return fStep * (pPlayer->fEase / pPlayer->fEaseTime);
    }
    if (pPlayer->uFlags & 0x10) {
        pPlayer->fEase -= fStep;
        if (pPlayer->fEase < pPlayer->fEaseFloor) {
            pPlayer->fEase = pPlayer->fEaseFloor;
        }
        if (pPlayer->fEase <= 0.0f) {
            pPlayer->uFlags &= ~0x10;
            pPlayer->uFlags |= 1;
            pPlayer->fEase = 0.0f;
            return fStep;
        }
        return fStep * (pPlayer->fEase / pPlayer->fEaseTime);
    }
    return fStep;
}

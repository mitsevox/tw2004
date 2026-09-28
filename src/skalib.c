// skalib.c (EA's name, from its asserts; TW07 golf/animation/skalib.c, whose SKALIB_InitModule /
// SKALIB_CloseModule start and stop this module): the golfers' animation library. It loads the
// animation libraries ('SAL ') and clip banks ('BNK ') of the three animation slots (slot 0's sac
// file is malesac, slot 1's femsac), and each golfer's own overlay library (from its 'CHR ' object
// and 'SAC ' file). Before the round and between holes it plans each slot's clip bank within the
// round's memory budget: the slot's library and its overlays are merged (same-named clips shared),
// cut down to fit, and the clips still used copied into the bank, their frame data kept in ARAM
// (AnimLib_PlanBank, AnimLib_MergeOverlay). It applies the created golfers' custom animations,
// picks the clip a golfer plays for an animation group, style, club class and key (AnimLib_Find,
// AnimLib_Pick), parks the bank files in ARAM (the front end brings them back per character), and
// lends bank memory to the memory card code for its save images. The types are in character.h.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "endian.h"
#include "core/goaram.h"
#include "game/save.h"

ClipBank* ClipBank_Get(u32 nSlot);
void  SKA_SwapClip(void* pClip);                        // swaps a clip in place
void  SKA_PatchMemory(struct Clip* pClip, u32 uAram);
void  AnimLib_ApplyCustomAnims(struct LibOverlay* pOv, int nSlot, s32 n);
void  AnimLib_SetLeafClipsByName(LibOverlay* pOv, int nSlot, int nGroup, int nClub, int nStyle, int nKey,
                                 char* pNames, int nNames);
u32   Skalib_NextSlot(void);
void  Skalib_SetBudgets(void);
u32   AnimLib_PlanBank(u32 nSlot);
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank);
u32   fn_8009EF90(void);
int   Skalib_HasOverlays(int nSlot);
void  AnimLib_FreeCopies(void);
void  ClipBank_FreeAram(void);

// This file's .bss (character.h), in reverse address order as the compiler lays it out.
UStreamObject* gClipBankFiles[3];
u32            gClipBankAramSizes[3];
u32            gClipBankAram[3];
LibSlot        gLibSlots[3];
AnimLib*       gBankLibs[3];
ClipBank*      gClipBanks[3];
SlotStats      gClipBankStats[3];
u8             gSkaFrame2Space[0x1DC];
u8             gSkaFrame1Space[0x1DC];
u8             gSkaRangeSpace[0x6290];
u8             gSkaKeySpace[0x6290];

// This file's .sdata (character.h).
s32 gTrimMinLeafClips = 3;
s32 gClipsPerLeaf = 6;
u32 gCurLibSlot = 1;

// This file's .sbss (character.h), in reverse address order as the compiler lays it out.
f32            gSlot0BankShare;
u32            lbl_80281D18;
char (*gLastReactionClips)[2][8][6][16];
u32            gLentBankAramSizes[2];
u32            gLentBankAram[2];
s16*           gBuildClubNode;
s16*           gBuildStyleNode;
s16*           gBuildGroupNode;
s32            gWalkKey;
s32            gWalkClub;
s32            gWalkStyle;
s32            gWalkGroup;
u8             gSacReloading;
UStreamObject* gClipBankRestoreFile;
u32            gClipBankBudget;
u8             gSlotsDoubleBuffered;

// The clip with this name, or NULL.
void* AnimLib_FindByName(AnimLib* pLib, const char* pName) {
    int i;
    for (i = 0; i < pLib->nClips; i++) {
        if (pLib->ppClips[i] != NULL && strcmp(((Clip*)pLib->ppClips[i])->name, pName) == 0) {
            return pLib->ppClips[i];
        }
    }
    return NULL;
}

// Starts the animation library module (Legacy_Character_InitModule): no library or clip bank in any
// of the three slots, every slot's overlay list empty (entries inactive, stream ids -1, no copies),
// lbl_801D9908[0] cleared, lbl_80281D18 cleared in game types 3 and 10, the last-played reaction
// table (four players) allocated and cleared, and ska_shared.c's four ARAM staging buffers (a frame
// of each of a clip's two frame streams, the tracks' ranges and the tracks' keys) pointed into this
// file's .bss, each at the first 32-byte boundary after its array's start.
void SKALIB_InitModule(void) {
    int i;
    s32 j;

    gBankLibs[0] = NULL;
    gClipBanks[0] = NULL;
    gBankLibs[1] = NULL;
    gClipBanks[1] = NULL;
    gBankLibs[2] = NULL;
    gClipBanks[2] = NULL;
    lbl_801D9908[0] = 0;
    for (i = 0; i < 3; i++) {
        gLibSlots[i].n150      = 0;
        gLibSlots[i].nOverlays = 0;
        gLibSlots[i].pLib      = NULL;
        gLibSlots[i].pCopy     = NULL;
        for (j = 0; j < 10; j++) {
            gLibSlots[i].overlays[j].bActive = 0;
            gLibSlots[i].overlays[j].n10     = -1;
            gLibSlots[i].overlays[j].n14     = -1;
            gLibSlots[i].overlays[j].pCopy   = NULL;
            gLibSlots[i].overlays[j].pWork   = NULL;
        }
    }
    if (gSession.nGameType == 3 || gSession.nGameType == 10) {
        lbl_80281D18 = 0;
    }
    // four players
    gLastReactionClips = StaticMem_Alloc(4 * sizeof(*gLastReactionClips), 2, 0, "skalib.c", 508);
    Mem_set(gLastReactionClips, 0, 4 * sizeof(*gLastReactionClips));
    gSKAAram8BitFrame = gSkaFrame2Space;
    gSKAAram8BitFrame = (u8*)((((uptr)gSKAAram8BitFrame >> 5) + 1) << 5);
    gSKAAram16BitFrame = gSkaFrame1Space;
    gSKAAram16BitFrame = (u8*)((((uptr)gSKAAram16BitFrame >> 5) + 1) << 5);
    gSKAAramRanges = gSkaRangeSpace;
    gSKAAramRanges = (u8*)((((uptr)gSKAAramRanges >> 5) + 1) << 5);
    gSKAAramKeys = gSkaKeySpace;
    gSKAAramKeys = (u8*)((((uptr)gSKAAramKeys >> 5) + 1) << 5);
}

void ClipBank_Free(ClipBank* pBank);

// Shuts the animation library module down (Legacy_Character_CloseModule): frees the slots' pristine
// library copies (AnimLib_FreeCopies), the last-played reaction table, each slot's bank library and
// clip bank (forgetting the restore buffer when a bank's file is that buffer, so it is not freed
// twice), then the banks' ARAM copies and the restore buffer (ClipBank_FreeAram).
void SKALIB_CloseModule(void) {
    int i;

    AnimLib_FreeCopies();
    StaticMem_Free(gLastReactionClips);
    gLastReactionClips = NULL;
    for (i = 0; i < 3; i++) {
        if (gBankLibs[i] != NULL) {
            AnimLib_Free(gBankLibs[i]);
            gBankLibs[i] = NULL;
        }
        if (gClipBanks[i] != NULL) {
            if (gClipBanks[i]->pFile == gClipBankRestoreFile) {
                gClipBankRestoreFile = NULL;
            }
            ClipBank_Free(gClipBanks[i]);
            gClipBanks[i] = NULL;
        }
    }
    ClipBank_FreeAram();
}

// Frees a library: just its file when it was loaded from one (plus the clip table it allocated
// for its own clips), otherwise each part it was built from.
void AnimLib_Free(AnimLib* pLib) {
    if (pLib->pFile != NULL) {
        if (pLib->pClipData != NULL) {
            StaticMem_Free(pLib->ppClips);
        }
        StaticMem_Free(pLib->pFile);
    } else {
        if (pLib->ppClips != NULL) {
            StaticMem_Free(pLib->ppClips);
        }
        if (pLib->pIndex != NULL) {
            StaticMem_Free(pLib->pIndex);
        }
        if (pLib->pTree != NULL) {
            StaticMem_Free(pLib->pTree);
        }
        if (pLib->pRecords != NULL) {
            StaticMem_Free(pLib->pRecords);
        }
        StaticMem_Free(pLib);
    }
}

// Frees a bank, and the ARAM of any of its clips that live there.
void ClipBank_Free(ClipBank* pBank) {
    u32   i;
    Clip* pClip;

    for (i = 0; i < pBank->nClips; i++) {
        pClip = pBank->ppClips[i];
        if (pClip != NULL && (pClip->uFlags & 4)) {
            GoARAM_Free(pClip->uAram);
        }
    }
    if (pBank->pFile != NULL) {
        StaticMem_Free(pBank->pFile);
    } else {
        StaticMem_Free(pBank);
    }
}

// A slot's clip bank (NULL past the three slots).
ClipBank* ClipBank_Get(u32 nSlot) {
    if (nSlot >= 3) return NULL;
    return gClipBanks[nSlot];
}

// The node at offset nOff of a library's tree, or NULL when there is no library or no node.
#define SKA_NODE(pLib, nOff) \
    (((pLib) != NULL && (nOff) >= 0) ? (s16*)((pLib)->pTree + (nOff)) : NULL)
// The child nIdx of a node, or NULL.
// fake match: an inline, not a macro, so each result is worked out before the call's arguments
// are loaded (as a macro, pA and pB go into r3/r4 before the last child is found).
static inline s16* fn_80021F50_Read(AnimLib* pLib, s16* pNode, int nIdx) {
    return (pNode != NULL && pNode[nIdx] >= 0) ? (s16*)(pLib->pTree + pNode[nIdx]) : NULL;
}

// fake match: SKA_NODE(pLib, pLib->nDefault) as an inline, for the same reason.
static inline s16* fn_80021F50_Get(AnimLib* pLib) {
    return (pLib != NULL && pLib->nDefault >= 0) ? (s16*)(pLib->pTree + pLib->nDefault) : NULL;
}

// Walks the clip trees of two libraries side by side (either may be NULL) and calls pfn at every
// position either one has: level 0 with the two libraries' default leaves, 1 each group (its
// default leaves), 2 each style (no leaves), 3 each club (its default leaves), 4 each key (its
// leaves); nIndex is the group, style, club or key. The position is also kept in gWalkGroup
// (group), gWalkStyle (style), gWalkClub (club) and gWalkKey (key), -1 for levels above
// it, for the callbacks. Stops at the first callback result above 0 and returns it; 0 when the walk
// ends.
int AnimLib_WalkPair(AnimLib* pA, AnimLib* pB, AnimLibWalkFn pfn, void* pCtx) {
    int  nRet;
    int  nGroup;
    int  nStyle;
    int  nClub;
    int  nKey;
    s16* pGroupA;
    s16* pGroupB;
    s16* pStyleA;
    s16* pStyleB;
    s16* pClubA;
    s16* pClubB;
    s16* pLeafA;
    s16* pLeafB;

    gWalkGroup = -1;
    gWalkStyle = -1;
    gWalkClub = -1;
    gWalkKey = -1;
    pLeafB = fn_80021F50_Get(pB);
    pLeafA = fn_80021F50_Get(pA);
    nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 0, 0);
    if (nRet > 0) return nRet;
    for (nGroup = 0; nGroup < 21; nGroup++) {
        gWalkGroup = nGroup;
        gWalkStyle = -1;
        gWalkClub = -1;
        gWalkKey = -1;
        pGroupA = SKA_NODE(pA, pA->groups[nGroup]);
        pGroupB = SKA_NODE(pB, pB->groups[nGroup]);
        if (pGroupA == NULL && pGroupB == NULL) continue;
        pLeafA = fn_80021F50_Read(pA, pGroupA, 0);
        pLeafB = fn_80021F50_Read(pB, pGroupB, 0);
        nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 1, nGroup);
        if (nRet > 0) return nRet;
        for (nStyle = 0; nStyle < 8; nStyle++) {
            gWalkStyle = nStyle;
            gWalkClub = -1;
            pStyleA = fn_80021F50_Read(pA, pGroupA, 1 + nStyle);
            pStyleB = fn_80021F50_Read(pB, pGroupB, 1 + nStyle);
            if (pStyleA == NULL && pStyleB == NULL) continue;
            nRet = pfn(pA, pB, NULL, NULL, pCtx, 2, nStyle);
            if (nRet > 0) return nRet;
            for (nClub = 0; nClub < 6; nClub++) {
                gWalkClub = nClub;
                gWalkKey = -1;
                pClubA = fn_80021F50_Read(pA, pStyleA, nClub);
                pClubB = fn_80021F50_Read(pB, pStyleB, nClub);
                if (pClubA == NULL && pClubB == NULL) continue;
                pLeafA = fn_80021F50_Read(pA, pClubA, 1);
                pLeafB = fn_80021F50_Read(pB, pClubB, 1);
                nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 3, nClub);
                if (nRet > 0) return nRet;
                for (nKey = 0; nKey < 11; nKey++) {
                    gWalkKey = nKey;
                    pLeafA = fn_80021F50_Read(pA, pClubA, 2 + nKey);
                    pLeafB = fn_80021F50_Read(pB, pClubB, 2 + nKey);
                    nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 4, nKey);
                    if (nRet > 0) return nRet;
                }
            }
        }
    }
    return 0;
}

// Merge walk, first pass (AnimLib_PlanBank: pLibA the slot's library, pLib an overlay, no context):
// adds to the overlay's nTreeSize the bytes of the merged tree at this position (8 for a leaf when
// either side has one, plus the level's node: 0x14 a group, 0xC a style, 0x20 a club) and marks the
// leaves (AnimLeaf.uMask; 2 its clips go unused, 1 they are kept). Where both have a leaf: at a
// streamed position (AnimStream_IsStreamed) both are marked 2; in group 20 the overlay's is; otherwise, when
// the overlay's leaf is not flagged 1, the library's clips come off the overlay's nClips, and the
// library's leaf is marked 2 (replaced) unless either leaf is flagged 1, then 1 (both kept). A
// library leaf alone is marked 1, or 2 at a streamed position. Always 0.
int AnimLib_MergeSizeCb(AnimLib* pLibA, AnimLib* pLib, AnimLeaf* pLeaf, AnimLeaf* pOver, MergeCtx* pCtx,
                        int nLevel, int nIndex) {
    int bAny = 0;

    if (pLeaf != NULL || pOver != NULL) {
        bAny = 1;
    }
    if (bAny) {
        pLib->nTreeSize += 8;
    }
    switch (nLevel) {
    case 0:
        break;
    case 1:
        pLib->nTreeSize += 0x14;
        break;
    case 2:
        pLib->nTreeSize += 0xC;
        break;
    case 3:
        pLib->nTreeSize += 0x20;
        break;
    case 4:
        break;
    }
    if (pLeaf != NULL) {
        if (pOver != NULL) {
            if (AnimStream_IsStreamed(gWalkGroup, gWalkStyle, gWalkClub, gWalkKey)) {
                pOver->uMask |= 2;
                pLeaf->uMask |= 2;
            } else if (gWalkGroup == 20) {
                pOver->uMask |= 2;
            } else {
                if (!(pOver->uMask & 1)) {
                    pLib->nClips -= pLeaf->nCount;
                }
                if (!(pOver->uMask & 1) && !(pLeaf->uMask & 1)) {
                    pLeaf->uMask |= 2;
                } else {
                    pLeaf->uMask &= ~2;
                    pLeaf->uMask |= 1;
                }
            }
        } else if (AnimStream_IsStreamed(gWalkGroup, gWalkStyle, gWalkClub, gWalkKey)) {
            pLeaf->uMask |= 2;
        } else {
            pLeaf->uMask &= ~2;
            pLeaf->uMask |= 1;
        }
    }
    return 0;
}

// Merge walk, release pass (AnimLib_PlanBank): each clip of a leaf marked 2 (unused by the merged
// library: replaced, streamed, or an overlay's group 20) loses a user (ClipRecord.n10); a clip no
// leaf uses any more comes off the context's clip count and byte total (its n14 bytes). Always 0.
int AnimLib_MergeReleaseCb(AnimLib* pLibA, AnimLib* pLibB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx,
                           int nLevel, int nIndex) {
    ClipRecord* pRec;
    int         i;
    s16*        pIdx;

    if (pLeafA != NULL && (pLeafA->uMask & 2) && pLeafA->nCount != 0) {
        pIdx = pLibA->pIndex + pLeafA->nFirst;
        for (i = 0; i < pLeafA->nCount; pIdx++, i++) {
            pRec = &pLibA->pRecords[*pIdx];
            pRec->n10--;
            if (pRec->n10 == 0) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
    }
    if (pLeafB != NULL && (pLeafB->uMask & 2) && pLeafB->nCount != 0) {
        pIdx = pLibB->pIndex + pLeafB->nFirst;
        for (i = 0; i < pLeafB->nCount; pIdx++, i++) {
            pRec = &pLibB->pRecords[*pIdx];
            pRec->n10--;
            if (pRec->n10 == 0) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
    }
    return 0;
}

// Whether slots 0 and 1 are double buffered (Skalib_SetBudgets: both have overlays and there is
// more than one player): their libraries are then reloaded in turn and the bank memory is split
// between them.
u8 Skalib_IsDoubleBuffered(void) {
    return gSlotsDoubleBuffered;
}

// Picks the slot whose libraries the next hole reloads (AnimLib_ReloadSlot), makes it the current
// slot (Skalib_CurSlot) and returns it: double buffered, it alternates between 0 and 1; otherwise
// it is slot 0 when that has overlays, else slot 1.
u32 Skalib_NextSlot(void) {
    if (Skalib_IsDoubleBuffered()) {
        gCurLibSlot = (gCurLibSlot == 0);
    } else if (Skalib_HasOverlays(0)) {
        gCurLibSlot = 0;
    } else {
        gCurLibSlot = 1;
    }
    return gCurLibSlot;
}

// The slot Skalib_NextSlot last picked (Skalib_SetBudgets starts it at random); streammanagerhole.c
// streams that slot's sac file.
u32 Skalib_CurSlot(void) {
    return gCurLibSlot;
}

// Sets the round's clip bank limits: the clips a leaf may keep (all of them with one player, 10
// with more) and each slot's bank budget (0xE6000 bytes, 920 KB). When slots 0 and 1 both have
// overlays and there is more than one player they are double buffered, and the 920 KB is split
// between them by the clip bytes (AnimLib.n140) of each slot's library and overlays: slot 0's share
// goes into gSlot0BankShare, kept to 44..56%. The current slot starts at random.
void Skalib_SetBudgets(void) {
    s32      aKeepSingle[4] = {100000, 10, 10, 10};
    s32      aKeepDouble[4] = {100000, 10, 10, 10};
    s32      aBytes[4];
    u32      nSize0;
    LibSlot* pSlot1;
    int      i;
    int      n;
    LibSlot* pSlot0;
    u32      nSize1;

    aBytes[0] = 0xE6000;
    aBytes[1] = 0xE6000;
    aBytes[2] = 0xE6000;
    aBytes[3] = 0xE6000;
    gSlotsDoubleBuffered = 0;
    if (!Skalib_HasOverlays(0) || !Skalib_HasOverlays(1)) {
        gClipsPerLeaf = aKeepSingle[gSession.nNumPlayers - 1];
        gClipBankBudget = aBytes[0];
    } else {
        n = gSession.nNumPlayers;
        if (n > 1) {
            gSlotsDoubleBuffered = 1;
        }
        gClipsPerLeaf = aKeepDouble[n - 1];
        gClipBankBudget = aBytes[n - 1];
    }
    gCurLibSlot = (Misc_RandFunc(1) & 1) ^ 1;
    if (gSlotsDoubleBuffered) {
        pSlot0 = &gLibSlots[0];
        pSlot1 = &gLibSlots[1];
        nSize0 = pSlot0->pLib->n140;
        nSize1 = pSlot1->pLib->n140;
        for (i = 0; pSlot0->nOverlays > i; i++) {
            nSize0 += pSlot0->overlays[i].pWork->n140;
        }
        for (i = 0; pSlot1->nOverlays > i; i++) {
            nSize1 += pSlot1->overlays[i].pWork->n140;
        }
        gSlot0BankShare = (f32)nSize0 / (f32)(nSize0 + nSize1);
        gSlot0BankShare = (gSlot0BankShare
                           < 0.44f) ? 0.44f : ((gSlot0BankShare > 0.56f) ? 0.56f : gSlot0BankShare);
    }
}

f32 Skalib_Random(void);

// Merge walk, trim pass (AnimLib_PlanBank, before the clips are matched): cuts each leaf down to
// the round's per-leaf limit (gClipsPerLeaf), keeping a run of that many clips from a random start
// (Skalib_Random); every clip cut loses a user (ClipRecord.n10), and with a context one nobody uses
// any more comes off its clip count and byte total. A streamed position (AnimStream_IsStreamed) keeps
// everything; when pA's leaf is marked 2 (unused) none are kept. Clips cut from pB's leaf also come
// off pB's nClips. Always 0.
int AnimLib_TrimCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx, int nLevel,
                   int nIndex) {
    s32         nKeep;
    s32         nCountA;
    s32         nCountB;
    int         nStart;
    int         i;
    ClipRecord* pRec;

    if (AnimStream_IsStreamed(gWalkGroup, gWalkStyle, gWalkClub, gWalkKey)) {
        nKeep = 10000;
    } else if (pLeafA != NULL && (pLeafA->uMask & 2)) {
        nKeep = 0;
    } else {
        nKeep = gClipsPerLeaf;
    }
    if (pLeafA != NULL && (nCountA = pLeafA->nCount) > nKeep) {
        nStart = (nCountA - nKeep) * Skalib_Random();
        for (i = 0; i < nStart; i++) {
            pRec = &pA->pRecords[pA->pIndex[pLeafA->nFirst + i]];
            pRec->n10--;
            if (pRec->n10 == 0 && pCtx != NULL) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
        for (i = nStart + nKeep; i < pLeafA->nCount; i++) {
            pRec = &pA->pRecords[pA->pIndex[pLeafA->nFirst + i]];
            pRec->n10--;
            if (pRec->n10 == 0 && pCtx != NULL) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
        pLeafA->nFirst += (s16)nStart;
        pLeafA->nCount = nKeep;
    }
    if (pLeafB != NULL && (nCountB = pLeafB->nCount) > nKeep) {
        nStart = (nCountB - nKeep) * Skalib_Random();
        for (i = 0; i < nStart; i++) {
            pRec = &pB->pRecords[pB->pIndex[pLeafB->nFirst + i]];
            pRec->n10--;
            pB->nClips--;
            if (pRec->n10 == 0 && pCtx != NULL) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
        for (i = nStart + nKeep; i < pLeafB->nCount; i++) {
            pRec = &pB->pRecords[pB->pIndex[pLeafB->nFirst + i]];
            pRec->n10--;
            pB->nClips--;
            if (pRec->n10 == 0 && pCtx != NULL) {
                (*pCtx->pCount)--;
                pCtx->nBytes -= pRec->n14;
            }
        }
        pLeafB->nFirst += (s16)nStart;
        pLeafB->nCount = nKeep;
    }
    return 0;
}

// A random fraction in [0, 1) (Misc_RandFuncf, stream 1): where AnimLib_TrimCb starts a leaf's kept
// run.
f32 Skalib_Random(void) {
    return Misc_RandFuncf(1);
}

// A clip that can still be flagged for dropping: not merged into another record (2, 0x10), not
// flagged already (1), and used by between 1 and nMaxUsers leaves.
#define SKA_MARKABLE(pRec, pCtx) \
    (!((pRec)->n12 & 2) && !((pRec)->n12 & 0x10) && !((pRec)->n12 & 1) && (pRec)->n10 > 0 && \
     (pRec)->n10 <= (pCtx)->nMaxUsers)

// Trim walk, mark pass (AnimLib_TrimToFit, first try): for the one leaf given (pA's, else pB's)
// when it is in use (not marked 2) and holds more than pCtx->nKeep clips, flags that many surplus
// clips (ClipRecord.n12 1) for AnimLib_DropCb, each picked at random: from a random start the first
// markable clip forward, else backward (markable: not merged into another record (2, 0x10), not
// flagged yet, used by 1 to nMaxUsers leaves). During a lesson (fn_80100294, game mode 11) a lesson
// animation (fn_80101E34) is never flagged but still counts as a pick. The picks it could not make
// are tried again through merged records, following each to the record it points to. Always 0.
int AnimLib_MarkDropRandomCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx,
                         int nLevel, int nIndex) {
    AnimLeaf*   pLeaf;
    AnimLib*    pLib;
    s16*        pIdx;
    ClipRecord* pRec;
    int         nStart;
    int         i;
    int         nMarked;
    int         nExtra;
    int         j;

    if (pLeafA != NULL) {
        pLeaf = pLeafA;
        pLib  = pA;
    } else {
        pLeaf = pLeafB;
        pLib  = pB;
    }
    if (pLeaf != NULL && pLib != NULL) {
        if (pLeaf->uMask & 2) return 0;
        if (pLeaf->nCount > pCtx->nKeep) {
            nExtra  = pLeaf->nCount - pCtx->nKeep;
            nMarked = 0;
            pIdx    = pLib->pIndex + pLeaf->nFirst;
            for (i = 0; i < nExtra; i++) {
                nStart = Misc_RandFunc(1) % pLeaf->nCount;
                for (j = nStart; j < pLeaf->nCount; j++) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    if (SKA_MARKABLE(pRec, pCtx)) {
                        // fake match: skips the other search (breaks and a test: 169 differ, not 0)
                        goto found;
                    }
                }
                for (j = nStart; j >= 0; j--) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    if (SKA_MARKABLE(pRec, pCtx)) {
                        // fake match: skips the other search (breaks and a test: 169 differ, not 0)
                        goto found;
                    }
                }
                continue;
            found:
                if (fn_80100294()) {
                    if (!fn_80101E34(pRec->name)) {
                        pRec->n12 |= 1;
                    }
                    nMarked++;
                } else {
                    pRec->n12 |= 1;
                    nMarked++;
                }
            }
            for (i = nMarked; i < nExtra; i++) {
                nStart = Misc_RandFunc(1) % pLeaf->nCount;
                for (j = nStart; j < pLeaf->nCount; j++) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    while ((pRec->n12 & 2) || (pRec->n12 & 0x10)) {
                        pRec = (ClipRecord*)pRec->pClip;
                    }
                    if (!(pRec->n12 & 1) && pRec->n10 > 0 && pRec->n10 <= pCtx->nMaxUsers) {
                        // fake match: skips the other search (breaks and a test: 169 differ, not 0)
                        goto found2;
                    }
                }
                for (j = nStart; j >= 0; j--) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    while ((pRec->n12 & 2) || (pRec->n12 & 0x10)) {
                        pRec = (ClipRecord*)pRec->pClip;
                    }
                    if (!(pRec->n12 & 1) && pRec->n10 > 0 && pRec->n10 <= pCtx->nMaxUsers) {
                        // fake match: skips the other search (breaks and a test: 169 differ, not 0)
                        goto found2;
                    }
                }
                continue;
            found2:
                if (fn_80100294()) {
                    if (!fn_80101E34(pRec->name)) {
                        pRec->n12 |= 1;
                    }
                } else {
                    pRec->n12 |= 1;
                }
            }
        }
    }
    return 0;
}

// Trim walk, mark pass (AnimLib_TrimToFit, retry): flags surplus clips for AnimLib_DropCb as
// AnimLib_MarkDropRandomCb does, but each pick is the markable clip of the leaf with the highest
// ClipRecord.n18 instead of a random one. Always 0.
int AnimLib_MarkDropHighestCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx,
                       int nLevel, int nIndex) {
    int         i;
    int         nMarked;
    AnimLeaf*   pLeaf;
    AnimLib*    pLib;
    s16*        pIdx;
    ClipRecord* pBest;
    int         nExtra;
    int         j;
    ClipRecord* pRec;

    if (pLeafA != NULL) {
        pLeaf = pLeafA;
        pLib  = pA;
    } else {
        pLeaf = pLeafB;
        pLib  = pB;
    }
    if (pLeaf != NULL && pLib != NULL) {
        if (pLeaf->uMask & 2) return 0;
        if (pLeaf->nCount > pCtx->nKeep) {
            nExtra  = pLeaf->nCount - pCtx->nKeep;
            nMarked = 0;
            pIdx    = pLib->pIndex + pLeaf->nFirst;
            for (i = 0; i < nExtra; i++) {
                pBest = NULL;
                for (j = 0; j < pLeaf->nCount; j++) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    if (SKA_MARKABLE(pRec, pCtx)) {
                        if (pBest == NULL || pRec->n18 > pBest->n18) {
                            pBest = pRec;
                        }
                    }
                }
                if (pBest != NULL) {
                    if (fn_80100294()) {
                        if (!fn_80101E34(pBest->name)) {
                            pBest->n12 |= 1;
                        }
                        nMarked++;
                    } else {
                        pBest->n12 |= 1;
                        nMarked++;
                    }
                }
            }
            for (i = nMarked; i < nExtra; i++) {
                pBest = NULL;
                for (j = 0; j < pLeaf->nCount; j++) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    while ((pRec->n12 & 2) || (pRec->n12 & 0x10)) {
                        pRec = (ClipRecord*)pRec->pClip;
                    }
                    if (!(pRec->n12 & 1) && pRec->n10 > 0 && pRec->n10 <= pCtx->nMaxUsers) {
                        if (pBest == NULL || pRec->n18 > pBest->n18) {
                            pBest = pRec;
                        }
                    }
                }
                if (pBest != NULL) {
                    if (fn_80100294()) {
                        if (!fn_80101E34(pBest->name)) {
                            pBest->n12 |= 1;
                        }
                    } else {
                        pBest->n12 |= 1;
                    }
                }
            }
        }
    }
    return 0;
}

// Merge walk: the largest clip count of any leaf still in play goes into gClipsPerLeaf.
int AnimLib_MaxCountCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx,
                       int nLevel, int nIndex) {
    AnimLeaf* pLeaf;
    AnimLib*  pLib;

    if (pLeafA != NULL) {
        pLeaf = pLeafA;
        pLib  = pA;
    } else {
        pLeaf = pLeafB;
        pLib  = pB;
    }
    if (pLeaf != NULL && pLib != NULL) {
        if (pLeaf->uMask & 2) return 0;
        if (pLeaf->nCount > gClipsPerLeaf) {
            gClipsPerLeaf = pLeaf->nCount;
        }
    }
    return 0;
}

// Trim walk, drop pass (AnimLib_TrimToFit): takes the flagged clips (ClipRecord.n12 1) out of the
// leaf (pA's, else pB's, when in use), while the leaf is longer than gTrimMinLeafClips; for pB's leaf
// merged records are followed to the record they point to. Each clip taken out loses a user; one
// nobody uses any more is dropped (pClip NULL, users -1) and comes off the context's byte total and
// clip count. Returns 1 (stopping the walk) once the bytes fall under pCtx->nTarget, else 0.
int AnimLib_DropCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx, int nLevel,
                   int nIndex) {
    int         bDone = 0;
    AnimLeaf*   pLeaf;
    AnimLib*    pLib;
    s16*        p;
    int         i;
    s16*        pIdx;
    ClipRecord* pRec;
    int         j;

    if (pLeafA != NULL) {
        pLeaf = pLeafA;
        pLib  = pA;
    } else {
        pLeaf = pLeafB;
        pLib  = pB;
    }
    if (pLeaf != NULL && pLib != NULL) {
        if (pLeaf->uMask & 2) return 0;
        pIdx = pLib->pIndex + pLeaf->nFirst;
        for (i = 0; i < pLeaf->nCount; i++) {
            if (pLeaf->nCount <= gTrimMinLeafClips) return 0;
            p    = &pIdx[i];
            pRec = &pLib->pRecords[*p];
            if (pLeaf == pLeafB) {
                while ((pRec->n12 & 2) || (pRec->n12 & 0x10)) {
                    pRec = (ClipRecord*)pRec->pClip;
                }
            }
            if (pRec->n12 & 1) {
                pRec->n10--;
                if (pRec->n10 == 0) {
                    pRec->pClip = NULL;
                    pRec->n10--;
                    pCtx->nBytes -= pRec->n14;
                    (*pCtx->pCount)--;
                    if (pCtx->nBytes < pCtx->nTarget) {
                        bDone = 1;
                    }
                }
                for (j = i; j < pLeaf->nCount - 1; j++, p++) {
                    *p = p[1];
                }
                pLeaf->nCount = pLeaf->nCount - 1;
                i--;
                if (bDone) return bDone;
            }
        }
    }
    return 0;
}


void* AnimLib_ResolveRecord(AnimLib* pLib, int nRec, ClipRecord* pOut, u8 bLink);

// Space for a node or leaf at the end of the tree being built.
#define SKA_ALLOC(pLib, p, nSize)                  \
    (p) = (void*)((pLib)->pTree + (pLib)->nTreeSize); \
    (pLib)->nTreeSize += (nSize)

// Merge walk, build pass (AnimLib_MergeOverlay: pA the slot's library, pB the golfer's overlay):
// writes the merged clip tree into pCtx->pLib (a node for each group, style and club, kept in
// gBuildGroupNode, gBuildStyleNode and gBuildClubNode while their children are written, and a leaf wherever
// either side has one) and fills each leaf's clips, each copied with its record into pCtx->pRecords
// (AnimLib_ResolveRecord): from the one side that has a leaf, from the overlay alone when its leaf
// replaces the library's (as AnimLib_MergeSizeCb marked them), or from both, the library's first.
// Group 20 takes only the library's. Copies taken from the library alone are flagged 2, as are the
// library's in a combined leaf at a streamed position. Always 0.
int AnimLib_BuildCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, BuildCtx* pCtx, int nLevel,
                    int nIndex) {
    AnimLib*    pLib    = pCtx->pLib;
    ClipRecord* pRecs   = pCtx->pRecords;
    AnimLeaf*   pSrc    = NULL;
    AnimLib*    pSrcLib = NULL;
    AnimLeaf*   pNew    = NULL;
    u8          bKeep;
    int         bAny    = 0;
    s32         i;
    s16*        pIdx;
    u8          bFromA;

    if (pLeafA != NULL || pLeafB != NULL) {
        bAny = 1;
    }
    bAny   = (bAny != 0);
    bFromA = 0;
    switch (nLevel) {
    case 0:
        for (i = 0; i < 21; i++) {
            pLib->groups[i] = -1;
        }
        if (bAny) {
            pLib->nDefault = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            pLib->nDefault = -1;
        }
        break;
    case 1:
        pLib->groups[nIndex] = (s16)pLib->nTreeSize;
        SKA_ALLOC(pLib, gBuildGroupNode, 0x14);
        if (bAny) {
            gBuildGroupNode[0] = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            gBuildGroupNode[0] = -1;
        }
        for (i = 0; i < 8; i++) {
            gBuildGroupNode[1 + i] = -1;
        }
        break;
    case 2:
        gBuildGroupNode[1 + nIndex] = pLib->nTreeSize;
        SKA_ALLOC(pLib, gBuildStyleNode, 0xC);
        for (i = 0; i < 6; i++) {
            gBuildStyleNode[i] = -1;
        }
        break;
    case 3:
        gBuildStyleNode[nIndex] = pLib->nTreeSize;
        SKA_ALLOC(pLib, gBuildClubNode, 0x20);
        if (bAny) {
            gBuildClubNode[1] = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            gBuildClubNode[1] = -1;
        }
        for (i = 0; i < 11; i++) {
            gBuildClubNode[2 + i] = -1;
        }
        break;
    case 4:
        if (bAny) {
            gBuildClubNode[2 + nIndex] = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            gBuildClubNode[2 + nIndex] = -1;
        }
        break;
    }
    if (pNew == NULL) return 0;
    bKeep = AnimStream_IsStreamed(gWalkGroup, gWalkStyle, gWalkClub, gWalkKey);
    if (gWalkGroup == 20) {
        pLeafB = NULL;
    }
    if (pLeafA != NULL && pLeafB == NULL) {
        pSrc    = pLeafA;
        pSrcLib = pA;
        bFromA  = 1;
    } else if (pLeafA == NULL && pLeafB != NULL) {
    useB:
        pSrc    = pLeafB;
        pSrcLib = pB;
    } else if (pLeafA != NULL && pLeafB != NULL) {
        if ((pLeafA->uMask & 2) || !(pLeafB->uMask & 1)) {
            if (!bKeep || !(pLeafB->uMask & 1)) {
                // fake match: a jump into the other branch (without: 97.4%, not 99.8%)
                goto useB;
            }
        }
    } else {
        return 0;
    }
    if (pSrc != NULL) {
        pNew->nCount = pSrc->nCount;
        pNew->nFirst = pLib->nClips;
        pNew->uMask  = 0;
        pIdx         = pSrcLib->pIndex + pSrc->nFirst;
        for (i = 0; i < pNew->nCount; i++) {
            pLib->ppClips[pLib->nClips] = AnimLib_ResolveRecord(pSrcLib, *pIdx, &pRecs[pLib->nClips], bKeep);
            pIdx++;
            if (bFromA) {
                pRecs[pLib->nClips].n12 |= 2;
            }
            pLib->nClips++;
        }
    } else {
        pNew->nCount = pLeafA->nCount + pLeafB->nCount;
        pNew->nFirst = pLib->nClips;
        pNew->uMask  = 0;
        pIdx         = pA->pIndex + pLeafA->nFirst;
        for (i = 0; i < pLeafA->nCount; i++) {
            pLib->ppClips[pLib->nClips] = AnimLib_ResolveRecord(pA, *pIdx, &pRecs[pLib->nClips], bKeep);
            pIdx++;
            if (bFromA || bKeep) {
                pRecs[pLib->nClips].n12 |= 2;
            }
            pLib->nClips++;
        }
        pIdx = pB->pIndex + pLeafB->nFirst;
        for (i = 0; i < pLeafB->nCount; i++) {
            pLib->ppClips[pLib->nClips] = AnimLib_ResolveRecord(pB, *pIdx, &pRecs[pLib->nClips], bKeep);
            pIdx++;
            if (bFromA) {
                pRecs[pLib->nClips].n12 |= 2;
            }
            pLib->nClips++;
        }
    }
    return 0;
}

// Copies record nRec of a library into pOut, following it to where it was moved; flags the copy
// 2 when it was moved, 4 (keeping the original's n20 unless it moved) when the original was
// linked (4) and bLink is set. Returns the clip, or NULL for a linked record.
void* AnimLib_ResolveRecord(AnimLib* pLib, int nRec, ClipRecord* pOut, u8 bLink) {
    ClipRecord* pRec    = &pLib->pRecords[nRec];
    u8          bMoved  = 0;
    u8          bLinked = 0;
    s32         n20;
    s32         bMove;

    if ((pRec->n12 & 4) && bLink) {
        bLinked = 1;
    }
    n20 = pRec->n20;
    while ((bMove = pRec->n12 & 2) || (pRec->n12 & 0x10)) {
        if (bMove) {
            bMoved = 1;
        }
        pRec = (ClipRecord*)pRec->pClip;
    }
    Mem_cpy(pOut, pRec, sizeof(ClipRecord));
    if (bMoved) {
        pOut->n12 |= 2;
    }
    if (bLinked) {
        pOut->n12 |= 4;
    }
    if (bLinked && !bMoved) {
        pOut->n20 = n20;
    }
    if (bLinked) return NULL;
    return pRec->pClip;
}

// Whether a slot has overlay libraries.
int Skalib_HasOverlays(int nSlot) {
    return gLibSlots[nSlot].nOverlays > 0;
}

// Cuts a library and its overlays down until their clips fit pCtx->nTarget: rounds of lowering
// the per-leaf limit, marking the surplus clips to drop (highest n18 first, or at random), and
// dropping the marked ones from leaves longer than gTrimMinLeafClips (3 down to 1). First the overlays'
// clips are marked, then the library's. TRUE when the target was reached.
u8 AnimLib_TrimToFit(MergeCtx* pCtx, AnimLib* pLib, LibOverlay* pOvs, int nOvs, u8 bBest) {
    int      nRet = 0;
    int      i;
    AnimLib* pOvLib;

    pCtx->nMaxUsers = 100000;
    gClipsPerLeaf    = 0;
    AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_MaxCountCb, pCtx);
    for (i = 0; i < nOvs; i++) {
        AnimLib_WalkPair(NULL, pOvs[i].pWork, (AnimLibWalkFn)AnimLib_MaxCountCb, pCtx);
    }
    pCtx->nKeep = gClipsPerLeaf;
    for (gTrimMinLeafClips = 3; gTrimMinLeafClips >= 1; gTrimMinLeafClips--) {
        while (pCtx->nKeep > gTrimMinLeafClips) {
            pCtx->nKeep--;
            for (i = 0; i < nOvs; i++) {
                pOvLib = pOvs[i].pWork;
                if (bBest) {
                    AnimLib_WalkPair(NULL, pOvLib, (AnimLibWalkFn)AnimLib_MarkDropHighestCb, pCtx);
                } else {
                    AnimLib_WalkPair(NULL, pOvLib, (AnimLibWalkFn)AnimLib_MarkDropRandomCb, pCtx);
                }
            }
            nRet = AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_DropCb, pCtx);
            if (nRet == 0) {
                for (i = 0; i < nOvs; i++) {
                    nRet = AnimLib_WalkPair(NULL, pOvs[i].pWork, (AnimLibWalkFn)AnimLib_DropCb, pCtx);
                    if (nRet != 0) break;
                }
            }
            if (nRet != 0) {
                // fake match: the shared exit as a jump (without: 90.9%, not 100%)
                goto done;
            }
        }
    }
    pCtx->nKeep = gClipsPerLeaf;
    for (gTrimMinLeafClips = 3; gTrimMinLeafClips >= 1; gTrimMinLeafClips--) {
        while (pCtx->nKeep > gTrimMinLeafClips) {
            pCtx->nKeep--;
            if (bBest) {
                AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_MarkDropHighestCb, pCtx);
            } else {
                AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_MarkDropRandomCb, pCtx);
            }
            nRet = AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_DropCb, pCtx);
            if (nRet != 0) {
                // fake match: the shared exit as a jump (without: 90.9%, not 100%)
                goto done;
            }
        }
    }
done:
    return nRet == 1;
}

// fake match: puts 1.0f in the constant pool ahead of AnimLib_PlanBank's 942080.0f (EA's
// order). Unused, so the linker strips it.
static f32 skalib_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Plans the clip bank of slot nSlot from its library and overlays (nothing, 0, for a slot without
// overlays). Every leaf is trimmed to the per-leaf limit (AnimLib_TrimCb; not in slot 2), the
// leaves the merge will not use are released (AnimLib_MergeSizeCb, AnimLib_MergeReleaseCb), and
// each overlay clip is matched by name against the library's and earlier overlays' clips: a match
// shares the first one's record (flag 2 when it is the library's, 0x10 an overlay's). Outside slot
// 2, when the result is over the slot's budget (gClipBankBudget, or its share of 920 KB when double
// buffered), AnimLib_TrimToFit cuts it down with random picks; if that cannot fit, it starts again
// from saved copies with the best-ranked picks, and if even that cannot fit the budget becomes what
// is left (and slot 0's share is recomputed when both slots have overlays). What was spent goes
// into gClipBankStats. Allocates the bank (slots other than 2 keep one they already have), sets up
// its clip table and record area, and returns its size.
u32 AnimLib_PlanBank(u32 nSlot) {
    // register note: the declaration order gives EA's spill slots (0xC8 pIndexCopy up to 0xEC
    // nHdr, in reverse declaration order) and EA's register colouring order.
    s32         nHdr;
    s32         nIndexSize;
    LibSlot*    pSlot = &gLibSlots[nSlot];
    u32         nRet  = 0;
    u8          bBoth = 0;
    s32         nRecSize;
    ClipRecord* apRecords[10];
    u8*         apTree[10];
    s16*        apIndex[10];
    MergeCtx    ctx;
    MergeCtx    ctxOv;
    s32         nClips;
    s32         nLibClips;
    s32         nOvClips;
    s32         nClipsAll;
    s32         nBytesBefore2;
    s32         nBytesBefore;
    ClipRecord* pRecordsCopy;
    u8*         pTreeCopy;
    s16*        pIndexCopy;
    AnimLib*    pLib;
    s32         nBytes;
    LibOverlay* pOvs;
    int         nOvs;
    AnimLib*    pOvLib;
    ClipRecord* pRec;
    int         j;
    s32         nLeft;
    int         i;
    AnimLib*    pOther;
    int         k;
    ClipRecord* pRecO;
    int         m;
    AnimLib*    pWork;
    s32         nTotal;
    s32         nBudget;
    ClipBank*   pBank;
    int         n;

    if (gLibSlots[0].nOverlays != 0 && gLibSlots[1].nOverlays != 0) {
        bBoth = 1;
    }
    if (pSlot->nOverlays == 0) {
        // fake match: the shared exit as a jump (without: 88.7%, not 91.6%)
        goto done;
    }
    nOvs   = pSlot->nOverlays;
    pLib   = pSlot->pLib;
    nClips = 0;
    pOvs   = pSlot->overlays;
    if (pLib != NULL) {
        nLibClips = pLib->nRecords;
        nBytes    = pLib->n140;
    } else {
        nLibClips = 0;
        nBytes    = 0;
    }
    ctx.nBytes = nBytes;
    ctx.pCount = &nLibClips;
    for (i = 0; i < nOvs; i++) {
        pWork = pOvs[i].pWork;
        if (pWork != NULL) {
            pOvs[i].nTree    = pWork->nTreeSize;
            pWork->nTreeSize = 0;
            if (pLib != NULL) {
                pWork->nClips += pLib->nClips;
            }
            AnimLib_WalkPair(pLib, pWork, (AnimLibWalkFn)AnimLib_MergeSizeCb, NULL);
            if (pWork->nTreeSize & 15) {
                pWork->nTreeSize = ((pWork->nTreeSize >> 4) + 1) << 4;
            }
            pOvs[i].n14 = pOvs[i].n10 - 3;
        }
    }
    if (pLib != NULL && nSlot != 2) {
        AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_TrimCb, &ctx);
        nBytes = ctx.nBytes;
    }
    for (i = 0; i < nOvs; i++) {
        if (pOvs[i].pWork != NULL && nSlot != 2) {
            AnimLib_WalkPair(NULL, pOvs[i].pWork, (AnimLibWalkFn)AnimLib_TrimCb, NULL);
        }
    }
    for (i = 0; i < nOvs; i++) {
        pOvLib       = pOvs[i].pWork;
        nOvClips     = pOvLib->nRecords;
        ctxOv.pCount = &nOvClips;
        AnimLib_WalkPair(NULL, pOvLib, (AnimLibWalkFn)AnimLib_MergeReleaseCb, &ctxOv);
        nLeft = pOvLib->nRecords;
        for (j = 0; j < pOvLib->nRecords; j++) {
            pRec = &pOvLib->pRecords[j];
            if (pRec->n10 != 0) {
                nBytes += pRec->n14;
                k      = -1;
                pOther = pLib;
                do {
                    if (pOther != NULL) {
                        for (m = 0; m < pOther->nRecords; m++) {
                            pRecO = &pOther->pRecords[m];
                            if (strcmp(pRec->name, pRecO->name) == 0) {
                                if (pRecO->n10 != 0) {
                                    nLeft--;
                                    nBytes -= pRec->n14;
                                }
                                pRecO->n10 += pRec->n10;
                                pRec->n10   = 0;
                                pRec->pClip = pRecO;
                                if (k != -1) {
                                    pRec->n12 |= 0x10;
                                } else {
                                    pRec->n12 |= 2;
                                }
                                // fake match: leaves both loops (a found flag: 635 differ, not 421)
                                goto next;
                            }
                        }
                    }
                    k++;
                    pOther = pOvs[k].pWork;
                } while (k < i);
            } else if (pRec->n12 & 4) {
                k      = -1;
                pOther = pLib;
                do {
                    if (pOther != NULL) {
                        for (m = 0; m < pOther->nRecords; m++) {
                            pRecO = &pOther->pRecords[m];
                            if (strcmp(pRec->name, pRecO->name) == 0) {
                                pRecO->n10 += pRec->n10;
                                nLeft--;
                                pRec->n10   = 0;
                                pRec->pClip = pRecO;
                                // fake match: leaves both search loops, as above
                                goto next;
                            }
                        }
                    }
                    k++;
                    pOther = pOvs[k].pWork;
                } while (k < i);
            } else {
                nLeft--;
            }
        next:;
        }
        nClips += nLeft;
    }
    ctx.nBytes = nBytes;
    ctx.pCount = &nLibClips;
    if (pLib != NULL && nClips != 0) {
        AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_MergeReleaseCb, &ctx);
    }
    for (i = 0; i < nOvs; i++) {
    }
    nClips   += nLibClips;
    nClipsAll = nClips;
    if (nClipsAll == 0) {
        // fake match: the shared exit as a jump (without: 88.7%, not 91.6%)
        goto done;
    }
    nIndexSize = ((nClipsAll * 4 >> 4) + 1) << 4;
    nRecSize   = ((nClipsAll >> 4) + 1) << 4;
    nTotal     = 0x20 + ctx.nBytes + nIndexSize + nRecSize;
    if (nSlot != 2) {
        nBudget = gClipBankBudget;
        if (gSlotsDoubleBuffered) {
            if (nSlot == 0) {
                nBudget = 942080.0f * gSlot0BankShare;
            } else {
                nBudget = 942080.0f * (1.0f - gSlot0BankShare);
            }
        }
        gClipBankStats[nSlot].nKeep     = gClipsPerLeaf;
        gClipBankStats[nSlot].nTrimmed  = 0;
        gClipBankStats[nSlot].nBytes    = ctx.nBytes;
        if (nTotal > nBudget) {
            ctx.pCount   = &nClips;
            nHdr         = 0x20 + nIndexSize + nRecSize;
            ctx.nTarget  = nBudget;
            ctx.nBytes   += nHdr;
            // fake match: two locals with the same value, set again in the retry (EA keeps one
            // in r20 until the retry and spills the one read at the end; one local: 99.4%)
            nBytesBefore2 = ctx.nBytes;
            nBytesBefore  = ctx.nBytes;
            pRecordsCopy = StaticMem_Alloc(pLib->nRecords * sizeof(ClipRecord), 1, 0, "skalib.c", 2078);
            Mem_cpy(pRecordsCopy, pLib->pRecords, pLib->nRecords * sizeof(ClipRecord));
            pIndexCopy = StaticMem_Alloc(pLib->nClips2 * 2, 1, 0, "skalib.c", 2080);
            Mem_cpy(pIndexCopy, pLib->pIndex, pLib->nClips2 * 2);
            pTreeCopy = StaticMem_Alloc(pLib->nTreeSize, 1, 0, "skalib.c", 2082);
            Mem_cpy(pTreeCopy, pLib->pTree, pLib->nTreeSize);
            for (i = 0; i < nOvs; i++) {
                apRecords[i] =
                    StaticMem_Alloc(pOvs[i].pWork->nRecords * sizeof(ClipRecord), 1, 0, "skalib.c", 2086);
                Mem_cpy(apRecords[i], pOvs[i].pWork->pRecords, pOvs[i].pWork->nRecords * sizeof(ClipRecord));
                apIndex[i] = StaticMem_Alloc(pOvs[i].pWork->nClips2 * 2, 1, 0, "skalib.c", 2088);
                Mem_cpy(apIndex[i], pOvs[i].pWork->pIndex, pOvs[i].pWork->nClips2 * 2);
                apTree[i] = StaticMem_Alloc(pOvs[i].nTree, 1, 0, "skalib.c", 2090);
                Mem_cpy(apTree[i], pOvs[i].pWork->pTree, pOvs[i].nTree);
            }
            if (!AnimLib_TrimToFit(&ctx, pLib, pOvs, nOvs, 0)) {
                gClipBankStats[nSlot].nKeep    = gClipsPerLeaf;
                gClipBankStats[nSlot].nTrimmed = 0;
                ctx.nTarget      = nBudget;
                gClipBankStats[nSlot].nBytes   = ctx.nBytes;
                nClips           = nClipsAll;
                ctx.nBytes       = nBytesBefore;
                nBytesBefore2    = ctx.nBytes;
                Mem_cpy(pLib->pRecords, pRecordsCopy, pLib->nRecords * sizeof(ClipRecord));
                Mem_cpy(pLib->pIndex, pIndexCopy, pLib->nClips2 * 2);
                Mem_cpy(pLib->pTree, pTreeCopy, pLib->nTreeSize);
                for (i = 0; i < nOvs; i++) {
                    Mem_cpy(pOvs[i].pWork->pRecords, apRecords[i],
                            pOvs[i].pWork->nRecords * sizeof(ClipRecord));
                    Mem_cpy(pOvs[i].pWork->pIndex, apIndex[i], pOvs[i].pWork->nClips2 * 2);
                    Mem_cpy(pOvs[i].pWork->pTree, apTree[i], pOvs[i].nTree);
                }
                if (!AnimLib_TrimToFit(&ctx, pLib, pOvs, nOvs, 1)) {
                    if (nSlot == 1 || !bBoth) {
                        nBudget = ctx.nBytes;
                    } else {
                        nBudget      = ctx.nBytes;
                        gSlot0BankShare = nBudget / 942080.0f;
                    }
                }
            }
            StaticMem_Free(pRecordsCopy);
            StaticMem_Free(pIndexCopy);
            StaticMem_Free(pTreeCopy);
            for (i = 0; i < nOvs; i++) {
                StaticMem_Free(apRecords[i]);
                StaticMem_Free(apIndex[i]);
                StaticMem_Free(apTree[i]);
            }
            gClipBankStats[nSlot].nBytes    = ctx.nBytes - nHdr;
            gClipBankStats[nSlot].nKeep     = ctx.nKeep;
            gClipBankStats[nSlot].nTrimmed  = nBytesBefore2 - ctx.nBytes;
            gClipBankStats[nSlot].nMaxUsers = ctx.nMaxUsers;
        }
        gClipBankStats[nSlot].nBudget = nBudget;
        gClipBankStats[nSlot].n04     = 0;
        pBank = gClipBanks[nSlot];
        if (pBank == NULL) {
            pBank = StaticMem_Alloc(nBudget, 2, 0x40, "skalib.c", 2159);
        }
        nRet = nBudget;
    } else {
        pBank = StaticMem_Alloc(nTotal, 2, 0x40, "skalib.c", 2175);
        nRet  = nTotal;
    }
    pBank->nClips  = nClips;
    pBank->uId     = 0;
    pBank->pFile   = NULL;
    pBank->ppClips = (void**)((u8*)pBank + 0x20);
    for (n = 0; n < nClips; n++) {
        pBank->ppClips[n] = NULL;
    }
    pBank->pRecords      = (u8*)pBank->ppClips + nIndexSize;
    pSlot->pEnd          = pBank->pRecords + nRecSize;
    gClipBanks[nSlot]  = pBank;
done:
    return nRet;
}

// The clip bank memory lent as save image n, uSize bytes: while only one of the first two slots has
// a bank, it is carved out of that bank; otherwise it is slot n's own bank.
static inline u8* Skalib_LentMemory(int n, u32 uSize) {
    if (gClipBanks[0] != NULL && gClipBanks[1] == NULL) return (u8*)gClipBanks[0] + n * uSize;
    if (gClipBanks[0] == NULL && gClipBanks[1] != NULL) return (u8*)gClipBanks[1] + n * uSize;
    return (u8*)gClipBanks[n];
}

// Lends clip bank memory to the memory card code as save file image n (0 or 1; MC_Gc fn_8009F02C in
// game type 6): fn_8009EF90 bytes (the image size) at n times that size into the one bank when only
// one of slots 0 and 1 has one, otherwise slot n's bank. Its clip data is first copied to ARAM
// (allocated the first time) and comes back with Skalib_ReclaimBankMemory. Returns the memory.
u8* Skalib_LendBankMemory(int n) {
    u32 uSize = fn_8009EF90();
    u8* p;
    if (gClipBanks[0] != NULL && gClipBanks[1] == NULL) {
        p = (u8*)gClipBanks[0] + n * uSize;
    } else if (gClipBanks[0] == NULL && gClipBanks[1] != NULL) {
        p = (u8*)gClipBanks[1] + n * uSize;
    } else {
        p = (u8*)gClipBanks[n];
    }
    gLentBankAramSizes[n] = uSize;
    if (gLentBankAram[n] == 0) {
        gLentBankAram[n] = GoARAM_Alloc(gLentBankAramSizes[n]);
    }
    GoARAM_WaitTransfer(GoARAM_CopyToAram(p, gLentBankAram[n], gLentBankAramSizes[n]));
    return p;
}

// Takes back the clip bank memory lent as save file image n (MC_Gc fn_8009EF98, game type 6, once
// the image is parked in ARAM): the bank's clip data is copied back from ARAM and the ARAM freed.
void Skalib_ReclaimBankMemory(int n) {
    u32 uSize = fn_8009EF90();
    GoARAM_WaitTransfer(GoARAM_CopyFromAram(Skalib_LentMemory(n, uSize), gLentBankAram[n],
                                            gLentBankAramSizes[n]));
    if (gLentBankAram[n] != 0) {
        GoARAM_Free(gLentBankAram[n]);
        gLentBankAram[n] = 0;
    }
}

// Copies the clips of a sac file (pData, the 'SAC ' object Character_LoadSacFromStream got) into
// its slot's clip bank, when the slot has overlays. nSlot 0..2 is the slot's own library (malesac /
// femsac); 3 and up the stream id of a golfer's overlay (LibOverlay.n10: golfer id + 3; -1 once
// merged). Each clip still in use (users above 0, n18 not 0) is byte-swapped and its header and the
// parts kept in main memory (tracks, the pE0 / pE8 data, its morph library, the tracks' two bit
// arrays) copied to the end of the bank's records (LibSlot.pEnd); its frame streams, ranges and
// keys go to ARAM through the staging buffers, each padded to 32 bytes. The copied bytes are added
// to the bank (ClipBank.uId) and the slot's stats, and the record then points at the copy (flag 8).
// For a golfer's overlay the merged library (AnimLib_BuildCb over the slot's library and the
// overlay) is then built into the golfer's 0x2800-byte library block, with its records in the
// character's pRecords (allocated the first time). Returns 0x2800 for a golfer's overlay, else 0.
s32 AnimLib_MergeOverlay(u8* pData, int nSlot) {
    u8*         pClipSrc;
    LibSlot*    pSlot;
    LibOverlay* pOv;
    ClipBank*   pBank;
    AnimLib*    pSrc;
    Clip*       pHdr;
    u32         uAram;
    u32         nStride1;
    u32         nStride2;
    u8*         pSrc1;
    u8*         pSrc2;
    int         f;
    u32         nCopied;
    s32         n4C;
    s32         n50;
    u32         nHdr;
    u32         uPad;
    s32         n;
    u32         uAl;
    u8*         pOut;
    AnimLib*    pNew;
    s32         nSize;
    ClipRecord* pRec;
    int         i;
    s32         nRet = 0;
    AnimLib*    pLibFile;
    u32         k;
    u32         uAramStart;
    s32         n4CAl;
    BuildCtx    ctx;
    u32         o;
    u32         s;
    int         p;
    u8          bFound;
    Player*     pPlayer;

    if (nSlot < 3) {
        k        = nSlot;
        pSlot    = &gLibSlots[k];
        pLibFile = pSlot->pLib;
        pSrc     = pLibFile;
    } else {
        for (k = 0; k < 3; k++) {
            pSlot = &gLibSlots[k];
            for (i = 0; i < pSlot->nOverlays; i++) {
                pOv = &pSlot->overlays[i];
                if (pOv->n10 == nSlot) {
                    k        = pOv->pChar->nSlot;
                    pOv->n10 = -1;
                    // fake match: leaves both loops (two breaks and a test: 84.8%, not 87.1%)
                    goto found;
                }
            }
        }
    found:
        pLibFile = pSlot->pLib;
        pSrc     = pOv->pWork;
    }
    if (pSrc != NULL && pSlot->nOverlays != 0) {
        u32 aPad[4] = {0, 0, 0, 0};

        pBank = gClipBanks[k];
        pHdr  = (Clip*)pSlot->pEnd;
        for (i = 0; i < pSrc->nRecords; i++) {
            // register note: n50Al is a block local so that it numbers after n4CAl (EA spills
            // n4CAl, keeps n50Al in r14); n4CAl stays last at function level for the spill slots
            s32 n50Al;

            pRec = &pSrc->pRecords[i];
            if (pRec->n10 <= 0 || pRec->n18 == 0) continue;
            pBank->ppClips[pSlot->n150] = pHdr;
            // port: a clip of an overlay library ('SAL '/'SAC '), little-endian on disc; a little-endian
            //       port does not swap here (Clip is then read in place)
            SKA_SwapClip(pData + (uptr)pSrc->pRecords[i].pClip);
            pClipSrc = pData + (uptr)pSrc->pRecords[i].pClip;
            nHdr     = ((Clip*)pClipSrc)->pD0 - pClipSrc;
            Mem_cpy(pHdr, pClipSrc, nHdr);
            nCopied = nHdr;
            pOut    = (u8*)pHdr + nHdr;
            if (((Clip*)pClipSrc)->n2C != 0) {
                Mem_cpy(pOut, ((Clip*)pClipSrc)->pD0, ((Clip*)pClipSrc)->n2C);
                pOut += ((Clip*)pClipSrc)->n2C;
                nCopied = nHdr + ((Clip*)pClipSrc)->n2C;
            }
            uPad = 16 - ((uptr)pOut & 15);
            if (uPad == 16) {
                uPad = 0;
            }
            if (uPad != 0) {
                Mem_cpy(pOut, aPad, uPad);
                pOut += uPad;
                nCopied += uPad;
            }
            n = ((Clip*)pClipSrc)->n40 + ((Clip*)pClipSrc)->n3C;
            Mem_cpy(pOut, (u8*)((Clip*)pClipSrc)->uAram - n, n);
            pOut += n;
            nCopied += n;
            if (((Clip*)pClipSrc)->n64 != 0) {
                Mem_cpy(pOut, ((Clip*)pClipSrc)->pF4, pHdr->n64);
                pHdr->pF4 = pOut;
                pOut += pHdr->n64;
                nCopied += pHdr->n64;
            } else {
                pHdr->pF4 = NULL;
            }
            n = (pHdr->n1C * 2 + 31) / 32 * 4;
            Mem_cpy(pOut, ((Clip*)pClipSrc)->pF8, n);
            pHdr->pF8 = pOut;
            pOut += n;
            nCopied += n;
            Mem_cpy(pOut, ((Clip*)pClipSrc)->pFC, n);
            pHdr->pFC = pOut;
            nCopied += n;
            // fake match: the four sizes are rounded up to 32 through one u32 scratch, tested and
            // rounded from the size itself (a local per size, or rounding the local in place, gives
            // other registers and keeps no copy of the result)
            uAl = pHdr->n8C * 2;
            if (pHdr->n8C * 2 & 31) {
                uAl = ((pHdr->n8C * 2 >> 5) + 1) << 5;
            }
            nStride1 = uAl;
            uAl      = pHdr->n8E;
            if (pHdr->n8E & 31) {
                uAl = ((pHdr->n8E >> 5) + 1) << 5;
            }
            nStride2   = uAl;
            pSrc1      = (u8*)((Clip*)pClipSrc)->uAram;
            pSrc2      = ((Clip*)pClipSrc)->pE4;
            pHdr->n38  = nStride1 * pHdr->nFrames;
            pHdr->n04  = nStride2 * pHdr->nFrames;
            nSize      = pHdr->n38 + pHdr->n04;
            n4C        = pHdr->n4C;
            uAl        = n4C;
            if (n4C & 31) {
                uAl = ((n4C >> 5) + 1) << 5;
            }
            n4CAl = uAl;
            n50   = pHdr->n50;
            uAl   = n50;
            if (n50 & 31) {
                uAl = ((n50 >> 5) + 1) << 5;
            }
            n50Al      = uAl;
            uAram      = GoARAM_Alloc(n4CAl + n50Al + nSize);
            uAramStart = uAram;
            if (pHdr->n38 != 0) {
                for (f = 0; f < pHdr->nFrames; f++) {
                    Mem_cpy(gSKAAram16BitFrame, pSrc1, pHdr->n8C * 2);
                    GoARAM_WaitTransfer(GoARAM_CopyToAram(gSKAAram16BitFrame, uAram, nStride1));
                    uAram += nStride1;
                    pSrc1 += pHdr->n8C * 2;
                }
            }
            if (pHdr->n04 != 0) {
                for (f = 0; f < pHdr->nFrames; f++) {
                    Mem_cpy(gSKAAram8BitFrame, pSrc2, pHdr->n8E);
                    GoARAM_WaitTransfer(GoARAM_CopyToAram(gSKAAram8BitFrame, uAram, nStride2));
                    uAram += nStride2;
                    pSrc2 += pHdr->n8E;
                }
            }
            // fake match: the strides are u32 (as s32, the ARAM calls' (u32) conversion becomes a
            // loop temp of its own); the halving is signed, as in the original
            pHdr->n8C = (s32)nStride1 / 2;
            pHdr->n8E = nStride2;
            if (n50 != 0) {
                Mem_cpy(gSKAAramRanges, ((Clip*)pClipSrc)->pEC, pHdr->n50);
                GoARAM_WaitTransfer(GoARAM_CopyToAram(gSKAAramRanges, uAram, n50Al));
                uAram += n50Al;
            }
            pHdr->n50 = n50Al;
            if (n4C != 0) {
                Mem_cpy(gSKAAramKeys, ((Clip*)pClipSrc)->pF0, pHdr->n4C);
                GoARAM_WaitTransfer(GoARAM_CopyToAram(gSKAAramKeys, uAram, n4CAl));
            }
            pHdr->n4C = n4CAl;
            SKA_PatchMemory(pHdr, uAramStart);
            pBank->uId += nCopied;
            pHdr = (Clip*)((u8*)pHdr + nCopied);
            gClipBankStats[k].n04 += nCopied;
            pSrc->pRecords[i].pClip = pBank->ppClips[pSlot->n150];
            pSrc->pRecords[i].n12 |= 8;
            pSlot->n150++;
        }
        pSlot->pEnd = (u8*)pHdr;
        if (nSlot >= 3) {
            nSize = sizeof(AnimLib) + pSrc->nTreeSize + pSrc->nClips * 4;
            pNew  = pOv->pChar->pLib;
            // fake match: += (nRet is still 0 here); with = the first scheduling pass issues the
            // store's li before the size's slwi, which changes the registers
            nRet += 0x2800;
            Mem_cpy(pNew, pSrc, 21 * 4);
            pNew->n108      = pSrc->n108;
            pNew->nDefault  = pSrc->nDefault;
            pNew->pTree     = (u8*)pNew + sizeof(AnimLib);
            pNew->ppClips   = (void**)(pNew->pTree + pSrc->nTreeSize);
            pNew->pIndex    = NULL;
            pNew->pFile     = pNew;
            pNew->n12C      = nSize;
            pNew->pClipData = NULL;
            pNew->uFlags    = pSrc->uFlags;
            pNew->nRecords  = 0;
            pNew->nTreeSize = 0;
            pNew->nClips    = 0;
            pNew->pBank     = pBank;
            if (pOv->pChar->pRecords == NULL) {
                pOv->pChar->pRecords = StaticMem_Alloc(pSrc->nClips * sizeof(ClipRecord), 2, 0, "skalib.c", 2943);
            }
            ctx.pLib     = pNew;
            ctx.pRecords = pOv->pChar->pRecords;
            AnimLib_WalkPair(pLibFile, pSrc, (AnimLibWalkFn)AnimLib_BuildCb, &ctx);
            if (pNew->nTreeSize & 15) {
                pNew->nTreeSize = ((pNew->nTreeSize >> 4) + 1) << 4;
            }
            pOv->pChar->pLib = pNew;
        } else {
            for (p = 0; p < gSession.nNumPlayers; p++) {
                bFound = 0;
                for (s = 0; s < 3; s++) {
                    if (bFound) break;
                    pSlot = &gLibSlots[k];
                    for (o = 0; o < pSlot->nOverlays; o++) {
                        if (pSlot->overlays[o].pChar == gPlayers[p].pChar) {
                            bFound = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
done:
    return nRet;
}

// Frees the working (swapped) copies of each slot's library and overlays once the sac files are
// merged (Character_PostInit, Character_ReloadSacFiles), marking the overlays merged (stream id -1)
// and restarting the slot's bank clip count; while gSacReloading is set (a reload between holes)
// only the current slot's.
void AnimLib_FreeWorkCopies(void) {
    LibSlot*    pSlot;
    int         k;
    u32         i;
    LibOverlay* pOv;

    for (i = 0; i < 3; i++) {
        pSlot = &gLibSlots[i];
        if (gSacReloading != 0 && i != gCurLibSlot) continue;
        if (pSlot->nOverlays != 0) {
            for (k = 0; pSlot->nOverlays > k; k++) {
            }
            for (k = 0; k < pSlot->nOverlays; k++) {
                pOv = &pSlot->overlays[k];
                AnimLib_Free(pOv->pWork);
                pOv->pWork = NULL;
                pOv->n10   = -1;
            }
            pSlot->n150 = 0;
        }
        if (pSlot->pLib != NULL) {
            AnimLib_Free(pSlot->pLib);
        }
        pSlot->pLib = NULL;
    }
}

// Frees the pristine (as-loaded, unswapped) copies of each slot's library and overlays that
// AnimLib_ReloadSlot rebuilds from, and empties every slot's overlay list.
void AnimLib_FreeCopies(void) {
    LibSlot*    pSlot;
    LibOverlay* pOv;
    u32         i;
    int         j;

    for (i = 0; i < 3; i++) {
        pSlot = &gLibSlots[i];
        if (pSlot->nOverlays != 0) {
            for (j = 0; j < pSlot->nOverlays; j++) {
                pOv = &pSlot->overlays[j];
                StaticMem_Free(pOv->pCopy);
                pOv->pCopy = NULL;
            }
        }
        pSlot->nOverlays = 0;
        if (pSlot->pCopy != NULL) {
            StaticMem_Free(pSlot->pCopy);
        }
        pSlot->pCopy = NULL;
    }
}

// Applies each active overlay's golfer's custom animations (AnimLib_ApplyCustomAnims, from the
// player's save profile) to the overlay in slot nSlot: Character_PostInit for slots 0 and 1,
// AnimLib_ReloadSlot for the slot it reloads.
void AnimLib_ApplySlotCustomAnims(int nSlot) {
    LibSlot*    pSlot = &gLibSlots[nSlot];
    LibOverlay* pOv;
    int         n     = pSlot->nOverlays;
    int         i;

    if (n != 0) {
        pOv = pSlot->overlays;
        for (i = 0; i < n; pOv++, i++) {
            if (pOv->bActive) {
                AnimLib_ApplyCustomAnims(pOv, nSlot, pOv->pChar->nPlayer);
            }
        }
    }
}

// Sets the round's clip bank budgets (Skalib_SetBudgets) and plans the bank of each of the three
// slots (AnimLib_PlanBank), before Character_PostInit loads the sac files into them. The sizes it
// adds up are not used.
void Skalib_PlanBanks(void) {
    u32 i;
    int nTotal = 0;
    Skalib_SetBudgets();
    for (i = 0; i < 3; i++) {
        nTotal += AnimLib_PlanBank(i);
    }
}

// Before a hole (Character_ReloadSacFiles): rebuilds the libraries of the next slot
// (Skalib_NextSlot) so its sac files can be merged again. The ARAM of the slot's bank clips is
// freed; when the slot has overlays its library and overlays are copied again from their pristine
// copies (they were swapped in place, so a reload starts from the file) and set up (AnimLib_Load),
// each overlay waiting for its sac file again (stream id golfer id + 3). Then the golfers' custom
// animations are applied, the animation stream set up for the slot's players
// (AnimStream_SizeSlotClips) and the bank planned (AnimLib_PlanBank).
void AnimLib_ReloadSlot(void) {
    u32         nSlot = Skalib_NextSlot();
    u32         i;
    LibSlot*    pSlot;
    int         j;
    LibOverlay* pOv;
    Clip*       pClip;
    AnimLib*    pLib;

    for (i = 0; i < gClipBanks[nSlot]->nClips; i++) {
        pClip = gClipBanks[nSlot]->ppClips[i];
        if (pClip != NULL && (pClip->uFlags & 4)) {
            GoARAM_Free(pClip->uAram);
        }
    }
    pSlot = &gLibSlots[nSlot];
    if (pSlot->nOverlays != 0) {
        pSlot->pLib = StaticMem_Alloc(pSlot->nSize, 1, 0x40, "skalib.c", 3193);
        Mem_cpy(pSlot->pLib, pSlot->pCopy, pSlot->nSize);
        pLib = AnimLib_Load((u8*)pSlot->pLib, ClipBank_Get(nSlot));
        pLib->pFile = pSlot->pLib;
        for (j = 0; j < pSlot->nOverlays; j++) {
            pOv        = &pSlot->overlays[j];
            pOv->pWork = StaticMem_Alloc(pOv->nSize, 1, 0x40, "skalib.c", 3207);
            Mem_cpy(pOv->pWork, pOv->pCopy, pOv->nSize);
            AnimLib_Load((u8*)pOv->pWork, ClipBank_Get(nSlot));
            pOv->n10 = pOv->n14 + 3;
        }
    }
    AnimLib_ApplySlotCustomAnims(nSlot);
    AnimStream_SizeSlotClips(nSlot);
    AnimLib_PlanBank(nSlot);
}

// The clips of a library for an animation group (0..20), style (0..7), club class (0..5) and key
// (0..10): returns the leaf's first entry in ppClips, with its clip count in *pCount, its played
// mask in *ppUsed and its first index in *pFirst (each when given). A missing style tries style 0;
// a missing group, style or club falls back to the group's default leaf, then the library's, and
// sets flag 1 in *pFlags; a missing key falls back to the club's default leaf, then the group's,
// then the library's, and sets flag 2. NULL for arguments out of range or when no default is left.
void** AnimLib_Find(AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, s32* pCount,
                    u32* pFlags, u32** ppUsed, s32* pFirst) {
    s16* pNode;
    s16* pClub;
    s32  nOff;
    s16* pLeaf;

    if (ppUsed != NULL) {
        *ppUsed = NULL;
    }
    if (nGroup >= 0 && nGroup < 21 && nStyle >= 0 && nStyle < 8 && nClub >= 0 && nClub < 6 && nKey >= 0 &&
        nKey < 11) {
        *pCount = 1;
        nOff = pLib->groups[nGroup];
        if (nOff < 0) {
            *pFlags |= 1;
            nOff = pLib->nDefault;
        } else {
            pNode = (s16*)(pLib->pTree + nOff);
            nOff  = pNode[1 + nStyle];
            if (nOff < 0 && nStyle != 0) {
                nOff = pNode[1];
            }
            if (nOff < 0) {
                *pFlags |= 1;
                nOff = pNode[0];
                if (nOff < 0) {
                    nOff = pLib->nDefault;
                    if (nOff < 0) return NULL;
                }
            } else {
                nOff = *(s16*)(pLib->pTree + nOff + nClub * 2);
                if (nOff < 0) {
                    *pFlags |= 1;
                    nOff = pNode[0];
                    if (nOff < 0) {
                        nOff = pLib->nDefault;
                        if (nOff < 0) return NULL;
                    }
                } else {
                    pClub = (s16*)(pLib->pTree + nOff);
                    nOff  = pClub[2 + nKey];
                    if (nOff < 0) {
                        *pFlags |= 2;
                        nOff = pClub[1];
                        if (nOff < 0) {
                            nOff = pNode[0];
                            if (nOff < 0) {
                                nOff = pLib->nDefault;
                                if (nOff < 0) return NULL;
                            }
                        }
                    }
                }
            }
        }
        pLeaf   = (s16*)(pLib->pTree + nOff);
        *pCount = pLeaf[0];
        if (ppUsed != NULL) {
            *ppUsed = (u32*)(pLeaf + 2);
        }
        if (pFirst != NULL) {
            *pFirst = pLeaf[1];
        }
        return pLib->ppClips + pLeaf[1];
    }
    return NULL;
}

// For reactions (groups 1 and 5): whether this clip is the one the player last played for this
// group, style and club class; *ppSlot gets that slot so the caller can record the new one.
u8 AnimLib_WasLastPlayed(int nPlayer, const char* pName, char** ppSlot, int nGroup, int nStyle, int nClub) {
    int nKind;
    int nOff;
    *ppSlot = NULL;
    switch (nGroup) {
    case 5:
        nKind = 0;
        break;
    case 1:
        nKind = 1;
        break;
    default:
        return 0;
    }
    if (nPlayer < 0 || nPlayer >= 4) return 0;
    // fake match: the slot's byte offset summed in one local, player and kind first, and added to
    // the table's base: the same element as gLastReactionClips[nPlayer][nKind][nStyle][nClub], whose
    // index form colours the sum and the base differently
    nOff = nPlayer * sizeof(gLastReactionClips[0]) + nKind * sizeof(gLastReactionClips[0][0]);
    nOff += nStyle * sizeof(gLastReactionClips[0][0][0]);
    nOff += nClub * sizeof(gLastReactionClips[0][0][0][0]);
    *ppSlot = (char*)gLastReactionClips + nOff;
    return strcmp(pName, (char*)gLastReactionClips + nOff) == 0;
}

int   AnimLib_RandomIndex(u32 uUsed, int nCount);
void* AnimStream_GetClip(int nPlayer, int nGroup, int nStyle, int nClub);

// The clip a player plays for an animation group, style, club class and key (NULL when the group is
// out of range or the leaf is empty). At a streamed position (AnimStream_IsStreamed, the key taken as the
// default when the leaf came from a key default) it is the animation stream's clip (AnimStream_GetClip)
// when there is one; with pName the library's clip of that name; otherwise one of the leaf's clips
// at random, never the reaction the player played last (the next one instead), and, through the
// leaf's played mask, none again until all of them have been played. A reaction's pick is recorded
// as the player's last.
void* AnimLib_Pick(int nPlayer, AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, u32* pFlags,
                   const char* pName) {
    s32    nCount;
    u32*   pUsed;
    char*  pSlot;
    void** ppClips;
    int    nPick;
    u32    uUsed;
    u32    uBit;
    u32    uAll;
    void*  pClip;

    AnimLib_Find(pLib, nGroup, nStyle, nClub, nKey, &nCount, pFlags, &pUsed, NULL);
    if (AnimStream_IsStreamed(nGroup, nStyle, nClub, (*pFlags & 2) ? -1 : nKey)) {
        pClip = AnimStream_GetClip(nPlayer, nGroup, nStyle, nClub);
        if (pClip != NULL) return pClip;
    }
    if (nGroup >= 0 && nGroup < 21) {
        ppClips = AnimLib_Find(pLib, nGroup, nStyle, nClub, nKey, &nCount, pFlags, &pUsed, NULL);
        if (ppClips != NULL && nCount > 0) {
            if (pName == NULL) {
                pSlot = NULL;
                nPick = AnimLib_RandomIndex(*pUsed, nCount);
                if (nCount > 1 && AnimLib_WasLastPlayed(nPlayer, ((Clip*)ppClips[nPick])->name, &pSlot,
                                                        nGroup, nStyle, nClub)) {
                    if (++nPick >= nCount) {
                        nPick = 0;
                    }
                }
                if (nPick < 32 && pUsed != NULL) {
                    uUsed = *pUsed;
                    uAll  = (1 << nCount) - 1;
                    if ((uAll & uUsed) == uAll) {
                        uUsed = 1 << nPick;
                    } else {
                        uBit = 1 << nPick;
                        while (uUsed & uBit) {
                            if (++nPick >= nCount) {
                                nPick = 0;
                            }
                            uBit = 1 << nPick;
                        }
                        uUsed |= uBit;
                        if ((uAll & uUsed) == uAll) {
                            uUsed = 1 << nPick;
                        }
                    }
                    if (pSlot != NULL) {
                        strcpy(pSlot, ((Clip*)ppClips[nPick])->name);
                    }
                    *pUsed |= uUsed;
                }
                return ppClips[nPick];
            }
            return AnimLib_FindByName(pLib, pName);
        }
    }
    return NULL;
}

// A random clip index under nCount, re-rolled up to twice when it has been played already.
int AnimLib_RandomIndex(u32 uUsed, int nCount) {
    int nTries = 0;
    u32 nPick;
    do {
        nPick = Misc_RandFunc(1) % nCount;
        if (!((1 << nPick) & uUsed)) break;
    } while (++nTries < 3);
    return nPick;
}

// Byte-swaps a clip bank file's header in place: its 64-bit id, then the next four words (clip
// count onwards).
void ClipBank_SwapHeader(void* p) {
    SwapField fmt[5] = {{8, -8}, {4, 4}, {4, 4}, {4, 4}, {4, 4}};
    void*     pSrc = p;
    void*     pDst = p;
    // port: a clip bank's header ('BNK '), little-endian on disc; a little-endian port does not swap here
    ByteSwap_Records(&pSrc, &pDst, fmt, 5, 1);
}

// Byte-swaps a group node of a library's clip tree from pSrc into pDst: its default leaf and eight
// style node offsets (halfwords), then two bytes left as they are.
void AnimLib_SwapGroupNode(void* pSrc, void* pDst) {
    SwapField fmt[3] = {{2, 2}, {0x10, 2}, {2, 1}};
    // port: a clip-tree group node ('SAL '), little-endian on disc; a little-endian port does not swap here
    ByteSwap_Records(&pSrc, &pDst, fmt, 3, 1);
}

// Byte-swaps a style node of a library's clip tree from pSrc into pDst: its six club node offsets
// (halfwords), then two bytes left as they are.
void AnimLib_SwapStyleNode(void* pSrc, void* pDst) {
    SwapField fmt[2] = {{12, 2}, {2, 1}};
    // port: a clip-tree style node ('SAL '), little-endian on disc; a little-endian port does not swap here
    ByteSwap_Records(&pSrc, &pDst, fmt, 2, 1);
}

// Byte-swaps a club node of a library's clip tree (AnimClubNode, 0x20 bytes) from pSrc into pDst:
// its halfwords (the default leaf and the eleven keys' leaves among them), then its flags word.
void AnimLib_SwapClubNode(void* pSrc, void* pDst) {
    SwapField fmt[5] = {{2, 2}, {2, 2}, {0x16, 2}, {2, 2}, {4, 4}};
    // port: a clip-tree club node ('SAL '), little-endian on disc; a little-endian port does not swap here
    ByteSwap_Records(&pSrc, &pDst, fmt, 5, 1);
}

// Byte-swaps a leaf of a library's clip tree (AnimLeaf) from pSrc into pDst: clip count, first
// entry, played mask.
void AnimLib_SwapLeaf(void* pSrc, void* pDst) {
    SwapField fmt[3] = {{2, 2}, {2, 2}, {4, 4}};
    // port: a clip-tree leaf ('SAL '), little-endian on disc; a little-endian port does not swap here
    ByteSwap_Records(&pSrc, &pDst, fmt, 3, 1);
}

// Swaps the whole clip tree, walking it the way AnimLib_Find does.
void AnimLib_SwapTree(AnimLib* pLib, u8* pSrc, u8* pDst) {
    int  nGroup;
    int  nStyle;
    int  nClub;
    int  nKey;
    s32  nOff;
    s16* pNode;
    s16* pStyle;
    s16* pClub;

    if (pLib->nDefault >= 0) {
        AnimLib_SwapLeaf(pSrc + pLib->nDefault, pDst + pLib->nDefault);
    }
    for (nGroup = 0; nGroup < 21; nGroup++) {
        nOff = pLib->groups[nGroup];
        if (nOff < 0) continue;
        pNode = (s16*)(pDst + nOff);
        AnimLib_SwapGroupNode(pSrc + nOff, pNode);
        if (pNode[0] >= 0) {
            AnimLib_SwapLeaf(pSrc + pNode[0], pDst + pNode[0]);
        }
        for (nStyle = 0; nStyle < 8; nStyle++) {
            nOff = pNode[1 + nStyle];
            if (nOff <= 0) continue;
            pStyle = (s16*)(pDst + nOff);
            AnimLib_SwapStyleNode(pSrc + nOff, pStyle);
            for (nClub = 0; nClub < 6; nClub++) {
                nOff = pStyle[nClub];
                if (nOff <= 0) continue;
                pClub = (s16*)(pDst + nOff);
                AnimLib_SwapClubNode(pSrc + nOff, pClub);
                if (pClub[1] >= 0) {
                    AnimLib_SwapLeaf(pSrc + pClub[1], pDst + pClub[1]);
                }
                for (nKey = 0; nKey < 11; nKey++) {
                    nOff = pClub[2 + nKey];
                    if (nOff > 0) {
                        AnimLib_SwapLeaf(pSrc + nOff, pDst + nOff);
                    }
                }
            }
        }
    }
}

// Sets up an animation library file at pData in place and returns it: the header, at the first
// 16-byte boundary in pData, is byte-swapped (its pFile then points at pData), the parts after it
// are swapped and its offsets turned into pointers. A library whose header names a bank takes its
// clips from pBank by number (all of them the bank's first clip when the ids differ; NULL when
// there is no bank). Otherwise it has a clip index and records: with flag 1 its clips follow them
// in the file (SKA_LoadFromMem, into a clip table allocated here); without, it is an overlay
// library whose clips a sac file brings into a bank later (no clip table).
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank) {

    SwapField hdrFmt[19] = {{0x100, 4}, {8, -8}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
                            {4, 4},     {4, -4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {2, 2},
                            {2, 2}};
    SwapField recFmt[7]  = {{0x10, -1}, {2, 2}, {2, 2}, {4, 4}, {4, 4}, {4, 4}, {4, 4}};
    void*     pDst;
    void*     pSrc;
    u32       uPad;
    AnimLib*  pLib;
    int       i;

    uPad = 16 - ((uptr)pData & 15);
    if (uPad == 16) {
        uPad = 0;
    }
    pLib        = (AnimLib*)(pData + uPad);
    pLib->pFile = pData;
    pData       = (u8*)pLib;
    pData       = pData + sizeof(AnimLib);   // from here on, where the next part of the file is
    pDst = pSrc = pLib;
    // port: an animation library's header ('SAL ', and 'SAC ' overlays), little-endian on disc; a
    //       little-endian port does not swap here.
    // port: The library is then used in place (AnimLib over the bytes; its offsets become 32-bit pointers).
    ByteSwap_Records(&pSrc, &pDst, hdrFmt, 19, 1);
    if (pLib->pBank != NULL) {
        if (pBank == NULL) return NULL;
        pLib->pClipData = NULL;
        pLib->ppClips   = (void**)pData;
        pData += pLib->nClips * 4;
        pLib->nClips2   = pLib->nClips;
        pLib->pTree = pData;
        pDst = pSrc = pLib->ppClips;
        // port: the library's clip numbers into its bank, little-endian on disc; a little-endian port does
        //       not swap here
        BYTESWAP_SWAPDATA((u8**)&pSrc, pDst, pLib->nClips * 4, 4);   // port: pSrc is a void* (ByteSwap_Records's)
        if (pLib->uId != pBank->uId) {
            for (i = 0; i < pLib->nClips; i++) {
                pLib->ppClips[i] = pBank->ppClips[0];
            }
        } else {
            for (i = 0; i < pLib->nClips; i++) {
                pLib->ppClips[i] = pBank->ppClips[(uptr)pLib->ppClips[i]];
            }
        }
        pLib->pBank = pBank;
    } else {
        pLib->pIndex  = (s16*)pData;
        pData += pLib->nClips * 2;
        pLib->nClips2 = pLib->nClips;
        if ((uptr)pData & 15) {
            pData = (u8*)((((uptr)pData >> 4) + 1) << 4);
        }
        pLib->pRecords = (ClipRecord*)pData;
        pData += pLib->nRecords * sizeof(ClipRecord);
        pLib->pTree = pData;
        pData += pLib->nTreeSize;
        pDst = pSrc = pLib->pIndex;
        // port: the library's clip index, little-endian on disc; a little-endian port does not swap here
        BYTESWAP_SWAPDATA((u8**)&pSrc, pDst, pLib->nClips * 2, 2);   // port: as above
        pDst = pSrc = pLib->pRecords;
        // port: the library's clip records (ClipRecord, laid over the bytes), little-endian on disc; a
        //       little-endian port does not swap here
        ByteSwap_Records(&pSrc, &pDst, recFmt, 7, pLib->nRecords);
        if (pLib->uFlags & 1) {
            if ((uptr)pData & 15) {
                pData = (u8*)((((uptr)pData >> 4) + 1) << 4);
            }
            pLib->pClipData = pData;
            pLib->ppClips   = StaticMem_Alloc(pLib->nClips * 4, 2, 0x40, "skalib.c", 4164);
            for (i = 0; i < pLib->nRecords; i++) {
                pLib->pRecords[i].pClip =
                    SKA_LoadFromMem(pLib->pClipData + (uptr)pLib->pRecords[i].pClip, NULL, 16);
            }
            for (i = 0; i < pLib->nClips; i++) {
                pLib->ppClips[i] = pLib->pRecords[pLib->pIndex[i]].pClip;
            }
        } else {
            pLib->pClipData = NULL;
            pLib->ppClips   = NULL;
        }
        pLib->pBank = NULL;
    }
    pDst = pSrc = pLib->pTree;
    AnimLib_SwapTree(pLib, pSrc, pDst);
    return pLib;
}

// Sets up a clip bank file at pFile in place and returns it: the header at the first uAlign
// boundary is byte-swapped, and each clip offset of the table after it turned into its clip
// (SKA_LoadFromMem, from the 16-aligned data after the table).
ClipBank* ClipBank_Load(u8* pFile, u32 uAlign) {
    u32       uUnused;
    u8*       pSrc;
    u32       uPad;
    ClipBank* pBank;
    int       i;
    u8*       pData;

    pData = pFile;
    uPad = uAlign - ((uptr)pData & (uAlign - 1));
    if (uPad == uAlign) {
        uPad = 0;
    }
    pBank = (ClipBank*)(pData + uPad);
    // port: a clip bank ('BNK '), little-endian on disc; a little-endian port does not swap here; the bank
    //       is then used in place (ClipBank over the bytes, its clip offsets turned into 32-bit pointers)
    ClipBank_SwapHeader(pData + uPad);
    pBank->pFile   = NULL;
    // The clip offsets are found from the file's start, not the aligned bank: this is only right for a
    // file that is already aligned (ClipBank_Restore's buffer is).
    pData += 0x20;
    pBank->ppClips = (void**)pData;
    pSrc = (u8*)pBank->ppClips;
    // port: the bank's clip offsets, little-endian on disc; a little-endian port does not swap here
    BYTESWAP_SWAPDATA(&pSrc, (u8*)pBank->ppClips, pBank->nClips * 4, 4);
    pData += pBank->nClips * 4;
    uPad = 16 - ((uptr)pData & 15);
    if (uPad == 16) {
        uPad = 0;
    }
    pData += uPad;
    for (i = 0; i < pBank->nClips; i++) {
        pBank->ppClips[i] = SKA_LoadFromMem(pData + (uptr)pBank->ppClips[i], &uUnused, 16);
    }
    return pBank;
}

// The 'SAL ' stream chunk handler (Skalib_Register). For slot uId (0..2) without a bank library
// yet: keeps a pristine copy of the file (for AnimLib_ReloadSlot), sets it up (AnimLib_Load against
// the slot's bank) and files it as the slot's bank library when its clips are in the bank, else as
// the slot's own library that overlays are merged over. Any other object is freed.
void AnimLib_OnLoaded(UStreamObject* pFile) {
    u8       bFree = 1;
    u32      nSlot = pFile->uId;
    AnimLib* pLib;

    if (nSlot < 3 && gBankLibs[nSlot] == NULL) {
        gLibSlots[nSlot].pCopy = StaticMem_Alloc(pFile->uSize, 2, 0x40, "skalib.c", 4520);
        Mem_cpy(gLibSlots[nSlot].pCopy, pFile->pData, pFile->uSize);
        gLibSlots[nSlot].nSize = pFile->uSize;
        pLib = AnimLib_Load(pFile->pData, ClipBank_Get(nSlot));
        pLib->pFile = pFile;
        if (pLib->pBank != NULL) {
            gBankLibs[nSlot] = pLib;
        } else {
            gLibSlots[nSlot].pLib = pLib;
        }
        bFree = 0;
    }
    if (bFree) {
        StaticMem_Free(pFile);
    }
}

void ClipBank_Stash(int nSlot);

// The 'BNK ' stream chunk handler (Skalib_Register): the clip bank file of slot uId goes to ARAM
// (ClipBank_Stash) until a character needs it (ClipBank_Restore).
void ClipBank_OnLoaded(UStreamObject* pFile) {
    u32 nSlot = pFile->uId;
    gClipBankFiles[nSlot] = pFile;
    ClipBank_Stash(nSlot);
}

// Makes a clip bank file the bank of slot uId (ClipBank_Load), the bank owning the file; when the
// slot is out of range or already has a bank the file is freed instead.
void ClipBank_Install(UStreamObject* pFile) {
    u8  bFree = 1;
    u32 nSlot = pFile->uId;

    if (nSlot < 3 && gClipBanks[nSlot] == NULL) {
        gClipBanks[nSlot]        = ClipBank_Load(pFile->pData, 16);
        bFree                      = 0;
        gClipBanks[nSlot]->pFile = pFile;
    }
    if (bFree) {
        StaticMem_Free(pFile);
    }
}

// Forgets slot nSlot's clip bank and bank file without freeing them: in the front end (game type 3)
// the bank lives in the restore buffer, and Character_Free lets it go so the next character brings
// it back from ARAM (ClipBank_Restore).
void ClipBank_Release(int nSlot) {
    if (gClipBanks[nSlot] != NULL) {
        if (gClipBanks[nSlot]->pFile != NULL) {
            gClipBanks[nSlot]->pFile = NULL;
            gClipBanks[nSlot]        = NULL;
            gClipBankFiles[nSlot]        = NULL;
        } else {
            gClipBanks[nSlot] = NULL;
        }
    }
    if (gClipBankFiles[nSlot] != NULL) {
        gClipBankFiles[nSlot] = NULL;
    }
}

// Copies slot nSlot's clip bank file (the stream object with its 0x80-byte header) to ARAM,
// allocated the first time, and frees it unless it is the restore buffer; for slot 0, when there is
// no restore buffer yet, allocates it at the file's size. Nothing without a file.
void ClipBank_Stash(int nSlot) {
    if (gClipBankFiles[nSlot] != NULL) {
        gClipBankAramSizes[nSlot] = ((gClipBankFiles[nSlot]->uSize + 0x80) / 32 + 1) * 32;
        if (gClipBankAram[nSlot] == 0) {
            gClipBankAram[nSlot] = GoARAM_Alloc(gClipBankAramSizes[nSlot]);
        }
        GoARAM_WaitTransfer(GoARAM_CopyToAram(gClipBankFiles[nSlot], gClipBankAram[nSlot],
                                              gClipBankAramSizes[nSlot]));
        if (gClipBankFiles[nSlot] != gClipBankRestoreFile) {
            StaticMem_Free(gClipBankFiles[nSlot]);
        }
        gClipBankFiles[nSlot] = NULL;
        if (nSlot == 0 && gClipBankRestoreFile == NULL) {
            gClipBankRestoreFile = StaticMem_Alloc(gClipBankAramSizes[nSlot], 2, 0x20, "skalib.c", 4671);
        }
    }
}

// Brings slot nSlot's clip bank file back from ARAM into the restore buffer and installs it
// (ClipBank_Install), unless the file is already in memory. Character_CreateFromMem calls it in the
// front end (game type 3).
void ClipBank_Restore(int nSlot) {
    if (gClipBankFiles[nSlot] == NULL) {
        gClipBankFiles[nSlot] = gClipBankRestoreFile;
        GoARAM_WaitTransfer(GoARAM_CopyFromAram(gClipBankFiles[nSlot], gClipBankAram[nSlot],
                                                gClipBankAramSizes[nSlot]));
        gClipBankFiles[nSlot]->pData = (u8*)gClipBankFiles[nSlot] + 0x80;
        ClipBank_Install(gClipBankFiles[nSlot]);
    }
}

// Frees the banks' ARAM and the restore buffer.
void ClipBank_FreeAram(void) {
    int i;
    for (i = 0; i < 3; i++) {
        if (gClipBankAram[i] != 0) {
            GoARAM_Free(gClipBankAram[i]);
            gClipBankAram[i] = 0;
        }
    }
    if (gClipBankRestoreFile != NULL) {
        StaticMem_Free(gClipBankRestoreFile);
        gClipBankRestoreFile = NULL;
    }
}

// Hooks the loaders up to the file streamer: 'SAL ' animation libraries and 'BNK ' clip banks.
// A stream object's uId says which of the three animation slots it is for.
void Skalib_Register(void) {
    Stream_RegisterLoadChunkCallback('SAL ', AnimLib_OnLoaded);
    Stream_RegisterLoadChunkCallback('BNK ', ClipBank_OnLoaded);
}

// Unhooks the 'SAL ' and 'BNK ' loaders from the file streamer (Skalib_Register's undo).
void Skalib_Unregister(void) {
    Stream_UnregisterLoadChunkCallback('SAL ');
    Stream_UnregisterLoadChunkCallback('BNK ');
}

// Makes one leaf of an overlay's clip tree play chosen clips: each of its entries in turn is
// pointed at the clip named pNames[i % nNames] (16 characters each) among the clips of group 20's
// pool (style 0, club 0, the default leaf), which gains a user; an entry whose name is not in the
// pool is left. The leaf is group nGroup, style nStyle, club class nClub and key nKey (the club's
// default leaf when nKey < 0). Nothing when there are no names or a node on either path is missing;
// nSlot is not used.
void AnimLib_SetLeafClipsByName(LibOverlay* pOv, int nSlot, int nGroup, int nClub, int nStyle, int nKey,
                                char* pNames,
                 int nNames) {
    int i;
    int j;
    s16* pToIdx;
    s16* pFromIdx;
    AnimLeaf* pTo;
    AnimLeaf* pFrom;
    ClipRecord* pRec;
    AnimClubNode* pNode;
    s16 nOff;
    int nGroupOff;
    u8* pTree;
    u8* pGroup;

    if (nNames == 0) return;
    nGroupOff = pOv->pWork->groups[20];
    if (nGroupOff < 0) return;
    pTree = pOv->pWork->pTree;
    pGroup = pTree;
    pGroup += nGroupOff;
    nOff = *(s16*)(pGroup + 2);
    if (nOff < 0) return;
    nOff = *(s16*)(pTree + nOff);
    if (nOff < 0) return;
    nOff = *(s16*)(pTree + nOff + 2);
    if (nOff < 0) return;
    pFrom = (AnimLeaf*)(pTree + nOff);

    nGroupOff = pOv->pWork->groups[nGroup];
    if (nGroupOff < 0) return;
    nOff = ((s16*)(pTree + nGroupOff + 2))[nStyle];
    if (nOff < 0) return;
    nOff = *(s16*)(pTree + nOff + nClub * 2);
    if (nOff < 0) return;
    pNode = (AnimClubNode*)(pTree + nOff);
    if (nKey < 0) {
        nOff = pNode->nDefault;
        if (nOff < 0) return;
        pTo = (AnimLeaf*)(pTree + nOff);
    } else {
        nOff = pNode->aKeys[nKey];
        if (nOff < 0) return;
        pTo = (AnimLeaf*)(pTree + nOff);
    }

    for (i = 0; i < pTo->nCount; i++) {
        pToIdx = pOv->pWork->pIndex + pTo->nFirst;
        pFromIdx = pOv->pWork->pIndex + pFrom->nFirst;
        for (j = 0; j < pFrom->nCount; j++) {
            pRec = &pOv->pWork->pRecords[pFromIdx[j]];
            if (strcmp(pRec->name, &pNames[(i % nNames) * 16]) == 0) {
                pToIdx[i] = pFromIdx[j];
                pRec->n10++;
                break;
            }
        }
    }
}

// Applies player n's custom animations (the three lists of gpSaveData[n].choices the
// Create-a-Player screen fills) to the overlay: list 0 (a1) replaces the reactions of group 5 style
// 7, list 1 (a82) those of group 5 style 1 (both club class 2, the default leaf), and list 2
// (sz103, one name) the clips of group 0, style 0, club 0, key 0 (AnimLib_SetLeafClipsByName).
void AnimLib_ApplyCustomAnims(LibOverlay* pOv, int nSlot, s32 n) {
    AnimLib_SetLeafClipsByName(pOv, nSlot, 5, 2, 7, -1, gpSaveData[n].choices.a1[0],
                               gpSaveData[n].choices.n0);
    AnimLib_SetLeafClipsByName(pOv, nSlot, 5, 2, 1, -1, gpSaveData[n].choices.a82[0],
                               gpSaveData[n].choices.n81);
    AnimLib_SetLeafClipsByName(pOv, nSlot, 0, 0, 0, 0, gpSaveData[n].choices.sz103,
                               gpSaveData[n].choices.n102);
}

// The working overlay library loaded for the character in slot 0 or 1, or NULL (AnimStream sets up
// a player's streamed clips from it).
AnimLib* AnimLib_GetCharOverlay(Character* pChar) {
    int i;
    int nSlot;

    for (nSlot = 0; nSlot <= 1; nSlot++) {
        for (i = 0; i < gLibSlots[nSlot].nOverlays; i++) {
            if (gLibSlots[nSlot].overlays[i].pChar == pChar) {
                return gLibSlots[nSlot].overlays[i].pWork;
            }
        }
    }
    return NULL;
}

// The own library (LibSlot.pLib, the one overlays merge over) of the character's animation slot;
// NULL once the work copies are freed.
AnimLib* AnimLib_GetCharSlotLib(Character* pChar) {
    return gLibSlots[pChar->nSlot].pLib;
}

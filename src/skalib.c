// skalib.c (EA's name, from its asserts): the skeletal animation library. So far only the clip
// lookup and pick, the part that chooses which clip a golfer plays.

#include "golfer.h"
#include "game.h"
#include "engine.h"

// An animation library (a SAL object: glbchar.gcb holds the shared male/female ones, and each
// character's CHR object embeds its own). On disc it is little-endian; the loader swaps it and
// fills in the pointers. Clips are found through a tree of s16 byte offsets into pTree:
//   group node:  [0] default leaf, [1 + style] style node (8 styles)
//   style node:  [club] club node (6 club classes)
//   club node:   [1] default leaf, [2 + key] leaf (11 keys)
//   leaf:        [0] clip count, [1] first entry in ppClips, then a u32 "played" mask
struct AnimLib {
    s32    groups[64];          // 0x000  byte offset of each group's node, -1 none (21 used)
    u64    uId;                 // 0x100  must match the clip bank's when the clips live there
    s32    n108;                // 0x108
    s32    nDefault;            // 0x10C  the leaf used when a group has nothing
    s32    nTreeSize;           // 0x110
    s32    nClips;              // 0x114  entries in ppClips
    s32    nRecords;            // 0x118  36-byte clip records (a 16-character name first)
    u8*    pTree;               // 0x11C
    void** ppClips;             // 0x120  clip pointers; a clip's name is at +0xA0
    s16*   pIndex;              // 0x124  record of each clip
    void*  pFile;               // 0x128  the loaded file this library sits in
    s32    n12C;                // 0x12C
    struct ClipRecord* pRecords;  // 0x130
    u8*    pClipData;           // 0x134  the library's own clips
    struct ClipBank* pBank;     // 0x138  set in the file: the clips are in a bank instead
    u32    uFlags;              // 0x13C  1: the library carries its own clips
    s32    n140;                // 0x140
    s16    n144;                // 0x144
    s16    nClips2;             // 0x146
};

typedef struct ClipRecord {
    char   name[16];            // 0x00
    s16    n10;                 // 0x10  leaves using the clip, while merging
    s16    n12;                 // 0x12  merge flags: 1 keep, 2 / 0x10 moved (pClip then points to the
                                //       record it went to)
    s32    n14;                 // 0x14
    s32    n18;                 // 0x18
    void*  pClip;               // 0x1C  offset into the clip data on disc, pointer once loaded
    s32    n20;                 // 0x20
} ClipRecord;

// A clip bank: clips shared by several libraries (a 0x20-byte header, the clip offsets, then the
// clips, each 16-aligned).
typedef struct ClipBank {
    u64    uId;                 // 0x00
    s32    nClips;              // 0x08
    s32    n0C;                 // 0x0C
    void** ppClips;             // 0x10
    void*  pFile;               // 0x14  the loaded file, while the bank owns it
    u8*    pRecords;            // 0x18  a planned bank: where its records go
    u32    n1C;                 // 0x1C
} ClipBank;

// One field of a byte-swap description: nBytes bytes made of nSize-byte values (negative: not swapped).
typedef struct SwapField {
    s32 nBytes;
    s32 nSize;
} SwapField;

void  fn_8001F08C(void** ppSrc, void** ppDst, SwapField* pFormat, int nFields, int nCount);   // byte-swap
                                                                                            // by format
void  fn_80076158(void** ppSrc, void* pDst, int nBytes, int nSize);   // byte-swap a run
void* fn_80020DD4(void* pClip, void* pOut, int nAlign);
ClipBank* ClipBank_Get(u32 nSlot);
u32   fn_800B6564(u32 uSize);                          // ARAM alloc
void  fn_800B6594(u32 uAram);                          // ARAM free
void  fn_800B6844(void* pSrc, u32 uAram, u32 uSize);   // copy to ARAM
void  fn_800B68B4(void* pDst, u32 uAram, u32 uSize);   // copy from ARAM
void  fn_800B67EC(void);                               // wait for the ARAM copy
void  fn_80020BC8(void* pClip);                        // swaps a clip in place
void  fn_80020F60(struct Clip* pClip, u32 uAram);
void  AnimLib_Free(AnimLib* pLib);
void  fn_800269E4(struct LibOverlay* pOv, int nSlot, s32 n);
void  fn_800CA9DC(int nSlot);
u32   Skalib_NextSlot(void);
void  Skalib_SetBudgets(void);
u32   AnimLib_PlanBank(u32 nSlot);
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank);
u32   fn_8009EF90(void);
int   Skalib_HasOverlays(int nSlot);
u8    fn_800C9828(int nGroup, int nStyle, int nClub, int nKey);
void  AnimLib_FreeCopies(void);
void  ClipBank_FreeAram(void);

// A library that can be layered over a slot's own (0x20 bytes).
typedef struct LibOverlay {
    AnimLib* pWork;             // 0x00  the loaded (swapped) copy
    void*  pCopy;               // 0x04  the file as it came off the disc
    u32    nSize;               // 0x08
    Character* pChar;           // 0x0C  the golfer it was loaded for
    s32    n10;                 // 0x10
    s32    n14;                 // 0x14
    u8     bActive;             // 0x18
    u8     pad19[3];
    s32    nTree;               // 0x1C  the tree size before a merge
} LibOverlay;

// One of the three animation slots (0x158 bytes).
typedef struct LibSlot {
    AnimLib*   pLib;            // 0x000  the loaded (swapped) library
    void*      pCopy;           // 0x004  the file as it came off the disc
    u32        nSize;           // 0x008
    LibOverlay overlays[10];    // 0x00C
    s32        nOverlays;       // 0x14C
    s32        n150;            // 0x150
    u8*        pEnd;            // 0x154  the end of the slot's clip bank records
} LibSlot;

extern ClipBank*   lbl_801C6050[3];   // the clip bank of each slot
extern AnimLib*    lbl_801C605C[3];   // the library of each slot, when its clips are in the bank
extern LibSlot     lbl_801C6068[3];
extern u32         lbl_801C6470[3];   // ARAM copy of each slot's bank file
extern u32         lbl_801C647C[3];   // its size
extern UStreamObject* lbl_801C6488[3];   // each slot's bank file, while it is in main memory
extern UStreamObject* lbl_80281CE0;      // the buffer banks are brought back from ARAM into
extern u8          lbl_80281CE4;
extern u32         lbl_80281078;      // the current slot
extern u32         lbl_80281D04[2];   // ARAM copy of each scratch area
extern u32         lbl_80281D0C[2];   // its size
extern u8          lbl_801D9908[0xC8];
extern u32         lbl_80281D18;
extern u8          lbl_80281CD8;      // double buffering: libraries alternate between slots 0 and 1
extern s32         lbl_80281CE8;      // group, style, club and key being merged
extern s32         lbl_80281CEC;
extern s32         lbl_80281CF0;
extern s32         lbl_80281CF4;
extern s32         lbl_80281074;      // clips a leaf may keep this round
extern u32         lbl_80281CDC;      // bytes of clips a slot may keep
extern f32         lbl_80281D1C;
extern s32         lbl_80281070;
extern s16*        lbl_80281CF8;      // the group, style and club node being built
extern s16*        lbl_80281CFC;
extern s16*        lbl_80281D00;      // leaves this short are left alone by the drop pass
extern u8*         lbl_80281CC4;      // staging buffers (32-aligned), see Skalib_Init
extern u8*         lbl_80281CC8;
extern u8*         lbl_80281CCC;
extern u8*         lbl_80281CD0;
extern u8          lbl_801C5E2C[0x1DC];
extern u8          lbl_801C5C50[0x1DC];
extern u8          lbl_801BF9C0[0x6290];
extern u8          lbl_801B9730[0x6290];

extern char (*lbl_80281D14)[2][8][6][16];   // the last clip name played: [player][reaction kind][style][club]

// The clip with this name (at +0xA0 in each clip), or NULL.
void* AnimLib_FindByName(AnimLib* pLib, const char* pName) {
    int i;
    for (i = 0; i < pLib->nClips; i++) {
        if (pLib->ppClips[i] != NULL && strcmp(((Clip*)pLib->ppClips[i])->name, pName) == 0) {
            return pLib->ppClips[i];
        }
    }
    return NULL;
}

// Sets everything up: no libraries or banks, the last-played table allocated and cleared, and the
// four 32-aligned staging buffers used when overlay clips are merged in.
void Skalib_Init(void) {
    int i;
    s32 j;

    lbl_801C605C[0] = NULL;
    lbl_801C6050[0] = NULL;
    lbl_801C605C[1] = NULL;
    lbl_801C6050[1] = NULL;
    lbl_801C605C[2] = NULL;
    lbl_801C6050[2] = NULL;
    lbl_801D9908[0] = 0;
    for (i = 0; i < 3; i++) {
        lbl_801C6068[i].n150      = 0;
        lbl_801C6068[i].nOverlays = 0;
        lbl_801C6068[i].pLib      = NULL;
        lbl_801C6068[i].pCopy     = NULL;
        for (j = 0; j < 10; j++) {
            lbl_801C6068[i].overlays[j].bActive = 0;
            lbl_801C6068[i].overlays[j].n10     = -1;
            lbl_801C6068[i].overlays[j].n14     = -1;
            lbl_801C6068[i].overlays[j].pCopy   = NULL;
            lbl_801C6068[i].overlays[j].pWork   = NULL;
        }
    }
    if (gSession.nGameType == 3 || gSession.nGameType == 10) {
        lbl_80281D18 = 0;
    }
    lbl_80281D14 = fn_80009B34(4 * sizeof(*lbl_80281D14), 2, 0, "skalib.c", 508);   // four players
    fn_80005AE8(lbl_80281D14, 0, 4 * sizeof(*lbl_80281D14));
    lbl_80281CC4 = lbl_801C5E2C;
    lbl_80281CC8 = lbl_801C5C50;
    lbl_80281CCC = lbl_801BF9C0;
    lbl_80281CD0 = lbl_801B9730;
    lbl_80281CC4 = (u8*)((((uptr)lbl_80281CC4 >> 5) + 1) << 5);
    lbl_80281CC8 = (u8*)((((uptr)lbl_80281CC8 >> 5) + 1) << 5);
    lbl_80281CCC = (u8*)((((uptr)lbl_80281CCC >> 5) + 1) << 5);
    lbl_80281CD0 = (u8*)((((uptr)lbl_80281CD0 >> 5) + 1) << 5);
}

void AnimLib_Free(AnimLib* pLib);
void ClipBank_Free(ClipBank* pBank);

// Frees every library and bank.
void Skalib_Shutdown(void) {
    int i;

    AnimLib_FreeCopies();
    fn_80009E70(lbl_80281D14);
    lbl_80281D14 = NULL;
    for (i = 0; i < 3; i++) {
        if (lbl_801C605C[i] != NULL) {
            AnimLib_Free(lbl_801C605C[i]);
            lbl_801C605C[i] = NULL;
        }
        if (lbl_801C6050[i] != NULL) {
            if (lbl_801C6050[i]->pFile == lbl_80281CE0) {
                lbl_80281CE0 = NULL;
            }
            ClipBank_Free(lbl_801C6050[i]);
            lbl_801C6050[i] = NULL;
        }
    }
    ClipBank_FreeAram();
}

// Frees a library: just its file when it was loaded from one (plus the clip table it allocated
// for its own clips), otherwise each part it was built from.
void AnimLib_Free(AnimLib* pLib) {
    if (pLib->pFile != NULL) {
        if (pLib->pClipData != NULL) {
            fn_80009E70(pLib->ppClips);
        }
        fn_80009E70(pLib->pFile);
    } else {
        if (pLib->ppClips != NULL) {
            fn_80009E70(pLib->ppClips);
        }
        if (pLib->pIndex != NULL) {
            fn_80009E70(pLib->pIndex);
        }
        if (pLib->pTree != NULL) {
            fn_80009E70(pLib->pTree);
        }
        if (pLib->pRecords != NULL) {
            fn_80009E70(pLib->pRecords);
        }
        fn_80009E70(pLib);
    }
}

// Frees a bank, and the ARAM of any of its clips that live there.
void ClipBank_Free(ClipBank* pBank) {
    u32   i;
    Clip* pClip;

    for (i = 0; i < pBank->nClips; i++) {
        pClip = pBank->ppClips[i];
        if (pClip != NULL && (pClip->uFlags & 4)) {
            fn_800B6594(pClip->uAram);
        }
    }
    if (pBank->pFile != NULL) {
        fn_80009E70(pBank->pFile);
    } else {
        fn_80009E70(pBank);
    }
}

// A slot's clip bank (NULL past the three slots).
ClipBank* ClipBank_Get(u32 nSlot) {
    if (nSlot >= 3) return NULL;
    return lbl_801C6050[nSlot];
}

// Called for each position of two clip trees walked side by side: level 0 the default leaves,
// 1 a group (its default leaves), 2 a style, 3 a club (its default leaves), 4 a key's leaves.
// nIndex is the group, style, club or key. A result above 0 stops the walk.
typedef int (*AnimLibWalkFn)(AnimLib* pA, AnimLib* pB, void* pLeafA, void* pLeafB, void* pCtx, int nLevel,
                             int nIndex);

// The node at offset nOff of a library's tree, or NULL when there is no library or no node.
#define SKA_NODE(pLib, nOff) \
    (((pLib) != NULL && (nOff) >= 0) ? (s16*)((pLib)->pTree + (nOff)) : NULL)
// The child nIdx of a node, or NULL.
#define SKA_CHILD(pLib, pNode, nIdx) \
    (((pNode) != NULL && (pNode)[nIdx] >= 0) ? (s16*)((pLib)->pTree + (pNode)[nIdx]) : NULL)

// Walks the clip trees of two libraries together (either may be NULL), telling pfn about every
// group, style, club and key either one has. lbl_80281CE8..CF4 hold where the walk is.
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

    lbl_80281CE8 = -1;
    lbl_80281CEC = -1;
    lbl_80281CF0 = -1;
    lbl_80281CF4 = -1;
    nRet = pfn(pA, pB, SKA_NODE(pA, pA->nDefault), SKA_NODE(pB, pB->nDefault), pCtx, 0, 0);
    if (nRet > 0) return nRet;
    for (nGroup = 0; nGroup < 21; nGroup++) {
        lbl_80281CE8 = nGroup;
        lbl_80281CEC = -1;
        lbl_80281CF0 = -1;
        lbl_80281CF4 = -1;
        pGroupA = SKA_NODE(pA, pA->groups[nGroup]);
        pGroupB = SKA_NODE(pB, pB->groups[nGroup]);
        if (pGroupA == NULL && pGroupB == NULL) continue;
        pLeafA = SKA_CHILD(pA, pGroupA, 0);
        pLeafB = SKA_CHILD(pB, pGroupB, 0);
        nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 1, nGroup);
        if (nRet > 0) return nRet;
        for (nStyle = 0; nStyle < 8; nStyle++) {
            lbl_80281CEC = nStyle;
            lbl_80281CF0 = -1;
            pStyleA = SKA_CHILD(pA, pGroupA, 1 + nStyle);
            pStyleB = SKA_CHILD(pB, pGroupB, 1 + nStyle);
            if (pStyleA == NULL && pStyleB == NULL) continue;
            nRet = pfn(pA, pB, NULL, NULL, pCtx, 2, nStyle);
            if (nRet > 0) return nRet;
            for (nClub = 0; nClub < 6; nClub++) {
                lbl_80281CF0 = nClub;
                lbl_80281CF4 = -1;
                pClubA = SKA_CHILD(pA, pStyleA, nClub);
                pClubB = SKA_CHILD(pB, pStyleB, nClub);
                if (pClubA == NULL && pClubB == NULL) continue;
                pLeafA = SKA_CHILD(pA, pClubA, 1);
                pLeafB = SKA_CHILD(pB, pClubB, 1);
                nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 3, nClub);
                if (nRet > 0) return nRet;
                for (nKey = 0; nKey < 11; nKey++) {
                    lbl_80281CF4 = nKey;
                    pLeafA = SKA_CHILD(pA, pClubA, 2 + nKey);
                    pLeafB = SKA_CHILD(pB, pClubB, 2 + nKey);
                    nRet = pfn(pA, pB, pLeafA, pLeafB, pCtx, 4, nKey);
                    if (nRet > 0) return nRet;
                }
            }
        }
    }
    return 0;
}

// A leaf of the clip tree (see AnimLib); while two trees are merged its mask holds flags
// instead: 1 keep this one, 2 replace it.
typedef struct AnimLeaf {
    s16 nCount;                 // 0x0
    s16 nFirst;                 // 0x2
    u32 uMask;                  // 0x4
} AnimLeaf;

// What the merge adds up as it walks.
typedef struct MergeCtx {
    u32  n0;
    s32* pCount;                // 0x04  clips still in use
    s32  nBytes;                // 0x08  bytes of clip data still in use
    s32  nTarget;               // 0x0C  bytes to get down to
    s32  nKeep;                 // 0x10  clips a leaf keeps
    s32  nMaxUsers;             // 0x14  clips shared by more leaves than this are not picked
} MergeCtx;

// Merge walk, sizing pass: counts the tree bytes the merged library needs (a leaf, then the
// node for this level) and decides for each leaf pair which side wins.
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
            if (fn_800C9828(lbl_80281CE8, lbl_80281CEC, lbl_80281CF0, lbl_80281CF4)) {
                pOver->uMask |= 2;
                pLeaf->uMask |= 2;
            } else if (lbl_80281CE8 == 20) {
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
        } else if (fn_800C9828(lbl_80281CE8, lbl_80281CEC, lbl_80281CF0, lbl_80281CF4)) {
            pLeaf->uMask |= 2;
        } else {
            pLeaf->uMask &= ~2;
            pLeaf->uMask |= 1;
        }
    }
    return 0;
}

// Merge walk, release pass: every clip of a replaced leaf loses a user; the ones nobody uses any
// more come off the totals.
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

u8 Skalib_IsDoubleBuffered(void) {
    return lbl_80281CD8;
}

// The slot the next library loads into: with double buffering it flips between 0 and 1.
u32 Skalib_NextSlot(void) {
    if (Skalib_IsDoubleBuffered()) {
        lbl_80281078 = (lbl_80281078 == 0);
    } else if (Skalib_HasOverlays(0)) {
        lbl_80281078 = 0;
    } else {
        lbl_80281078 = 1;
    }
    return lbl_80281078;
}

u32 Skalib_CurSlot(void) {
    return lbl_80281078;
}

// Sets the per-round limits: how many clips each leaf may keep (every clip with one player, at
// most 10 with more) and the memory for them; with two players' libraries loaded (double
// buffering), how the memory is split between the two slots, from their sizes, kept to 44-56%.
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
    lbl_80281CD8 = 0;
    if (!Skalib_HasOverlays(0) || !Skalib_HasOverlays(1)) {
        lbl_80281074 = aKeepSingle[gSession.nNumPlayers - 1];
        lbl_80281CDC = aBytes[0];
    } else {
        n = gSession.nNumPlayers;
        if (n > 1) {
            lbl_80281CD8 = 1;
        }
        lbl_80281074 = aKeepDouble[n - 1];
        lbl_80281CDC = aBytes[n - 1];
    }
    lbl_80281078 = (Rand_Next(1) & 1) ^ 1;
    if (lbl_80281CD8) {
        pSlot0 = &lbl_801C6068[0];
        pSlot1 = &lbl_801C6068[1];
        nSize0 = pSlot0->pLib->n140;
        nSize1 = pSlot1->pLib->n140;
        for (i = 0; pSlot0->nOverlays > i; i++) {
            nSize0 += pSlot0->overlays[i].pWork->n140;
        }
        for (i = 0; pSlot1->nOverlays > i; i++) {
            nSize1 += pSlot1->overlays[i].pWork->n140;
        }
        lbl_80281D1C = (f32)nSize0 / (f32)(nSize0 + nSize1);
        lbl_80281D1C = (lbl_80281D1C < 0.44f) ? 0.44f : ((lbl_80281D1C > 0.56f) ? 0.56f : lbl_80281D1C);
    }
}

f32 Skalib_Random(void);

// Merge walk, trim pass: cuts each leaf down to the clip limit (none for a leaf being replaced,
// all of them for the ones fn_800C9828 protects), keeping a run of clips at a random start.
int AnimLib_TrimCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx, int nLevel,
                   int nIndex) {
    s32         nKeep;
    s32         nCount;
    int         nStart;
    int         i;
    ClipRecord* pRec;

    if (fn_800C9828(lbl_80281CE8, lbl_80281CEC, lbl_80281CF0, lbl_80281CF4)) {
        nKeep = 10000;
    } else if (pLeafA != NULL && (pLeafA->uMask & 2)) {
        nKeep = 0;
    } else {
        nKeep = lbl_80281074;
    }
    if (pLeafA != NULL && (nCount = pLeafA->nCount) > nKeep) {
        nStart = (nCount - nKeep) * Skalib_Random();
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
    if (pLeafB != NULL && (nCount = pLeafB->nCount) > nKeep) {
        nStart = (nCount - nKeep) * Skalib_Random();
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

f32 Skalib_Random(void) {
    return Rand_Float(1);
}

// A clip that can still be kept: not moved (2, 0x10), not kept already (1), and used by
// between 1 and nMaxUsers leaves.
#define SKA_KEEPABLE(pRec, pCtx) \
    (!((pRec)->n12 & 2) && !((pRec)->n12 & 0x10) && !((pRec)->n12 & 1) && (pRec)->n10 > 0 && \
     (pRec)->n10 <= (pCtx)->nMaxUsers)

// Merge walk, keep pass: marks nCount - nKeep more clips of each leaf to keep, picked at random
// (the next keepable one from a random start, looking forward, then back).
int AnimLib_KeepRandomCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx,
                         int nLevel, int nIndex) {
    AnimLeaf*   pLeaf;
    AnimLib*    pLib;
    ClipRecord* pRec;
    int         nMarked;
    s16*        pIdx;
    int         i;
    int         nStart;
    int         j;
    int         nExtra;

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
                nStart = Rand_Next(1) % pLeaf->nCount;
                for (j = nStart; j < pLeaf->nCount; j++) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    if (SKA_KEEPABLE(pRec, pCtx)) {
                        // fake match: the search's exit as a jump (not tested without it)
                        goto found;
                    }
                }
                for (j = nStart; j >= 0; j--) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    if (SKA_KEEPABLE(pRec, pCtx)) {
                        // fake match: the search's exit as a jump (not tested without it)
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
                nStart = Rand_Next(1) % pLeaf->nCount;
                for (j = nStart; j < pLeaf->nCount; j++) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    while ((pRec->n12 & 2) || (pRec->n12 & 0x10)) {
                        pRec = (ClipRecord*)pRec->pClip;
                    }
                    if (!(pRec->n12 & 1) && pRec->n10 > 0 && pRec->n10 <= pCtx->nMaxUsers) {
                        // fake match: the search's exit as a jump (not tested without it)
                        goto found2;
                    }
                }
                for (j = nStart; j >= 0; j--) {
                    pRec = &pLib->pRecords[pIdx[j]];
                    while ((pRec->n12 & 2) || (pRec->n12 & 0x10)) {
                        pRec = (ClipRecord*)pRec->pClip;
                    }
                    if (!(pRec->n12 & 1) && pRec->n10 > 0 && pRec->n10 <= pCtx->nMaxUsers) {
                        // fake match: the search's exit as a jump (not tested without it)
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

// Merge walk, keep pass: marks nCount - nKeep more clips of each leaf to keep, highest n18 first.
int AnimLib_KeepBestCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, MergeCtx* pCtx,
                       int nLevel, int nIndex) {
    AnimLeaf*   pLeaf;
    AnimLib*    pLib;
    int         nExtra;
    int         nMarked;
    int         i;
    int         j;
    ClipRecord* pRec;
    ClipRecord* pBest;
    s16*        pIdx;

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
                    if (SKA_KEEPABLE(pRec, pCtx)) {
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
            for (; nMarked < nExtra; nMarked++) {
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

// Merge walk: the largest clip count of any leaf still in play goes into lbl_80281074.
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
        if (pLeaf->nCount > lbl_80281074) {
            lbl_80281074 = pLeaf->nCount;
        }
    }
    return 0;
}

// Merge walk, drop pass: takes the clips marked 1 out of leaves longer than lbl_80281070, freeing
// the ones nobody uses any more; stops (returning 1) once the bytes in use fall under the target.
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
            if (pLeaf->nCount <= lbl_80281070) return 0;
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

// The library a merge writes, and the records it copies clips into.
typedef struct BuildCtx {
    AnimLib*    pLib;
    ClipRecord* pRecords;
} BuildCtx;

// Space for a node or leaf at the end of the tree being built.
#define SKA_ALLOC(pLib, p, nSize)                  \
    (p) = (void*)((pLib)->pTree + (pLib)->nTreeSize); \
    (pLib)->nTreeSize += (nSize)

// Merge walk, build pass: writes the merged tree into pCtx->pLib (a node for each group, style
// and club, a leaf wherever either side has one) and copies the clips of each leaf: from the one
// side that has it, or from both when the overlay's clips are added to the library's.
int AnimLib_BuildCb(AnimLib* pA, AnimLib* pB, AnimLeaf* pLeafA, AnimLeaf* pLeafB, BuildCtx* pCtx, int nLevel,
                    int nIndex) {
    AnimLeaf*   pSrc    = NULL;
    AnimLib*    pSrcLib = NULL;
    int         bAny    = 0;
    AnimLib*    pLib    = pCtx->pLib;
    ClipRecord* pRecs   = pCtx->pRecords;
    AnimLeaf*   pNew    = NULL;
    u8          bFromA;
    u8          bKeep;
    s32         i;
    s16*        pIdx;

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
        SKA_ALLOC(pLib, lbl_80281CF8, 0x14);
        if (bAny) {
            lbl_80281CF8[0] = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            lbl_80281CF8[0] = -1;
        }
        for (i = 0; i < 8; i++) {
            lbl_80281CF8[1 + i] = -1;
        }
        break;
    case 2:
        lbl_80281CF8[1 + nIndex] = pLib->nTreeSize;
        SKA_ALLOC(pLib, lbl_80281CFC, 0xC);
        for (i = 0; i < 6; i++) {
            lbl_80281CFC[i] = -1;
        }
        break;
    case 3:
        lbl_80281CFC[nIndex] = pLib->nTreeSize;
        SKA_ALLOC(pLib, lbl_80281D00, 0x20);
        if (bAny) {
            lbl_80281D00[1] = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            lbl_80281D00[1] = -1;
        }
        for (i = 0; i < 11; i++) {
            lbl_80281D00[2 + i] = -1;
        }
        break;
    case 4:
        if (bAny) {
            lbl_80281D00[2 + nIndex] = pLib->nTreeSize;
            SKA_ALLOC(pLib, pNew, 8);
        } else {
            lbl_80281D00[2 + nIndex] = -1;
        }
        break;
    }
    if (pNew == NULL) return 0;
    bKeep = fn_800C9828(lbl_80281CE8, lbl_80281CEC, lbl_80281CF0, lbl_80281CF4);
    if (lbl_80281CE8 == 20) {
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
    return lbl_801C6068[nSlot].nOverlays > 0;
}

// Cuts a library and its overlays down until their clips fit pCtx->nTarget: rounds of lowering
// the per-leaf limit, marking clips to keep (by n18, or at random), and dropping the rest from
// leaves longer than lbl_80281070 (3 down to 1). First the overlays are trimmed, then the
// library itself. TRUE when the target was reached.
u8 AnimLib_TrimToFit(MergeCtx* pCtx, AnimLib* pLib, LibOverlay* pOvs, int nOvs, u8 bBest) {
    int      nRet = 0;
    int      i;
    AnimLib* pOvLib;

    pCtx->nMaxUsers = 100000;
    lbl_80281074    = 0;
    AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_MaxCountCb, pCtx);
    for (i = 0; i < nOvs; i++) {
        AnimLib_WalkPair(NULL, pOvs[i].pWork, (AnimLibWalkFn)AnimLib_MaxCountCb, pCtx);
    }
    pCtx->nKeep = lbl_80281074;
    for (lbl_80281070 = 3; lbl_80281070 >= 1; lbl_80281070--) {
        while (pCtx->nKeep > lbl_80281070) {
            pCtx->nKeep--;
            for (i = 0; i < nOvs; i++) {
                pOvLib = pOvs[i].pWork;
                if (bBest) {
                    AnimLib_WalkPair(NULL, pOvLib, (AnimLibWalkFn)AnimLib_KeepBestCb, pCtx);
                } else {
                    AnimLib_WalkPair(NULL, pOvLib, (AnimLibWalkFn)AnimLib_KeepRandomCb, pCtx);
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
    pCtx->nKeep = lbl_80281074;
    for (lbl_80281070 = 3; lbl_80281070 >= 1; lbl_80281070--) {
        while (pCtx->nKeep > lbl_80281070) {
            pCtx->nKeep--;
            if (bBest) {
                AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_KeepBestCb, pCtx);
            } else {
                AnimLib_WalkPair(pLib, NULL, (AnimLibWalkFn)AnimLib_KeepRandomCb, pCtx);
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

// What was spent on each slot's clip bank, kept for reference.
typedef struct SlotStats {
    s32 nBudget;                // 0x00  bytes the bank may use
    s32 n04;                    // 0x04
    s32 nBytes;                 // 0x08  clip bytes wanted before trimming
    s32 nTrimmed;               // 0x0C  bytes the trim took off
    s32 nKeep;                  // 0x10  clips per leaf it settled on
    s32 nMaxUsers;              // 0x14
} SlotStats;

extern SlotStats lbl_801C6008[3];

// Plans the clip bank for a slot: merges the slot's library with its overlays (clips with the
// same name are shared, later copies pointing at the first), and, unless it is slot 2, trims
// the result to the slot's share of the 920 KB clip budget (random picks first; if that cannot
// fit, the best-ranked picks from a fresh copy). Allocates the bank and returns its size.
u32 AnimLib_PlanBank(u32 nSlot) {
    LibSlot*    pSlot = &lbl_801C6068[nSlot];
    u32         nRet  = 0;
    u8          bBoth = 0;
    s32         nRecSize;
    u8*         apTree[10];
    s16*        apIndex[10];
    MergeCtx    ctx;
    MergeCtx    ctxOv;
    s32         nClips;
    s32         nLibClips;
    s32         nOvClips;
    s32         nClipsAll;
    s16*        pIndexCopy;
    u8*         pTreeCopy;
    ClipRecord* pRecordsCopy;
    s32         nBytesBefore;
    ClipRecord* apRecords[10];
    s32         nIndexSize;
    s32         nHdr;
    AnimLib*    pLib;
    s32         nBytes;
    LibOverlay* pOvs;
    int         nOvs;
    int         i;
    int         j;
    int         k;
    int         m;
    AnimLib*    pOvLib;
    AnimLib*    pOther;
    LibOverlay* pp;
    ClipRecord* pRec;
    ClipRecord* pRecO;
    s32         nLeft;
    s32         nTotal;
    s32         nBudget;
    ClipBank*   pBank;

    if (lbl_801C6068[0].nOverlays != 0 && lbl_801C6068[1].nOverlays != 0) {
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
        pOvLib = pOvs[i].pWork;
        if (pOvLib != NULL) {
            pOvs[i].nTree     = pOvLib->nTreeSize;
            pOvLib->nTreeSize = 0;
            if (pLib != NULL) {
                pOvLib->nClips += pLib->nClips;
            }
            AnimLib_WalkPair(pLib, pOvLib, (AnimLibWalkFn)AnimLib_MergeSizeCb, NULL);
            if (pOvLib->nTreeSize & 15) {
                pOvLib->nTreeSize = ((pOvLib->nTreeSize >> 4) + 1) << 4;
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
                pp     = pOvs - 1;
                do {
                    if (pOther == NULL) {
                        // fake match: the search's exit as a jump (not tested without it)
                        goto skip;
                    }
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
                            // fake match: the search's exit as a jump (not tested without it)
                            goto next;
                        }
                    }
                skip:
                    k++;
                    pOther = (++pp)->pWork;
                } while (k < i);
            } else if (pRec->n12 & 4) {
                k      = -1;
                pOther = pLib;
                pp     = pOvs - 1;
                do {
                    if (pOther == NULL) {
                        // fake match: the search's exit as a jump (not tested without it)
                        goto skip2;
                    }
                    for (m = 0; m < pOther->nRecords; m++) {
                        pRecO = &pOther->pRecords[m];
                        if (strcmp(pRec->name, pRecO->name) == 0) {
                            pRecO->n10 += pRec->n10;
                            nLeft--;
                            pRec->n10   = 0;
                            pRec->pClip = pRecO;
                            // fake match: the search's exit as a jump (not tested without it)
                            goto next;
                        }
                    }
                skip2:
                    k++;
                    pOther = (++pp)->pWork;
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
    nClipsAll = nClips + nLibClips;
    nClips    = nClipsAll;
    if (nClipsAll == 0) {
        // fake match: the shared exit as a jump (without: 88.7%, not 91.6%)
        goto done;
    }
    nIndexSize = ((nClipsAll * 4 >> 4) + 1) << 4;
    nRecSize   = ((nClipsAll >> 4) + 1) << 4;
    nTotal     = ctx.nBytes + nIndexSize + nRecSize + 0x20;
    if (nSlot != 2) {
        nBudget = lbl_80281CDC;
        if (lbl_80281CD8) {
            if (nSlot == 0) {
                nBudget = 942080.0f * lbl_80281D1C;
            } else {
                nBudget = 942080.0f * (1.0f - lbl_80281D1C);
            }
        }
        lbl_801C6008[nSlot].nKeep     = lbl_80281074;
        lbl_801C6008[nSlot].nTrimmed  = 0;
        lbl_801C6008[nSlot].nBytes    = ctx.nBytes;
        if (nTotal > nBudget) {
            ctx.pCount   = &nClips;
            nHdr         = nIndexSize + nRecSize + 0x20;
            ctx.nTarget  = nBudget;
            ctx.nBytes  += nHdr;
            nBytesBefore = ctx.nBytes;
            pRecordsCopy = fn_80009B34(pLib->nRecords * sizeof(ClipRecord), 1, 0, "skalib.c", 2078);
            Mem_cpy(pRecordsCopy, pLib->pRecords, pLib->nRecords * sizeof(ClipRecord));
            pIndexCopy = fn_80009B34(pLib->nClips2 * 2, 1, 0, "skalib.c", 2080);
            Mem_cpy(pIndexCopy, pLib->pIndex, pLib->nClips2 * 2);
            pTreeCopy = fn_80009B34(pLib->nTreeSize, 1, 0, "skalib.c", 2082);
            Mem_cpy(pTreeCopy, pLib->pTree, pLib->nTreeSize);
            for (i = 0; i < nOvs; i++) {
                apRecords[i] =
                    fn_80009B34(pOvs[i].pWork->nRecords * sizeof(ClipRecord), 1, 0, "skalib.c", 2086);
                Mem_cpy(apRecords[i], pOvs[i].pWork->pRecords, pOvs[i].pWork->nRecords * sizeof(ClipRecord));
                apIndex[i] = fn_80009B34(pOvs[i].pWork->nClips2 * 2, 1, 0, "skalib.c", 2088);
                Mem_cpy(apIndex[i], pOvs[i].pWork->pIndex, pOvs[i].pWork->nClips2 * 2);
                apTree[i] = fn_80009B34(pOvs[i].nTree, 1, 0, "skalib.c", 2090);
                Mem_cpy(apTree[i], pOvs[i].pWork->pTree, pOvs[i].nTree);
            }
            if (!AnimLib_TrimToFit(&ctx, pLib, pOvs, nOvs, 0)) {
                lbl_801C6008[nSlot].nKeep    = lbl_80281074;
                lbl_801C6008[nSlot].nTrimmed = 0;
                ctx.nTarget      = nBudget;
                lbl_801C6008[nSlot].nBytes   = ctx.nBytes;
                nClips           = nClipsAll;
                ctx.nBytes       = nBytesBefore;
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
                        lbl_80281D1C = nBudget / 942080.0f;
                    }
                }
            }
            fn_80009E70(pRecordsCopy);
            fn_80009E70(pIndexCopy);
            fn_80009E70(pTreeCopy);
            for (i = 0; i < nOvs; i++) {
                fn_80009E70(apRecords[i]);
                fn_80009E70(apIndex[i]);
                fn_80009E70(apTree[i]);
            }
            lbl_801C6008[nSlot].nBytes    = ctx.nBytes - nHdr;
            lbl_801C6008[nSlot].nKeep     = ctx.nKeep;
            lbl_801C6008[nSlot].nTrimmed  = nBytesBefore - ctx.nBytes;
            lbl_801C6008[nSlot].nMaxUsers = ctx.nMaxUsers;
        }
        lbl_801C6008[nSlot].nBudget = nBudget;
        lbl_801C6008[nSlot].n04     = 0;
        pBank = lbl_801C6050[nSlot];
        if (pBank == NULL) {
            pBank = fn_80009B34(nBudget, 2, 0x40, "skalib.c", 2159);
        }
        nRet = nBudget;
    } else {
        pBank = fn_80009B34(nTotal, 2, 0x40, "skalib.c", 2175);
        nRet  = nTotal;
    }
    pBank->nClips  = nClips;
    pBank->uId     = 0;
    pBank->pFile   = NULL;
    pBank->ppClips = (void**)((u8*)pBank + 0x20);
    for (i = 0; i < nClips; i++) {
        pBank->ppClips[i] = NULL;
    }
    pBank->pRecords      = (u8*)pBank->ppClips + nIndexSize;
    lbl_801C6050[nSlot]  = pBank;
    pSlot->pEnd          = pBank->pRecords + nRecSize;
done:
    return nRet;
}

// The scratch area for slot n, uSize bytes: while only one of the first two slots has a bank,
// it is carved out of that bank; otherwise it is the slot's own bank.
static inline u8* Skalib_Scratch(int n, u32 uSize) {
    if (lbl_801C6050[0] != NULL && lbl_801C6050[1] == NULL) return (u8*)lbl_801C6050[0] + n * uSize;
    if (lbl_801C6050[0] == NULL && lbl_801C6050[1] != NULL) return (u8*)lbl_801C6050[1] + n * uSize;
    return (u8*)lbl_801C6050[n];
}

// Copies slot n's scratch area to ARAM.
u8* Skalib_ScratchToAram(int n) {
    u32 uSize = fn_8009EF90();
    u8* p;
    if (lbl_801C6050[0] != NULL && lbl_801C6050[1] == NULL) {
        p = (u8*)lbl_801C6050[0] + n * uSize;
    } else if (lbl_801C6050[0] == NULL && lbl_801C6050[1] != NULL) {
        p = (u8*)lbl_801C6050[1] + n * uSize;
    } else {
        p = (u8*)lbl_801C6050[n];
    }
    lbl_80281D0C[n] = uSize;
    if (lbl_80281D04[n] == 0) {
        lbl_80281D04[n] = fn_800B6564(lbl_80281D0C[n]);
    }
    fn_800B6844(p, lbl_80281D04[n], lbl_80281D0C[n]);
    fn_800B67EC();
    return p;
}

// Brings slot n's scratch area back from ARAM and frees the ARAM.
void Skalib_ScratchFromAram(int n) {
    u32 uSize = fn_8009EF90();
    fn_800B68B4(Skalib_Scratch(n, uSize), lbl_80281D04[n], lbl_80281D0C[n]);
    fn_800B67EC();
    if (lbl_80281D04[n] != 0) {
        fn_800B6594(lbl_80281D04[n]);
        lbl_80281D04[n] = 0;
    }
}

// Copies the clips an overlay library adds into its slot's clip bank (header, keys and curves in
// main memory, the per-frame data streamed out to ARAM), then, for a golfer's own overlay
// (nSlot >= 3: the id the overlay was loaded under), builds the merged library into the
// golfer's character object. Returns 0x2800 for a golfer's overlay, else 0.
s32 AnimLib_MergeOverlay(u8* pData, int nSlot) {
    u8*         pClipSrc;
    LibSlot*    pSlot;
    LibOverlay* pOv;
    ClipBank*   pBank;
    AnimLib*    pSrc;
    u8*         pEnd;
    u32         uAram;
    s32         nStride1;
    s32         nStride2;
    u8*         pSrc1;
    u8*         pSrc2;
    int         f;
    s32         nCopied;
    s32         n4C;
    s32         n50;
    s32         nHdr;
    u32         uPad;
    s32         n;
    u8*         pOut;
    s32         n50Al;
    AnimLib*    pNew;
    Character*  pChar;
    s32         nSize;
    Clip*       pHdr;
    ClipRecord* pRec;
    s32*        pUsed;
    int         i;
    s32         nRet = 0;
    AnimLib*    pLibFile;
    u32         k;
    u32         uAramStart;
    s32         n4CAl;
    BuildCtx    ctx;
    u32         o;
    int         j;
    int         p;
    u8          bFound;
    u32         s;
    Player*     pPlayer;

    if (nSlot < 3) {
        k        = nSlot;
        pSlot    = &lbl_801C6068[nSlot];
        pLibFile = pSlot->pLib;
        pSrc     = pLibFile;
    } else {
        for (k = 0; k < 3; k++) {
            pSlot = &lbl_801C6068[k];
            for (j = 0; j < pSlot->nOverlays; j++) {
                pOv = &pSlot->overlays[j];
                if (pOv->n10 == nSlot) {
                    k       = pOv->pChar->nSlot;
                    pOv->n10 = -1;
                    // fake match: the search's exit as a jump (not tested without it)
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

        pBank = lbl_801C6050[k];
        pUsed = &lbl_801C6008[k].n04;
        pEnd  = pSlot->pEnd;
        for (i = 0; i < pSrc->nRecords; i++) {
            pRec = &pSrc->pRecords[i];
            if (pRec->n10 <= 0 || pRec->n18 == 0) continue;
            pBank->ppClips[pSlot->n150] = pEnd;
            fn_80020BC8(pData + (uptr)pSrc->pRecords[i].pClip);
            pClipSrc = pData + (uptr)pSrc->pRecords[i].pClip;
            pHdr     = (Clip*)pEnd;
            nHdr     = ((Clip*)pClipSrc)->pD0 - pClipSrc;
            Mem_cpy(pEnd, pClipSrc, nHdr);
            nCopied = nHdr;
            pOut    = pEnd + nHdr;
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
            nStride1 = pHdr->n8C * 2;
            if (nStride1 & 31) {
                nStride1 = ((nStride1 >> 5) + 1) << 5;
            }
            nStride2 = pHdr->n8E;
            if (nStride2 & 31) {
                nStride2 = ((nStride2 >> 5) + 1) << 5;
            }
            pSrc1      = (u8*)((Clip*)pClipSrc)->uAram;
            pSrc2      = ((Clip*)pClipSrc)->pE4;
            pHdr->n38  = nStride1 * pHdr->nFrames;
            pHdr->n04  = nStride2 * pHdr->nFrames;
            n4C        = pHdr->n4C;
            n4CAl      = n4C;
            if (n4C & 31) {
                n4CAl = ((n4C >> 5) + 1) << 5;
            }
            n50   = pHdr->n50;
            n50Al = n50;
            if (n50 & 31) {
                n50Al = ((n50 >> 5) + 1) << 5;
            }
            uAram      = fn_800B6564(pHdr->n38 + pHdr->n04 + n50Al + n4CAl);
            uAramStart = uAram;
            if (pHdr->n38 != 0) {
                for (f = 0; f < pHdr->nFrames; f++) {
                    Mem_cpy(lbl_80281CC8, pSrc1, pHdr->n8C * 2);
                    fn_800B6844(lbl_80281CC8, uAram, nStride1);
                    fn_800B67EC();
                    uAram += nStride1;
                    pSrc1 += pHdr->n8C * 2;
                }
            }
            if (pHdr->n04 != 0) {
                for (f = 0; f < pHdr->nFrames; f++) {
                    Mem_cpy(lbl_80281CC4, pSrc2, pHdr->n8E);
                    fn_800B6844(lbl_80281CC4, uAram, nStride2);
                    fn_800B67EC();
                    uAram += nStride2;
                    pSrc2 += pHdr->n8E;
                }
            }
            pHdr->n8C = nStride1 / 2;
            pHdr->n8E = nStride2;
            if (n50 != 0) {
                Mem_cpy(lbl_80281CCC, ((Clip*)pClipSrc)->pEC, pHdr->n50);
                fn_800B6844(lbl_80281CCC, uAram, n50Al);
                fn_800B67EC();
                uAram += n50Al;
            }
            pHdr->n50 = n50Al;
            if (n4C != 0) {
                Mem_cpy(lbl_80281CD0, ((Clip*)pClipSrc)->pF0, pHdr->n4C);
                fn_800B6844(lbl_80281CD0, uAram, n4CAl);
                fn_800B67EC();
            }
            pHdr->n4C = n4CAl;
            fn_80020F60(pHdr, uAramStart);
            pBank->uId += nCopied;
            pEnd += nCopied;
            *pUsed += nCopied;
            pSrc->pRecords[i].pClip = pBank->ppClips[pSlot->n150];
            pSrc->pRecords[i].n12 |= 8;
            pSlot->n150++;
        }
        pSlot->pEnd = pEnd;
        if (nSlot >= 3) {
            nRet  = 0x2800;
            pChar = pOv->pChar;
            pNew  = pChar->pLib;
            nSize = pSrc->nTreeSize + pSrc->nClips * 4 + sizeof(AnimLib);
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
            if (pChar->pRecords == NULL) {
                pChar->pRecords = fn_80009B34(pSrc->nClips * sizeof(ClipRecord), 2, 0, "skalib.c", 2943);
            }
            ctx.pLib     = pNew;
            ctx.pRecords = pChar->pRecords;
            AnimLib_WalkPair(pLibFile, pSrc, (AnimLibWalkFn)AnimLib_BuildCb, &ctx);
            if (pNew->nTreeSize & 15) {
                pNew->nTreeSize = ((pNew->nTreeSize >> 4) + 1) << 4;
            }
            pChar->pLib = pNew;
        } else {
            for (p = 0; p < gSession.nNumPlayers; p++) {
                bFound = 0;
                for (s = 0; s < 3; s++) {
                    if (bFound) break;
                    for (o = 0; o < lbl_801C6068[k].nOverlays; o++) {
                        if (lbl_801C6068[k].overlays[o].pChar == gPlayers[p].pChar) {
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

// Frees the working copies of every slot's libraries (only the current slot's while
// lbl_80281CE4 is set).
void AnimLib_FreeWorkCopies(void) {
    LibSlot*    pSlot;
    int         j;
    int         k;
    u32         i;
    LibOverlay* pOv;

    pSlot = lbl_801C6068;
    for (i = 0; i < 3; i++, pSlot++) {
        if (lbl_80281CE4 != 0 && i != lbl_80281078) continue;
        if (pSlot->nOverlays != 0) {
            for (k = 0; pSlot->nOverlays > k; k++) {
            }
            for (j = 0; j < pSlot->nOverlays; j++) {
                pOv = &pSlot->overlays[j];
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

// Frees the pristine copies of every slot's libraries.
void AnimLib_FreeCopies(void) {
    LibSlot*    pSlot;
    LibOverlay* pOv;
    u32         i;
    int         j;

    pSlot = lbl_801C6068;
    for (i = 0; i < 3; i++, pSlot++) {
        if (pSlot->nOverlays != 0) {
            for (j = 0; j < pSlot->nOverlays; j++) {
                pOv = &pSlot->overlays[j];
                fn_80009E70(pOv->pCopy);
                pOv->pCopy = NULL;
            }
        }
        pSlot->nOverlays = 0;
        if (pSlot->pCopy != NULL) {
            fn_80009E70(pSlot->pCopy);
        }
        pSlot->pCopy = NULL;
    }
}

// Re-applies a slot's active overlay libraries.
void AnimLib_ApplyOverlays(int nSlot) {
    LibSlot*    pSlot = &lbl_801C6068[nSlot];
    LibOverlay* pOv;
    int         n     = pSlot->nOverlays;
    int         i;

    if (n != 0) {
        pOv = pSlot->overlays;
        for (i = 0; i < n; pOv++, i++) {
            if (pOv->bActive) {
                fn_800269E4(pOv, nSlot, pOv->pChar->nPlayer);
            }
        }
    }
}

void fn_80025478(void) {
    u32 i;
    int nTotal = 0;
    Skalib_SetBudgets();
    for (i = 0; i < 3; i++) {
        nTotal += AnimLib_PlanBank(i);
    }
}

// Rebuilds the current slot's libraries from their pristine copies (they are swapped in place
// when loaded, so a reload starts from the copy), first freeing the bank clips kept in ARAM.
void AnimLib_ReloadSlot(void) {
    u32         nSlot = Skalib_NextSlot();
    u32         i;
    LibSlot*    pSlot;
    int         j;
    LibOverlay* pOv;
    Clip*       pClip;
    AnimLib*    pLib;

    for (i = 0; i < lbl_801C6050[nSlot]->nClips; i++) {
        pClip = lbl_801C6050[nSlot]->ppClips[i];
        if (pClip != NULL && (pClip->uFlags & 4)) {
            fn_800B6594(pClip->uAram);
        }
    }
    pSlot = &lbl_801C6068[nSlot];
    if (pSlot->nOverlays != 0) {
        pSlot->pLib = fn_80009B34(pSlot->nSize, 1, 0x40, "skalib.c", 3193);
        Mem_cpy(pSlot->pLib, pSlot->pCopy, pSlot->nSize);
        pLib = AnimLib_Load((u8*)pSlot->pLib, ClipBank_Get(nSlot));
        pLib->pFile = pSlot->pLib;
        for (j = 0; j < pSlot->nOverlays; j++) {
            pOv        = &pSlot->overlays[j];
            pOv->pWork = fn_80009B34(pOv->nSize, 1, 0x40, "skalib.c", 3207);
            Mem_cpy(pOv->pWork, pOv->pCopy, pOv->nSize);
            AnimLib_Load((u8*)pOv->pWork, ClipBank_Get(nSlot));
            pOv->n10 = pOv->n14 + 3;
        }
    }
    AnimLib_ApplyOverlays(nSlot);
    fn_800CA9DC(nSlot);
    AnimLib_PlanBank(nSlot);
}

// The clips for an animation group, style, club class and key: each level falls back to its
// default (flag 1 when the group, style or club level did, flag 2 when only the key did), and
// the library default is the last resort. Returns the leaf's clip pointers.
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
            // fake match: the search's exit as a jump (not tested without it)
            goto leaf;
        }
        pNode = (s16*)(pLib->pTree + nOff);
        nOff  = pNode[1 + nStyle];
        if (nOff < 0 && nStyle != 0) {
            nOff = pNode[1];
        }
        if (nOff < 0) {
            *pFlags |= 1;
            nOff = pNode[0];
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            nOff = pLib->nDefault;
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            return NULL;
        }
        nOff = *(s16*)(pLib->pTree + nOff + nClub * 2);
        if (nOff < 0) {
            *pFlags |= 1;
            nOff = pNode[0];
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            nOff = pLib->nDefault;
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            return NULL;
        }
        pClub = (s16*)(pLib->pTree + nOff);
        nOff  = pClub[2 + nKey];
        if (nOff < 0) {
            *pFlags |= 2;
            nOff = pClub[1];
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            nOff = pNode[0];
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            nOff = pLib->nDefault;
            if (nOff >= 0) {
                // fake match: the search's exit as a jump (not tested without it)
                goto leaf;
            }
            return NULL;
        }
leaf:
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
    *ppSlot = lbl_80281D14[nPlayer][nKind][nStyle][nClub];
    return strcmp(pName, lbl_80281D14[nPlayer][nKind][nStyle][nClub]) == 0;
}

int   AnimLib_RandomIndex(u32 uUsed, int nCount);
void* fn_800CAA7C(int nPlayer, int nGroup, int nStyle, int nClub);

// The clip a player plays for a group and style. A named clip (lessons) is looked up by name.
// Otherwise one of the leaf's clips at random, never the reaction played last time, and - through
// the leaf's played mask - none again until all of them have been played.
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
    if (fn_800C9828(nGroup, nStyle, nClub, (*pFlags & 2) ? -1 : nKey)) {
        pClip = fn_800CAA7C(nPlayer, nGroup, nStyle, nClub);
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
                        strcpy(pSlot, (char*)ppClips[nPick] + 0xA0);
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
        nPick = Rand_Next(1) % nCount;
        if (!((1 << nPick) & uUsed)) break;
    } while (++nTries < 3);
    return nPick;
}

// Swaps a clip bank's header.
void ClipBank_SwapHeader(void* p) {
    SwapField fmt[5] = {{8, -8}, {4, 4}, {4, 4}, {4, 4}, {4, 4}};
    void*     pSrc = p;
    void*     pDst = p;
    fn_8001F08C(&pSrc, &pDst, fmt, 5, 1);
}

// Swap one node of the clip tree (see AnimLib).
void AnimLib_SwapGroupNode(void* pSrc, void* pDst) {
    SwapField fmt[3] = {{2, 2}, {0x10, 2}, {2, 1}};
    fn_8001F08C(&pSrc, &pDst, fmt, 3, 1);
}

void AnimLib_SwapStyleNode(void* pSrc, void* pDst) {
    SwapField fmt[2] = {{12, 2}, {2, 1}};
    fn_8001F08C(&pSrc, &pDst, fmt, 2, 1);
}

void AnimLib_SwapClubNode(void* pSrc, void* pDst) {
    SwapField fmt[5] = {{2, 2}, {2, 2}, {0x16, 2}, {2, 2}, {4, 4}};
    fn_8001F08C(&pSrc, &pDst, fmt, 5, 1);
}

void AnimLib_SwapLeaf(void* pSrc, void* pDst) {
    SwapField fmt[3] = {{2, 2}, {2, 2}, {4, 4}};
    fn_8001F08C(&pSrc, &pDst, fmt, 3, 1);
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

// Sets up an animation library loaded at pData (swapping it in place). Its clips come either from
// the clip bank pBank, or (flag 1) from the library itself; a library whose id does not match the
// bank's plays the bank's first clip for everything. NULL when it needs a bank and there is none.
AnimLib* AnimLib_Load(u8* pData, ClipBank* pBank) {

    SwapField hdrFmt[19] = {{0x100, 4}, {8, -8}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4},
                            {4, 4},     {4, -4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {4, 4}, {2, 2},
                            {2, 2}};
    SwapField recFmt[7]  = {{0x10, -1}, {2, 2}, {2, 2}, {4, 4}, {4, 4}, {4, 4}, {4, 4}};
    void*     pDst;
    void*     pSrc;
    u32       uPad;
    AnimLib*  pLib;
    u8*       p;
    int       i;

    uPad = 16 - ((uptr)pData & 15);
    if (uPad == 16) {
        uPad = 0;
    }
    pLib        = (AnimLib*)(pData + uPad);
    pLib->pFile = pData;
    p           = (u8*)pLib + sizeof(AnimLib);
    pDst = pSrc = pLib;
    fn_8001F08C(&pSrc, &pDst, hdrFmt, 19, 1);
    if (pLib->pBank != NULL) {
        if (pBank == NULL) return NULL;
        pLib->pClipData = NULL;
        pLib->ppClips   = (void**)p;
        p += pLib->nClips * 4;
        pLib->nClips2   = pLib->nClips;
        pLib->pTree = p;
        pDst = pSrc = pLib->ppClips;
        fn_80076158(&pSrc, pDst, pLib->nClips * 4, 4);
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
        pLib->pIndex  = (s16*)p;
        p += pLib->nClips * 2;
        pLib->nClips2 = pLib->nClips;
        if ((uptr)p & 15) {
            p = (u8*)((((uptr)p >> 4) + 1) << 4);
        }
        pLib->pRecords = (ClipRecord*)p;
        p += pLib->nRecords * sizeof(ClipRecord);
        pLib->pTree = p;
        p += pLib->nTreeSize;
        pDst = pSrc = pLib->pIndex;
        fn_80076158(&pSrc, pDst, pLib->nClips * 2, 2);
        pDst = pSrc = pLib->pRecords;
        fn_8001F08C(&pSrc, &pDst, recFmt, 7, pLib->nRecords);
        if (pLib->uFlags & 1) {
            if ((uptr)p & 15) {
                p = (u8*)((((uptr)p >> 4) + 1) << 4);
            }
            pLib->pClipData = p;
            pLib->ppClips   = fn_80009B34(pLib->nClips * 4, 2, 0x40, "skalib.c", 4164);
            for (i = 0; i < pLib->nRecords; i++) {
                pLib->pRecords[i].pClip =
                    fn_80020DD4(pLib->pClipData + (uptr)pLib->pRecords[i].pClip, NULL, 16);
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

// Sets up a clip bank loaded at pData, aligned to uAlign.
ClipBank* ClipBank_Load(u8* pFile, u32 uAlign) {
    u32       uUnused;
    void*     pSrc;
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
    ClipBank_SwapHeader(pBank);
    pBank->pFile   = NULL;
    pData += 0x20;
    pBank->ppClips = (void**)pData;
    pSrc = pBank->ppClips;
    fn_80076158(&pSrc, pBank->ppClips, pBank->nClips * 4, 4);
    pData += pBank->nClips * 4;
    uPad = 16 - ((uptr)pData & 15);
    if (uPad == 16) {
        uPad = 0;
    }
    pData += uPad;
    for (i = 0; i < pBank->nClips; i++) {
        pBank->ppClips[i] = fn_80020DD4(pData + (uptr)pBank->ppClips[i], &uUnused, 16);
    }
    return pBank;
}

// A slot's animation library has loaded: keep a copy and set it up against the slot's bank.
void AnimLib_OnLoaded(UStreamObject* pFile) {
    u8       bFree = 1;
    u32      nSlot = pFile->uId;
    AnimLib* pLib;

    if (nSlot < 3 && lbl_801C605C[nSlot] == NULL) {
        lbl_801C6068[nSlot].pCopy = fn_80009B34(pFile->uSize, 2, 0x40, "skalib.c", 4520);
        Mem_cpy(lbl_801C6068[nSlot].pCopy, pFile->pData, pFile->uSize);
        lbl_801C6068[nSlot].nSize = pFile->uSize;
        pLib = AnimLib_Load(pFile->pData, ClipBank_Get(nSlot));
        pLib->pFile = pFile;
        if (pLib->pBank != NULL) {
            lbl_801C605C[nSlot] = pLib;
        } else {
            lbl_801C6068[nSlot].pLib = pLib;
        }
        bFree = 0;
    }
    if (bFree) {
        fn_80009E70(pFile);
    }
}

void ClipBank_Stash(int nSlot);

// A slot's clip bank file has loaded: park it in ARAM.
void ClipBank_OnLoaded(UStreamObject* pFile) {
    u32 nSlot = pFile->uId;
    lbl_801C6488[nSlot] = pFile;
    ClipBank_Stash(nSlot);
}

// Makes a loaded clip bank file the slot's bank.
void ClipBank_Install(UStreamObject* pFile) {
    u8  bFree = 1;
    u32 nSlot = pFile->uId;

    if (nSlot < 3 && lbl_801C6050[nSlot] == NULL) {
        lbl_801C6050[nSlot]        = ClipBank_Load(pFile->pData, 16);
        bFree                      = 0;
        lbl_801C6050[nSlot]->pFile = pFile;
    }
    if (bFree) {
        fn_80009E70(pFile);
    }
}

// Forgets a slot's bank.
void ClipBank_Release(int nSlot) {
    if (lbl_801C6050[nSlot] != NULL) {
        if (lbl_801C6050[nSlot]->pFile != NULL) {
            lbl_801C6050[nSlot]->pFile = NULL;
            lbl_801C6050[nSlot]        = NULL;
            lbl_801C6488[nSlot]        = NULL;
        } else {
            lbl_801C6050[nSlot] = NULL;
        }
    }
    if (lbl_801C6488[nSlot] != NULL) {
        lbl_801C6488[nSlot] = NULL;
    }
}

// Copies a slot's bank file to ARAM and frees it; the first time, allocates the buffer it is
// brought back into.
void ClipBank_Stash(int nSlot) {
    if (lbl_801C6488[nSlot] != NULL) {
        lbl_801C647C[nSlot] = ((lbl_801C6488[nSlot]->uSize + 0x80) / 32 + 1) * 32;
        if (lbl_801C6470[nSlot] == 0) {
            lbl_801C6470[nSlot] = fn_800B6564(lbl_801C647C[nSlot]);
        }
        fn_800B6844(lbl_801C6488[nSlot], lbl_801C6470[nSlot], lbl_801C647C[nSlot]);
        fn_800B67EC();
        if (lbl_801C6488[nSlot] != lbl_80281CE0) {
            fn_80009E70(lbl_801C6488[nSlot]);
        }
        lbl_801C6488[nSlot] = NULL;
        if (nSlot == 0 && lbl_80281CE0 == NULL) {
            lbl_80281CE0 = fn_80009B34(lbl_801C647C[nSlot], 2, 0x20, "skalib.c", 4671);
        }
    }
}

// Brings a slot's bank back from ARAM and installs it.
void ClipBank_Restore(int nSlot) {
    if (lbl_801C6488[nSlot] == NULL) {
        lbl_801C6488[nSlot] = lbl_80281CE0;
        fn_800B68B4(lbl_801C6488[nSlot], lbl_801C6470[nSlot], lbl_801C647C[nSlot]);
        fn_800B67EC();
        lbl_801C6488[nSlot]->pData = (u8*)lbl_801C6488[nSlot] + 0x80;
        ClipBank_Install(lbl_801C6488[nSlot]);
    }
}

// Frees the banks' ARAM and the restore buffer.
void ClipBank_FreeAram(void) {
    int i;
    for (i = 0; i < 3; i++) {
        if (lbl_801C6470[i] != 0) {
            fn_800B6594(lbl_801C6470[i]);
            lbl_801C6470[i] = 0;
        }
    }
    if (lbl_80281CE0 != NULL) {
        fn_80009E70(lbl_80281CE0);
        lbl_80281CE0 = NULL;
    }
}

// Hooks the loaders up to the file streamer: 'SAL ' animation libraries and 'BNK ' clip banks.
// A stream object's uId says which of the three animation slots it is for.
void Skalib_Register(void) {
    UStream_RegisterHandler('SAL ', AnimLib_OnLoaded);
    UStream_RegisterHandler('BNK ', ClipBank_OnLoaded);
}

void Skalib_Unregister(void) {
    UStream_UnregisterHandler('SAL ');
    UStream_UnregisterHandler('BNK ');
}

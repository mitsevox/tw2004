// mtalib.c (EA's name, from its asserts; also in EA's 2002 source tree): animation helpers, and
// the 'MAL ' banks loaded from the stream files (two slots, lbl_80281CB4): groups of items that the
// animation code picks from at random.

#include "charstate.h"

static MalBank* lbl_80281CB4[2];
static int lbl_80281CB0;                // bytes the banks have allocated
static u8 lbl_801B9668[200];

// The bone names by bone id (character.h): 0x15 "rwrst", 0x36-0x3A the right leg ("rhip" to "rtoe"),
// 0x44-0x48 the left leg, 0x52 "IGdriver" (the club bone), 0x53 "clubhead".
char* lbl_80187278[90] = {
    "", "root", "ctrgrav", "waist", "s1", "s2", "s3", "s4", "s5", "neck", "head", "tail1", "tail2",
    "tail3", "tail4", "skull", "rcolr", "rshld", "rbictwst", "relb", "r4rm", "rwrst", "rf1", "rf2",
    "rf3", "ri1", "ri2", "ri3", "rt1", "rt2", "rt3", "rslvBjnt", "rslvFjnt", "rslvHjnt", "rshlddef",
    "lcolr", "lshld", "lbictwst", "lelb", "l4rm", "lwrst", "lf1", "lf2", "lf3", "li1", "li2", "li3",
    "lt1", "lt2", "lt3", "lslvBjnt", "lslvFjnt", "lslvHjnt", "lshlddef", "rhip", "rthitwst",
    "rknee", "rankl", "rtoe", "ropnt1", "ropnt2", "ropnt3", "rbpnt1", "rbpnt2", "rbpnt3", "rfpnt1",
    "rfpnt2", "rfpnt3", "lhip", "lthitwst", "lknee", "lankl", "ltoe", "lopnt1", "lopnt2", "lopnt3",
    "lbpnt1", "lbpnt2", "lbpnt3", "lfpnt1", "lfpnt2", "lfpnt3", "IGdriver", "clubhead", "GBall1",
    "tail5", "tail6", "rboob", "lboob", NULL,
};

void fn_8001F6D8(MalBank* pBank);

// The entry's values at the two frames around fTime (clamped to its last frame) into *pfA and *pfB;
// returns how far fTime is from the first frame to the second (0 when clamped).
f32 fn_8001F32C(MtaEntry* pEntry, f32* pfA, f32* pfB, f32 fTime) {
    f32 fFrame;
    int nLast;
    int nA;
    int nB;
    f32 fT;
    f32 fRange;

    if (fTime < 0.0f) {
        fTime = 0.0f;
    }
    fFrame = fTime / pEntry->fFrameTime;
    nLast = pEntry->nLastFrame;
    nA = (int)fFrame;
    nB = nA + 1;
    fT = fFrame - (f32)nA;
    if (nA > nLast) {
        fT = 0.0f;
        nB = nLast;
        nA = nLast;
    } else if (nB > nLast) {
        nB = nLast;
        fT = 0.0f;
    }
    fRange = pEntry->fHi - pEntry->fLo;
    if (fRange != 0.0f) {
        *pfA = fRange * (pEntry->pData[nA] / 256.0f) + pEntry->fLo;
        *pfB = fRange * (pEntry->pData[nB] / 256.0f) + pEntry->fLo;
    } else {
        *pfA = pEntry->fLo;
        *pfB = pEntry->fLo;
    }
    return fT;
}

// Sets morph nMorph's weight in pBlock from the entry at fTime and marks the morph set.
void fn_8001F42C(MtaEntry* pEntry, SkelPoseBlock* pBlock, int nMorph, SkelPose1* pPose, f32 fTime,
                 f32 fWeight) {
    f32 fA;
    f32 fB;
    f32 fT;

    // pPose and fWeight are unused: fn_8001F494 passes them
    fT = fn_8001F32C(pEntry, &fA, &fB, fTime);
    pBlock->af8[nMorph] = fT * (fB - fA) + fA;
    BitArray_SetBit(pBlock->aBits, nMorph);
}

// Sets every morph weight the library drives in pPose from its tracks at fTime.
int fn_8001F494(void* pUnused, MtaLib* pLib, SkelPose1* pPose, f32 fTime) {
    int i;
    int j;
    MtaRecord* pRecord;
    SkelPoseBlock* pBlock;

    // pUnused: the caller (animblender.c) passes it; nothing here reads it
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
        pBlock = &pPose->aBlocks[pRecord->nBlock];
        for (j = 0; j < pRecord->nEntries; j++) {
            if (pRecord->pEntries[j].nMorph >= 0) {
                fn_8001F42C(&pRecord->pEntries[j], pBlock, pRecord->pEntries[j].nMorph, pPose, fTime, 1.0f);
            }
        }
    }
    return 0;
}

void fn_8001F558(void* pItem) {
    StaticMem_Free(pItem);
}

// Links a library that is already in the machine's byte order (fn_8001F110 without the swap): the
// records after the header, each record's entries after those, then each entry's data.
void fn_8001F578(MtaLib* pLib) {
    int i;
    int nPad;
    MtaRecord* pRecord;
    MtaEntry* pEntry;
    int nOffset;
    int j;
    MtaRecord* pRecord2;

    pLib->pRecords = (MtaRecord*)(pLib + 1);
    nOffset = sizeof(MtaLib) + pLib->nRecords * sizeof(MtaRecord);
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord = &pLib->pRecords[i];
        pRecord->pEntries = (MtaEntry*)((u8*)pLib + nOffset);
        nOffset += pRecord->nEntries * sizeof(MtaEntry);
    }
    for (i = 0; i < pLib->nRecords; i++) {
        pRecord2 = &pLib->pRecords[i];
        for (j = 0; j < pRecord2->nEntries; j++) {
            pEntry = &pRecord2->pEntries[j];
            pEntry->pData = (u8*)pLib + nOffset;
            nOffset += pEntry->nBytes;
            nPad = nOffset % 4;
            if (nPad != 0) {
                nOffset += 4 - nPad;
            }
        }
    }
}

void fn_8001F64C(void) {
    lbl_80281CB4[0] = NULL;
    lbl_80281CB4[1] = NULL;
    lbl_801B9668[0] = 0;
    lbl_80281CB0 = 0;
}

// Frees both banks.
void fn_8001F66C(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (lbl_80281CB4[i] != NULL) {
            fn_8001F6D8(lbl_80281CB4[i]);
            lbl_80281CB4[i] = NULL;
        }
    }
    lbl_80281CB0 = 0;
}

void fn_8001F6D8(MalBank* pBank) {
    int i;
    int j;

    for (i = 0; i < 3; i++) {
        if (pBank->aGroup[i].nNum != 0) {
            for (j = 0; j < pBank->aGroup[i].nNum; j++) {
                fn_8001F558(pBank->aGroup[i].apItem[j]);
            }
            StaticMem_Free(pBank->aGroup[i].apItem);
        }
    }
    StaticMem_Free(pBank);
}

// EA bug: nBank is only range-checked; bank 0 is returned either way.
MalBank* fn_8001F760(int nBank) {
    if (nBank < 0 || nBank >= 2) return NULL;
    return lbl_80281CB4[0];
}

void** fn_8001F780(MalBank* pBank, int nGroup, int* pnNum, int n) {
    MalGroup* pGroup;

    pGroup = &pBank->aGroup[nGroup];
    *pnNum = pGroup->nNum;
    return pGroup->apItem;
}

// A random item of the group, or NULL if it is empty.
void* fn_8001F79C(MalBank* pBank, int nGroup, int n) {
    int nNum;
    void** apItem;

    apItem = fn_8001F780(pBank, nGroup, &nNum, n);
    if (nNum != 0) {
        return apItem[Misc_RandFunc(1) % nNum];
    }
    return NULL;
}

// Builds a bank from a 'MAL ' object's data (little-endian): its group count, then per group its
// index, its item count and its items, each a library (MtaLib) starting on a 16-byte boundary.
// Each library is copied out, byte-swapped and linked.
MalBank* fn_8001F804(u8* pData) {
    SwapField aHeader[10] = {
        { 16, 1 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 6, -1 },
        { 4, 4 },
    };
    s32 nSize;
    s32 nGroup;
    void* pA;
    void* pB;
    int i;
    int j;
    MalGroup* pGroup;
    MalBank* pBank;
    MtaLib* pLib;

    pBank = StaticMem_Alloc(sizeof(MalBank), 2, 0x40, "mtalib.c", 474);
    lbl_80281CB0 += sizeof(MalBank);
    BYTESWAP_SWAPDATA(&pData, (u8*)&pBank->nNumGroups, 4, 4);
    if ((uptr)pData & 0xF) {
        pData = (u8*)(((uptr)pData & ~0xF) + 0x10);
    }
    for (i = 0; i < pBank->nNumGroups; i++) {
        BYTESWAP_SWAPDATA(&pData, (u8*)&nGroup, 4, 4);
        pGroup = &pBank->aGroup[nGroup];
        BYTESWAP_SWAPDATA(&pData, (u8*)&pGroup->nNum, 4, 4);
        if (pGroup->nNum != 0) {
            pGroup->apItem = StaticMem_Alloc(pGroup->nNum * 4, 2, 0x40, "mtalib.c", 490);
            for (j = 0; j < pGroup->nNum; j++) {
                if ((uptr)pData & 0xF) {
                    pData = (u8*)(((uptr)pData & ~0xF) + 0x10);
                }
                // swap the header in place to read the library's size, then swap it back
                pB = pA = pData;
                ByteSwap_Records(&pA, &pB, aHeader, 10, 1);
                pLib = (MtaLib*)pData;
                pGroup->apItem[j] = StaticMem_Alloc(pLib->nBytes, 2, 0x40, "mtalib.c", 501);
                lbl_80281CB0 += pLib->nBytes;
                nSize = pLib->nBytes;
                pB = pA = pData;
                ByteSwap_Records(&pA, &pB, aHeader, 10, 1);
                memcpy(pGroup->apItem[j], pData, nSize);
                pGroup->apItem[j] = fn_8001F110(pGroup->apItem[j], &nSize);
                pData += nSize;
            }
        } else {
            pGroup->apItem = NULL;
        }
    }
    return pBank;
}

// The 'MAL ' stream handler: the object's id is the bank slot; a slot already filled is kept.
void fn_8001FA3C(UStreamObject* pObject) {
    u32 uSlot;

    uSlot = pObject->uId;
    if (uSlot < 2 && lbl_80281CB4[uSlot] == NULL) {
        lbl_80281CB4[uSlot] = fn_8001F804(pObject->pData);
    }
    StaticMem_Free(pObject);
}

void fn_8001FAA8(void) {
    Stream_RegisterLoadChunkCallback('MAL ', fn_8001FA3C);
}

void fn_8001FAD8(void) {
    Stream_UnregisterLoadChunkCallback('MAL ');
}

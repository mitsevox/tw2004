// LLTexGrp.c (EA's name, from its asserts; also in EA's 2002 source tree): the list of loaded
// texture banks. Each 'txf ' stream object becomes a bank in a free slot of the list, and a
// texture is found by its name's hash in every bank of the list (TG_iFindTextureGroupAndTexture).

#include "engine.h"

static TexGrpList lbl_801A3458;
static TexGrpList* lbl_80280DD8 = &lbl_801A3458;

void fn_80010028(TexBank* pBank);                  // LLTex.c: free a bank

int  fn_800106F0(TexBank* pBank);
void fn_800106B8(u8 b);
void fn_800106D8(int nSlot, int n);
void fn_80010754(u8 b);
int  fn_80010760(u32 uId, int nAfter);

// The last free slot, or -1 if the list is full.
int fn_8001013C(void) {
    TexGrpSlot* aSlots;
    int i;

    aSlots = lbl_80280DD8->pSlots;
    for (i = lbl_80280DD8->nNumSlots - 1; i >= 0; i--) {
        if (aSlots[i].pBank == NULL) return i;
    }
    return -1;
}

// The 'txf ' stream handler: loads the object's bank into the list (once for each record that
// asks for it, when records are in use), then frees the object.
void fn_80010180(TStreamData* pObject) {
    TexGrpRec* pRec;
    int nC;
    int nRec;
    u8 bMore;
    u8 bSkip;
    int nSlot;

    nRec = -1;
    do {
        bMore = 0;
        nC = 0;
        bSkip = 0;
        pRec = NULL;
        if (lbl_80280DD8->bUseRecs && pObject->uId % 100000 >= 20000) {
            nRec = fn_80010760(pObject->uId % 100000, nRec);
            if (nRec != -1) {
                bMore = 1;
                pRec = &lbl_80280DD8->pRecs[nRec];
                pRec->bUsed = 1;
                nC = pRec->nC;
                bSkip = pRec->b0;
            } else {
                bSkip = 1;
            }
        }
        if (!bSkip) {
            nSlot = fn_800106F0(TX_spDownloadData(pObject, NULL, nC));
            if (pRec != NULL) {
                pRec->nSlot = nSlot;
                fn_800106D8(nSlot, pRec->n8);
            }
        }
    } while (bMore);
    Circular_MemFreeFunc(pObject);
}

// Has the stream loader hand every 'txf ' object (a texture bank) to this file's handler, which
// puts it in the bank list.
void TG_vRegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('txf ', fn_80010180);
}

// Stops the stream loader handing 'txf ' objects to the bank list.
void TG_vUnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('txf ');
}

int TG_iFindTextureGroupAndTexture(u64 uHash, TexBank** ppBank, TexEntry** ppTex) {
    int nTex;
    TexGrpSlot* pSlot;
    TexGrpSlot* aSlots;
    int i;

    nTex = TX_NO_TEXTURE_INDEX;
    i = 0;
    aSlots = lbl_80280DD8->pSlots;
    for (pSlot = aSlots; i < lbl_80280DD8->nNumSlots; pSlot++, i++) {
        if (pSlot->pBank == NULL) continue;
        if (lbl_80280DD8->b14 && lbl_80280DD8->n10 != pSlot->n8) continue;
        nTex = TX_uiFindTextureIndexFromID(pSlot->pBank, uHash);
        if (nTex != TX_NO_TEXTURE_INDEX) break;
    }
    if (nTex == TX_NO_TEXTURE_INDEX) {
        *ppBank = NULL;
        *ppTex = NULL;
        return TX_NO_TEXTURE_INDEX;
    }
    *ppBank = aSlots[i].pBank;
    *ppTex = TX_spGetTextureGroupTextureFromIndex(*ppBank, nTex);
    return i;
}

void fn_800103C0(void) {
    int i;

    lbl_80280DD8->nNumSlots = 100;
    lbl_80280DD8->pSlots =
        Circular_MemAllocFunc(lbl_80280DD8->nNumSlots * sizeof(TexGrpSlot), MEMTYPE_LOW, 16,
                              "LLTexGrp.c", 859);
    for (i = 0; i < lbl_80280DD8->nNumSlots; i++) {
        lbl_80280DD8->pSlots[i].pBank = NULL;
        lbl_80280DD8->pSlots[i].n8 = -1;
        lbl_80280DD8->pSlots[i].n4 = lbl_80280DD8->n8;
    }
    lbl_80280DD8->n8 = 0;
    lbl_80280DD8->n10 = -1;
    fn_800106B8(0);
    fn_80010754(0);
    lbl_80280DD8->nNumRecs = 0;
}

// Frees every bank and the list itself.
void fn_8001049C(void) {
    int i;

    for (i = 0; i < lbl_80280DD8->nNumSlots; i++) {
        if (lbl_80280DD8->pSlots[i].pBank != NULL) {
            fn_80010028(lbl_80280DD8->pSlots[i].pBank);
            lbl_80280DD8->pSlots[i].pBank = NULL;
        }
    }
    Circular_MemFreeFunc(lbl_80280DD8->pSlots);
    lbl_80280DD8->pSlots = NULL;
}

void fn_8001052C(int nSlot) {
    lbl_80280DD8->pSlots[nSlot].pBank = NULL;
}

void fn_80010544(int nSlot) {
    fn_80010028(lbl_80280DD8->pSlots[nSlot].pBank);
    lbl_80280DD8->pSlots[nSlot].pBank = NULL;
}

// Frees every bank.
void fn_8001058C(void) {
    int i;

    for (i = 0; i < lbl_80280DD8->nNumSlots; i++) {
        if (lbl_80280DD8->pSlots[i].pBank != NULL) {
            fn_80010028(lbl_80280DD8->pSlots[i].pBank);
            lbl_80280DD8->pSlots[i].pBank = NULL;
        }
    }
}

// Frees the banks that came in while the list's n8 was n.
void fn_80010608(int n) {
    int i;

    for (i = 0; i < lbl_80280DD8->nNumSlots; i++) {
        if (lbl_80280DD8->pSlots[i].pBank != NULL && n == lbl_80280DD8->pSlots[i].n4) {
            fn_80010028(lbl_80280DD8->pSlots[i].pBank);
            lbl_80280DD8->pSlots[i].pBank = NULL;
        }
    }
}

void fn_800106A0(int n) {
    lbl_80280DD8->n8 = n;
}

void fn_800106AC(int n) {
    lbl_80280DD8->n10 = n;
}

void fn_800106B8(u8 b) {
    lbl_80280DD8->b14 = b;
}

TexBank* fn_800106C4(int nSlot) {
    return lbl_80280DD8->pSlots[nSlot].pBank;
}

void fn_800106D8(int nSlot, int n) {
    lbl_80280DD8->pSlots[nSlot].n8 = n;
}

// Puts a bank in the last free slot; returns the slot.
int fn_800106F0(TexBank* pBank) {
    int nSlot;

    nSlot = fn_8001013C();
    lbl_80280DD8->pSlots[nSlot].pBank = pBank;
    lbl_80280DD8->pSlots[nSlot].n8 = lbl_80280DD8->n10;
    lbl_80280DD8->pSlots[nSlot].n4 = lbl_80280DD8->n8;
    return nSlot;
}

void fn_80010754(u8 b) {
    lbl_80280DD8->bUseRecs = b;
}

// The first unused record after nAfter for the object id uId, or -1.
int fn_80010760(u32 uId, int nAfter) {
    TexGrpRec* pRec;
    int i;

    pRec = lbl_80280DD8->pRecs + nAfter + 1;
    for (i = nAfter + 1; i < lbl_80280DD8->nNumRecs; i++) {
        if (!pRec->bUsed && pRec->uId == uId) return i;
        pRec++;
    }
    return -1;
}

int fn_800107C0(TStreamData* pObject, TexBank* pBank, int n) {
    return fn_800106F0(TX_spDownloadData(pObject, pBank, n));
}

TexEntry* TX_spGetTextureGroupTextureFromIndex(TexBank* pBank, int nTex) {
    return &pBank->pTextures[nTex];
}

// mtalib.c (EA's name, from its asserts; also in EA's 2002 source tree): animation helpers, and
// the 'MAL ' banks loaded from the stream files (two slots, lbl_80281CB4): groups of items that the
// animation code picks from at random. Only part of it is decompiled.

#include "character.h"

static MalBank* lbl_80281CB4[2];
static int lbl_80281CB0;                // bytes the banks have allocated
static u8 lbl_801B9668[200];

MalBank* fn_8001F804(u8* pData);
void fn_8001F6D8(MalBank* pBank);

void fn_8001F558(void* pItem) {
    fn_80009E70(pItem);
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
            fn_80009E70(pBank->aGroup[i].apItem);
        }
    }
    fn_80009E70(pBank);
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
        return apItem[Rand_Next(1) % nNum];
    }
    return NULL;
}

// The 'MAL ' stream handler: the object's id is the bank slot; a slot already filled is kept.
void fn_8001FA3C(UStreamObject* pObject) {
    u32 uSlot;

    uSlot = pObject->uId;
    if (uSlot < 2 && lbl_80281CB4[uSlot] == NULL) {
        lbl_80281CB4[uSlot] = fn_8001F804(pObject->pData);
    }
    fn_80009E70(pObject);
}

void fn_8001FAA8(void) {
    UStream_RegisterHandler('MAL ', fn_8001FA3C);
}

void fn_8001FAD8(void) {
    UStream_UnregisterHandler('MAL ');
}

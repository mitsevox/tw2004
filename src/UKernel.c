// UKernel.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the kernel's list
// of the course's dynamic objects (dynobj.h), chained through DynObj.pNext from lbl_80281DBC to
// lbl_80281DB8.

#include "dynobj.h"
#include "terrain.h"
#include "psmgr.h"

DynObj* fn_80049018(DynObjSetup* pSetup);
void fn_8000E830(DynObj* pObj);
void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
void fn_8000C5A4(f32 (*pMtx)[4]);

void fn_800646D0(UStreamObject* pObject);
void fn_80064A0C(UStreamObject* pObject);
void fn_800A4CB8(UStreamObject* pObject, int n);
void fn_800EADDC(void* pObj);

void fn_80048B70(void* p);
void fn_80048BDC(UStreamObject* pObject);

// Memory for an object of nSize bytes: a node of the small or the large pool while one is free,
// else from the heap.
void* fn_80048AF4_DynObjAlloc(int nSize) {
    if (nSize < 400 && lbl_80281DAC->nFree != 0) {
        return AllocPoolMem(lbl_80281DAC);
    }
    if (nSize < 528 && lbl_80281DA8->nFree != 0) {
        return AllocPoolMem(lbl_80281DA8);
    }
    return fn_80009B34(nSize, 1, 16, "UKernel.c", 201);
}

// Gives an object's memory back to the pool it came from, or to the heap.
void fn_80048B70(void* p) {
    if ((u8*)p > (u8*)lbl_80281DAC && (u8*)p < lbl_80281DAC->pEnd) {
        ReturnPoolMem(lbl_80281DAC, p);
    } else if ((u8*)p > (u8*)lbl_80281DA8 && (u8*)p < lbl_80281DA8->pEnd) {
        ReturnPoolMem(lbl_80281DA8, p);
    } else {
        fn_80009E70(p);
    }
}

// The 'Cact' stream handler: an object's definition arrived. Some types are handed to their own
// systems (the tee and pin positions, types 7, 9, 10, 200 and 201; type 8 is dropped); the others
// get their stream objects looked up and become a dynamic object, and the stream object is freed.
void fn_80048BDC(UStreamObject* pObject) {
    DynObjSetup setup;
    TagRecord* pChunk;
    int i;

    setup.pDef = (DynObjDef*)((u8*)fn_8000B748(pObject->pData, pObject->uSize, 'tACT', pObject->uId) -
                              sizeof(u32));
    switch (setup.pDef->n4) {
    case 200:
        fn_800646D0(pObject);
        return;
    case 201:
        fn_80064A0C(pObject);
        return;
    case 5:
        fn_80034720(pObject);
        return;
    case 6:
        if (!fn_800347B4(pObject)) return;
        break;
    case 7:
        fn_8009943C((PsEmitterRecord*)pObject->pData, pObject->uSize);
        fn_80009E70(pObject);
        return;
    case 8:
        fn_80009E70(pObject);
        return;
    case 10:
        fn_800EADDC(pObject);
        return;
    case 9:
        fn_800A4CB8(pObject, 1);
        fn_80009E70(pObject);
        return;
    }
    pChunk = fn_8000B7B0(pObject->pData, pObject->uSize, 'aRSL', pObject->uId);
    if (pChunk != NULL) {
        setup.pModel = (DynObjModel*)&pChunk->uId;
        setup.pModel->nEntries = (pChunk->uSize - sizeof(TagRecord)) / sizeof(DynObjModelEntry);
        for (i = 0; i < setup.pModel->nEntries; i++) {
            if (setup.pModel->aEntries[i].u.nId != 0) {
                setup.pModel->aEntries[i].u.pRef = (DynObjModelRef*)fn_8000B70C(
                    setup.pModel->aEntries[i].uType, setup.pModel->aEntries[i].u.nId);
            }
        }
    } else {
        setup.pModel = NULL;
    }
    setup.pfnHandler = fn_800499B0(setup.pDef->n4);
    setup.pC = (DynObjNames*)pObject;
    pObject->pData = (u8*)setup.pModel;
    pObject->pfn8 = NULL;
    pObject->uUnk4 = fn_800490B8(&setup);
    fn_80009E70(pObject);
}

// Sets the kernel up: the 'Cact' stream handler, the two node pools and an empty list.
void fn_80048DD0(void) {
    Stream_RegisterLoadChunkCallback('Cact', fn_80048BDC);
    lbl_80281DAC = UMemPool_Create(256, 400, 2, 16);
    lbl_80281DA8 = UMemPool_Create(256, 528, 2, 16);
    lbl_80281DBC = NULL;
    lbl_80281DB8 = NULL;
    lbl_80281DB4 = 0;
    lbl_80281DB0 = 0;
}

DynObj* fn_80048E44(void) {
    return lbl_80281DBC;
}

DynObj* fn_80048E4C(int nId) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 == nId) {
            return pObj;
        }
    }
    return NULL;
}

// Shuts the kernel down: every object with an id gives it up (flag 0x10000000 set first), the
// list is swept twice (fn_800490EC) and the pools are freed.
void fn_80048E7C(void) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 != 0) {
            pObj->uFlags |= 0x10000000;
            fn_800491C4(pObj);
        }
    }
    fn_800490EC();
    fn_800490EC();
    lbl_80281DB4 = 0;
    DeleteMemPool(lbl_80281DAC);
    DeleteMemPool(lbl_80281DA8);
}

// The same, keeping the pools, and the list starts over empty.
void fn_80048EF4(void) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 != 0) {
            pObj->uFlags |= 0x10000000;
            fn_800491C4(pObj);
        }
    }
    fn_800490EC();
    fn_800490EC();
    lbl_80281DBC = NULL;
    lbl_80281DB8 = NULL;
    lbl_80281DB4 = 0;
    lbl_80281DB0 = 0;
}

void fn_80048F68(int nMsg, void* pArg, void* pArg2) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 > 0) {
            pObj->pfnHandler(nMsg, pObj, pArg, pArg2);
        }
    }
}

void fn_80048FEC(DynObj* pObj) {
    if (lbl_80281DB8 != NULL) {
        lbl_80281DB8->pNext = pObj;
        lbl_80281DB8 = pObj;
    } else {
        lbl_80281DBC = pObj;
        lbl_80281DB8 = pObj;
    }
    pObj->pNext = NULL;
}

// A new object: its handler gives its size (message 1), then sets it up (message 2), and it gets
// the next id and goes at the end of the list. NULL when there was no memory.
DynObj* fn_80049018(DynObjSetup* pSetup) {
    DynObj* pObj = fn_80048AF4_DynObjAlloc(pSetup->pfnHandler(1, NULL, NULL, NULL));

    if (pObj == NULL) {
        return NULL;
    }
    pObj->pfnHandler = pSetup->pfnHandler;
    pObj->n134 = ++lbl_80281DB4;
    pObj->pfnHandler(2, pObj, pSetup, NULL);
    fn_80048FEC(pObj);
    return pObj;
}

// The id of a new object (fn_80049018), or -2 when none could be made.
s32 fn_800490B8(DynObjSetup* pSetup) {
    DynObj* pObj = fn_80049018(pSetup);

    if (pObj != NULL) {
        return pObj->n134;
    }
    return -2;
}

// Sweeps the list: an object that gave up its id (0) is marked -1, and one already marked is shut
// down (message 5), taken out of the list and freed.
void fn_800490EC(void) {
    DynObj* pNext;
    DynObj* pPrev = NULL;
    DynObj* pObj;
    DynObj* pAfter;

    pNext = lbl_80281DBC;
    while (pNext != NULL) {
        pObj = pNext;
        pNext = pNext->pNext;
        if (pObj->n134 <= 0) {
            if (pObj->n134 == -1) {
                if (pObj->uFlags & 0x40000000) {
                    fn_8000E830(pObj);
                }
                pObj->pfnHandler(5, pObj, NULL, NULL);
                if (pPrev != NULL) {
                    pAfter = pPrev->pNext = pObj->pNext;
                } else {
                    pAfter = lbl_80281DBC = pObj->pNext;
                }
                if (pAfter == NULL) {
                    lbl_80281DB8 = pPrev;
                }
                fn_80048B70(pObj);
            } else {
                pObj->n134 = -1;
            }
        } else {
            pPrev = pObj;
        }
    }
}

// Takes the object's id away (n134 = 0), first passing its p160 to fn_8000EA1C unless p160 is
// NULL or flag 0x20000000 is set.
void fn_800491C4(DynObj* pObj) {
    if (pObj->n134 != 0) {
        if (!(pObj->uFlags & 0x20000000) && pObj->p160 != NULL) {
            fn_8000EA1C(pObj->p160, !(pObj->uFlags & 0x10000000), -1, pObj);
        }
        pObj->n134 = 0;
    }
}

// Takes a free entry of lbl_801D5228 for the object and the pair (a, b). 0 when all 16 are taken.
int fn_80049230(DynObj* pObj, int a, int b) {
    DynObjSlot* pSlot;
    u32 uBit;

    if (lbl_80281DB0 == 0xFFFF) {
        return 0;
    }
    uBit = 1;
    pSlot = lbl_801D5228;
    while (lbl_80281DB0 & uBit) {
        uBit <<= 1;
        pSlot++;
    }
    pSlot->pObj = pObj;
    pSlot->pair.b0 = a;
    lbl_80281DB0 |= uBit;
    pSlot->pair.b1 = b;
    pSlot->nId = pObj->n134;
    pSlot->n2 = 600;
    return 1;
}

// Records the pair (a, b) on the object, in its first free pair; when all four are taken and
// bOverflow is set, in lbl_801D5228 instead. 0 when it could not be recorded.
int fn_80049298(DynObj* pObj, int a, int b, int bOverflow) {
    int i = 0;

    do {
        if (pObj->a138[i].b0 == 0) {
            pObj->a138[i].b0 = a;
            pObj->a138[i].b1 = b;
            return 1;
        }
    } while (++i < 4);
    if (bOverflow) {
        return fn_80049230(pObj, a, b);
    }
    return 0;
}

// Records (a, b) on every object with an id whose n140 is nKey (a = 126: none).
void fn_80049304(int nKey, int a, int b) {
    DynObj* pObj;

    switch (a) {
    case 126:
        return;
    default:
        for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
            if (pObj->n140 == nKey && pObj->n134 != 0) {
                fn_80049298(pObj, a, b, 1);
            }
        }
    }
}

// The same, without the 126 test, for every object whose n147 is nKey.
void fn_8004939C(int nKey, int a, int b) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n147 == nKey && pObj->n134 != 0) {
            fn_80049298(pObj, a, b, 1);
        }
    }
}

// The same for every object whose n148 is nKey.
void fn_80049424(int nKey, int a, int b) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n148 == nKey && pObj->n134 != 0) {
            fn_80049298(pObj, a, b, 1);
        }
    }
}

// Asks the first object whose n140 is nKey for its value nWhat (message 9); 0 when there is none.
int fn_800494AC(int nKey, uptr nWhat) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n140 == nKey) {
            return pObj->pfnHandler(9, pObj, (void*)nWhat, NULL);
        }
    }
    return 0;
}

void fn_8004950C(void) {
}

void fn_80049510(void) {
}

// Type 0's message 2: sets the object up from its definition (its flags, n147 and n14E too); a
// model's size is taken from its first level of detail.
void fn_80049514(DynObj* pObj, DynObjSetup* pSetup) {
    int nFlags = 0;
    DynObjNames* pNames;
    DynObjDef* pDef;
    DynObjModelRef* pModel;

    pObj->n168 = 0;
    pObj->a138[0].b0 = 0;
    pObj->a138[1].b0 = 0;
    pObj->a138[2].b0 = 0;
    pObj->a138[3].b0 = 0;
    pNames = pSetup->pC;
    pDef = pSetup->pDef;
    if (pNames != NULL) {
        pObj->p15C = pNames->p28;
        pObj->p160 = pNames->p2C;
        pObj->p164 = pNames->p30;
    } else {
        pObj->p160 = NULL;
        pObj->p15C = NULL;
        pObj->p164 = NULL;
    }
    pObj->n148 = 0;
    pObj->n140 = pDef->n0;
    pObj->n147 = pDef->n18;
    pObj->n146 = pDef->n4;
    pObj->n150 = 0;
    pObj->n14C = 0;
    pObj->n14E = pDef->n1A;
    pObj->n144 = 0;
    pObj->n142 = 0;
    pObj->uFlags = pDef->u14;
    pObj->uFlags &= ~0x01000000;
    if (pObj->uFlags & 4) {
        pObj->uFlags |= 0x04000000;
    }
    if (pObj->uFlags & 0x400) {
        pObj->uFlags |= 0x02000000;
    }
    if (pSetup->pModel != NULL) {
        pModel = pSetup->pModel->aEntries[0].u.pRef;
    } else {
        pModel = NULL;
    }
    if (pObj->uFlags & 0x80) {
        nFlags |= 0x11;
    }
    if (pObj->uFlags & 0x200) {
        nFlags |= 0x40;
    }
    if (pModel != NULL) {
        pModel->p4->f5C = pModel->p4->apLod[0]->pInfo->f64;
        fn_800486F4(&pObj->obj, pModel->p4, nFlags);
        Vec3Copy(pObj->obj.pModel->apLod[0]->pInfo->v58, pObj->obj.pModel->v2C);
    } else {
        fn_800486F4(&pObj->obj, NULL, nFlags);
    }
    fn_8000ADC0(pObj->obj.m0);
    fn_8000ADC0(pObj->obj.m40);
    fn_8000C5A4(pObj->obj.m0);
    pObj->obj.m80[3][0] = pDef->aPos[0];
    pObj->obj.m80[3][1] = pDef->aPos[1];
    pObj->obj.m80[3][2] = pDef->aPos[2];
    pObj->obj.m80[3][3] = 1.0f;
    pObj->aRot[0] = 0.0f;
    pObj->aRot[1] = 0.0f;
    pObj->aRot[2] = 0.0f;
    pObj->aRot[3] = 1.0f;
    pObj->f158 = 0.0f;
}

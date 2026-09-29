// UKernel.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the kernel's list
// of the course's dynamic objects (dynobj.h), chained through DynObj.pNext from lbl_80281DBC to
// lbl_80281DB8.

#include "dynobj.h"
#include "terrain.h"
#include "psmgr.h"

DynObjSlot lbl_801D5228[32];

DynObj* lbl_80281DBC;
DynObj* lbl_80281DB8;
s32 lbl_80281DB4;
u32 lbl_80281DB0;
UMemPool* lbl_80281DAC;
UMemPool* lbl_80281DA8;

DynObj* Kernel_CreateObject(DynObjSetup* pSetup);
void fn_8000E830(DynObj* pObj);
void LLMath_IdentifyMat(f32 (*pMtx)[4]);                   // identity
void UObject_ComposeRotation(f32 (*pMtx)[4]);

void StaticCam_ParseFlybyCameraActor(UStreamObject* pObject);
void StaticCam_ParseStaticCameraActor(UStreamObject* pObject);
void Gaud_ActorDownloadCallback(UStreamObject* pObject, int n);
void PlayNow_LoadBallSpot(void* pObj);

void Kernel_FreeObjectMem(void* p);
void Kernel_DownloadActors(UStreamObject* pObject);

// Memory for an object of nSize bytes: a node of the small or the large pool while one is free,
// else from the heap.
void* Kernel_AllocObjectMem(int nSize) {
    if (nSize < 400 && lbl_80281DAC->nFree != 0) {
        return AllocPoolMem(lbl_80281DAC);
    }
    if (nSize < 528 && lbl_80281DA8->nFree != 0) {
        return AllocPoolMem(lbl_80281DA8);
    }
    return StaticMem_Alloc(nSize, 1, 16, "UKernel.c", 201);
}

// Gives an object's memory back to the pool it came from, or to the heap.
void Kernel_FreeObjectMem(void* p) {
    if ((u8*)p > (u8*)lbl_80281DAC && (u8*)p < lbl_80281DAC->pEnd) {
        ReturnPoolMem(lbl_80281DAC, p);
    } else if ((u8*)p > (u8*)lbl_80281DA8 && (u8*)p < lbl_80281DA8->pEnd) {
        ReturnPoolMem(lbl_80281DA8, p);
    } else {
        StaticMem_Free(p);
    }
}

// The 'Cact' stream handler: one actor (dynamic object) of the hole arrived. By its type (the
// 'tACT' data's n4): 200 and 201 are fly-by and static cameras (StaticCam), 5 and 6 go to
// fn_80034720 / fn_800347B4 (6 continues when that answers nonzero), 7 is a particle emitter
// (fn_8009943C), 8 is dropped, 9 is a sound (Gaud_ActorDownloadCallback), 10 the Play Now ball
// spot. Any other type becomes a dynamic object: its 'aRSL' resource list is resolved into model
// references, its handler found by type (fn_800499B0), and the stream object is freed.
void Kernel_DownloadActors(UStreamObject* pObject) {
    DynObjSetup setup;
    TagRecord* pChunk;
    int i;

    setup.pDef = (DynObjDef*)((u8*)fn_8000B748(pObject->pData, pObject->uSize, 'tACT', pObject->uId) -
                              sizeof(u32));
    switch (setup.pDef->n4) {
    case 200:
        StaticCam_ParseFlybyCameraActor(pObject);
        return;
    case 201:
        StaticCam_ParseStaticCameraActor(pObject);
        return;
    case 5:
        fn_80034720(pObject);
        return;
    case 6:
        if (!fn_800347B4(pObject)) return;
        break;
    case 7:
        fn_8009943C((PsEmitterRecord*)pObject->pData, pObject->uSize);
        StaticMem_Free(pObject);
        return;
    case 8:
        StaticMem_Free(pObject);
        return;
    case 10:
        PlayNow_LoadBallSpot(pObject);
        return;
    case 9:
        Gaud_ActorDownloadCallback(pObject, 1);
        StaticMem_Free(pObject);
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
    setup.pfnHandler = DynObj_GetTypeHandler(setup.pDef->n4);
    setup.pC = (DynObjNames*)pObject;
    pObject->pData = (u8*)setup.pModel;
    pObject->pfn8 = NULL;
    pObject->uUnk4 = Kernel_CreateObjectId(&setup);
    StaticMem_Free(pObject);
}

// Sets the kernel up when a round starts (GO_vInitIG): the 'Cact' handler (Kernel_DownloadActors),
// the two node pools (256 nodes each of 400 and 528 bytes) and an empty object list.
void Kernel_InitModule(void) {
    Stream_RegisterLoadChunkCallback('Cact', Kernel_DownloadActors);
    lbl_80281DAC = CreateMemPool(256, 400, 2, 16);
    lbl_80281DA8 = CreateMemPool(256, 528, 2, 16);
    lbl_80281DBC = NULL;
    lbl_80281DB8 = NULL;
    lbl_80281DB4 = 0;
    lbl_80281DB0 = 0;
}

DynObj* Kernel_GetFirstObject(void) {
    return lbl_80281DBC;
}

// The dynamic object whose id (n134, given by Kernel_CreateObject) is nId; NULL when none has it.
DynObj* Kernel_FindObjectById(int nId) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 == nId) {
            return pObj;
        }
    }
    return NULL;
}

// Shuts the kernel down at a round's end: every object with an id gives it up (flag 0x10000000 set
// first, Kernel_ReleaseObject), the list is swept twice (Kernel_SweepDeadObjects: marked, then
// freed) and the two pools are deleted.
void Kernel_CloseModule(void) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 != 0) {
            pObj->uFlags |= 0x10000000;
            Kernel_ReleaseObject(pObj);
        }
    }
    Kernel_SweepDeadObjects();
    Kernel_SweepDeadObjects();
    lbl_80281DB4 = 0;
    DeleteMemPool(lbl_80281DAC);
    DeleteMemPool(lbl_80281DA8);
}

// Removes every dynamic object as Kernel_CloseModule does, but keeps the pools and starts the list,
// the ids and the overflow slots over; called by fn_8006F568.
void Kernel_RemoveAllObjects(void) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 != 0) {
            pObj->uFlags |= 0x10000000;
            Kernel_ReleaseObject(pObj);
        }
    }
    Kernel_SweepDeadObjects();
    Kernel_SweepDeadObjects();
    lbl_80281DBC = NULL;
    lbl_80281DB8 = NULL;
    lbl_80281DB4 = 0;
    lbl_80281DB0 = 0;
}

// Sends message nMsg with pArg and pArg2 to the handler of every live object (id above 0).
void Kernel_BroadcastMessage(int nMsg, void* pArg, void* pArg2) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n134 > 0) {
            pObj->pfnHandler(nMsg, pObj, pArg, pArg2);
        }
    }
}

// Links the new object in at the end of the kernel's list (its pNext cleared); Kernel_CreateObject
// calls it.
void Kernel_AppendObject(DynObj* pObj) {
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
DynObj* Kernel_CreateObject(DynObjSetup* pSetup) {
    DynObj* pObj = Kernel_AllocObjectMem(pSetup->pfnHandler(1, NULL, NULL, NULL));

    if (pObj == NULL) {
        return NULL;
    }
    pObj->pfnHandler = pSetup->pfnHandler;
    pObj->n134 = ++lbl_80281DB4;
    pObj->pfnHandler(2, pObj, pSetup, NULL);
    Kernel_AppendObject(pObj);
    return pObj;
}

// The id of a new object (Kernel_CreateObject), or -2 when none could be made.
s32 Kernel_CreateObjectId(DynObjSetup* pSetup) {
    DynObj* pObj = Kernel_CreateObject(pSetup);

    if (pObj != NULL) {
        return pObj->n134;
    }
    return -2;
}

// Sweeps the list: an object that gave up its id (0) is marked -1, and one already marked is shut
// down (message 5), taken out of the list and freed.
void Kernel_SweepDeadObjects(void) {
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
                Kernel_FreeObjectMem(pObj);
            } else {
                pObj->n134 = -1;
            }
        } else {
            pPrev = pObj;
        }
    }
}

// Marks the object for removal: its id n134 becomes 0 (Kernel_SweepDeadObjects frees it later),
// first running its p160 byte-code (fn_8000EA1C) unless p160 is NULL or flag 0x20000000 is set.
void Kernel_ReleaseObject(DynObj* pObj) {
    if (pObj->n134 != 0) {
        if (!(pObj->uFlags & 0x20000000) && pObj->p160 != NULL) {
            fn_8000EA1C(pObj->p160, !(pObj->uFlags & 0x10000000), -1, pObj);
        }
        pObj->n134 = 0;
    }
}

// Takes a free entry of lbl_801D5228 for the object and the pair (a, b). 0 when all 16 are taken.
int Kernel_PostPairToOverflowSlot(DynObj* pObj, int a, int b) {
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
int Kernel_PostPairToObject(DynObj* pObj, int a, int b, int bOverflow) {
    int i = 0;

    do {
        if (pObj->a138[i].b0 == 0) {
            pObj->a138[i].b0 = a;
            pObj->a138[i].b1 = b;
            return 1;
        }
    } while (++i < 4);
    if (bOverflow) {
        return Kernel_PostPairToOverflowSlot(pObj, a, b);
    }
    return 0;
}

// Stores (a, b) on every live object whose actor id n140 (the actor chunk's id) is nKey; nothing
// when a is 126. The actor byte-code's opcode 56 (fn_8000EA1C).
void Kernel_PostPairByActorId(int nKey, int a, int b) {
    DynObj* pObj;

    switch (a) {
    case 126:
        return;
    default:
        for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
            if (pObj->n140 == nKey && pObj->n134 != 0) {
                Kernel_PostPairToObject(pObj, a, b, 1);
            }
        }
    }
}

// Stores (a, b) on every live object whose byte n147 (set from its definition's n18) is nKey; no
// 126 test. The actor byte-code's opcode 57.
void Kernel_PostPairByKey147(int nKey, int a, int b) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n147 == nKey && pObj->n134 != 0) {
            Kernel_PostPairToObject(pObj, a, b, 1);
        }
    }
}

// Stores (a, b) on every live object whose byte n148 is nKey. The actor byte-code's opcode 58.
void Kernel_PostPairByKey148(int nKey, int a, int b) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n148 == nKey && pObj->n134 != 0) {
            Kernel_PostPairToObject(pObj, a, b, 1);
        }
    }
}

// Asks the first object whose actor id n140 is nKey for its value nWhat (message 9); 0 when there
// is none.
int Kernel_QueryActorById(int nKey, uptr nWhat) {
    DynObj* pObj;

    for (pObj = lbl_80281DBC; pObj != NULL; pObj = pObj->pNext) {
        if (pObj->n140 == nKey) {
            return pObj->pfnHandler(9, pObj, (void*)nWhat, NULL);
        }
    }
    return 0;
}

// The kernel's one-time init at startup (fn_8006C7A8): empty in this build.
void Kernel_InitOnStartup(void) {
}

// The kernel's one-time close at shutdown (fn_8006C854): empty in this build.
void Kernel_CloseOnShutdown(void) {
}

// Type 0's message 2: sets the object up from its definition (its flags, n147 and n14E too); a
// model's size is taken from its first level of detail.
void Kernel_InitObjectFromDef(DynObj* pObj, DynObjSetup* pSetup) {
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
        Object_Init(&pObj->obj, pModel->p4, nFlags);
        Vec3Copy(pObj->obj.pModel->apLod[0]->pInfo->v58, pObj->obj.pModel->v2C);
    } else {
        Object_Init(&pObj->obj, NULL, nFlags);
    }
    LLMath_IdentifyMat(pObj->obj.m0);
    LLMath_IdentifyMat(pObj->obj.m40);
    UObject_ComposeRotation(pObj->obj.m0);
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

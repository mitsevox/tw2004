// GoAnimalActors.c (TW06's golf/hi-rendering/goanimalactors.c; our spelling): the animals on the
// course (ActAnimal_*). Not yet decompiled; the unit covers the file's certain core, whose
// functions share its constant block (0x80283218-0x80283280).

#include "dynobj.h"

void fn_8004A24C(DynObjAnimal* pAnimal, DynObjSetup* pSetup);  // message 2: sets it up
void fn_8004A578(DynObjAnimal* pAnimal, void* pArg);            // message 6: pArg holds the frame
                                                                // time's bits

// Scales f16C and f174 by the length of the animal's route (once round its points).
void fn_8004A14C(DynObjAnimal* pAnimal) {
    f32 vStep[4];
    f32 fLength = 0.0f;
    AnimalNode* pNode;
    int i;
    AnimalNode* pNext;

    if (pAnimal->pRoute != NULL) {
        pNode = &pAnimal->pRoute->aNodes[0];
        for (i = 0; i < pAnimal->pRoute->nNodes; i++) {
            pNext = &pAnimal->pRoute->aNodes[pNode->nNext];
            vStep[0] = pNext->vPos[0] - pNode->vPos[0];
            vStep[1] = pNext->vPos[1] - pNode->vPos[1];
            vStep[2] = pNext->vPos[2] - pNode->vPos[2];
            vStep[3] = 1.0f;
            fLength += (f32)fn_80009680(fn_80009744(vStep));
            pNode = pNext;
        }
        pAnimal->f16C /= fLength;
        pAnimal->f174 /= fLength;
    }
}

// Type 11's message handler; other messages go to type 0's.
int fn_8004AAEC(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObjAnimal);
    case 2:
        fn_8004A24C((DynObjAnimal*)pObj, pArg);
        return 0;
    case 6:
        fn_8004A578((DynObjAnimal*)pObj, pArg);
        return 0;
    case 3:
        if (pObj->obj.pModel != NULL) {
            pObj->obj.n108 = ((DynObjAnimal*)pObj)->n1AC;
            pObj->obj.f10C = ((DynObjAnimal*)pObj)->f1B4;
            fn_80048894(&pObj->obj);
        }
        return 0;
    default:
        return fn_80049820(nMsg, pObj, pArg, pArg2);
    }
}

// The same as UObject.c's fn_80048AD4, compiled into this file too.
int fn_8004AB90(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->a24[i];
}

// The same as UObject.c's fn_80048AC4, compiled into this file too.
UObjMesh* fn_8004ABA4(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_8004ABB4(u8* p);

s32 fn_8004ABB4(u8* p) {
    return *(s32*)(p + 0xEC);
}

// ---- end of sweep code ----

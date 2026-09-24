// GoAnimalActors.c (TW06's golf/hi-rendering/goanimalactors.c; our spelling): the animals on the
// course (ActAnimal_*). Not yet decompiled; the unit covers the file's certain core, whose
// functions share its constant block (0x80283218-0x80283280).

#include "dynobj.h"
#include "ball.h"

void fn_8000ADC0(f32 (*pMtx)[4]);                       // identity
void fn_8000A194(f32 (*pMtx)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void fn_8000C5A4(f32 (*pMtx)[4]);
void ActAnimal_SetWorldMatrix(DynObjAnimal* pAnimal, f32 f);
int  fn_8004AB90(UObjMesh* pMesh, int i);
UObjMesh* fn_8004ABA4(UObjMesh* pMesh, int i);
UObjMesh* fn_8004ABB4(UObjModelRoot* pRoot);
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

// Message 2: sets the animal up from its definition (type 0's setup first), picks its n1A4 from
// its model's mesh bits and its route from the second stream object, then places it: along the
// route, or at the definition's position raised by f198.
void fn_8004A24C(DynObjAnimal* pAnimal, DynObjSetup* pSetup) {
    f32 vPos[4];
    DynObjModel* pModel;
    int i;
    DynObjAnimalDef* pDef;
    DynObjModelRef* pRef;
    UObjMesh* pMesh;
    int nBitsA;
    int nBitsB;
    CourseInfo* pCourse;
    f32 fGround;
    f32 fY;

    fn_80049514(&pAnimal->base, pSetup);
    pDef = (DynObjAnimalDef*)pSetup->pDef;
    pAnimal->f16C = pDef->f1C;
    pAnimal->f170 = 0.0f;
    pAnimal->f174 = 2.0f * pAnimal->f16C;
    pAnimal->a178[0] = pDef->a20[0];
    pAnimal->a178[1] = pDef->a20[1];
    pAnimal->a178[2] = pDef->a20[2];
    pAnimal->a178[3] = pDef->a20[3];
    pAnimal->a178[4] = pDef->a20[4];
    pAnimal->f18C = 0.0f;
    pAnimal->f190 = 0.0f;
    pAnimal->f194 = pDef->f34;
    pAnimal->f198 = pDef->f38;
    pAnimal->n1AC = 0;
    pAnimal->n1B0 = 0;
    pAnimal->f1B4 = 0.5f;
    pAnimal->n1B8 = 0;
    pAnimal->b1BC = 0;

    pModel = pSetup->pModel;
    if (pModel != NULL) {
        pRef = pModel->aEntries[0].u.pRef;
    } else {
        pRef = NULL;
    }
    if (pRef != NULL) {
        pMesh = fn_8004ABB4(pRef->p4->p10);
        pMesh = fn_8004ABA4(pMesh, 0);
        pMesh = fn_8004ABA4(pMesh, 0);
        nBitsA = fn_8004AB90(pMesh, 0);
        nBitsB = fn_8004AB90(pMesh, 2);
        if (nBitsA & 1) {
            if (nBitsA & 2) {
                pAnimal->n1A4 = 2;
            } else if (nBitsB & 1) {
                pAnimal->n1A4 = 0;
            } else if (nBitsB & 2) {
                pAnimal->n1A4 = 1;
            } else {
                pAnimal->n1A4 = 3;
            }
        } else {
            pAnimal->n1A4 = 4;
        }
    } else {
        pAnimal->n1A4 = 4;
    }

    pAnimal->b1BD = 0;
    if (pModel != NULL && (u32)pModel->nEntries > 1) {  // fake match: EA compares unsigned here
        pAnimal->pRoute = (AnimalRoute*)(pModel->aEntries[1].u.pRef->pData + 12);
        pCourse = fn_8000C594();
        for (i = 0; i < pAnimal->pRoute->nNodes; i++) {
            vPos[0] = pAnimal->pRoute->aNodes[i].vPos[0];
            fY = pAnimal->pRoute->aNodes[i].vPos[1];
            vPos[1] = 10.0f + fY;
            vPos[2] = pAnimal->pRoute->aNodes[i].vPos[2];
            vPos[3] = 1.0f;
            if (pCourse != NULL) {
                fGround = fn_8004D620(pCourse, vPos);
                if (fGround != -65536.125f) {
                    if (fY - fGround < 0.5f && pAnimal->f198 != -999.0f) {
                        pAnimal->b1BD = 1;
                    } else {
                        pAnimal->b1BD = 0;
                    }
                    break;
                }
            }
        }
    } else {
        pAnimal->pRoute = NULL;
    }
    if (pAnimal->f198 == -999.0f) {
        pAnimal->f198 = 0.0f;
    }
    pAnimal->f19C = 0.0f;
    pAnimal->f1A0 = 0.0f;
    fn_8000A194(pAnimal->base.obj.m0, pDef->aAngles[1] * (PI / 180.0f), pDef->aAngles[0] * (PI / 180.0f),
                pDef->aAngles[2] * (PI / 180.0f));
    fn_8000ADC0(pAnimal->base.obj.m40);
    fn_8000C5A4(pAnimal->base.obj.m0);
    if (pAnimal->pRoute != NULL) {
        ActAnimal_SetWorldMatrix(pAnimal, 0.0f);
    } else {
        pAnimal->base.obj.m80[3][0] = pDef->base.aPos[0];
        pAnimal->base.obj.m80[3][1] = pDef->base.aPos[1] + pAnimal->f198;
        pAnimal->base.obj.m80[3][2] = pDef->base.aPos[2];
    }
    fn_8004A14C(pAnimal);
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

UObjMesh* fn_8004ABB4(UObjModelRoot* pRoot) {
    return pRoot->pMesh;
}

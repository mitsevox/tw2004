// UObject.c (EA's name, from its asserts; also in EA's 2002 source tree): a drawable object (dynobj.h):
// three matrices and a model with up to four levels of detail. Every dynamic object of the course
// holds one (DynObj.obj).

#include "dynobj.h"

void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
void fn_800082CC(UObjMeshPart* pPart);
void fn_800488B4(UObject* pObj);

// Sets the object up: the three matrices to identity, the model and flags; a model whose levels of
// detail differ gets flag 4 and a level-of-detail scale from its size.
void fn_800486F4(UObject* pObj, UObjModel* pModel, u32 uFlags) {
    f32 fScale;

    fn_8000ADC0(pObj->m0);
    fn_8000ADC0(pObj->m40);
    fn_8000ADC0(pObj->m80);
    pObj->pModel = pModel;
    pObj->uFlags = uFlags;
    pObj->n104 = 0;
    pObj->n108 = 0;
    pObj->f10C = 0.5f;
    if (pModel != NULL) {
        pObj->fE4 = pModel->f5C;
        pObj->f100 = 0.0f;
        if (pModel->apLod[0] != pModel->apLod[1] || pModel->apLod[0] != pModel->apLod[2] ||
            pModel->apLod[0] != pModel->apLod[3]) {
            pObj->uFlags |= 4;
            if (0.0f != pModel->f5C) {
                fScale = 0.5f / pModel->f5C;
                if (fScale < 0.1f) {
                    fScale = 0.1f;
                } else if (fScale > 2.5f) {
                    fScale = 2.5f;
                }
                pObj->fFC = fScale;
            } else {
                pObj->fFC = 0.1f;
            }
        }
    }
    pObj->n106 = 0;
}

void fn_80048804(UObject* pObj) {
}

// A new object of the model, from the heap.
UObject* fn_80048808(UObjModel* pModel) {
    UObject* pObj = fn_80009B34(sizeof(UObject), 2, 1, "UObject.c", 368);

    fn_800486F4(pObj, pModel, 0);
    return pObj;
}

// Frees an object fn_80048808 made.
void fn_80048860(UObject* pObj) {
    fn_80048804(pObj);
    fn_80009E70(pObj);
}

void fn_80048894(UObject* pObj) {
    fn_800488B4(pObj);
}

void fn_80048A84(UObjMesh* pMesh) {
    if (pMesh->a1C[pMesh->n28] != 0) {
        fn_800082CC(&pMesh->p18[pMesh->n28]);
    }
}

s32 fn_80048AC4(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}

int fn_80048AD4(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->a24[i];
}

// The level of detail drawn.
int fn_80048AE8(UObject* pObj) {
    return pObj->n104;
}

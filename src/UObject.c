// UObject.c (EA's name, from its asserts; also in EA's 2002 source tree): a drawable object (dynobj.h):
// three matrices and a model with up to four levels of detail. Every dynamic object of the course
// holds one (DynObj.obj).

#include "dynobj.h"
#include "camera.h"
#include "terrain.h"
#include "golfer.h"

void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
void fn_800488B4(UObject* pObj);
void fn_80048A84(UObjMesh* pMesh);
UObjMesh* fn_80048AC4(UObjMesh* pMesh, int i);
int  fn_80048AD4(UObjMesh* pMesh, int i);
int  fn_80048AE8(UObject* pObj);
f32  fn_8001414C(u8* p);
f32  fn_80014280(f32 f);
void fn_8004B78C(CourseInfo* pCourse, f32* pPos);  // the ground's light at pPos
void fn_80036024(f32 f);
void fn_80035FFC(void);
void fn_80035FDC(UObject* pObj);
void fn_8003519C(int nRow, void* pData);
void fn_80035FBC(void);

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

// Draws the object: its level of detail's mesh, unless fn_80007B2C finds it off screen (3); lit by
// the ground under it (outside game type 3) when its mesh asks for it.
void fn_800488B4(UObject* pObj) {
    int bLit;
    int nFlags0;
    int nClip;
    int nLod;
    UObjMesh* pMesh;
    f32 fFov;
    f32 fMax;
    int nFlags2;
    f32 fLod;

    nLod = fn_80048AE8(pObj);
    pMesh = pObj->pModel->apLod[nLod];
    fFov = fn_8001F004()->fFov;
    fMax = 0.75f * fFov * fn_8001414C((u8*)fn_8003526C());
    fn_80035240(pObj->m80);
    fFov = fn_80014280((fFov <= fMax ? fFov : fMax) / 2.0f);
    nClip = fn_80007B2C(pMesh, fn_8001614C(), 0.0f, fFov, 1.0f);
    if (nClip == 3) return;
    nFlags0 = fn_80048AD4(pMesh, 0);
    nFlags2 = fn_80048AD4(pMesh, 2);
    bLit = nFlags2 & 4;
    if (bLit) {
        if (gSession.nGameType != 3) {
            fn_8004B78C(fn_8000C594(), pObj->m80[3]);
            fn_80036024(0.8f);
        }
        fn_80035FFC();
        fn_80035FDC(pObj);
    } else if (nFlags0 & 1) {
        if ((nFlags0 & 2) || (nFlags2 & 1) || (nFlags2 & 2)) {
            pMesh = fn_80048AC4(pMesh, pObj->n108);
        }
        fn_80012EF8();
        fLod = pObj->f10C;
        fn_8003519C(3, &fLod);
    }
    switch (nClip) {
    case 2:
        fn_80016B9C();
        fn_80035138(1);
        break;
    case 1:
        fn_80016B9C();
        fn_80035138(1);
        break;
    default:
        fn_80016B9C();
        fn_80035138(0);
        break;
    }
    fn_80012EF8();
    fn_80048A84(pMesh);
    if (bLit) {
        fn_80035FBC();
    }
}

void fn_80048A84(UObjMesh* pMesh) {
    if (pMesh->a1C[pMesh->n28] != 0) {
        fn_800082CC(&pMesh->p18[pMesh->n28]);
    }
}

// Mesh i of the level of detail's alternatives.
UObjMesh* fn_80048AC4(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}

int fn_80048AD4(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->a24[i];
}

// The level of detail drawn.
int fn_80048AE8(UObject* pObj) {
    return pObj->n104;
}

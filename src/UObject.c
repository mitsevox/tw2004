// UObject.c (EA's name, from its asserts; also in EA's 2002 source tree): a drawable object (dynobj.h):
// three matrices and a model with up to four levels of detail. Every dynamic object of the course
// holds one (DynObj.obj).

#include "dynobj.h"
#include "camera.h"
#include "terrain.h"
#include "golfer.h"

void LLMath_IdentifyMat(f32 (*pMtx)[4]);                   // identity
void Object_Render(UObject* pObj);
void Object_DrawMesh(UObjMesh* pMesh);
UObjMesh* Object_GetMeshAlternative(UObjMesh* pMesh, int i);
int  Object_GetMeshFlags(UObjMesh* pMesh, int i);
int  Object_GetLod(UObject* pObj);
f32  fn_8001414C(u8* p);
f32  Math_Tan(f32 f);
void Ter_GetAmbientLight(CourseInfo* pCourse, f32* pPos);  // the ground's light at pPos
void LF_SetCurrentBrightness(f32 f);
void LF_LoadCurrentLights(void);
void LI_SetObjectLights(UObject* pObj);
void SD_SetShaderTypeParameters(int nRow, void* pData);
void LI_ResetLights(void);

// Sets the object up: the three matrices to identity, the model and flags; a model whose levels of
// detail differ gets flag 4 and a level-of-detail scale from its size.
void Object_Init(UObject* pObj, UObjModel* pModel, u32 uFlags) {
    f32 fScale;

    LLMath_IdentifyMat(pObj->m0);
    LLMath_IdentifyMat(pObj->m40);
    LLMath_IdentifyMat(pObj->m80);
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

// Empty in this build; called before an object is freed (Object_Free, GoDynObjBase.c).
void Object_Destroy(UObject* pObj) {
}

// A new object of the model, from the heap (freed by Object_Free).
UObject* Object_Create(UObjModel* pModel) {
    UObject* pObj = StaticMem_Alloc(sizeof(UObject), 2, 1, "UObject.c", 368);

    Object_Init(pObj, pModel, 0);
    return pObj;
}

// Frees an object Object_Create made.
void Object_Free(UObject* pObj) {
    Object_Destroy(pObj);
    StaticMem_Free(pObj);
}

void Object_Draw(UObject* pObj) {
    Object_Render(pObj);
}

// fake match: stands in for a function the original linker stripped. The file's pool has 1.0 before
// Object_Render's 0.75 (0x802831F0, 0x802831F4); its body is unknown, this one only reproduces the order.
static f32 UObject_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Draws the object: its level of detail's mesh, unless fn_80007B2C finds it off screen (3); lit by
// the ground under it (outside game type 3) when its mesh's flag byte 2 has bit 4, else with the
// alternative mesh n108 and shader value f10C when flag byte 0 asks for them.
void Object_Render(UObject* pObj) {
    int bLit;
    int nFlags0;
    int nClip;
    int nLod;
    UObjMesh* pMesh;
    s32* pN108;
    f32 fFov;
    f32 fMax;
    int nFlags2;
    f32 fLod;
    f32 fTemp;

    nLod = Object_GetLod(pObj);
    pMesh = pObj->pModel->apLod[nLod];
    fFov = Camera_GetCurrentLens()->fFov;
    fMax = 0.75f * fFov * fn_8001414C((u8*)fn_8003526C());
    RC_vSetCurrentRenderCtxTransformationMatrix(pObj->m80);
    fTemp = fFov <= fMax ? fFov : fMax;
    // fake match: n108's address is taken here only so the 0.5 is loaded after the min, as in the
    // original; nothing in TW07 shows EA wrote it so. A port reads pObj->n108 directly below.
    // Found by an anonymous decomp.me user: https://decomp.me/scratch/SOh7Q
    pN108 = &pObj->n108;
    fFov = Math_Tan(0.5f * fTemp);
    nClip = fn_80007B2C(pMesh, RC_spGetCurrentRenderCtx(), 0.0f, fFov, 1.0f);
    if (nClip == 3) return;
    nFlags0 = Object_GetMeshFlags(pMesh, 0);
    nFlags2 = Object_GetMeshFlags(pMesh, 2);
    bLit = nFlags2 & 4;
    if (bLit) {
        if (gSession.nGameType != 3) {
            Ter_GetAmbientLight(Ter_GetTGD(), pObj->m80[3]);
            LF_SetCurrentBrightness(0.8f);
        }
        LF_LoadCurrentLights();
        LI_SetObjectLights(pObj);
    } else if (nFlags0 & 1) {
        if ((nFlags0 & 2) || (nFlags2 & 1) || (nFlags2 & 2)) {
            pMesh = Object_GetMeshAlternative(pMesh, *pN108);
        }
        RenderState_Flush();
        fLod = pObj->f10C;
        SD_SetShaderTypeParameters(3, &fLod);
    }
    switch (nClip) {
    case 2:
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(1);
        break;
    case 1:
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(1);
        break;
    default:
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(0);
        break;
    }
    RenderState_Flush();
    Object_DrawMesh(pMesh);
    if (bLit) {
        LI_ResetLights();
    }
}

// Draws the mesh's part n28 (fn_800082CC) when a1C marks it used.
void Object_DrawMesh(UObjMesh* pMesh) {
    if (pMesh->a1C[pMesh->n28] != 0) {
        fn_800082CC(&pMesh->p18[pMesh->n28]);
    }
}

// Mesh i of the level of detail's alternatives.
UObjMesh* Object_GetMeshAlternative(UObjMesh* pMesh, int i) {
    return pMesh->p8[i];
}

int Object_GetMeshFlags(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->a24[i];
}

// The level of detail drawn.
int Object_GetLod(UObject* pObj) {
    return pObj->n104;
}

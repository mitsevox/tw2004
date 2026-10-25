// UObject.c: a drawable object (TObject), a model with up to four levels of detail and the matrices
// that place it. Every dynamic object of the course holds one (DynObj.obj); the ball, its tee and
// divot, and the front end's 3D objects are drawn as one too.

#include "dynobj.h"
#include "camera.h"
#include "terrain.h"
#include "lighting.h"
#include "golfer.h"

static void Object_Render(TObject* pObj);
static void Object_DrawMesh(UObjMesh* pMesh);
static UObjMesh* Object_GetMeshChild(UObjMesh* pMesh, int i);
static int  Object_GetMeshFlags(UObjMesh* pMesh, int i);
static int  Object_GetLod(TObject* pObj);

// Sets the object up: its three matrices to the identity, its model and flags, level of detail 0.
// A model whose four levels of detail are not all one mesh gets TOBJECT_HAS_LODS and a
// level-of-detail scale of 0.5 / its radius, kept to 0.1..2.5.
void Object_Init(TObject* pObject, UObjModel* pModel, u32 Flags) {
    f32 fScale;

    LLMath_IdentifyMat(pObject->mRotation);
    LLMath_IdentifyMat(pObject->mScale);
    LLMath_IdentifyMat(pObject->mObjToWorld);
    pObject->pModel = pModel;
    pObject->uFlags = Flags;
    pObject->nLod = 0;
    pObject->nMeshChild = 0;
    pObject->fShaderParam = 0.5f;
    if (pModel != NULL) {
        pObject->fRadius = pModel->fRadius;
        pObject->fUnread = 0.0f;
        if (pModel->apLod[0] != pModel->apLod[1] || pModel->apLod[0] != pModel->apLod[2] ||
            pModel->apLod[0] != pModel->apLod[3]) {
            pObject->uFlags |= TOBJECT_HAS_LODS;
            if (0.0f != pModel->fRadius) {
                fScale = 0.5f / pModel->fRadius;
                if (fScale < 0.1f) {
                    fScale = 0.1f;
                } else if (fScale > 2.5f) {
                    fScale = 2.5f;
                }
                pObject->fLodScale = fScale;
            } else {
                pObject->fLodScale = 0.1f;
            }
        }
    }
    pObject->nUnread = 0;
}

// Does nothing. Object_Free calls it before freeing the object, and a dynamic object's handler on
// message 5 (GoDynObjBase.c).
void Object_Destroy(TObject* xpObject_p) {
}

// A new object drawing pModel, from the heap (Object_Free frees it).
TObject* Object_Create(UObjModel* pModel) {
    TObject* pObj = Circular_MemAllocFunc(sizeof(TObject), MEMTYPE_LOW, 1, "UObject.c", 368);

    Object_Init(pObj, pModel, 0);
    return pObj;
}

void Object_Free(TObject* pObj) {
    Object_Destroy(pObj);
    Circular_MemFreeFunc(pObj);
}

void Object_Draw(TObject* pObj) {
    Object_Render(pObj);
}

// fake match: never called; its 1.0 puts that constant before Object_Render's 0.75
static f32 UObject_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Draws the object with mObjToWorld: the mesh of its level of detail, unless it is out of view.
// LLObj_GetMeshClipMethod culls by the mesh's bounding sphere; the tangent of half the view angle
// worked out for it is not used by it. A UOBJMESH_F2_LIT mesh is lit: outside the front end the
// light set's brightness is set to 0.8 (the ground's light under the object is asked for and
// dropped), and the lights are loaded in the object's space, then reset after the draw. Otherwise
// a UOBJMESH_F0_ANIMATED mesh is drawn with fShaderParam handed to shader type 3, and when its
// flags ask for a pose, its child nMeshChild is drawn in its place. GX clipping is off for a mesh
// wholly in view (DC_CLIP_UNTESTED never comes: LLObj_GetMeshClipMethod does not return it).
static void Object_Render(TObject* pObj) {
    int bLit;
    int nFlags0;
    int nClip;
    int nLod;
    UObjMesh* pMesh;
    s32* pnMeshChild;
    f32 fFov;
    f32 fMax;
    int nFlags2;
    f32 fParam;
    f32 fView;

    nLod = Object_GetLod(pObj);
    pMesh = pObj->pModel->apLod[nLod];
    fFov = Camera_GetCurrentLens()->fFov;
    fMax = 0.75f * fFov * VM_fGetViewportHeightOverWidth(RC_spGetCurrentRenderCtxViewport());
    RC_vSetCurrentRenderCtxTransformationMatrix(pObj->mObjToWorld);
    fView = fFov <= fMax ? fFov : fMax;
    // fake match: nMeshChild read through a pointer taken here holds the 0.5 load back after the min
    pnMeshChild = &pObj->nMeshChild;
    fFov = Math_Tan(0.5f * fView);
    nClip = LLObj_GetMeshClipMethod(pMesh, RC_spGetCurrentRenderCtx(), 0.0f, fFov, 1.0f);
    if (nClip == DC_CLIP_OUT_OF_VIEW) return;
    nFlags0 = Object_GetMeshFlags(pMesh, 0);
    nFlags2 = Object_GetMeshFlags(pMesh, 2);
    bLit = nFlags2 & UOBJMESH_F2_LIT;
    if (bLit) {
        if (gSession.nGameType != GAMESTATE_FE_RUN) {
            Ter_GetAmbientLight(Ter_GetTGD(), pObj->mObjToWorld[3]);
            LF_SetCurrentBrightness(0.8f);
        }
        LF_LoadCurrentLights();
        LI_SetObjectLights(pObj);
    } else if (nFlags0 & UOBJMESH_F0_ANIMATED) {
        if ((nFlags0 & UOBJMESH_F0_POSES) || (nFlags2 & UOBJMESH_F2_POSE_KIND0)
            || (nFlags2 & UOBJMESH_F2_POSE_KIND1)) {
            pMesh = Object_GetMeshChild(pMesh, *pnMeshChild);
        }
        RenderState_Flush();
        fParam = pObj->fShaderParam;
        SD_SetShaderTypeParameters(3, &fParam);
    }
    switch (nClip) {
    case DC_CLIP_IN_VIEW:
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(GX_CLIP_DISABLE);
        break;
    case DC_CLIP_UNTESTED:
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(GX_CLIP_DISABLE);
        break;
    default:
        RenderState_SetCameraMatrices();
        RenderState_SetClipMode(GX_CLIP_ENABLE);
        break;
    }
    RenderState_Flush();
    Object_DrawMesh(pMesh);
    if (bLit) {
        LI_ResetLights();
    }
}

// Draws the mesh's part nDrawPart, when that part is in use.
static void Object_DrawMesh(UObjMesh* pMesh) {
    if (pMesh->abPartUsed[pMesh->nDrawPart] != 0) {
        ObjPart_Draw(&pMesh->pParts[pMesh->nDrawPart]);
    }
}

static UObjMesh* Object_GetMeshChild(UObjMesh* pMesh, int i) {
    return pMesh->ppChildren[i];
}

// The mesh's flag byte i.
static int Object_GetMeshFlags(UObjMesh* pMesh, int i) {
    return pMesh->pInfo->aFlags[i];
}

static int Object_GetLod(TObject* pObj) {
    return pObj->nLod;
}

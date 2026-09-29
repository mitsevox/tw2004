// GoDynObjTypes.c (our name; TW06's and TW07's file lists name no file for it): two more types of
// the course's dynamic objects (dynobj.h): type 6 (DynObjType6_MessageHandler), a model that is
// drawn and does nothing else, and type 9 (DynObjType9_MessageHandler), which has no model to
// draw. Both set up their object the way type 0 does (Kernel_InitObjectFromDef), without the definition's
// flags. Its constant block is 0x80283280-0x80283288.

#include "dynobj.h"

void LLMath_IdentifyMat(f32 (*pMtx)[4]);                                           // identity
void UObject_ComposeRotation(f32 (*pMtx)[4]);

// Type 6's message 2: sets the object up from its definition as type 0 does, but with no flags (so
// the flag tests below never hold), n14E at -1, and the model of the setup's first entry (or none).
void DynObjType6_Init(DynObj* pObj, DynObjSetup* pSetup) {
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
    pObj->n147 = 0;
    pObj->n146 = pDef->n4;
    pObj->n150 = 0;
    pObj->n14C = 0;
    pObj->n14E = -1;
    pObj->n144 = 0;
    pObj->n142 = 0;
    pObj->uFlags = 0;
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
        Object_Init(&pObj->obj,pModel->p4, nFlags);
    } else {
        Object_Init(&pObj->obj,NULL, nFlags);
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

// Type 6's message handler: 1 its size, 2 DynObjType6_Init, 3 draws its model (if any); 6 and every
// other message answer 0.
int DynObjType6_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObj);
    case 2:
        DynObjType6_Init(pObj, pArg);
        return 0;
    case 6:
        return 0;
    case 3:
        if (pObj->obj.pModel != NULL) {
            Object_Draw(&pObj->obj);
        }
        return 0;
    default:
        return 0;
    }
}

// Type 9's message 2: as type 6's, but with no model and n14E from the definition.
void DynObjType9_Init(DynObj* pObj, DynObjSetup* pSetup) {
    DynObjNames* pNames;
    DynObjDef* pDef;

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
    pObj->n147 = 0;
    pObj->n146 = pDef->n4;
    pObj->n150 = 0;
    pObj->n14C = 0;
    pObj->n14E = pDef->n1A;
    pObj->n144 = 0;
    pObj->n142 = 0;
    pObj->uFlags = 0;
    pObj->uFlags &= ~0x01000000;
    if (pObj->uFlags & 4) {
        pObj->uFlags |= 0x04000000;
    }
    if (pObj->uFlags & 0x400) {
        pObj->uFlags |= 0x02000000;
    }
    Object_Init(&pObj->obj,NULL, 0);
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

// Type 9's message 6, the per-frame update: nothing to do.
void DynObjType9_Update(DynObj* pObj, void* pArg) {
}

// Type 9's message handler (an object with no model, never drawn): 1 its size, 2 DynObjType9_Init,
// 6 DynObjType9_Update; any other -1.
int DynObjType9_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObj);
    case 2:
        DynObjType9_Init(pObj, pArg);
        return 0;
    case 6:
        DynObjType9_Update(pObj, pArg);
        return 0;
    default:
        return -1;
    }
}

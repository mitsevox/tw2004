// GoDynObjTypes.c (our name): two more types of the course's dynamic objects (dynobj.h), types 6
// (fn_8004AD54) and 9 (fn_8004AF2C, which has no model to draw). Both set up their object the way
// type 0 does (fn_80049514), without the definition's flags. Its constant block is
// 0x80283280-0x80283288.

#include "dynobj.h"

void fn_8000ADC0(f32 (*pMtx)[4]);                                           // identity
void fn_8000C5A4(f32 (*pMtx)[4]);

// Type 6's message 2: set the object up from its definition, with no flags and n14E at -1.
void fn_8004ABBC(DynObj* pObj, DynObjSetup* pSetup) {
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
        fn_800486F4(&pObj->obj,pModel->p4, nFlags);
    } else {
        fn_800486F4(&pObj->obj,NULL, nFlags);
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

int fn_8004AD54(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObj);
    case 2:
        fn_8004ABBC(pObj, pArg);
        return 0;
    case 6:
        return 0;
    case 3:
        if (pObj->obj.pModel != NULL) {
            fn_80048894(&pObj->obj);
        }
        return 0;
    default:
        return 0;
    }
}

// Type 9's message 2: as type 6's, but with no model and n14E from the definition.
void fn_8004ADDC(DynObj* pObj, DynObjSetup* pSetup) {
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
    fn_800486F4(&pObj->obj,NULL, 0);
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

// Type 9's message 6, the per-frame update: nothing to do.
void fn_8004AF28(DynObj* pObj, void* pArg) {
}

int fn_8004AF2C(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObj);
    case 2:
        fn_8004ADDC(pObj, pArg);
        return 0;
    case 6:
        fn_8004AF28(pObj, pArg);
        return 0;
    default:
        return -1;
    }
}

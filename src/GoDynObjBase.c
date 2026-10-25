// GoDynObjBase.c (our name; TW06's and TW07's file lists name no file for it): the course's dynamic
// objects (dynobj.h): type 0, the base type, whose message handler (DynObjBase_MessageHandler) is
// also the default of types 2 and 11; type 2, an object that turns at a steady speed
// (DynObjTurning_*); and the table of the types' handlers (DynObj_GetTypeHandler).

#include "dynobj.h"
#include "camera.h"
#include "golfer.h"

int  DynObjTurning_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// Sets DYNOBJ_REMOVED once; the first time, with bNotify, also gives the object up
// (Kernel_ReleaseObject). 1: it was set now; nUnused is not read.
int DynObjBase_SetRemoved(DynObj* pObj, u8 bNotify, int nUnused) {
    u32 uFlags = pObj->uFlags;

    if (uFlags & DYNOBJ_REMOVED) {
        return 0;
    }
    pObj->uFlags = uFlags | DYNOBJ_REMOVED;
    if (bNotify) {
        Kernel_ReleaseObject(pObj);
    }
    return 1;
}

// Messages 8 and 10: take an amount off nPoints. Message 8 takes pArg's (another object's)
// nHitAmount; message 10 takes the message number itself, 10, and pArg is a number (EA's code).
int DynObjBase_TakeAmount(int nMsg, DynObj* pObj, void* pArg) {
    int nAmount;
    uptr nId;
    DynObj* pOther;

    if (!(pObj->uFlags & DYNOBJ_INACTIVE)) {
        nAmount = nMsg;
        nId = (uptr)pArg;
        if (nMsg != 10) {
            pOther = pArg;
            // fake match: EA tests the argument as a signed number, not the pointer (port: a
            // 64-bit pointer must be tested whole)
            if ((s32)nId != 0) {
                nAmount = pOther->nHitAmount;
                nId = pOther->nActorId;
            } else {
                nAmount = 0;
                nId = 0;
            }
        }
        if (nAmount > 0) {
            pObj->uFlags |= 0x08000000;
            pObj->nPoints -= (s16)nAmount;
            if (pObj->pHitScript != NULL) {
                RPN_RunScript(pObj->pHitScript, (u32)pObj->nPoints >> 31, nId & 0x7FFF, pObj);
            }
        }
    }
    return 0;
}

// Message 9: one of the object's values by nWhat (0: nPoints, 4: fValue, 6: nType, 7: nGroup, 8:
// nKey; else 0).
int DynObjBase_GetValue(DynObj* pObj, u32 nWhat) {
    switch (nWhat) {
    case 0:
        return pObj->nPoints;
    case 4:
        return pObj->fValue;
    case 6:
        return pObj->nType;
    case 7:
        return pObj->nGroup;
    case 8:
        return pObj->nKey;
    default:
        return 0;
    }
}

// Type 0's message handler, and the default of types 2 and 11: 1 its size, 2 set up
// (Kernel_InitObjectFromDef), 3 draw (DYNOBJ_HIDE_WITH_FLAG_OUT, DYNOBJ_NO_ZWRITE,
// DYNOBJ_NO_ALPHA_TEST), 4 remove (DynObjBase_SetRemoved), 5 destroy, 6 update (nothing), 7
// answers 1, 8 and 10 DynObjBase_TakeAmount, 9 DynObjBase_GetValue, 11 its heading (atan2 of
// aRot), 12 answers 0; any other -1.
int DynObjBase_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case DYNOBJ_MSG_SIZE:
        return sizeof(DynObj);
    case DYNOBJ_MSG_INIT:
        Kernel_InitObjectFromDef(pObj, pArg);
        return 0;
    case DYNOBJ_MSG_DRAW:
        if (pObj->obj.pModel != NULL &&
            (!(pObj->uFlags & DYNOBJ_HIDE_WITH_FLAG_OUT) ||
             !ViewController_GetCurrentViewController()->bFlagOut)) {
            if (pObj->uFlags & DYNOBJ_NO_ZWRITE) {
                DS_vEnableZBufferUpdate(0);
            }
            if (pObj->uFlags & DYNOBJ_NO_ALPHA_TEST) {
                DS_vSetAlphaTestMode(0, DS_COMPARE_GEQUAL, 0x80);
            }
            Object_Draw(&pObj->obj);
            if (pObj->uFlags & DYNOBJ_NO_ZWRITE) {
                DS_vEnableZBufferUpdate(1);
                RenderState_Flush();
            }
            if (pObj->uFlags & DYNOBJ_NO_ALPHA_TEST) {
                DS_vSetAlphaTestMode(1, DS_COMPARE_GEQUAL, 0x80);
            }
            if ((pObj->uFlags & DYNOBJ_NO_ZWRITE) || (pObj->uFlags & DYNOBJ_NO_ALPHA_TEST)) {
                RenderState_Flush();
            }
        }
        return 0;
    case 5:
        Object_Destroy(&pObj->obj);
        return 0;
    case 4:
        return DynObjBase_SetRemoved(pObj, 1, 1);
    case DYNOBJ_MSG_UPDATE:
        return 0;
    case 7:
        return 1;
    case 8:
    case 10:
        return DynObjBase_TakeAmount(nMsg, pObj, pArg);
    case 9:
        return DynObjBase_GetValue(pObj, (uptr)pArg);
    case 11:
        return atan2f(pObj->aRot[2], pObj->aRot[0]);
    case 12:
        return 0;
    default:
        return -1;
    }
}

// The message handler of object type nType (0, 2, 6, 9 or 11), or NULL for any other.
DynObjHandler DynObj_GetTypeHandler(int nType) {
    switch (nType) {
    case 0:
        return DynObjBase_MessageHandler;
    case 2:
        return DynObjTurning_MessageHandler;
    case DYNOBJ_TYPE_PIN:
        return DynObjPin_MessageHandler;
    case DYNOBJ_TYPE_SOUND:
        return DynObjSound_MessageHandler;
    case 11:
        return ActAnimal_MessageHandler;
    default:
        return NULL;
    }
}

// Type 2's DYNOBJ_MSG_INIT: sets up the object as type 0 does (Kernel_InitObjectFromDef), then
// takes its turning speed from its definition.
void DynObjTurning_Init(DynObjTurning* pObj, DynObjSetup* pSetup) {
    DynObjTurningDef* pDef = (DynObjTurningDef*)pSetup->pDef;

    Kernel_InitObjectFromDef(&pObj->base, pSetup);
    pObj->fSpeed = pDef->fSpeed;
}

// Type 2's DYNOBJ_MSG_UPDATE: turns the object by fSpeed degrees a second, a frame being 1/60 s.
void DynObjTurning_Update(DynObjTurning* pObj, void* pArg) {
    f32 mTurn[4][4];

    LLMath_IdentifyMat(mTurn);
    mat44flt_EulerAngles(2.0f * PI * (pObj->fSpeed / 360.0f) / 60.0f, 0.0f, 0.0f, mTurn);
    LLMath_mat44fltMultiplyList(pObj->base.obj.mRotation, mTurn, pObj->base.obj.mRotation, 4);
    Object_UpdateO2WComposed33(&pObj->base.obj);
}

// Type 2's message handler (an object that turns at a steady speed): its size, DynObjTurning_Init
// and DynObjTurning_Update; the rest as type 0 (DynObjBase_MessageHandler).
int DynObjTurning_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case DYNOBJ_MSG_SIZE:
        return sizeof(DynObjTurning);
    case DYNOBJ_MSG_INIT:
        DynObjTurning_Init((DynObjTurning*)pObj, pArg);
        return 0;
    case DYNOBJ_MSG_UPDATE:
        DynObjTurning_Update((DynObjTurning*)pObj, pArg);
        return 0;
    default:
        return DynObjBase_MessageHandler(nMsg, pObj, pArg, pArg2);
    }
}

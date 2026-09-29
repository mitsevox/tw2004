// GoDynObjBase.c (our name): the course's dynamic objects, type 0 and the types' table. Type 0's
// message handler (DynObjBase_MessageHandler) is also types 2 and 11's default;
// DynObj_GetTypeHandler finds a type's handler. Type 2 is an object that turns at a steady speed.

#include "dynobj.h"
#include "camera.h"
#include "golfer.h"

void LLMath_IdentifyMat(f32 (*pMtx)[4]);                                           // identity
void mat44flt_EulerAngles(f32 (*pMtx)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void LLMath_mat44fltMultiplyList(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void UObject_ComposeRotation(f32 (*pMtx)[4]);
int  DynObjTurning_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// Sets flag 0x04000000 once; the first time, with bNotify, also gives the object up (fn_800491C4).
// 1: it was set now; nUnused is not read.
int DynObjBase_SetRemoved(DynObj* pObj, u8 bNotify, int nUnused) {
    u32 uFlags = pObj->uFlags;

    if (uFlags & 0x04000000) {
        return 0;
    }
    pObj->uFlags = uFlags | 0x04000000;
    if (bNotify) {
        fn_800491C4(pObj);
    }
    return 1;
}

// Messages 8 and 10: take an amount off n144. Message 8 takes pArg's (another object's) n142;
// message 10 takes the message number itself, 10, and pArg is a number (EA's code).
int DynObjBase_TakeAmount(int nMsg, DynObj* pObj, void* pArg) {
    int nAmount;
    uptr nId;
    DynObj* pOther;

    if (!(pObj->uFlags & 4)) {
        nAmount = nMsg;
        nId = (uptr)pArg;
        if (nMsg != 10) {
            pOther = pArg;
            // fake match: EA tests the argument as a signed number, not the pointer (port: a
            // 64-bit pointer must be tested whole)
            if ((s32)nId != 0) {
                nAmount = pOther->n142;
                nId = pOther->n140;
            } else {
                nAmount = 0;
                nId = 0;
            }
        }
        if (nAmount > 0) {
            pObj->uFlags |= 0x08000000;
            pObj->n144 -= (s16)nAmount;
            if (pObj->p15C != NULL) {
                fn_8000EA1C(pObj->p15C, (u32)pObj->n144 >> 31, nId & 0x7FFF, pObj);
            }
        }
    }
    return 0;
}

// Message 9: one of the object's values by nWhat (0: n144, 4: f158, 6: n146, 7: n147, 8: n148; else
// 0).
int DynObjBase_GetValue(DynObj* pObj, u32 nWhat) {
    switch (nWhat) {
    case 0:
        return pObj->n144;
    case 4:
        return pObj->f158;
    case 6:
        return pObj->n146;
    case 7:
        return pObj->n147;
    case 8:
        return pObj->n148;
    default:
        return 0;
    }
}

// Type 0's message handler, and the default of types 2 and 11: 1 its size, 2 set up (fn_80049514),
// 3 draw (flag 0x200: hidden while the flagstick is out; 0x400: no z-buffer writes; 0x800: no alpha
// test), 4 remove (DynObjBase_SetRemoved), 5 destroy, 6 update (nothing), 7 answers 1, 8 and 10
// DynObjBase_TakeAmount, 9 DynObjBase_GetValue, 11 its heading (atan2 of aRot), 12 answers 0; any
// other -1.
int DynObjBase_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObj);
    case 2:
        fn_80049514(pObj, pArg);
        return 0;
    case 3:
        // flag 0x200: not while the flagstick is out
        if (pObj->obj.pModel
            != NULL &&(!(pObj->uFlags & 0x200) || !ViewController_GetCurrentViewController()->bFlagOut)) {
            if (pObj->uFlags & 0x400) {
                DS_vEnableZBufferUpdate(0);
            }
            if (pObj->uFlags & 0x800) {
                DS_vSetAlphaTestMode(0, 6, 0x80);
            }
            Object_Draw(&pObj->obj);
            if (pObj->uFlags & 0x400) {
                DS_vEnableZBufferUpdate(1);
                RenderState_Flush();
            }
            if (pObj->uFlags & 0x800) {
                DS_vSetAlphaTestMode(1, 6, 0x80);
            }
            if ((pObj->uFlags & 0x400) || (pObj->uFlags & 0x800)) {
                RenderState_Flush();
            }
        }
        return 0;
    case 5:
        Object_Destroy(&pObj->obj);
        return 0;
    case 4:
        return DynObjBase_SetRemoved(pObj, 1, 1);
    case 6:
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
    case 6:
        return DynObjType6_MessageHandler;
    case 9:
        return DynObjType9_MessageHandler;
    case 11:
        return ActAnimal_MessageHandler;
    default:
        return NULL;
    }
}

// Type 2's message 2: sets up the object as type 0 does (fn_80049514), then takes its turning speed
// from its definition.
void DynObjTurning_Init(DynObjTurning* pObj, DynObjSetup* pSetup) {
    DynObjTurningDef* pDef = (DynObjTurningDef*)pSetup->pDef;

    fn_80049514(&pObj->base, pSetup);
    pObj->fSpeed = pDef->fSpeed;
}

// Type 2's message 6: turns the object by fSpeed degrees a second, a frame being 1/60 s.
void DynObjTurning_Update(DynObjTurning* pObj, void* pArg) {
    f32 mTurn[4][4];

    LLMath_IdentifyMat(mTurn);
    mat44flt_EulerAngles(mTurn, 2.0f * PI * (pObj->fSpeed / 360.0f) / 60.0f, 0.0f, 0.0f);
    LLMath_mat44fltMultiplyList(pObj->base.obj.m0, mTurn, pObj->base.obj.m0, 4);
    UObject_ComposeRotation(pObj->base.obj.m0);
}

// Type 2's message handler (an object that turns at a steady speed): 1 its size, 2
// DynObjTurning_Init, 6 DynObjTurning_Update; the rest as type 0 (DynObjBase_MessageHandler).
int DynObjTurning_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObjTurning);
    case 2:
        DynObjTurning_Init((DynObjTurning*)pObj, pArg);
        return 0;
    case 6:
        DynObjTurning_Update((DynObjTurning*)pObj, pArg);
        return 0;
    default:
        return DynObjBase_MessageHandler(nMsg, pObj, pArg, pArg2);
    }
}

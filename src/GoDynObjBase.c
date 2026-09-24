// GoDynObjBase.c (our name): the course's dynamic objects, type 0 and the types' table. Type 0's
// message handler (fn_80049820) is also every other type's default; fn_800499B0 finds a type's
// handler. Type 2 is an object that turns at a steady speed.

#include "dynobj.h"
#include "camera.h"
#include "golfer.h"

void fn_8000ADC0(f32 (*pMtx)[4]);                                           // identity
void fn_8000A194(f32 (*pMtx)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void fn_8000C5A4(f32 (*pMtx)[4]);
int  fn_80049ACC(int nMsg, DynObj* pObj, void* pArg, void* pArg2);

// Set flag 0x04000000 once; the first time, with bNotify, also run fn_800491C4. 1: it was set now.
int fn_800496E0(DynObj* pObj, u8 bNotify, int nUnused) {
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
int fn_80049728(int nMsg, DynObj* pObj, void* pArg) {
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

// Message 9: one of the object's values.
int fn_800497BC(DynObj* pObj, u32 nWhat) {
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

int fn_80049820(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObj);
    case 2:
        fn_80049514(pObj, pArg);
        return 0;
    case 3:
        // flag 0x200: not while the flagstick is out
        if (pObj->obj.pModel != NULL &&(!(pObj->uFlags & 0x200) || !fn_80016CF4()->bFlagOut)) {
            if (pObj->uFlags & 0x400) {
                fn_80012F34(0);
            }
            if (pObj->uFlags & 0x800) {
                fn_80012F50(0, 6, 0x80);
            }
            fn_80048894(&pObj->obj);
            if (pObj->uFlags & 0x400) {
                fn_80012F34(1);
                fn_80012EF8();
            }
            if (pObj->uFlags & 0x800) {
                fn_80012F50(1, 6, 0x80);
            }
            if ((pObj->uFlags & 0x400) || (pObj->uFlags & 0x800)) {
                fn_80012EF8();
            }
        }
        return 0;
    case 5:
        fn_80048804(&pObj->obj);
        return 0;
    case 4:
        return fn_800496E0(pObj, 1, 1);
    case 6:
        return 0;
    case 7:
        return 1;
    case 8:
    case 10:
        return fn_80049728(nMsg, pObj, pArg);
    case 9:
        return fn_800497BC(pObj, (uptr)pArg);
    case 11:
        return atan2f(pObj->aRot[2], pObj->aRot[0]);
    case 12:
        return 0;
    default:
        return -1;
    }
}

DynObjHandler fn_800499B0(int nType) {
    switch (nType) {
    case 0:
        return fn_80049820;
    case 2:
        return fn_80049ACC;
    case 6:
        return fn_8004AD54;
    case 9:
        return fn_8004AF2C;
    case 11:
        return fn_8004AAEC;
    default:
        return NULL;
    }
}

void fn_80049A14(DynObjTurning* pObj, DynObjSetup* pSetup) {
    DynObjTurningDef* pDef = (DynObjTurningDef*)pSetup->pDef;

    fn_80049514(&pObj->base, pSetup);
    pObj->fSpeed = pDef->fSpeed;
}

// Turn by fSpeed degrees a second, a frame being 1/60 s.
void fn_80049A54(DynObjTurning* pObj, void* pArg) {
    f32 mTurn[4][4];

    fn_8000ADC0(mTurn);
    fn_8000A194(mTurn, 2.0f * PI * (pObj->fSpeed / 360.0f) / 60.0f, 0.0f, 0.0f);
    fn_800BADF8(pObj->base.obj.m0, mTurn, pObj->base.obj.m0, 4);
    fn_8000C5A4(pObj->base.obj.m0);
}

int fn_80049ACC(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case 1:
        return sizeof(DynObjTurning);
    case 2:
        fn_80049A14((DynObjTurning*)pObj, pArg);
        return 0;
    case 6:
        fn_80049A54((DynObjTurning*)pObj, pArg);
        return 0;
    default:
        return fn_80049820(nMsg, pObj, pArg, pArg2);
    }
}

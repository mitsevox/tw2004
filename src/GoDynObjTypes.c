// GoDynObjTypes.c: the message handlers of two types of the course's dynamic objects (dynobj.h):
// pins, a model that is only drawn, and sounds, which have no model. No object of either type is
// ever made: UKernel.c's Kernel_DownloadActors hands both kinds of actor to other code (DynObjType).

#include "dynobj.h"

// A pin's DYNOBJ_MSG_INIT: sets the object up as Kernel_InitObjectFromDef does, but with no flags
// and no group (so the flag tests below never hold), nUnread2 at -1, and the setup's first stream
// object as its model (or none), whose sizes are not copied in.
static void DynObjPin_Init(DynObj* pObj, DynObjSetup* pSetup) {
    int nObjFlags = 0;
    DynObjScripts* pScripts;
    DynObjDef* pDef;
    DynObjModelRef* pRef;

    pObj->nUnread4 = 0;
    pObj->aPairs[0].nFirst = 0;
    pObj->aPairs[1].nFirst = 0;
    pObj->aPairs[2].nFirst = 0;
    pObj->aPairs[3].nFirst = 0;
    pScripts = pSetup->pScripts;
    pDef = pSetup->pDef;
    if (pScripts != NULL) {
        pObj->pHitScript = pScripts->pHitScript;
        pObj->pReleaseScript = pScripts->pReleaseScript;
        pObj->pUnreadScript = pScripts->pUnreadScript;
    } else {
        pObj->pReleaseScript = NULL;
        pObj->pHitScript = NULL;
        pObj->pUnreadScript = NULL;
    }
    pObj->nKey = 0;
    pObj->nActorId = pDef->nActorId;
    pObj->nGroup = 0;
    pObj->nType = pDef->nType;
    pObj->nUnread3 = 0;
    pObj->nUnread1 = 0;
    pObj->nUnread2 = -1;
    pObj->nPoints = 0;
    pObj->nHitAmount = 0;
    pObj->uFlags = 0;
    pObj->uFlags &= ~DYNOBJ_UNREAD_1000000;
    if (pObj->uFlags & DYNOBJ_INACTIVE) {
        pObj->uFlags |= DYNOBJ_REMOVED;
    }
    if (pObj->uFlags & DYNOBJ_NO_ZWRITE) {
        pObj->uFlags |= DYNOBJ_UNREAD_2000000;
    }
    if (pSetup->pModel != NULL) {
        pRef = pSetup->pModel->aEntries[0].u.pRef;
    } else {
        pRef = NULL;
    }
    if (pObj->uFlags & DYNOBJ_TOBJECT_FLAGS) {
        nObjFlags |= TOBJECT_UNREAD_1 | TOBJECT_UNREAD_10;
    }
    if (pObj->uFlags & DYNOBJ_HIDE_WITH_FLAG_OUT) {
        nObjFlags |= TOBJECT_UNREAD_40;
    }
    if (pRef != NULL) {
        Object_Init(&pObj->obj, pRef->pModel, nObjFlags);
    } else {
        Object_Init(&pObj->obj, NULL, nObjFlags);
    }
    LLMath_IdentifyMat(pObj->obj.mRotation);
    LLMath_IdentifyMat(pObj->obj.mScale);
    Object_UpdateO2WComposed33(&pObj->obj);
    pObj->obj.mObjToWorld[3][0] = pDef->aPos[0];
    pObj->obj.mObjToWorld[3][1] = pDef->aPos[1];
    pObj->obj.mObjToWorld[3][2] = pDef->aPos[2];
    pObj->obj.mObjToWorld[3][3] = 1.0f;
    pObj->aRot[0] = 0.0f;
    pObj->aRot[1] = 0.0f;
    pObj->aRot[2] = 0.0f;
    pObj->aRot[3] = 1.0f;
    pObj->fValue = 0.0f;
}

// The handler of DYNOBJ_TYPE_PIN: its size, its setup (DynObjPin_Init) and drawing its model (if
// it has one); DYNOBJ_MSG_UPDATE and every other message answer 0.
int DynObjPin_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case DYNOBJ_MSG_SIZE:
        return sizeof(DynObj);
    case DYNOBJ_MSG_INIT:
        DynObjPin_Init(pObj, pArg);
        return 0;
    case DYNOBJ_MSG_UPDATE:
        return 0;
    case DYNOBJ_MSG_DRAW:
        if (pObj->obj.pModel != NULL) {
            Object_Draw(&pObj->obj);
        }
        return 0;
    default:
        return 0;
    }
}

// A sound's DYNOBJ_MSG_INIT: as a pin's, but with no model and nUnread2 from the definition.
static void DynObjSound_Init(DynObj* pObj, DynObjSetup* pSetup) {
    DynObjScripts* pScripts;
    DynObjDef* pDef;

    pObj->nUnread4 = 0;
    pObj->aPairs[0].nFirst = 0;
    pObj->aPairs[1].nFirst = 0;
    pObj->aPairs[2].nFirst = 0;
    pObj->aPairs[3].nFirst = 0;
    pScripts = pSetup->pScripts;
    pDef = pSetup->pDef;
    if (pScripts != NULL) {
        pObj->pHitScript = pScripts->pHitScript;
        pObj->pReleaseScript = pScripts->pReleaseScript;
        pObj->pUnreadScript = pScripts->pUnreadScript;
    } else {
        pObj->pReleaseScript = NULL;
        pObj->pHitScript = NULL;
        pObj->pUnreadScript = NULL;
    }
    pObj->nKey = 0;
    pObj->nActorId = pDef->nActorId;
    pObj->nGroup = 0;
    pObj->nType = pDef->nType;
    pObj->nUnread3 = 0;
    pObj->nUnread1 = 0;
    pObj->nUnread2 = pDef->nUnread;
    pObj->nPoints = 0;
    pObj->nHitAmount = 0;
    pObj->uFlags = 0;
    pObj->uFlags &= ~DYNOBJ_UNREAD_1000000;
    if (pObj->uFlags & DYNOBJ_INACTIVE) {
        pObj->uFlags |= DYNOBJ_REMOVED;
    }
    if (pObj->uFlags & DYNOBJ_NO_ZWRITE) {
        pObj->uFlags |= DYNOBJ_UNREAD_2000000;
    }
    Object_Init(&pObj->obj, NULL, 0);
    LLMath_IdentifyMat(pObj->obj.mRotation);
    LLMath_IdentifyMat(pObj->obj.mScale);
    Object_UpdateO2WComposed33(&pObj->obj);
    pObj->obj.mObjToWorld[3][0] = pDef->aPos[0];
    pObj->obj.mObjToWorld[3][1] = pDef->aPos[1];
    pObj->obj.mObjToWorld[3][2] = pDef->aPos[2];
    pObj->obj.mObjToWorld[3][3] = 1.0f;
    pObj->aRot[0] = 0.0f;
    pObj->aRot[1] = 0.0f;
    pObj->aRot[2] = 0.0f;
    pObj->aRot[3] = 1.0f;
    pObj->fValue = 0.0f;
}

// A sound's DYNOBJ_MSG_UPDATE (pArg holds the frame time's bits): nothing to do.
static void DynObjSound_Update(DynObj* pObj, void* pArg) {
}

// The handler of DYNOBJ_TYPE_SOUND (no model, so nothing to draw): its size, its setup
// (DynObjSound_Init) and its update (DynObjSound_Update); any other message answers -1.
int DynObjSound_MessageHandler(int nMsg, DynObj* pObj, void* pArg, void* pArg2) {
    switch (nMsg) {
    case DYNOBJ_MSG_SIZE:
        return sizeof(DynObj);
    case DYNOBJ_MSG_INIT:
        DynObjSound_Init(pObj, pArg);
        return 0;
    case DYNOBJ_MSG_UPDATE:
        DynObjSound_Update(pObj, pArg);
        return 0;
    default:
        return -1;
    }
}

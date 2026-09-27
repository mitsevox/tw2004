// UISEvent.c (EA's name, from its asserts): the event side of EA's UI Studio library, the menu
// screens' runtime. It runs the studio's event stack and keeps the rate functions, which move a
// screen variable towards a target over a set time.
// section order: built with -inline auto,deferred, which emits the functions last-first, so they
// are written here from the highest address down.

#include "frontend/uistudio.h"

// Pushes an event on the event stack whose top is pTop: the event record, with its type on
// the top word, then its arguments below it, the last one first. Returns the new top.
// Signature (top by value, new top returned; pStudio unused): Madden 2003 STABS
static inline s32* UISEvent_Push(UISInfoT* pStudio, s32* pTop, UISThreadGroupInfoT* pData, s16 nA, s16 nB,
                                 s32 nType, s32 nArgs, const s32* pArgs) {
    UISEvent* pEvent;
    s32* pDst;
    s32 i;

    pEvent = (UISEvent*)(pTop - 8);
    pEvent->nType = nType;
    pDst = (s32*)pEvent - 1;
    pEvent->nA = nA;
    pEvent->nB = nB;
    pEvent->data = *pData;
    pEvent->nArgs = nArgs;
    if (pArgs != NULL) {
        for (i = nArgs - 1; i >= 0; i--) {
            *pDst-- = pArgs[i];
        }
    }
    return pDst;
}

// Whether no event from p up to the stack's top waits for the screen (uA, uB), loads and unloads
// aside: a screen is unloaded only then.
// Parameter order and the nArgs local: Madden 2003 STABS
static inline u8 UISEvent_NoneWaiting(u16 uA, u16 uB, UISInfoT* pStudio, s32* p) {
    s32 nType;
    u16 uThisA;
    u16 uThisB;
    s32 nArgs;

    while (p > pStudio->ThreadInfo.pCurrentParams) {
        nType = p[0];
        uThisA = p[-1];
        uThisB = p[-2];
        if (nType != 0 && nType != 1 && uThisA == uA && uThisB == uB) return 0;
        p -= 8;
        nArgs = *p;
        p -= nArgs;
        p--;
    }
    return 1;
}

// Returns the index of a rate function, or the count when there is none.
// The functions above it in this file (compiled after it: deferred build) have it inlined.
u32 UISFindRateFnc(UISInfoT* pStudio, UISControlInfoT* pNodeInfo, u32 uId) {
    u32 i;
    UISRateFncT* pFn;

    for (i = 0; i < pStudio->NumRateFncs; i++) {
        pFn = &pStudio->RateFncs[i];
        if (pFn->RateFncID == uId && pFn->pControlInfo == pNodeInfo) break;
    }
    return i;
}

// fake match: an identity: x goes to the high word of a u64 and back down. The backend turns
// it into a chain of word copies, and each copy-propagation pass removes only one link of a chain.
// port: the (s64) conversion of a u64 relies on wrapping; callers pass pointers as u32.
static inline u32 fn_80165E9C_Read(u32 x) {
    return (u32)((u64)(s64)((u64)x << 32) >> 32);
}

// Loads a rate function that moves a variable to fTarget in uTime, replacing one with the same
// ID. The step per tick is the distance left divided by the number of ticks.
void UISLoadAdvRateFnc(UISInfoT* pStudio, UISScreenT* pScreen, UISControlInfoT* pNodeInfo, s32 n30, u32 uId,
                       u8* pDoneScript, u8* pStepScript, u32 uTime, f32 fTarget, u32 u20) {
    char szMsg[256];
    u32 i;
    UISRateFncT* pRateFn;
    f32* p;

    // fake match: pStudio goes through six identity reads (the value is unchanged). The copies
    // they leave reach the first scheduling pass, which then puts the string pool's base ahead
    // of pStudio's saved copy (the original's addi r29 before mr r30,r3).
    // port: pStudio is passed as a u32.
    pStudio = (UISInfoT*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UISInfoT*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UISInfoT*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UISInfoT*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UISInfoT*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UISInfoT*)fn_80165E9C_Read((u32)pStudio);
    if (uTime == 0) {
        RuntimeErrorFnc(1, "UISEvent.c", 97,
                     "Attempting to load rate function with duration 0 ms.  Rate function not loaded");
        return;
    }
    if (pScreen->bWaitingToBeUnloaded) {
        sprintf(szMsg,
                "Attempt to load rate function (ID: %d) ignored.  "
                "The screen (Group: %d, Screen: %d) is being unloaded.",
                uId, pScreen->GroupID, pScreen->ScreenID);
        RuntimeErrorFnc(0, "UISEvent.c", 107, szMsg);
        return;
    }
    i = UISFindRateFnc(pStudio, pNodeInfo, uId);
    if (i == pStudio->NumRateFncs) {
        pStudio->NumRateFncs++;
    }
    pRateFn = &pStudio->RateFncs[i];
    pRateFn->pControlInfo = pNodeInfo;
    pRateFn->AnimationData.n30 = n30;
    pRateFn->pScreen = pScreen;
    pRateFn->RateFncID = uId;
    pRateFn->pFnc = pStepScript;
    pRateFn->MSRate = pStudio->MSPerTick;
    pRateFn->MSCount = 0;
    pRateFn->MSLastCount = 0;
    pRateFn->State = 0;
    pRateFn->AnimationData.iType = u20;
    pRateFn->AnimationData.fEndValue = fTarget;
    pRateFn->AnimationData.pEndFnc = pDoneScript;
    p = UISGetActionPtrValue(pRateFn->AnimationData.iType, pRateFn->AnimationData.pSubControlInfo);
    // fake match: p goes through s64 and back, then its word is swapped into the high half of a
    // u64 and back (the value is unchanged). The first leaves copies of the call's result that
    // reach the first scheduling pass (the original's lis r4 before lwz r0 and the load in f1);
    // the OR with the zero low word makes constant propagation run, and the load deletion after it
    // drops the unused zero words the identity reads above leave in the entry block.
    // port: p is passed as a u32.
    p = (f32*)(u32)(s64)(s32)p;
    p = (f32*)(u32)((((u64)(u32)p << 32) | (u64)(u32)p) >> 32);
    pRateFn->AnimationData.fStepValue = (fTarget - *p) / ((f32)uTime / (f32)pStudio->MSPerTick);
}

// Loads a rate function with no duration, replacing one with the same ID. Refused while the
// screen is being unloaded.
void UISLoadRateFnc(UISInfoT* pStudio, UISScreenT* pScreen, UISControlInfoT* pNodeInfo, u32 uId,
                    u8* pStepScript, u32 u10) {
    char szMsg[256];
    u32 i;
    UISRateFncT* pRateFn;

    if (pScreen->bWaitingToBeUnloaded) {
        sprintf(szMsg,
                "Attempt to load rate function (ID: %d) ignored.  "
                "The screen (Group: %d, Screen: %d) is being unloaded.",
                uId, pScreen->GroupID, pScreen->ScreenID);
        RuntimeErrorFnc(0, "UISEvent.c", 151, szMsg);
        return;
    }
    i = UISFindRateFnc(pStudio, pNodeInfo, uId);
    if (i == pStudio->NumRateFncs) {
        pStudio->NumRateFncs++;
    }
    pRateFn = &pStudio->RateFncs[i];
    pRateFn->pControlInfo = pNodeInfo;
    pRateFn->RateFncID = uId;
    pRateFn->pFnc = pStepScript;
    pRateFn->MSRate = u10;
    pRateFn->MSCount = 0;
    pRateFn->MSLastCount = 0;
    pRateFn->pScreen = pScreen;
    pRateFn->State = 0;
    pRateFn->AnimationData.iType = 0;
    pRateFn->AnimationData.fEndValue = 0.0f;
    pRateFn->AnimationData.pEndFnc = NULL;
}

// Marks a rate function as finished.
void UISUnloadRateFnc(UISInfoT* pStudio, UISControlInfoT* pNodeInfo, u32 uId) {
    u32 i;
    u32 n = pStudio->NumRateFncs;

    i = UISFindRateFnc(pStudio, pNodeInfo, uId);
    if (i < n) {
        pStudio->RateFncs[i].State = 1;
    }
}

// Drops the rate functions that have finished and marks the new ones as running.
void UISRemoveUnNessaryRateFncs(UISInfoT* pStudio) {
    int i;
    int j;

    i = pStudio->NumRateFncs;
    while (i-- != 0) {
        if (pStudio->RateFncs[i].State == 1) {
            pStudio->NumRateFncs--;
            for (j = i; j < pStudio->NumRateFncs; j++) {
                memmove(&pStudio->RateFncs[j], &pStudio->RateFncs[j + 1], sizeof(UISRateFncT));
            }
        } else if (pStudio->RateFncs[i].State == 0) {
            pStudio->RateFncs[i].State = 2;
        }
    }
}

UISRuntimeErrorFncT* RuntimeErrorFnc;

void UISRegisterRuntimeErrorFnc(UISRuntimeErrorFncT* pfnReport) {
    RuntimeErrorFnc = pfnReport;
}

// Pushes an event on the studio's event stack: the event record, with its type on the top word,
// then its arguments below it, the last one first.
void UISAddThreadAction(s16 nA, s16 nB, UISInfoT* pStudio, s32 nType, UISThreadGroupInfoT* pData, s32 nArgs,
                        const s32* pArgs) {
    // fake match: nArgs goes through s64 and back (the value is unchanged); the dead high word
    // lives until register allocation and gives the original's order of the nA/nB sign extensions
    pStudio->ThreadInfo.pCurrentParams = UISEvent_Push(pStudio, pStudio->ThreadInfo.pCurrentParams, pData, nA,
                                                       nB, nType, (s64)nArgs, pArgs);
}

// Walks the event stack from the bottom up and, for each type 9 event queued for the given
// screen, while that screen is still loaded, calls UISDoHint with the event's first data word
// and its arguments.
s32 UISThreadProcessHints(UISInfoT* pStudio, u16 uGroup, u16 uScreen) {
    s32* pData;
    s32 nArgs;
    s32* pArgs;
    u16 uB;
    s32 nType;
    u16 uA;
    s32* p;

    p = pStudio->ThreadInfo.pBeginParams;
    while (p > pStudio->ThreadInfo.pCurrentParams) {
        nType = p[0];
        uA = p[-1];
        uB = p[-2];
        pData = p -= 7;
        nArgs = p[-1];
        p -= 1;
        p -= nArgs;
        pArgs = p;
        p -= 1;
        if (uA == uGroup && uB == uScreen && nType == 9
            && UISFindScreen(pStudio, uA, uB) < pStudio->NumScreens) {
            // fake match: *pData and nArgs go through s64 and back (the values are unchanged);
            // the two dead high words live until register allocation and give p, pStudio and
            // uScreen the extra neighbours that put them in the original's saved registers
            UISDoHint(pStudio, (s64)*pData, (s64)nArgs, pArgs);
        }
    }
    return 1;
}

// Runs the event whose type word is at pTop and returns the next one's. An event that cannot run
// yet (unloading a screen that still has events queued, or that UISInternalUnloadScreen refuses) is pushed
// again on the stack at *ppKeep.
// fake match: optimization level 3 for this function only: at level 4 an extra copy-propagation
// pass folds the copies of uA and uB that UISEvent_NoneWaiting's loop compares with (the original
// keeps them: mr r4,r28; mr r3,r27)
#pragma optimization_level 3
s32* _UISDoThreadAction(UISInfoT* pStudio, s32* pTop, s32** ppKeep) {
    s32 nType;
    UISThreadGroupInfoT* pData;
    s32 nArgs;
    s32* pArgs;
    u16 uA;
    u16 uB;
    u32 nIndex;
    UISScreenT* pScreen;

    // fake match: pTop goes through u64 and back (the value is unchanged); the copy chain keeps
    // the original's copy of the parameter (mr r25,r4) through the copy-propagation passes
    // port: a 32-bit pointer through u32; drop this line in a port
    pTop =(s32*)(u32)((u64)(s64)((u64)(u32)pTop << 32) >> 32);
    nType = pTop[0];
    uA = pTop[-1];
    uB = pTop[-2];
    pData = (UISThreadGroupInfoT*)(pTop -= 7);
    pTop--;
    nArgs = *pTop;
    pTop -= nArgs;
    pArgs = pTop;
    pTop--;
    switch (nType) {
    case 0:
        UISInternalLoadScreen(pStudio, pData->ScreenInfo.GroupID, pData->ScreenInfo.ScreenID,
                              pData->ScreenInfo.ParentGroupID, pData->ScreenInfo.ParentScreenID, nArgs,
                              pArgs);
        break;
    case 1:
        nIndex = UISFindScreen(pStudio, pData->ScreenInfo.GroupID, pData->ScreenInfo.ScreenID);
        if (nIndex < pStudio->NumScreens) {
            pScreen = &pStudio->Screens[nIndex];
            pScreen->bWaitingToBeUnloaded = 1;
        }
        if (!UISEvent_NoneWaiting(uA, uB, pStudio, pTop)) {
            *ppKeep = UISEvent_Push(pStudio, *ppKeep, pData, uA, uB, nType, nArgs, pArgs);
        } else if (!UISInternalUnloadScreen(pStudio, pData->ScreenInfo.GroupID, pData->ScreenInfo.ScreenID,
                                            pData->ScreenInfo.iRetVal)) {
            *ppKeep = UISEvent_Push(pStudio, *ppKeep, pData, uA, uB, nType, nArgs, pArgs);
        }
        break;
    case 9:
        if (UISFindScreen(pStudio, uA, uB) < pStudio->NumScreens) {
            UISDoHint(pStudio, pData->GenericInfo.Data[0], nArgs, pArgs);
        }
        break;
    case 2:
        pStudio->CriticalRegions |= 2;
        UISProcessInternalEvents(pStudio, &pStudio->EventStack, pData->GenericInfo.Data[0], -8, nArgs,
                                 pArgs, 1);
        pStudio->CriticalRegions &= ~2;
        break;
    case 3:
        UISInternalActivateScreen(pStudio, 1, pData->ScreenInfo.GroupID, pData->ScreenInfo.ScreenID);
        break;
    case 4:
        UISInternalActivateScreen(pStudio, 0, pData->ScreenInfo.GroupID, pData->ScreenInfo.ScreenID);
        break;
    case 5:
        if (pData->ActivateInfo.iProcessed == 0) {
            UISInternalActivateControl(pStudio, 1, pData->ActivateInfo.iDir, pData->ActivateInfo.pControlInfo,
                                       pData->ActivateInfo.pTableEntry, pData->ActivateInfo.ScreenID,
                                       pData->ActivateInfo.GroupID);
            pData->ActivateInfo.iProcessed = 1;
        }
        break;
    case 6:
        if (pData->ActivateInfo.iProcessed == 0) {
            UISInternalActivateControl(pStudio, 0, pData->ActivateInfo.iDir, pData->ActivateInfo.pControlInfo,
                                       pData->ActivateInfo.pTableEntry, pData->ActivateInfo.ScreenID,
                                       pData->ActivateInfo.GroupID);
            pData->ActivateInfo.iProcessed = 1;
        }
        break;
    case 7:
        pStudio->CriticalRegions |= 2;
        UISProcessInternalEvents(pStudio, &pStudio->EventStack, pData->MessageInfo.Controller,
                                 pData->MessageInfo.Message, nArgs, pArgs, 0);
        pStudio->CriticalRegions &= ~2;
        break;
    case 8:
        UISMoveScreenDrawPosition(pStudio, pData->ScreenInfo.GroupID, pData->ScreenInfo.ScreenID,
                                  pData->ScreenInfo.iDir);
        break;
    }
    return pTop;
}
#pragma optimization_level reset

// Runs the event stack from the bottom up. With bScreenOnly only the type 5 and 6 events run
// (each once), and they stay queued; otherwise every event runs and those that must wait are
// queued again, in order, from the bottom.
// fake match: dead-assignment removal off for this function only: with it on, nType and the
// argument count read below trade r0 and r3 (no source spelling found that does the same)
#pragma opt_dead_assignments off
void UISProcessThreadAction(UISInfoT* pStudio, u8 bScreenOnly) {
    s32* p;
    s32* pKeep;
    UISThreadGroupInfoT* pData;
    s32 nType;
    s32* pNext;

    if (pStudio->CriticalRegions & 1) return;
    pStudio->CriticalRegions |= 1;
    p = pStudio->ThreadInfo.pBeginParams;
    pKeep = p;
    if (bScreenOnly) {
        while (p > pStudio->ThreadInfo.pCurrentParams) {
            nType = p[0];
            // below the type word: the event's data, its argument count and its arguments
            pNext = p - 7;
            pData = (UISThreadGroupInfoT*)pNext;
            pNext--;
            pNext -= p[-8];
            switch (nType) {
            case 5:
                if (pData->ActivateInfo.iProcessed == 0) {
                    UISInternalActivateControl(pStudio, 1, pData->ActivateInfo.iDir,
                                               pData->ActivateInfo.pControlInfo,
                                               pData->ActivateInfo.pTableEntry, pData->ActivateInfo.ScreenID,
                                               pData->ActivateInfo.GroupID);
                    pData->ActivateInfo.iProcessed = 1;
                }
                break;
            case 6:
                if (pData->ActivateInfo.iProcessed == 0) {
                    UISInternalActivateControl(pStudio, 0, pData->ActivateInfo.iDir,
                                               pData->ActivateInfo.pControlInfo,
                                               pData->ActivateInfo.pTableEntry, pData->ActivateInfo.ScreenID,
                                               pData->ActivateInfo.GroupID);
                    pData->ActivateInfo.iProcessed = 1;
                }
                break;
            }
            p = pNext - 1;
        }
    } else {
        while (p > pStudio->ThreadInfo.pCurrentParams) {
            p = _UISDoThreadAction(pStudio, p, &pKeep);
        }
        pStudio->ThreadInfo.pCurrentParams = pKeep;
    }
    pStudio->CriticalRegions &= ~1;
}
#pragma opt_dead_assignments reset

// UISEvent.c (EA's name, from its asserts): the event side of EA's UI Studio library, the menu
// screens' runtime. It runs the studio's event stack and keeps the rate functions, which move a
// screen variable towards a target over a set time.
// section order: built with -inline auto,deferred, which emits the functions last-first, so they
// are written here from the highest address down.

#include "frontend/uistudio.h"

// Pushes an event on the event stack whose top is pLocalThreadInfo: the event record, with its type on
// the top word, then its arguments below it, the last one first. Returns the new top.
// Signature (top by value, new top returned; pInfo unused): Madden 2003 STABS
static inline s32* UISAddThreadActionAt(UISInfoT* pInfo, s32* pLocalThreadInfo,
                                        UISThreadGroupInfoT* pInputThreadInfo, s16 GroupID, s16 ScreenID,
                                        UISThreadActionT Action, s32 nParms, const s32* pParms) {
    UISEvent* pEvent;
    s32* pDst;
    s32 idxParams;

    pEvent = (UISEvent*)(pLocalThreadInfo - 8);
    pEvent->nType = Action;
    pDst = (s32*)pEvent - 1;
    pEvent->nA = GroupID;
    pEvent->nB = ScreenID;
    pEvent->data = *pInputThreadInfo;
    pEvent->nArgs = nParms;
    if (pParms != NULL) {
        for (idxParams = nParms - 1; idxParams >= 0; idxParams--) {
            *pDst-- = pParms[idxParams];
        }
    }
    return pDst;
}

// Whether no event from pLocalThreadInfo up to the stack's top waits for the screen (GroupID,
// ScreenID), loads and unloads
// aside: a screen is unloaded only then.
// Parameter order and the nParms local: Madden 2003 STABS
static inline u8 _UISCanDoUnloadAction(u16 GroupID, u16 ScreenID, UISInfoT* pInfo, s32* pLocalThreadInfo) {
    UISThreadActionT Action;
    u16 uThisA;
    u16 uThisB;
    s32 nParms;

    while (pLocalThreadInfo > pInfo->ThreadInfo.pCurrentParams) {
        Action = pLocalThreadInfo[0];
        uThisA = pLocalThreadInfo[-1];
        uThisB = pLocalThreadInfo[-2];
        if (Action != UISThreadAction_Load && Action != UISThreadAction_Unload && uThisA == GroupID
            && uThisB == ScreenID) {
            return 0;
        }
        pLocalThreadInfo -= 8;
        nParms = *pLocalThreadInfo;
        pLocalThreadInfo -= nParms;
        pLocalThreadInfo--;
    }
    return 1;
}

// Returns the index of a rate function, or the count when there is none.
// The functions above it in this file (compiled after it: deferred build) have it inlined.
u32 UISFindRateFnc(UISInfoT* pInfo, UISControlInfoT* pControlInfo, u32 RateFncID) {
    u32 RateFnc;
    UISRateFncT* pRateFnc;

    for (RateFnc = 0; RateFnc < pInfo->NumRateFncs; RateFnc++) {
        pRateFnc = &pInfo->RateFncs[RateFnc];
        if (pRateFnc->RateFncID == RateFncID && pRateFnc->pControlInfo == pControlInfo) break;
    }
    return RateFnc;
}

// fake match: an identity: x goes to the high word of a u64 and back down. The backend turns
// it into a chain of word copies, and each copy-propagation pass removes only one link of a chain.
// port: the (s64) conversion of a u64 relies on wrapping; callers pass pointers as u32.
static inline u32 fn_80165E9C_Read(u32 x) {
    return (u32)((u64)(s64)((u64)x << 32) >> 32);
}

// Loads a rate function that moves a variable to targValue in MSDur, replacing one with the same
// ID. The step per tick is the distance left divided by the number of ticks.
void UISLoadAdvRateFnc(UISInfoT* pInfo, UISScreenT* pScreen, UISControlInfoT* pControlInfo,
                       UISControlInfoT* pSubControlInfo, u32 RateFncID, u8* pEndFnc, u8* pAcelFnc, u32 MSDur,
                       f32 targValue, u32 animType) {
    char szMsg[256];
    u32 RateFnc;
    UISRateFncT* pRateFnc;
    f32* p;

    // fake match: pInfo goes through six identity reads (the value is unchanged). The copies
    // they leave reach the first scheduling pass, which then puts the string pool's base ahead
    // of pInfo's saved copy (the original's addi r29 before mr r30,r3).
    // port: pInfo is passed as a u32.
    pInfo = (UISInfoT*)fn_80165E9C_Read((u32)pInfo);
    pInfo = (UISInfoT*)fn_80165E9C_Read((u32)pInfo);
    pInfo = (UISInfoT*)fn_80165E9C_Read((u32)pInfo);
    pInfo = (UISInfoT*)fn_80165E9C_Read((u32)pInfo);
    pInfo = (UISInfoT*)fn_80165E9C_Read((u32)pInfo);
    pInfo = (UISInfoT*)fn_80165E9C_Read((u32)pInfo);
    if (MSDur == 0) {
        RuntimeErrorFnc(1, "UISEvent.c", 97,
                        "Attempting to load rate function with duration 0 ms.  Rate function not loaded");
        return;
    }
    if (pScreen->bWaitingToBeUnloaded) {
        sprintf(szMsg,
                "Attempt to load rate function (ID: %d) ignored.  "
                "The screen (Group: %d, Screen: %d) is being unloaded.",
                RateFncID, pScreen->GroupID, pScreen->ScreenID);
        RuntimeErrorFnc(0, "UISEvent.c", 107, szMsg);
        return;
    }
    RateFnc = UISFindRateFnc(pInfo, pControlInfo, RateFncID);
    if (RateFnc == pInfo->NumRateFncs) {
        pInfo->NumRateFncs++;
    }
    pRateFnc = &pInfo->RateFncs[RateFnc];
    pRateFnc->pControlInfo = pControlInfo;
    pRateFnc->AnimationData.pSubControlInfo = pSubControlInfo;
    pRateFnc->pScreen = pScreen;
    pRateFnc->RateFncID = RateFncID;
    pRateFnc->pFnc = pAcelFnc;
    pRateFnc->MSRate = pInfo->MSPerTick;
    pRateFnc->MSCount = 0;
    pRateFnc->MSLastCount = 0;
    pRateFnc->State = UISRATE_LOAD;
    pRateFnc->AnimationData.iType = animType;
    pRateFnc->AnimationData.fEndValue = targValue;
    pRateFnc->AnimationData.pEndFnc = pEndFnc;
    p = UISGetActionPtrValue(pRateFnc->AnimationData.iType, pRateFnc->AnimationData.pSubControlInfo);
    // fake match: p goes through s64 and back, then its word is swapped into the high half of a
    // u64 and back (the value is unchanged). The first leaves copies of the call's result that
    // reach the first scheduling pass (the original's lis r4 before lwz r0 and the load in f1);
    // the OR with the zero low word makes constant propagation run, and the load deletion after it
    // drops the unused zero words the identity reads above leave in the entry block.
    // port: p is passed as a u32.
    p = (f32*)(u32)(s64)(s32)p;
    p = (f32*)(u32)((((u64)(u32)p << 32) | (u64)(u32)p) >> 32);
    pRateFnc->AnimationData.fStepValue = (targValue - *p) / ((f32)MSDur / (f32)pInfo->MSPerTick);
}

// Loads a rate function with no duration, replacing one with the same ID. Refused while the
// screen is being unloaded.
void UISLoadRateFnc(UISInfoT* pInfo, UISScreenT* pScreen, UISControlInfoT* pControlInfo, u32 RateFncID,
                    u8* pFnc, u32 MSRate) {
    char szMsg[256];
    u32 RateFnc;
    UISRateFncT* pRateFnc;

    if (pScreen->bWaitingToBeUnloaded) {
        sprintf(szMsg,
                "Attempt to load rate function (ID: %d) ignored.  "
                "The screen (Group: %d, Screen: %d) is being unloaded.",
                RateFncID, pScreen->GroupID, pScreen->ScreenID);
        RuntimeErrorFnc(0, "UISEvent.c", 151, szMsg);
        return;
    }
    RateFnc = UISFindRateFnc(pInfo, pControlInfo, RateFncID);
    if (RateFnc == pInfo->NumRateFncs) {
        pInfo->NumRateFncs++;
    }
    pRateFnc = &pInfo->RateFncs[RateFnc];
    pRateFnc->pControlInfo = pControlInfo;
    pRateFnc->RateFncID = RateFncID;
    pRateFnc->pFnc = pFnc;
    pRateFnc->MSRate = MSRate;
    pRateFnc->MSCount = 0;
    pRateFnc->MSLastCount = 0;
    pRateFnc->pScreen = pScreen;
    pRateFnc->State = UISRATE_LOAD;
    pRateFnc->AnimationData.iType = 0;
    pRateFnc->AnimationData.fEndValue = 0.0f;
    pRateFnc->AnimationData.pEndFnc = NULL;
}

// Marks a rate function as finished.
void UISUnloadRateFnc(UISInfoT* pInfo, UISControlInfoT* pControlInfo, u32 RateFncID) {
    u32 RateFnc;
    u32 n = pInfo->NumRateFncs;

    RateFnc = UISFindRateFnc(pInfo, pControlInfo, RateFncID);
    if (RateFnc < n) {
        pInfo->RateFncs[RateFnc].State = UISRATE_UNLOAD;
    }
}

// Drops the rate functions that have finished and marks the new ones as running.
void UISRemoveUnNessaryRateFncs(UISInfoT* pInfo) {
    int nRateFnc;
    int nSlideFnc;

    nRateFnc = pInfo->NumRateFncs;
    while (nRateFnc-- != 0) {
        if (pInfo->RateFncs[nRateFnc].State == UISRATE_UNLOAD) {
            pInfo->NumRateFncs--;
            for (nSlideFnc = nRateFnc; nSlideFnc < pInfo->NumRateFncs; nSlideFnc++) {
                memmove(&pInfo->RateFncs[nSlideFnc], &pInfo->RateFncs[nSlideFnc + 1], sizeof(UISRateFncT));
            }
        } else if (pInfo->RateFncs[nRateFnc].State == UISRATE_LOAD) {
            pInfo->RateFncs[nRateFnc].State = UISRATE_ACTIVE;
        }
    }
}

UISRuntimeErrorFncT* RuntimeErrorFnc;

void UISRegisterRuntimeErrorFnc(UISRuntimeErrorFncT* pRuntimeErrorFnc) {
    RuntimeErrorFnc = pRuntimeErrorFnc;
}

// Pushes an event on the studio's event stack: the event record, with its type on the top word,
// then its arguments below it, the last one first.
void UISAddThreadAction(s16 GroupID, s16 ScreenID, UISInfoT* pInfo, UISThreadActionT Action,
                        UISThreadGroupInfoT* pInputThreadInfo, s32 nParms, const s32* pParms) {
    // fake match: nParms goes through s64 and back (the value is unchanged); the dead high word
    // lives until register allocation and gives the original's order of the GroupID/ScreenID sign extensions
    pInfo->ThreadInfo.pCurrentParams = UISAddThreadActionAt(pInfo, pInfo->ThreadInfo.pCurrentParams,
                                                            pInputThreadInfo, GroupID, ScreenID, Action,
                                                            (s64)nParms, pParms);
}

// Walks the event stack from the bottom up and, for each type 9 event queued for the given
// screen, while that screen is still loaded, calls UISDoHint with the event's first data word
// and its arguments.
s32 UISThreadProcessHints(UISInfoT* pInfo, u16 GroupID, u16 ScreenID) {
    s32* pLoadInfo;
    s32 nParms;
    s32* pParms;
    u16 uB;
    UISThreadActionT Action;
    u16 uA;
    s32* pLocalThreadInfo;

    pLocalThreadInfo = pInfo->ThreadInfo.pBeginParams;
    while (pLocalThreadInfo > pInfo->ThreadInfo.pCurrentParams) {
        Action = pLocalThreadInfo[0];
        uA = pLocalThreadInfo[-1];
        uB = pLocalThreadInfo[-2];
        pLoadInfo = pLocalThreadInfo -= 7;
        nParms = pLocalThreadInfo[-1];
        pLocalThreadInfo -= 1;
        pLocalThreadInfo -= nParms;
        pParms = pLocalThreadInfo;
        pLocalThreadInfo -= 1;
        if (uA == GroupID && uB == ScreenID && Action == UISThreadAction_HINT
            && UISFindScreen(pInfo, uA, uB) < pInfo->NumScreens) {
            // fake match: *pLoadInfo and nParms go through s64 and back (the values are unchanged);
            // the two dead high words live until register allocation and give pLocalThreadInfo, pInfo and
            // ScreenID the extra neighbours that put them in the original's saved registers
            UISDoHint(pInfo, (s64)*pLoadInfo, (s64)nParms, pParms);
        }
    }
    return 1;
}

// Runs the event whose type word is at pLocalThreadInfo and returns the next one's. An event that cannot run
// yet (unloading a screen that still has events queued, or that UISInternalUnloadScreen refuses) is pushed
// again on the stack at *pNextFrameThreadInfo.
// fake match: optimization level 3 for this function only: at level 4 an extra copy-propagation
// pass folds the copies of GroupID and ScreenID that _UISCanDoUnloadAction's loop compares with (the original
// keeps them: mr r4,r28; mr r3,r27)
#pragma optimization_level 3
s32* _UISDoThreadAction(UISInfoT* pInfo, s32* pLocalThreadInfo, s32** pNextFrameThreadInfo) {
    UISThreadActionT Action;
    UISThreadGroupInfoT* pLoadInfo;
    s32 nParms;
    s32* pParms;
    u16 GroupID;
    u16 ScreenID;
    u32 nIndex;
    UISScreenT* pScreen;

    // fake match: pLocalThreadInfo goes through u64 and back (the value is unchanged); the copy chain keeps
    // the original's copy of the parameter (mr r25,r4) through the copy-propagation passes
    // port: a 32-bit pointer through u32; drop this line in a port
    pLocalThreadInfo =(s32*)(u32)((u64)(s64)((u64)(u32)pLocalThreadInfo << 32) >> 32);
    Action = pLocalThreadInfo[0];
    GroupID = pLocalThreadInfo[-1];
    ScreenID = pLocalThreadInfo[-2];
    pLoadInfo = (UISThreadGroupInfoT*)(pLocalThreadInfo -= 7);
    pLocalThreadInfo--;
    nParms = *pLocalThreadInfo;
    pLocalThreadInfo -= nParms;
    pParms = pLocalThreadInfo;
    pLocalThreadInfo--;
    switch (Action) {
    case UISThreadAction_Load:
        UISInternalLoadScreen(pInfo, pLoadInfo->ScreenInfo.GroupID, pLoadInfo->ScreenInfo.ScreenID,
                              pLoadInfo->ScreenInfo.ParentGroupID, pLoadInfo->ScreenInfo.ParentScreenID,
                              nParms, pParms);
        break;
    case UISThreadAction_Unload:
        nIndex = UISFindScreen(pInfo, pLoadInfo->ScreenInfo.GroupID, pLoadInfo->ScreenInfo.ScreenID);
        if (nIndex < pInfo->NumScreens) {
            pScreen = &pInfo->Screens[nIndex];
            pScreen->bWaitingToBeUnloaded = 1;
        }
        if (!_UISCanDoUnloadAction(GroupID, ScreenID, pInfo, pLocalThreadInfo)) {
            *pNextFrameThreadInfo = UISAddThreadActionAt(pInfo, *pNextFrameThreadInfo, pLoadInfo, GroupID,
                                                         ScreenID, Action, nParms, pParms);
        } else if (!UISInternalUnloadScreen(pInfo, pLoadInfo->ScreenInfo.GroupID,
                                            pLoadInfo->ScreenInfo.ScreenID, pLoadInfo->ScreenInfo.iRetVal)) {
            *pNextFrameThreadInfo = UISAddThreadActionAt(pInfo, *pNextFrameThreadInfo, pLoadInfo, GroupID,
                                                         ScreenID, Action, nParms, pParms);
        }
        break;
    case UISThreadAction_HINT:
        if (UISFindScreen(pInfo, GroupID, ScreenID) < pInfo->NumScreens) {
            UISDoHint(pInfo, pLoadInfo->GenericInfo.Data[0], nParms, pParms);
        }
        break;
    case UISThreadAction_Update:
        pInfo->CriticalRegions |= 2;
        UISProcessInternalEvents(pInfo, &pInfo->EventStack, pLoadInfo->GenericInfo.Data[0], -8, nParms,
                                 pParms, 1);
        pInfo->CriticalRegions &= ~2;
        break;
    case UISThreadAction_ScreenActivate:
        UISInternalActivateScreen(pInfo, 1, pLoadInfo->ScreenInfo.GroupID, pLoadInfo->ScreenInfo.ScreenID);
        break;
    case UISThreadAction_ScreenDeactivate:
        UISInternalActivateScreen(pInfo, 0, pLoadInfo->ScreenInfo.GroupID, pLoadInfo->ScreenInfo.ScreenID);
        break;
    case UISThreadAction_ControlActivate:
        if (pLoadInfo->ActivateInfo.iProcessed == 0) {
            UISInternalActivateControl(pInfo, 1, pLoadInfo->ActivateInfo.iDir,
                                       pLoadInfo->ActivateInfo.pControlInfo,
                                       pLoadInfo->ActivateInfo.pTableEntry, pLoadInfo->ActivateInfo.ScreenID,
                                       pLoadInfo->ActivateInfo.GroupID);
            pLoadInfo->ActivateInfo.iProcessed = 1;
        }
        break;
    case UISThreadAction_ControlDeactivate:
        if (pLoadInfo->ActivateInfo.iProcessed == 0) {
            UISInternalActivateControl(pInfo, 0, pLoadInfo->ActivateInfo.iDir,
                                       pLoadInfo->ActivateInfo.pControlInfo,
                                       pLoadInfo->ActivateInfo.pTableEntry, pLoadInfo->ActivateInfo.ScreenID,
                                       pLoadInfo->ActivateInfo.GroupID);
            pLoadInfo->ActivateInfo.iProcessed = 1;
        }
        break;
    case UISThreadAction_ProcessEvent:
        pInfo->CriticalRegions |= 2;
        UISProcessInternalEvents(pInfo, &pInfo->EventStack, pLoadInfo->MessageInfo.Controller,
                                 pLoadInfo->MessageInfo.Message, nParms, pParms, 0);
        pInfo->CriticalRegions &= ~2;
        break;
    case UISThreadAction_MoveScreen:
        UISMoveScreenDrawPosition(pInfo, pLoadInfo->ScreenInfo.GroupID, pLoadInfo->ScreenInfo.ScreenID,
                                  pLoadInfo->ScreenInfo.iDir);
        break;
    }
    return pLocalThreadInfo;
}
#pragma optimization_level reset

// Runs the event stack from the bottom up. With bControlEventsOnly only the type 5 and 6 events run
// (each once), and they stay queued; otherwise every event runs and those that must wait are
// queued again, in order, from the bottom.
// fake match: dead-assignment removal off for this function only: with it on, Action and the
// argument count read below trade r0 and r3 (no source spelling found that does the same)
#pragma opt_dead_assignments off
void UISProcessThreadAction(UISInfoT* pInfo, u8 bControlEventsOnly) {
    s32* pLocalThreadInfo;
    s32* pNextFrameInfo;
    UISThreadGroupInfoT* pLoadInfo;
    UISThreadActionT Action;
    s32* pNext;

    if (pInfo->CriticalRegions & 1) return;
    pInfo->CriticalRegions |= 1;
    pLocalThreadInfo = pInfo->ThreadInfo.pBeginParams;
    pNextFrameInfo = pLocalThreadInfo;
    if (bControlEventsOnly) {
        while (pLocalThreadInfo > pInfo->ThreadInfo.pCurrentParams) {
            Action = pLocalThreadInfo[0];
            // below the type word: the event's data, its argument count and its arguments
            pNext = pLocalThreadInfo - 7;
            pLoadInfo = (UISThreadGroupInfoT*)pNext;
            pNext--;
            pNext -= pLocalThreadInfo[-8];
            switch (Action) {
            case UISThreadAction_ControlActivate:
                if (pLoadInfo->ActivateInfo.iProcessed == 0) {
                    UISInternalActivateControl(pInfo, 1, pLoadInfo->ActivateInfo.iDir,
                                               pLoadInfo->ActivateInfo.pControlInfo,
                                               pLoadInfo->ActivateInfo.pTableEntry,
                                               pLoadInfo->ActivateInfo.ScreenID,
                                               pLoadInfo->ActivateInfo.GroupID);
                    pLoadInfo->ActivateInfo.iProcessed = 1;
                }
                break;
            case UISThreadAction_ControlDeactivate:
                if (pLoadInfo->ActivateInfo.iProcessed == 0) {
                    UISInternalActivateControl(pInfo, 0, pLoadInfo->ActivateInfo.iDir,
                                               pLoadInfo->ActivateInfo.pControlInfo,
                                               pLoadInfo->ActivateInfo.pTableEntry,
                                               pLoadInfo->ActivateInfo.ScreenID,
                                               pLoadInfo->ActivateInfo.GroupID);
                    pLoadInfo->ActivateInfo.iProcessed = 1;
                }
                break;
            }
            pLocalThreadInfo = pNext - 1;
        }
    } else {
        while (pLocalThreadInfo > pInfo->ThreadInfo.pCurrentParams) {
            pLocalThreadInfo = _UISDoThreadAction(pInfo, pLocalThreadInfo, &pNextFrameInfo);
        }
        pInfo->ThreadInfo.pCurrentParams = pNextFrameInfo;
    }
    pInfo->CriticalRegions &= ~1;
}
#pragma opt_dead_assignments reset

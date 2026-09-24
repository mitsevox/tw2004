// UISEvent.c (EA's name, from its asserts): the event side of EA's UI Studio library, the menu
// screens' runtime. It runs the studio's event stack and keeps the rate functions, which move a
// screen variable towards a target over a set time.

#include "frontend/uistudio.h"

// Pushes an event on the event stack whose top is *ppTop: the event record, with its type on
// the top word, then its arguments below it, the last one first.
static inline void UISEvent_Push(s32** ppTop, s16 nA, s16 nB, s32 nType, const UISEventData* pData, s32 nArgs,
                                 const s32* pArgs) {
    UISEvent* pEvent;
    s32* pDst;
    s32 i;

    pEvent = (UISEvent*)(*ppTop - 8);
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
    *ppTop = pDst;
}

// Whether no event from p up to the stack's top waits for the screen (uA, uB), loads and unloads
// aside: a screen is unloaded only then.
static inline u8 UISEvent_NoneWaiting(UIStudio* pStudio, s32* p, u16 uA, u16 uB) {
    while (p > pStudio->pEventTop) {
        if (p[0] != 0 && p[0] != 1 && (u16)p[-1] == uA && (u16)p[-2] == uB) return 0;
        p -= 8;
        p -= *p;
        p--;
    }
    return 1;
}

// Runs the event stack from the bottom up. With bScreenOnly only the type 5 and 6 events run
// (each once), and they stay queued; otherwise every event runs and those that must wait are
// queued again, in order, from the bottom.
void fn_80165528(UIStudio* pStudio, u8 bScreenOnly) {
    s32* p;
    s32* pKeep;
    UISEventData* pData;
    s32 nType;
    s32* pNext;

    if (pStudio->uFlags & 1) return;
    pStudio->uFlags |= 1;
    p = pStudio->pEventBase;
    pKeep = p;
    if (bScreenOnly) {
        while (p > pStudio->pEventTop) {
            nType = p[0];
            // below the type word: the event's data, its argument count and its arguments
            pNext = p - 7;
            pData = (UISEventData*)pNext;
            pNext--;
            pNext -= p[-8];
            switch (nType) {
            case 5:
                if (pData->as[1] == 0) {
                    fn_80168918(pStudio, 1, pData->as[0], pData->ap[2], pData->ap[1], pData->aw[6],
                                pData->aw[7]);
                    pData->as[1] = 1;
                }
                break;
            case 6:
                if (pData->as[1] == 0) {
                    fn_80168918(pStudio, 0, pData->as[0], pData->ap[2], pData->ap[1], pData->aw[6],
                                pData->aw[7]);
                    pData->as[1] = 1;
                }
                break;
            }
            p = pNext - 1;
        }
    } else {
        while (p > pStudio->pEventTop) {
            p = fn_80165670(pStudio, p, &pKeep);
        }
        pStudio->pEventTop = pKeep;
    }
    pStudio->uFlags &= ~1;
}

// Runs the event whose type word is at pTop and returns the next one's. An event that cannot run
// yet (unloading a screen that still has events queued, or that fn_80168FC8 refuses) is pushed
// again on the stack at *ppKeep.
s32* fn_80165670(UIStudio* pStudio, s32* pTop, s32** ppKeep) {
    s32 nType;
    s32 nArgs;
    s32* pArgs;
    UISEventData* pData;
    u16 uA;
    u16 uB;
    u32 nIndex;

    nType = pTop[0];
    nArgs = pTop[-8];
    uA = pTop[-1];
    uB = pTop[-2];
    pData = (UISEventData*)(pTop - 7);
    pArgs = pTop - 8 - nArgs;
    switch (nType) {
    case 0:
        fn_80169858(pStudio, pData->aw[0], pData->aw[1], pData->aw[2], pData->aw[3], nArgs, pArgs);
        break;
    case 1:
        nIndex = fn_8016C6C4(pStudio, pData->aw[0], pData->aw[1]);
        if (nIndex < pStudio->nScreens) {
            pStudio->pScreens[nIndex].bUnloading = 1;
        }
        if (!UISEvent_NoneWaiting(pStudio, pArgs - 1, uA, uB)) {
            UISEvent_Push(ppKeep, uA, uB, nType, pData, nArgs, pArgs);
        } else if (!fn_80168FC8(pStudio, pData->aw[0], pData->aw[1], pData->au[2])) {
            UISEvent_Push(ppKeep, uA, uB, nType, pData, nArgs, pArgs);
        }
        break;
    case 9:
        if (fn_8016C6C4(pStudio, uA, uB) < pStudio->nScreens) {
            fn_8016B0F8(pStudio, pData->au[0], nArgs, pArgs);
        }
        break;
    case 2:
        pStudio->uFlags |= 2;
        fn_80168CD8(pStudio, &pStudio->stack64, pData->au[0], -8, nArgs, pArgs, 1);
        pStudio->uFlags &= ~2;
        break;
    case 3:
        fn_801686F8(pStudio, 1, pData->aw[0], pData->aw[1]);
        break;
    case 4:
        fn_801686F8(pStudio, 0, pData->aw[0], pData->aw[1]);
        break;
    case 5:
        if (pData->as[1] == 0) {
            fn_80168918(pStudio, 1, pData->as[0], pData->ap[2], pData->ap[1], pData->aw[6], pData->aw[7]);
            pData->as[1] = 1;
        }
        break;
    case 6:
        if (pData->as[1] == 0) {
            fn_80168918(pStudio, 0, pData->as[0], pData->ap[2], pData->ap[1], pData->aw[6], pData->aw[7]);
            pData->as[1] = 1;
        }
        break;
    case 7:
        pStudio->uFlags |= 2;
        fn_80168CD8(pStudio, &pStudio->stack64, pData->au[1], pData->au[0], nArgs, pArgs, 0);
        pStudio->uFlags &= ~2;
        break;
    case 8:
        fn_8016B4D4(pStudio, pData->aw[0], pData->aw[1], pData->au[3]);
        break;
    }
    return pArgs - 1;
}

// Walks the event stack from the bottom up and, for each type 9 event queued for the given
// screen, while that screen is still loaded, calls fn_8016B0F8 with the event's first data word
// and its arguments.
s32 fn_80165ACC(UIStudio* pStudio, u16 uGroup, u16 uScreen) {
    s32* p;
    s32* pData;
    s32* pArgs;
    s32 nArgs;
    s32 nType;
    u16 uA;
    u16 uB;

    p = pStudio->pEventBase;
    while (p > pStudio->pEventTop) {
        nArgs = p[-8];
        nType = p[0];
        uA = p[-1];
        uB = p[-2];
        p -= 7;
        pData = p;
        p -= 1;
        p -= nArgs;
        pArgs = p;
        p -= 1;
        if (uA == uGroup && uB == uScreen && nType == 9
            && fn_8016C6C4(pStudio, uA, uB) < pStudio->nScreens) {
            fn_8016B0F8(pStudio, *pData, nArgs, pArgs);
        }
    }
    return 1;
}

// Pushes an event on the studio's event stack: the event record, with its type on the top word,
// then its arguments below it, the last one first.
void fn_80165B90(s16 nA, s16 nB, UIStudio* pStudio, s32 nType, const UISEventData* pData, s32 nArgs,
                 const s32* pArgs) {
    UISEvent_Push(&pStudio->pEventTop, nA, nB, nType, pData, nArgs, pArgs);
}

void fn_80165C6C(UISReportFn pfnReport) {
    lbl_80282A28 = pfnReport;
}

// Drops the rate functions that have finished and marks the new ones as running.
void fn_80165C74(UIStudio* pStudio) {
    int i;
    int j;

    i = pStudio->nRateFns;
    while (i-- != 0) {
        if (pStudio->pRateFns[i].uState == 1) {
            pStudio->nRateFns--;
            for (j = i; j < pStudio->nRateFns; j++) {
                memmove(&pStudio->pRateFns[j], &pStudio->pRateFns[j + 1], sizeof(UISRateFn));
            }
        } else if (pStudio->pRateFns[i].uState == 0) {
            pStudio->pRateFns[i].uState = 2;
        }
    }
}

// Marks a rate function as finished.
void fn_80165D2C(UIStudio* pStudio, UISNodeInfo* pNodeInfo, u32 uId) {
    u32 i;

    for (i = 0; i < pStudio->nRateFns; i++) {
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].pNodeInfo == pNodeInfo) break;
    }
    if (i < pStudio->nRateFns) {
        pStudio->pRateFns[i].uState = 1;
    }
}

// Loads a rate function with no duration, replacing one with the same ID. Refused while the
// screen is being unloaded.
void fn_80165D90(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pNodeInfo, u32 uId, u8* pStepScript,
                 u32 u10) {
    char szMsg[256];
    u32 i;
    UISRateFn* pRateFn;

    if (pScreen->bUnloading) {
        sprintf(szMsg,
                "Attempt to load rate function (ID: %d) ignored.  "
                "The screen (Group: %d, Screen: %d) is being unloaded.",
                uId, pScreen->uGroup, pScreen->uScreen);
        lbl_80282A28(0, "UISEvent.c", 151, szMsg);
        return;
    }
    for (i = 0; i < pStudio->nRateFns; i++) {
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].pNodeInfo == pNodeInfo) break;
    }
    if (i == pStudio->nRateFns) {
        pStudio->nRateFns++;
    }
    pRateFn = &pStudio->pRateFns[i];
    pRateFn->pNodeInfo = pNodeInfo;
    pRateFn->uId = uId;
    pRateFn->pStepScript = pStepScript;
    pRateFn->u10 = u10;
    pRateFn->n8 = 0;
    pRateFn->nC = 0;
    pRateFn->pScreen = pScreen;
    pRateFn->uState = 0;
    pRateFn->u20 = 0;
    pRateFn->fTarget = 0.0f;
    pRateFn->pDoneScript = NULL;
}

// Loads a rate function that moves a variable to fTarget in uTime, replacing one with the same
// ID. The step per tick is the distance left divided by the number of ticks.
void fn_80165E9C(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pNodeInfo, s32 n30, u32 uId,
                 u8* pDoneScript, u8* pStepScript, u32 uTime, f32 fTarget, u32 u20) {
    char szMsg[256];
    u32 i;
    UISRateFn* pRateFn;

    if (uTime == 0) {
        lbl_80282A28(1, "UISEvent.c", 97,
                     "Attempting to load rate function with duration 0 ms.  Rate function not loaded");
        return;
    }
    if (pScreen->bUnloading) {
        sprintf(szMsg,
                "Attempt to load rate function (ID: %d) ignored.  "
                "The screen (Group: %d, Screen: %d) is being unloaded.",
                uId, pScreen->uGroup, pScreen->uScreen);
        lbl_80282A28(0, "UISEvent.c", 107, szMsg);
        return;
    }
    for (i = 0; i < pStudio->nRateFns; i++) {
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].pNodeInfo == pNodeInfo) break;
    }
    if (i == pStudio->nRateFns) {
        pStudio->nRateFns++;
    }
    pRateFn = &pStudio->pRateFns[i];
    pRateFn->pNodeInfo = pNodeInfo;
    pRateFn->n30 = n30;
    pRateFn->pScreen = pScreen;
    pRateFn->uId = uId;
    pRateFn->pStepScript = pStepScript;
    pRateFn->u10 = pStudio->uMsPerTick;
    pRateFn->n8 = 0;
    pRateFn->nC = 0;
    pRateFn->uState = 0;
    pRateFn->u20 = u20;
    pRateFn->fTarget = fTarget;
    pRateFn->pDoneScript = pDoneScript;
    pRateFn->fStep =
        (fTarget - *fn_8016C1A4(pRateFn->u20, pRateFn->pInfo)) / ((f32)uTime / (f32)pStudio->uMsPerTick);
}

// Returns the index of a rate function, or the count when there is none.
u32 fn_8016604C(UIStudio* pStudio, UISNodeInfo* pNodeInfo, u32 uId) {
    u32 i;
    UISRateFn* pFn;

    for (i = 0; i < pStudio->nRateFns; i++) {
        pFn = &pStudio->pRateFns[i];
        if (pFn->uId == uId && pFn->pNodeInfo == pNodeInfo) break;
    }
    return i;
}

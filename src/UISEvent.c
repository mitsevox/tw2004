// UISEvent.c (EA's name, from its asserts): the event side of EA's UI Studio library, the menu
// screens' runtime. It runs the studio's event stack and keeps the rate functions, which move a
// screen variable towards a target over a set time.
// section order: built with -inline auto,deferred, which emits the functions last-first, so they
// are written here from the highest address down.

#include "frontend/uistudio.h"

// Pushes an event on the event stack whose top is pTop: the event record, with its type on
// the top word, then its arguments below it, the last one first. Returns the new top.
// Signature (top by value, new top returned; pStudio unused): Madden 2003 STABS
static inline s32* UISEvent_Push(UIStudio* pStudio, s32* pTop, UISEventData* pData, s16 nA, s16 nB,
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
static inline u8 UISEvent_NoneWaiting(u16 uA, u16 uB, UIStudio* pStudio, s32* p) {
    s32 nType;
    u16 uThisA;
    u16 uThisB;
    s32 nArgs;

    while (p > pStudio->pEventTop) {
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
u32 fn_8016604C(UIStudio* pStudio, UISNodeInfo* pNodeInfo, u32 uId) {
    u32 i;
    UISRateFn* pFn;

    for (i = 0; i < pStudio->nRateFns; i++) {
        pFn = &pStudio->pRateFns[i];
        if (pFn->uId == uId && pFn->pNodeInfo == pNodeInfo) break;
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
void fn_80165E9C(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pNodeInfo, s32 n30, u32 uId,
                 u8* pDoneScript, u8* pStepScript, u32 uTime, f32 fTarget, u32 u20) {
    char szMsg[256];
    u32 i;
    UISRateFn* pRateFn;
    f32* p;

    // fake match: pStudio goes through six identity reads (the value is unchanged). The copies
    // they leave reach the first scheduling pass, which then puts the string pool's base ahead
    // of pStudio's saved copy (the original's addi r29 before mr r30,r3).
    // port: pStudio is passed as a u32.
    pStudio = (UIStudio*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UIStudio*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UIStudio*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UIStudio*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UIStudio*)fn_80165E9C_Read((u32)pStudio);
    pStudio = (UIStudio*)fn_80165E9C_Read((u32)pStudio);
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
    i = fn_8016604C(pStudio, pNodeInfo, uId);
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
    p = fn_8016C1A4(pRateFn->u20, pRateFn->pInfo);
    // fake match: p goes through s64 and back, then its word is swapped into the high half of a
    // u64 and back (the value is unchanged). The first leaves copies of the call's result that
    // reach the first scheduling pass (the original's lis r4 before lwz r0 and the load in f1);
    // the OR with the zero low word makes constant propagation run, and the load deletion after it
    // drops the unused zero words the identity reads above leave in the entry block.
    // port: p is passed as a u32.
    p = (f32*)(u32)(s64)(s32)p;
    p = (f32*)(u32)((((u64)(u32)p << 32) | (u64)(u32)p) >> 32);
    pRateFn->fStep = (fTarget - *p) / ((f32)uTime / (f32)pStudio->uMsPerTick);
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
    i = fn_8016604C(pStudio, pNodeInfo, uId);
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

// Marks a rate function as finished.
void fn_80165D2C(UIStudio* pStudio, UISNodeInfo* pNodeInfo, u32 uId) {
    u32 i;
    u32 n = pStudio->nRateFns;

    i = fn_8016604C(pStudio, pNodeInfo, uId);
    if (i < n) {
        pStudio->pRateFns[i].uState = 1;
    }
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

UISReportFn lbl_80282A28;

void fn_80165C6C(UISReportFn pfnReport) {
    lbl_80282A28 = pfnReport;
}

// Pushes an event on the studio's event stack: the event record, with its type on the top word,
// then its arguments below it, the last one first.
void fn_80165B90(s16 nA, s16 nB, UIStudio* pStudio, s32 nType, UISEventData* pData, s32 nArgs,
                 const s32* pArgs) {
    // fake match: nArgs goes through s64 and back (the value is unchanged); the dead high word
    // lives until register allocation and gives the original's order of the nA/nB sign extensions
    pStudio->pEventTop = UISEvent_Push(pStudio, pStudio->pEventTop, pData, nA, nB, nType, (s64)nArgs, pArgs);
}

// Walks the event stack from the bottom up and, for each type 9 event queued for the given
// screen, while that screen is still loaded, calls fn_8016B0F8 with the event's first data word
// and its arguments.
s32 fn_80165ACC(UIStudio* pStudio, u16 uGroup, u16 uScreen) {
    s32* pData;
    s32 nArgs;
    s32* pArgs;
    u16 uB;
    s32 nType;
    u16 uA;
    s32* p;

    p = pStudio->pEventBase;
    while (p > pStudio->pEventTop) {
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
            && fn_8016C6C4(pStudio, uA, uB) < pStudio->nScreens) {
            // fake match: *pData and nArgs go through s64 and back (the values are unchanged);
            // the two dead high words live until register allocation and give p, pStudio and
            // uScreen the extra neighbours that put them in the original's saved registers
            fn_8016B0F8(pStudio, (s64)*pData, (s64)nArgs, pArgs);
        }
    }
    return 1;
}

// Runs the event whose type word is at pTop and returns the next one's. An event that cannot run
// yet (unloading a screen that still has events queued, or that fn_80168FC8 refuses) is pushed
// again on the stack at *ppKeep.
// fake match: optimization level 3 for this function only: at level 4 an extra copy-propagation
// pass folds the copies of uA and uB that UISEvent_NoneWaiting's loop compares with (the original
// keeps them: mr r4,r28; mr r3,r27)
#pragma optimization_level 3
s32* fn_80165670(UIStudio* pStudio, s32* pTop, s32** ppKeep) {
    s32 nType;
    UISEventData* pData;
    s32 nArgs;
    s32* pArgs;
    u16 uA;
    u16 uB;
    u32 nIndex;
    UISScreen* pScreen;

    // fake match: pTop goes through u64 and back (the value is unchanged); the copy chain keeps
    // the original's copy of the parameter (mr r25,r4) through the copy-propagation passes
    // port: a 32-bit pointer through u32; drop this line in a port
    pTop =(s32*)(u32)((u64)(s64)((u64)(u32)pTop << 32) >> 32);
    nType = pTop[0];
    uA = pTop[-1];
    uB = pTop[-2];
    pData = (UISEventData*)(pTop -= 7);
    pTop--;
    nArgs = *pTop;
    pTop -= nArgs;
    pArgs = pTop;
    pTop--;
    switch (nType) {
    case 0:
        fn_80169858(pStudio, pData->aw[0], pData->aw[1], pData->aw[2], pData->aw[3], nArgs, pArgs);
        break;
    case 1:
        nIndex = fn_8016C6C4(pStudio, pData->aw[0], pData->aw[1]);
        if (nIndex < pStudio->nScreens) {
            pScreen = &pStudio->pScreens[nIndex];
            pScreen->bUnloading = 1;
        }
        if (!UISEvent_NoneWaiting(uA, uB, pStudio, pTop)) {
            *ppKeep = UISEvent_Push(pStudio, *ppKeep, pData, uA, uB, nType, nArgs, pArgs);
        } else if (!fn_80168FC8(pStudio, pData->aw[0], pData->aw[1], pData->au[2])) {
            *ppKeep = UISEvent_Push(pStudio, *ppKeep, pData, uA, uB, nType, nArgs, pArgs);
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
    return pTop;
}
#pragma optimization_level reset

// Runs the event stack from the bottom up. With bScreenOnly only the type 5 and 6 events run
// (each once), and they stay queued; otherwise every event runs and those that must wait are
// queued again, in order, from the bottom.
// fake match: dead-assignment removal off for this function only: with it on, nType and the
// argument count read below trade r0 and r3 (no source spelling found that does the same)
#pragma opt_dead_assignments off
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
#pragma opt_dead_assignments reset

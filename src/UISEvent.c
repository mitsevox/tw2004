// UISEvent.c (EA's name, from its asserts): the event side of EA's UI Studio library, the menu
// screens' runtime. It runs the studio's event stack and keeps the rate functions, which move a
// screen variable towards a target over a set time.

#include "frontend/uistudio.h"

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
            && fn_8016C6C4(pStudio, uA, uB) < (u32)pStudio->nScreens) {
            fn_8016B0F8(pStudio, *pData, nArgs, pArgs);
        }
    }
    return 1;
}

// Pushes an event on the studio's event stack: the event record, with its type on the top word,
// then its arguments below it, the last one first.
void fn_80165B90(s16 nA, s16 nB, UIStudio* pStudio, s32 nType, const UISEventData* pData, s32 nArgs,
                 const s32* pArgs) {
    UISEvent* pEvent;
    s32* pDst;
    s32 i;

    pEvent = (UISEvent*)(pStudio->pEventTop - 8);
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
    pStudio->pEventTop = pDst;
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
void fn_80165D2C(UIStudio* pStudio, u32 u18, u32 uId) {
    u32 i;

    for (i = 0; i < pStudio->nRateFns; i++) {
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].u18 == u18) break;
    }
    if (i < pStudio->nRateFns) {
        pStudio->pRateFns[i].uState = 1;
    }
}

// Loads a rate function with no duration, replacing one with the same ID. Refused while the
// screen is being unloaded.
void fn_80165D90(UIStudio* pStudio, UISScreen* pScreen, u32 u18, u32 uId, s32 n4, u32 u10) {
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
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].u18 == u18) break;
    }
    if (i == pStudio->nRateFns) {
        pStudio->nRateFns++;
    }
    pRateFn = &pStudio->pRateFns[i];
    pRateFn->u18 = u18;
    pRateFn->uId = uId;
    pRateFn->n4 = n4;
    pRateFn->u10 = u10;
    pRateFn->n8 = 0;
    pRateFn->nC = 0;
    pRateFn->pScreen = pScreen;
    pRateFn->uState = 0;
    pRateFn->n20 = 0;
    pRateFn->fTarget = 0.0f;
    pRateFn->n2C = 0;
}

// Loads a rate function that moves a variable to fTarget in uTime, replacing one with the same
// ID. The step per tick is the distance left divided by the number of ticks.
void fn_80165E9C(UIStudio* pStudio, UISScreen* pScreen, u32 u18, s32 n30, u32 uId, s32 n2C, s32 n4,
                 u32 uTime, f32 fTarget, s32 n20) {
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
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].u18 == u18) break;
    }
    if (i == pStudio->nRateFns) {
        pStudio->nRateFns++;
    }
    pRateFn = &pStudio->pRateFns[i];
    pRateFn->u18 = u18;
    pRateFn->n30 = n30;
    pRateFn->pScreen = pScreen;
    pRateFn->uId = uId;
    pRateFn->n4 = n4;
    pRateFn->u10 = pStudio->uMsPerTick;
    pRateFn->n8 = 0;
    pRateFn->nC = 0;
    pRateFn->uState = 0;
    pRateFn->n20 = n20;
    pRateFn->fTarget = fTarget;
    pRateFn->n2C = n2C;
    pRateFn->fStep =
        (fTarget - *fn_8016C1A4(pRateFn->n20, pRateFn->pInfo)) / ((f32)uTime / (f32)pStudio->uMsPerTick);
}

// Returns the index of a rate function, or the count when there is none.
u32 fn_8016604C(UIStudio* pStudio, u32 u18, u32 uId) {
    u32 i;

    for (i = 0; i < pStudio->nRateFns; i++) {
        if (pStudio->pRateFns[i].uId == uId && pStudio->pRateFns[i].u18 == u18) break;
    }
    return i;
}

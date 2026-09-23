// UISApi.c (our name): the calls the game makes into EA's UI Studio library (the menu screens'
// runtime): setting the studio up in one block of memory, handing it the game's callbacks,
// running it every frame, and queueing screen changes. A separate file from UIStudio.c: it has
// its own copies of the 0.0f and 1.0f constants.

#include "frontend/uistudio.h"

// Runs the studio for nTicks ticks, then updates every screen.
void fn_80168C24(UIStudio* pStudio, s32 nTicks) {
    u32 uMs;
    u32 i;
    u32 n;

    uMs = pStudio->uMsPerTick * nTicks;
    fn_8016C174(1.0f, 1.0f, 1.0f, 1.0f);
    fn_8016C15C(0.0f, 0.0f, 0.0f, 0.0f);
    fn_8016A030(pStudio, uMs);
    n = pStudio->nScreens;
    for (i = 0; i < n; i++) {
        fn_8016A510(pStudio, &pStudio->pScreens[i], NULL, -2);
    }
}

// Sends event uEvent to the current screen, or to every screen when bAll is set. Event -8 skips
// a screen that is being unloaded.
void fn_80168CD8(UIStudio* pStudio, UISWordStack* pStack, u32 uEvent, s32 n, u8 b, void* p, u8 bAll) {
    u32 i;
    u32 nEnd;
    UISScreen* pScreen;
    u8 bOut;

    if (bAll) {
        nEnd = pStudio->nScreens;
        i = 0;
    } else {
        i = pStudio->nCurScreen;
        nEnd = i + 1;
        if (i == -1) return;
    }
    for (; i < nEnd; i++) {
        pScreen = &pStudio->pScreens[i];
        if (n != -8 || pScreen->bUnloading != 1) {
            bOut = 0;
            fn_8016A2D4(pStudio, pScreen, pStack, 0, uEvent, n, b, p, &bOut);
        }
    }
}

// Runs the queued events, makes the screen named by the last p60 record current, then sends
// event uEvent to it (or to every screen when bAll is set) unless the screen has taken it
// already; with n < 0 it is sent again.
void fn_80168DB0(UIStudio* pStudio, u32 uEvent, s32 n, u8 b, void* p, u8 bAll) {
    s32 nLast;
    u32 i;
    u32 nEnd;
    u32 uMask;
    u32 uTaken;
    u8 bOut;

    fn_80165528(pStudio, 0);
    nLast = pStudio->n5C - 1;
    if (nLast >= 0) {
        pStudio->nCurScreen = fn_8016C6C4(pStudio, pStudio->p60[nLast].u26, pStudio->p60[nLast].u24);
    }
    if (bAll) {
        nEnd = pStudio->nScreens;
        i = 0;
    } else {
        i = pStudio->nCurScreen;
        nEnd = i + 1;
        if (i == -1) return;
    }
    uMask = 1 << uEvent;
    for (; i < nEnd; i++) {
        uTaken = pStudio->pScreens[i].uMask & uMask;
        if ((uTaken == 1 && n < 0) || uTaken == 0) {
            bOut = 0;
            pStudio->uFlags |= 2;
            fn_8016A2D4(pStudio, &pStudio->pScreens[i], &pStudio->stack64, 0, uEvent, n, b, p, &bOut);
            pStudio->uFlags &= ~2;
        }
    }
}

// Returns the current screen's group and screen IDs, 0xFFFF when there is no current screen.
void fn_80168EE8(UIStudio* pStudio, u16* puGroup, u16* puScreen) {
    if (puGroup != NULL) {
        *puGroup = 0xFFFF;
        if (pStudio->nCurScreen != -1) {
            *puGroup = pStudio->pScreens[pStudio->nCurScreen].uGroup;
        }
    }
    if (puScreen != NULL) {
        *puScreen = 0xFFFF;
        if (pStudio->nCurScreen != -1) {
            *puScreen = pStudio->pScreens[pStudio->nCurScreen].uScreen;
        }
    }
}

// Queues event 3 for a screen and runs the queue, unless an event is being sent right now.
void fn_80168F5C(UIStudio* pStudio, s16 nGroup, s16 nScreen) {
    UISEventData data;

    data.aw[0] = nGroup;
    data.aw[1] = nScreen;
    fn_80165B90(nGroup, nScreen, pStudio, 3, &data, 0, NULL);
    if (!(pStudio->uFlags & 2)) {
        fn_80165528(pStudio, 0);
    }
}

// Goes to a screen: queued as event 0 while an event is being sent, at once otherwise.
s32 fn_801694A0(UIStudio* pStudio, s16 nGroup, s16 nScreen, u8 nArgs, s32* pArgs) {
    UISEventData data;

    if (pStudio->uFlags & 2) {
        data.aw[0] = nGroup;
        data.aw[1] = nScreen;
        data.aw[2] = 0xFFFF;
        data.aw[3] = 0xFFFF;
        fn_80165B90(nGroup, nScreen, pStudio, 0, &data, nArgs, pArgs);
        return 1;
    }
    return fn_80169590(pStudio, nGroup, nScreen, 0, nArgs, pArgs);
}

// Makes pFile the studio's UI file, fixing up its offsets the first time it is seen. Returns
// whether the file can be used; a file that cannot is dropped.
u8 fn_80169520(UIStudio* pStudio, void* pFile) {
    u8 bOk;

    bOk = 0;
    if (pFile != NULL) {
        if (pFile != pStudio->pCurrent->p10) {
            bOk = fn_80169DC4(pFile);
            if (!bOk) {
                pFile = NULL;
            }
        } else {
            bOk = 1;
        }
    }
    pStudio->pCurrent->p10 = pFile;
    return bOk;
}

void fn_80169B0C(UIStudio* pStudio, s32 nIndex, UISHandlerFn pfnHandler) {
    pStudio->ppfnHandlers[nIndex] = pfnHandler;
    pStudio->nHandlers++;
}

void fn_80169B28(UIStudio* pStudio, UISTransformFn pfnTransform) {
    pStudio->pfnTransform = pfnTransform;
}

void fn_80169B30(UIStudio* pStudio, UISLoadFn pfnLoad, UISUnloadFn pfnUnload) {
    pStudio->pfnLoad = pfnLoad;
    pStudio->pfnUnload = pfnUnload;
}

void fn_80169B3C(UIStudio* pStudio, UISScreenDataFn pfnScreen28) {
    pStudio->pfnScreen28 = pfnScreen28;
}

void fn_80169B44(UIStudio* pStudio, UISCommandFn pfnCommand) {
    pStudio->pfnCommand = pfnCommand;
}

// Unloads every screen, the last one first (event 1 with one argument, 0), then marks the
// studio as no longer set up.
void fn_80169B4C(UIStudio* pStudio) {
    s32 i;
    s32 nArg;
    UISEventData data;

    pStudio->bUnloadingAll = 1;
    for (i = pStudio->nScreens - 1; i >= 0; i--) {
        nArg = 0;
        data.aw[0] = pStudio->pScreens[i].uGroup;
        data.aw[1] = pStudio->pScreens[i].uScreen;
        fn_80165B90(pStudio->pScreens[i].uGroup, pStudio->pScreens[i].uScreen, pStudio, 1, &data, 1, &nArg);
        fn_80165528(pStudio, 0);
    }
    pStudio->uMagic = 0;
    pStudio->bUnloadingAll = 0;
}

// Sets the studio up in the block at pStudio (fn_80169D90 bytes): the header, then each table in
// turn, the current record and the two word stacks.
void fn_80169C0C(UIStudio* pStudio, u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords,
                 u32 nWords2, u32 uMsPerTick) {
    u32 uOffset;

    uOffset = sizeof(UIStudio) + nScreens * sizeof(UISScreen);
    pStudio->uMagic = UIS_MAGIC;
    pStudio->uFlags = 0;
    pStudio->bUnloadingAll = 0;
    pStudio->nMaxScreens = nScreens;
    pStudio->nScreens = 0;
    pStudio->pScreens = (UISScreen*)(pStudio + 1);
    pStudio->nMaxHandlers = nHandlers;
    pStudio->nHandlers = 0;
    pStudio->ppfnHandlers = (UISHandlerFn*)((u8*)pStudio + uOffset);
    uOffset += nHandlers * sizeof(UISHandlerFn);
    pStudio->nMaxRateFns = nRateFns;
    pStudio->nRateFns = 0;
    pStudio->pRateFns = (UISRateFn*)((u8*)pStudio + uOffset);
    uOffset += nRateFns * sizeof(UISRateFn);
    pStudio->nMax60 = n60;
    pStudio->n5C = 0;
    pStudio->p60 = (UISRecord60*)((u8*)pStudio + uOffset);
    uOffset += n60 * 0x28;
    pStudio->pCurrent = (UISCurrent*)((u8*)pStudio + uOffset);
    uOffset += sizeof(UISCurrent);
    pStudio->pCurrent->nC = 0;
    pStudio->pCurrent->n0 = -1;
    pStudio->pCurrent->u4 = 0xFFFF;
    pStudio->pCurrent->u6 = 0xFFFF;
    pStudio->pCurrent->u8 = 0xFFFF;
    pStudio->pCurrent->uA = 0xFFFF;
    pStudio->pCurrent->p10 = NULL;
    pStudio->stack64.p0 = pStudio->stack64.p4 = pStudio->stack64.pC = (s32*)((u8*)pStudio + uOffset);
    pStudio->stack64.p8 = pStudio->stack64.p4 + nEventWords;
    pStudio->pEventBase = pStudio->stack64.p8 - 1;
    pStudio->pEventTop = pStudio->stack64.p8 - 1;
    pStudio->pp68 = &pStudio->stack64.p4;
    uOffset += nEventWords * sizeof(s32);
    pStudio->stack78.p0 = pStudio->stack78.p4 = pStudio->stack78.pC = (s32*)((u8*)pStudio + uOffset);
    pStudio->stack78.p8 = pStudio->stack78.p4 + nWords2;
    pStudio->uMsPerTick = uMsPerTick;
    pStudio->nCurScreen = -1;
    pStudio->pfnCommand = NULL;
    pStudio->n10 = 0;
    pStudio->pfnTransform = NULL;
    pStudio->n20 = 0;
    pStudio->pfnLoad = NULL;
    pStudio->pfnUnload = NULL;
    pStudio->pfnScreen24 = NULL;
    pStudio->pfnScreen28 = NULL;
    lbl_80282A28 = NULL;
}

// The size of the block fn_80169C0C builds, for the given table sizes. The screen table has one
// more entry's worth of room: the current record (UISCurrent, the same size).
u32 fn_80169D90(u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords, u32 nWords2) {
    u32 uSize;

    uSize = (nScreens + 1) * sizeof(UISScreen);
    uSize += nHandlers * sizeof(UISHandlerFn);
    uSize += nRateFns * sizeof(UISRateFn);
    uSize += n60 * 0x28;
    uSize += (nWords2 + nEventWords) * sizeof(s32);
    return uSize + sizeof(UIStudio);
}

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
        if ((u32)n != -8 || pScreen->bUnloading != 1) {  // fake match: the original compares unsigned
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
    UISScreen* pScreen;
    s32 nTaken;
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
    for (; i < nEnd; i++) {
        pScreen = &pStudio->pScreens[i];
        nTaken = pScreen->uMask & (1 << uEvent);
        // EA bug: the masked bit equals 1 only for event 0, so the resend works for that event alone
        if ((nTaken == 1 && n < 0) || nTaken == 0) {
            bOut = 0;
            pStudio->uFlags |= 2;
            fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, uEvent, n, b, p, &bOut);
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

// Unloads a screen. Screens that named it as their previous screen take its previous screen
// instead; it gets event -1, its queued events, nodes and rate functions are dropped, the unload
// callback frees its data and the table closes up. With no current screen left, its previous
// screen (or the last one) becomes current through event 3. Returns 0 when fn_80169308 says the
// screen cannot go yet.
s32 fn_80168FC8(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 n) {
    u32 nIndex;
    UISScreen* pScreen;
    UISScreen* pSrc;
    u16 uPrevScreen;
    u16 uPrevGroup;
    u32 i;
    UISEventData data;
    u8 bOut;

    nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
    if (nIndex < pStudio->nScreens) {
        pScreen = &pStudio->pScreens[nIndex];
        uPrevGroup = pScreen->uPrevGroup;
        uPrevScreen = pScreen->uPrevScreen;
        if (!fn_80169308(pStudio, uGroup, uScreen, n)) return 0;
        if (nIndex == pStudio->nCurScreen) {
            pStudio->nCurScreen = -1;
        }
        for (i = 0; i < pStudio->nScreens; i++) {
            pSrc = &pStudio->pScreens[i];
            if (pSrc->uPrevGroup == uGroup && pSrc->uPrevScreen == uScreen) {
                pSrc->uPrevGroup = uPrevGroup;
                pSrc->uPrevScreen = uPrevScreen;
            }
        }
        bOut = 0;
        pStudio->uFlags |= 2;
        fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, -1, -3, 0, NULL, &bOut);
        pStudio->uFlags &= ~2;
        fn_80165ACC(pStudio, uGroup, uScreen);
        fn_8016AEEC(pStudio, pScreen, 0, -3);
        fn_8016A830(pStudio, 3, pScreen, 0);
        i = pStudio->nRateFns;
        while (i-- != 0) {
            if (pStudio->pRateFns[i].pScreen == pScreen) {
                pStudio->pRateFns[i].uState = 1;
            }
        }
        fn_80165C74(pStudio);
        pStudio->pfnUnload(pScreen->uGroup, pScreen->uScreen, pScreen->pData);
        pScreen->pData = NULL;
        pStudio->nScreens--;
        for (; nIndex < pStudio->nScreens; nIndex++) {
            pScreen = &pStudio->pScreens[nIndex];
            pSrc = &pStudio->pScreens[nIndex + 1];
            memmove(pScreen, pSrc, sizeof(UISScreen));
            i = pStudio->n5C;
            while (i-- != 0) {
                if (pStudio->p60[i].pScreen == pSrc) {
                    pStudio->p60[i].pScreen = pScreen;
                }
            }
            i = pStudio->nMaxRateFns;
            while (i-- != 0) {
                if (pStudio->pRateFns[i].pScreen == pSrc) {
                    pStudio->pRateFns[i].pScreen = pScreen;
                }
            }
            if (nIndex + 1 == pStudio->nCurScreen) {
                pStudio->nCurScreen = nIndex;
            }
        }
        if (pStudio->nCurScreen == -1) {
            if (pStudio->nScreens != 0) {
                pStudio->nCurScreen = fn_8016C6C4(pStudio, uPrevGroup, uPrevScreen);
                if (pStudio->nCurScreen < pStudio->nScreens) {
                    pScreen = &pStudio->pScreens[pStudio->nCurScreen];
                } else {
                    pStudio->nCurScreen = pStudio->nScreens - 1;
                    pScreen = &pStudio->pScreens[pStudio->nCurScreen];
                }
                if (pScreen != NULL) {
                    data.aw[0] = pScreen->uGroup;
                    data.aw[1] = pScreen->uScreen;
                    fn_80165B90(data.aw[0], data.aw[1], pStudio, 3, &data, 0, NULL);
                }
            } else {
                pStudio->nCurScreen = -1;
            }
        }
    }
    return 1;
}

// Called before a screen is unloaded. If the last p60 record names the screen, it is dropped:
// the screen it holds becomes current, and its paused script runs on with n on the top of its
// stack. Returns 0 when an older record names the screen, or holds it: it cannot go yet.
u8 fn_80169308(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 n) {
    s32 i;
    UISRecord60* pRecords;
    UISRecord60* pRec;
    UISScreen* pScreen;
    s32* p1C;
    UISFrame* pFrame;

    i = pStudio->n5C;
    if (i > 0) {
        pRecords = pStudio->p60;
        pRec = &pRecords[i - 1];
        if (pRec->u24 == uScreen && pRec->u26 == uGroup) {
            pStudio->n5C = i - 1;
            if (pRec->pScreen != NULL) {
                pStudio->nCurScreen = fn_8016C6C4(pStudio, pRec->pScreen->uGroup, pRec->pScreen->uScreen);
            } else {
                pStudio->nCurScreen = -1;
                return 1;
            }
            if (pStudio->nCurScreen < pStudio->nScreens) {
                p1C = pRec->p1C;
                if (p1C != NULL) {
                    pFrame = pRec->pFrame;
                    *pFrame = pRec->frame;
                    pFrame->pC[-1] = n;
                    if (fn_80166098(pStudio, pRec->p1C, pFrame, pRec->pScreen, pRec->pDesc) != 3) {
                        pFrame->pC = p1C;
                    }
                }
            }
        } else {
            while (i-- != 0) {
                pRec = &pRecords[i];
                if (pRec->u24 == uScreen && pRec->u26 == uGroup) return 0;
                pScreen = pRec->pScreen;
                if (pScreen != NULL && pScreen->uScreen == uScreen && pScreen->uGroup == uGroup) return 0;
            }
        }
    }
    return 1;
}

// Goes to a screen: queued as event 0 while an event is being sent, at once otherwise.
s32 fn_801694A0(UIStudio* pStudio, u16 uGroup, u16 uScreen, u8 nArgs, s32* pArgs) {
    UISEventData data;

    if (pStudio->uFlags & 2) {
        data.aw[0] = uGroup;
        data.aw[1] = uScreen;
        data.aw[2] = 0xFFFF;
        data.aw[3] = 0xFFFF;
        fn_80165B90(uGroup, uScreen, pStudio, 0, &data, nArgs, pArgs);
    } else {
        return fn_80169590(pStudio, uGroup, uScreen, 0, nArgs, pArgs);
    }
    return 1;
}

// Makes pFile the studio's UI file, fixing up its offsets the first time it is seen. Returns
// whether the file can be used; a file that cannot is dropped.
u8 fn_80169520(UIStudio* pStudio, UISFile* pFile) {
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

// Brings back a screen that is already loaded: its rate functions are finished, its first node
// is marked active again and it gets event -2 with the arguments (nArgs 0xFF: the count is in
// pArgs[10]). Returns 0: nothing new was loaded.
static inline s32 Screen_BringBack(UIStudio* pStudio, u16 uGroup, u16 uScreen, u8 nArgs, s32* pArgs) {
    UISScreen* pScreen;
    u32 i;
    u32 n;
    u8 bOut;

    pScreen = &pStudio->pScreens[fn_8016C6C4(pStudio, uGroup, uScreen)];
    fn_80165C74(pStudio);
    pStudio->uFlags |= 4;
    n = pStudio->nRateFns;
    for (i = 0; i < n; i++) {
        if (pStudio->pRateFns[i].pScreen == pScreen) {
            pStudio->pRateFns[i].uState = 1;
        }
    }
    pStudio->uFlags &= ~4;
    fn_80165C74(pStudio);
    pScreen->pData->pNodes[0].pDesc->n4 = 1;
    bOut = 0;
    if (nArgs == 0xFF) {
        nArgs = pArgs[10];
    }
    pStudio->uFlags |= 2;
    fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, -1, -2, nArgs, pArgs, &bOut);
    pStudio->uFlags &= ~2;
    return 0;
}

// Goes to a screen. A loaded one is brought back; otherwise it is loaded, and with bPush (forced
// while p60 holds records) a p60 record is pushed for it that remembers the screen that was
// current and the new screen's first node. Returns what fn_80169858 returned.
s32 fn_80169590(UIStudio* pStudio, u16 uGroup, u16 uScreen, u8 bPush, u8 nArgs, s32* pArgs) {
    u32 nRecord;
    s16 nPrevGroup;
    s16 nPrevScreen;
    u8 bLoaded;
    s32 nResult;
    UISRecord60* pRec;
    u16 uIndex;

    nRecord = 0;
    if (pStudio->nScreens == 0 || pStudio->nCurScreen == -1) {
        nPrevGroup = -1;
        nPrevScreen = -1;
    } else {
        nPrevGroup = pStudio->pScreens[pStudio->nCurScreen].uGroup;
        nPrevScreen = pStudio->pScreens[pStudio->nCurScreen].uScreen;
    }
    bLoaded = fn_8016C6C4(pStudio, uGroup, uScreen) < pStudio->nScreens;
    if (bLoaded == 1) {
        return Screen_BringBack(pStudio, uGroup, uScreen, nArgs, pArgs);
    }
    if (pStudio->n5C != 0 && !bLoaded) {
        bPush = 1;
    }
    if (bPush) {
        nRecord = pStudio->n5C;
        pRec = &pStudio->p60[nRecord];
        memset(pRec, 0, sizeof(UISRecord60));
        pRec->u26 = uGroup;
        pRec->u24 = uScreen;
        pStudio->n5C++;
    }
    nResult = fn_80169858(pStudio, uGroup, uScreen, nPrevGroup, nPrevScreen, nArgs, pArgs);
    if (bPush) {
        uIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
        if (uIndex < (s32)pStudio->nScreens) {  // fake match: this compare is signed
            pRec = &pStudio->p60[nRecord];
            pRec->pDesc = pStudio->pScreens[uIndex].pData->pNodes[0].pDesc;
            uIndex = fn_8016C6C4(pStudio, nPrevGroup, nPrevScreen);
            if (uIndex < (s32)pStudio->nScreens) {  // fake match: this compare is signed
                pRec->pScreen = &pStudio->pScreens[uIndex];
            }
        } else {
            pStudio->n5C--;
        }
    }
    return nResult;
}

// Loads a screen and makes it the newest in the table, with uPrevGroup and uPrevScreen as the
// screen to go back to. Its UI file is fixed up (event -9 when that was its first time), its
// nodes are set up and it gets event -2 with the arguments. A loaded screen is brought back
// instead. Returns 0 when the load callback gave nothing or the screen was already loaded.
s32 fn_80169858(UIStudio* pStudio, u16 uGroup, u16 uScreen, u16 uPrevGroup, u16 uPrevScreen, u8 nArgs,
                s32* pArgs) {
    u32 nIndex;
    u32 uFile;
    UISScreen* pScreen;
    u8 bFixed;
    u8 bOut;

    nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
    if (nIndex < pStudio->nScreens) {
        return Screen_BringBack(pStudio, uGroup, uScreen, nArgs, pArgs);
    }
    uFile = pStudio->pfnLoad(uGroup, uScreen);
    if (uFile == 0) return 0;
    pStudio->nScreens++;
    pScreen = &pStudio->pScreens[nIndex];
    pScreen->uGroup = uGroup;
    pScreen->uScreen = uScreen;
    pScreen->uPrevGroup = uPrevGroup;
    pScreen->uPrevScreen = uPrevScreen;
    pScreen->uMask = 0;
    pScreen->bUnloading = 0;
    pScreen->pData = (UISFile*)uFile;  // port: the load callback returns the file's address as a u32
    bFixed = fn_80169DC4(pScreen->pData);
    fn_8016AEEC(pStudio, pScreen, 0, -1);
    pScreen->pData->pNodes[0].pDesc->n4 = 1;
    if (bFixed) {
        bOut = 0;
        pStudio->uFlags |= 2;
        fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, -1, -9, 0, NULL, &bOut);
        pStudio->uFlags &= ~2;
    }
    fn_8016A830(pStudio, 0, pScreen, 0);
    bOut = 0;
    if (nArgs == 0xFF) {
        nArgs = pArgs[10];
    }
    pStudio->uFlags |= 2;
    fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, -1, -2, nArgs, pArgs, &bOut);
    pStudio->uFlags &= ~2;
    return 1;
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
    UISScreen* pScreen;
    s32 nArg;
    UISEventData data;
    u16 uGroup;
    u16 uScreen;

    pStudio->bUnloadingAll = 1;
    for (i = pStudio->nScreens - 1; i >= 0; i--) {
        pScreen = &pStudio->pScreens[i];
        uScreen = pScreen->uScreen;
        uGroup = pScreen->uGroup;
        nArg = 0;
        data.aw[0] = uGroup;
        data.aw[1] = uScreen;
        fn_80165B90(uGroup, uScreen, pStudio, 1, &data, 1, &nArg);
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
    uOffset += n60 * sizeof(UISRecord60);
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
    uSize += n60 * sizeof(UISRecord60);
    uSize += (nWords2 + nEventWords) * sizeof(s32);
    return uSize + sizeof(UIStudio);
}

// Turns a file offset stored in a pointer field into the pointer.
// port: the UI file keeps 32-bit offsets in its pointer fields.
static inline void* UISFile_Fix(UISFile* pFile, void* p) {
    return (u8*)pFile + (uptr)p;
}

// Fixes up a UI file the first time it is seen: every offset in it becomes a pointer, and each
// link word names its pEntriesC entry by pointer (0 when out of range). Returns 1, or -1 when the
// file was already fixed up (its node table then lies after its start).
s32 fn_80169DC4(UISFile* pFile) {
    u32 i;
    u32 j;
    u32 k;
    UISNode* pNode;
    UISNodeList* pList;
    UISNodeItem* pItem;
    u32* pLink;

    if ((u8*)pFile->pNodes < (u8*)pFile) {
        pFile->pNodes = (UISNode*)UISFile_Fix(pFile, pFile->pNodes);
        for (i = 0; i < pFile->nNodes; i++) {
            pNode = &pFile->pNodes[i];
            pNode->pDesc = (UISNodeDesc*)UISFile_Fix(pFile, pNode->pDesc);
            pNode->ppLists = (UISNodeList**)UISFile_Fix(pFile, pNode->ppLists);
            j = pNode->nLists;
            while (j-- != 0) {
                pNode->ppLists[j] = (UISNodeList*)UISFile_Fix(pFile, pNode->ppLists[j]);
                pList = pNode->ppLists[j];
                pList->p0 = UISFile_Fix(pFile, pList->p0);
                pList->pItems = (UISNodeItem*)UISFile_Fix(pFile, pList->pItems);
                k = pList->nItems;
                while (k-- != 0) {
                    pItem = &pList->pItems[k];
                    if (pItem->uId != 0xFFFF) {
                        pItem->u2 = 0;
                        pItem->p4 = UISFile_Fix(pFile, pItem->p4);
                    }
                }
            }
            pNode->pHandlers = (UISHandler*)UISFile_Fix(pFile, pNode->pHandlers);
            k = pNode->nHandlers;
            while (k-- != 0) {
                if (pNode->pHandlers[k].uEvent != 0xFFFF) {
                    pNode->pHandlers[k].u4 = (uptr)UISFile_Fix(pFile, (void*)pNode->pHandlers[k].u4);
                }
            }
        }
        pFile->pEntriesC = (UISFileEntryC*)UISFile_Fix(pFile, pFile->pEntriesC);
        i = pFile->nEntriesC;
        while (i-- != 0) {
            pFile->pEntriesC[i].p8 = UISFile_Fix(pFile, pFile->pEntriesC[i].p8);
        }
        pFile->pLinks = (u32*)UISFile_Fix(pFile, pFile->pLinks);
        i = pFile->nLinks;
        while (i-- != 0) {
            pLink = (u32*)((u8*)pFile + pFile->pLinks[i]);
            if (*pLink < pFile->nEntriesC) {
                *pLink = (uptr)&pFile->pEntriesC[*pLink];  // port: a pointer stored in a 32-bit word
            } else {
                *pLink = 0;
            }
        }
        pFile->pEntries1C = (UISFileEntry1C*)UISFile_Fix(pFile, pFile->pEntries1C);
        i = pFile->nEntries1C;
        while (i-- != 0) {
            if (pFile->pEntries1C[i].p4 != NULL) {
                pFile->pEntries1C[i].p4 = UISFile_Fix(pFile, pFile->pEntries1C[i].p4);
            }
        }
        return 1;
    }
    return -1;
}

// Runs the rate functions for uMs milliseconds. Each running one steps once per u10 ms: its
// n4 handler (if any) gives the step's scale, the variable moves by fStep times it, and once it
// reaches fTarget the function finishes and its n2C handler (or the first node's event -14
// handler) runs.
void fn_8016A030(UIStudio* pStudio, u32 uMs) {
    u32 i;
    u32 n;
    UISRateFn* pRate;
    UISScreen* pScreen;
    UISWordStack* pStack;
    s32 nLeft;
    s32 nArgs;
    u32 uHandler;
    f32 fScale;
    f32* pfVar;
    f32 fValue;
    f32 fOut;
    s32 aArgs[3];

    fn_80165C74(pStudio);
    pStack = &pStudio->stack78;
    pStudio->uFlags |= 4;
    n = pStudio->nRateFns;
    for (i = 0; i < n; i++) {
        pRate = &pStudio->pRateFns[i];
        if (pRate->uState != 2) continue;
        pScreen = pRate->pScreen;
        if (pRate->u10 == 0) continue;
        pRate->n8 += uMs;
        nLeft = pRate->n8 - pRate->nC;
        while (nLeft >= (s32)pRate->u10) {
            nLeft -= pRate->u10;
            pRate->nC += pRate->u10;
            if (pRate->uStepHandler != 0) {
                if (pRate->uState != 2) break;
                fOut = 1.0f;
                aArgs[0] = pRate->uId;
                aArgs[1] = pRate->nC;
                if (pRate->u20 == 0) {
                    nArgs = 3;
                    aArgs[2] = pRate->u10;
                } else {
                    nArgs = 2;
                }
                fn_8016C270(pStudio, pScreen, pRate->u18, pStack, pRate->uStepHandler, nArgs, aArgs, 0, 0, 0,
                            -1, &fOut);
                fScale = fOut;
            } else {
                fScale = 1.0f;
            }
            if (pRate->u20 != 0) {
                pfVar = fn_8016C1A4(pRate->u20, pRate->n30);
                if (fScale < 0.0f) {
                    fScale = 1.0f;
                }
                fValue = pRate->fStep * fScale + *pfVar;
                if ((pRate->fStep > 0.0f && fValue < pRate->fTarget)
                    || (pRate->fStep < 0.0f && fValue > pRate->fTarget)) {
                    *pfVar = fValue;
                } else {
                    *pfVar = pRate->fTarget;
                    pRate->uState = 1;
                    if (pRate->uDoneHandler != 0) {
                        aArgs[0] = pRate->uId;
                        fn_8016C270(pStudio, pScreen, pRate->u18, pStack, pRate->uDoneHandler, 1, aArgs, 0, 0,
                                    0, -1, NULL);
                    } else {
                        uHandler = fn_8016C674(pScreen->pData->pNodes, -14);
                        if (uHandler != 0) {
                            aArgs[0] = pRate->uId;
                            fn_8016C270(pStudio, pScreen, pRate->u18, pStack, uHandler, 1, aArgs, 0, 0, 0, -1,
                                        NULL);
                        }
                    }
                }
            }
        }
    }
    pStudio->uFlags &= ~4;
    fn_80165C74(pStudio);
}

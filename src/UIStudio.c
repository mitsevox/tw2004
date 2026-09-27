// UIStudio.c (EA's name, from its asserts): the core of EA's UI Studio library, the menu screens'
// runtime. It loads, activates and unloads screens and runs the events sent to them. Its own
// data: the "UIStudio.c" assert string and a warning in .rodata (0x801861A0-0x8018620C); the
// three jump tables in .data and the constants in .sdata2 (0x802851A8-0x802851B7) are UISStack.c's.
// Section note: CW's deferred inlining emits this file's functions last-defined-first, so they are
// written in reverse address order (Madden 2003's UIStudio.c has the same order).

#include "frontend/uistudio.h"

// Runs the queued events, then sends event uEvent to every screen.
void UISIdleProcess(UISInfoT* pInfo, u32 uEvent) {
    s32 idxScreen;
    s32 numScreens;
    UISScreenT* pScreen;
    u8 bProcess;

    UISProcessThreadAction(pInfo, 0);
    numScreens = pInfo->NumScreens;
    for (idxScreen = 0; idxScreen < numScreens; idxScreen++) {
        pScreen = &pInfo->Screens[idxScreen];
        bProcess = 0;
        pInfo->CriticalRegions |= 2;
        _ParseMaps(pInfo, pScreen, &pInfo->EventStack, 0, uEvent, -10, 0, NULL, &bProcess);
        pInfo->CriticalRegions &= ~2;
    }
}

// Switches (bActivate) the node with info pControlInfo in a screen on or off and runs its scripts
// for event -6 (on) or -7 (off), and those of the node that links to it by index. Switching one on
// first switches off the one UISFindSiblingEnableControl finds set. The scripts get iDir and
// pControlInfo's place in the list pTableEntry (a count, a word, then file offsets; -1 when not
// there).
void UISInternalActivateControl(UISInfoT* pInfo, u8 bActivate, s32 iDir, UISControlInfoT* pControlInfo,
                                s32* pTableEntry, u16 uScreen, u16 uGroup) {
    u32 idxControl;
    UISScreenT* pScreen;
    UISControlT* pParentControls;
    UISControlT* pTarget;
    s32 nSlot;
    void* pInfoAlias;
    UISControlInfoT* pFind;
    u32 idxEvent;
    s32 nEvent;
    u32 idxScreen;
    UISControlInfoT* pOldEnabled;
    UISControlT* pCheck;
    UISMapT* pMap; // name: Madden 2003 STABS
    s32 idxMap;
    s32 nNode16;
    void* pcEvent;
    u32 nControls;
    s32 aArgs[2];

    nSlot = -1;
    idxScreen = UISFindScreen(pInfo, uGroup, uScreen);
    pTarget = NULL;
    if (idxScreen >= pInfo->NumScreens) {
        return;
    }
    pScreen = &pInfo->Screens[idxScreen];
    nEvent = bActivate == 1 ? -6 : -7;
    idxControl = pScreen->pScrData->NumControls;
    while (idxControl-- != 0) {
        pCheck = &pScreen->pScrData->Controls[idxControl];
        if (pCheck->pControlInfo == pControlInfo) {
            pTarget = pCheck;
            break;
        }
    }
    pParentControls = NULL;
    nControls = pScreen->pScrData->NumControls;
    // fake match: EA compares (s16)idxControl directly. Set once from a 64-bit value and again (the
    // same value) in the inner loop's step, the compare's operand has two definitions: the inner
    // loop's hoisted conversion reaches it through a copy that survives copy propagation and is
    // coalesced by the register allocator, which gives EA's registers and preheader order.
    nNode16 = (s16)(s64)idxControl;
    while (nControls-- != 0) {
        pCheck = &pScreen->pScrData->Controls[nControls];
        for (idxMap = 0; idxMap < (s32)pCheck->NumMaps; idxMap++, nNode16 = (s16)idxControl) {
            pMap = &pCheck->Maps[idxMap];
            if (!(pMap->ControlIndex & 0xC000) && (pMap->ControlIndex & 0x2FFF) == nNode16) {
                pParentControls = pCheck;
                break;
            }
        }
    }
    if (bActivate == 1) {
        pOldEnabled = UISFindSiblingEnableControl(pScreen, pControlInfo);
        if (pOldEnabled != NULL) {
            UISInternalActivateControl(pInfo, 0, 0, pOldEnabled, NULL, uScreen, uGroup);
        }
    }
    // EA bug: pTarget is NULL when no node of the screen has pControlInfo; nothing checks it.
    // fake match: EA has one u32 event local (Madden 2003 STABS); a single u32 local changes the
    // register allocation, the s32 copied into a second u32 local gives EA's.
    idxEvent = (u32)nEvent;
    pcEvent = UISFindEventPC(pTarget, idxEvent);
    pTarget->pControlInfo->IsEnabled = bActivate;
    // fake match: two equal pointer values feed an OR that encodes as EA's retained mr.
    // port: uptr preserves the full pointer width outside the 32-bit GameCube build.
    pInfoAlias = pControlInfo;
    if (pTableEntry != NULL) {
        pFind = (UISControlInfoT*)((uptr)pInfoAlias | (uptr)pControlInfo);
        nSlot = pTableEntry[0];
        while (nSlot-- != 0) {
            // fake match: cancelling equal offsets keeps the slot load ahead of the data-base load.
            if (pFind == (UISControlInfoT*)((uptr)pScreen->pScrData + (uptr)pTableEntry[nSlot + 2]
                                             - (uptr)pTableEntry[nSlot + 2] + (uptr)pTableEntry[nSlot + 2])) {
                break;
            }
        }
    }
    aArgs[0] = iDir;
    aArgs[1] = nSlot;
    if (pcEvent != NULL) {
        UISExecuteFnc(pInfo, pScreen, pTarget->pControlInfo, &pInfo->EventStack, pcEvent, 2, aArgs, 0, NULL,
                      0, 0, NULL);
    }
    if (pParentControls != NULL) {
        pcEvent = UISFindSubControlEventPC(pParentControls, idxControl, idxEvent);
        if (pcEvent != NULL) {
            UISExecuteFnc(pInfo, pScreen, pParentControls->pControlInfo, &pInfo->EventStack, pcEvent, 2,
                          aArgs, 0, NULL, 0, 0, NULL);
        }
    }
}

// Activates (bActivate) a screen. With no ModalStack record open the current screen first gets event -5;
// then, unless every screen is being unloaded, the screen becomes current, its first node is
// switched on, the game hears of it (pScreenActivatedFnc) and it gets event -4. A screen waiting to be
// unloaded is refused with a warning.
void UISInternalActivateScreen(UISInfoT* pInfo, u8 bActivate, u16 GroupID, u16 ScreenID) {
    char strBuffer[512];
    UISScreenT* pScreen;
    u32 idxActiveScreen;

    if (pInfo->NumModals == 0) {
        pInfo->CriticalRegions |= 2;
        UIStudio_Send(pInfo, &pInfo->EventStack, 0, -5, 0, NULL, 0);
        pInfo->CriticalRegions &= ~2;
    }
    if (bActivate && !(u8)pInfo->bShuttingDown) {
        idxActiveScreen = UISFindScreen(pInfo, GroupID, ScreenID);
        if (idxActiveScreen < pInfo->NumScreens) {
            pScreen = &pInfo->Screens[idxActiveScreen];
            if (pScreen->bWaitingToBeUnloaded == 0) {
                pScreen->pScrData->Controls[0].pControlInfo->IsEnabled = 1;
                // port: a marker, not an owner
                pScreen->pScrData->Controls[0].pControlInfo->IsVisible = (void*)1;
                pInfo->ActiveScreenIdx = idxActiveScreen;
                if (pInfo->pScreenActivatedFnc != NULL) {
                    pInfo->pScreenActivatedFnc(pScreen->GroupID, pScreen->ScreenID);
                }
                pInfo->CriticalRegions |= 2;
                UIStudio_Send(pInfo, &pInfo->EventStack, 0, -4, 0, NULL, 0);
                pInfo->CriticalRegions &= ~2;
            } else {
                sprintf(strBuffer,
                        "Attempting to activate screen (Group ID: %d, Screen ID: %d) which is waiting to be "
                        "unloaded.\n",
                        GroupID, ScreenID);
                RuntimeErrorFnc(0, "UIStudio.c", 2942, strBuffer);
                return;
            }
        }
    }
}

// Runs _ParseVisibility on pTarget in pScreen, unless _DetermineVisibility finds pTarget's switch
// already set as uNewVisibility asks (or not at all).
void UISUpdateVisibility(UISInfoT* pInfo, UISScreenT* pScreen, s32 targType, void* pTarget,
                         s32 uNewVisibility) {
    s32 iOldVisiblity;
    s32 nOn;

    if (pScreen == NULL || pScreen->pScrData == NULL || pTarget == NULL) {
        return;
    }
    // EA passes the address of pTarget itself as the info to look for; _DetermineVisibility writes
    // the owner it finds into it.
    iOldVisiblity = _DetermineVisibility(pScreen, (UISControlInfoT*)&pTarget, 8, pScreen->pScrData->Controls);
    nOn = uNewVisibility != 0;
    if (iOldVisiblity != -1 && iOldVisiblity != nOn) {
        _ParseVisibility(pInfo, pScreen, nOn, targType, pTarget, 1);
    }
}

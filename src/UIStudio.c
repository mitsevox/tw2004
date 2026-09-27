// UIStudio.c (EA's name, from its asserts): the core of EA's UI Studio library, the menu screens'
// runtime. It loads, activates and unloads screens and runs the events sent to them. Its own
// data: the "UIStudio.c" assert string and a warning in .rodata (0x801861A0-0x8018620C); the
// three jump tables in .data and the constants in .sdata2 (0x802851A8-0x802851B7) are UISStack.c's.
// Section note: CW's deferred inlining emits this file's functions last-defined-first, so they are
// written in reverse address order (Madden 2003's UIStudio.c has the same order).

#include "frontend/uistudio.h"

// Runs the queued events, then sends event uEvent to every screen.
void UISIdleProcess(UISInfoT* pStudio, u32 uEvent) {
    s32 i;
    s32 n;
    UISScreenT* pScreen;
    u8 bOut;

    UISProcessThreadAction(pStudio, 0);
    n = pStudio->NumScreens;
    for (i = 0; i < n; i++) {
        pScreen = &pStudio->Screens[i];
        bOut = 0;
        pStudio->CriticalRegions |= 2;
        _ParseMaps(pStudio, pScreen, &pStudio->EventStack, 0, uEvent, -10, 0, NULL, &bOut);
        pStudio->CriticalRegions &= ~2;
    }
}

// Switches (bOn) the node with info pInfo in a screen on or off and runs its scripts for event
// -6 (on) or -7 (off), and those of the node that links to it by index. Switching one on first
// switches off the one UISFindSiblingEnableControl finds set. The scripts get nId and pInfo's place in the
// list p (a count, a word, then file offsets; -1 when not there).
void UISInternalActivateControl(UISInfoT* pStudio, u8 bOn, s32 nId, UISControlInfoT* pInfo, s32* p,
                                u16 uScreen, u16 uGroup) {
    u32 nNode;
    UISScreenT* pScreen;
    UISControlT* pLinkNode;
    UISControlT* pNode;
    s32 nSlot;
    void* pInfoAlias;
    UISControlInfoT* pFind;
    u32 nEventArg;
    s32 nEvent;
    u32 nIndex;
    UISControlInfoT* pOther;
    UISControlT* pCheck;
    UISMapT* pMap; // name: Madden 2003 STABS
    s32 i;
    s32 nNode16;
    void* pScript;
    u32 n;
    s32 aArgs[2];

    nSlot = -1;
    nIndex = UISFindScreen(pStudio, uGroup, uScreen);
    pNode = NULL;
    if (nIndex >= pStudio->NumScreens) {
        return;
    }
    pScreen = &pStudio->Screens[nIndex];
    nEvent = bOn == 1 ? -6 : -7;
    nNode = pScreen->pScrData->NumControls;
    while (nNode-- != 0) {
        pCheck = &pScreen->pScrData->Controls[nNode];
        if (pCheck->pControlInfo == pInfo) {
            pNode = pCheck;
            break;
        }
    }
    pLinkNode = NULL;
    n = pScreen->pScrData->NumControls;
    // fake match: EA compares (s16)nNode directly. Set once from a 64-bit value and again (the
    // same value) in the inner loop's step, the compare's operand has two definitions: the inner
    // loop's hoisted conversion reaches it through a copy that survives copy propagation and is
    // coalesced by the register allocator, which gives EA's registers and preheader order.
    nNode16 = (s16)(s64)nNode;
    while (n-- != 0) {
        pCheck = &pScreen->pScrData->Controls[n];
        for (i = 0; i < (s32)pCheck->NumMaps; i++, nNode16 = (s16)nNode) {
            pMap = &pCheck->Maps[i];
            if (!(pMap->ControlIndex & 0xC000) && (pMap->ControlIndex & 0x2FFF) == nNode16) {
                pLinkNode = pCheck;
                break;
            }
        }
    }
    if (bOn == 1) {
        pOther = UISFindSiblingEnableControl(pScreen, pInfo);
        if (pOther != NULL) {
            UISInternalActivateControl(pStudio, 0, 0, pOther, NULL, uScreen, uGroup);
        }
    }
    // EA bug: pNode is NULL when no node of the screen has pInfo; nothing checks it.
    // fake match: EA has one u32 event local (Madden 2003 STABS); a single u32 local changes the
    // register allocation, the s32 copied into a second u32 local gives EA's.
    nEventArg = (u32)nEvent;
    pScript = UISFindEventPC(pNode, nEventArg);
    pNode->pControlInfo->IsEnabled = bOn;
    // fake match: two equal pointer values feed an OR that encodes as EA's retained mr.
    // port: uptr preserves the full pointer width outside the 32-bit GameCube build.
    pInfoAlias = pInfo;
    if (p != NULL) {
        pFind = (UISControlInfoT*)((uptr)pInfoAlias | (uptr)pInfo);
        nSlot = p[0];
        while (nSlot-- != 0) {
            // fake match: cancelling equal offsets keeps the slot load ahead of the data-base load.
            if (pFind == (UISControlInfoT*)((uptr)pScreen->pScrData + (uptr)p[nSlot + 2]
                                             - (uptr)p[nSlot + 2] + (uptr)p[nSlot + 2])) {
                break;
            }
        }
    }
    aArgs[0] = nId;
    aArgs[1] = nSlot;
    if (pScript != NULL) {
        UISExecuteFnc(pStudio, pScreen, pNode->pControlInfo, &pStudio->EventStack, pScript, 2, aArgs, 0, NULL,
                      0, 0, NULL);
    }
    if (pLinkNode != NULL) {
        pScript = UISFindSubControlEventPC(pLinkNode, nNode, nEventArg);
        if (pScript != NULL) {
            UISExecuteFnc(pStudio, pScreen, pLinkNode->pControlInfo, &pStudio->EventStack, pScript, 2, aArgs,
                          0, NULL, 0, 0, NULL);
        }
    }
}

// Activates (bOn) a screen. With no ModalStack record open the current screen first gets event -5;
// then, unless every screen is being unloaded, the screen becomes current, its first node is
// switched on, the game hears of it (pScreenActivatedFnc) and it gets event -4. A screen waiting to be
// unloaded is refused with a warning.
void UISInternalActivateScreen(UISInfoT* pStudio, u8 bOn, u16 uGroup, u16 uScreen) {
    char szMsg[512];
    UISScreenT* pScreen;
    u32 nIndex;

    if (pStudio->NumModals == 0) {
        pStudio->CriticalRegions |= 2;
        UIStudio_Send(pStudio, &pStudio->EventStack, 0, -5, 0, NULL, 0);
        pStudio->CriticalRegions &= ~2;
    }
    if (bOn && !(u8)pStudio->bShuttingDown) {
        nIndex = UISFindScreen(pStudio, uGroup, uScreen);
        if (nIndex < pStudio->NumScreens) {
            pScreen = &pStudio->Screens[nIndex];
            if (pScreen->bWaitingToBeUnloaded == 0) {
                pScreen->pScrData->Controls[0].pControlInfo->IsEnabled = 1;
                // port: a marker, not an owner
                pScreen->pScrData->Controls[0].pControlInfo->IsVisible = (void*)1;
                pStudio->ActiveScreenIdx = nIndex;
                if (pStudio->pScreenActivatedFnc != NULL) {
                    pStudio->pScreenActivatedFnc(pScreen->GroupID, pScreen->ScreenID);
                }
                pStudio->CriticalRegions |= 2;
                UIStudio_Send(pStudio, &pStudio->EventStack, 0, -4, 0, NULL, 0);
                pStudio->CriticalRegions &= ~2;
            } else {
                sprintf(szMsg,
                        "Attempting to activate screen (Group ID: %d, Screen ID: %d) which is waiting to be "
                        "unloaded.\n",
                        uGroup, uScreen);
                RuntimeErrorFnc(0, "UIStudio.c", 2942, szMsg);
                return;
            }
        }
    }
}

// Runs _ParseVisibility on p in pScreen, unless _DetermineVisibility finds p's switch already set as bOn
// asks (or not at all).
void UISUpdateVisibility(UISInfoT* pStudio, UISScreenT* pScreen, s32 nKind, void* p, s32 bOn) {
    s32 nFound;
    s32 nOn;

    if (pScreen == NULL || pScreen->pScrData == NULL || p == NULL) {
        return;
    }
    // EA passes the address of p itself as the info to look for; _DetermineVisibility writes the owner it
    // finds into it.
    nFound = _DetermineVisibility(pScreen, (UISControlInfoT*)&p, 8, pScreen->pScrData->Controls);
    nOn = bOn != 0;
    if (nFound != -1 && nFound != nOn) {
        _ParseVisibility(pStudio, pScreen, nOn, nKind, p, 1);
    }
}

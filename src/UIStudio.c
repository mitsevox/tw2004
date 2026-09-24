// UIStudio.c (EA's name, from its asserts): the core of EA's UI Studio library, the menu screens'
// runtime. It loads, activates and unloads screens and runs the events sent to them. Its own
// data: the "UIStudio.c" assert string and a warning in .rodata (0x801861A0-0x8018620C), three
// jump tables in .data and constants in .sdata2 (0x802851A8-0x802851B7).

#include "frontend/uistudio.h"

// Runs fn_8016ABBC on p in pScreen, unless fn_8016AD54 finds p's switch already set as bOn
// asks (or not at all).
void fn_80168644(UIStudio* pStudio, UISScreen* pScreen, s32 nKind, void* p, s32 bOn) {
    s32 nFound;
    s32 nOn;

    if (pScreen == NULL || pScreen->pData == NULL || p == NULL) {
        return;
    }
    // EA passes the address of p itself as the info to look for; fn_8016AD54 writes the owner it
    // finds into it.
    nFound = fn_8016AD54(pScreen, (UISNodeInfo*)&p, 8, pScreen->pData->pNodes);
    nOn = bOn != 0;
    if (nFound != -1 && nFound != nOn) {
        fn_8016ABBC(pStudio, pScreen, nOn, nKind, p, 1);
    }
}

// Activates (bOn) a screen. With no p60 record open the current screen first gets event -5;
// then, unless every screen is being unloaded, the screen becomes current, its first node is
// switched on, the game hears of it (pfnScreen24) and it gets event -4. A screen waiting to be
// unloaded is refused with a warning.
void fn_801686F8(UIStudio* pStudio, u8 bOn, u16 uGroup, u16 uScreen) {
    char szMsg[512];
    u32 nIndex;
    UISScreen* pScreen;

    if (pStudio->n5C == 0) {
        pStudio->uFlags |= 2;
        UIStudio_Send(pStudio, &pStudio->stack64, 0, -5, 0, NULL, 0);
        pStudio->uFlags &= ~2;
    }
    if (bOn && !(u8)pStudio->bUnloadingAll) {
        nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
        if (nIndex < pStudio->nScreens) {
            pScreen = &pStudio->pScreens[nIndex];
            if (pScreen->bUnloading == 0) {
                pScreen->pData->pNodes[0].pInfo->u4 = 1;
                pScreen->pData->pNodes[0].pInfo->p0 = (void*)1;  // port: a marker, not an owner
                pStudio->nCurScreen = nIndex;
                if (pStudio->pfnScreen24 != NULL) {
                    pStudio->pfnScreen24(pScreen->uGroup, pScreen->uScreen);
                }
                pStudio->uFlags |= 2;
                UIStudio_Send(pStudio, &pStudio->stack64, 0, -4, 0, NULL, 0);
                pStudio->uFlags &= ~2;
            } else {
                sprintf(szMsg,
                        "Attempting to activate screen (Group ID: %d, Screen ID: %d) which is waiting to be "
                        "unloaded.\n",
                        uGroup, uScreen);
                lbl_80282A28(0, "UIStudio.c", 2942, szMsg);
            }
        }
    }
}

// Switches (bOn) the node with info pInfo in a screen on or off and runs its scripts for event
// -6 (on) or -7 (off), and those of the node that links to it by index. Switching one on first
// switches off the one fn_8016B6BC finds set. The scripts get nId and pInfo's place in the
// list p (a count, a word, then file offsets; -1 when not there).
void fn_80168918(UIStudio* pStudio, u8 bOn, s16 nId, UISNodeInfo* pInfo, s32* p, u16 uScreen, u16 uGroup) {
    s32 aArgs[2];
    UISScreen* pScreen;
    UISScreenFile* pFile;
    UISNode* pNode;
    UISNode* pLinkNode;
    UISNode* pCheck;
    UISNodeInfo* pOther;
    u8* pScript;
    u16 nIndex;
    u32 nNode;
    u32 n;
    s32 i;
    s32 nSlot;
    s32 nEvent;

    nSlot = -1;
    nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
    pNode = NULL;
    if (nIndex >= pStudio->nScreens) {
        return;
    }
    pScreen = &pStudio->pScreens[nIndex];
    nEvent = (bOn == 1) - 7;
    pFile = pScreen->pData;
    nNode = pFile->nNodes;
    while (nNode-- != 0) {
        pCheck = &pFile->pNodes[nNode];
        if (pCheck->pInfo == pInfo) {
            pNode = pCheck;
            break;
        }
    }
    pLinkNode = NULL;
    n = pFile->nNodes;
    while (n-- != 0) {
        pCheck = &pFile->pNodes[n];
        for (i = 0; i < (s32)pCheck->nHandlers; i++) {
            if (!(pCheck->pHandlers[i].uFlags & 0xC000)
                && (pCheck->pHandlers[i].uFlags & 0x2FFF) == (s16)nNode) {
                pLinkNode = pCheck;
                break;
            }
        }
    }
    if (bOn == 1) {
        pOther = fn_8016B6BC(pScreen, pInfo);
        if (pOther != NULL) {
            fn_80168918(pStudio, 0, 0, pOther, NULL, uScreen, uGroup);
        }
    }
    // EA bug: pNode is NULL when no node of the screen has pInfo; nothing checks it.
    pScript = fn_8016C674(pNode, nEvent);
    pNode->pInfo->u4 = bOn;
    if (p != NULL) {
        nSlot = p[0];
        while (nSlot-- != 0) {
            if (pInfo == (UISNodeInfo*)((u8*)pScreen->pData + p[nSlot + 2])) {
                break;
            }
        }
    }
    aArgs[0] = nId;
    aArgs[1] = nSlot;
    if (pScript != NULL) {
        fn_8016C270(pStudio, pScreen, pNode->pInfo, &pStudio->stack64, pScript, 2, aArgs, 0, NULL, 0, 0,
                    NULL);
    }
    if (pLinkNode != NULL) {
        pScript = fn_8016C614(pLinkNode, nNode, nEvent);
        if (pScript != NULL) {
            fn_8016C270(pStudio, pScreen, pLinkNode->pInfo, &pStudio->stack64, pScript, 2, aArgs, 0, NULL, 0,
                        0, NULL);
        }
    }
}

// Runs the queued events, then sends event uEvent to every screen.
void fn_80168B80(UIStudio* pStudio, u32 uEvent) {
    s32 i;
    s32 n;
    UISScreen* pScreen;
    u8 bOut;

    fn_80165528(pStudio, 0);
    n = pStudio->nScreens;
    for (i = 0; i < n; i++) {
        pScreen = &pStudio->pScreens[i];
        bOut = 0;
        pStudio->uFlags |= 2;
        fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, uEvent, -10, 0, NULL, &bOut);
        pStudio->uFlags &= ~2;
    }
}

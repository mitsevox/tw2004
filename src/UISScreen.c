// UISScreen.c (our name): the screen side of EA's UI Studio library (frontend/uistudio.h). It walks
// a loaded screen's nodes to draw them with a scale and offset, finds and runs the handlers
// nodes have for an event, formats text for them (a printf of its own) and finds the variable a
// rate function drives. Called by UIStudio.c and by the game's menus. The original was built with
// automatic inlining (-inline auto in configure.py): fn_8016A830 and fn_8016B188 have their own
// recursion inlined three deep.

#include "frontend/uistudio.h"

void fn_8016B188(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent,
                 s32 nArgs, const s32* pArgs);
char* fn_8016BEDC(char* pOut, char* pEnd, s32 nWidth, s32 nPrec, f32 f);

// Sends event uEvent to node nNode and, first, to the nodes it links to. A node takes events only
// while its info has u4 and u60 set, except the studio's own events (n5 -2 to -5 and -8 to -11).
// A linked node that answers with 1 gets the handler this node has for it. Returns 2 as soon as a
// handler returns 2.
s32 fn_8016A2D4(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent, u32 n5,
                u8 nArgs, const s32* pArgs, u8* pbOut) {
    s32 nResult;
    UISNode* pNode;
    u32 i;
    u8* pScript;
    u8 bOut;

    nResult = 0;
    if (pScreen->pData == NULL || nNode >= pScreen->pData->nNodes) return 0;
    pNode = &pScreen->pData->pNodes[nNode];
    if ((pNode->pInfo->u4 != 0 && pNode->pInfo->u60 != 0) || (n5 >= (u32)-10 && n5 <= (u32)-8) ||
        (n5 >= (u32)-5 && n5 <= (u32)-2) || n5 == (u32)-11) {
        for (i = 0; i < pNode->nHandlers; i++) {
            UISHandler* pHandler = &pNode->pHandlers[i];
            if (pHandler->uEvent == 0xFFFF) {
                bOut = 0;
                nResult = fn_8016A2D4(pStudio, pScreen, pStack, pHandler->u4.nNode, uEvent, n5, nArgs, pArgs,
                                      &bOut);
                if (bOut == 1) {
                    u8* pLinked = fn_8016C614(pNode, (u16)pHandler->u4.nNode, n5);
                    if (pLinked != NULL) {
                        s32 nRet = fn_8016C270(pStudio, pScreen, pNode->pInfo, pStack, pLinked, nArgs, pArgs,
                                               0, NULL, 1, uEvent, NULL);
                        if (nRet == 2) return nRet;
                    }
                }
            }
        }
        fn_80165528(pStudio, 1);
        pScript = fn_8016C674(pNode, n5);
        // Events -6 and -7 go only to the node their third word names.
        // port: the event word holds a pointer
        if ((n5 == (u32)-6 || n5 == (u32)-7) && pNode->pInfo != (UISNodeInfo*)pArgs[2]) {
            pScript = NULL;
        }
        if (pScript != NULL) {
            nResult = fn_8016C270(pStudio, pScreen, pNode->pInfo, pStack, pScript, nArgs, pArgs, 0, NULL, 1,
                                  uEvent, NULL);
        }
        if (nResult == 2) return nResult;
    }
    *pbOut = pNode->pInfo->u4;
    return nResult;
}

// Draws node nNode and the nodes it links to: runs the screen file's start entries, then, if the
// node is shown, draws its children with its own values folded into the studio's.
void fn_8016A510(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg) {
    UISNode* pNode;
    u32 i;
    u32 j;
    UISVec4 mul;
    UISVec4 add;
    UISNodeInfo* pInfo;

    if (pScreen->pData != NULL) {
        pNode = &pScreen->pData->pNodes[nNode];
        for (i = 0; i < pScreen->pData->nStart; i++) {
            UISEntry* pEntry = &pScreen->pData->pStart[i];
            if (pEntry->uHandler < pStudio->nHandlers) {
                UISHandlerFn pfnHandler = pStudio->ppfnHandlers[pEntry->uHandler];
                if (pfnHandler != NULL) {
                    pfnHandler(pEntry->u4.pnOffset != NULL ? (u8*)pScreen->pData + *pEntry->u4.pnOffset
                                                           : NULL,
                               nMsg, 0, NULL, 0);
                }
            }
        }
        if (pNode->pInfo->p0 != NULL) {
            mul = *fn_8016C198();
            add = *fn_8016C18C();
            pInfo = pNode->pInfo;
            fn_8016C174(mul.a[0] * pInfo->afMul[0], mul.a[1] * pInfo->afMul[1], mul.a[2] * pInfo->afMul[2],
                        mul.a[3] * pInfo->afMul[3]);
            fn_8016C15C(add.a[0] + pInfo->afAdd[0], add.a[1] + pInfo->afAdd[1], add.a[2] + pInfo->afAdd[2],
                        add.a[3] + pInfo->afAdd[3]);
            pStudio->pfnTransform(1, pNode->pInfo->af8);
            for (i = 0; i < pNode->nGroups; i++) {
                UISGroup* pGroup = pNode->ppGroups[i];
                if (pGroup->pInfo->p0 != NULL) {
                    for (j = 0; j < pGroup->nEntries; j++) {
                        UISEntry* pEntry = &pGroup->pEntries[j];
                        if (pEntry->uHandler == 0xFFFF) {
                            fn_8016A510(pStudio, pScreen, pEntry->u4.nNode, nMsg);
                        } else if (pEntry->n2 != 0 && pEntry->uHandler < pStudio->nHandlers) {
                            UISHandlerFn pfnHandler = pStudio->ppfnHandlers[pEntry->uHandler];
                            if (pfnHandler != NULL) {
                                pfnHandler((u8*)pScreen->pData + *pEntry->u4.pnOffset, nMsg, 0, NULL, 0);
                            }
                        }
                    }
                }
            }
            fn_8016C174(mul.a[0], mul.a[1], mul.a[2], mul.a[3]);
            fn_8016C15C(add.a[0], add.a[1], add.a[2], add.a[3]);
            pStudio->pfnTransform(2, pNode->pInfo->af8);
        }
    }
}

// Hands node nNode and every node it links to to the transform callback with operation nOp.
// Operations 0 and 3 also reach groups whose info has no owner.
void fn_8016A830(UIStudio* pStudio, s32 nOp, UISScreen* pScreen, u32 nNode) {
    UISNode* pNode;
    u32 i;
    u32 j;

    if (pScreen->pData != NULL) {
        pNode = &pScreen->pData->pNodes[nNode];
        pStudio->pfnTransform(nOp, pNode->pInfo->af8);
        for (i = 0; i < pNode->nGroups; i++) {
            UISGroup* pGroup = pNode->ppGroups[i];
            if (pGroup->pInfo->p0 != NULL || nOp == 0 || nOp == 3) {
                for (j = 0; j < pGroup->nEntries; j++) {
                    UISEntry* pEntry = &pGroup->pEntries[j];
                    if (pEntry->uHandler == 0xFFFF) {
                        fn_8016A830(pStudio, nOp, pScreen, pEntry->u4.nNode);
                    }
                }
            }
        }
    }
}

// Runs every handler under a node (nKind 8) or a group (nKind 7) with message -4 and a pointer to
// n. Without bAll, nodes and groups whose info has no owner are skipped.
void fn_8016ABBC(UIStudio* pStudio, UISScreen* pScreen, s32 n, s32 nKind, void* p, u8 bAll) {
    u32 i;

    if (pStudio == NULL || pScreen == NULL) return;
    if (p == NULL) return;
    switch (nKind) {
    case 8: {
        UISNode* pNode = p;
        if (pNode->pInfo != NULL && (bAll || pNode->pInfo->p0 != NULL)) {
            u32 nGroups = pNode->nGroups;
            for (i = 0; i < nGroups; i++) {
                fn_8016ABBC(pStudio, pScreen, n, 7, pNode->ppGroups[i], 0);
            }
        }
        break;
    }
    case 7: {
        UISGroup* pGroup = p;
        if (pGroup->pInfo != NULL && (bAll || pGroup->pInfo->p0 != NULL)) {
            u32 nEntries = pGroup->nEntries;
            for (i = 0; i < nEntries; i++) {
                UISEntry* pEntry = &pGroup->pEntries[i];
                if (pEntry->uHandler == 0xFFFF) {
                    fn_8016ABBC(pStudio, pScreen, n, 8, &pScreen->pData->pNodes[pEntry->u4.nNode], 0);
                } else if (pEntry->uHandler < pStudio->nHandlers) {
                    UISHandlerFn pfnHandler = pStudio->ppfnHandlers[pEntry->uHandler];
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pData + *pEntry->u4.pnOffset, -4, 1, &n, 0);
                    }
                }
            }
        }
        break;
    }
    }
}

// Looks under a node (nKind 8) or a group (nKind 7) for the one pInfo belongs to and records it
// as pInfo's owner. Returns -1 when it is not found.
s32 fn_8016AD54(UISScreen* pScreen, UISNodeInfo* pInfo, s32 nKind, void* p) {
    u32 i;
    s32 nFound;

    if (pInfo == NULL || p == NULL || pScreen == NULL) return -1;
    switch (nKind) {
    case 8: {
        UISNode* pNode = p;
        if (pNode->pInfo == pInfo) {
            pInfo->p0 = pNode;
            return pNode->pInfo->p0 != NULL;
        }
        if (pNode->pInfo != NULL && pNode->pInfo->p0 != NULL) {
            u32 nGroups = pNode->nGroups;
            for (i = 0; i < nGroups; i++) {
                nFound = fn_8016AD54(pScreen, pInfo, 7, pNode->ppGroups[i]);
                if (nFound != -1) return nFound;
            }
        }
        break;
    }
    case 7: {
        UISGroup* pGroup = p;
        if (pGroup->pInfo == pInfo) {
            pInfo->p0 = pGroup;
            return pGroup->pInfo->p0 != NULL;
        }
        if (pGroup->pInfo != NULL && pGroup->pInfo->p0 != NULL) {
            u32 nEntries = pGroup->nEntries;
            for (i = 0; i < nEntries; i++) {
                UISEntry* pEntry = &pGroup->pEntries[i];
                if (pEntry->uHandler == 0xFFFF) {
                    nFound = fn_8016AD54(pScreen, pInfo, 8, &pScreen->pData->pNodes[pEntry->u4.nNode]);
                    if (nFound != -1) return nFound;
                }
            }
        }
        break;
    }
    }
    return -1;
}

// Runs the screen file's start entries that have not run yet, then every handler under node
// nNode; a handler run with message -1 is marked as run.
void fn_8016AEEC(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg) {
    UISNode* pNode;
    u32 i;
    u32 j;
    u8 bLast;

    if (pScreen->pData != NULL) {
        pNode = &pScreen->pData->pNodes[nNode];
        bLast = (nMsg == -1);
        for (i = 0; i < pScreen->pData->nStart; i++) {
            UISEntry* pEntry = &pScreen->pData->pStart[i];
            if (pEntry->n2 == 0) {
                if (pEntry->uHandler < pStudio->nHandlers) {
                    UISHandlerFn pfnHandler = pStudio->ppfnHandlers[pEntry->uHandler];
                    if (pfnHandler != NULL) {
                        pfnHandler(pEntry->u4.pnOffset != NULL ? (u8*)pScreen->pData + *pEntry->u4.pnOffset
                                                               : NULL,
                                   nMsg, 0, NULL, 0);
                    }
                }
                pEntry->n2 = 1;
            }
        }
        for (i = 0; i < pNode->nGroups; i++) {
            UISGroup* pGroup = pNode->ppGroups[i];
            for (j = 0; j < pGroup->nEntries; j++) {
                UISEntry* pEntry = &pGroup->pEntries[j];
                if (pEntry->uHandler == 0xFFFF) {
                    fn_8016AEEC(pStudio, pScreen, pEntry->u4.nNode, nMsg);
                } else if (pEntry->uHandler < pStudio->nHandlers) {
                    UISHandlerFn pfnHandler = pStudio->ppfnHandlers[pEntry->uHandler];
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pData + *pEntry->u4.pnOffset, nMsg, 0, NULL, 0);
                        pEntry->n2 = bLast;
                    }
                }
            }
        }
    }
}

// Send an event to every screen. While the studio is already sending one (flag 2 or 4), it is
// queued on the event stack instead.
void fn_8016B09C(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs) {
    UISEventData data;
    if ((pStudio->uFlags & 2) || (pStudio->uFlags & 4)) {
        data.au[0] = uEvent;
        fn_80165B90(-1, -1, pStudio, 9, &data, nArgs, pArgs);
    } else {
        fn_8016B0F8(pStudio, uEvent, nArgs, pArgs);
    }
}

void fn_8016B0F8(UIStudio* pStudio, u32 uEvent, s32 nArgs, const s32* pArgs) {
    u32 i;
    u32 nScreens = pStudio->nScreens;
    for (i = 0; i < nScreens; i++) {
        UISScreen* pScreen = &pStudio->pScreens[i];
        pStudio->uFlags |= 2;
        fn_8016B188(pStudio, pScreen, &pStudio->stack64, 0, uEvent, nArgs, pArgs);
        pStudio->uFlags &= ~2;
    }
}

// Runs the 0x4000 handlers for an event of node nNode and of every node it links to, the linked
// nodes first.
void fn_8016B188(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent,
                 s32 nArgs, const s32* pArgs) {
    UISNode* pNode;
    u32 i;
    u8* pScript;

    pNode = &pScreen->pData->pNodes[nNode];
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if (pHandler->uEvent == 0xFFFF) {
            fn_8016B188(pStudio, pScreen, pStack, pHandler->u4.nNode, uEvent, nArgs, pArgs);
        }
    }
    pScript = fn_8016C5C4(pNode, uEvent);
    if (pScript != NULL) {
        fn_8016C270(pStudio, pScreen, pNode->pInfo, pStack, pScript, nArgs, pArgs, 0, NULL, 0, 0, NULL);
    }
}

// The index of a loaded screen, or the number of screens when it is not loaded.
// fake match: fn_8016C6C4's body, kept here so fn_8016B4D4 can inline it. The original has
// fn_8016C6C4 pasted into fn_8016B4D4 although it sits at the end of the file; -inline auto only
// pastes functions defined earlier, and -inline deferred pastes too much (see configure.py).
static inline u16 UIS_FindScreen(UIStudio* pStudio, u16 uGroup, u16 uScreen) {
    u16 i;
    for (i = 0; i < pStudio->nScreens; i++) {
        if (pStudio->pScreens[i].uGroup == uGroup && pStudio->pScreens[i].uScreen == uScreen) break;
    }
    return i;
}

// Moves a loaded screen nMove places up or down the screen table, one swap at a time, keeping
// the current screen, the rate functions and the p60 records on the screens they named.
void fn_8016B4D4(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 nMove) {
    s32 nScreens;
    s32 nIndex;
    s32 nCount;
    s32 nStep;
    s32 nFrom;
    s32 nTo;
    u32 i;
    UISScreen tmp;

    nScreens = pStudio->nScreens;
    nIndex = UIS_FindScreen(pStudio, uGroup, uScreen);
    if (nIndex < nScreens) {
        if (nMove >= 0) {
            nCount = nMove;
            nStep = 1;
        } else {
            nCount = -nMove;
            nStep = -1;
        }
        nTo = nIndex;
        while (nCount-- != 0) {
            nFrom = nTo;
            nTo += nStep;
            if (nTo >= nScreens || nTo < 0) return;
            if (pStudio->nCurScreen == nTo) {
                pStudio->nCurScreen = nFrom;
            } else if (pStudio->nCurScreen == nFrom) {
                pStudio->nCurScreen = nTo;
            }
            for (i = 0; i < pStudio->nRateFns; i++) {
                if (pStudio->pRateFns[i].pScreen == &pStudio->pScreens[nTo]) {
                    pStudio->pRateFns[i].pScreen = &pStudio->pScreens[nFrom];
                } else if (pStudio->pRateFns[i].pScreen == &pStudio->pScreens[nFrom]) {
                    pStudio->pRateFns[i].pScreen = &pStudio->pScreens[nTo];
                }
            }
            for (i = 0; i < pStudio->n5C; i++) {
                if (pStudio->p60[i].pScreen == &pStudio->pScreens[nTo]) {
                    pStudio->p60[i].pScreen = &pStudio->pScreens[nFrom];
                } else if (pStudio->p60[i].pScreen == &pStudio->pScreens[nFrom]) {
                    pStudio->p60[i].pScreen = &pStudio->pScreens[nTo];
                }
            }
            memcpy(&tmp, &pStudio->pScreens[nTo], sizeof(UISScreen));
            memcpy(&pStudio->pScreens[nTo], &pStudio->pScreens[nFrom], sizeof(UISScreen));
            memcpy(&pStudio->pScreens[nFrom], &tmp, sizeof(UISScreen));
        }
    }
}

// Whether one of pNode's groups links to the node pInfo belongs to.
static inline s32 UIS_NodeLinks(UISScreenFile* pData, UISNode* pNode, UISNodeInfo* pInfo) {
    u32 i;
    u32 j;

    for (i = 0; i < pNode->nGroups; i++) {
        UISGroup* pGroup = pNode->ppGroups[i];
        for (j = 0; j < pGroup->nEntries; j++) {
            UISEntry* pEntry = &pGroup->pEntries[j];
            if (pEntry->uHandler == 0xFFFF && pData->pNodes[pEntry->u4.nNode].pInfo == pInfo) return 1;
        }
    }
    return 0;
}

// Finds the node that links to pInfo's node and returns the info of the first node it links to
// whose u4 is set.
UISNodeInfo* fn_8016B6BC(UISScreen* pScreen, UISNodeInfo* pInfo) {
    UISScreenFile* pData;
    u32 i;
    u32 j;
    u32 k;

    pData = pScreen->pData;
    for (i = 0; i < pData->nNodes; i++) {
        UISNode* pNode = &pData->pNodes[i];
        if (UIS_NodeLinks(pData, pNode, pInfo)) {
            for (j = 0; j < pNode->nGroups; j++) {
                UISGroup* pGroup = pNode->ppGroups[j];
                for (k = 0; k < pGroup->nEntries; k++) {
                    UISEntry* pEntry = &pGroup->pEntries[k];
                    if (pEntry->uHandler == 0xFFFF) {
                        UISNodeInfo* pLinked = pData->pNodes[pEntry->u4.nNode].pInfo;
                        if (pLinked->u4 != 0) return pLinked;
                    }
                }
            }
            return NULL;
        }
    }
    return NULL;
}

// Formats pFormat's text with pArgs into pOut's buffer.
void fn_8016B808(u32 u0, UISText* pOut, UISText* pFormat, s32 nArgs, const UISWord* pArgs) {
    if (pFormat != NULL && pOut != NULL) {
        fn_8016B844(pOut->szText, pOut->nSize, pFormat->szText, nArgs, pArgs);
    }
}

// Copies sz to pOut, padded with spaces to nWidth characters (on the left, or on the right when
// bLeft), stopping at pEnd. Returns the end of the copy.
static inline char* UIS_PutString(char* pOut, char* pEnd, const char* sz, s32 nWidth, u8 bLeft) {
    s32 nAbs;
    s32 nCount;
    s32 nPad;
    s32 i;

    nAbs = nWidth;
    nCount = 0;
    if (nWidth < 0) {
        nAbs = -nWidth;
    }
    nPad = nAbs - strlen(sz);
    if (!bLeft && nPad > 0) {
        for (i = 0; i < nPad; i++) {
            *pOut++ = ' ';
        }
        nCount = nPad;
    }
    while (*sz != 0) {
        *pOut++ = *sz++;
        nCount++;
        if (pOut == pEnd) break;
    }
    if (bLeft == 1 && nCount < nAbs) {
        for (i = nAbs - nCount; i > 0; i--) {
            *pOut++ = ' ';
        }
    }
    return pOut;
}

// Formats szFormat with pArgs into pOut (nSize bytes). Returns the length written, or -1.
// EA bug: '-' is never cleared, so every conversion after one with '-' is left-justified too.
s32 fn_8016B844(char* pOut, s32 nSize, const char* szFormat, s32 nArgs, const UISWord* pArgs) {
    char* pStart;
    char* pEnd;
    u8 bLeft;
    s32 nArg;
    char c;
    u8 bUnsigned;
    s32 nWidth;
    s32 nPrec;
    char cPad;
    u8 bUpper;
    s32 nDigits;
    s32 nPad;
    u32 u;
    u8 bNeg;
    char* p;
    char aDec[20];
    char aHex[12];

    pStart = pOut;
    pEnd = pOut + nSize;
    bLeft = 0;
    if (pOut == NULL || nSize == 0 || szFormat == NULL) return -1;
    nArg = 0;
    while ((c = *szFormat++) != 0 && pOut < pEnd) {
        if (c == '%') {
            bUnsigned = 0;
            nWidth = 0;
            nPrec = -1;
            c = *szFormat++;
            cPad = ' ';
            if (c == '-') {
                bLeft = 1;
                c = *szFormat++;
            }
            if (c == '0') {
                cPad = c;
                c = *szFormat++;
            }
            while (c >= '0' && c <= '9') {
                nWidth = nWidth * 10 + c - '0';
                c = *szFormat++;
            }
            if (c == '.') {
                c = *szFormat++;
            }
            if (c >= '0' && c <= '9') {
                nPrec = c - '0';
                c = *szFormat++;
            }
            while (c >= '0' && c <= '9') {
                nPrec = nPrec * 10 + c - '0';
                c = *szFormat++;
            }
            bUpper = 0;
            if (c >= 'A' && c <= 'Z') {
                bUpper = 1;
            }
            switch (c) {
            case 'c':
                *pOut++ = pArgs[nArg++].n;
                continue;
            case 's': {
                UISText* pText = pArgs[nArg++].pText;
                if (pText->szText != NULL) {
                    pOut = UIS_PutString(pOut, pEnd, pText->szText, nWidth, bLeft);
                } else {
                    pOut = UIS_PutString(pOut, pEnd, "(null)", nWidth, bLeft);
                }
                continue;
            }
            case 'u':
                bUnsigned = 1;
            case 'd':
            case 'i':
                nDigits = 0;
                u = pArgs[nArg++].u;
                bNeg = 0;
                if (!bUnsigned && (s32)u < 0) {
                    bNeg = 1;
                }
                if (bNeg) {
                    u = -u;
                }
                do {
                    aDec[nDigits++] = u % 10 + '0';
                    u /= 10;
                } while (u != 0);
                if (nWidth != 0) {
                    for (nPad = nWidth - bNeg - nDigits; nPad > 0; nPad--) {
                        aDec[nDigits++] = cPad;
                    }
                }
                if (bNeg) {
                    aDec[nDigits++] = '-';
                }
                while (nDigits-- > 0 && pOut < pEnd) {
                    *pOut++ = aDec[nDigits];
                }
                continue;
            case 'f':
                pOut = fn_8016BEDC(pOut, pEnd, nWidth, nPrec, pArgs[nArg++].f);
                continue;
            case 'X':
            case 'p':
            case 'x':
                u = pArgs[nArg++].u;
                nDigits = 0;
                do {
                    c = (u & 0xF) + '0';
                    if (c > '9') {
                        c += (bUpper ? 'A' : 'a') - '9' - 1;
                    }
                    u >>= 4;
                    aHex[nDigits++] = c;
                } while (u != 0);
                if (nWidth != 0) {
                    for (nPad = nWidth - nDigits; nPad > 0; nPad--) {
                        aHex[nDigits++] = cPad;
                    }
                }
                while (nDigits-- > 0 && pOut < pEnd) {
                    *pOut++ = aHex[nDigits];
                }
                continue;
            }
        }
        if (c != 0) {
            *pOut++ = c;
        }
    }
    *pOut++ = '\0';
    return pOut - pStart - 1;
}

// Writes f with nPrec decimals (6 when negative), padded with spaces to nWidth characters, into
// pOut up to pEnd. Returns the end of what it wrote.
char* fn_8016BEDC(char* pOut, char* pEnd, s32 nWidth, s32 nPrec, f32 f) {
    char aDigits[64];
    s32 nDigits;
    u8 bNeg;
    f32 fRound;
    s32 i;
    f32 fFrac;
    f32 fNext;
    s32 nPad;

    nDigits = 0;
    if (nPrec < 0) {
        nPrec = 6;
    }
    bNeg = f < 0.0f;
    if (bNeg) {
        f = -f;
    }
    fRound = 0.5f;
    for (i = 0; i < nPrec; i++) {
        fRound *= 0.1f;
    }
    f += fRound;
    fFrac = f - (s32)f;
    do {
        fNext = f / 10.0f;
        aDigits[nDigits++] = (s32)(f - (s32)fNext * 10) + '0';
        f = fNext;
    } while (f >= 1.0f);
    if (bNeg) {
        aDigits[nDigits++] = '-';
    }
    if (nWidth != 0) {
        for (nPad = nWidth - (nPrec + (nDigits + (nPrec != 0))); nPad > 0; nPad--) {
            aDigits[nDigits++] = ' ';
        }
    }
    while (nDigits-- > 0 && pOut < pEnd) {
        *pOut++ = aDigits[nDigits];
    }
    if (nPrec != 0 && pOut < pEnd) {
        *pOut++ = '.';
        do {
            fFrac *= 10.0f;
            *pOut++ = (s32)fFrac + '0';
            fFrac -= (s32)fFrac;
        } while (--nPrec != 0 && pOut < pEnd);
    }
    return pOut;
}

// The values every node is drawn with: fn_8016A510 adds a node's afAdd to the first and
// multiplies its afMul into the second for the node's children.
void fn_8016C15C(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    lbl_80280628.a[0] = f1;
    lbl_80280628.a[3] = f4;
    lbl_80280628.a[1] = f2;
    lbl_80280628.a[2] = f3;
}

void fn_8016C174(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    lbl_80280638.a[0] = f1;
    lbl_80280638.a[3] = f4;
    lbl_80280638.a[1] = f2;
    lbl_80280638.a[2] = f3;
}

UISVec4* fn_8016C18C(void) {
    return &lbl_80280628;
}

UISVec4* fn_8016C198(void) {
    return &lbl_80280638;
}

f32* fn_8016C1A4(s32 n20, UISNodeInfo* pInfo) {
    switch (n20) {
    case 16:
        return &pInfo->af8[6];
    case 17:
        return &pInfo->af8[7];
    case 18:
        return &pInfo->af8[8];
    case 19:
        return &pInfo->af8[0];
    case 20:
        return &pInfo->af8[1];
    case 21:
        return &pInfo->af8[2];
    case 22:
        return &pInfo->af8[9];
    case 23:
        return &pInfo->af8[10];
    case 24:
        return &pInfo->af8[11];
    case 25:
        return &pInfo->af8[3];
    case 26:
        return &pInfo->af8[4];
    case 27:
        return &pInfo->af8[5];
    case 32:
        return &pInfo->afAdd[3];
    case 33:
        return &pInfo->afAdd[0];
    case 34:
        return &pInfo->afAdd[1];
    case 35:
        return &pInfo->afAdd[2];
    case 36:
        return &pInfo->afMul[3];
    case 37:
        return &pInfo->afMul[0];
    case 38:
        return &pInfo->afMul[1];
    case 39:
        return &pInfo->afMul[2];
    default:
        return &pInfo->afMul[3];
    }
}

// Pushes a call frame on pStack (the saved word, the extra word, both argument lists, the node's
// info and a 0) and runs pScript on it. The frame stays on the stack only when the script
// returns 3 (it paused).
s8 fn_8016C270(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pInfo, UISWordStack* pStack, u8* pScript,
               u32 nArgs, const s32* pArgs, u32 nArgs2, const s32* pArgs2, u8 bExtra, s32 nExtra,
               s32* pnSaved) {
    s32* pFrame;
    u32 i;
    s8 nResult;

    pFrame = pStack->pC;
    if (pnSaved == NULL) {
        *pFrame = 0;
    } else {
        *pFrame = *pnSaved;
    }
    pStack->pC++;
    if (bExtra) {
        *pStack->pC = nExtra;
        pStack->pC++;
    }
    for (i = 0; i < nArgs; i++) {
        *pStack->pC = pArgs[i];
        pStack->pC++;
    }
    for (i = 0; i < nArgs2; i++) {
        *pStack->pC = pArgs2[i];
        pStack->pC++;
    }
    // port: the frame keeps the info pointer in a word
    *pStack->pC = (s32)pInfo;
    pStack->pC++;
    *pStack->pC = 0;
    pStack->pC++;
    pStack->p10 = pScript;
    nResult = fn_80166098(pStudio, pFrame, pStack, pScreen, pInfo);
    if (pnSaved != NULL) {
        *pnSaved = *pFrame;
    }
    if (nResult != 3) {
        pStack->pC = pFrame;
    }
    return nResult;
}

// The node's handler of the kind marked 0x4000 for an event.
u8* fn_8016C5C4(UISNode* pNode, u16 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if ((pHandler->uFlags & 0x4000) && pHandler->uEvent == uEvent) {
            return pHandler->u4.pScript;
        }
    }
    return NULL;
}

// A node's plain handler (neither kind bit) with the given ID for an event.
u8* fn_8016C614(UISNode* pNode, u16 uId, u16 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if (!(pHandler->uFlags & 0xC000) && pHandler->uEvent == uEvent &&
            (pHandler->uFlags & 0x2FFF) == uId) {
            return pHandler->u4.pScript;
        }
    }
    return NULL;
}

// A node's handler of the kind marked 0x8000 for an event.
u8* fn_8016C674(UISNode* pNode, u16 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if ((pHandler->uFlags & 0x8000) && pHandler->uEvent == uEvent) {
            return pHandler->u4.pScript;
        }
    }
    return NULL;
}

// The index of a loaded screen, or the number of screens when it is not loaded.
u16 fn_8016C6C4(UIStudio* pStudio, u16 uGroup, u16 uScreen) {
    return UIS_FindScreen(pStudio, uGroup, uScreen);
}

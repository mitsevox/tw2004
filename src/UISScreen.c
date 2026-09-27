// UISScreen.c (our name): the screen side of EA's UI Studio library (frontend/uistudio.h). It walks
// a loaded screen's nodes to draw them with a scale and offset, finds and runs the handlers
// nodes have for an event, formats text for them (a printf of its own) and finds the variable a
// rate function drives. Called by UIStudio.c and by the game's menus. The original was built with
// automatic inlining (-inline auto in configure.py): fn_8016A830 and fn_8016B188 have their own
// recursion inlined three deep.
// section order: built with -inline auto,deferred, which emits the functions last-first, so they
// are written here from the highest address down.

#include "frontend/uistudio.h"

void fn_8016B188(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent,
                 s32 nArgs, s32* pArgs);
char* fn_8016BEDC(char* pOut, char* pEnd, s32 nWidth, s32 nPrec, f32 f);

// Whether one of pNode's groups links to the node pInfo belongs to.
static inline u8 UIS_NodeLinks(UISScreenFile* pData, UISNode* pNode, UISNodeInfo* pInfo) {
    UISGroup* pGroup;
    UISEntry* pEntry;
    u32 i;
    u32 j;

    for (i = 0; i < pNode->nGroups; i++) {
        pGroup = pNode->ppGroups[i];
        for (j = 0; j < pGroup->nEntries; j++) {
            pEntry = &pGroup->pEntries[j];
            if (pEntry->uHandler == 0xFFFF && pData->pNodes[pEntry->u4.nNode].pInfo == pInfo) return 1;
        }
    }
    return 0;
}

// The info of the first node pNode links to whose u4 is set, or NULL.
static inline UISNodeInfo* UIS_LinkedOn(UISScreenFile* pData, UISNode* pNode) {
    UISGroup* pGroup;
    UISEntry* pEntry;
    UISNodeInfo* pLinked;
    u32 i;
    u32 j;

    for (i = 0; i < pNode->nGroups; i++) {
        pGroup = pNode->ppGroups[i];
        for (j = 0; j < pGroup->nEntries; j++) {
            pEntry = &pGroup->pEntries[j];
            if (pEntry->uHandler == 0xFFFF) {
                pLinked = pData->pNodes[pEntry->u4.nNode].pInfo;
                if (pLinked->u4 != 0) return pLinked;
            }
        }
    }
    return NULL;
}

// Copies sz to pOut, padded with spaces to nWidth characters (on the left, or on the right when
// bLeft), stopping at pEnd. Returns the end of the copy.
// Register note: the walking copies p and s and their declaration order (after nAbs, before
// nCount) give EA's r22-r25 in both inlined copies.
static inline char* UIS_PutString(char* pOut, char* pEnd, const char* sz, s32 nWidth, u8 bLeft) {
    s32 nAbs;
    char* p;
    const char* s;
    s32 nCount;
    s32 nPad;
    s32 nLen;
    s32 i;

    nAbs = nWidth;
    p = pOut;
    s = sz;
    nCount = 0;
    if (nWidth < 0) {
        nAbs = -nWidth;
    }
    nLen = strlen(s);
    if (!bLeft) {
        nPad = nAbs - nLen;
        while (nCount < nPad) {
            nCount++;
            *p++ = ' ';
        }
    }
    while (*s != 0) {
        *p++ = *s++;
        nCount++;
        if (p == pEnd) break;
    }
    if (bLeft == 1) {
        for (i = nCount; i < nAbs; i++) {
            *p++ = ' ';
        }
    }
    return p;
}

// The index of a loaded screen, or the number of screens when it is not loaded. Written out, not
// through UIS_FindScreen: returning an inlined call adds a copy of the index (88.6 -> 90.5%).
// fake match: scheduled once, not twice: EA's entry block has `li i` between the two u16 masks,
// which only the single scheduling pass gives (twice puts the li last). Code is unchanged.
#pragma scheduling once
u16 fn_8016C6C4(UIStudio* pStudio, u16 uGroup, u16 uScreen) {
    u16 i;
    UISScreen* pScreen;

    for (i = 0; i < pStudio->nScreens; i++) {
        pScreen = &pStudio->pScreens[i];
        if (pScreen->uGroup == uGroup && pScreen->uScreen == uScreen) break;
    }
    return i;
}
#pragma scheduling reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// A node's handler of the kind marked 0x8000 for an event.
u8* fn_8016C674(UISNode* pNode, u32 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if ((pHandler->uFlags & 0x8000) && pHandler->uEvent == (u16)uEvent) {
            return pHandler->u4.pScript;
        }
    }
    return NULL;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// A node's plain handler (neither kind bit) with the given ID for an event.
// fake match: scheduled once, not twice: EA's entry block has `li i` between the two masks,
// which only the single scheduling pass gives (twice puts the li last). Code is unchanged.
#pragma scheduling once
u8* fn_8016C614(UISNode* pNode, u16 uId, u32 uEvent) {
    u32 i;
    int nId = uId;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if (!(pHandler->uFlags & 0xC000) && pHandler->uEvent == (u16)uEvent &&
            (pHandler->uFlags & 0x2FFF) == nId) {
            return pHandler->u4.pScript;
        }
    }
    return NULL;
}
#pragma scheduling reset
#pragma auto_inline reset

// The node's handler of the kind marked 0x4000 for an event.
u8* fn_8016C5C4(UISNode* pNode, u32 uEvent) {
    u32 i;
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if ((pHandler->uFlags & 0x4000) && pHandler->uEvent == (u16)uEvent) {
            return pHandler->u4.pScript;
        }
    }
    return NULL;
}

// Pushes a call frame on pStack (the saved word, the extra word, both argument lists, the node's
// info and a 0) and runs pScript on it. The frame stays on the stack only when the script
// returns 3 (it paused).
s32 fn_8016C270(UIStudio* pStudio, UISScreen* pScreen, UISNodeInfo* pInfo, UISWordStack* pStack, u8* pScript,
               s32 nArgs, s32* pArgs, u32 nArgs2, const s32* pArgs2, u8 bExtra, s32 nExtra,
               s32* pnSaved) {
    // fake match: pStackCopy is pStack through a void* copy, declared first; the local's higher
    // variable number makes the allocator colour it before pFrame and pnSaved (EA's r31).
    UISWordStack* pStackCopy = (UISWordStack*)(void*)pStack;
    s32* pFrame;
    u32 i;
    s32 nResult;
    void* pScriptCopy = pScript;

    // fake match: pScriptCopy is pScript, so this OR leaves pScript unchanged. The frontend cannot
    // fold the OR of two variables; it stays as `or r11,r7,r7` (the original's `mr r11,r7`, the
    // same encoding), which the allocator never coalesces, so pScript leaves r7 as in EA.
    // port: the pointer goes through a 32-bit integer
    pScript = (u8*)((u32)pScript | (u32)pScriptCopy);
    pFrame = pStackCopy->pC;
    if (pnSaved == NULL) {
        *pFrame = 0;
    } else {
        *pFrame = *pnSaved;
    }
    pStackCopy->pC++;
    if (bExtra) {
        *pStackCopy->pC = nExtra;
        pStackCopy->pC++;
    }
    for (i = 0; i < nArgs; i++) {
        *pStackCopy->pC = pArgs[i];
        pStackCopy->pC++;
    }
    for (i = 0; i < nArgs2; i++) {
        *pStackCopy->pC = pArgs2[i];
        pStackCopy->pC++;
    }
    // port: the frame keeps the info pointer in a word
    *pStackCopy->pC = (s32)pInfo;
    pStackCopy->pC++;
    *pStackCopy->pC = 0;
    pStackCopy->pC++;
    pStackCopy->p10 = pScript;
    nResult = fn_80166098(pStudio, pFrame, pStackCopy, pScreen, pInfo);
    if (pnSaved != NULL) {
        *pnSaved = *pFrame;
    }
    if (nResult != 3) {
        pStackCopy->pC = pFrame;
    }
    return nResult;
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

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
UISVec4* fn_8016C198(void) {
    return &lbl_80280638;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
UISVec4* fn_8016C18C(void) {
    return &lbl_80280628;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
void fn_8016C174(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    lbl_80280638.a[0] = f1;
    lbl_80280638.a[3] = f4;
    lbl_80280638.a[1] = f2;
    lbl_80280638.a[2] = f3;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// The values every node is drawn with: fn_8016A510 adds a node's afAdd to the first and
// multiplies its afMul into the second for the node's children.
void fn_8016C15C(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    lbl_80280628.a[0] = f1;
    lbl_80280628.a[3] = f4;
    lbl_80280628.a[1] = f2;
    lbl_80280628.a[2] = f3;
}
#pragma auto_inline reset

// Writes f with nPrec decimals (6 when negative), padded with spaces to nWidth characters, into
// pOut up to pEnd. Returns the end of what it wrote.
char* fn_8016BEDC(char* pOut, char* pEnd, s32 nWidth, s32 nPrec, f32 f) {
    char aDigits[64];
    s32 nDigits;
    s32 bNeg;
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
        fRound = 0.1f * fRound;
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
        for (nPad = nWidth - (nPrec + nDigits + (nPrec != 0)); nPad > 0; nPad--) {
            aDigits[nDigits++] = ' ';
        }
    }
    while (--nDigits >= 0 && pOut < pEnd) {
        *pOut++ = aDigits[nDigits];
    }
    if (nPrec != 0 && pOut < pEnd) {
        *pOut++ = '.';
        do {
            fFrac *= 10.0f;
            // (int) and (s32) (a long) are separate conversions to the compiler: EA stores the
            // digit and the whole part as two words.
            *pOut++ = (int)fFrac + '0';
            fFrac -= (s32)fFrac;
        } while (--nPrec != 0 && pOut < pEnd);
    }
    return pOut;
}

// fake match: fn_8016B844's 'x' case as an inline. Inlined code gets the frontend's goto
// cleanup that a large function body skips, which places the output loop's preheader at the
// end of fn_8016B844 as in EA's code. Code is unchanged.
static inline char* fn_8016B844_CaseX(char* pOut, char* pEnd, u32 u, s32 nWidth, char cPad, s32 nUpper) {
    s32 nDigits;
    s32 nPad;
    char c;
    char aHex[12];

    nDigits = 0;
    do {
        c = (u & 0xF) + '0';
        if (c > '9') {
            c += nUpper + 'a' - '9' - 1;
        }
        u >>= 4;
        aHex[nDigits++] = c;
    } while (u != 0);
    if (nWidth != 0) {
        for (nPad = nWidth - nDigits; nPad > 0; nPad--) {
            aHex[nDigits++] = cPad;
        }
    }
    while (--nDigits >= 0 && pOut < pEnd) {
        *pOut++ = aHex[nDigits];
    }
    return pOut;
}

// fake match: fn_8016B844's 'd' case as an inline, for the same preheader placement as
// fn_8016B844_CaseX. `bUnsigned ^ 1` for !bUnsigned (it is 0 or 1) gives EA's xori, and
// `bNeg = n >> 31` inside the && EA's srwi. whose result is the sign kept in r0. The caller
// passes its own u as n, which keeps EA's `mr r11,r7` before the negate. Code is unchanged.
static inline char* fn_8016B844_CaseD(char* pOut, char* pEnd, u32 n, s32 nWidth, char cPad, s32 bUnsigned) {
    s32 nDigits;
    s32 nPad;
    s32 bNeg;
    char aDec[20];
    u32 u;

    nDigits = 0;
    bNeg = 0;
    u = ((bUnsigned ^ 1) && (bNeg = n >> 31)) ? -n : n;
    do {
        aDec[nDigits++] = u + '0' - u / 10 * 10;
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
    while (--nDigits >= 0 && pOut < pEnd) {
        *pOut++ = aDec[nDigits];
    }
    return pOut;
}

// fake match: fn_8016B844's start pointer read through an inline whose parameter is changed (the
// dead p++), so the parameter is a frontend variable numbered before every other inline's; the
// start then ranks lowest of the saved registers (EA's r26). Returns p unchanged.
static inline char* fn_8016B844_Get(char* p) {
    return p++;
}

// Formats szFormat with pArgs into pOut (nSize bytes). Returns the length written, or -1.
// EA bug: '-' is never cleared, so every conversion after one with '-' is left-justified too.
s32 fn_8016B844(char* pOut, s32 nSize, const char* szFormat, s32 nArgs, const UISWord* pArgs) {
    // Register note: this declaration order gives EA's c r7, cPad r8 and nUpper r0.
    char* pEnd;
    s32 nArg;
    s32 nUpper;
    u32 u;
    s32 nWidth;
    s32 nPrec;
    s32 bUpper;
    s32 bUnsigned;
    u8 bLeft;
    char c;
    char cPad;
    char* pStart;

    pStart = fn_8016B844_Get(pOut);
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
            nUpper = bUpper ? 'A' - 'a' : 0;
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
                u = pArgs[nArg++].u;
                pOut = fn_8016B844_CaseD(pOut, pEnd, u, nWidth, cPad, bUnsigned);
                continue;
            case 'f':
                pOut = fn_8016BEDC(pOut, pEnd, nWidth, nPrec, pArgs[nArg++].f);
                continue;
            case 'X':
            case 'p':
            case 'x':
                u = pArgs[nArg++].u;
                pOut = fn_8016B844_CaseX(pOut, pEnd, u, nWidth, cPad, nUpper);
                continue;
            }
        }
        if (c != 0) {
            *pOut++ = c;
        }
    }
    *pOut++ = '\0';
    return pOut - (pStart + 1);
}

// Formats pFormat's text with pArgs into pOut's buffer.
void fn_8016B808(u32 u0, UISText* pOut, UISText* pFormat, s32 nArgs, const UISWord* pArgs) {
    if (pFormat != NULL && pOut != NULL) {
        fn_8016B844(pOut->szText, pOut->nSize, pFormat->szText, nArgs, pArgs);
    }
}

// Finds the node that links to pInfo's node and returns the info of the first node it links to
// whose u4 is set.
UISNodeInfo* fn_8016B6BC(UISScreen* pScreen, UISNodeInfo* pInfo) {
    UISScreenFile* pData;
    u32 i;

    pData = pScreen->pData;
    for (i = 0; i < pData->nNodes; i++) {
        UISNode* pNode = &pData->pNodes[i];
        if (UIS_NodeLinks(pData, pNode, pInfo)) {
            return UIS_LinkedOn(pData, pNode);
        }
    }
    return NULL;
}

// Moves a loaded screen nMove places up or down the screen table, one swap at a time, keeping
// the current screen, the rate functions and the p60 records on the screens they named.
void fn_8016B4D4(UIStudio* pStudio, u16 uGroup, u16 uScreen, s32 nMove) {
    s32 nScreens;
    s32 nLimit;
    s32 nIndex;
    s32 nCount;
    s32 nFrom;
    s32 nTo;
    u32 i;
    u32 j;
    s32 nStep;
    UISScreen tmp;

    nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
    nScreens = pStudio->nScreens;
    if (nIndex < nScreens) {
        if (nMove >= 0) {
            nCount = nMove;
            nStep = 1;
        } else {
            nCount = -nMove;
            nStep = -1;
        }
        nTo = nIndex;
        // fake match: nLimit is always nScreens; its second assignment in the loop keeps EA's copy of
        // the count (`mr r28,r8`) from being propagated away.
        nLimit = nScreens;
        while (nCount-- != 0) {
            nFrom = nTo;
            nTo += nStep;
            if (nTo >= nLimit || nTo < 0) break;
            nLimit = nScreens;
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
            for (j = 0; j < pStudio->n5C; j++) {
                if (pStudio->p60[j].pScreen == &pStudio->pScreens[nTo]) {
                    pStudio->p60[j].pScreen = &pStudio->pScreens[nFrom];
                } else if (pStudio->p60[j].pScreen == &pStudio->pScreens[nFrom]) {
                    pStudio->p60[j].pScreen = &pStudio->pScreens[nTo];
                }
            }
            memcpy(&tmp, &pStudio->pScreens[nTo], sizeof(UISScreen));
            memcpy(&pStudio->pScreens[nTo], &pStudio->pScreens[nFrom], sizeof(UISScreen));
            memcpy(&pStudio->pScreens[nFrom], &tmp, sizeof(UISScreen));
        }
    }
}

// Runs the 0x4000 handlers for an event of node nNode and of every node it links to, the linked
// nodes first.
void fn_8016B188(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent,
                 s32 nArgs, s32* pArgs) {
    UISNode* pNode;
    u32 i;
    u8* pScript;

    // fake match: the byte offset as a 64-bit product (low word = nNode * 20, the same address as
    // pNodes[nNode]); its dead high word (li 20; mulhw) goes first in the pre-allocation schedule,
    // which moves the pStudio copy after the pData load as in the original, and is deleted later.
    // port: the offset is truncated to 32 bits.
    pNode = (UISNode*)((u8*)pScreen->pData->pNodes + (s32)nNode * (s64)sizeof(UISNode));
    for (i = 0; i < pNode->nHandlers; i++) {
        UISHandler* pHandler = &pNode->pHandlers[i];
        if (pHandler->uEvent == 0xFFFF) {
            fn_8016B188(pStudio, pScreen, pStack, pHandler->u4.nNode, uEvent, nArgs, pArgs);
        }
    }
    // The node's script for the event (fn_8016C5C4's search). The original has fn_8016C5C4
    // inlined here and called only at the deepest inlined level; a static inline copy of it matches
    // better (93%) but adds a function the original does not have, so the unit could not link.
    pScript = fn_8016C5C4(pNode, uEvent);
    if (pScript != NULL) {
        fn_8016C270(pStudio, pScreen, pNode->pInfo, pStack, pScript, nArgs, pArgs, 0, NULL, 0, 0, NULL);
    }
}

void fn_8016B0F8(UIStudio* pStudio, u32 uEvent, s32 nArgs, s32* pArgs) {
    u32 i;
    u32 nScreens = pStudio->nScreens;
    for (i = 0; i < nScreens; i++) {
        UISScreen* pScreen = &pStudio->pScreens[i];
        pStudio->uFlags |= 2;
        fn_8016B188(pStudio, pScreen, &pStudio->stack64, 0, uEvent, nArgs, pArgs);
        pStudio->uFlags &= ~2;
    }
}

// Send an event to every screen. While the studio is busy (flag 2: sending an event; 4: running
// its rate functions), it is queued on the event stack instead.
void fn_8016B09C(UIStudio* pStudio, u32 uEvent, s32 nArgs, s32* pArgs) {
    UISEventData data;
    if ((pStudio->uFlags & 2) || (pStudio->uFlags & 4)) {
        data.au[0] = uEvent;
        fn_80165B90(-1, -1, pStudio, 9, &data, nArgs, pArgs);
    } else {
        fn_8016B0F8(pStudio, uEvent, nArgs, pArgs);
    }
}

// fake match: an identity read; it gives EA's register order.
static inline u8 fn_8016AEEC_Read(u8 b) { return b; }

// Runs the screen file's start entries that have not run yet, then every handler under node
// nNode; a handler run with message -1 is marked as run.
void fn_8016AEEC(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg) {
    // fake match: the second loop's pEntry is declared here, ahead of pNode and j, and shadowed by
    // the first loop's own pEntry; this order gives the original's loop registers.
    u8 bLast;
    UISEntry* pEntry;
    UISNode* pNode;
    u32 j;

    if (pScreen->pData != NULL) {
        pNode = &pScreen->pData->pNodes[nNode];
        bLast = fn_8016AEEC_Read(nMsg == -1);
        // fake match: nNode (dead after pNode) is the counter of both entry loops, which gives the
        // original's loop registers.
        for (nNode = 0; nNode < pScreen->pData->nStart; nNode++) {
            UISEntry* pEntry = &pScreen->pData->pStart[nNode];
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
        for (j = 0; j < pNode->nGroups; j++) {
            // fake match: b is bLast, and the stored `b | bLast` is bLast. The frontend cannot fold the
            // OR of two variables, so after the copy is propagated it stays as `or r30,r27,r27` (the
            // original's `mr r30,r27`, the same encoding), which the allocator never coalesces.
            int b = bLast;
            UISGroup* pGroup = pNode->ppGroups[j];
            for (nNode = 0; nNode < pGroup->nEntries; nNode++) {
                pEntry = &pGroup->pEntries[nNode];
                if (pEntry->uHandler == 0xFFFF) {
                    fn_8016AEEC(pStudio, pScreen, pEntry->u4.nNode, nMsg);
                } else if (pEntry->uHandler < pStudio->nHandlers) {
                    UISHandlerFn pfnHandler = pStudio->ppfnHandlers[pEntry->uHandler];
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pData + *pEntry->u4.pnOffset, nMsg, 0, NULL, 0);
                        pEntry->n2 = b | bLast;
                    }
                }
            }
        }
    }
}

// Looks under a node (nKind 8) or a group (nKind 7) for the one pInfo belongs to and records it
// as pInfo's owner. Returns -1 when it is not found.
s32 fn_8016AD54(UISScreen* pScreen, UISNodeInfo* pInfo, s32 nKind, void* p) {
    // fake match: this declaration order (with the copies below) gives EA's registers.
    UISGroup* pGroup;
    u32 i;
    UISNode* pNode;
    // fake match: one count for both loops (nGroups, then nEntries): with it nEntries takes r28
    // and case 7's counter r27 (EA's li r27,0 / lwz r28 / mr r29,r27); with two, they swap.
    u32 nCount;
    UISEntry* pEntry;
    UISNode* pLoop8;
    UISNode* pPrev8;
    UISGroup* pLoop7;
    s32 nFound;

    if (pInfo == NULL || p == NULL || pScreen == NULL) return -1;
    switch (nKind) {
    case 8:
        pNode = p;
        if (pNode->pInfo == pInfo) {
            pInfo->p0 = pNode;
            return pNode->pInfo->p0 != NULL;
        }
        if (pNode->pInfo != NULL && pNode->pInfo->p0 != NULL) {
            nCount = pNode->nGroups;
            // fake match: pLoop8 and pPrev8 always hold pNode; the copies carried round the loop
            // keep pNode apart from p (EA's mr r27,r6 with the first read through r6), and taking
            // pPrev8 before the copy keeps EA's argument order (li r5,7 before the lwzx).
            pLoop8 = pNode;
            for (i = 0; i < nCount; i++) {
                pPrev8 = pLoop8;
                pLoop8 = pNode;
                nFound = fn_8016AD54(pScreen, pInfo, 7, pPrev8->ppGroups[i]);
                // fake match: nFound goes through a 64-bit shift up and back down (the value is
                // unchanged). The shifts become a chain of word copies; each copy-propagation pass
                // removes one link, so EA's copy of the call result survives (mr r0,r3; cmpwi r0,-1).
                // port: relies on the conversion to s64 wrapping and on >> of a negative s64 being
                // arithmetic.
                nFound = (s32)((s64)((u64)(u32)nFound << 32) >> 32);
                if (nFound != -1) return nFound;
            }
        }
        break;
    case 7:
        pGroup = p;
        if (pGroup->pInfo == pInfo) {
            pInfo->p0 = pGroup;
            return pGroup->pInfo->p0 != NULL;
        }
        if (pGroup->pInfo != NULL && pGroup->pInfo->p0 != NULL) {
            nCount = pGroup->nEntries;
            // fake match: pLoop7 always holds pGroup; the copy carried round the loop keeps pGroup
            // apart from p (EA's mr r26,r6 with the first read through r6).
            pLoop7 = pGroup;
            for (i = 0; i < nCount; i++) {
                pEntry = &pLoop7->pEntries[i];
                pLoop7 = pGroup;
                if (pEntry->uHandler == 0xFFFF) {
                    nFound = fn_8016AD54(pScreen, pInfo, 8, &pScreen->pData->pNodes[pEntry->u4.nNode]);
                    // fake match: the same 64-bit shift as in case 8, then a 64-bit round trip (both
                    // leave the value unchanged); this block needs one copy link more to keep EA's
                    // copy of the call result (mr r0,r3; cmpwi r0,-1).
                    // port: as in case 8.
                    nFound = (s32)((s64)((u64)(u32)nFound << 32) >> 32);
                    nFound = (s32)(u64)(u32)nFound;
                    if (nFound != -1) return nFound;
                }
            }
        }
        break;
    }
    return -1;
}

// Runs every handler under a node (nKind 8) or a group (nKind 7) with message -4 and a pointer to
// n. Without bAll, nodes and groups whose info has no owner are skipped.
void fn_8016ABBC(UIStudio* pStudio, UISScreen* pScreen, s32 n, s32 nKind, void* p, u8 bAll) {
    // fake match: this declaration order (with the copies below) gives EA's registers.
    UISGroup* pGroup;
    UISNode* pNode;
    u32 i;
    // fake match: one count for both loops (nGroups, then nEntries), as in fn_8016AD54: with it
    // case 7's counter and count take EA's registers (li r29,0 / lwz r30 / mr r31,r29).
    u32 nCount;
    UISEntry* pEntry;
    UISNode* pPrev8;
    UISNode* pLoop8;
    UISGroup* pLoop7;

    if (pStudio == NULL) return;
    if (pScreen == NULL || p == NULL) return;
    switch (nKind) {
    case 8:
        pNode = p;
        if (pNode->pInfo != NULL && (bAll || pNode->pInfo->p0 != NULL)) {
            nCount = pNode->nGroups;
            // fake match: pLoop8 and pPrev8 always hold pNode; the copies carried round the loop
            // keep pNode apart from p (EA's mr r29,r7 with the first read through r7), and taking
            // pPrev8 before the copy keeps EA's argument order (the lwzx before li r6,7).
            pLoop8 = pNode;
            for (i = 0; i < nCount; i++) {
                pPrev8 = pLoop8;
                pLoop8 = pNode;
                fn_8016ABBC(pStudio, pScreen, n, 7, pPrev8->ppGroups[i], 0);
            }
        }
        break;
    case 7:
        pGroup = p;
        if (pGroup->pInfo != NULL && (bAll || pGroup->pInfo->p0 != NULL)) {
            nCount = pGroup->nEntries;
            // fake match: pLoop7 always holds pGroup; the copy carried round the loop keeps pGroup
            // apart from p (EA's mr r28,r7 with the first read through r7).
            pLoop7 = pGroup;
            for (i = 0; i < nCount; i++) {
                pEntry = &pLoop7->pEntries[i];
                pLoop7 = pGroup;
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

// Hands node nNode and every node it links to to the transform callback with operation nOp.
// Operations 0 and 3 also reach groups whose info has no owner.
void fn_8016A830(UIStudio* pStudio, int nOp, UISScreen* pScreen, u32 nNode) {
    UISNode* pNode;
    u32 i;

    if (pScreen->pData != NULL) {
        pNode = &pScreen->pData->pNodes[nNode];
        pStudio->pfnTransform(nOp, pNode->pInfo->af8);
        for (i = 0; i < pNode->nGroups; i++) {
            UISGroup* pGroup = pNode->ppGroups[i];
            if (pGroup->pInfo->p0 != NULL || nOp == 0 || nOp == 3) {
                u32 j;
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

// Draws node nNode and the nodes it links to: runs the screen file's start entries, then, if the
// node is shown, draws its children with its own values folded into the studio's.
void fn_8016A510(UIStudio* pStudio, UISScreen* pScreen, u32 nNode, s32 nMsg) {
    UISNode* pNode;
    u32 j;
    UISVec4 mul;
    UISVec4 add;
    UISNodeInfo* pInfo;

    // fake match: pScreen through a 64-bit round trip (the same pointer); the conversion's dead
    // high word (srawi) is deleted by the register allocator, and it moves the pScreen copy after
    // the pData load as in the original. port: truncates the pointer to 32 bits.
    if (((UISScreen*)(s64)(s32)pScreen)->pData != NULL) {
        pNode = &pScreen->pData->pNodes[nNode];
        // fake match: nNode (dead after pNode) is the counter of both entry loops, which gives the
        // original's loop registers.
        for (nNode = 0; nNode < pScreen->pData->nStart; nNode++) {
            UISEntry* pEntry = &pScreen->pData->pStart[nNode];
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
            // fake match: the same 64-bit round trip on the pointer (the same address); its dead
            // high word gives the original's order for the copy's loads among the products.
            // port: truncates the pointer to 32 bits.
            add = *(UISVec4*)(s64)(s32)fn_8016C18C();
            pInfo = pNode->pInfo;
            fn_8016C174(mul.a[0] * pInfo->afMul[0], mul.a[1] * pInfo->afMul[1], mul.a[2] * pInfo->afMul[2],
                        mul.a[3] * pInfo->afMul[3]);
            fn_8016C15C(add.a[0] + pInfo->afAdd[0], add.a[1] + pInfo->afAdd[1], add.a[2] + pInfo->afAdd[2],
                        add.a[3] + pInfo->afAdd[3]);
            pStudio->pfnTransform(1, pNode->pInfo->af8);
            for (j = 0; j < pNode->nGroups; j++) {
                UISGroup* pGroup = pNode->ppGroups[j];
                if (pGroup->pInfo->p0 != NULL) {
                    for (nNode = 0; nNode < pGroup->nEntries; nNode++) {
                        UISEntry* pEntry = &pGroup->pEntries[nNode];
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

// Sends event uEvent to node nNode and, first, to the nodes it links to. A node takes events only
// while its info has u4 and u60 set, except the studio's own events (n5 -2 to -5 and -8 to -11).
// A linked node that answers with 1 gets the handler this node has for it. Returns 2 as soon as a
// handler returns 2.
s32 fn_8016A2D4(UIStudio* pStudio, UISScreen* pScreen, UISWordStack* pStack, u32 nNode, u32 uEvent, u32 n5,
                s32 nArgs, s32* pArgs, u8* pbOut) {
    // fake match: uEventLoop, uEventPost and uEventPre all hold uEvent (see below); these copies,
    // this declaration order and the function-level pHandler / nRet / pLinked give EA's registers.
    s32 uEventLoop;
    u32 uEventPost;
    s32 uEventPre;
    UISHandler* pHandler;
    UISNode* pNode;
    s32 nResult;
    u32 i;
    s32 nRet;
    u8* pLinked;
    u8* pScript;
    u8 bOut;

    nResult = 0;
    if (pScreen->pData == NULL || nNode >= pScreen->pData->nNodes) return 0;
    uEventPost = uEvent;
    pNode = &pScreen->pData->pNodes[nNode];
    if ((pNode->pInfo->u4 != 0 && pNode->pInfo->u60 != 0) || n5 - (u32)-10 <= 2 ||
        n5 - (u32)-5 <= 3 || n5 == (u32)-11) {
        // fake match: uEventPre is uEvent: the OR's low word is uEvent | 0 (uEvent shifted up only
        // fills the high word, which is dropped). It is a copy only after constant propagation, so
        // with the loop's two links below the late copy-propagation passes stop at this one: EA's
        // kept `mr r19,r28` for the linked handler's event. The dead high-word OR leaves no code.
        uEventPre = (s32)((u64)(u32)uEvent | ((u64)(u32)uEvent << 32));
        for (i = 0; i < pNode->nHandlers; i++) {
            pHandler = &pNode->pHandlers[i];
            // fake match: uEventPost again (the same value); the second definition keeps it a
            // variable of its own.
            uEventPost = uEvent;
            // fake match: uEventLoop is uEventPre: i only goes into the dropped high word. Two
            // links of copies-after-constant-propagation, loop-variant so they stay in the loop.
            uEventLoop = (s32)((u64)(u32)uEventPre | ((u64)(u32)i << 32));
            uEventLoop = (s32)((u64)(u32)uEventLoop | ((u64)(u32)i << 32));
            if (pHandler->uEvent == 0xFFFF) {
                bOut = 0;
                nResult = fn_8016A2D4(pStudio, pScreen, pStack, pHandler->u4.nNode, uEvent, n5, nArgs, pArgs,
                                      &bOut);
                // fake match: the (s32) gives EA's signed cmpwi
                if ((s32)bOut == 1) {
                    pLinked = fn_8016C614(pNode, (u16)pHandler->u4.nNode, n5);
                    if (pLinked != NULL) {
                        nRet = fn_8016C270(pStudio, pScreen, pNode->pInfo, pStack, pLinked, nArgs, pArgs, 0,
                                           NULL, 1, uEventLoop, NULL);
                        // fake match: nRet through a 64-bit shift up and back down (unchanged), then
                        // a dropped identity conversion: the copy chain keeps EA's copy of the call
                        // result (mr r0,r3; cmpwi r0,2).
                        // port: relies on the conversion to s64 wrapping and on >> of a negative s64
                        // being arithmetic.
                        nRet = (s32)((s64)((u64)(u32)nRet << 32) >> 32);
                        nRet = (u32)(s32)nRet;
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
        // fake match: uEvent goes into the high word of a 64-bit OR whose low word is nResult, so
        // nResult is unchanged. The dead OR keeps uEvent live across the calls above at register
        // allocation (its own register, copied from r7 with the other parameters, as in EA) and is
        // deleted after allocation.
        // port: a port leaves this line out.
        nResult = (s32)((u64)(s64)nResult | ((u64)(u32)uEvent << 32));
        if (pScript != NULL) {
            nResult = fn_8016C270(pStudio, pScreen, pNode->pInfo, pStack, pScript, nArgs, pArgs, 0, NULL, 1,
                                  uEventPost, NULL);
        }
        if (nResult == 2) return nResult;
    }
    *pbOut = pNode->pInfo->u4;
    return nResult;
}

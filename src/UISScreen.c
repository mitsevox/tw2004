// UISScreen.c (our name): the screen side of EA's UI Studio library (frontend/uistudio.h). It walks
// a loaded screen's nodes to draw them with a scale and offset, finds and runs the handlers
// nodes have for an event, formats text for them (a printf of its own) and finds the variable a
// rate function drives. Called by UIStudio.c and by the game's menus. The original was built with
// automatic inlining (-inline auto in configure.py): _ParseTransforms and _ParseHints have their own
// recursion inlined three deep.
// section order: built with -inline auto,deferred, which emits the functions last-first, so they
// are written here from the highest address down.

#include "frontend/uistudio.h"

void _ParseHints(UISInfoT* pStudio, UISScreenT* pScreen, UISStackInfoT* pStack, u32 nNode, u32 uEvent,
                 s32 nArgs, s32* pArgs);
char* _WriteFloat(char* pOut, char* pEnd, s32 nWidth, s32 nPrec, f32 f);

// .bss, reverse address order
UISColorVectorT _MultiplerColorFactor;
UISColorVectorT _AdditiveColorFactor;

// Whether one of pNode's groups links to the node pInfo belongs to.
static inline u8 UIS_NodeLinks(UISScrDataT* pData, UISControlT* pNode, UISControlInfoT* pInfo) {
    UISLayerT* pGroup;
    UISObjT* pEntry;
    u32 i;
    u32 j;

    for (i = 0; i < pNode->NumLayers; i++) {
        pGroup = pNode->Layers[i];
        for (j = 0; j < pGroup->NumObjs; j++) {
            pEntry = &pGroup->Objs[j];
            if (pEntry->PluginIndex == 0xFFFF && pData->Controls[pEntry->nNode].pControlInfo == pInfo) {
                return 1;
            }
        }
    }
    return 0;
}

// The info of the first node pNode links to whose IsEnabled is set, or NULL.
static inline UISControlInfoT* UIS_LinkedOn(UISScrDataT* pData, UISControlT* pNode) {
    UISLayerT* pGroup;
    UISObjT* pEntry;
    UISControlInfoT* pLinked;
    u32 i;
    u32 j;

    for (i = 0; i < pNode->NumLayers; i++) {
        pGroup = pNode->Layers[i];
        for (j = 0; j < pGroup->NumObjs; j++) {
            pEntry = &pGroup->Objs[j];
            if (pEntry->PluginIndex == 0xFFFF) {
                pLinked = pData->Controls[pEntry->nNode].pControlInfo;
                if (pLinked->IsEnabled != 0) return pLinked;
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
u16 UISFindScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen) {
    u16 i;
    UISScreenT* pScreen;

    for (i = 0; i < pStudio->NumScreens; i++) {
        pScreen = &pStudio->Screens[i];
        if (pScreen->GroupID == uGroup && pScreen->ScreenID == uScreen) break;
    }
    return i;
}
#pragma scheduling reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// A node's handler of the kind marked 0x8000 for an event.
u8* UISFindEventPC(UISControlT* pNode, u32 uEvent) {
    u32 i;
    for (i = 0; i < pNode->NumMaps; i++) {
        UISMapT* pHandler = &pNode->Maps[i];
        if ((pHandler->ControlIndex & 0x8000) && pHandler->EventID == (u16)uEvent) {
            return pHandler->pFnc;
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
u8* UISFindSubControlEventPC(UISControlT* pNode, u16 uId, u32 uEvent) {
    u32 i;
    int nId = uId;
    for (i = 0; i < pNode->NumMaps; i++) {
        UISMapT* pHandler = &pNode->Maps[i];
        if (!(pHandler->ControlIndex & 0xC000) && pHandler->EventID == (u16)uEvent &&
            (pHandler->ControlIndex & 0x2FFF) == nId) {
            return pHandler->pFnc;
        }
    }
    return NULL;
}
#pragma scheduling reset
#pragma auto_inline reset

// The node's handler of the kind marked 0x4000 for an event.
u8* _UISFindHintPC(UISControlT* pNode, u32 uEvent) {
    u32 i;
    for (i = 0; i < pNode->NumMaps; i++) {
        UISMapT* pHandler = &pNode->Maps[i];
        if ((pHandler->ControlIndex & 0x4000) && pHandler->EventID == (u16)uEvent) {
            return pHandler->pFnc;
        }
    }
    return NULL;
}

// Pushes a call frame on pStack (the saved word, the extra word, both argument lists, the node's
// info and a 0) and runs pScript on it. The frame stays on the stack only when the script
// returns 3 (it paused).
s32 UISExecuteFnc(UISInfoT* pStudio, UISScreenT* pScreen, UISControlInfoT* pInfo, UISStackInfoT* pStack,
                  u8* pScript, s32 nArgs, s32* pArgs, u32 nArgs2, const s32* pArgs2, u8 bExtra, s32 nExtra,
                  s32* pnSaved) {
    // fake match: pStackCopy is pStack through a void* copy, declared first; the local's higher
    // variable number makes the allocator colour it before pFrame and pnSaved (EA's r31).
    UISStackInfoT* pStackCopy = (UISStackInfoT*)(void*)pStack;
    s32* pFrame;
    u32 i;
    s32 nResult;
    void* pScriptCopy = pScript;

    // fake match: pScriptCopy is pScript, so this OR leaves pScript unchanged. The frontend cannot
    // fold the OR of two variables; it stays as `or r11,r7,r7` (the original's `mr r11,r7`, the
    // same encoding), which the allocator never coalesces, so pScript leaves r7 as in EA.
    // port: the pointer goes through a 32-bit integer
    pScript = (u8*)((u32)pScript | (u32)pScriptCopy);
    pFrame = pStackCopy->pStack;
    if (pnSaved == NULL) {
        *pFrame = 0;
    } else {
        *pFrame = *pnSaved;
    }
    pStackCopy->pStack++;
    if (bExtra) {
        *pStackCopy->pStack = nExtra;
        pStackCopy->pStack++;
    }
    for (i = 0; i < nArgs; i++) {
        *pStackCopy->pStack = pArgs[i];
        pStackCopy->pStack++;
    }
    for (i = 0; i < nArgs2; i++) {
        *pStackCopy->pStack = pArgs2[i];
        pStackCopy->pStack++;
    }
    // port: the frame keeps the info pointer in a word
    *pStackCopy->pStack = (s32)pInfo;
    pStackCopy->pStack++;
    *pStackCopy->pStack = 0;
    pStackCopy->pStack++;
    pStackCopy->pPC = pScript;
    nResult = UISStackProcess(pStudio, pFrame, pStackCopy, pScreen, pInfo);
    if (pnSaved != NULL) {
        *pnSaved = *pFrame;
    }
    if (nResult != 3) {
        pStackCopy->pStack = pFrame;
    }
    return nResult;
}

f32* UISGetActionPtrValue(s32 n20, UISControlInfoT* pInfo) {
    switch (n20) {
    case 16:
        return &pInfo->Transform.Rotation.x;
    case 17:
        return &pInfo->Transform.Rotation.y;
    case 18:
        return &pInfo->Transform.Rotation.z;
    case 19:
        return &pInfo->Transform.Offset.x;
    case 20:
        return &pInfo->Transform.Offset.y;
    case 21:
        return &pInfo->Transform.Offset.z;
    case 22:
        return &pInfo->Transform.Scale.x;
    case 23:
        return &pInfo->Transform.Scale.y;
    case 24:
        return &pInfo->Transform.Scale.z;
    case 25:
        return &pInfo->Transform.Pivot.x;
    case 26:
        return &pInfo->Transform.Pivot.y;
    case 27:
        return &pInfo->Transform.Pivot.z;
    case 32:
        return &pInfo->Transform.AdditiveFactor.a;
    case 33:
        return &pInfo->Transform.AdditiveFactor.r;
    case 34:
        return &pInfo->Transform.AdditiveFactor.g;
    case 35:
        return &pInfo->Transform.AdditiveFactor.b;
    case 36:
        return &pInfo->Transform.MultiplerFactor.a;
    case 37:
        return &pInfo->Transform.MultiplerFactor.r;
    case 38:
        return &pInfo->Transform.MultiplerFactor.g;
    case 39:
        return &pInfo->Transform.MultiplerFactor.b;
    default:
        return &pInfo->Transform.MultiplerFactor.a;
    }
}

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
UISColorVectorT* UISGetColorMultipler(void) {
    return &_MultiplerColorFactor;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
UISColorVectorT* UISGetColorAdditive(void) {
    return &_AdditiveColorFactor;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
void UISSetColorMultipler(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    _MultiplerColorFactor.r = f1;
    _MultiplerColorFactor.a = f4;
    _MultiplerColorFactor.g = f2;
    _MultiplerColorFactor.b = f3;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// The values every node is drawn with: _ParseObjects adds a node's Transform.AdditiveFactor to the first and
// multiplies its Transform.MultiplerFactor into the second for the node's children.
void UISSetColorAdditive(f32 f1, f32 f2, f32 f3, f32 f4) {
    // fake match: the original stores the fourth value second
    _AdditiveColorFactor.r = f1;
    _AdditiveColorFactor.a = f4;
    _AdditiveColorFactor.g = f2;
    _AdditiveColorFactor.b = f3;
}
#pragma auto_inline reset

// Writes f with nPrec decimals (6 when negative), padded with spaces to nWidth characters, into
// pOut up to pEnd. Returns the end of what it wrote.
char* _WriteFloat(char* pOut, char* pEnd, s32 nWidth, s32 nPrec, f32 f) {
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

// fake match: UISSprintf's 'x' case as an inline. Inlined code gets the frontend's goto
// cleanup that a large function body skips, which places the output loop's preheader at the
// end of UISSprintf as in EA's code. Code is unchanged.
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

// fake match: UISSprintf's 'd' case as an inline, for the same preheader placement as
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

// fake match: UISSprintf's start pointer read through an inline whose parameter is changed (the
// dead p++), so the parameter is a frontend variable numbered before every other inline's; the
// start then ranks lowest of the saved registers (EA's r26). Returns p unchanged.
static inline char* fn_8016B844_Get(char* p) {
    return p++;
}

// Formats szFormat with pArgs into pOut (nSize bytes). Returns the length written, or -1.
// EA bug: '-' is never cleared, so every conversion after one with '-' is left-justified too.
s32 UISSprintf(char* pOut, s32 nSize, const char* szFormat, s32 nArgs, const UISParamT* pArgs) {
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
                *pOut++ = pArgs[nArg++].iValue;
                continue;
            case 's': {
                UISStringT* pText = pArgs[nArg++].strAddr;
                if (pText->ptr != NULL) {
                    pOut = UIS_PutString(pOut, pEnd, pText->ptr, nWidth, bLeft);
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
                pOut = _WriteFloat(pOut, pEnd, nWidth, nPrec, pArgs[nArg++].fValue);
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
void UISStringFormat(u32 u0, UISStringT* pOut, UISStringT* pFormat, s32 nArgs, const UISParamT* pArgs) {
    if (pFormat != NULL && pOut != NULL) {
        UISSprintf(pOut->ptr, pOut->length, pFormat->ptr, nArgs, pArgs);
    }
}

// Finds the node that links to pInfo's node and returns the info of the first node it links to
// whose IsEnabled is set.
UISControlInfoT* UISFindSiblingEnableControl(UISScreenT* pScreen, UISControlInfoT* pInfo) {
    UISScrDataT* pData;
    u32 i;

    pData = pScreen->pScrData;
    for (i = 0; i < pData->NumControls; i++) {
        UISControlT* pNode = &pData->Controls[i];
        if (UIS_NodeLinks(pData, pNode, pInfo)) {
            return UIS_LinkedOn(pData, pNode);
        }
    }
    return NULL;
}

// Moves a loaded screen nMove places up or down the screen table, one swap at a time, keeping
// the current screen, the rate functions and the ModalStack records on the screens they named.
void UISMoveScreenDrawPosition(UISInfoT* pStudio, u16 uGroup, u16 uScreen, s32 nMove) {
    s32 nScreens;
    s32 nLimit;
    s32 nIndex;
    s32 nCount;
    s32 nFrom;
    s32 nTo;
    u32 i;
    u32 j;
    s32 nStep;
    UISScreenT tmp;

    nIndex = UISFindScreen(pStudio, uGroup, uScreen);
    nScreens = pStudio->NumScreens;
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
            if (pStudio->ActiveScreenIdx == nTo) {
                pStudio->ActiveScreenIdx = nFrom;
            } else if (pStudio->ActiveScreenIdx == nFrom) {
                pStudio->ActiveScreenIdx = nTo;
            }
            for (i = 0; i < pStudio->NumRateFncs; i++) {
                if (pStudio->RateFncs[i].pScreen == &pStudio->Screens[nTo]) {
                    pStudio->RateFncs[i].pScreen = &pStudio->Screens[nFrom];
                } else if (pStudio->RateFncs[i].pScreen == &pStudio->Screens[nFrom]) {
                    pStudio->RateFncs[i].pScreen = &pStudio->Screens[nTo];
                }
            }
            for (j = 0; j < pStudio->NumModals; j++) {
                if (pStudio->ModalStack[j].pScreen == &pStudio->Screens[nTo]) {
                    pStudio->ModalStack[j].pScreen = &pStudio->Screens[nFrom];
                } else if (pStudio->ModalStack[j].pScreen == &pStudio->Screens[nFrom]) {
                    pStudio->ModalStack[j].pScreen = &pStudio->Screens[nTo];
                }
            }
            memcpy(&tmp, &pStudio->Screens[nTo], sizeof(UISScreenT));
            memcpy(&pStudio->Screens[nTo], &pStudio->Screens[nFrom], sizeof(UISScreenT));
            memcpy(&pStudio->Screens[nFrom], &tmp, sizeof(UISScreenT));
        }
    }
}

// Runs the 0x4000 handlers for an event of node nNode and of every node it links to, the linked
// nodes first.
void _ParseHints(UISInfoT* pStudio, UISScreenT* pScreen, UISStackInfoT* pStack, u32 nNode, u32 uEvent,
                 s32 nArgs, s32* pArgs) {
    UISControlT* pNode;
    u32 i;
    u8* pScript;

    // fake match: the byte offset as a 64-bit product (low word = nNode * 20, the same address as
    // Controls[nNode]); its dead high word (li 20; mulhw) goes first in the pre-allocation schedule,
    // which moves the pStudio copy after the pScrData load as in the original, and is deleted later.
    // port: the offset is truncated to 32 bits.
    pNode = (UISControlT*)((u8*)pScreen->pScrData->Controls + (s32)nNode * (s64)sizeof(UISControlT));
    for (i = 0; i < pNode->NumMaps; i++) {
        UISMapT* pHandler = &pNode->Maps[i];
        if (pHandler->EventID == 0xFFFF) {
            _ParseHints(pStudio, pScreen, pStack, pHandler->nNode, uEvent, nArgs, pArgs);
        }
    }
    // The node's script for the event (_UISFindHintPC's search). The original has _UISFindHintPC
    // inlined here and called only at the deepest inlined level; a static inline copy of it matches
    // better (93%) but adds a function the original does not have, so the unit could not link.
    pScript = _UISFindHintPC(pNode, uEvent);
    if (pScript != NULL) {
        UISExecuteFnc(pStudio, pScreen, pNode->pControlInfo, pStack, pScript, nArgs, pArgs, 0, NULL, 0, 0,
                      NULL);
    }
}

void UISDoHint(UISInfoT* pStudio, u32 uEvent, s32 nArgs, s32* pArgs) {
    u32 i;
    u32 nScreens = pStudio->NumScreens;
    for (i = 0; i < nScreens; i++) {
        UISScreenT* pScreen = &pStudio->Screens[i];
        pStudio->CriticalRegions |= 2;
        _ParseHints(pStudio, pScreen, &pStudio->EventStack, 0, uEvent, nArgs, pArgs);
        pStudio->CriticalRegions &= ~2;
    }
}

// Send an event to every screen. While the studio is busy (flag 2: sending an event; 4: running
// its rate functions), it is queued on the event stack instead.
void fn_8016B09C(UISInfoT* pStudio, u32 uEvent, s32 nArgs, s32* pArgs) {
    UISThreadGroupInfoT data;
    if ((pStudio->CriticalRegions & 2) || (pStudio->CriticalRegions & 4)) {
        data.GenericInfo.Data[0] = uEvent;
        UISAddThreadAction(-1, -1, pStudio, 9, &data, nArgs, pArgs);
    } else {
        UISDoHint(pStudio, uEvent, nArgs, pArgs);
    }
}

// fake match: an identity read; it gives EA's register order.
static inline u8 fn_8016AEEC_Read(u8 b) { return b; }

// Runs the screen file's start entries that have not run yet, then every handler under node
// nNode; a handler run with message -1 is marked as run.
void _ParseInitialize(UISInfoT* pStudio, UISScreenT* pScreen, u32 nNode, s32 nMsg) {
    // fake match: the second loop's pEntry is declared here, ahead of pNode and j, and shadowed by
    // the first loop's own pEntry; this order gives the original's loop registers.
    u8 bLast;
    UISObjT* pEntry;
    UISControlT* pNode;
    u32 j;

    if (pScreen->pScrData != NULL) {
        pNode = &pScreen->pScrData->Controls[nNode];
        bLast = fn_8016AEEC_Read(nMsg == -1);
        // fake match: nNode (dead after pNode) is the counter of both entry loops, which gives the
        // original's loop registers.
        for (nNode = 0; nNode < pScreen->pScrData->NumStaticObjects; nNode++) {
            UISObjT* pEntry = &pScreen->pScrData->StaticObjects[nNode];
            if (pEntry->bInitialized == 0) {
                if (pEntry->PluginIndex < pStudio->NumPlugins) {
                    UISPluginFncT* pfnHandler = pStudio->Plugins[pEntry->PluginIndex].pFnc;
                    if (pfnHandler != NULL) {
                        pfnHandler(pEntry->pData != NULL ? (u8*)pScreen->pScrData + *pEntry->pData
                                                               : NULL,
                                   nMsg, 0, NULL, 0);
                    }
                }
                pEntry->bInitialized = 1;
            }
        }
        for (j = 0; j < pNode->NumLayers; j++) {
            // fake match: b is bLast, and the stored `b | bLast` is bLast. The frontend cannot fold the
            // OR of two variables, so after the copy is propagated it stays as `or r30,r27,r27` (the
            // original's `mr r30,r27`, the same encoding), which the allocator never coalesces.
            int b = bLast;
            UISLayerT* pGroup = pNode->Layers[j];
            for (nNode = 0; nNode < pGroup->NumObjs; nNode++) {
                pEntry = &pGroup->Objs[nNode];
                if (pEntry->PluginIndex == 0xFFFF) {
                    _ParseInitialize(pStudio, pScreen, pEntry->nNode, nMsg);
                } else if (pEntry->PluginIndex < pStudio->NumPlugins) {
                    UISPluginFncT* pfnHandler = pStudio->Plugins[pEntry->PluginIndex].pFnc;
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pScrData + *pEntry->pData, nMsg, 0, NULL, 0);
                        pEntry->bInitialized = b | bLast;
                    }
                }
            }
        }
    }
}

// Looks under a node (nKind 8) or a group (nKind 7) for the one pInfo belongs to and records it
// as pInfo's owner. Returns -1 when it is not found.
s32 _DetermineVisibility(UISScreenT* pScreen, UISControlInfoT* pInfo, s32 nKind, void* p) {
    // fake match: this declaration order (with the copies below) gives EA's registers.
    UISLayerT* pGroup;
    u32 i;
    UISControlT* pNode;
    // fake match: one count for both loops (NumLayers, then NumObjs): with it NumObjs takes r28
    // and case 7's counter r27 (EA's li r27,0 / lwz r28 / mr r29,r27); with two, they swap.
    u32 nCount;
    UISObjT* pEntry;
    UISControlT* pLoop8;
    UISControlT* pPrev8;
    UISLayerT* pLoop7;
    s32 nFound;

    if (pInfo == NULL || p == NULL || pScreen == NULL) return -1;
    switch (nKind) {
    case 8:
        pNode = p;
        if (pNode->pControlInfo == pInfo) {
            pInfo->IsVisible = pNode;
            return pNode->pControlInfo->IsVisible != NULL;
        }
        if (pNode->pControlInfo != NULL && pNode->pControlInfo->IsVisible != NULL) {
            nCount = pNode->NumLayers;
            // fake match: pLoop8 and pPrev8 always hold pNode; the copies carried round the loop
            // keep pNode apart from p (EA's mr r27,r6 with the first read through r6), and taking
            // pPrev8 before the copy keeps EA's argument order (li r5,7 before the lwzx).
            pLoop8 = pNode;
            for (i = 0; i < nCount; i++) {
                pPrev8 = pLoop8;
                pLoop8 = pNode;
                nFound = _DetermineVisibility(pScreen, pInfo, 7, pPrev8->Layers[i]);
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
        if (pGroup->pLayerInfo == pInfo) {
            pInfo->IsVisible = pGroup;
            return pGroup->pLayerInfo->IsVisible != NULL;
        }
        if (pGroup->pLayerInfo != NULL && pGroup->pLayerInfo->IsVisible != NULL) {
            nCount = pGroup->NumObjs;
            // fake match: pLoop7 always holds pGroup; the copy carried round the loop keeps pGroup
            // apart from p (EA's mr r26,r6 with the first read through r6).
            pLoop7 = pGroup;
            for (i = 0; i < nCount; i++) {
                pEntry = &pLoop7->Objs[i];
                pLoop7 = pGroup;
                if (pEntry->PluginIndex == 0xFFFF) {
                    nFound = _DetermineVisibility(pScreen, pInfo, 8,
                                                  &pScreen->pScrData->Controls[pEntry->nNode]);
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
void _ParseVisibility(UISInfoT* pStudio, UISScreenT* pScreen, s32 n, s32 nKind, void* p, u8 bAll) {
    // fake match: this declaration order (with the copies below) gives EA's registers.
    UISLayerT* pGroup;
    UISControlT* pNode;
    u32 i;
    // fake match: one count for both loops (NumLayers, then NumObjs), as in _DetermineVisibility: with it
    // case 7's counter and count take EA's registers (li r29,0 / lwz r30 / mr r31,r29).
    u32 nCount;
    UISObjT* pEntry;
    UISControlT* pPrev8;
    UISControlT* pLoop8;
    UISLayerT* pLoop7;

    if (pStudio == NULL) return;
    if (pScreen == NULL || p == NULL) return;
    switch (nKind) {
    case 8:
        pNode = p;
        if (pNode->pControlInfo != NULL && (bAll || pNode->pControlInfo->IsVisible != NULL)) {
            nCount = pNode->NumLayers;
            // fake match: pLoop8 and pPrev8 always hold pNode; the copies carried round the loop
            // keep pNode apart from p (EA's mr r29,r7 with the first read through r7), and taking
            // pPrev8 before the copy keeps EA's argument order (the lwzx before li r6,7).
            pLoop8 = pNode;
            for (i = 0; i < nCount; i++) {
                pPrev8 = pLoop8;
                pLoop8 = pNode;
                _ParseVisibility(pStudio, pScreen, n, 7, pPrev8->Layers[i], 0);
            }
        }
        break;
    case 7:
        pGroup = p;
        if (pGroup->pLayerInfo != NULL && (bAll || pGroup->pLayerInfo->IsVisible != NULL)) {
            nCount = pGroup->NumObjs;
            // fake match: pLoop7 always holds pGroup; the copy carried round the loop keeps pGroup
            // apart from p (EA's mr r28,r7 with the first read through r7).
            pLoop7 = pGroup;
            for (i = 0; i < nCount; i++) {
                pEntry = &pLoop7->Objs[i];
                pLoop7 = pGroup;
                if (pEntry->PluginIndex == 0xFFFF) {
                    _ParseVisibility(pStudio, pScreen, n, 8, &pScreen->pScrData->Controls[pEntry->nNode], 0);
                } else if (pEntry->PluginIndex < pStudio->NumPlugins) {
                    UISPluginFncT* pfnHandler = pStudio->Plugins[pEntry->PluginIndex].pFnc;
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pScrData + *pEntry->pData, -4, 1, &n, 0);
                    }
                }
            }
        }
        break;
    }
}

// Hands node nNode and every node it links to to the transform callback with operation nOp.
// Operations 0 and 3 also reach groups whose info has no owner.
void _ParseTransforms(UISInfoT* pStudio, int nOp, UISScreenT* pScreen, u32 nNode) {
    UISControlT* pNode;
    u32 i;

    if (pScreen->pScrData != NULL) {
        pNode = &pScreen->pScrData->Controls[nNode];
        pStudio->pTransformFnc(nOp, &pNode->pControlInfo->Transform);
        for (i = 0; i < pNode->NumLayers; i++) {
            UISLayerT* pGroup = pNode->Layers[i];
            if (pGroup->pLayerInfo->IsVisible != NULL || nOp == 0 || nOp == 3) {
                u32 j;
                for (j = 0; j < pGroup->NumObjs; j++) {
                    UISObjT* pEntry = &pGroup->Objs[j];
                    if (pEntry->PluginIndex == 0xFFFF) {
                        _ParseTransforms(pStudio, nOp, pScreen, pEntry->nNode);
                    }
                }
            }
        }
    }
}

// Draws node nNode and the nodes it links to: runs the screen file's start entries, then, if the
// node is shown, draws its children with its own values folded into the studio's.
void _ParseObjects(UISInfoT* pStudio, UISScreenT* pScreen, u32 nNode, s32 nMsg) {
    UISControlT* pNode;
    u32 j;
    UISColorVectorT mul;
    UISColorVectorT add;
    UISControlInfoT* pInfo;

    // fake match: pScreen through a 64-bit round trip (the same pointer); the conversion's dead
    // high word (srawi) is deleted by the register allocator, and it moves the pScreen copy after
    // the pScrData load as in the original. port: truncates the pointer to 32 bits.
    if (((UISScreenT*)(s64)(s32)pScreen)->pScrData != NULL) {
        pNode = &pScreen->pScrData->Controls[nNode];
        // fake match: nNode (dead after pNode) is the counter of both entry loops, which gives the
        // original's loop registers.
        for (nNode = 0; nNode < pScreen->pScrData->NumStaticObjects; nNode++) {
            UISObjT* pEntry = &pScreen->pScrData->StaticObjects[nNode];
            if (pEntry->PluginIndex < pStudio->NumPlugins) {
                UISPluginFncT* pfnHandler = pStudio->Plugins[pEntry->PluginIndex].pFnc;
                if (pfnHandler != NULL) {
                    pfnHandler(pEntry->pData != NULL ? (u8*)pScreen->pScrData + *pEntry->pData
                                                           : NULL,
                               nMsg, 0, NULL, 0);
                }
            }
        }
        if (pNode->pControlInfo->IsVisible != NULL) {
            mul = *UISGetColorMultipler();
            // fake match: the same 64-bit round trip on the pointer (the same address); its dead
            // high word gives the original's order for the copy's loads among the products.
            // port: truncates the pointer to 32 bits.
            add = *(UISColorVectorT*)(s64)(s32)UISGetColorAdditive();
            pInfo = pNode->pControlInfo;
            UISSetColorMultipler(mul.r * pInfo->Transform.MultiplerFactor.r,
                                 mul.g * pInfo->Transform.MultiplerFactor.g,
                                 mul.b * pInfo->Transform.MultiplerFactor.b,
                                 mul.a * pInfo->Transform.MultiplerFactor.a);
            UISSetColorAdditive(add.r + pInfo->Transform.AdditiveFactor.r,
                                add.g + pInfo->Transform.AdditiveFactor.g,
                                add.b + pInfo->Transform.AdditiveFactor.b,
                                add.a + pInfo->Transform.AdditiveFactor.a);
            pStudio->pTransformFnc(1, &pNode->pControlInfo->Transform);
            for (j = 0; j < pNode->NumLayers; j++) {
                UISLayerT* pGroup = pNode->Layers[j];
                if (pGroup->pLayerInfo->IsVisible != NULL) {
                    for (nNode = 0; nNode < pGroup->NumObjs; nNode++) {
                        UISObjT* pEntry = &pGroup->Objs[nNode];
                        if (pEntry->PluginIndex == 0xFFFF) {
                            _ParseObjects(pStudio, pScreen, pEntry->nNode, nMsg);
                        } else if (pEntry->bInitialized != 0 && pEntry->PluginIndex < pStudio->NumPlugins) {
                            UISPluginFncT* pfnHandler = pStudio->Plugins[pEntry->PluginIndex].pFnc;
                            if (pfnHandler != NULL) {
                                pfnHandler((u8*)pScreen->pScrData + *pEntry->pData, nMsg, 0, NULL, 0);
                            }
                        }
                    }
                }
            }
            UISSetColorMultipler(mul.r, mul.g, mul.b, mul.a);
            UISSetColorAdditive(add.r, add.g, add.b, add.a);
            pStudio->pTransformFnc(2, &pNode->pControlInfo->Transform);
        }
    }
}

// Sends event uEvent to node nNode and, first, to the nodes it links to. A node takes events only
// while its info has IsEnabled and CanHandleMessages set, except the studio's own events (n5 -2
// to -5 and -8 to -11). A linked node that answers with 1 gets the handler this node has for it.
// Returns 2 as soon as a handler returns 2.
s32 _ParseMaps(UISInfoT* pStudio, UISScreenT* pScreen, UISStackInfoT* pStack, u32 nNode, u32 uEvent, u32 n5,
               s32 nArgs, s32* pArgs, u8* pbOut) {
    // fake match: uEventLoop, uEventPost and uEventPre all hold uEvent (see below); these copies,
    // this declaration order and the function-level pHandler / nRet / pLinked give EA's registers.
    s32 uEventLoop;
    u32 uEventPost;
    s32 uEventPre;
    UISMapT* pHandler;
    UISControlT* pNode;
    s32 nResult;
    u32 i;
    s32 nRet;
    u8* pLinked;
    u8* pScript;
    u8 bOut;

    nResult = 0;
    if (pScreen->pScrData == NULL || nNode >= pScreen->pScrData->NumControls) return 0;
    uEventPost = uEvent;
    pNode = &pScreen->pScrData->Controls[nNode];
    if ((pNode->pControlInfo->IsEnabled != 0 && pNode->pControlInfo->CanHandleMessages != 0) ||
        n5 - (u32)-10 <= 2 || n5 - (u32)-5 <= 3 || n5 == (u32)-11) {
        // fake match: uEventPre is uEvent: the OR's low word is uEvent | 0 (uEvent shifted up only
        // fills the high word, which is dropped). It is a copy only after constant propagation, so
        // with the loop's two links below the late copy-propagation passes stop at this one: EA's
        // kept `mr r19,r28` for the linked handler's event. The dead high-word OR leaves no code.
        uEventPre = (s32)((u64)(u32)uEvent | ((u64)(u32)uEvent << 32));
        for (i = 0; i < pNode->NumMaps; i++) {
            pHandler = &pNode->Maps[i];
            // fake match: uEventPost again (the same value); the second definition keeps it a
            // variable of its own.
            uEventPost = uEvent;
            // fake match: uEventLoop is uEventPre: i only goes into the dropped high word. Two
            // links of copies-after-constant-propagation, loop-variant so they stay in the loop.
            uEventLoop = (s32)((u64)(u32)uEventPre | ((u64)(u32)i << 32));
            uEventLoop = (s32)((u64)(u32)uEventLoop | ((u64)(u32)i << 32));
            if (pHandler->EventID == 0xFFFF) {
                bOut = 0;
                nResult = _ParseMaps(pStudio, pScreen, pStack, pHandler->nNode, uEvent, n5, nArgs, pArgs,
                                     &bOut);
                // fake match: the (s32) gives EA's signed cmpwi
                if ((s32)bOut == 1) {
                    pLinked = UISFindSubControlEventPC(pNode, (u16)pHandler->nNode, n5);
                    if (pLinked != NULL) {
                        nRet = UISExecuteFnc(pStudio, pScreen, pNode->pControlInfo, pStack, pLinked, nArgs,
                                             pArgs, 0, NULL, 1, uEventLoop, NULL);
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
        UISProcessThreadAction(pStudio, 1);
        pScript = UISFindEventPC(pNode, n5);
        // Events -6 and -7 go only to the node their third word names.
        // port: the event word holds a pointer
        if ((n5 == (u32)-6 || n5 == (u32)-7) && pNode->pControlInfo != (UISControlInfoT*)pArgs[2]) {
            pScript = NULL;
        }
        // fake match: uEvent goes into the high word of a 64-bit OR whose low word is nResult, so
        // nResult is unchanged. The dead OR keeps uEvent live across the calls above at register
        // allocation (its own register, copied from r7 with the other parameters, as in EA) and is
        // deleted after allocation.
        // port: a port leaves this line out.
        nResult = (s32)((u64)(s64)nResult | ((u64)(u32)uEvent << 32));
        if (pScript != NULL) {
            nResult = UISExecuteFnc(pStudio, pScreen, pNode->pControlInfo, pStack, pScript, nArgs, pArgs, 0,
                                    NULL, 1, uEventPost, NULL);
        }
        if (nResult == 2) return nResult;
    }
    *pbOut = pNode->pControlInfo->IsEnabled;
    return nResult;
}

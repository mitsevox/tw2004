// UISScreen.c (our name): the screen side of EA's UI Studio library (frontend/uistudio.h). It walks
// a loaded screen's nodes to draw them with a scale and offset, finds and runs the handlers
// nodes have for an event, formats text for them (a printf of its own) and finds the variable a
// rate function drives. Called by UIStudio.c and by the game's menus. The original was built with
// automatic inlining (-inline auto in configure.py): _ParseTransforms and _ParseHints have their own
// recursion inlined three deep.
// section order: built with -inline auto,deferred, which emits the functions last-first, so they
// are written here from the highest address down.

#include "frontend/uistudio.h"

void _ParseHints(UISInfoT* pInfo, UISScreenT* pScreen, UISStackInfoT* pStackInfo, u32 idxControl, u32 HINT,
                 s32 nParam, s32* pParam);
char* _WriteFloat(char* buf, char* eob, s32 width, s32 precision, f32 fval);

// .bss, reverse address order
UISColorVectorT _MultiplerColorFactor;
UISColorVectorT _AdditiveColorFactor;

// Whether one of pControl's groups links to the node pControlInfo belongs to.
static inline u8 _IsChildOfControl(UISScrDataT* pScrData, UISControlT* pControl,
                                   UISControlInfoT* pControlInfo) {
    UISLayerT* pLayer;
    UISObjT* pObj;
    u32 idxLayer;
    u32 idxObj;

    for (idxLayer = 0; idxLayer < pControl->NumLayers; idxLayer++) {
        pLayer = pControl->Layers[idxLayer];
        for (idxObj = 0; idxObj < pLayer->NumObjs; idxObj++) {
            pObj = &pLayer->Objs[idxObj];
            if (pObj->PluginIndex == 0xFFFF && pScrData->Controls[pObj->nNode].pControlInfo == pControlInfo) {
                return 1;
            }
        }
    }
    return 0;
}

// The info of the first node pControl links to whose IsEnabled is set, or NULL.
static inline UISControlInfoT* _GetFirstEnableControl(UISScrDataT* pScrData, UISControlT* pControl) {
    UISLayerT* pLayer;
    UISObjT* pObj;
    UISControlInfoT* pControlInfo;
    u32 idxLayer;
    u32 idxObj;

    for (idxLayer = 0; idxLayer < pControl->NumLayers; idxLayer++) {
        pLayer = pControl->Layers[idxLayer];
        for (idxObj = 0; idxObj < pLayer->NumObjs; idxObj++) {
            pObj = &pLayer->Objs[idxObj];
            if (pObj->PluginIndex == 0xFFFF) {
                pControlInfo = pScrData->Controls[pObj->nNode].pControlInfo;
                if (pControlInfo->IsEnabled != 0) return pControlInfo;
            }
        }
    }
    return NULL;
}

// Copies sz to buf, padded with spaces to width characters (on the left, or on the right when
// leftadjust), stopping at eob. Returns the end of the copy.
// Register note: the walking copies p and s and their declaration order (after nAbs, before
// nCount) give EA's r22-r25 in both inlined copies.
static inline char* _WriteString(char* buf, char* eob, const char* sz, s32 width, u8 leftadjust) {
    s32 nAbs;
    char* p;
    const char* s;
    s32 nCount;
    s32 nPad;
    s32 len;
    s32 i;

    nAbs = width;
    p = buf;
    s = sz;
    nCount = 0;
    if (width < 0) {
        nAbs = -width;
    }
    len = strlen(s);
    if (!leftadjust) {
        nPad = nAbs - len;
        while (nCount < nPad) {
            nCount++;
            *p++ = ' ';
        }
    }
    while (*s != 0) {
        *p++ = *s++;
        nCount++;
        if (p == eob) break;
    }
    if (leftadjust == 1) {
        for (i = nCount; i < nAbs; i++) {
            *p++ = ' ';
        }
    }
    return p;
}

// The index of a loaded screen, or the number of screens when it is not loaded. Written out, not
// through UIS_FindScreen: returning an inlined call adds a copy of the index (88.6 -> 90.5%).
// fake match: scheduled once, not twice: EA's entry block has `li idx` between the two u16 masks,
// which only the single scheduling pass gives (twice puts the li last). Code is unchanged.
#pragma scheduling once
u16 UISFindScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID) {
    u16 idx;
    UISScreenT* pScreen;

    for (idx = 0; idx < pInfo->NumScreens; idx++) {
        pScreen = &pInfo->Screens[idx];
        if (pScreen->GroupID == GroupID && pScreen->ScreenID == ScreenID) break;
    }
    return idx;
}
#pragma scheduling reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// A node's handler of the kind marked 0x8000 for an event.
u8* UISFindEventPC(UISControlT* pControl, u32 EventID) {
    u32 idxMap;
    for (idxMap = 0; idxMap < pControl->NumMaps; idxMap++) {
        UISMapT* pMap = &pControl->Maps[idxMap];
        if ((pMap->ControlIndex & 0x8000) && pMap->EventID == (u16)EventID) {
            return pMap->pFnc;
        }
    }
    return NULL;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// A node's plain handler (neither kind bit) with the given ID for an event.
// fake match: scheduled once, not twice: EA's entry block has `li idxMap` between the two masks,
// which only the single scheduling pass gives (twice puts the li last). Code is unchanged.
#pragma scheduling once
u8* UISFindSubControlEventPC(UISControlT* pControl, u16 idxControl, u32 EventID) {
    u32 idxMap;
    int nId = idxControl;
    for (idxMap = 0; idxMap < pControl->NumMaps; idxMap++) {
        UISMapT* pMap = &pControl->Maps[idxMap];
        if (!(pMap->ControlIndex & 0xC000) && pMap->EventID == (u16)EventID &&
            (pMap->ControlIndex & 0x2FFF) == nId) {
            return pMap->pFnc;
        }
    }
    return NULL;
}
#pragma scheduling reset
#pragma auto_inline reset

// The node's handler of the kind marked 0x4000 for an event.
u8* _UISFindHintPC(UISControlT* pControl, u32 HintID) {
    u32 idxMap;
    for (idxMap = 0; idxMap < pControl->NumMaps; idxMap++) {
        UISMapT* pMap = &pControl->Maps[idxMap];
        if ((pMap->ControlIndex & 0x4000) && pMap->EventID == (u16)HintID) {
            return pMap->pFnc;
        }
    }
    return NULL;
}

// Pushes a call frame on pStackInfo (the saved word, the extra word, both argument lists, the node's
// info and a 0) and runs pcEvent on it. The frame stays on the stack only when the script
// returns 3 (it paused).
s32 UISExecuteFnc(UISInfoT* pInfo, UISScreenT* pScreen, UISControlInfoT* pControlInfo,
                  UISStackInfoT* pStackInfo, u8* pcEvent, s32 nParam, s32* pParam, u32 nAppend,
                  const s32* pAppend, u8 bUseChannel, s32 Channel, s32* pReturn) {
    // fake match: pStackCopy is pStackInfo through a void* copy, declared first; the local's higher
    // variable number makes the allocator colour it before pStackBase and pReturn (EA's r31).
    UISStackInfoT* pStackCopy = (UISStackInfoT*)(void*)pStackInfo;
    s32* pStackBase;
    u32 idxParam;
    s32 rVal;
    void* pScriptCopy = pcEvent;

    // fake match: pScriptCopy is pcEvent, so this OR leaves pcEvent unchanged. The frontend cannot
    // fold the OR of two variables; it stays as `or r11,r7,r7` (the original's `mr r11,r7`, the
    // same encoding), which the allocator never coalesces, so pcEvent leaves r7 as in EA.
    // port: the pointer goes through a 32-bit integer
    pcEvent = (u8*)((u32)pcEvent | (u32)pScriptCopy);
    pStackBase = pStackCopy->pStack;
    if (pReturn == NULL) {
        *pStackBase = 0;
    } else {
        *pStackBase = *pReturn;
    }
    pStackCopy->pStack++;
    if (bUseChannel) {
        *pStackCopy->pStack = Channel;
        pStackCopy->pStack++;
    }
    for (idxParam = 0; idxParam < nParam; idxParam++) {
        *pStackCopy->pStack = pParam[idxParam];
        pStackCopy->pStack++;
    }
    for (idxParam = 0; idxParam < nAppend; idxParam++) {
        *pStackCopy->pStack = pAppend[idxParam];
        pStackCopy->pStack++;
    }
    // port: the frame keeps the info pointer in a word
    *pStackCopy->pStack = (s32)pControlInfo;
    pStackCopy->pStack++;
    *pStackCopy->pStack = 0;
    pStackCopy->pStack++;
    pStackCopy->pPC = pcEvent;
    rVal = UISStackProcess(pInfo, pStackBase, pStackCopy, pScreen, pControlInfo);
    if (pReturn != NULL) {
        *pReturn = *pStackBase;
    }
    if (rVal != UISPROCESS_DOMODAL) {
        pStackCopy->pStack = pStackBase;
    }
    return rVal;
}

f32* UISGetActionPtrValue(s32 Action, UISControlInfoT* pControlInfo) {
    switch (Action) {
    case UIS_ACTION_ROTATION_X:
        return &pControlInfo->Transform.Rotation.x;
    case UIS_ACTION_ROTATION_Y:
        return &pControlInfo->Transform.Rotation.y;
    case UIS_ACTION_ROTATION_Z:
        return &pControlInfo->Transform.Rotation.z;
    case UIS_ACTION_TRANSLATE_X:
        return &pControlInfo->Transform.Offset.x;
    case UIS_ACTION_TRANSLATE_Y:
        return &pControlInfo->Transform.Offset.y;
    case UIS_ACTION_TRANSLATE_Z:
        return &pControlInfo->Transform.Offset.z;
    case UIS_ACTION_SCALE_X:
        return &pControlInfo->Transform.Scale.x;
    case UIS_ACTION_SCALE_Y:
        return &pControlInfo->Transform.Scale.y;
    case UIS_ACTION_SCALE_Z:
        return &pControlInfo->Transform.Scale.z;
    case UIS_ACTION_PIVOT_X:
        return &pControlInfo->Transform.Pivot.x;
    case UIS_ACTION_PIVOT_Y:
        return &pControlInfo->Transform.Pivot.y;
    case UIS_ACTION_PIVOT_Z:
        return &pControlInfo->Transform.Pivot.z;
    case UIS_ACTION_ADD_ALPHA:
        return &pControlInfo->Transform.AdditiveFactor.a;
    case UIS_ACTION_ADD_RED:
        return &pControlInfo->Transform.AdditiveFactor.r;
    case UIS_ACTION_ADD_GREEN:
        return &pControlInfo->Transform.AdditiveFactor.g;
    case UIS_ACTION_ADD_BLUE:
        return &pControlInfo->Transform.AdditiveFactor.b;
    case UIS_ACTION_MULTIPLY_ALPHA:
        return &pControlInfo->Transform.MultiplerFactor.a;
    case UIS_ACTION_MULTIPLY_RED:
        return &pControlInfo->Transform.MultiplerFactor.r;
    case UIS_ACTION_MULTIPLY_GREEN:
        return &pControlInfo->Transform.MultiplerFactor.g;
    case UIS_ACTION_MULTIPLY_BLUE:
        return &pControlInfo->Transform.MultiplerFactor.b;
    default:
        return &pControlInfo->Transform.MultiplerFactor.a;
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
void UISSetColorMultipler(f32 r, f32 g, f32 b, f32 a) {
    // fake match: the original stores the fourth value second
    _MultiplerColorFactor.r = r;
    _MultiplerColorFactor.a = a;
    _MultiplerColorFactor.g = g;
    _MultiplerColorFactor.b = b;
}
#pragma auto_inline reset

#pragma auto_inline off
// fake match: not pasted into its callers: the file is built with -inline auto,deferred (see
// configure.py), and EA calls this one.
// The values every node is drawn with: _ParseObjects adds a node's Transform.AdditiveFactor to the first and
// multiplies its Transform.MultiplerFactor into the second for the node's children.
void UISSetColorAdditive(f32 r, f32 g, f32 b, f32 a) {
    // fake match: the original stores the fourth value second
    _AdditiveColorFactor.r = r;
    _AdditiveColorFactor.a = a;
    _AdditiveColorFactor.g = g;
    _AdditiveColorFactor.b = b;
}
#pragma auto_inline reset

// Writes fval with precision decimals (6 when negative), padded with spaces to width characters, into
// buf up to eob. Returns the end of what it wrote.
char* _WriteFloat(char* buf, char* eob, s32 width, s32 precision, f32 fval) {
    char revStr[64];
    s32 nDigits;
    s32 sign;
    f32 fRound;
    s32 i;
    f32 fraction;
    f32 nextVal;
    s32 nPad;

    nDigits = 0;
    if (precision < 0) {
        precision = 6;
    }
    sign = fval < 0.0f;
    if (sign) {
        fval = -fval;
    }
    fRound = 0.5f;
    for (i = 0; i < precision; i++) {
        fRound = 0.1f * fRound;
    }
    fval += fRound;
    fraction = fval - (s32)fval;
    do {
        nextVal = fval / 10.0f;
        revStr[nDigits++] = (s32)(fval - (s32)nextVal * 10) + '0';
        fval = nextVal;
    } while (fval >= 1.0f);
    if (sign) {
        revStr[nDigits++] = '-';
    }
    if (width != 0) {
        for (nPad = width - (precision + nDigits + (precision != 0)); nPad > 0; nPad--) {
            revStr[nDigits++] = ' ';
        }
    }
    while (--nDigits >= 0 && buf < eob) {
        *buf++ = revStr[nDigits];
    }
    if (precision != 0 && buf < eob) {
        *buf++ = '.';
        do {
            fraction *= 10.0f;
            // (int) and (s32) (a long) are separate conversions to the compiler: EA stores the
            // digit and the whole part as two words.
            *buf++ = (int)fraction + '0';
            fraction -= (s32)fraction;
        } while (--precision != 0 && buf < eob);
    }
    return buf;
}

// fake match: UISSprintf's 'x' case as an inline. Inlined code gets the frontend's goto
// cleanup that a large function body skips, which places the output loop's preheader at the
// end of UISSprintf as in EA's code. Code is unchanged.
static inline char* _WriteHex(char* buf, char* eob, u32 uval, s32 width, char pad, s32 caseDelta) {
    s32 nDigits;
    s32 nPad;
    char c;
    char revStr[12];

    nDigits = 0;
    do {
        c = (uval & 0xF) + '0';
        if (c > '9') {
            c += caseDelta + 'a' - '9' - 1;
        }
        uval >>= 4;
        revStr[nDigits++] = c;
    } while (uval != 0);
    if (width != 0) {
        for (nPad = width - nDigits; nPad > 0; nPad--) {
            revStr[nDigits++] = pad;
        }
    }
    while (--nDigits >= 0 && buf < eob) {
        *buf++ = revStr[nDigits];
    }
    return buf;
}

// fake match: UISSprintf's 'd' case as an inline, for the same preheader placement as
// _WriteHex. `bUnsigned ^ 1` for !bUnsigned (it is 0 or 1) gives EA's xori, and
// `sign = ival >> 31` inside the && EA's srwi. whose result is the sign kept in r0. The caller
// passes its own u as ival, which keeps EA's `mr r11,r7` before the negate. Code is unchanged.
static inline char* _WriteInt(char* buf, char* eob, u32 ival, s32 width, char pad, s32 bUnsigned) {
    s32 nDigits;
    s32 nPad;
    s32 sign;
    char revStr[20];
    u32 val;

    nDigits = 0;
    sign = 0;
    val = ((bUnsigned ^ 1) && (sign = ival >> 31)) ? -ival : ival;
    do {
        revStr[nDigits++] = val + '0' - val / 10 * 10;
        val /= 10;
    } while (val != 0);
    if (width != 0) {
        for (nPad = width - sign - nDigits; nPad > 0; nPad--) {
            revStr[nDigits++] = pad;
        }
    }
    if (sign) {
        revStr[nDigits++] = '-';
    }
    while (--nDigits >= 0 && buf < eob) {
        *buf++ = revStr[nDigits];
    }
    return buf;
}

// fake match: UISSprintf's start pointer read through an inline whose parameter is changed (the
// dead p++), so the parameter is a frontend variable numbered before every other inline's; the
// start then ranks lowest of the saved registers (EA's r26). Returns p unchanged.
static inline char* fn_8016B844_Get(char* p) {
    return p++;
}

// Formats fmt with pParam into buf (destSize bytes). Returns the length written, or -1.
// EA bug: '-' is never cleared, so every conversion after one with '-' is left-justified too.
s32 UISSprintf(char* buf, s32 destSize, const char* fmt, s32 nParam, const UISParamT* pParam) {
    // Register note: this declaration order gives EA's c r7, pad r8 and caseDelta r0.
    char* eob;
    s32 iParam;
    s32 caseDelta;
    u32 u;
    s32 width;
    s32 precision;
    s32 bUpper;
    s32 bUnsigned;
    u8 leftadjust;
    char c;
    char pad;
    char* buf0;

    buf0 = fn_8016B844_Get(buf);
    eob = buf + destSize;
    leftadjust = 0;
    if (buf == NULL || destSize == 0 || fmt == NULL) return -1;
    iParam = 0;
    while ((c = *fmt++) != 0 && buf < eob) {
        if (c == '%') {
            bUnsigned = 0;
            width = 0;
            precision = -1;
            c = *fmt++;
            pad = ' ';
            if (c == '-') {
                leftadjust = 1;
                c = *fmt++;
            }
            if (c == '0') {
                pad = c;
                c = *fmt++;
            }
            while (c >= '0' && c <= '9') {
                width = width * 10 + c - '0';
                c = *fmt++;
            }
            if (c == '.') {
                c = *fmt++;
            }
            if (c >= '0' && c <= '9') {
                precision = c - '0';
                c = *fmt++;
            }
            while (c >= '0' && c <= '9') {
                precision = precision * 10 + c - '0';
                c = *fmt++;
            }
            bUpper = 0;
            if (c >= 'A' && c <= 'Z') {
                bUpper = 1;
            }
            caseDelta = bUpper ? 'A' - 'a' : 0;
            switch (c) {
            case 'c':
                *buf++ = pParam[iParam++].iValue;
                continue;
            case 's': {
                UISStringT* pText = pParam[iParam++].strAddr;
                if (pText->ptr != NULL) {
                    buf = _WriteString(buf, eob, pText->ptr, width, leftadjust);
                } else {
                    buf = _WriteString(buf, eob, "(null)", width, leftadjust);
                }
                continue;
            }
            case 'u':
                bUnsigned = 1;
            case 'd':
            case 'i':
                u = pParam[iParam++].u;
                buf = _WriteInt(buf, eob, u, width, pad, bUnsigned);
                continue;
            case 'f':
                buf = _WriteFloat(buf, eob, width, precision, pParam[iParam++].fValue);
                continue;
            case 'X':
            case 'p':
            case 'x':
                u = pParam[iParam++].u;
                buf = _WriteHex(buf, eob, u, width, pad, caseDelta);
                continue;
            }
        }
        if (c != 0) {
            *buf++ = c;
        }
    }
    *buf++ = '\0';
    return buf - (buf0 + 1);
}

// Formats pFormatStr's text with pParam into pString's buffer.
void UISStringFormat(u32 pScrData, UISStringT* pString, UISStringT* pFormatStr, s32 nParam,
                     const UISParamT* pParam) {
    if (pFormatStr != NULL && pString != NULL) {
        UISSprintf(pString->ptr, pString->length, pFormatStr->ptr, nParam, pParam);
    }
}

// Finds the node that links to pControlInfo's node and returns the info of the first node it links to
// whose IsEnabled is set.
UISControlInfoT* UISFindSiblingEnableControl(UISScreenT* pScreen, UISControlInfoT* pControlInfo) {
    UISScrDataT* pScrData;
    u32 idxControl;

    pScrData = pScreen->pScrData;
    for (idxControl = 0; idxControl < pScrData->NumControls; idxControl++) {
        UISControlT* pControl = &pScrData->Controls[idxControl];
        if (_IsChildOfControl(pScrData, pControl, pControlInfo)) {
            return _GetFirstEnableControl(pScrData, pControl);
        }
    }
    return NULL;
}

// Moves a loaded screen iDir places up or down the screen table, one swap at a time, keeping
// the current screen, the rate functions and the ModalStack records on the screens they named.
void UISMoveScreenDrawPosition(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, s32 iDir) {
    s32 nScreens;
    s32 nLimit;
    s32 iScreen;
    s32 iMovements;
    s32 idxOld;
    s32 idxNew;
    u32 idxRate;
    u32 idxModals;
    s32 increment;
    UISScreenT TempScreen;

    iScreen = UISFindScreen(pInfo, GroupID, ScreenID);
    nScreens = pInfo->NumScreens;
    if (iScreen < nScreens) {
        if (iDir >= 0) {
            iMovements = iDir;
            increment = 1;
        } else {
            iMovements = -iDir;
            increment = -1;
        }
        idxNew = iScreen;
        // fake match: nLimit is always nScreens; its second assignment in the loop keeps EA's copy of
        // the count (`mr r28,r8`) from being propagated away.
        nLimit = nScreens;
        while (iMovements-- != 0) {
            idxOld = idxNew;
            idxNew += increment;
            if (idxNew >= nLimit || idxNew < 0) break;
            nLimit = nScreens;
            if (pInfo->ActiveScreenIdx == idxNew) {
                pInfo->ActiveScreenIdx = idxOld;
            } else if (pInfo->ActiveScreenIdx == idxOld) {
                pInfo->ActiveScreenIdx = idxNew;
            }
            for (idxRate = 0; idxRate < pInfo->NumRateFncs; idxRate++) {
                if (pInfo->RateFncs[idxRate].pScreen == &pInfo->Screens[idxNew]) {
                    pInfo->RateFncs[idxRate].pScreen = &pInfo->Screens[idxOld];
                } else if (pInfo->RateFncs[idxRate].pScreen == &pInfo->Screens[idxOld]) {
                    pInfo->RateFncs[idxRate].pScreen = &pInfo->Screens[idxNew];
                }
            }
            for (idxModals = 0; idxModals < pInfo->NumModals; idxModals++) {
                if (pInfo->ModalStack[idxModals].pScreen == &pInfo->Screens[idxNew]) {
                    pInfo->ModalStack[idxModals].pScreen = &pInfo->Screens[idxOld];
                } else if (pInfo->ModalStack[idxModals].pScreen == &pInfo->Screens[idxOld]) {
                    pInfo->ModalStack[idxModals].pScreen = &pInfo->Screens[idxNew];
                }
            }
            memcpy(&TempScreen, &pInfo->Screens[idxNew], sizeof(UISScreenT));
            memcpy(&pInfo->Screens[idxNew], &pInfo->Screens[idxOld], sizeof(UISScreenT));
            memcpy(&pInfo->Screens[idxOld], &TempScreen, sizeof(UISScreenT));
        }
    }
}

// Runs the 0x4000 handlers for an event of node idxControl and of every node it links to, the linked
// nodes first.
void _ParseHints(UISInfoT* pInfo, UISScreenT* pScreen, UISStackInfoT* pStackInfo, u32 idxControl, u32 HINT,
                 s32 nParam, s32* pParam) {
    UISControlT* pControl;
    u32 idxMap;
    u8* pcEvent;

    // fake match: the byte offset as a 64-bit product (low word = idxControl * 20, the same address as
    // Controls[idxControl]); its dead high word (li 20; mulhw) goes first in the pre-allocation schedule,
    // which moves the pInfo copy after the pScrData load as in the original, and is deleted later.
    // port: the offset is truncated to 32 bits.
    pControl = (UISControlT*)((u8*)pScreen->pScrData->Controls + (s32)idxControl * (s64)sizeof(UISControlT));
    for (idxMap = 0; idxMap < pControl->NumMaps; idxMap++) {
        UISMapT* pMap = &pControl->Maps[idxMap];
        if (pMap->EventID == 0xFFFF) {
            _ParseHints(pInfo, pScreen, pStackInfo, pMap->nNode, HINT, nParam, pParam);
        }
    }
    // The node's script for the event (_UISFindHintPC's search). The original has _UISFindHintPC
    // inlined here and called only at the deepest inlined level; a static inline copy of it matches
    // better (93%) but adds a function the original does not have, so the unit could not link.
    pcEvent = _UISFindHintPC(pControl, HINT);
    if (pcEvent != NULL) {
        UISExecuteFnc(pInfo, pScreen, pControl->pControlInfo, pStackInfo, pcEvent, nParam, pParam, 0, NULL, 0,
                      0, NULL);
    }
}

void UISDoHint(UISInfoT* pInfo, u32 Hint, s32 nParms, s32* pParam) {
    u32 idxScreen;
    u32 numScreens = pInfo->NumScreens;
    for (idxScreen = 0; idxScreen < numScreens; idxScreen++) {
        UISScreenT* pScreen = &pInfo->Screens[idxScreen];
        pInfo->CriticalRegions |= 2;
        _ParseHints(pInfo, pScreen, &pInfo->EventStack, 0, Hint, nParms, pParam);
        pInfo->CriticalRegions &= ~2;
    }
}

// Send an event to every screen. While the studio is busy (flag 2: sending an event; 4: running
// its rate functions), it is queued on the event stack instead.
void UISProcessHint(UISInfoT* pInfo, u32 Hint, s32 nParms, s32* pParam) {
    UISThreadGroupInfoT ThreadInfo;
    if ((pInfo->CriticalRegions & 2) || (pInfo->CriticalRegions & 4)) {
        ThreadInfo.GenericInfo.Data[0] = Hint;
        UISAddThreadAction(-1, -1, pInfo, UISThreadAction_HINT, &ThreadInfo, nParms, pParam);
    } else {
        UISDoHint(pInfo, Hint, nParms, pParam);
    }
}

// fake match: an identity read; it gives EA's register order.
static inline u8 fn_8016AEEC_Read(u8 b) { return b; }

// Runs the screen file's start entries that have not run yet, then every handler under node
// idxControl; a handler run with message -1 is marked as run.
void _ParseInitialize(UISInfoT* pInfo, UISScreenT* pScreen, u32 idxControl, s32 FncID) {
    // fake match: the second loop's pObj is declared here, ahead of pControl and idxLayer, and shadowed by
    // the first loop's own pObj; this order gives the original's loop registers.
    u8 bInit;
    UISObjT* pObj;
    UISControlT* pControl;
    u32 idxLayer;

    if (pScreen->pScrData != NULL) {
        pControl = &pScreen->pScrData->Controls[idxControl];
        bInit = fn_8016AEEC_Read(FncID == -1);
        // fake match: idxControl (dead after pControl) is the counter of both entry loops, which gives the
        // original's loop registers.
        for (idxControl = 0; idxControl < pScreen->pScrData->NumStaticObjects; idxControl++) {
            UISObjT* pObj = &pScreen->pScrData->StaticObjects[idxControl];
            if (pObj->bInitialized == 0) {
                if (pObj->PluginIndex < pInfo->NumPlugins) {
                    UISPluginFncT* pfnHandler = pInfo->Plugins[pObj->PluginIndex].pFnc;
                    if (pfnHandler != NULL) {
                        pfnHandler(pObj->pData != NULL ? (u8*)pScreen->pScrData + *pObj->pData
                                                               : NULL,
                                   FncID, 0, NULL, 0);
                    }
                }
                pObj->bInitialized = 1;
            }
        }
        for (idxLayer = 0; idxLayer < pControl->NumLayers; idxLayer++) {
            // fake match: b is bInit, and the stored `b | bInit` is bInit. The frontend cannot fold the
            // OR of two variables, so after the copy is propagated it stays as `or r30,r27,r27` (the
            // original's `mr r30,r27`, the same encoding), which the allocator never coalesces.
            int b = bInit;
            UISLayerT* pLayer = pControl->Layers[idxLayer];
            for (idxControl = 0; idxControl < pLayer->NumObjs; idxControl++) {
                pObj = &pLayer->Objs[idxControl];
                if (pObj->PluginIndex == 0xFFFF) {
                    _ParseInitialize(pInfo, pScreen, pObj->nNode, FncID);
                } else if (pObj->PluginIndex < pInfo->NumPlugins) {
                    UISPluginFncT* pfnHandler = pInfo->Plugins[pObj->PluginIndex].pFnc;
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pScrData + *pObj->pData, FncID, 0, NULL, 0);
                        pObj->bInitialized = b | bInit;
                    }
                }
            }
        }
    }
}

// Looks under a node (contextType 8) or a group (contextType 7) for the one pTarget belongs to and records it
// as pTarget's owner. Returns -1 when it is not found.
s32 _DetermineVisibility(UISScreenT* pScreen, UISControlInfoT* pTarget, s32 contextType, void* pContext) {
    // fake match: this declaration order (with the copies below) gives EA's registers.
    UISLayerT* pGroup;
    u32 uIdxItems;
    UISControlT* pControl;
    // fake match: one count for both loops (NumLayers, then NumObjs): with it NumObjs takes r28
    // and case 7's counter r27 (EA's li r27,0 / lwz r28 / mr r29,r27); with two, they swap.
    u32 uNumItems;
    UISObjT* pObj;
    UISControlT* pLoop8;
    UISControlT* pPrev8;
    UISLayerT* pLoop7;
    s32 iResult;

    if (pTarget == NULL || pContext == NULL || pScreen == NULL) return -1;
    switch (contextType) {
    case 8:
        pControl = pContext;
        if (pControl->pControlInfo == pTarget) {
            pTarget->IsVisible = pControl;
            return pControl->pControlInfo->IsVisible != NULL;
        }
        if (pControl->pControlInfo != NULL && pControl->pControlInfo->IsVisible != NULL) {
            uNumItems = pControl->NumLayers;
            // fake match: pLoop8 and pPrev8 always hold pControl; the copies carried round the loop
            // keep pControl apart from pContext (EA's mr r27,r6 with the first read through r6), and taking
            // pPrev8 before the copy keeps EA's argument order (li r5,7 before the lwzx).
            pLoop8 = pControl;
            for (uIdxItems = 0; uIdxItems < uNumItems; uIdxItems++) {
                pPrev8 = pLoop8;
                pLoop8 = pControl;
                iResult = _DetermineVisibility(pScreen, pTarget, 7, pPrev8->Layers[uIdxItems]);
                // fake match: iResult goes through a 64-bit shift up and back down (the value is
                // unchanged). The shifts become a chain of word copies; each copy-propagation pass
                // removes one link, so EA's copy of the call result survives (mr r0,r3; cmpwi r0,-1).
                // port: relies on the conversion to s64 wrapping and on >> of a negative s64 being
                // arithmetic.
                iResult = (s32)((s64)((u64)(u32)iResult << 32) >> 32);
                if (iResult != -1) return iResult;
            }
        }
        break;
    case 7:
        pGroup = pContext;
        if (pGroup->pLayerInfo == pTarget) {
            pTarget->IsVisible = pGroup;
            return pGroup->pLayerInfo->IsVisible != NULL;
        }
        if (pGroup->pLayerInfo != NULL && pGroup->pLayerInfo->IsVisible != NULL) {
            uNumItems = pGroup->NumObjs;
            // fake match: pLoop7 always holds pGroup; the copy carried round the loop keeps pGroup
            // apart from pContext (EA's mr r26,r6 with the first read through r6).
            pLoop7 = pGroup;
            for (uIdxItems = 0; uIdxItems < uNumItems; uIdxItems++) {
                pObj = &pLoop7->Objs[uIdxItems];
                pLoop7 = pGroup;
                if (pObj->PluginIndex == 0xFFFF) {
                    iResult = _DetermineVisibility(pScreen, pTarget, 8,
                                                   &pScreen->pScrData->Controls[pObj->nNode]);
                    // fake match: the same 64-bit shift as in case 8, then a 64-bit round trip (both
                    // leave the value unchanged); this block needs one copy link more to keep EA's
                    // copy of the call result (mr r0,r3; cmpwi r0,-1).
                    // port: as in case 8.
                    iResult = (s32)((s64)((u64)(u32)iResult << 32) >> 32);
                    iResult = (s32)(u64)(u32)iResult;
                    if (iResult != -1) return iResult;
                }
            }
        }
        break;
    }
    return -1;
}

// Runs every handler under a node (uChangeType 8) or a group (uChangeType 7) with message -4 and a
// pointer to uNewVisibility. Without bFirstPass, nodes and groups whose info has no owner are skipped.
void _ParseVisibility(UISInfoT* pInfo, UISScreenT* pScreen, s32 uNewVisibility, s32 uChangeType,
                      void* pChange, u8 bFirstPass) {
    // fake match: this declaration order (with the copies below) gives EA's registers.
    UISLayerT* pGroup;
    UISControlT* pControl;
    u32 uIdxItems;
    // fake match: one count for both loops (NumLayers, then NumObjs), as in _DetermineVisibility: with it
    // case 7's counter and count take EA's registers (li r29,0 / lwz r30 / mr r31,r29).
    u32 uNumItems;
    UISObjT* pObj;
    UISControlT* pPrev8;
    UISControlT* pLoop8;
    UISLayerT* pLoop7;

    if (pInfo == NULL) return;
    if (pScreen == NULL || pChange == NULL) return;
    switch (uChangeType) {
    case 8:
        pControl = pChange;
        if (pControl->pControlInfo != NULL && (bFirstPass || pControl->pControlInfo->IsVisible != NULL)) {
            uNumItems = pControl->NumLayers;
            // fake match: pLoop8 and pPrev8 always hold pControl; the copies carried round the loop
            // keep pControl apart from pChange (EA's mr r29,r7 with the first read through r7), and taking
            // pPrev8 before the copy keeps EA's argument order (the lwzx before li r6,7).
            pLoop8 = pControl;
            for (uIdxItems = 0; uIdxItems < uNumItems; uIdxItems++) {
                pPrev8 = pLoop8;
                pLoop8 = pControl;
                _ParseVisibility(pInfo, pScreen, uNewVisibility, 7, pPrev8->Layers[uIdxItems], 0);
            }
        }
        break;
    case 7:
        pGroup = pChange;
        if (pGroup->pLayerInfo != NULL && (bFirstPass || pGroup->pLayerInfo->IsVisible != NULL)) {
            uNumItems = pGroup->NumObjs;
            // fake match: pLoop7 always holds pGroup; the copy carried round the loop keeps pGroup
            // apart from pChange (EA's mr r28,r7 with the first read through r7).
            pLoop7 = pGroup;
            for (uIdxItems = 0; uIdxItems < uNumItems; uIdxItems++) {
                pObj = &pLoop7->Objs[uIdxItems];
                pLoop7 = pGroup;
                if (pObj->PluginIndex == 0xFFFF) {
                    _ParseVisibility(pInfo, pScreen, uNewVisibility, 8,
                                     &pScreen->pScrData->Controls[pObj->nNode], 0);
                } else if (pObj->PluginIndex < pInfo->NumPlugins) {
                    UISPluginFncT* pfnHandler = pInfo->Plugins[pObj->PluginIndex].pFnc;
                    if (pfnHandler != NULL) {
                        pfnHandler((u8*)pScreen->pScrData + *pObj->pData, -4, 1, &uNewVisibility, 0);
                    }
                }
            }
        }
        break;
    }
}

// Hands node idxControl and every node it links to to the transform callback with operation action.
// Operations 0 and 3 also reach groups whose info has no owner.
void _ParseTransforms(UISInfoT* pInfo, int action, UISScreenT* pScreen, u32 idxControl) {
    UISControlT* pControl;
    u32 idxLayer;

    if (pScreen->pScrData != NULL) {
        pControl = &pScreen->pScrData->Controls[idxControl];
        pInfo->pTransformFnc(action, &pControl->pControlInfo->Transform);
        for (idxLayer = 0; idxLayer < pControl->NumLayers; idxLayer++) {
            UISLayerT* pLayer = pControl->Layers[idxLayer];
            if (pLayer->pLayerInfo->IsVisible != NULL || action == UISTransformInit
                || action == UISTransformShutdown) {
                u32 idxObj;
                for (idxObj = 0; idxObj < pLayer->NumObjs; idxObj++) {
                    UISObjT* pObj = &pLayer->Objs[idxObj];
                    if (pObj->PluginIndex == 0xFFFF) {
                        _ParseTransforms(pInfo, action, pScreen, pObj->nNode);
                    }
                }
            }
        }
    }
}

// Draws node idxControl and the nodes it links to: runs the screen file's start entries, then, if the
// node is shown, draws its children with its own values folded into the studio's.
void _ParseObjects(UISInfoT* pInfo, UISScreenT* pScreen, u32 idxControl, s32 FncID) {
    UISControlT* pControl;
    u32 idxLayer;
    UISColorVectorT oldMultiplerVector;
    UISColorVectorT oldAdditiveVector;
    UISControlInfoT* pControlInfo;

    // fake match: pScreen through a 64-bit round trip (the same pointer); the conversion's dead
    // high word (srawi) is deleted by the register allocator, and it moves the pScreen copy after
    // the pScrData load as in the original. port: truncates the pointer to 32 bits.
    if (((UISScreenT*)(s64)(s32)pScreen)->pScrData != NULL) {
        pControl = &pScreen->pScrData->Controls[idxControl];
        // fake match: idxControl (dead after pControl) is the counter of both entry loops, which gives the
        // original's loop registers.
        for (idxControl = 0; idxControl < pScreen->pScrData->NumStaticObjects; idxControl++) {
            UISObjT* pObj = &pScreen->pScrData->StaticObjects[idxControl];
            if (pObj->PluginIndex < pInfo->NumPlugins) {
                UISPluginFncT* pfnHandler = pInfo->Plugins[pObj->PluginIndex].pFnc;
                if (pfnHandler != NULL) {
                    pfnHandler(pObj->pData != NULL ? (u8*)pScreen->pScrData + *pObj->pData
                                                           : NULL,
                               FncID, 0, NULL, 0);
                }
            }
        }
        if (pControl->pControlInfo->IsVisible != NULL) {
            oldMultiplerVector = *UISGetColorMultipler();
            // fake match: the same 64-bit round trip on the pointer (the same address); its dead
            // high word gives the original's order for the copy's loads among the products.
            // port: truncates the pointer to 32 bits.
            oldAdditiveVector = *(UISColorVectorT*)(s64)(s32)UISGetColorAdditive();
            pControlInfo = pControl->pControlInfo;
            UISSetColorMultipler(oldMultiplerVector.r * pControlInfo->Transform.MultiplerFactor.r,
                                 oldMultiplerVector.g * pControlInfo->Transform.MultiplerFactor.g,
                                 oldMultiplerVector.b * pControlInfo->Transform.MultiplerFactor.b,
                                 oldMultiplerVector.a * pControlInfo->Transform.MultiplerFactor.a);
            UISSetColorAdditive(oldAdditiveVector.r + pControlInfo->Transform.AdditiveFactor.r,
                                oldAdditiveVector.g + pControlInfo->Transform.AdditiveFactor.g,
                                oldAdditiveVector.b + pControlInfo->Transform.AdditiveFactor.b,
                                oldAdditiveVector.a + pControlInfo->Transform.AdditiveFactor.a);
            pInfo->pTransformFnc(UISTransformPush, &pControl->pControlInfo->Transform);
            for (idxLayer = 0; idxLayer < pControl->NumLayers; idxLayer++) {
                UISLayerT* pLayer = pControl->Layers[idxLayer];
                if (pLayer->pLayerInfo->IsVisible != NULL) {
                    for (idxControl = 0; idxControl < pLayer->NumObjs; idxControl++) {
                        UISObjT* pObj = &pLayer->Objs[idxControl];
                        if (pObj->PluginIndex == 0xFFFF) {
                            _ParseObjects(pInfo, pScreen, pObj->nNode, FncID);
                        } else if (pObj->bInitialized != 0 && pObj->PluginIndex < pInfo->NumPlugins) {
                            UISPluginFncT* pfnHandler = pInfo->Plugins[pObj->PluginIndex].pFnc;
                            if (pfnHandler != NULL) {
                                pfnHandler((u8*)pScreen->pScrData + *pObj->pData, FncID, 0, NULL, 0);
                            }
                        }
                    }
                }
            }
            UISSetColorMultipler(oldMultiplerVector.r, oldMultiplerVector.g, oldMultiplerVector.b,
                                 oldMultiplerVector.a);
            UISSetColorAdditive(oldAdditiveVector.r, oldAdditiveVector.g, oldAdditiveVector.b,
                                oldAdditiveVector.a);
            pInfo->pTransformFnc(UISTransformPop, &pControl->pControlInfo->Transform);
        }
    }
}

// Sends event Channel to node idxControl and, first, to the nodes it links to. A node takes events only
// while its info has IsEnabled and CanHandleMessages set, except the studio's own events (EventID -2
// to -5 and -8 to -11). A linked node that answers with 1 gets the handler this node has for it.
// Returns 2 as soon as a handler returns 2.
s32 _ParseMaps(UISInfoT* pInfo, UISScreenT* pScreen, UISStackInfoT* pStackInfo, u32 idxControl, u32 Channel,
               u32 EventID, s32 nParam, s32* pParam, u8* bIsControlActive) {
    // fake match: uEventLoop, uEventPost and uEventPre all hold Channel (see below); these copies,
    // this declaration order and the function-level pMap / nRet / pLinked give EA's registers.
    s32 uEventLoop;
    u32 uEventPost;
    s32 uEventPre;
    UISMapT* pMap;
    UISControlT* pControl;
    s32 rVal;
    u32 idxMap;
    s32 nRet;
    u8* pLinked;
    u8* pcEvent;
    u8 bLocalProcess;

    rVal = 0;
    if (pScreen->pScrData == NULL || idxControl >= pScreen->pScrData->NumControls) return 0;
    uEventPost = Channel;
    pControl = &pScreen->pScrData->Controls[idxControl];
    if ((pControl->pControlInfo->IsEnabled != 0 && pControl->pControlInfo->CanHandleMessages != 0) ||
        EventID - (u32)-10 <= 2 || EventID - (u32)-5 <= 3 || EventID == (u32)-11) {
        // fake match: uEventPre is Channel: the OR's low word is Channel | 0 (Channel shifted up only
        // fills the high word, which is dropped). It is a copy only after constant propagation, so
        // with the loop's two links below the late copy-propagation passes stop at this one: EA's
        // kept `mr r19,r28` for the linked handler's event. The dead high-word OR leaves no code.
        uEventPre = (s32)((u64)(u32)Channel | ((u64)(u32)Channel << 32));
        for (idxMap = 0; idxMap < pControl->NumMaps; idxMap++) {
            pMap = &pControl->Maps[idxMap];
            // fake match: uEventPost again (the same value); the second definition keeps it a
            // variable of its own.
            uEventPost = Channel;
            // fake match: uEventLoop is uEventPre: idxMap only goes into the dropped high word. Two
            // links of copies-after-constant-propagation, loop-variant so they stay in the loop.
            uEventLoop = (s32)((u64)(u32)uEventPre | ((u64)(u32)idxMap << 32));
            uEventLoop = (s32)((u64)(u32)uEventLoop | ((u64)(u32)idxMap << 32));
            if (pMap->EventID == 0xFFFF) {
                bLocalProcess = 0;
                rVal = _ParseMaps(pInfo, pScreen, pStackInfo, pMap->nNode, Channel, EventID, nParam, pParam,
                                  &bLocalProcess);
                // fake match: the (s32) gives EA's signed cmpwi
                if ((s32)bLocalProcess == 1) {
                    pLinked = UISFindSubControlEventPC(pControl, (u16)pMap->nNode, EventID);
                    if (pLinked != NULL) {
                        nRet = UISExecuteFnc(pInfo, pScreen, pControl->pControlInfo, pStackInfo, pLinked,
                                             nParam, pParam, 0, NULL, 1, uEventLoop, NULL);
                        // fake match: nRet through a 64-bit shift up and back down (unchanged), then
                        // a dropped identity conversion: the copy chain keeps EA's copy of the call
                        // result (mr r0,r3; cmpwi r0,2).
                        // port: relies on the conversion to s64 wrapping and on >> of a negative s64
                        // being arithmetic.
                        nRet = (s32)((s64)((u64)(u32)nRet << 32) >> 32);
                        nRet = (u32)(s32)nRet;
                        if (nRet == UISPROCESS_HARDABORT) return nRet;
                    }
                }
            }
        }
        UISProcessThreadAction(pInfo, 1);
        pcEvent = UISFindEventPC(pControl, EventID);
        // Events -6 and -7 go only to the node their third word names.
        // port: the event word holds a pointer
        if ((EventID == (u32)-6 || EventID == (u32)-7)
            && pControl->pControlInfo != (UISControlInfoT*)pParam[2]) {
            pcEvent = NULL;
        }
        // fake match: Channel goes into the high word of a 64-bit OR whose low word is rVal, so
        // rVal is unchanged. The dead OR keeps Channel live across the calls above at register
        // allocation (its own register, copied from r7 with the other parameters, as in EA) and is
        // deleted after allocation.
        // port: a port leaves this line out.
        rVal = (s32)((u64)(s64)rVal | ((u64)(u32)Channel << 32));
        if (pcEvent != NULL) {
            rVal = UISExecuteFnc(pInfo, pScreen, pControl->pControlInfo, pStackInfo, pcEvent, nParam, pParam,
                                 0, NULL, 1, uEventPost, NULL);
        }
        if (rVal == UISPROCESS_HARDABORT) return rVal;
    }
    *bIsControlActive = pControl->pControlInfo->IsEnabled;
    return rVal;
}

// UISApi.c (our name): the calls the game makes into EA's UI Studio library (the menu screens'
// runtime): setting the studio up in one block of memory, handing it the game's callbacks,
// running it every frame, and queueing screen changes. A separate file from UIStudio.c: it has
// its own copies of the 0.0f and 1.0f constants.

#include "frontend/uistudio.h"

// Runs the rate functions for uMs milliseconds. Each running one steps once per MSRate ms: its
// step script (if any) gives the step's scale, the variable moves by fStepValue times it, and once it
// reaches fEndValue the function finishes and its done script (or the first node's event -14
// handler) runs.
void _ParseRateFncs(UISInfoT* pStudio, u32 uMs) {
    // fake match: this declaration order (with uMsCopy and the pfVar identity below) gives EA's
    // registers: n, pArg1, pArg2 and uMsCopy leave the allocator's graph before the others.
    u32 uMsCopy;
    u32 n;
    s32* pArg1;
    s32* pArg2;
    UISRateFncT* pRate;
    u32 i;
    UISScreenT* pScreen;
    UISStackInfoT* pStack;
    s32 nLeft;
    s32 nArgs;
    u8* pScript;
    f32 fScale;
    f32* pfVar;
    f32 fValue;
    UISParamT wScale;
    s32 aArgs[3];

    // fake match: uMsCopy is uMs: each OR's low word is the value | 0 (the value shifted up only
    // fills the high word, which is dropped). The three become copies only after constant
    // propagation, so one link reaches register allocation, which merges uMs into uMsCopy; the
    // loop's value then has uMsCopy's higher number (EA's r22, removed before pStudio).
    // port: a port writes uMsCopy = uMs.
    uMsCopy = (u32)((u64)uMs | ((u64)uMs << 32));
    uMsCopy = (u32)((u64)uMsCopy | ((u64)uMsCopy << 32));
    uMsCopy = (u32)((u64)uMsCopy | ((u64)uMsCopy << 32));
    UISRemoveUnNessaryRateFncs(pStudio);
    pStack = &pStudio->RateStack;
    pStudio->CriticalRegions |= 4;
    n = pStudio->NumRateFncs;
    for (i = 0; i < n; i++) {
        pRate = &pStudio->RateFncs[i];
        if (pRate->State != 2) continue;
        pScreen = pRate->pScreen;
        if (pRate->MSRate == 0) continue;
        // fake match: &aArgs[1] / &aArgs[2] set on both arms of a test that is always true here (the
        // branch reuses the continue's compare and disappears); the two definitions keep the stores'
        // addresses in registers hoisted out of the loops (EA's addi r24,r1,0x20 / addi r23,r1,0x24).
        if (pRate->MSRate) {
            pArg1 = &aArgs[1];
            pArg2 = &aArgs[2];
        } else {
            pArg1 = &aArgs[1];
            pArg2 = &aArgs[2];
        }
        pRate->MSCount += uMsCopy;
        nLeft = pRate->MSCount - pRate->MSLastCount;
        while (nLeft >= (s32)pRate->MSRate) {
            nLeft -= pRate->MSRate;
            pRate->MSLastCount += pRate->MSRate;
            if (pRate->pFnc != NULL) {
                if (pRate->State != 2) break;
                wScale.fValue = 1.0f;
                aArgs[0] = pRate->RateFncID;
                *pArg1 = pRate->MSLastCount;
                if (pRate->AnimationData.iType == 0) {
                    nArgs = 3;
                    *pArg2 = pRate->MSRate;
                } else {
                    nArgs = 2;
                }
                UISExecuteFnc(pStudio, pScreen, pRate->pControlInfo, pStack, pRate->pFnc, nArgs, aArgs, 0,
                              NULL, 0, -1, &wScale.iValue);
                fScale = wScale.fValue;
            } else {
                fScale = 1.0f;
            }
            if (pRate->AnimationData.iType != 0) {
                pfVar = UISGetActionPtrValue(pRate->AnimationData.iType,
                                             pRate->AnimationData.pSubControlInfo);
                // fake match: pfVar goes through a 64-bit shift up and back down (the value is
                // unchanged). The copy of the call's result it leaves is merged at register
                // allocation and stays a neighbour of every variable live here: one more for each
                // of them, which gives EA's allocation order.
                // port: relies on the conversion to s64 wrapping and on >> of a negative s64 being
                // arithmetic; truncates the pointer to 32 bits. A port leaves this line out.
                pfVar = (f32*)(u32)((s64)((u64)(u32)pfVar << 32) >> 32);
                // fake match: fStepValue read into fValue before the scale test (EA loads it there)
                fValue = pRate->AnimationData.fStepValue;
                if (fScale < 0.0f) {
                    fScale = 1.0f;
                }
                fValue = fValue * fScale + *pfVar;
                if ((pRate->AnimationData.fStepValue > 0.0f && fValue < pRate->AnimationData.fEndValue)
                    || (pRate->AnimationData.fStepValue < 0.0f && fValue > pRate->AnimationData.fEndValue)) {
                    *pfVar = fValue;
                } else {
                    *pfVar = pRate->AnimationData.fEndValue;
                    pRate->State = 1;
                    if (pRate->AnimationData.pEndFnc != NULL) {
                        aArgs[0] = pRate->RateFncID;
                        UISExecuteFnc(pStudio, pScreen, pRate->pControlInfo, pStack,
                                      pRate->AnimationData.pEndFnc, 1, aArgs, 0, NULL, 0, -1, NULL);
                    } else {
                        pScript = UISFindEventPC(pScreen->pScrData->Controls, -14);
                        if (pScript != NULL) {
                            aArgs[0] = pRate->RateFncID;
                            UISExecuteFnc(pStudio, pScreen, pRate->pControlInfo, pStack, pScript, 1, aArgs, 0,
                                          NULL, 0, -1, NULL);
                        }
                    }
                }
            }
        }
    }
    pStudio->CriticalRegions &= ~4;
    UISRemoveUnNessaryRateFncs(pStudio);
}

// Turns a file offset stored in a pointer field into the pointer.
// port: the UI file keeps 32-bit offsets in its pointer fields.
static inline void* UISFile_Fix(UISScrDataT* pFile, void* p) {
    return (void*)((int)pFile + (int)p);
}

// Fixes up a UI file the first time it is seen: every offset in it becomes a pointer, and each
// link word names its Strings entry by pointer (0 when out of range). Returns 1, or -1 when the
// file was already fixed up (its node table then lies after its start).
// fake match: the file's address is held in integer locals (nBase1 for most fixes, nBase2 for the
// entries', nBase3 for the handlers'; EA's own locals, from Madden 2003's STABS, have none), and
// the node loop steps a byte offset nOff alongside i; with nOff and nBase1 declared first, the
// allocator gives the node-level values EA's registers. nBase2 is an OR of the address with itself
// (x | x == x) and nBase3 the address through a 64-bit round trip (the same low word), both set at
// the top of the node body: the frontend cannot fold them, and they give EA's per-loop copies of
// the base (`mr r4,r6`, `mr r0,r6`) in EA's order. The Strings, link and start loops' bases are
// ORs of the address with a second copy of it, which stay as `or rD,rS,rS`, the original's
// `mr rD,rS` (the same encoding); the three use different integer types so the frontend does not
// share one OR between the loops.
// port: the fixes add 32-bit offsets to the file's address held in an integer.
s32 PatchScrData(UISScrDataT* pFile) {
    u32 nOff;
    u32 nBase1;
    u32 i;
    UISControlT* pNode;
    u32 j;
    UISLayerT* pGroup;
    u32 kEntry;
    UISObjT* pEntry;
    u32 k;
    UISMapT* pHandler;
    u32 i1;
    u32 i2;
    u32 nBase4;
    long nBase5;
    int nBase6;
    u32 nBase2;
    int nBase3;

    if ((u8*)pFile->Controls < (u8*)pFile) {
        pFile->Controls = (UISControlT*)UISFile_Fix(pFile, pFile->Controls);
        nBase1 = (u32)pFile;
        for (i = 0, nOff = 0; i < pFile->NumControls; nOff += sizeof(UISControlT), i++) {
            pNode = (UISControlT*)((u8*)pFile->Controls + nOff);
            nBase2 = (int)(s64)nBase1 | nBase1;
            nBase3 = (int)(s64)nBase1;
            pNode->pControlInfo = (UISControlInfoT*)(nBase1 + (u32)pNode->pControlInfo);
            pNode->Layers = (UISLayerT**)(nBase1 + (u32)pNode->Layers);
            j = pNode->NumLayers;
            while (j-- != 0) {
                pNode->Layers[j] = (UISLayerT*)(nBase1 + (u32)pNode->Layers[j]);
                pGroup = pNode->Layers[j];
                pGroup->pLayerInfo = (UISControlInfoT*)(nBase1 + (u32)pGroup->pLayerInfo);
                pGroup->Objs = (UISObjT*)(nBase1 + (u32)pGroup->Objs);
                kEntry = pGroup->NumObjs;
                while (kEntry-- != 0) {
                    pEntry = &pGroup->Objs[kEntry];
                    if (pEntry->PluginIndex != 0xFFFF) {
                        pEntry->bInitialized = 0;
                        pEntry->pData = (u32*)(nBase2 + (u32)pEntry->pData);
                    }
                }
            }
            pNode->Maps = (UISMapT*)(nBase1 + (u32)pNode->Maps);
            k = pNode->NumMaps;
            while (k-- != 0) {
                pHandler = &pNode->Maps[k];
                if (pHandler->EventID != 0xFFFF) {
                    pHandler->pFnc = (u8*)(nBase3 + (u32)pHandler->pFnc);
                }
            }
        }
        nBase4 = (u32)pFile;
        pFile->Strings = (UISStringT*)UISFile_Fix(pFile, pFile->Strings);
        i1 = pFile->NumStrings;
        while (i1-- != 0) {
            pFile->Strings[i1].ptr = (void*)((nBase4 | (u32)pFile) + (u32)pFile->Strings[i1].ptr);
        }
        nBase5 = (long)pFile;
        pFile->StringPatchTable = (u32*)UISFile_Fix(pFile, pFile->StringPatchTable);
        i2 = pFile->NumPatchStrings;
        while (i2-- != 0) {
            u32* pLink = (u32*)((u8*)(nBase5 | (long)pFile) + pFile->StringPatchTable[i2]);
            if (*pLink < pFile->NumStrings) {
                *pLink = (uptr)&pFile->Strings[*pLink];  // port: a pointer stored in a 32-bit word
            } else {
                *pLink = 0;
            }
        }
        nBase6 = (int)pFile;
        pFile->StaticObjects = (UISObjT*)UISFile_Fix(pFile, pFile->StaticObjects);
        i = pFile->NumStaticObjects;
        while (i-- != 0) {
            if (pFile->StaticObjects[i].pData != NULL) {
                pFile->StaticObjects[i].pData =
                    (u32*)((nBase6 | (int)pFile) + (u32)pFile->StaticObjects[i].pData);
            }
        }
        return 1;
    }
    return -1;
}

// The size of the block UISInit builds, for the given table sizes. The screen table has one
// more entry's worth of room: the global script's record (pGlobalScript, a UISScreenT).
// fake match: the screen table's size as a 64-bit product (low word = (nScreens + 1) * 20); its
// dead high word (li 20; mulhw) moves the handlers shift to the second slot of the pre-allocation
// schedule as in the original, and is deleted after register allocation.
// port: the product is truncated to 32 bits.
u32 UISGetMemSize(u32 nScreens, u32 nHandlers, u32 nRateFns, u32 n60, u32 nEventWords, u32 nWords2) {
    return sizeof(UISInfoT) + (u32)((s32)(nScreens + 1) * (s64)sizeof(UISScreenT)) +
           nHandlers * sizeof(UISPluginT) + nRateFns * sizeof(UISRateFncT) + n60 * sizeof(UISModalStackT) +
           (nWords2 + nEventWords) * sizeof(s32);
}

// Sets the studio up in the block at pStudio (UISGetMemSize bytes): the header, then each table in
// turn, the global script's record and the two word stacks.
// fake match: optimization level 2 for this function only, the four table counts are copies of
// the parameters, and the three non-power-of-two table sizes are 64-bit products (low word = the
// 32-bit product). The copies and the products' dead high words fill the pre-allocation schedule
// (all are gone after register allocation), which delays the offset adds as in the original: uOffset
// in r12, the 0xFFFF in r31 and each table pointer computed after the one before it is stored.
// port: the products are truncated to 32 bits.
#pragma optimization_level 2
void UISInit(UISInfoT* pStudio, u32 nScreensArg, u32 nHandlersArg, u32 nRateFnsArg, u32 n60Arg,
             u32 nEventWords, u32 nWords2, u32 uMsPerTick) {
    u32 uOffset;
    u32 nScreens = nScreensArg;
    u32 nHandlers = nHandlersArg;
    u32 nRateFns = nRateFnsArg;
    u32 n60 = n60Arg;

    uOffset = sizeof(UISInfoT) + (u32)(nScreens * (u64)sizeof(UISScreenT));
    pStudio->InfoID = UIS_MAGIC;
    pStudio->CriticalRegions = 0;
    pStudio->bShuttingDown = 0;
    pStudio->MaxScreens = nScreens;
    pStudio->NumScreens = 0;
    pStudio->Screens = (UISScreenT*)(pStudio + 1);
    pStudio->MaxPlugins = nHandlers;
    pStudio->NumPlugins = 0;
    pStudio->Plugins = (UISPluginT*)((u8*)pStudio + uOffset);
    uOffset += nHandlers * sizeof(UISPluginT);
    pStudio->MaxRateFncs = nRateFns;
    pStudio->NumRateFncs = 0;
    pStudio->RateFncs = (UISRateFncT*)((u8*)pStudio + uOffset);
    uOffset += (u32)(nRateFns * (u64)sizeof(UISRateFncT));
    pStudio->MaxModals = n60;
    pStudio->NumModals = 0;
    pStudio->ModalStack = (UISModalStackT*)((u8*)pStudio + uOffset);
    uOffset += (u32)(n60 * (u64)sizeof(UISModalStackT));
    pStudio->pGlobalScript = (UISScreenT*)((u8*)pStudio + uOffset);
    uOffset += sizeof(UISScreenT);
    pStudio->pGlobalScript->bWaitingToBeUnloaded = 0;
    pStudio->pGlobalScript->ControllerDisable = -1;
    pStudio->pGlobalScript->GroupID = 0xFFFF;
    pStudio->pGlobalScript->ScreenID = 0xFFFF;
    pStudio->pGlobalScript->ParentGroupID = 0xFFFF;
    pStudio->pGlobalScript->ParentScreenID = 0xFFFF;
    pStudio->pGlobalScript->pScrData = NULL;
    pStudio->EventStack.pStackStart = pStudio->EventStack.pStackCurrent = pStudio->EventStack.pStack =
        (s32*)((u8*)pStudio + uOffset);
    pStudio->EventStack.pStackEnd = pStudio->EventStack.pStackCurrent + nEventWords;
    pStudio->ThreadInfo.pBeginParams = pStudio->EventStack.pStackEnd - 1;
    pStudio->ThreadInfo.pCurrentParams = pStudio->EventStack.pStackEnd - 1;
    pStudio->ThreadInfo.ppEndParams = &pStudio->EventStack.pStackCurrent;
    uOffset += nEventWords * sizeof(s32);
    pStudio->RateStack.pStackStart = pStudio->RateStack.pStackCurrent = pStudio->RateStack.pStack =
        (s32*)((u8*)pStudio + uOffset);
    pStudio->RateStack.pStackEnd = pStudio->RateStack.pStackCurrent + nWords2;
    pStudio->MSPerTick = uMsPerTick;
    pStudio->ActiveScreenIdx = -1;
    pStudio->pMessageFnc = NULL;
    pStudio->pShutdownScreenFnc = 0;
    pStudio->pTransformFnc = NULL;
    pStudio->pLocalizeFnc = 0;
    pStudio->pLoadFnc = NULL;
    pStudio->pUnloadFnc = NULL;
    pStudio->pScreenActivatedFnc = NULL;
    pStudio->pScreenDrawDebugFnc = NULL;
    RuntimeErrorFnc = NULL;
}
#pragma optimization_level reset

// Unloads every screen, the last one first (event 1 with one argument, 0), then marks the
// studio as no longer set up.
void UISShutdown(UISInfoT* pStudio) {
    s32 i;
    UISScreenT* pScreen;
    s32 nArg;
    UISThreadGroupInfoT data;
    u16 uGroup;
    u16 uScreen;

    pStudio->bShuttingDown = 1;
    for (i = pStudio->NumScreens - 1; i >= 0; i--) {
        pScreen = &pStudio->Screens[i];
        uScreen = pScreen->ScreenID;
        uGroup = pScreen->GroupID;
        nArg = 0;
        data.ScreenInfo.GroupID = uGroup;
        data.ScreenInfo.ScreenID = uScreen;
        UISAddThreadAction(uGroup, uScreen, pStudio, 1, &data, 1, &nArg);
        UISProcessThreadAction(pStudio, 0);
    }
    pStudio->InfoID = 0;
    pStudio->bShuttingDown = 0;
}

void UISRegisterMessageFnc(UISInfoT* pStudio, UISMessageFncT* pfnCommand) {
    pStudio->pMessageFnc = pfnCommand;
}

void fn_80169B3C(UISInfoT* pStudio, UISScreenDrawDebugFncT* pfnScreen28) {
    pStudio->pScreenDrawDebugFnc = pfnScreen28;
}

void UISRegisterResourceFncs(UISInfoT* pStudio, UISResLoadFncT* pfnLoad, UISResUnloadFncT* pfnUnload) {
    pStudio->pLoadFnc = pfnLoad;
    pStudio->pUnloadFnc = pfnUnload;
}

void UISRegisterTransformFncs(UISInfoT* pStudio, UISTransformFncT* pfnTransform) {
    pStudio->pTransformFnc = pfnTransform;
}

void UISRegisterPluginFnc(UISInfoT* pStudio, s32 nIndex, UISPluginFncT* pfnHandler) {
    pStudio->Plugins[nIndex].pFnc = pfnHandler;
    pStudio->NumPlugins++;
}

// Brings back a screen that is already loaded: its rate functions are finished, its first node
// is marked active again and it gets event -2 with the arguments (nArgs 0xFF: the count is in
// pArgs[10]). Returns 0: nothing new was loaded.
static inline s32 Screen_BringBack(UISInfoT* pStudio, u16 uGroup, u16 uScreen, u8 nArgs, s32* pArgs) {
    UISScreenT* pScreen;
    u32 i;
    u32 n;
    u8 bOut;

    i = UISFindScreen(pStudio, uGroup, uScreen);
    pScreen = &pStudio->Screens[i];
    UISRemoveUnNessaryRateFncs(pStudio);
    pStudio->CriticalRegions |= 4;
    n = pStudio->NumRateFncs;
    for (i = 0; i < n; i++) {
        if (pStudio->RateFncs[i].pScreen == pScreen) {
            pStudio->RateFncs[i].State = 1;
        }
    }
    pStudio->CriticalRegions &= ~4;
    UISRemoveUnNessaryRateFncs(pStudio);
    pScreen->pScrData->Controls[0].pControlInfo->IsEnabled = 1;
    bOut = 0;
    if (nArgs == 0xFF) {
        nArgs = pArgs[10];
    }
    pStudio->CriticalRegions |= 2;
    _ParseMaps(pStudio, pScreen, &pStudio->EventStack, 0, -1, -2, nArgs, pArgs, &bOut);
    pStudio->CriticalRegions &= ~2;
    return 0;
}

// Loads a screen and makes it the newest in the table, with uPrevGroup and uPrevScreen as the
// screen to go back to. Its UI file is fixed up (event -9 when that was its first time), its
// nodes are set up and it gets event -2 with the arguments. A loaded screen is brought back
// instead. Returns 0 when the load callback gave nothing or the screen was already loaded.
s32 UISInternalLoadScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen, u16 uPrevGroup, u16 uPrevScreen,
                          u8 nArgs, s32* pArgs) {
    u32 nIndex;
    void* pFile;
    UISScreenT* pScreen;
    s32 bFixed;
    u8 bOut;

    // fake match: nScreens and nIndex go through s32 -> s64 -> u32 (the values are unchanged);
    // the dead high words give uGroup, uScreen and uPrevGroup the neighbours that put them in
    // pStudio's allocation level, so they take EA's r25-r27 above pStudio's r24
    nIndex = UISFindScreen(pStudio, uGroup, uScreen);
    if (nIndex < (u32)(s64)(s32)pStudio->NumScreens) {
        return Screen_BringBack(pStudio, uGroup, uScreen, nArgs, pArgs);
    }
    pFile = pStudio->pLoadFnc(uGroup, uScreen);
    if (pFile == NULL) return 0;
    pStudio->NumScreens = (u32)(s64)(s32)(pStudio->NumScreens + 1);
    pScreen = &pStudio->Screens[(u32)(s64)(s32)nIndex];
    pScreen->GroupID = uGroup;
    pScreen->ScreenID = uScreen;
    pScreen->ParentGroupID = uPrevGroup;
    pScreen->ParentScreenID = uPrevScreen;
    pScreen->ControllerDisable = 0;
    pScreen->bWaitingToBeUnloaded = 0;
    pScreen->pScrData = pFile;
    bFixed = PatchScrData(pScreen->pScrData);
    // fake match: bFixed goes through a 64-bit shift up and back down (the value is unchanged).
    // The shifts become a chain of word copies after bFixed; each copy-propagation pass removes
    // one link, so bFixed's copy from the call result survives them all (EA's mr r0,r3 ...
    // mr r25,r0).
    // port: relies on the conversion to s64 wrapping and on >> of a negative s64 being arithmetic.
    bFixed = (s32)((s64)((u64)bFixed << 32) >> 32);
    _ParseInitialize(pStudio, pScreen, 0, -1);
    pScreen->pScrData->Controls[0].pControlInfo->IsEnabled = 1;
    if (bFixed) {
        bOut = 0;
        pStudio->CriticalRegions |= 2;
        _ParseMaps(pStudio, pScreen, &pStudio->EventStack, 0, -1, -9, 0, NULL, &bOut);
        pStudio->CriticalRegions &= ~2;
    }
    _ParseTransforms(pStudio, 0, pScreen, 0);
    bOut = 0;
    if (nArgs == 0xFF) {
        nArgs = pArgs[10];
    }
    pStudio->CriticalRegions |= 2;
    _ParseMaps(pStudio, pScreen, &pStudio->EventStack, 0, -1, -2, nArgs, pArgs, &bOut);
    pStudio->CriticalRegions &= ~2;
    return 1;
}

// Goes to a screen. A loaded one is brought back; otherwise it is loaded, and with bPush (forced
// while ModalStack holds records) a ModalStack record is pushed for it that remembers the screen that was
// current and the new screen's first node. Returns what UISInternalLoadScreen returned.
s32 _UISInternalLoad(UISInfoT* pStudio, u16 uGroup, u16 uScreen, u8 bPush, u8 nArgs, s32* pArgsArg) {
    s16 nPrevGroup;
    UISModalStackT* pRec;
    s16 nPrevScreen;
    s32 nResult;
    u32 bLoaded;
    u32 nRecord;
    u16 uIndex;
    s32* pArgs;

    // fake match: pArgs goes through a 64-bit shift up and back down (the value is unchanged).
    // The shifts become a chain of word copies at the entry that register allocation coalesces;
    // the merged copies stay neighbours of pStudio, which then leaves the graph last and takes
    // EA's r31 (the u16 conversion of nPrevGroup, live only after the entry, keeps EA's r22).
    // port: truncates the pointer to 32 bits; a port writes pArgs = pArgsArg.
    pArgs = (s32*)(u32)((u64)((u64)(u32)pArgsArg << 32) >> 32);
    nRecord = 0;
    if (pStudio->NumScreens == 0 || pStudio->ActiveScreenIdx == -1) {
        nPrevGroup = -1;
        nPrevScreen = -1;
    } else {
        nPrevGroup = pStudio->Screens[pStudio->ActiveScreenIdx].GroupID;
        nPrevScreen = pStudio->Screens[pStudio->ActiveScreenIdx].ScreenID;
    }
    bLoaded = UISFindScreen(pStudio, uGroup, uScreen) < pStudio->NumScreens;
    if (bLoaded == 1) {
        return Screen_BringBack(pStudio, uGroup, uScreen, nArgs, pArgs);
    }
    if (pStudio->NumModals != 0 && !bLoaded) {
        bPush = 1;
    }
    if (bPush) {
        nRecord = pStudio->NumModals;
        pRec = &pStudio->ModalStack[nRecord];
        memset(pRec, 0, sizeof(UISModalStackT));
        pRec->GroupID = uGroup;
        pRec->ScreenID = uScreen;
        pStudio->NumModals++;
    }
    nResult = UISInternalLoadScreen(pStudio, uGroup, uScreen, nPrevGroup, nPrevScreen, nArgs, pArgs);
    if (bPush) {
        uIndex = UISFindScreen(pStudio, uGroup, uScreen);
        if (uIndex < (s32)pStudio->NumScreens) {  // fake match: this compare is signed
            pRec = &pStudio->ModalStack[nRecord];
            // fake match: the screen addressed by byte offset (EA's addi r0,r5,0x10; lwzx)
            pRec->pControlInfo = ((UISScreenT*)((u8*)pStudio->Screens + uIndex * sizeof(UISScreenT)))
                                     ->pScrData->Controls[0].pControlInfo;
            uIndex = UISFindScreen(pStudio, nPrevGroup, nPrevScreen);
            if (uIndex < (s32)pStudio->NumScreens) {  // fake match: this compare is signed
                pRec->pScreen = &pStudio->Screens[uIndex];
            }
        } else {
            pStudio->NumModals--;
        }
    }
    return nResult;
}

// Makes pFile the studio's UI file (pGlobalScript->pScrData), fixing up its offsets unless it
// already is that file. Returns whether the file can be used; a file that cannot is dropped.
u8 UISSetGlobalScript(UISInfoT* pStudio, UISScrDataT* pFile) {
    u8 bOk;

    bOk = 0;
    if (pFile != NULL) {
        if (pFile != pStudio->pGlobalScript->pScrData) {
            bOk = PatchScrData(pFile);
            if (!bOk) {
                pFile = NULL;
            }
        } else {
            bOk = 1;
        }
    }
    pStudio->pGlobalScript->pScrData = pFile;
    return bOk;
}

// Goes to a screen: queued as event 0 while an event is being sent, at once otherwise.
// fake match: optimization level 2 for this function only, and uGroup / uScreen are copies of the
// parameters. At level 4 the extra copy-propagation passes fold uScreen into r5 before the
// argument setup, so pStudio is copied out of r3 instead of EA's `mr r8,r5` copy of uScreen.
#pragma optimization_level 2
s32 UISLoadScreen(UISInfoT* pStudio, u16 uGroupArg, u16 uScreenArg, u8 nArgs, s32* pArgs) {
    UISThreadGroupInfoT data;
    u16 uGroup;
    u16 uScreen;

    uGroup = uGroupArg;
    uScreen = uScreenArg;
    if (pStudio->CriticalRegions & 2) {
        data.ScreenInfo.GroupID = uGroup;
        data.ScreenInfo.ScreenID = uScreen;
        data.ScreenInfo.ParentGroupID = 0xFFFF;
        data.ScreenInfo.ParentScreenID = 0xFFFF;
        UISAddThreadAction(uGroup, uScreen, pStudio, 0, &data, nArgs, pArgs);
    } else {
        return _UISInternalLoad(pStudio, uGroup, uScreen, 0, nArgs, pArgs);
    }
    return 1;
}
#pragma optimization_level reset

// Called before a screen is unloaded. If the last ModalStack record names the screen, it is dropped:
// the screen it holds becomes current, and its paused script runs on with n on the top of its
// stack. Returns 0 when an older record names the screen, or holds it: it cannot go yet.
u8 UISInternalUnloadModal(UISInfoT* pStudio, u16 uGroup, u16 uScreen, s32 n) {
    s32 i;
    UISModalStackT* pRec;
    UISScreenT* pScreen;
    UISStackInfoT* pFrame;
    s32* p1C;

    i = pStudio->NumModals;
    if (i > 0) {
        pRec = &pStudio->ModalStack[i - 1];
        if (pRec->ScreenID == uScreen && pRec->GroupID == uGroup) {
            // fake match: i - 1 goes through s64 and back (the stored value is unchanged); the
            // dead high word gives pStudio one neighbour more in register allocation, so it
            // takes EA's r29 above pFrame and p1C
            pStudio->NumModals = (s64)(i - 1);
            if (pRec->pScreen != NULL) {
                pStudio->ActiveScreenIdx = UISFindScreen(pStudio, pRec->pScreen->GroupID,
                                                         pRec->pScreen->ScreenID);
            } else {
                pStudio->ActiveScreenIdx = -1;
                return 1;
            }
            if (pStudio->ActiveScreenIdx < pStudio->NumScreens) {
                p1C = pRec->pRestoreStack;
                if (p1C != NULL) {
                    pFrame = pRec->pRestoreState;
                    *pFrame = pRec->StackState;
                    pFrame->pStack[-1] = n;
                    if (UISStackProcess(pStudio, pRec->pRestoreStack, pFrame, pRec->pScreen,
                                        pRec->pControlInfo) != 3) {
                        pFrame->pStack = p1C;
                    }
                }
            }
        } else {
            while (i-- != 0) {
                pRec = &pStudio->ModalStack[i];
                if (pRec->ScreenID == uScreen && pRec->GroupID == uGroup) return 0;
                pScreen = pRec->pScreen;
                if (pScreen != NULL && pScreen->ScreenID == uScreen && pScreen->GroupID == uGroup) return 0;
            }
        }
    }
    return 1;
}

// Unloads a screen. Screens that named it as their previous screen take its previous screen
// instead; it gets event -1 and the type 9 events queued for it (UISThreadProcessHints), its nodes and rate
// functions are dropped, the unload callback frees its data and the table closes up. With no
// current screen left, its previous screen (or the last one) becomes current through event 3.
// Returns 0 when UISInternalUnloadModal says the screen cannot go yet.
u8 UISInternalUnloadScreen(UISInfoT* pStudio, u16 uGroup, u16 uScreen, s32 n) {
    UISScreenT* pScreen;
    UISScreenT* pSrc;
    UISScreenT* pOther;
    u32 nIndex;
    u16 uPrevScreen;
    u16 uPrevGroup;
    u32 i;
    UISThreadGroupInfoT data;
    u8 bOut;
    u32 j;

    nIndex = UISFindScreen(pStudio, uGroup, uScreen);
    if (nIndex < pStudio->NumScreens) {
        pScreen = &pStudio->Screens[nIndex];
        uPrevGroup = pScreen->ParentGroupID;
        uPrevScreen = pScreen->ParentScreenID;
        if (!UISInternalUnloadModal(pStudio, uGroup, uScreen, n)) return 0;
        if (nIndex == pStudio->ActiveScreenIdx) {
            pStudio->ActiveScreenIdx = -1;
        }
        for (i = 0; i < pStudio->NumScreens; i++) {
            pOther = &pStudio->Screens[i];
            if (pOther->ParentGroupID == uGroup && pOther->ParentScreenID == uScreen) {
                pOther->ParentGroupID = uPrevGroup;
                pOther->ParentScreenID = uPrevScreen;
            }
        }
        bOut = 0;
        pStudio->CriticalRegions |= 2;
        _ParseMaps(pStudio, pScreen, &pStudio->EventStack, 0, -1, -3, 0, NULL, &bOut);
        pStudio->CriticalRegions &= ~2;
        UISThreadProcessHints(pStudio, uGroup, uScreen);
        _ParseInitialize(pStudio, pScreen, 0, -3);
        _ParseTransforms(pStudio, 3, pScreen, 0);
        j = pStudio->NumRateFncs;
        while (j-- != 0) {
            if (pStudio->RateFncs[j].pScreen == pScreen) {
                pStudio->RateFncs[j].State = 1;
            }
        }
        UISRemoveUnNessaryRateFncs(pStudio);
        pStudio->pUnloadFnc(pScreen->GroupID, pScreen->ScreenID, pScreen->pScrData);
        pScreen->pScrData = NULL;
        pStudio->NumScreens--;
        for (; nIndex < pStudio->NumScreens; nIndex++) {
            pScreen = &pStudio->Screens[nIndex];
            pSrc = &pStudio->Screens[nIndex + 1];
            memmove(pScreen, pSrc, sizeof(UISScreenT));
            i = pStudio->NumModals;
            while (i-- != 0) {
                if (pStudio->ModalStack[i].pScreen == pSrc) {
                    pStudio->ModalStack[i].pScreen = pScreen;
                }
            }
            i = pStudio->MaxRateFncs;
            while (i-- != 0) {
                if (pStudio->RateFncs[i].pScreen == pSrc) {
                    pStudio->RateFncs[i].pScreen = pScreen;
                }
            }
            if (nIndex + 1 == pStudio->ActiveScreenIdx) {
                pStudio->ActiveScreenIdx = nIndex;
            }
        }
        if (pStudio->ActiveScreenIdx == -1) {
            if (pStudio->NumScreens != 0) {
                pStudio->ActiveScreenIdx = UISFindScreen(pStudio, uPrevGroup, uPrevScreen);
                if (pStudio->ActiveScreenIdx < pStudio->NumScreens) {
                    pScreen = &pStudio->Screens[pStudio->ActiveScreenIdx];
                } else {
                    pStudio->ActiveScreenIdx = pStudio->NumScreens - 1;
                    pScreen = &pStudio->Screens[pStudio->ActiveScreenIdx];
                }
                if (pScreen != NULL) {
                    data.ScreenInfo.GroupID = pScreen->GroupID;
                    data.ScreenInfo.ScreenID = pScreen->ScreenID;
                    UISAddThreadAction(data.ScreenInfo.GroupID, data.ScreenInfo.ScreenID, pStudio, 3, &data,
                                       0, NULL);
                }
            } else {
                pStudio->ActiveScreenIdx = -1;
            }
        }
    }
    return 1;
}

// Queues event 3 for a screen and runs the queue, unless an event is being sent right now.
// fake match: optimization level 1 for this function only. At level 4 the copy-propagation passes
// fold uScreen into r5 before the argument setup, so pStudio is set after the extsh's instead of
// EA's `mr r0,r5` copy of uScreen and `mr r5,r31` first (see UISLoadScreen).
#pragma optimization_level 1
void UISSetScreenActive(UISInfoT* pStudio, u16 uGroup, u16 uScreen) {
    UISThreadGroupInfoT data;

    data.ScreenInfo.GroupID = uGroup;
    data.ScreenInfo.ScreenID = uScreen;
    UISAddThreadAction(uGroup, uScreen, pStudio, 3, &data, 0, NULL);
    if (!(pStudio->CriticalRegions & 2)) {
        UISProcessThreadAction(pStudio, 0);
    }
}
#pragma optimization_level reset

// Returns the current screen's group and screen IDs, 0xFFFF when there is no current screen.
void UISGetActiveScreen(UISInfoT* pStudio, u16* puGroup, u16* puScreen) {
    if (puGroup != NULL) {
        *puGroup = 0xFFFF;
        if (pStudio->ActiveScreenIdx != -1) {
            *puGroup = pStudio->Screens[pStudio->ActiveScreenIdx].GroupID;
        }
    }
    if (puScreen != NULL) {
        *puScreen = 0xFFFF;
        if (pStudio->ActiveScreenIdx != -1) {
            *puScreen = pStudio->Screens[pStudio->ActiveScreenIdx].ScreenID;
        }
    }
}

// Runs the queued events, makes the screen named by the last ModalStack record current, then sends
// event uEvent to it (or to every screen when bAll is set) unless the screen has taken it
// already; with n < 0 it is sent again.
void UISProcessEvent(UISInfoT* pStudio, u32 uEvent, s32 n, s32 b, void* p, u8 bAll) {
    s32 nLast;
    u32 i;
    u32 nEnd;
    UISScreenT* pScreen;
    s32 nTaken;
    u8 bOut;

    UISProcessThreadAction(pStudio, 0);
    nLast = pStudio->NumModals - 1;
    if (nLast >= 0) {
        pStudio->ActiveScreenIdx = UISFindScreen(pStudio, pStudio->ModalStack[nLast].GroupID,
                                                 pStudio->ModalStack[nLast].ScreenID);
    }
    if (bAll) {
        nEnd = pStudio->NumScreens;
        i = 0;
    } else {
        i = pStudio->ActiveScreenIdx;
        nEnd = i + 1;
        if (i == -1) return;
    }
    for (; i < nEnd; i++) {
        pScreen = &pStudio->Screens[i];
        nTaken = pScreen->ControllerDisable & (1 << uEvent);
        // EA bug: the masked bit equals 1 only for event 0, so the resend works for that event alone
        if ((nTaken == 1 && n < 0) || nTaken == 0) {
            bOut = 0;
            pStudio->CriticalRegions |= 2;
            // fake match: the (int) cast sets uEvent's register (an int uEvent parameter does the same)
            _ParseMaps(pStudio, pScreen, &pStudio->EventStack, 0, (int)uEvent, n, b, p, &bOut);
            pStudio->CriticalRegions &= ~2;
        }
    }
}

// Sends event uEvent to the current screen, or to every screen when bAll is set. Event -8 skips
// a screen that is being unloaded.
void UISProcessInternalEvents(UISInfoT* pStudio, UISStackInfoT* pStack, int uEvent, u32 n, s32 b, void* p,
                              u8 bAll) {
    u32 i;
    u32 nEnd;
    UISScreenT* pScreen;
    u8 bOut;

    // fake match: the round trip through s8 gives back bAll for every u8, but the longer chain
    // before the compare lets the scheduler put the pStudio and pStack copies ahead of it, as in
    // EA's code (their registers follow from that)
    if ((u8)(s32)(s8)bAll) {
        nEnd = pStudio->NumScreens;
        i = 0;
    } else {
        i = pStudio->ActiveScreenIdx;
        nEnd = i + 1;
        if (i == -1) return;
    }
    for (; i < nEnd; i++) {
        pScreen = &pStudio->Screens[i];
        if (n != -8 || pScreen->bWaitingToBeUnloaded != 1) {
            bOut = 0;
            _ParseMaps(pStudio, pScreen, pStack, 0, uEvent, n, b, p, &bOut);
        }
    }
}

// Runs the studio for nTicks ticks, then updates every screen.
void UISDrawObjects(UISInfoT* pStudio, s32 nTicks) {
    u32 uMs;
    u32 i;
    u32 n;

    uMs = pStudio->MSPerTick * nTicks;
    UISSetColorMultipler(1.0f, 1.0f, 1.0f, 1.0f);
    UISSetColorAdditive(0.0f, 0.0f, 0.0f, 0.0f);
    _ParseRateFncs(pStudio, uMs);
    n = pStudio->NumScreens;
    for (i = 0; i < n; i++) {
        _ParseObjects(pStudio, &pStudio->Screens[i], NULL, -2);
    }
}

// UISApi.c (our name): the calls the game makes into EA's UI Studio library (the menu screens'
// runtime): setting the studio up in one block of memory, handing it the game's callbacks,
// running it every frame, and queueing screen changes. A separate file from UIStudio.c: it has
// its own copies of the 0.0f and 1.0f constants.

#include "frontend/uistudio.h"

// Runs the rate functions for MSElapsed milliseconds. Each running one steps once per MSRate ms: its
// step script (if any) gives the step's scale, the variable moves by fStepValue times it, and once it
// reaches fEndValue the function finishes and its done script (or the first node's event -14
// handler) runs.
void _ParseRateFncs(UISInfoT* pInfo, u32 MSElapsed) {
    // fake match: this declaration order (with uMsCopy and the pfModVal identity below) gives EA's
    // registers: numRateFncs, pArg1, pArg2 and uMsCopy leave the allocator's graph before the others.
    u32 uMsCopy;
    u32 numRateFncs;
    s32* pArg1;
    s32* pArg2;
    UISRateFncT* pRateFnc;
    u32 idxRateFnc;
    UISScreenT* pScreen;
    UISStackInfoT* pStackInfo;
    s32 iElapse;
    s32 nParam;
    u8* pEndRateFncPC;
    f32 fAcel;
    f32* pfModVal;
    f32 newVal;
    UISParamT ReturnParam;
    s32 RateParams[3];

    // fake match: uMsCopy is MSElapsed: each OR's low word is the value | 0 (the value shifted up only
    // fills the high word, which is dropped). The three become copies only after constant
    // propagation, so one link reaches register allocation, which merges MSElapsed into uMsCopy; the
    // loop's value then has uMsCopy's higher number (EA's r22, removed before pInfo).
    // port: a port writes uMsCopy = MSElapsed.
    uMsCopy = (u32)((u64)MSElapsed | ((u64)MSElapsed << 32));
    uMsCopy = (u32)((u64)uMsCopy | ((u64)uMsCopy << 32));
    uMsCopy = (u32)((u64)uMsCopy | ((u64)uMsCopy << 32));
    UISRemoveUnNessaryRateFncs(pInfo);
    pStackInfo = &pInfo->RateStack;
    pInfo->CriticalRegions |= 4;
    numRateFncs = pInfo->NumRateFncs;
    for (idxRateFnc = 0; idxRateFnc < numRateFncs; idxRateFnc++) {
        pRateFnc = &pInfo->RateFncs[idxRateFnc];
        if (pRateFnc->State != 2) continue;
        pScreen = pRateFnc->pScreen;
        if (pRateFnc->MSRate == 0) continue;
        // fake match: &RateParams[1] / &RateParams[2] set on both arms of a test that is always true
        // here (the branch reuses the continue's compare and disappears); the two definitions keep the
        // stores' addresses in registers hoisted out of the loops (EA's addi r24,r1,0x20 / addi
        // r23,r1,0x24).
        if (pRateFnc->MSRate) {
            pArg1 = &RateParams[1];
            pArg2 = &RateParams[2];
        } else {
            pArg1 = &RateParams[1];
            pArg2 = &RateParams[2];
        }
        pRateFnc->MSCount += uMsCopy;
        iElapse = pRateFnc->MSCount - pRateFnc->MSLastCount;
        while (iElapse >= (s32)pRateFnc->MSRate) {
            iElapse -= pRateFnc->MSRate;
            pRateFnc->MSLastCount += pRateFnc->MSRate;
            if (pRateFnc->pFnc != NULL) {
                if (pRateFnc->State != 2) break;
                ReturnParam.fValue = 1.0f;
                RateParams[0] = pRateFnc->RateFncID;
                *pArg1 = pRateFnc->MSLastCount;
                if (pRateFnc->AnimationData.iType == 0) {
                    nParam = 3;
                    *pArg2 = pRateFnc->MSRate;
                } else {
                    nParam = 2;
                }
                UISExecuteFnc(pInfo, pScreen, pRateFnc->pControlInfo, pStackInfo, pRateFnc->pFnc, nParam,
                              RateParams, 0, NULL, 0, -1, &ReturnParam.iValue);
                fAcel = ReturnParam.fValue;
            } else {
                fAcel = 1.0f;
            }
            if (pRateFnc->AnimationData.iType != 0) {
                pfModVal = UISGetActionPtrValue(pRateFnc->AnimationData.iType,
                                                pRateFnc->AnimationData.pSubControlInfo);
                // fake match: pfModVal goes through a 64-bit shift up and back down (the value is
                // unchanged). The copy of the call's result it leaves is merged at register
                // allocation and stays a neighbour of every variable live here: one more for each
                // of them, which gives EA's allocation order.
                // port: relies on the conversion to s64 wrapping and on >> of a negative s64 being
                // arithmetic; truncates the pointer to 32 bits. A port leaves this line out.
                pfModVal = (f32*)(u32)((s64)((u64)(u32)pfModVal << 32) >> 32);
                // fake match: fStepValue read into newVal before the scale test (EA loads it there)
                newVal = pRateFnc->AnimationData.fStepValue;
                if (fAcel < 0.0f) {
                    fAcel = 1.0f;
                }
                newVal = newVal * fAcel + *pfModVal;
                if ((pRateFnc->AnimationData.fStepValue > 0.0f
                     && newVal < pRateFnc->AnimationData.fEndValue)
                    || (pRateFnc->AnimationData.fStepValue < 0.0f
                        && newVal > pRateFnc->AnimationData.fEndValue)) {
                    *pfModVal = newVal;
                } else {
                    *pfModVal = pRateFnc->AnimationData.fEndValue;
                    pRateFnc->State = 1;
                    if (pRateFnc->AnimationData.pEndFnc != NULL) {
                        RateParams[0] = pRateFnc->RateFncID;
                        UISExecuteFnc(pInfo, pScreen, pRateFnc->pControlInfo, pStackInfo,
                                      pRateFnc->AnimationData.pEndFnc, 1, RateParams, 0, NULL, 0, -1, NULL);
                    } else {
                        pEndRateFncPC = UISFindEventPC(pScreen->pScrData->Controls, -14);
                        if (pEndRateFncPC != NULL) {
                            RateParams[0] = pRateFnc->RateFncID;
                            UISExecuteFnc(pInfo, pScreen, pRateFnc->pControlInfo, pStackInfo, pEndRateFncPC,
                                          1, RateParams, 0, NULL, 0, -1, NULL);
                        }
                    }
                }
            }
        }
    }
    pInfo->CriticalRegions &= ~4;
    UISRemoveUnNessaryRateFncs(pInfo);
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
// the node loop steps a byte offset nOff alongside idxControl; with nOff and nBase1 declared first, the
// allocator gives the node-level values EA's registers. nBase2 is an OR of the address with itself
// (x | x == x) and nBase3 the address through a 64-bit round trip (the same low word), both set at
// the top of the node body: the frontend cannot fold them, and they give EA's per-loop copies of
// the base (`mr r4,r6`, `mr r0,r6`) in EA's order. The Strings, link and start loops' bases are
// ORs of the address with a second copy of it, which stay as `or rD,rS,rS`, the original's
// `mr rD,rS` (the same encoding); the three use different integer types so the frontend does not
// share one OR between the loops.
// port: the fixes add 32-bit offsets to the file's address held in an integer.
s32 PatchScrData(UISScrDataT* pNewBase) {
    u32 nOff;
    u32 nBase1;
    u32 idxControl;
    UISControlT* pControl;
    u32 idxLayer;
    UISLayerT* pGroup;
    u32 idxObj;
    UISObjT* pObj;
    u32 idxMap;
    UISMapT* pMap;
    u32 idxStr;
    u32 idxPatch;
    u32 nBase4;
    long nBase5;
    int nBase6;
    u32 nBase2;
    int nBase3;

    if ((u8*)pNewBase->Controls < (u8*)pNewBase) {
        pNewBase->Controls = (UISControlT*)UISFile_Fix(pNewBase, pNewBase->Controls);
        nBase1 = (u32)pNewBase;
        for (idxControl = 0, nOff = 0; idxControl < pNewBase->NumControls;
             nOff += sizeof(UISControlT), idxControl++) {
            pControl = (UISControlT*)((u8*)pNewBase->Controls + nOff);
            nBase2 = (int)(s64)nBase1 | nBase1;
            nBase3 = (int)(s64)nBase1;
            pControl->pControlInfo = (UISControlInfoT*)(nBase1 + (u32)pControl->pControlInfo);
            pControl->Layers = (UISLayerT**)(nBase1 + (u32)pControl->Layers);
            idxLayer = pControl->NumLayers;
            while (idxLayer-- != 0) {
                pControl->Layers[idxLayer] = (UISLayerT*)(nBase1 + (u32)pControl->Layers[idxLayer]);
                pGroup = pControl->Layers[idxLayer];
                pGroup->pLayerInfo = (UISControlInfoT*)(nBase1 + (u32)pGroup->pLayerInfo);
                pGroup->Objs = (UISObjT*)(nBase1 + (u32)pGroup->Objs);
                idxObj = pGroup->NumObjs;
                while (idxObj-- != 0) {
                    pObj = &pGroup->Objs[idxObj];
                    if (pObj->PluginIndex != 0xFFFF) {
                        pObj->bInitialized = 0;
                        pObj->pData = (u32*)(nBase2 + (u32)pObj->pData);
                    }
                }
            }
            pControl->Maps = (UISMapT*)(nBase1 + (u32)pControl->Maps);
            idxMap = pControl->NumMaps;
            while (idxMap-- != 0) {
                pMap = &pControl->Maps[idxMap];
                if (pMap->EventID != 0xFFFF) {
                    pMap->pFnc = (u8*)(nBase3 + (u32)pMap->pFnc);
                }
            }
        }
        nBase4 = (u32)pNewBase;
        pNewBase->Strings = (UISStringT*)UISFile_Fix(pNewBase, pNewBase->Strings);
        idxStr = pNewBase->NumStrings;
        while (idxStr-- != 0) {
            pNewBase->Strings[idxStr].ptr =
                (void*)((nBase4 | (u32)pNewBase) + (u32)pNewBase->Strings[idxStr].ptr);
        }
        nBase5 = (long)pNewBase;
        pNewBase->StringPatchTable = (u32*)UISFile_Fix(pNewBase, pNewBase->StringPatchTable);
        idxPatch = pNewBase->NumPatchStrings;
        while (idxPatch-- != 0) {
            u32* patchAddr = (u32*)((u8*)(nBase5 | (long)pNewBase) + pNewBase->StringPatchTable[idxPatch]);
            if (*patchAddr < pNewBase->NumStrings) {
                *patchAddr = (uptr)&pNewBase->Strings[*patchAddr];  // port: a pointer stored in a 32-bit word
            } else {
                *patchAddr = 0;
            }
        }
        nBase6 = (int)pNewBase;
        pNewBase->StaticObjects = (UISObjT*)UISFile_Fix(pNewBase, pNewBase->StaticObjects);
        idxControl = pNewBase->NumStaticObjects;
        while (idxControl-- != 0) {
            if (pNewBase->StaticObjects[idxControl].pData != NULL) {
                pNewBase->StaticObjects[idxControl].pData =
                    (u32*)((nBase6 | (int)pNewBase) + (u32)pNewBase->StaticObjects[idxControl].pData);
            }
        }
        return 1;
    }
    return -1;
}

// The size of the block UISInit builds, for the given table sizes. The screen table has one
// more entry's worth of room: the global script's record (pGlobalScript, a UISScreenT).
// fake match: the screen table's size as a 64-bit product (low word = (MaxScreens + 1) * 20); its
// dead high word (li 20; mulhw) moves the handlers shift to the second slot of the pre-allocation
// schedule as in the original, and is deleted after register allocation.
// port: the product is truncated to 32 bits.
u32 UISGetMemSize(u32 MaxScreens, u32 MaxPlugins, u32 MaxRateFncs, u32 MaxModals, u32 StackSize,
                  u32 RateStackSize) {
    return sizeof(UISInfoT) + (u32)((s32)(MaxScreens + 1) * (s64)sizeof(UISScreenT)) +
           MaxPlugins * sizeof(UISPluginT) + MaxRateFncs * sizeof(UISRateFncT) +
           MaxModals * sizeof(UISModalStackT) + (RateStackSize + StackSize) * sizeof(s32);
}

// Sets the studio up in the block at pInfo (UISGetMemSize bytes): the header, then each table in
// turn, the global script's record and the two word stacks.
// fake match: optimization level 2 for this function only, the four table counts are copies of
// the parameters, and the three non-power-of-two table sizes are 64-bit products (low word = the
// 32-bit product). The copies and the products' dead high words fill the pre-allocation schedule
// (all are gone after register allocation), which delays the offset adds as in the original: Offset
// in r12, the 0xFFFF in r31 and each table pointer computed after the one before it is stored.
// port: the products are truncated to 32 bits.
#pragma optimization_level 2
void UISInit(UISInfoT* pInfo, u32 MaxScreens, u32 MaxPlugins, u32 MaxRateFncs, u32 MaxModals, u32 StackSize,
             u32 RateStackSize, u32 MSPerTick) {
    u32 Offset;
    u32 nScreens = MaxScreens;
    u32 nHandlers = MaxPlugins;
    u32 nRateFns = MaxRateFncs;
    u32 n60 = MaxModals;

    Offset = sizeof(UISInfoT) + (u32)(nScreens * (u64)sizeof(UISScreenT));
    pInfo->InfoID = UIS_MAGIC;
    pInfo->CriticalRegions = 0;
    pInfo->bShuttingDown = 0;
    pInfo->MaxScreens = nScreens;
    pInfo->NumScreens = 0;
    pInfo->Screens = (UISScreenT*)(pInfo + 1);
    pInfo->MaxPlugins = nHandlers;
    pInfo->NumPlugins = 0;
    pInfo->Plugins = (UISPluginT*)((u8*)pInfo + Offset);
    Offset += nHandlers * sizeof(UISPluginT);
    pInfo->MaxRateFncs = nRateFns;
    pInfo->NumRateFncs = 0;
    pInfo->RateFncs = (UISRateFncT*)((u8*)pInfo + Offset);
    Offset += (u32)(nRateFns * (u64)sizeof(UISRateFncT));
    pInfo->MaxModals = n60;
    pInfo->NumModals = 0;
    pInfo->ModalStack = (UISModalStackT*)((u8*)pInfo + Offset);
    Offset += (u32)(n60 * (u64)sizeof(UISModalStackT));
    pInfo->pGlobalScript = (UISScreenT*)((u8*)pInfo + Offset);
    Offset += sizeof(UISScreenT);
    pInfo->pGlobalScript->bWaitingToBeUnloaded = 0;
    pInfo->pGlobalScript->ControllerDisable = -1;
    pInfo->pGlobalScript->GroupID = 0xFFFF;
    pInfo->pGlobalScript->ScreenID = 0xFFFF;
    pInfo->pGlobalScript->ParentGroupID = 0xFFFF;
    pInfo->pGlobalScript->ParentScreenID = 0xFFFF;
    pInfo->pGlobalScript->pScrData = NULL;
    pInfo->EventStack.pStackStart = pInfo->EventStack.pStackCurrent = pInfo->EventStack.pStack =
        (s32*)((u8*)pInfo + Offset);
    pInfo->EventStack.pStackEnd = pInfo->EventStack.pStackCurrent + StackSize;
    pInfo->ThreadInfo.pBeginParams = pInfo->EventStack.pStackEnd - 1;
    pInfo->ThreadInfo.pCurrentParams = pInfo->EventStack.pStackEnd - 1;
    pInfo->ThreadInfo.ppEndParams = &pInfo->EventStack.pStackCurrent;
    Offset += StackSize * sizeof(s32);
    pInfo->RateStack.pStackStart = pInfo->RateStack.pStackCurrent = pInfo->RateStack.pStack =
        (s32*)((u8*)pInfo + Offset);
    pInfo->RateStack.pStackEnd = pInfo->RateStack.pStackCurrent + RateStackSize;
    pInfo->MSPerTick = MSPerTick;
    pInfo->ActiveScreenIdx = -1;
    pInfo->pMessageFnc = NULL;
    pInfo->pShutdownScreenFnc = 0;
    pInfo->pTransformFnc = NULL;
    pInfo->pLocalizeFnc = 0;
    pInfo->pLoadFnc = NULL;
    pInfo->pUnloadFnc = NULL;
    pInfo->pScreenActivatedFnc = NULL;
    pInfo->pScreenDrawDebugFnc = NULL;
    RuntimeErrorFnc = NULL;
}
#pragma optimization_level reset

// Unloads every screen, the last one first (event 1 with one argument, 0), then marks the
// studio as no longer set up.
void UISShutdown(UISInfoT* pInfo) {
    s32 idxScreen;
    UISScreenT* pScreen;
    s32 nArg;
    UISThreadGroupInfoT data;
    u16 uGroup;
    u16 uScreen;

    pInfo->bShuttingDown = 1;
    for (idxScreen = pInfo->NumScreens - 1; idxScreen >= 0; idxScreen--) {
        pScreen = &pInfo->Screens[idxScreen];
        uScreen = pScreen->ScreenID;
        uGroup = pScreen->GroupID;
        nArg = 0;
        data.ScreenInfo.GroupID = uGroup;
        data.ScreenInfo.ScreenID = uScreen;
        UISAddThreadAction(uGroup, uScreen, pInfo, 1, &data, 1, &nArg);
        UISProcessThreadAction(pInfo, 0);
    }
    pInfo->InfoID = 0;
    pInfo->bShuttingDown = 0;
}

void UISRegisterMessageFnc(UISInfoT* pInfo, UISMessageFncT* pMessageFnc) {
    pInfo->pMessageFnc = pMessageFnc;
}

void UISRegisterScreenDrawDebugFnc(UISInfoT* pInfo, UISScreenDrawDebugFncT* pScreenDrawDebugFnc) {
    pInfo->pScreenDrawDebugFnc = pScreenDrawDebugFnc;
}

void UISRegisterResourceFncs(UISInfoT* pInfo, UISResLoadFncT* pLoadFnc, UISResUnloadFncT* pUnloadFnc) {
    pInfo->pLoadFnc = pLoadFnc;
    pInfo->pUnloadFnc = pUnloadFnc;
}

void UISRegisterTransformFncs(UISInfoT* pInfo, UISTransformFncT* pTransformFnc) {
    pInfo->pTransformFnc = pTransformFnc;
}

void UISRegisterPluginFnc(UISInfoT* pInfo, s32 PluginIndex, UISPluginFncT* pPluginFnc) {
    pInfo->Plugins[PluginIndex].pFnc = pPluginFnc;
    pInfo->NumPlugins++;
}

// Brings back a screen that is already loaded: its rate functions are finished, its first node
// is marked active again and it gets event -2 with the arguments (nParams 0xFF: the count is in
// pParams[10]). Returns 0: nothing new was loaded.
static inline s32 _UISInternalReInitScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u8 nParams,
                                           s32* pParams) {
    UISScreenT* pScreen;
    u32 i;
    u32 n;
    u8 bProcess;

    i = UISFindScreen(pInfo, GroupID, ScreenID);
    pScreen = &pInfo->Screens[i];
    UISRemoveUnNessaryRateFncs(pInfo);
    pInfo->CriticalRegions |= 4;
    n = pInfo->NumRateFncs;
    for (i = 0; i < n; i++) {
        if (pInfo->RateFncs[i].pScreen == pScreen) {
            pInfo->RateFncs[i].State = 1;
        }
    }
    pInfo->CriticalRegions &= ~4;
    UISRemoveUnNessaryRateFncs(pInfo);
    pScreen->pScrData->Controls[0].pControlInfo->IsEnabled = 1;
    bProcess = 0;
    if (nParams == 0xFF) {
        nParams = pParams[10];
    }
    pInfo->CriticalRegions |= 2;
    _ParseMaps(pInfo, pScreen, &pInfo->EventStack, 0, -1, -2, nParams, pParams, &bProcess);
    pInfo->CriticalRegions &= ~2;
    return 0;
}

// Loads a screen and makes it the newest in the table, with ParentGroupID and ParentScreenID as the
// screen to go back to. Its UI file is fixed up (event -9 when that was its first time), its
// nodes are set up and it gets event -2 with the arguments. A loaded screen is brought back
// instead. Returns 0 when the load callback gave nothing or the screen was already loaded.
s32 UISInternalLoadScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u16 ParentGroupID, u16 ParentScreenID,
                          u8 nParams, s32* pParams) {
    u32 idxScreen;
    void* pLoadScreen;
    UISScreenT* pScreen;
    s32 bNotLoaded;
    u8 bProcess;

    // fake match: nScreens and idxScreen go through s32 -> s64 -> u32 (the values are unchanged);
    // the dead high words give GroupID, ScreenID and ParentGroupID the neighbours that put them in
    // pInfo's allocation level, so they take EA's r25-r27 above pInfo's r24
    idxScreen = UISFindScreen(pInfo, GroupID, ScreenID);
    if (idxScreen < (u32)(s64)(s32)pInfo->NumScreens) {
        return _UISInternalReInitScreen(pInfo, GroupID, ScreenID, nParams, pParams);
    }
    pLoadScreen = pInfo->pLoadFnc(GroupID, ScreenID);
    if (pLoadScreen == NULL) return 0;
    pInfo->NumScreens = (u32)(s64)(s32)(pInfo->NumScreens + 1);
    pScreen = &pInfo->Screens[(u32)(s64)(s32)idxScreen];
    pScreen->GroupID = GroupID;
    pScreen->ScreenID = ScreenID;
    pScreen->ParentGroupID = ParentGroupID;
    pScreen->ParentScreenID = ParentScreenID;
    pScreen->ControllerDisable = 0;
    pScreen->bWaitingToBeUnloaded = 0;
    pScreen->pScrData = pLoadScreen;
    bNotLoaded = PatchScrData(pScreen->pScrData);
    // fake match: bNotLoaded goes through a 64-bit shift up and back down (the value is unchanged).
    // The shifts become a chain of word copies after bNotLoaded; each copy-propagation pass removes
    // one link, so bNotLoaded's copy from the call result survives them all (EA's mr r0,r3 ...
    // mr r25,r0).
    // port: relies on the conversion to s64 wrapping and on >> of a negative s64 being arithmetic.
    bNotLoaded = (s32)((s64)((u64)bNotLoaded << 32) >> 32);
    _ParseInitialize(pInfo, pScreen, 0, -1);
    pScreen->pScrData->Controls[0].pControlInfo->IsEnabled = 1;
    if (bNotLoaded) {
        bProcess = 0;
        pInfo->CriticalRegions |= 2;
        _ParseMaps(pInfo, pScreen, &pInfo->EventStack, 0, -1, -9, 0, NULL, &bProcess);
        pInfo->CriticalRegions &= ~2;
    }
    _ParseTransforms(pInfo, 0, pScreen, 0);
    bProcess = 0;
    if (nParams == 0xFF) {
        nParams = pParams[10];
    }
    pInfo->CriticalRegions |= 2;
    _ParseMaps(pInfo, pScreen, &pInfo->EventStack, 0, -1, -2, nParams, pParams, &bProcess);
    pInfo->CriticalRegions &= ~2;
    return 1;
}

// Goes to a screen. A loaded one is brought back; otherwise it is loaded, and with bModal (forced
// while ModalStack holds records) a ModalStack record is pushed for it that remembers the screen that was
// current and the new screen's first node. Returns what UISInternalLoadScreen returned.
s32 _UISInternalLoad(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u8 bModal, u8 nParams, s32* pParams) {
    s16 iActiveGroup;
    UISModalStackT* pModal;
    s16 iActiveScreen;
    s32 rVal;
    u32 bScreenLoaded;
    u32 idxModal;
    u16 idxScreen;
    s32* pArgs;

    // fake match: pArgs goes through a 64-bit shift up and back down (the value is unchanged).
    // The shifts become a chain of word copies at the entry that register allocation coalesces;
    // the merged copies stay neighbours of pInfo, which then leaves the graph last and takes
    // EA's r31 (the u16 conversion of iActiveGroup, live only after the entry, keeps EA's r22).
    // port: truncates the pointer to 32 bits; a port writes pArgs = pParams.
    pArgs = (s32*)(u32)((u64)((u64)(u32)pParams << 32) >> 32);
    idxModal = 0;
    if (pInfo->NumScreens == 0 || pInfo->ActiveScreenIdx == -1) {
        iActiveGroup = -1;
        iActiveScreen = -1;
    } else {
        iActiveGroup = pInfo->Screens[pInfo->ActiveScreenIdx].GroupID;
        iActiveScreen = pInfo->Screens[pInfo->ActiveScreenIdx].ScreenID;
    }
    bScreenLoaded = UISFindScreen(pInfo, GroupID, ScreenID) < pInfo->NumScreens;
    if (bScreenLoaded == 1) {
        return _UISInternalReInitScreen(pInfo, GroupID, ScreenID, nParams, pArgs);
    }
    if (pInfo->NumModals != 0 && !bScreenLoaded) {
        bModal = 1;
    }
    if (bModal) {
        idxModal = pInfo->NumModals;
        pModal = &pInfo->ModalStack[idxModal];
        memset(pModal, 0, sizeof(UISModalStackT));
        pModal->GroupID = GroupID;
        pModal->ScreenID = ScreenID;
        pInfo->NumModals++;
    }
    rVal = UISInternalLoadScreen(pInfo, GroupID, ScreenID, iActiveGroup, iActiveScreen, nParams, pArgs);
    if (bModal) {
        idxScreen = UISFindScreen(pInfo, GroupID, ScreenID);
        if (idxScreen < (s32)pInfo->NumScreens) {  // fake match: this compare is signed
            pModal = &pInfo->ModalStack[idxModal];
            // fake match: the screen addressed by byte offset (EA's addi r0,r5,0x10; lwzx)
            pModal->pControlInfo = ((UISScreenT*)((u8*)pInfo->Screens + idxScreen * sizeof(UISScreenT)))
                                     ->pScrData->Controls[0].pControlInfo;
            idxScreen = UISFindScreen(pInfo, iActiveGroup, iActiveScreen);
            if (idxScreen < (s32)pInfo->NumScreens) {  // fake match: this compare is signed
                pModal->pScreen = &pInfo->Screens[idxScreen];
            }
        } else {
            pInfo->NumModals--;
        }
    }
    return rVal;
}

// Makes pGlobalScriptData the studio's UI file (pGlobalScript->pScrData), fixing up its offsets unless it
// already is that file. Returns whether the file can be used; a file that cannot is dropped.
u8 UISSetGlobalScript(UISInfoT* pInfo, UISScrDataT* pGlobalScriptData) {
    u8 bLoaded;

    bLoaded = 0;
    if (pGlobalScriptData != NULL) {
        if (pGlobalScriptData != pInfo->pGlobalScript->pScrData) {
            bLoaded = PatchScrData(pGlobalScriptData);
            if (!bLoaded) {
                pGlobalScriptData = NULL;
            }
        } else {
            bLoaded = 1;
        }
    }
    pInfo->pGlobalScript->pScrData = pGlobalScriptData;
    return bLoaded;
}

// Goes to a screen: queued as event 0 while an event is being sent, at once otherwise.
// fake match: optimization level 2 for this function only, and uGroup / uScreen are copies of the
// parameters. At level 4 the extra copy-propagation passes fold uScreen into r5 before the
// argument setup, so pInfo is copied out of r3 instead of EA's `mr r8,r5` copy of uScreen.
#pragma optimization_level 2
s32 UISLoadScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, u8 nParams, s32* pParams) {
    UISThreadGroupInfoT ThreadInfo;
    u16 uGroup;
    u16 uScreen;

    uGroup = GroupID;
    uScreen = ScreenID;
    if (pInfo->CriticalRegions & 2) {
        ThreadInfo.ScreenInfo.GroupID = uGroup;
        ThreadInfo.ScreenInfo.ScreenID = uScreen;
        ThreadInfo.ScreenInfo.ParentGroupID = 0xFFFF;
        ThreadInfo.ScreenInfo.ParentScreenID = 0xFFFF;
        UISAddThreadAction(uGroup, uScreen, pInfo, 0, &ThreadInfo, nParams, pParams);
    } else {
        return _UISInternalLoad(pInfo, uGroup, uScreen, 0, nParams, pParams);
    }
    return 1;
}
#pragma optimization_level reset

// Called before a screen is unloaded. If the last ModalStack record names the screen, it is dropped:
// the screen it holds becomes current, and its paused script runs on with iRetVal on the top of its
// stack. Returns 0 when an older record names the screen, or holds it: it cannot go yet.
u8 UISInternalUnloadModal(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, s32 iRetVal) {
    s32 nModals;
    UISModalStackT* pModalStack;
    UISScreenT* pScreen;
    UISStackInfoT* pStackInfo;
    s32* pStackBase;

    nModals = pInfo->NumModals;
    if (nModals > 0) {
        pModalStack = &pInfo->ModalStack[nModals - 1];
        if (pModalStack->ScreenID == ScreenID && pModalStack->GroupID == GroupID) {
            // fake match: nModals - 1 goes through s64 and back (the stored value is unchanged); the
            // dead high word gives pInfo one neighbour more in register allocation, so it
            // takes EA's r29 above pStackInfo and pStackBase
            pInfo->NumModals = (s64)(nModals - 1);
            if (pModalStack->pScreen != NULL) {
                pInfo->ActiveScreenIdx = UISFindScreen(pInfo, pModalStack->pScreen->GroupID,
                                                       pModalStack->pScreen->ScreenID);
            } else {
                pInfo->ActiveScreenIdx = -1;
                return 1;
            }
            if (pInfo->ActiveScreenIdx < pInfo->NumScreens) {
                pStackBase = pModalStack->pRestoreStack;
                if (pStackBase != NULL) {
                    pStackInfo = pModalStack->pRestoreState;
                    *pStackInfo = pModalStack->StackState;
                    pStackInfo->pStack[-1] = iRetVal;
                    if (UISStackProcess(pInfo, pModalStack->pRestoreStack, pStackInfo, pModalStack->pScreen,
                                        pModalStack->pControlInfo) != 3) {
                        pStackInfo->pStack = pStackBase;
                    }
                }
            }
        } else {
            while (nModals-- != 0) {
                pModalStack = &pInfo->ModalStack[nModals];
                if (pModalStack->ScreenID == ScreenID && pModalStack->GroupID == GroupID) return 0;
                pScreen = pModalStack->pScreen;
                if (pScreen != NULL && pScreen->ScreenID == ScreenID && pScreen->GroupID == GroupID) return 0;
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
u8 UISInternalUnloadScreen(UISInfoT* pInfo, u16 GroupID, u16 ScreenID, s32 iRetVal) {
    UISScreenT* pScreen;
    UISScreenT* pSrc;
    UISScreenT* pOther;
    u32 idxScreen;
    u16 idxParentScreenID;
    u16 idxParentGroupID;
    u32 idx;
    UISThreadGroupInfoT ThreadInfo;
    u8 bProcess;
    u32 nRateFnc;

    idxScreen = UISFindScreen(pInfo, GroupID, ScreenID);
    if (idxScreen < pInfo->NumScreens) {
        pScreen = &pInfo->Screens[idxScreen];
        idxParentGroupID = pScreen->ParentGroupID;
        idxParentScreenID = pScreen->ParentScreenID;
        if (!UISInternalUnloadModal(pInfo, GroupID, ScreenID, iRetVal)) return 0;
        if (idxScreen == pInfo->ActiveScreenIdx) {
            pInfo->ActiveScreenIdx = -1;
        }
        for (idx = 0; idx < pInfo->NumScreens; idx++) {
            pOther = &pInfo->Screens[idx];
            if (pOther->ParentGroupID == GroupID && pOther->ParentScreenID == ScreenID) {
                pOther->ParentGroupID = idxParentGroupID;
                pOther->ParentScreenID = idxParentScreenID;
            }
        }
        bProcess = 0;
        pInfo->CriticalRegions |= 2;
        _ParseMaps(pInfo, pScreen, &pInfo->EventStack, 0, -1, -3, 0, NULL, &bProcess);
        pInfo->CriticalRegions &= ~2;
        UISThreadProcessHints(pInfo, GroupID, ScreenID);
        _ParseInitialize(pInfo, pScreen, 0, -3);
        _ParseTransforms(pInfo, 3, pScreen, 0);
        nRateFnc = pInfo->NumRateFncs;
        while (nRateFnc-- != 0) {
            if (pInfo->RateFncs[nRateFnc].pScreen == pScreen) {
                pInfo->RateFncs[nRateFnc].State = 1;
            }
        }
        UISRemoveUnNessaryRateFncs(pInfo);
        pInfo->pUnloadFnc(pScreen->GroupID, pScreen->ScreenID, pScreen->pScrData);
        pScreen->pScrData = NULL;
        pInfo->NumScreens--;
        for (; idxScreen < pInfo->NumScreens; idxScreen++) {
            pScreen = &pInfo->Screens[idxScreen];
            pSrc = &pInfo->Screens[idxScreen + 1];
            memmove(pScreen, pSrc, sizeof(UISScreenT));
            idx = pInfo->NumModals;
            while (idx-- != 0) {
                if (pInfo->ModalStack[idx].pScreen == pSrc) {
                    pInfo->ModalStack[idx].pScreen = pScreen;
                }
            }
            idx = pInfo->MaxRateFncs;
            while (idx-- != 0) {
                if (pInfo->RateFncs[idx].pScreen == pSrc) {
                    pInfo->RateFncs[idx].pScreen = pScreen;
                }
            }
            if (idxScreen + 1 == pInfo->ActiveScreenIdx) {
                pInfo->ActiveScreenIdx = idxScreen;
            }
        }
        if (pInfo->ActiveScreenIdx == -1) {
            if (pInfo->NumScreens != 0) {
                pInfo->ActiveScreenIdx = UISFindScreen(pInfo, idxParentGroupID, idxParentScreenID);
                if (pInfo->ActiveScreenIdx < pInfo->NumScreens) {
                    pScreen = &pInfo->Screens[pInfo->ActiveScreenIdx];
                } else {
                    pInfo->ActiveScreenIdx = pInfo->NumScreens - 1;
                    pScreen = &pInfo->Screens[pInfo->ActiveScreenIdx];
                }
                if (pScreen != NULL) {
                    ThreadInfo.ScreenInfo.GroupID = pScreen->GroupID;
                    ThreadInfo.ScreenInfo.ScreenID = pScreen->ScreenID;
                    UISAddThreadAction(ThreadInfo.ScreenInfo.GroupID, ThreadInfo.ScreenInfo.ScreenID, pInfo,
                                       3, &ThreadInfo, 0, NULL);
                }
            } else {
                pInfo->ActiveScreenIdx = -1;
            }
        }
    }
    return 1;
}

// Queues event 3 for a screen and runs the queue, unless an event is being sent right now.
// fake match: optimization level 1 for this function only. At level 4 the copy-propagation passes
// fold ScreenID into r5 before the argument setup, so pInfo is set after the extsh's instead of
// EA's `mr r0,r5` copy of ScreenID and `mr r5,r31` first (see UISLoadScreen).
#pragma optimization_level 1
void UISSetScreenActive(UISInfoT* pInfo, u16 GroupID, u16 ScreenID) {
    UISThreadGroupInfoT ThreadInfo;

    ThreadInfo.ScreenInfo.GroupID = GroupID;
    ThreadInfo.ScreenInfo.ScreenID = ScreenID;
    UISAddThreadAction(GroupID, ScreenID, pInfo, 3, &ThreadInfo, 0, NULL);
    if (!(pInfo->CriticalRegions & 2)) {
        UISProcessThreadAction(pInfo, 0);
    }
}
#pragma optimization_level reset

// Returns the current screen's group and screen IDs, 0xFFFF when there is no current screen.
void UISGetActiveScreen(UISInfoT* pInfo, u16* pGroupID, u16* pScreenID) {
    if (pGroupID != NULL) {
        *pGroupID = 0xFFFF;
        if (pInfo->ActiveScreenIdx != -1) {
            *pGroupID = pInfo->Screens[pInfo->ActiveScreenIdx].GroupID;
        }
    }
    if (pScreenID != NULL) {
        *pScreenID = 0xFFFF;
        if (pInfo->ActiveScreenIdx != -1) {
            *pScreenID = pInfo->Screens[pInfo->ActiveScreenIdx].ScreenID;
        }
    }
}

// Runs the queued events, makes the screen named by the last ModalStack record current, then sends
// event Channel to it (or to every screen when AllScreens is set) unless the screen has taken it
// already; with Message < 0 it is sent again.
void UISProcessEvent(UISInfoT* pInfo, u32 Channel, s32 Message, s32 nParam, void* pParam, u8 AllScreens) {
    s32 idxModals;
    u32 idxScreen;
    u32 numScreens;
    UISScreenT* pScreen;
    s32 bDontProcessEvent;
    u8 bProcess;

    UISProcessThreadAction(pInfo, 0);
    idxModals = pInfo->NumModals - 1;
    if (idxModals >= 0) {
        pInfo->ActiveScreenIdx = UISFindScreen(pInfo, pInfo->ModalStack[idxModals].GroupID,
                                               pInfo->ModalStack[idxModals].ScreenID);
    }
    if (AllScreens) {
        numScreens = pInfo->NumScreens;
        idxScreen = 0;
    } else {
        idxScreen = pInfo->ActiveScreenIdx;
        numScreens = idxScreen + 1;
        if (idxScreen == -1) return;
    }
    for (; idxScreen < numScreens; idxScreen++) {
        pScreen = &pInfo->Screens[idxScreen];
        bDontProcessEvent = pScreen->ControllerDisable & (1 << Channel);
        // EA bug: the masked bit equals 1 only for event 0, so the resend works for that event alone
        if ((bDontProcessEvent == 1 && Message < 0) || bDontProcessEvent == 0) {
            bProcess = 0;
            pInfo->CriticalRegions |= 2;
            // fake match: the (int) cast sets Channel's register (an int Channel parameter does the same)
            _ParseMaps(pInfo, pScreen, &pInfo->EventStack, 0, (int)Channel, Message, nParam, pParam,
                       &bProcess);
            pInfo->CriticalRegions &= ~2;
        }
    }
}

// Sends event Channel to the current screen, or to every screen when AllScreens is set. Event -8 skips
// a screen that is being unloaded.
void UISProcessInternalEvents(UISInfoT* pInfo, UISStackInfoT* pStackInfo, int Channel, u32 Message,
                              s32 nParam, void* pParam, u8 AllScreens) {
    u32 idxScreen;
    u32 numScreens;
    UISScreenT* pScreen;
    u8 bProcess;

    // fake match: the round trip through s8 gives back AllScreens for every u8, but the longer chain
    // before the compare lets the scheduler put the pInfo and pStackInfo copies ahead of it, as in
    // EA's code (their registers follow from that)
    if ((u8)(s32)(s8)AllScreens) {
        numScreens = pInfo->NumScreens;
        idxScreen = 0;
    } else {
        idxScreen = pInfo->ActiveScreenIdx;
        numScreens = idxScreen + 1;
        if (idxScreen == -1) return;
    }
    for (; idxScreen < numScreens; idxScreen++) {
        pScreen = &pInfo->Screens[idxScreen];
        if (Message != -8 || pScreen->bWaitingToBeUnloaded != 1) {
            bProcess = 0;
            _ParseMaps(pInfo, pScreen, pStackInfo, 0, Channel, Message, nParam, pParam, &bProcess);
        }
    }
}

// Runs the studio for NumTicks ticks, then updates every screen.
void UISDrawObjects(UISInfoT* pInfo, s32 NumTicks) {
    u32 ticks;
    u32 idxScreen;
    u32 numScreens;

    ticks = pInfo->MSPerTick * NumTicks;
    UISSetColorMultipler(1.0f, 1.0f, 1.0f, 1.0f);
    UISSetColorAdditive(0.0f, 0.0f, 0.0f, 0.0f);
    _ParseRateFncs(pInfo, ticks);
    numScreens = pInfo->NumScreens;
    for (idxScreen = 0; idxScreen < numScreens; idxScreen++) {
        _ParseObjects(pInfo, &pInfo->Screens[idxScreen], NULL, -2);
    }
}

# EA's UI Studio (Madden NFL 2003 PS2) <-> TW2004's UIS functions

Evidence for later renames (renames are parked; nothing in `src/` was changed). Madden names,
types and locals are in the `*.c.txt` files here; "M" addresses are Madden's.

## The library is the same one, a year newer

- **Every struct has TW2004's size and member offsets** (Madden type = our type):
  `UISInfoT` 0xBC = `UIStudio`; `UISScreenT` 0x14 = `UISScreen`; `UISScrDataT` 0x20 =
  `UISScreenFile`; `UISControlT` 0x14 = `UISNode`; `UISControlInfoT` 0x64 = `UISNodeInfo`;
  `UISLayerT` 0xC = `UISGroup`; `UISObjT` 8 = `UISEntry`; `UISMapT` 8 = `UISHandler`;
  `UISStringT` 0xC = `UISFileEntryC` and `UISText`; `UISRateFncT` 0x34 (with `UISAnimateDataT`
  at 0x20) = `UISRateFn`; `UISModalStackT` 0x28 = `UISRecord60`; `UISStackInfoT` 0x14 =
  `UISWordStack` / `UISFrame`; `UISLocalThreadInfoT` (0xAC in UISInfoT) = our
  `pEventBase` / `pEventTop` / `pp68`; `UISThreadGroupInfoT` (16-byte union) = `UISEventData`;
  `UISParamT` (4-byte union) = `UISWord` / the `s32` stack words; `UISControlListT` = the `p`
  list of `fn_80168918`; `UISColorVectorT` = `UISVec4`.
- Field names that correct our guesses (evidence for the audit, not applied): UISInfoT 0x04
  `CriticalRegions` (our uFlags), 0x10 `pShutdownScreenFnc`, 0x20 `pLocalizeFnc`, 0x24
  `pScreenActivatedFnc`, 0x28 `pScreenDrawDebugFnc`, 0x3C `pGlobalScript` (a `UISScreenT*`: our
  `UISCurrent` is a `UISScreenT`), 0x58-0x60 the modal stack (our p60 records), 0x8C / 0x9C
  `MultiplerFactor` / `AdditiveFactor`, 0xB8 `bShuttingDown`. UISControlInfoT 0x00 `IsVisible`,
  0x04 `IsEnabled`, 0x08 `Transform` (Offset, Pivot, Rotation, Scale vectors, `pGlobalInfo` at
  0x38, the color factors at 0x3C / 0x4C), 0x5C `idxFocusChild`, 0x60 `CanHandleMessages`.
  UISLayerT's first member is a `UISLayerInfoT*` (`{ Uint32 IsVisible; }`), not a node info.
  Our event types 0-9 are EA's `UISThreadActionT` (Load, Unload, Update, ScreenActivate,
  ScreenDeactivate, ControlActivate, ControlDeactivate, ProcessEvent, MoveScreen, HINT); the
  script opcodes are `UISStackOpCode` (`opcodes.md`).
- EA's typedefs: `Uint8/Int8/Uint16/Int16/Uint32/Int32/Float32`, `Bool` = `Uint8`, `String` =
  `char*`. `Bool` parameters and returns are 8-bit (e.g. `UISInternalUnloadScreen` returns Bool:
  our `u8 fn_80168FC8`).
- Newer in TW2004: opcodes 0x79-0x7E; `UISInternalActivateControl` uses its `pTableEntry` list and
  passes two script arguments (Madden ignores the list and passes none); a queue-or-run wrapper
  for hints (`fn_8016B09C`); a register function for `pScreenDrawDebugFnc` (`fn_80169B3C`);
  `_UISDoThreadAction`'s unload check uses the action's own group / screen (Madden: the target
  screen's, from `pLoadInfo`); no float-compare tolerance in the interpreter.
- TW2004 lays each file's functions out last-first (`-inline deferred`) and links the files in the
  order UISActionProcess, UISError, UISEvent, UISStack, UIStudio, UISUtils (Madden: UIStudio,
  UISEvent, UISStack, UISUtils, UISError, UISActionProcess). So EA's files map onto ours as:
  our `UISEvent.c` = UISActionProcess.c + UISError.c + UISEvent.c; our `UIStudio.c` =
  UISStack.c (fn_80166098) + the end of UIStudio.c; our `UISApi.c` and the first half of
  `UISScreen.c` = the rest of UIStudio.c; the rest of `UISScreen.c` = UISUtils.c.

## Pairing (TW2004 address order)

Confidence: **high** = same parameter list (count, order, types), same callees and same
struct use, and the position fits EA's reversed file order; **medium** = body matches, name
uncertain. 58 of our 60 functions pair with a Madden function; 2 are TW-only.

| ours | size | EA (Madden file) | M addr, size | conf. | evidence |
|---|---|---|---|---|---|
| fn_80165528 | 0x148 | UISProcessThreadAction (ActionProcess) + _UISDoThreadControlAction inlined | 3F9328 0xD4, 3F9278 0xB0 | high | (pInfo, Bool bControlEventsOnly); control-only mode runs actions 5/6 once; else loops _UISDoThreadAction and resets the top |
| fn_80165670 | 0x45C | _UISDoThreadAction (static) + UISAddThreadActionAt, _UISCanDoUnloadAction inlined | 3F8F60 0x318 | high | (pInfo, pLocalThreadInfo, UISParamT **pNextFrameThreadInfo); switch on the 10 actions, same callees per action |
| fn_80165ACC | 0xC4 | UISThreadProcessHints | 3F8E48 0x114 | high | (pInfo, GroupID, ScreenID) Bool; walks the actions, HINT for that screen -> UISDoHint |
| fn_80165B90 | 0xDC | UISAddThreadAction + UISAddThreadActionAt inlined | 3F8D68 0x5C, 3F8CD0 0x94 | high | (Int16 GroupID, Int16 ScreenID, pInfo, Action, pInputThreadInfo, nParms, pParms): our order exactly |
| fn_80165C6C | 0x8 | UISRegisterRuntimeErrorFnc (UISError) | 3F8CC8 0x8 | high | sets the global; `lbl_80282A28` = EA's `RuntimeErrorFnc` (`UISRuntimeErrorFncT*`) |
| fn_80165C74 | 0xB8 | UISRemoveUnNessaryRateFncs (UISEvent) | 3F5CA0 0xEC | high | (pInfo); locals nRateFnc, nSlideFnc |
| fn_80165D2C | 0x64 | UISUnloadRateFnc | 3F5C50 0x4C | high | (pInfo, pControlInfo, RateFncID) |
| fn_80165D90 | 0x10C | UISLoadRateFnc | 3F5B60 0xEC | high | (pInfo, pScreen, pControlInfo, RateFncID, pFnc, MSRate) |
| fn_80165E9C | 0x1B0 | UISLoadAdvRateFnc | 3F59B8 0x1A4 | high | 10 params in our order (…, pSubControlInfo, RateFncID, pEndFnc, pAcelFnc, MSDur, Float32 targValue, animType) |
| fn_8016604C | 0x4C | UISFindRateFnc | 3F5950 0x64 | high | (pInfo, pControlInfo, RateFncID) Uint32 |
| fn_80166098 | 0x25AC | UISStackProcess (UISStack) | 3F5DB8 0x1CA0 | high | same 5 params, Int8 return, same opcode switch (`opcodes.md`) |
| fn_80168644 | 0xB4 | UISUpdateVisibility (UIStudio) | 3F5860 0x9C | high | (pInfo, pScreen, targType, pTarget, uNewVisibility) |
| fn_801686F8 | 0x220 | UISInternalActivateScreen | 3F56D8 0x188 | high | (pInfo, Bool bActivate, GroupID, ScreenID); TW's warning is at line 2942, Madden's function starts at 2923 |
| fn_80168918 | 0x268 | UISInternalActivateControl | 3F5448 0x290 | high | (pInfo, Bool bActivate, Uint32 iDir, pControlInfo, UISControlListT *pTableEntry, Uint16 GroupID, Uint16 ScreenID) |
| fn_80168B80 | 0xA4 | UISIdleProcess | 3F52B0 0xD4 | high | (pInfo, Uint32 NumTicks); locals idxScreen, numScreens, Int8 bProcess |
| fn_80168C24 | 0xB4 | UISDrawObjects | 3F51A0 0x10C | high | (pInfo, NumTicks); `ticks` = MSPerTick * NumTicks, _ParseRateFncs, _ParseObjects per screen |
| fn_80168CD8 | 0xD8 | UISProcessInternalEvents | 3F5090 0x10C | high | (pInfo, pStackInfo, Int32 Channel, Uint32 Message, nParam, pParam, Bool AllScreens) |
| fn_80168DB0 | 0x138 | UISProcessEvent | 3F4F18 0x174 | high | (pInfo, Channel, Message, nParam, pParam, Bool AllScreens) |
| fn_80168EE8 | 0x74 | UISGetActiveScreen | 3F4EA0 0x74 | high | (pInfo, Uint16 *pGroupID, Uint16 *pScreenID) |
| fn_80168F5C | 0x6C | UISSetScreenActive | 3F4E28 0x74 | high | (pInfo, GroupID, ScreenID), local ThreadInfo, queues ScreenActivate |
| fn_80168FC8 | 0x340 | UISInternalUnloadScreen | 3F49B8 0x3F8 | high | (pInfo, GroupID, ScreenID, Int32 iRetVal) Bool |
| fn_80169308 | 0x198 | UISInternalUnloadModal (static in Madden) | 3F4818 0x19C | high | (pInfo, GroupID, ScreenID, iRetVal) Bool; the modal stack |
| fn_801694A0 | 0x80 | UISLoadScreen | 3F4758 0x8C | high | (pInfo, GroupID, ScreenID, Uint8 nParams, pParams); local ThreadInfo |
| fn_80169520 | 0x70 | UISSetGlobalScript | 3F4400 0x68 | high | (pInfo, void *pGlobalScriptData) Bool; stores to pGlobalScript->pScrData |
| fn_80169590 | 0x2C8 | _UISInternalLoad (static) | 3F4550 0x204 | high | (pInfo, GroupID, ScreenID, Bool bModal, nParams, pParams) |
| fn_80169858 | 0x2B4 | UISInternalLoadScreen + _UISInternalReInitScreen inlined (our Screen_BringBack) | 3F41D8 0x228, 3F4468 0xE4 | high | (pInfo, GroupID, ScreenID, ParentGroupID, ParentScreenID, Uint8 nParams, pParams) |
| fn_80169B0C | 0x1C | UISRegisterPluginFnc | 3F41B8 0x20 | high | (pInfo, PluginIndex, pPluginFnc) |
| fn_80169B28 | 0x8 | UISRegisterTransformFncs | 3F41A8 0x8 | high | sets 0x1C |
| fn_80169B30 | 0xC | UISRegisterResourceFncs | 3F4190 0xC | high | sets 0x14, 0x18 |
| fn_80169B3C | 0x8 | (TW only) sets pScreenDrawDebugFnc (0x28) | - | - | no Madden function; field name only |
| fn_80169B44 | 0x8 | UISRegisterMessageFnc | 3F4188 0x8 | high | sets 0x0C |
| fn_80169B4C | 0xC0 | UISShutdown | 3F4108 0x80 | high | (pInfo) |
| fn_80169C0C | 0x184 | UISInit | 3F3FB8 0x150 | high | (pInfo, MaxScreens, MaxPlugins, MaxRateFncs, MaxModals, StackSize, RateStackSize, MSPerTick) |
| fn_80169D90 | 0x34 | UISGetMemSize | 3F3F78 0x40 | high | same six counts |
| fn_80169DC4 | 0x26C | PatchScrData | 3F3C98 0x2DC | high | (UISScrDataT *pNewBase) Int32: 1 fixed, -1 already fixed |
| fn_8016A030 | 0x2A4 | _ParseRateFncs (static) | 3F3978 0x31C | high | (pInfo, Uint32 MSElapsed) |
| fn_8016A2D4 | 0x23C | _ParseMaps (static) | 3F3650 0x2B4 | high | 9 params in our order, last `Int8 *bIsControlActive` |
| fn_8016A510 | 0x320 | _ParseObjects (static) | 3F3308 0x340 | high | (pInfo, pScreen, idxControl, FncID) |
| fn_8016A830 | 0x38C | _ParseTransforms (static) | 3F3160 0x124 | high | (pInfo, UISTransformAction action, pScreen, idxControl); enum Init/Push/Pop/Shutdown |
| fn_8016ABBC | 0x198 | _ParseVisibility (static) | 3F2FA0 0x1C0 | high | (pInfo, pScreen, uNewVisibility, uChangeType, pChange, Bool bFirstPass) |
| fn_8016AD54 | 0x198 | _DetermineVisibility (static) | 3F2DF0 0x1AC | high | (pScreen, void **pTarget, contextType, pContext) Int32: our "address of p" |
| fn_8016AEEC | 0x1B0 | _ParseInitialize (static) | 3F2C10 0x1DC | high | (pInfo, pScreen, idxControl, FncID) |
| fn_8016B09C | 0x5C | (TW only) queues a HINT action while busy, else UISDoHint | - | - | Madden callers queue HINT themselves |
| fn_8016B0F8 | 0x90 | UISDoHint (UISUtils) | 3F8B88 0xD4 | high | (pInfo, Hint, nParms, pParam); _UISDoThreadAction's HINT case calls it in both builds |
| fn_8016B188 | 0x34C | _ParseHints (static) | 3F8A58 0x12C | high | (pInfo, pScreen, pStackInfo, idxControl, HINT, nParam, pParam) |
| fn_8016B4D4 | 0x1E8 | UISMoveScreenDrawPosition | 3F87E8 0x26C | high | (pInfo, GroupID, ScreenID, Int32 iDir) |
| fn_8016B6BC | 0x14C | UISFindSiblingEnableControl + _IsChildOfControl, _GetFirstEnableControl inlined (our UIS_NodeLinks, UIS_LinkedOn) | 3F8748 0xA0 | high | (pScreen, pControlInfo); the inlines' parameter lists are EA's |
| fn_8016B808 | 0x3C | UISStringFormat | 3F85C8 0x28 | high | (pScrData, pString, pFormatStr, nParam, pParam) |
| fn_8016B844 | 0x698 | UISSprintf + _WriteString, _WriteHex, _WriteInt inlined (our UIS_PutString, fn_8016B844_CaseX, _CaseD) | 3F82E0 0x2E8 | high | (char *buf, int destSize, char *fmt, nParam, pParam) int |
| fn_8016BEDC | 0x280 | _WriteFloat (static) | 3F7F40 0x1D8 | high | (float fval, buf, eob, width, precision) |
| fn_8016C15C | 0x18 | UISSetColorAdditive | 3F7E38 0x1C | high | four floats; TW calls it with 0s |
| fn_8016C174 | 0x18 | UISSetColorMultipler | 3F7E18 0x1C | high | four floats; TW calls it with 1s |
| fn_8016C18C | 0xC | UISGetColorAdditive | 3F7E08 0xC | high | reversed file order |
| fn_8016C198 | 0xC | UISGetColorMultipler | 3F7DF8 0xC | high | reversed file order |
| fn_8016C1A4 | 0xCC | UISGetActionPtrValue | 3F7D28 0xD0 | high | (Uint32 Action, pControlInfo) Float32*; `UIS_ACTION_*` enum |
| fn_8016C270 | 0x354 | UISExecuteFnc | 3F7BE0 0x144 | high | 12 params in our order (…, nAppend, pAppend, int bUseChannel, Int32 Channel, UISParamT *pReturn) |
| fn_8016C5C4 | 0x50 | _UISFindHintPC (static) | 3F7B88 0x58 | high | (pControl, HintID) |
| fn_8016C614 | 0x60 | UISFindSubControlEventPC | 3F7B20 0x64 | high | (pControl, Uint16 idxControl, EventID) |
| fn_8016C674 | 0x50 | UISFindEventPC | 3F7AC8 0x58 | high | (pControl, EventID) |
| fn_8016C6C4 | 0x54 | UISFindScreen | 3F7A58 0x6C | high | (pInfo, GroupID, ScreenID); Madden's stab says Uint32, but its callers mask the result to 16 bits (`andi 0xffff`), as TW's u16 does: the header prototype likely returns Uint16 |

Madden functions with no TW2004 function (inlined, or not linked into TW2004): UISAddThreadActionAt,
_UISCanDoUnloadAction, _UISDoThreadControlAction, _UISPatchFncPC (the interpreter's script
addresses; TW has no separate copy), _UISKillRateFncsInScreen, _UISInternalReInitScreen,
_IsChildOfControl, _GetFirstEnableControl, _WriteInt, _WriteHex, _WriteString (all inlined, see
above), and UISSetScreenParent, UISRegisterShutdownFnc, UISRegisterScreenActivatedFnc,
UISPopupScreen, UISUnloadScreen, UISFindStaticObject, UISIsActiveScreenEnabled,
UISIsShuttingDown, UISAreEventsEnabled (absent: unused by TW, or inlined by a caller).

## Leads for the open functions

EA's locals are listed in declaration order; register notes are about CodeWarrior's allocation
only as hypotheses to try. gcc 2 kept each C variable as one symbol, so the lists are exact.

### fn_80166098 = UISStackProcess (98.75%)

1. **EA declares only four function-level locals**: `UISParamT *pStack; Uint8 OpCode;
   UISThreadGroupInfoT ThreadInfo; Int32 iCompare;` Every other variable is declared in the
   case's own `{ }` block (full list per opcode in `opcodes.md`). Ours has ~30 function-level
   locals (`u, n, n2, i, j, k, f, pText, pRec, nArgs, uGroup, uScreen, word, uByte0-3, nByte0-3`,
   plus the pA1-pA3 fakes). Earlier lanes found that moving single cases to block locals helped
   (0x59, 0x5B.., 0x63); the complete EA set is now known. MWCC numbers variables in
   declaration order, which sets the allocator's tie order (the ledger's pTop r6 vs EA r7).
2. The immediate reads: EA declares `Uint32 Word, a, b, c, d;` (or `Float32 Value, a, b, c, d`)
   **inside each case** that reads one (PUSH_INT/STR, PUSH_FLT, GET_D_THIS / ADDR_D_THIS,
   PUT_D_THIS, CALOFFSET_THIS (as Int32), VISIBILITY_CHANGE, the element cases, CALL (a-d only)),
   i.e. a byte-at-a-time read into a, b, c, d then `Word = (a << 24) | (b << 16) | (c << 8) | d`.
   Ours shares function-level uByte0-3 / nByte0-3. The Int32 variant (CALOFFSET_THIS) and the
   Uint32 ones explain why some of our reads need masked u8 copies.
3. Case-level types EA used: LOAD_SCREEN(_PARAMS) `Uint32 PackID; Uint8 nParams`; UNLOAD_SCREEN
   `Uint32 PackID; Uint16 GroupID, ScreenID; Int32 iValue`; SET_SCREEN_ACTIVE `PackID, GroupID,
   ScreenID`; ADVRATEFNC `pInternControlInfo, pSubControlInfo, Uint32 nParams, RateFncID, Type,
   Float32 Value, Uint32 MSTime, Uint8 *pEndFnc, *pAcelFnc`; DEACTIVECONTROL `pControlInfo,
   Int32 *PrevTableOffset, UISControlListT *pPrevTable, Int32 TableEntry`; ACTIVECONTROL
   `pControlInfo2, Int32 iDir, Int32 *NextTableOffset, UISControlListT *pNextTable, Int32
   TableEntry` (TableEntry on gcc's stack: likely address-taken, the `&nEntry` our 0x47 passes);
   SWAPSTACK `Int32 iTemp`; DOMODAL(_PARAMS) `PackID, Uint32 idxModal, Uint8 nParams,
   UISModalStackT *pModal`; the array cases `int dim` (or `int i`), `UISParamT *pArrayHeader`,
   `Int32 offset, idxSize, nDimens, consume` (+ nested `{Word,a,b,c,d}` and `{Int32 dimIndex,
   dimSize}` blocks).
4. Script addresses go through `static Uint8 *_UISPatchFncPC(UISInfoT *pInfo, UISScrDataT
   *pScrData, Uint32 uOffset)`: `uOffset & 0x80000000` ? `pInfo->pGlobalScript->pScrData +
   (uOffset & 0x7FFFFFFF)` : `(Uint8*)pScrData + uOffset` (M 3F5D90, a real call in Madden). Our
   case 0x58 writes it out with temporaries ("as from an inline helper"). Since CW does not inline
   helpers into this function (ledger), TW2004's EA source probably has it as a macro or written
   out; the exact expression order above is EA's.
5. The event cases fill `ThreadInfo` through its union members (`ThreadInfo.ScreenInfo.GroupID =
   PackID; .ScreenID = PackID >> 16; .ParentGroupID = pScreen->GroupID; ...`,
   `ThreadInfo.ActivateInfo.pControlInfo`, `.ScreenID` / `.GroupID` at 0xC / 0xE) and call
   `UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, <action>, &ThreadInfo,
   nParams, pStack - nParams)`. Our pA1-pA3 "event-word address" fakes stand for those member
   stores (`&data.aw[1]` = `&ThreadInfo.ScreenInfo.ScreenID`).

### fn_80168918 = UISInternalActivateControl (98.47%)

1. Parameters: `(UISInfoT *pInfo, Bool bActivate, Uint32 iDir, UISControlInfoT *pControlInfo,
   UISControlListT *pTableEntry, Uint16 GroupID, Uint16 ScreenID)`. Ours types iDir `s32` and the
   list `s32*`, and names the last two uScreen, uGroup: EA calls `UISFindScreen(pInfo, <7th>,
   <6th>)` and the thread action passes `(ActivateInfo.ScreenID, ActivateInfo.GroupID)`, two
   swaps that cancel (an EA naming slip; our names follow the data).
2. EA's locals (Madden, in order): `Uint32 idxControl; Uint32 nControls; Uint32 idxScreen;
   UISScreenT *pScreen; void *pcEvent; UISControlT *pControls; UISControlT *pParentControls;
   UISControlT *pTarget; UISControlInfoT *pOldEnabled; UISMapT *pMap; Int32 idxMap; Uint32
   idxEvent;` Ours: nNode(=idxControl), pScreen, pLinkNode(=pParentControls), pNode(=pTarget),
   nSlot, pInfoAlias, pFind, nEventArg, nEvent(=idxEvent), pFile, nIndex(=idxScreen), pOther
   (=pOldEnabled), pCheck, i(=idxMap), pScript(=pcEvent), n(=nControls), aArgs.
   Concrete differences: (a) EA keeps `pControls` (= the screen data's `Controls` array), not
   the file pointer (our pFile) and indexes `pControls[idx]`; (b) EA has a `UISMapT *pMap`
   cursor in the link search where ours indexes `pCheck->pHandlers[i]`; (c) **one `Uint32
   idxEvent`** (`bActivate == TRUE ? -6 : -7` stored unsigned), where ours has `s32 nEvent` plus
   the `u32 nEventArg` copy (a fake); (d) `idxScreen` is `Uint32` (ours `u16 nIndex`); (e) the
   script PC is `void *pcEvent`; (f) EA's order puts idxControl, nControls, idxScreen first.
   TW2004's version adds the pTableEntry slot search and the two script arguments (Madden passes
   `iDir` as the call's channel instead), so expect one or two more locals there (a slot index,
   a 2-word `UISParamT` argument array) that Madden does not show.
3. Madden's code order (gcc): find screen; `idxEvent` (movz) before the control loop; find
   pTarget; find pParentControls (outer `while (nControls--)`, inner `for (idxMap = 0; idxMap <
   NumMaps; idxMap++)` with `break`); `if (bActivate == TRUE) { pOldEnabled =
   UISFindSiblingEnableControl(pScreen, pControlInfo); if (pOldEnabled) UISInternalActivateControl
   (pInfo, FALSE, 0, pOldEnabled, NULL, GroupID, ScreenID); }`; `pcEvent = UISFindEventPC(pTarget,
   idxEvent)`; `pTarget->pControlInfo->IsEnabled = bActivate`; execute; then the parent's
   `UISFindSubControlEventPC(pParentControls, (Uint16)idxControl, idxEvent)`. Same as ours.

### fn_80165670 = _UISDoThreadAction (96.95%)

1. Signature `static UISParamT *_UISDoThreadAction(UISInfoT *pInfo, UISParamT *pLocalThreadInfo,
   UISParamT **pNextFrameThreadInfo)`; locals in order: `UISThreadActionT Action;
   UISThreadGroupInfoT *pLoadInfo; Int32 nParms; UISParamT *pParms; Uint16 GroupID; Uint16
   ScreenID; UISScreenT *pScreen;` and one empty `{ }` block. Ours: nType, pData, nArgs, pArgs,
   uA, uB, **nIndex**, pScreen: the same order except our extra `u32 nIndex` (EA has none: the
   UISFindScreen result is compared and used in place, 16-bit masked) and `UISThreadActionT`
   (an enum, i.e. int) for the action.
2. EA walks the **parameter itself** one slot at a time (Madden: `Action = *p; p--; GroupID; p--;
   ScreenID; p -= 5 -> pLoadInfo = p; p--; nParms = *p; p -= nParms; pParms = p; p--`, returning
   p). Our `pTop -= 7; pTop -= 1; pTop -= nArgs` is the same walk in larger steps.
3. The re-queue is `*pNextFrameThreadInfo = UISAddThreadActionAt(pInfo, *pNextFrameThreadInfo,
   pLoadInfo, GroupID, ScreenID, Action, nParms, pParms)`: the inline takes the top **by value**
   and returns the new top (8 params, pInfo unused; locals `UISThreadGroupInfoT *pLoadInfo; Int32
   idxParams`, loop `for (idxParams = nParms - 1; idxParams >= 0; idxParams--)` guarded by
   `if (pParms)`), where our UISEvent_Push takes `s32** ppTop`. EA's store pattern (TW asm): Action
   at the top, Int16 GroupID / ScreenID below, the 16-byte info at top-7..-4 (top-3 unused), nParms
   at top-8, parameters below.
4. The unload check is `static Bool _UISCanDoUnloadAction(Uint16 GroupID, Uint16 ScreenID,
   UISInfoT *pInfo, UISParamT *pLocalThreadInfo)` with locals `UISThreadActionT Action; Int32
   nParms;` only (no copies of the IDs: ours has uThisA, uThisB). The IDs are its first two
   parameters: the TW prologue's `mr r4,r28; mr r3,r27` into the loop (which the O3 note in the
   ledger reproduces) are the inline's two Uint16 parameters.

### fn_80169DC4 = PatchScrData (99.39%)

1. `Int32 PatchScrData(UISScrDataT *pNewBase)`: locals in order `Uint32 idxControl; UISControlT
   *pControl; Uint32 idxLayer; Uint32 idxObj; UISObjT *pObj; Uint32 idxMap; UISMapT *pMap; Uint32
   idxStr; Uint32 idxPatch;` and, inside the patch-table loop, `{ UISStringT **patchAddr; }`.
2. Against ours: **no layer pointer local** (ours `pGroup`; EA writes `pControl->Layers[idxLayer]`
   each time); `pObj` serves the layer objects and, by the local list, the static objects too (our
   start loop indexes `pFile->pStart[i]`); a separate counter for the strings (idxStr: our i1) and
   the patch table (idxPatch: our i2); the patch address is a block-local `UISStringT **`
   (ours `u32* pLink`, function level). EA has **no integer base locals**: our nBase1-nBase6 are
   fakes; the fix itself is probably a cast expression or macro (Madden's code is a plain
   `base + field` add for every pointer).
3. Loop shapes (Madden): controls counted up from 0 (`idxControl < NumControls`); layers, objects,
   maps, strings, patches and static objects counted down (`while (idx--)` style); an object is
   fixed only when `PluginIndex != 0xFFFF` (then `bInitialized = 0`), a map only when `EventID !=
   0xFFFF`, a static object only when `pData != NULL`; a patch entry `*patchAddr < NumStrings` ?
   `&Strings[*patchAddr]` : NULL. Returns 1, or -1 when `Controls` is already above the base.

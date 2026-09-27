# Madden NFL 2003 PS2 prototype: EA's UI Studio source evidence (b3, 2026-09-27)

The prototype's `SLUS_205.29` (ee-gcc, not stripped) carries full STABS debug info in `.mdebug`
for EA Tiburon's UI Studio library, `Source/Common/UIStudio\`: UIStudio.c (40 functions),
UISEvent.c (5), UISStack.c (2: the interpreter UISStackProcess and _UISPatchFncPC), UISUtils.c
(23), UISError.c (1), UISActionProcess.c (7): 78 functions with EA's names, parameter names and
types, every local in declaration order (with lexical blocks), and all the library's types.
Extracted with the new `tools/ref/mdebug.py`; derived text in
`docs/reference-builds/madden2003-ps2/` (README, one file per source file, `opcodes.md`,
`pairing.md`).

- **Same library as TW2004's, a year newer.** Every struct matches our sizes and offsets
  (UISInfoT 0xBC = UIStudio, UISControlT = UISNode, UISControlInfoT 0x64 = UISNodeInfo,
  UISRateFncT 0x34, UISModalStackT 0x28 = UISRecord60, UISStackInfoT 0x14 = UISWordStack, ...).
  Our event types 0-9 are EA's UISThreadActionT, the script opcodes EA's UISStackOpCode (120
  named). TW2004 adds opcodes 0x79-0x7E, the pTableEntry search in UISInternalActivateControl, a
  HINT queue wrapper and a draw-debug register function.
- **58 of our 60 UIS functions pair** with a Madden function, all high confidence (same parameter
  lists, callees, struct use, and EA's file order reversed as TW2004's deferred build emits it);
  fn_80169B3C and fn_8016B09C are TW-only. Our file split maps as: our UISEvent.c =
  UISActionProcess.c + UISError.c + UISEvent.c; fn_80166098 = UISStack.c; the rest of our
  UIStudio.c, UISApi.c and half of UISScreen.c = UIStudio.c; the rest of UISScreen.c = UISUtils.c.
- Leads for the four open functions (details in `pairing.md`):
  - fn_80166098 (UISStackProcess): EA has only 4 function-level locals (`pStack, OpCode,
    ThreadInfo, iCompare`); all others are case-block locals (list per opcode in `opcodes.md`),
    including a fresh `Uint32 Word, a, b, c, d` in every case that reads an immediate. Script
    addresses go through `_UISPatchFncPC(pInfo, pScrData, uOffset)`.
  - fn_80168918 (UISInternalActivateControl): EA's 12 locals (`idxControl, nControls, idxScreen,
    pScreen, pcEvent, pControls, pParentControls, pTarget, pOldEnabled, pMap, idxMap, idxEvent`):
    one `Uint32 idxEvent` (ours nEvent + nEventArg), `Uint32 idxScreen` (ours u16), a `pControls`
    array local instead of the file, a `UISMapT *pMap` cursor.
  - fn_80165670 (_UISDoThreadAction): EA's locals are ours in the same order minus our `nIndex`;
    the inlined helpers are `UISAddThreadActionAt(pInfo, top by value, ...)` returning the new
    top (ours passes `s32**`) and `_UISCanDoUnloadAction(Uint16 GroupID, Uint16 ScreenID, pInfo,
    p)` with only `Action, nParms` locals (ours has two ID copies).
  - fn_80169DC4 (PatchScrData): EA's 9 locals + a block-local `UISStringT **patchAddr`; no layer
    pointer, no integer base locals (our nBase1-6 are fakes); `pObj` reused for the static objects.
- Evidence for the parked renames / audit: EA's names for all 58 pairs, the struct member names
  (e.g. UISControlInfoT starts `IsVisible, IsEnabled`; UISInfoT 0x3C is `pGlobalScript`, a screen;
  `lbl_80282A28` is `RuntimeErrorFnc`), and a naming slip: EA's UISInternalActivateControl calls
  `UISFindScreen(pInfo, ScreenID, GroupID)` and is passed the IDs swapped, so the two swaps cancel.

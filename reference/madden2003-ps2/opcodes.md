# UISStackProcess (Madden NFL 2003 PS2): opcodes and their case-local variables

EA's script interpreter, `Int8 UISStackProcess(UISInfoT *pInfo, UISParamT *pBeginStack,
UISStackInfoT *pStackState, UISScreenT *pScreen, UISControlInfoT *pControlInfo)` in
`Source/Common/UIStudio\UISStack.c` (TW2004: `fn_80166098`, the same parameter order). Its
function-level locals, in EA's declaration order, are only:

```c
UISParamT *pStack;              // the value-stack top (our pTop)
Uint8 OpCode;                   // our uOp
UISThreadGroupInfoT ThreadInfo; // 16 bytes, handed to UISAddThreadAction (our UISEventData data)
Int32 iCompare;
```

Everything else is declared inside the case that uses it, in a `{ }` block. The table maps each
opcode (the jump table at 0x50ECE0, opcode - 1 indexed, 120 entries) to its code and to the
variables of the block that code lies in, in EA's declaration order. Opcode names are EA's
(`UISStackOpCode`, an anonymous enum typedef in the same file). Rows are in code (= source)
order; gcc moved a few blocks' code (zero-length ranges), those are placed by source order and
marked. The enum's full definition is in `UISStack.c.txt`.

Repeated shapes: `Uint32 Word, a, b, c, d` is EA's big-endian 32-bit immediate read (four bytes into
a, b, c, d, then `Word` combined), declared again in every case that reads one (CALL declares only
a, b, c, d). Each `*_ELEMENT_*` family shares one block.

| code | opcode(s) | EA's case-local variables |
|---|---|---|
| 0x3F5E40 | UIS_LOAD_SCREEN (0x02), UIS_LOAD_SCREEN_PARAMS (0x71) | Uint32 PackID, Uint8 nParams |
| 0x3F5EE8 | UIS_UNLOAD_SCREEN (0x03) | Uint32 PackID, Uint16 GroupID, Uint16 ScreenID, Int32 iValue |
| 0x3F5F3C | UIS_SET_SCREEN_ACTIVE (0x08) | Uint32 PackID, Uint16 GroupID, Uint16 ScreenID |
| 0x3F5F88 | UIS_ADVRATEFNC (0x58) | UISControlInfoT *pInternControlInfo, UISControlInfoT *pSubControlInfo, Uint32 nParams, Uint32 RateFncID, Uint32 Type, Float32 Value, Uint32 MSTime, Uint8 *pEndFnc, Uint8 *pAcelFnc |
| 0x3F603C | UIS_NOOP (0x01), UIS_ADDR_D (0x6A) | (none) |
| 0x3F6044 | UIS_LOAD_RATEFNC (0x06) | UISControlInfoT *pControlInfo, Uint32 MSRate |
| 0x3F6098 | UIS_UNLOAD_RATEFNC (0x07) | UISControlInfoT *pControlInfo |
| 0x3F60BC | UIS_PROCESS_OBJECT (0x0A) | Uint32 nParam, Uint32 PluginID, Uint32 ProcessID |
| 0x3F6100 | UIS_HINT (0x76) | UISParamT *pParam, Uint32 Hint, Uint32 nParam |
| 0x3F613C | UIS_SEND_MESSAGE (0x0B) | Uint8 *pSavedPC, UISParamT *pParam, Uint32 GameMessage, UISParamT *pReturn, Uint32 nParam |
| 0x3F6180 | UIS_PROCESS_EVENT (0x0C) | Uint32 nParams |
| 0x3F61C0 | UIS_BASE_ADDR (0x0D) | (none) |
| 0x3F61C8 | UIS_STRNCPY (0x0E) | UISStringT *dst, UISStringT *src, size_t num |
| 0x3F620C | UIS_PUSH_STR (0x0F), UIS_PUSH_INT (0x10) | Uint32 Word, Uint32 a, Uint32 b, Uint32 c, Uint32 d |
| 0x3F6270 | UIS_PUSH_FLT (0x11) | Float32 Value, Uint32 a, Uint32 b, Uint32 c, Uint32 d |
| 0x3F62D8 | UIS_CAST_INT (0x12) | (none) |
| 0x3F62EC | UIS_CAST_FLT (0x13) | (none) |
| 0x3F6300 | UIS_CAST_INT_1 (0x14) | (none) |
| 0x3F6314 | UIS_CAST_FLT_1 (0x15) | (none) |
| 0x3F6328 | UIS_GET_D (0x18) | (none) |
| 0x3F6338 | UIS_PUT_D (0x19) | (none) |
| 0x3F6358 | UIS_ADDR_S (0x69) | (none) |
| 0x3F6370 | UIS_GET_S (0x1A) | (none) |
| 0x3F638C | UIS_PUT_S (0x1B) | Int32 Offset, Int32 *pValue |
| 0x3F63BC | UIS_BAND (0x1C) | Int32 A, Int32 B |
| 0x3F63E0 | UIS_BOR (0x1D) | Int32 A, Int32 B |
| 0x3F6404 | UIS_BNEG (0x1E) | Int32 A |
| 0x3F641C | UIS_LAND (0x1F) | Int32 A, Int32 B |
| 0x3F644C | UIS_LOR (0x20) | Int32 A, Int32 B |
| 0x3F648C | UIS_LNOT (0x21) | Int32 A |
| 0x3F64A4 | UIS_ABS (0x22) | Int32 Value |
| 0x3F64BC | UIS_ABS_F (0x23) | Float32 Value |
| 0x3F64D4 | UIS_NEG (0x24) | (none) |
| 0x3F64E4 | UIS_ADD (0x25) | (none) |
| 0x3F64F8 | UIS_SUB (0x26) | (none) |
| 0x3F6510 | UIS_MUL (0x27) | (none) |
| 0x3F6524 | UIS_DIV (0x28) | (none) |
| 0x3F6548 | UIS_NEG_F (0x29) | Float32 A |
| 0x3F6560 | UIS_ADD_F (0x2A) | (none) |
| 0x3F6578 | UIS_SUB_F (0x2B) | (none) |
| 0x3F6590 | UIS_MUL_F (0x2C) | (none) |
| 0x3F65A8 | UIS_DIV_F (0x2D) | (none) |
| 0x3F65C0 | UIS_INC (0x2E) | (none) |
| 0x3F65D0 | UIS_DEC (0x2F) | (none) |
| 0x3F65E4 | UIS_INC_F (0x30) | (none) |
| 0x3F65FC | UIS_DEC_F (0x31) | (none) |
| 0x3F6618 | UIS_GE (0x32) | (none) |
| 0x3F6630 | UIS_LE (0x33) | (none) |
| 0x3F6648 | UIS_GT (0x34) | (none) |
| 0x3F665C | UIS_LT (0x35) | (none) |
| 0x3F6670 | UIS_EQ (0x36) | (none) |
| 0x3F6688 | UIS_NE (0x37) | (none) |
| 0x3F66B4 | UIS_GE_F (0x38) | (none) |
| 0x3F66E4 | UIS_LE_F (0x39) | (none) |
| 0x3F6714 | UIS_GT_F (0x3A) | (none) |
| 0x3F6744 | UIS_LT_F (0x3B) | (none) |
| 0x3F6774 | UIS_EQ_F (0x3C) | (none) |
| 0x3F67A8 | UIS_NE_F (0x3D) | (none) |
| 0x3F67DC | UIS_BRA_TRUE (0x3E) | Uint32 Offset, Uint32 Condition |
| 0x3F6800 | UIS_BRA_FALSE (0x3F) | Uint32 Offset, Uint32 Condition |
| 0x3F6824 | UIS_JUMP (0x40) | Uint32 Offset |
| 0x3F683C | UIS_PUSH (0x41) | (none) |
| 0x3F6858 | UIS_CALL (0x43) | Uint32 a, Uint32 b, Uint32 c, Uint32 d |
| 0x3F68C8 | UIS_RET (0x44) | Uint32 ReturnAddr |
| 0x3F68E4 | UIS_GOTONEXTSCREEN (0x45) | Uint32 PackID, Int32 *pScreenTable, Int32 *ScreenOffset, Uint8 nParams |
| 0x3F69C8 | UIS_DEACTIVECONTROL (0x46) | UISControlInfoT *pControlInfo, Int32 *PrevTableOffset, UISControlListT *pPrevTable, Int32 TableEntry |
| 0x3F6A44 | UIS_SET_CONTROL_ACTIVE (0x09) | {  } (block moved by gcc; placed by source order) |
| 0x3F6A98 | UIS_ACTIVECONTROL (0x47) | UISControlInfoT *pControlInfo2, Int32 iDir, Int32 *NextTableOffset, UISControlListT *pNextTable, Int32 TableEntry |
| 0x3F6B38 | UIS_ACTIVEPARENTSCREEN (0x48) | (none) |
| 0x3F6B68 | UIS_STR_GETLENTGH (0x49) | Uint32 len, UISStringT *pString |
| 0x3F6B8C | UIS_STR_GETCHAR (0x4A) | Uint32 idxChar, UISStringT *pString, Uint32 charValue |
| 0x3F6BD0 | UIS_STR_SETCHAR (0x4B) | Uint32 charValue, Uint32 idxChar, UISStringT *pString |
| 0x3F6C0C | UIS_STR_FORMAT (0x4C) | Uint32 nParms, UISStringT *pFormat, Uint32 iItem |
| 0x3F6CB0 | UIS_ISINGROUP (0x77) | Uint32 iGroup |
| 0x3F6CDC | UIS_GET_ACTIVE_SCREEN (0x78) | { Uint16 uGroupID, Uint16 uScreenID } (block moved by gcc; placed by source order) |
| 0x3F6D08 | UIS_UPDATESCREENS (0x4D) | Uint32 iParam, Uint32 Message |
| 0x3F6D44 | UIS_MAPLINE_DEBUGONLY (0x4E) | (none) |
| 0x3F6D58 | UIS_SWAPSTACK (0x52) | { Int32 iTemp } (block moved by gcc; placed by source order) |
| 0x3F6D78 | UIS_EVENTSON (0x4F) | (none) |
| 0x3F6D80 | UIS_EVENTSOFF (0x50) | (none) |
| 0x3F6D94 | UIS_DOMODAL (0x51), UIS_DOMODAL_PARAMS (0x70) | Uint32 PackID, Uint32 idxModal, Uint8 nParams, UISModalStackT *pModal |
| 0x3F6F00 | UIS_SET_SCREEN_OBJECT (0x54) | { UISControlInfoT *pControlInfo2 } (block moved by gcc; placed by source order) |
| 0x3F6F58 | UIS_GET_D_THIS (0x55), UIS_ADDR_D_THIS (0x6B) | Uint32 Word, Uint32 a, Uint32 b, Uint32 c, Uint32 d |
| 0x3F7010 | UIS_PUT_D_THIS (0x56) | { Uint32 Word, Uint32 a, Uint32 b, Uint32 c, Uint32 d } (block moved by gcc; placed by source order) |
| 0x3F70AC | UIS_CALOFFSET_THIS (0x57) | { Int32 Word, Int32 a, Int32 b, Int32 c, Int32 d } (block moved by gcc; placed by source order) |
| 0x3F7144 | UIS_MOD (0x5A) | (none) |
| 0x3F7168 | UIS_GET_ELEMENT_S (0x5B), UIS_GET_ELEMENT_D (0x5D), UIS_GET_ELEMENT_D_THIS (0x5F), UIS_GET_ELEMENT_ADDR (0x61), UIS_ADDR_ELEMENT_S (0x6C), UIS_ADDR_ELEMENT_D (0x6D), UIS_ADDR_ELEMENT_D_THIS (0x6E) | int dim, UISParamT *pArrayHeader, Int32 offset, Int32 idxSize, Int32 nDimens, Int32 consume |
| 0x3F7348 | UIS_PUT_ELEMENT_S (0x5C), UIS_PUT_ELEMENT_D (0x5E), UIS_PUT_ELEMENT_D_THIS (0x60), UIS_PUT_ELEMENT_ADDR (0x62) | int dim, UISParamT *pArrayHeader, Int32 offset, Int32 idxSize, Int32 nDimens, Int32 consume |
| 0x3F74F4 | UIS_PUSH_MULTIPLE (0x63) | Int32 Word, Int32 i |
| 0x3F7590 | UIS_POP_MULTIPLE (0x64) | Int32 Word |
| 0x3F75F4 | UIS_COPY_MULTIPLE (0x65) | Int32 i, Int32 Word |
| 0x3F7698 | UIS_FILL_ARRAY_S (0x66), UIS_FILL_ARRAY_D (0x67), UIS_FILL_ARRAY_D_THIS (0x68) | int i, UISParamT *pArrayHeader, Int32 nDimens, Int32 nElems, Int32 consume |
| 0x3F7808 | UIS_PATCH_STRING (0x6F) | Uint32 idxString |
| 0x3F7840 | UIS_VISIBILITY_CHANGE (0x59) | Uint32 Word, Uint32 a, Uint32 b, Uint32 c, Uint32 d |
| 0x3F78E4 | UIS_POP (0x42) | (none of its own: gcc merged its `pStack--` into the tail of the 0x59 block) |
| 0x3F78F8 | UIS_IS_TIMER_LOADED (0x73) | { UISControlInfoT *pControlInfo } (block moved by gcc; placed by source order) |
| 0x3F7934 | UIS_MOVE_SCREEN (0x72) | { Int32 iDir, Uint32 PackID } (block moved by gcc; placed by source order) |
| 0x3F799C | UIS_ENABLE_CONTROLLER (0x74) | Int32 iController, Int32 bEnabled, Int32 iMask |
| 0x3F79F0 | UIS_IS_CONTROLLER_ENABLED (0x75) | { Int32 iController, Int32 bEnabled } (block moved by gcc; placed by source order) |
| 0x3F7A24 | 0x04, 0x05, 0x16, 0x17 (not in the enum), UIS_PRINT_DEBUGONLY (0x53) | (none: the loop end / default) |

TW2004 (our `fn_80166098`) keeps this case order almost everywhere (the paired cases too: 0x02 with
0x71, 0x51 with 0x70, 0x55 with 0x6B, 0x76 after 0x0A, 0x77 / 0x78 after 0x4C, 0x52 after 0x4E,
0x6F, 0x59, 0x73, 0x72, 0x74, 0x75 at the end). Differences: 0x7E (TW) stands where Madden has
UIS_SET_CONTROL_ACTIVE (0x09, whose block is `{ }`) and TW has no case 0x09; TW's 0x6A sits
after 0x15 (Madden: shares UIS_NOOP's code); new opcodes 0x79-0x7D after the last case.
Madden's float compares add a small constant (`c.ole.s` against `[-2] + const`); TW2004's
(0x38-0x3D) do not.

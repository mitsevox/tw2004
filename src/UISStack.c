// UISStack.c (EA's file name: Madden NFL 2003's STABS, Source/Common/UIStudio\UISStack.c):
// the UI Studio's script interpreter, split from UIStudio.c.

#include "frontend/uistudio.h"

// The arguments a script's format opcode (0x4C) hands UISStringFormat: at most 20.
UISParamT uisFormatStringStack[20];

// name: Madden 2003 STABS (UISStack.c)
// fake match: EA's build inlines this into UISStackProcess, whose source here is over CW's default
// inline budget (7000), so the budget is raised (deferred inlining reads it at the end of the file).
#pragma inline_max_total_size(12000)
static inline u8* _UISPatchFncPC(UISInfoT* pInfo, UISScrDataT* pScrData, u32 uOffset) {
    u8* pFncPC;

    if ((uOffset & 0x80000000) == 0x80000000) {
        pFncPC = (u8*)pInfo->pGlobalScript->pScrData + (uOffset & 0x7FFFFFFF);
    } else {
        pFncPC = (u8*)pScrData + uOffset;
    }
    return pFncPC;
}

// Runs a screen's script from pStackState->pPC: a byte-code machine with a stack of 32-bit words
// (ints, floats and pointers) that grows up from pStackState->pStack. It stops at the script's end
// (returns 0) or when the script waits for another screen (returns 3; UISInternalUnloadModal resumes it
// from the ModalStack record it keeps). Immediates are big-endian; opcodes not listed do nothing.
// port: the stack keeps pointers in 32-bit words, like the rest of the studio.
s8 UISStackProcess(UISInfoT* pInfo, s32* pBeginStack, UISStackInfoT* pStackState, UISScreenT* pScreen,
                   UISControlInfoT* pControlInfo) {
    s32* pStack;
    u8 OpCode;
    UISThreadGroupInfoT ThreadInfo;

    while (pStackState->pPC != NULL) {
        OpCode = *pStackState->pPC;
        pStack = pStackState->pStack;
        pStackState->pPC++;
        switch (OpCode) {
        case UIS_LOAD_SCREEN:  // send event 0 (a call to screen group|screen<<16 below the arguments)
            // fake match: both opcodes share this event body.
            goto event0;
        case UIS_LOAD_SCREEN_PARAMS:
        event0: {
            u32 u;
            u8 nArgs;
            s32* pArgs;

            nArgs = *pStackState->pPC;
            pStackState->pPC++;
            pArgs = pStackState->pStack - nArgs;
            u = pArgs[-1];
            ThreadInfo.ScreenInfo.GroupID = u;
            ThreadInfo.ScreenInfo.ScreenID = u >> 16;
            ThreadInfo.ScreenInfo.ParentGroupID = pScreen->GroupID;
            ThreadInfo.ScreenInfo.ParentScreenID = pScreen->ScreenID;
            UISAddThreadAction(ThreadInfo.ScreenInfo.ParentGroupID, ThreadInfo.ScreenInfo.ParentScreenID,
                               pInfo, UISThreadAction_Load, &ThreadInfo, nArgs, pArgs);
            while (nArgs-- != 0) {
                pStackState->pStack--;
            }
            pStackState->pStack--;
            break;
        }
        case UIS_UNLOAD_SCREEN: {  // send event 1 to a screen (0xFFFF: this one) with a word
            u32 u;
            u16 uGroup;
            u16 uScreen;
            s32 n;
            u16* pA1;

            n = *--pStackState->pStack;
            u = *--pStackState->pStack;
            uGroup = u;
            uScreen = u >> 16;
            // fake match: join the event-word pointer through the existing branch.
            pA1 = (uGroup == 0xFFFF) ? (uGroup = pScreen->GroupID, uScreen = pScreen->ScreenID,
                                        &ThreadInfo.ScreenInfo.ScreenID)
                                     : &ThreadInfo.ScreenInfo.ScreenID;
            ThreadInfo.ScreenInfo.GroupID = uGroup;
            *pA1 = uScreen;
            ThreadInfo.ScreenInfo.iRetVal = n;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_Unload,
                               &ThreadInfo, 0, NULL);
            break;
        }
        case UIS_SET_SCREEN_ACTIVE: {  // send event 3 to a screen (0xFFFF: this one)
            u32 u;
            u16 uGroup;
            u16 uScreen;

            u = *--pStackState->pStack;
            uGroup = u;
            uScreen = u >> 16;
            if (uGroup == 0xFFFF) {
                uGroup = pScreen->GroupID;
                uScreen = pScreen->ScreenID;
            }
            ThreadInfo.ScreenInfo.GroupID = uGroup;
            ThreadInfo.ScreenInfo.ScreenID = uScreen;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_ScreenActivate,
                               &ThreadInfo, 0, NULL);
            break;
        }
        case UIS_ADVRATEFNC: {  // start a rate function (the byte: how many script addresses follow)
            // Script addresses: an offset into the studio's current UI file when the top bit is
            // set, otherwise into the screen's own file.
            UISControlInfoT* pInternControlInfo;
            UISControlInfoT* pSubControlInfo;
            u32 nParams;
            s32 RateFncID;
            s32 Type;
            f32 Value;
            s32 MSTime;
            u8* pEndFnc = NULL;
            u8* pAcelFnc = NULL;
            u32 u;

            nParams = *pStackState->pPC;
            pStackState->pPC++;
            pInternControlInfo = (UISControlInfoT*)*--pStackState->pStack;
            pSubControlInfo = (UISControlInfoT*)*--pStackState->pStack;
            if (nParams >= 5) {
                if (nParams >= 6) {
                    u = *--pStackState->pStack;
                    pAcelFnc = _UISPatchFncPC(pInfo, pScreen->pScrData, u);
                }
                u = *--pStackState->pStack;
                pEndFnc = _UISPatchFncPC(pInfo, pScreen->pScrData, u);
            }
            MSTime = *--pStackState->pStack;
            Value = *(f32*)--pStackState->pStack;
            Type = *--pStackState->pStack;
            RateFncID = *--pStackState->pStack;
            UISLoadAdvRateFnc(pInfo, pScreen, pInternControlInfo, pSubControlInfo, RateFncID, pEndFnc,
                              pAcelFnc, MSTime, Value, Type);
            break;
        }
        case UIS_LOAD_RATEFNC: {  // start a stepped rate function
            UISControlInfoT* pNodeInfo = (UISControlInfoT*)*--pStackState->pStack;
            s32 nU10 = *--pStackState->pStack;
            u8* pStepScript;
            s32 nId;
            u32 u;

            u = *--pStackState->pStack;
            pStepScript = _UISPatchFncPC(pInfo, pScreen->pScrData, u);
            nId = *--pStackState->pStack;
            UISLoadRateFnc(pInfo, pScreen, pNodeInfo, nId, pStepScript, nU10);
            break;
        }
        case UIS_UNLOAD_RATEFNC: {  // stop a rate function
            UISControlInfoT* pNodeInfo = (UISControlInfoT*)*--pStackState->pStack;
            s32 nId = *--pStackState->pStack;

            UISUnloadRateFnc(pInfo, pNodeInfo, nId);
            break;
        }
        case UIS_PROCESS_OBJECT: {  // call one of the game's handlers with a variable of the screen file
            s32 n;
            s32 i;
            s32 n2;
            u32* pnOffset;
            void* pVar;
            s32* pArgs;

            n = *--pStackState->pStack;
            i = *--pStackState->pStack;
            n2 = *--pStackState->pStack;
            pnOffset = (u32*)*--pStackState->pStack;
            if (pnOffset != NULL) {
                // fake match: an integer sum, offset first (EA's add order)
                pVar = (void*)(*pnOffset + (uptr)pScreen->pScrData);
            } else {
                pVar = NULL;
            }
            pArgs = pStackState->pStack - n;
            // port: the last argument is the word below the arguments, passed as an address
            pInfo->Plugins[i].pFnc(pVar, n2, n, pArgs, (s32)(pArgs - 1));
            break;
        }
        case UIS_HINT: {  // send event 9 with n words
            s32* pArgs;
            s32 n;

            n = *--pStackState->pStack;
            pStackState->pStack--;
            pArgs = pStackState->pStack - n;
            ThreadInfo.GenericInfo.Data[0] = pArgs[0];
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_HINT, &ThreadInfo,
                               n, pArgs + 1);
            break;
        }
        case UIS_SEND_MESSAGE: {  // a command to the game (pMessageFnc)
            u16 uGroup;
            u16 uScreen;
            s32* pArgs;
            s32 n;

            uGroup = pScreen->GroupID;
            uScreen = pScreen->ScreenID;
            n = *--pStackState->pStack;
            pStackState->pStack--;
            pArgs = pStackState->pStack - n;
            // port: pointers passed as the callback's words
            pInfo->pMessageFnc(pArgs[0], uGroup, uScreen, n, (s32)(pArgs + 1), (s32)(pArgs - 1));
            break;
        }
        case UIS_PROCESS_EVENT: {  // send event 7 with two words and n more
            s32 n;

            n = *--pStackState->pStack;
            ThreadInfo.MessageInfo.Message = *--pStackState->pStack;
            ThreadInfo.MessageInfo.Controller = *--pStackState->pStack;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_ProcessEvent,
                               &ThreadInfo, n, pStackState->pStack - n);
            break;
        }
        case UIS_BASE_ADDR:  // push the screen file
            *pStack = (s32)pScreen->pScrData;
            pStackState->pStack++;
            break;
        case UIS_STRNCPY: {  // copy a text
            UISStringT* pText;
            UISStringT* pFind;
            u32 u;

            pText = (UISStringT*)*--pStackState->pStack;
            pFind = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL && pFind != NULL) {
                u = pFind->length;
                if (pText->length < u) {
                    u = pText->length;
                }
                strncpy(pText->ptr, pFind->ptr, u);
                pText->ptr[u] = 0;
            }
            break;
        }
        case UIS_PUSH_STR:  // push a word
        case UIS_PUSH_INT: {
            u32 u;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *pStackState->pStack = u;
            pStackState->pStack++;
            break;
        }
        case UIS_PUSH_FLT: {  // push a float
            UISParamT word;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            word.u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *(f32*)pStackState->pStack = word.fValue;
            pStackState->pStack++;
            break;
        }
        case UIS_CAST_INT:  // float to int, top
            pStack[-1] = *(f32*)&pStack[-1];
            break;
        case UIS_CAST_FLT:  // int to float, top
            *(f32*)&pStack[-1] = pStack[-1];
            break;
        case UIS_CAST_INT_1:  // float to int, second
            pStack[-2] = *(f32*)&pStack[-2];
            break;
        case UIS_CAST_FLT_1:  // int to float, second
            *(f32*)&pStack[-2] = pStack[-2];
            break;
        case UIS_ADDR_D:  // no-op opcode
            // fake match: retain this opcode's distinct jump-table destination.
            if (OpCode) {
                break;
            }
            break;
        case UIS_GET_D:  // load through a pointer
            pStack[-1] = *(s32*)pStack[-1];
            break;
        case UIS_PUT_D:  // store through a pointer
            *(s32*)pStack[-1] = pStack[-2];
            pStackState->pStack -= 2;
            break;
        case UIS_ADDR_S:  // the address of a local
            pStack[-1] = (s32)&pStack[pStack[-1] - 1];  // port: a stack word holds the pointer
            break;
        case UIS_GET_S:  // load a local
            pStack[-1] = pStack[pStack[-1] - 1];
            break;
        case UIS_PUT_S: {  // store a local
            s32 n;

            n = pStack[-1] - 1;
            pStack[n] = pStack[-2];
            pStackState->pStack -= 2;
            break;
        }
        case UIS_BAND: {  // bitwise and
            s32 n;
            s32 n2;

            n = *--pStackState->pStack;
            n2 = *--pStackState->pStack;
            *pStackState->pStack = n & n2;
            pStackState->pStack++;
            break;
        }
        case UIS_BOR: {  // bitwise or
            s32 n;
            s32 n2;

            n = *--pStackState->pStack;
            n2 = *--pStackState->pStack;
            *pStackState->pStack = n | n2;
            pStackState->pStack++;
            break;
        }
        case UIS_BNEG: {  // bitwise not
            s32 n;

            n = *--pStackState->pStack;
            *pStackState->pStack = ~n;
            pStackState->pStack++;
            break;
        }
        case UIS_LAND: {  // logical and
            s32 n;
            s32 n2;

            n = *--pStackState->pStack;
            n2 = *--pStackState->pStack;
            *pStackState->pStack = n != 0 && n2 != 0;
            pStackState->pStack++;
            break;
        }
        case UIS_LOR: {  // logical or
            s32 n;
            s32 n2;

            n = *--pStackState->pStack;
            n2 = *--pStackState->pStack;
            *pStackState->pStack = n != 0 || n2 != 0;
            pStackState->pStack++;
            break;
        }
        case UIS_LNOT: {  // logical not
            s32 n;

            n = *--pStackState->pStack;
            *pStackState->pStack = n == 0;
            pStackState->pStack++;
            break;
        }
        case UIS_ABS: {  // int abs
            s32 n;

            n = pStack[-1];
            pStack[-1] = (n < 0) ? -n : n;
            break;
        }
        case UIS_ABS_F: {  // float abs
            f32 f;

            f = *(f32*)--pStackState->pStack;
            *(f32*)pStackState->pStack = (f < 0.0f) ? -f : f;
            pStackState->pStack++;
            break;
        }
        case UIS_NEG:  // int negate
            pStackState->pStack[-1] = -pStack[-1];
            break;
        case UIS_ADD:  // int add
            pStack[-2] = pStack[-2] + pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_SUB:  // int subtract
            pStack = pStackState->pStack;
            pStack[-2] = pStack[-2] - pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_MUL:  // int multiply
            pStack[-2] = pStack[-2] * pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_DIV:  // int divide
            pStack[-2] = pStack[-2] / pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_NEG_F: {  // float negate
            f32 f;

            f = *(f32*)--pStackState->pStack;
            *(f32*)pStackState->pStack = -f;
            pStackState->pStack++;
            break;
        }
        case UIS_ADD_F:  // float add
            *(f32*)&pStack[-2] = *(f32*)&pStack[-2] + *(f32*)&pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_SUB_F:  // float subtract
            *(f32*)&pStack[-2] = *(f32*)&pStack[-2] - *(f32*)&pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_MUL_F:  // float multiply
            *(f32*)&pStack[-2] = *(f32*)&pStack[-2] * *(f32*)&pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_DIV_F:  // float divide (0 by zero)
            if (*(f32*)&pStack[-1] != 0.0f) {
                *(f32*)&pStack[-2] = *(f32*)&pStack[-2] / *(f32*)&pStack[-1];
            } else {
                *(f32*)&pStack[-2] = 0.0f;
            }
            pStackState->pStack--;
            break;
        case UIS_INC:  // int increment
            pStack[-1] = pStack[-1] + 1;
            break;
        case UIS_DEC:  // int decrement
            pStack[-1] = pStack[-1] - 1;
            break;
        case UIS_INC_F:  // float increment
            *(f32*)&pStack[-1] += 1.0f;
            break;
        case UIS_DEC_F:  // float decrement
            *(f32*)&pStack[-1] -= 1.0f;
            break;
        case UIS_GE:  // int >=
            pStack[-2] = pStack[-1] <= pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_LE:  // int <=
            pStack[-2] = pStack[-1] >= pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_GT:  // int >
            pStack[-2] = pStack[-1] < pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_LT:  // int <
            pStack[-2] = pStack[-1] > pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_EQ:  // int ==
            pStack[-2] = pStack[-1] == pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_NE:  // int !=
            pStack[-2] = pStack[-1] != pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_GE_F:  // float >=
            pStack[-2] = *(f32*)&pStack[-1] <= *(f32*)&pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_LE_F:  // float <=
            pStack[-2] = *(f32*)&pStack[-1] >= *(f32*)&pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_GT_F:  // float >
            pStack[-2] = *(f32*)&pStack[-1] < *(f32*)&pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_LT_F:  // float <
            pStack[-2] = *(f32*)&pStack[-1] > *(f32*)&pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_EQ_F:  // float ==
            pStack[-2] = *(f32*)&pStack[-1] == *(f32*)&pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_NE_F:  // float !=
            pStack[-2] = *(f32*)&pStack[-1] != *(f32*)&pStack[-2];
            pStackState->pStack--;
            break;
        case UIS_BRA_TRUE: {  // jump by an offset if true
            s32 n;
            u32 u;

            n = *--pStackState->pStack;
            u = *--pStackState->pStack;
            if (u != 0) {
                pStackState->pPC += n;
            }
            break;
        }
        case UIS_BRA_FALSE: {  // jump by an offset if false
            s32 n;
            u32 u;

            n = *--pStackState->pStack;
            u = *--pStackState->pStack;
            if (u == 0) {
                pStackState->pPC += n;
            }
            break;
        }
        case UIS_JUMP: {  // jump by an offset
            s32 n;

            n = *--pStackState->pStack;
            pStackState->pPC += n;
            break;
        }
        case UIS_PUSH:  // duplicate the top
            pStack[0] = pStack[-1];
            pStackState->pStack++;
            break;
        case UIS_POP:  // drop the top
            pStackState->pStack--;
            break;
        case UIS_CALL: {  // call a script address, pushing the return address
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            u32 u;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *pStackState->pStack = (s32)pStackState->pPC;
            pStackState->pStack++;
            pStackState->pPC = _UISPatchFncPC(pInfo, pScreen->pScrData, u);
            break;
        }
        case UIS_RET:  // return to the popped address
            pStackState->pPC = (u8*)*--pStackState->pStack;
            break;
        case UIS_GOTONEXTSCREEN: {  // send events 0 and 3 to the screen a file word names, with n words
            u32 u;
            s32 n;
            u8 nArgs;

            nArgs = *pStackState->pPC;
            pStackState->pPC++;
            n = *--pStackState->pStack;
            u = *(u32*)(*(u32*)n + (u32)pScreen->pScrData);  // port: EA sums the pointer as a u32
            if (u != 0xFFFFFFFF) {
                // fake match: a same-value ?: on the test just made (both arms equal, no
                // compare left): this address phi keeps &ThreadInfo.ScreenInfo.ParentScreenID in a
                // register through the script loop, as in EA's build.
                u16* pA3 = (u != 0xFFFFFFFF) ? &ThreadInfo.ScreenInfo.ParentScreenID
                                             : &ThreadInfo.ScreenInfo.ParentScreenID;

                ThreadInfo.ScreenInfo.GroupID = u;
                ThreadInfo.ScreenInfo.ScreenID = u >> 16;
                ThreadInfo.ScreenInfo.ParentGroupID = pScreen->GroupID;
                *pA3 = pScreen->ScreenID;
                UISAddThreadAction(ThreadInfo.ScreenInfo.ParentGroupID, ThreadInfo.ScreenInfo.ParentScreenID,
                                   pInfo, UISThreadAction_Load, &ThreadInfo, nArgs,
                                   pStackState->pStack - nArgs);
                UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_ScreenActivate,
                                   &ThreadInfo, 0, NULL);
                while (nArgs-- != 0) {
                    pStackState->pStack--;
                }
            }
            break;
        }
        case UIS_DEACTIVECONTROL: {  // send event 6 for a node info
            s32 n;

            n = *--pStackState->pStack;
            ThreadInfo.ActivateInfo.iDir = 0;
            ThreadInfo.ActivateInfo.iProcessed = 0;
            ThreadInfo.ActivateInfo.pControlInfo = (UISControlInfoT*)n;
            ThreadInfo.ActivateInfo.pTableEntry = NULL;
            ThreadInfo.ActivateInfo.GroupID = pScreen->GroupID;
            ThreadInfo.ActivateInfo.ScreenID = pScreen->ScreenID;
            UISAddThreadAction(ThreadInfo.ActivateInfo.GroupID, ThreadInfo.ActivateInfo.ScreenID, pInfo,
                               UISThreadAction_ControlDeactivate, &ThreadInfo, 0, NULL);
            break;
        }
        case 0x7E: {  // send event 5 for a node info
            s32 n;

            n = *--pStackState->pStack;
            ThreadInfo.ActivateInfo.iDir = 0;
            ThreadInfo.ActivateInfo.iProcessed = 0;
            ThreadInfo.ActivateInfo.pControlInfo = (UISControlInfoT*)n;
            ThreadInfo.ActivateInfo.pTableEntry = NULL;
            ThreadInfo.ActivateInfo.GroupID = pScreen->GroupID;
            ThreadInfo.ActivateInfo.ScreenID = pScreen->ScreenID;
            UISAddThreadAction(ThreadInfo.ActivateInfo.GroupID, ThreadInfo.ActivateInfo.ScreenID, pInfo,
                               UISThreadAction_ControlActivate, &ThreadInfo, 0, NULL);
            break;
        }
        // send event 5 for entry n of a list of node infos, unless it is this one and set
        case UIS_ACTIVECONTROL: {
            s32 nId = *--pStackState->pStack;
            s32* pList = (s32*)*--pStackState->pStack;
            s32* pnList;
            UISControlInfoT* pEntry;
            s32 nEntry;

            nEntry = *--pStackState->pStack;
            pnList = (s32*)(*pList + (u32)pScreen->pScrData);  // port: EA sums the pointer as a u32
            if (nEntry < pnList[0]) {
                // fake match: the same for &ThreadInfo.ActivateInfo.pTableEntry.
                s32** pA2 = (nEntry < pnList[0]) ? &ThreadInfo.ActivateInfo.pTableEntry
                                                 : &ThreadInfo.ActivateInfo.pTableEntry;
                s32 nEntryOffset = pnList[nEntry + 2];
                pEntry = (UISControlInfoT*)((uptr)nEntryOffset + (uptr)pScreen->pScrData);
                if (pControlInfo != pEntry || pEntry->IsEnabled == 0) {
                    ThreadInfo.ActivateInfo.iDir = nId;
                    ThreadInfo.ActivateInfo.iProcessed = 0;
                    ThreadInfo.ActivateInfo.pControlInfo = pEntry;
                    *pA2 = pnList;
                    ThreadInfo.ActivateInfo.GroupID = pScreen->GroupID;
                    ThreadInfo.ActivateInfo.ScreenID = pScreen->ScreenID;
                    UISAddThreadAction(ThreadInfo.ActivateInfo.GroupID, ThreadInfo.ActivateInfo.ScreenID,
                                       pInfo, UISThreadAction_ControlActivate, &ThreadInfo, 1, &nEntry);
                }
            }
            break;
        }
        case UIS_ACTIVEPARENTSCREEN:  // send event 3 to the screen that made this one current
            ThreadInfo.ScreenInfo.GroupID = pScreen->ParentGroupID;
            ThreadInfo.ScreenInfo.ScreenID = pScreen->ParentScreenID;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_ScreenActivate,
                               &ThreadInfo, 0, NULL);
            break;
        case UIS_STR_GETLENTGH: {  // a text's length
            UISStringT* pText;

            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL) {
                pStackState->pStack[-1] = strlen(pText->ptr);
            } else {
                pStackState->pStack[-1] = 0;
            }
            break;
        }
        case UIS_STR_GETCHAR: {  // a text's character n
            u32 u;
            UISStringT* pText;
            s32 c = 0;

            u = *--pStackState->pStack;
            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL && u < pText->length) {
                c = pText->ptr[u];
            }
            pStackState->pStack[-1] = c;
            break;
        }
        case UIS_STR_SETCHAR: {  // set a text's character n
            s32 n;
            u32 u;
            UISStringT* pText;

            n = *--pStackState->pStack;
            u = *--pStackState->pStack;
            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL && u < pText->length) {
                pText->ptr[u] = n;
            }
            break;
        }
        case UIS_STR_FORMAT: {  // format a text (at most 20 arguments)
            s32 n;
            UISStringT* pFind;
            u32 k;
            UISStringT* pText;

            n = *--pStackState->pStack;
            for (k = 0; k < n; k++) {
                if (k < 20) {
                    uisFormatStringStack[n - k - 1].iValue = *--pStackState->pStack;
                } else {
                    pStackState->pStack--;
                }
            }
            pFind = (UISStringT*)*--pStackState->pStack;
            pText = (UISStringT*)*--pStackState->pStack;
            UISStringFormat((u32)pScreen->pScrData, pText, pFind, n, uisFormatStringStack);
            break;
        }
        case UIS_ISINGROUP: {  // whether a group is this screen's
            u32 u;

            u = *--pStackState->pStack;
            if (u == pScreen->GroupID) {
                pStackState->pStack[-1] = 1;
            } else {
                pStackState->pStack[-1] = 0;
            }
            break;
        }
        case UIS_GET_ACTIVE_SCREEN: {  // the current screen, group|screen<<16
            u16 uCurScreen;
            u16 uCurGroup;

            UISGetActiveScreen(pInfo, &uCurGroup, &uCurScreen);
            pStackState->pStack[-1] = ((u32)uCurScreen << 16) | uCurGroup;
            break;
        }
        case UIS_UPDATESCREENS: {  // send event 2 with a word
            s32 nArg;

            nArg = *--pStackState->pStack;
            ThreadInfo.GenericInfo.Data[0] = *--pStackState->pStack;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pInfo, UISThreadAction_Update,
                               &ThreadInfo, 1, &nArg);
            break;
        }
        case UIS_MAPLINE_DEBUGONLY:  // skip a word
            pStackState->pPC += 4;
            break;
        case UIS_SWAPSTACK: {  // swap the top two
            s32 n;

            n = pStackState->pStack[-1];
            pStackState->pStack[-1] = pStackState->pStack[-2];
            pStackState->pStack[-2] = n;
            break;
        }
        case UIS_EVENTSON:  // clear the screen's event mask
            pScreen->ControllerDisable = 0;
            break;
        case UIS_EVENTSOFF:  // set every bit of it
            pScreen->ControllerDisable = -1;
            break;
        case UIS_DOMODAL:  // call a screen and wait for it: the script pauses in a ModalStack record
        case UIS_DOMODAL_PARAMS: {
            u32 u;
            s32 n;
            u8 nArgs;
            UISModalStackT* pRec;
            s32* pArgs;

            nArgs = *pStackState->pPC;
            pStackState->pPC++;
            n = pInfo->NumModals;
            if (n < pInfo->MaxModals) {
                pArgs = pStackState->pStack - nArgs;
                u = pArgs[-1];
                ThreadInfo.ScreenInfo.GroupID = u;
                ThreadInfo.ScreenInfo.ScreenID = u >> 16;
                ThreadInfo.ScreenInfo.ParentGroupID = pScreen->GroupID;
                ThreadInfo.ScreenInfo.ParentScreenID = pScreen->ScreenID;
                UISAddThreadAction(ThreadInfo.ScreenInfo.ParentGroupID, ThreadInfo.ScreenInfo.ParentScreenID,
                                   pInfo, UISThreadAction_Load, &ThreadInfo, nArgs, pArgs);
                while (nArgs-- != 0) {
                    pStackState->pStack--;
                }
                pStackState->pStack--;
                pRec = &pInfo->ModalStack[n];
                pRec->StackState.pPC = pStackState->pPC;
                pRec->StackState.pStack = pStackState->pStack;
                pRec->StackState.pStackCurrent = pStackState->pStackCurrent;
                pRec->StackState.pStackEnd = pStackState->pStackEnd;
                pRec->StackState.pStackStart = pStackState->pStackStart;
                pRec->pRestoreStack = pBeginStack;
                pRec->pRestoreState = pStackState;
                pStackState->pStackCurrent = pRec->StackState.pStack;
                pRec->GroupID = ThreadInfo.ScreenInfo.GroupID;
                pRec->ScreenID = ThreadInfo.ScreenInfo.ScreenID;
                pRec->pScreen = pScreen;
                pRec->pControlInfo = pControlInfo;
                pInfo->NumModals++;
                return UISPROCESS_DOMODAL;
            }
            while (nArgs-- != 0) {
                pStackState->pStack--;
            }
            break;
        }
        case UIS_SET_SCREEN_OBJECT: {  // send event 5 for a node info
            s32 n;

            n = *--pStackState->pStack;
            ThreadInfo.ActivateInfo.iDir = 0;
            ThreadInfo.ActivateInfo.iProcessed = 0;
            ThreadInfo.ActivateInfo.pControlInfo = (UISControlInfoT*)n;
            ThreadInfo.ActivateInfo.pTableEntry = NULL;
            ThreadInfo.ActivateInfo.GroupID = pScreen->GroupID;
            ThreadInfo.ActivateInfo.ScreenID = pScreen->ScreenID;
            UISAddThreadAction(ThreadInfo.ActivateInfo.GroupID, ThreadInfo.ActivateInfo.ScreenID, pInfo,
                               UISThreadAction_ControlActivate, &ThreadInfo, 0, NULL);
            break;
        }
        case UIS_GET_D_THIS:  // push a word at a local's pointer plus an offset (0x6B: its address)
        case UIS_ADDR_D_THIS: {
            u32 uOffset;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s32 n;
            s32* pArgs;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            n = (uByte0 << 8) | uByte1;
            uOffset += pStackState->pStack[(s16)n];
            pArgs = (s32*)uOffset;
            if (OpCode == UIS_ADDR_D_THIS) {
                *pStackState->pStack = (s32)pArgs;  // port: a stack word holds the pointer
                pStackState->pStack++;
            } else {
                *pStackState->pStack = *pArgs;
                pStackState->pStack++;
            }
            break;
        }
        case UIS_PUT_D_THIS: {  // store the top at a local's pointer plus an offset
            u32 uOffset;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s32 n;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            n = (uByte0 << 8) | uByte1;
            uOffset += pStackState->pStack[(s16)n];
            *(s32*)uOffset = pStack[-1];
            pStackState->pStack--;
            break;
        }
        case UIS_CALOFFSET_THIS: {  // push a local's pointer plus an offset
            u32 uOffset;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s32 n;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            n = (uByte0 << 8) | uByte1;
            pStack = pStackState->pStack;
            uOffset += pStack[(s16)n];
            *pStack = uOffset;
            pStackState->pStack++;
            break;
        }
        case UIS_MOD:  // int remainder
            pStack[-2] = pStack[-2] % pStack[-1];
            pStackState->pStack--;
            break;
        case UIS_GET_ELEMENT_S:  // read an array element (0x6C-0x6E: its address); the array is a count of
        case UIS_GET_ELEMENT_D:  // dimensions, the dimensions, then the elements; indexes are clamped
        case UIS_GET_ELEMENT_D_THIS:
        case UIS_GET_ELEMENT_ADDR:
        case UIS_ADDR_ELEMENT_S:
        case UIS_ADDR_ELEMENT_D:
        case UIS_ADDR_ELEMENT_D_THIS: {
            s32 i;
            s32* pArr;
            s32 nIndex;
            s32 nMul;
            s32 nDims;
            s32 bOnStack;

            pArr = NULL;
            nIndex = 0;
            nMul = 1;
            bOnStack = 0;
            switch (OpCode) {
            case UIS_GET_ELEMENT_ADDR:
                bOnStack = 1;
                pArr = (s32*)pStack[pStack[-1] - 1];
                break;
            case UIS_GET_ELEMENT_S:
            case UIS_ADDR_ELEMENT_S:
                bOnStack = 1;
                pArr = &pStack[pStack[-1] - 1];
                break;
            case UIS_GET_ELEMENT_D:
            case UIS_ADDR_ELEMENT_D:
                pArr = (s32*)pStack[-1];
                bOnStack = 1;
                break;
            case UIS_GET_ELEMENT_D_THIS:
            case UIS_ADDR_ELEMENT_D_THIS: {
                u32 uOffset;
                u32 uByte0;
                u32 uByte1;
                u32 uByte2;
                u32 uByte3;
                s32 n;

                bOnStack = 0;
                uByte0 = *pStackState->pPC;
                pStackState->pPC++;
                uByte1 = *pStackState->pPC;
                pStackState->pPC++;
                uByte2 = *pStackState->pPC;
                pStackState->pPC++;
                uByte3 = *pStackState->pPC;
                pStackState->pPC++;
                uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pStackState->pPC;
                pStackState->pPC++;
                uByte1 = *pStackState->pPC;
                pStackState->pPC++;
                n = (uByte0 << 8) | uByte1;
                uOffset += pStackState->pStack[(s16)n];
                pArr = (s32*)uOffset;
                break;
            }
            }
            nDims = pArr[0];
            for (i = 1; i <= nDims; i++) {
                s32 n;
                s32 dimSize;  // name: Madden 2003 STABS

                n = pStack[-(i + bOnStack)];
                dimSize = pArr[i];
                if (n >= dimSize || n < 0) {
                    n = dimSize - 1;
                }
                nIndex += nMul * n;
                nMul *= dimSize;
            }
            if (OpCode == UIS_ADDR_ELEMENT_S || OpCode == UIS_ADDR_ELEMENT_D
                || OpCode == UIS_ADDR_ELEMENT_D_THIS) {
                // port: a stack word holds the pointer
                pStack[-(nDims + bOnStack)] = (s32)&pArr[nDims + 1 + nIndex];
            } else {
                pStack[-(nDims + bOnStack)] = pArr[nDims + 1 + nIndex];
            }
            pStackState->pStack -= nDims - (1 - bOnStack);
            break;
        }
        case UIS_PUT_ELEMENT_S:  // write an array element
        case UIS_PUT_ELEMENT_D:
        case UIS_PUT_ELEMENT_D_THIS:
        case UIS_PUT_ELEMENT_ADDR: {
            s32 i;
            s32* pArr;
            s32 nIndex;
            s32 nMul;
            s32 nDims;
            s32 bOnStack;

            pArr = NULL;
            nIndex = 0;
            nMul = 1;
            bOnStack = 0;
            switch (OpCode) {
            case UIS_PUT_ELEMENT_ADDR:
                bOnStack = 1;
                pArr = (s32*)pStack[pStack[-1] - 1];
                break;
            case UIS_PUT_ELEMENT_S:
                bOnStack = 1;
                pArr = &pStack[pStack[-1] - 1];
                break;
            case UIS_PUT_ELEMENT_D:
                pArr = (s32*)pStack[-1];
                bOnStack = 1;
                break;
            case UIS_PUT_ELEMENT_D_THIS: {
                u32 uOffset;
                u32 uByte0;
                u32 uByte1;
                u32 uByte2;
                u32 uByte3;
                s32 n;

                bOnStack = 0;
                uByte0 = *pStackState->pPC;
                pStackState->pPC++;
                uByte1 = *pStackState->pPC;
                pStackState->pPC++;
                uByte2 = *pStackState->pPC;
                pStackState->pPC++;
                uByte3 = *pStackState->pPC;
                pStackState->pPC++;
                uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pStackState->pPC;
                pStackState->pPC++;
                uByte1 = *pStackState->pPC;
                pStackState->pPC++;
                n = (uByte0 << 8) | uByte1;
                uOffset += pStackState->pStack[(s16)n];
                pArr = (s32*)uOffset;
                break;
            }
            }
            nDims = pArr[0];
            for (i = 1; i <= nDims; i++) {
                s32 n;
                s32 dimSize;  // name: Madden 2003 STABS

                n = pStack[-(i + 1 + bOnStack)];
                dimSize = pArr[i];
                if (n >= dimSize || n < 0) {
                    n = dimSize - 1;
                }
                nIndex += nMul * n;
                nMul *= dimSize;
            }
            pArr[nDims + 1 + nIndex] = pStack[-1 - bOnStack];
            pStackState->pStack -= nDims + 1 + bOnStack;
            break;
        }
        case UIS_PUSH_MULTIPLE: {  // push n copies of the top
            s32 nCount;
            s32 i;
            u8 nByte0;
            u8 nByte1;
            u8 nByte2;
            u8 nByte3;

            nByte0 = *pStackState->pPC;
            pStackState->pPC++;
            nByte1 = *pStackState->pPC;
            pStackState->pPC++;
            nByte2 = *pStackState->pPC;
            pStackState->pPC++;
            nByte3 = *pStackState->pPC;
            pStackState->pPC++;
            nCount = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            for (i = 0; i < nCount; i++) {
                pStack[i] = pStack[-1];
            }
            pStackState->pStack += nCount;
            break;
        }
        case UIS_POP_MULTIPLE: {  // drop n words
            s32 nCount;
            u8 nByte0;
            u8 nByte1;
            u8 nByte2;
            u8 nByte3;

            nByte0 = *pStackState->pPC;
            pStackState->pPC++;
            nByte1 = *pStackState->pPC;
            pStackState->pPC++;
            nByte2 = *pStackState->pPC;
            pStackState->pPC++;
            nByte3 = *pStackState->pPC;
            pStackState->pPC++;
            nCount = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            pStackState->pStack -= nCount;
            break;
        }
        case UIS_COPY_MULTIPLE: {  // push a copy of the top n words
            s32 i;
            s32 nCount;
            u8 nByte0;
            u8 nByte1;
            u8 nByte2;
            u8 nByte3;

            nByte0 = *pStackState->pPC;
            pStackState->pPC++;
            nByte1 = *pStackState->pPC;
            pStackState->pPC++;
            nByte2 = *pStackState->pPC;
            pStackState->pPC++;
            nByte3 = *pStackState->pPC;
            pStackState->pPC++;
            nCount = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            for (i = 0; i < nCount; i++) {
                pStack[i] = pStack[i - nCount];
            }
            pStackState->pStack += nCount;
            break;
        }
        case UIS_FILL_ARRAY_S:  // fill an array with a value
        case UIS_FILL_ARRAY_D:
        case UIS_FILL_ARRAY_D_THIS: {
            int i2;  // an int: EA's loop guards compare it with the s32 bound
            s32* pArr;
            s32 nDims;
            s32 nMul;
            s32 bOnStack;

            pArr = NULL;
            nMul = 1;
            bOnStack = 0;
            switch (OpCode) {
            case UIS_FILL_ARRAY_S:
                bOnStack = 1;
                pArr = &pStack[pStack[-1] - 1];
                break;
            case UIS_FILL_ARRAY_D:
                pArr = (s32*)pStack[-1];
                bOnStack = 1;
                break;
            case UIS_FILL_ARRAY_D_THIS: {
                u32 uOffset;
                u32 uByte0;
                u32 uByte1;
                u32 uByte2;
                u32 uByte3;
                s32 n;

                bOnStack = 0;
                uByte0 = *pStackState->pPC;
                pStackState->pPC++;
                uByte1 = *pStackState->pPC;
                pStackState->pPC++;
                uByte2 = *pStackState->pPC;
                pStackState->pPC++;
                uByte3 = *pStackState->pPC;
                pStackState->pPC++;
                uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pStackState->pPC;
                pStackState->pPC++;
                uByte1 = *pStackState->pPC;
                pStackState->pPC++;
                n = (uByte0 << 8) | uByte1;
                uOffset += pStackState->pStack[(s16)n];
                pArr = (s32*)uOffset;
                break;
            }
            }
            nDims = pArr[0];
            for (i2 = 1; i2 <= nDims; i2++) {
                nMul *= pArr[i2];
            }
            for (i2 = 0; i2 < nMul; i2++) {
                pArr[nDims + 1 + i2] = pStack[-1 - bOnStack];
            }
            pStackState->pStack -= bOnStack + 1;
            break;
        }
        case UIS_PATCH_STRING: {  // entry n of the screen file's third table (0 past its end)
            u32 u;

            u = pStack[-1];
            if (u < pScreen->pScrData->NumStrings) {
                pStack[-1] = (s32)&pScreen->pScrData->Strings[u];  // port: a stack word holds the pointer
            } else {
                pStack[-1] = 0;
            }
            break;
        }
        case UIS_VISIBILITY_CHANGE: {  // switch a node on or off (UISUpdateVisibility)
            u32 u;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s16 nIndex;

            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            uByte2 = *pStackState->pPC;
            pStackState->pPC++;
            uByte3 = *pStackState->pPC;
            pStackState->pPC++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            uByte1 = *pStackState->pPC;
            pStackState->pPC++;
            nIndex = (uByte0 << 8) | uByte1;
            uByte0 = *pStackState->pPC;
            pStackState->pPC++;
            u += pStackState->pStack[nIndex];  // port: the sum is a 32-bit address
            UISUpdateVisibility(pInfo, pScreen, uByte0, (void*)u, pStack[-1]);
            pStackState->pStack--;
            break;
        }
        case UIS_IS_TIMER_LOADED: {  // whether a rate function runs
            UISControlInfoT* pNodeInfo = (UISControlInfoT*)*--pStackState->pStack;
            s32 nId = *--pStackState->pStack;

            *pStackState->pStack = UISFindRateFnc(pInfo, pNodeInfo, nId) < pInfo->NumRateFncs;
            pStackState->pStack++;
            break;
        }
        case UIS_MOVE_SCREEN: {  // send event 8 to a screen with a word
            s32 n;
            u32 u;

            n = *--pStackState->pStack;
            u = *--pStackState->pStack;
            ThreadInfo.ScreenInfo.GroupID = u;
            ThreadInfo.ScreenInfo.ScreenID = u >> 16;
            ThreadInfo.ScreenInfo.ParentGroupID = pScreen->GroupID;
            ThreadInfo.ScreenInfo.ParentScreenID = pScreen->ScreenID;
            ThreadInfo.ScreenInfo.iDir = n;
            UISAddThreadAction(ThreadInfo.ScreenInfo.ParentGroupID, ThreadInfo.ScreenInfo.ParentScreenID,
                               pInfo, UISThreadAction_MoveScreen, &ThreadInfo, 0, NULL);
            break;
        }
        case UIS_ENABLE_CONTROLLER: {  // set or clear a bit of the screen's event mask (-1: all)
            s32 n;
            s32 bOn;
            u32 uBits;

            bOn = *--pStackState->pStack;
            n = *--pStackState->pStack;
            uBits = n >= 0 ? 1 << n : -1;
            if (bOn != 0) {
                pScreen->ControllerDisable |= uBits;
            } else {
                pScreen->ControllerDisable &= ~uBits;
            }
            break;
        }
        case UIS_IS_CONTROLLER_ENABLED: {  // whether a bit of the event mask is clear
            s32 n;

            n = *--pStackState->pStack;
            *pStackState->pStack = (pScreen->ControllerDisable & (1 << n)) == 0;
            pStackState->pStack++;
            break;
        }
        case 0x79: {  // a text's buffer size, as a float
            UISStringT* pText;

            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL) {
                pStackState->pStack[-1] = pText->length;
            } else {
                pStackState->pStack[-1] = 0;
            }
            break;
        }
        case 0x7A: {  // a text to upper case
            UISStringT* pText;
            u32 j;

            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL) {
                for (j = 0; j < pText->length; j++) {
                    if (pText->ptr[j] >= 'a' && pText->ptr[j] <= 'z') {
                        pText->ptr[j] -= 0x20;
                    }
                }
            }
            break;
        }
        case 0x7B: {  // a text to lower case
            UISStringT* pText;
            u32 j;

            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL) {
                for (j = 0; j < pText->length; j++) {
                    if (pText->ptr[j] >= 'A' && pText->ptr[j] <= 'Z') {
                        pText->ptr[j] += 0x20;
                    }
                }
            }
            break;
        }
        case 0x7C: {  // replace every pFind in a text by pRep
            UISStringT* pRep;
            UISStringT* pFind;
            UISStringT* pText;
            u32 nFind;
            u32 nText;
            u32 nRep;
            u32 j;
            s32 nGrow;
            u32 k;
            u8 bMatch;
            u32 nGrowCopy;

            pRep = (UISStringT*)*--pStackState->pStack;
            pFind = (UISStringT*)*--pStackState->pStack;
            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL && pFind != NULL && pRep != NULL) {
                nText = strlen(pText->ptr);
                // fake match: identity round trips keep EA's copies of the two lengths.
                nText = (u32)((s64)((u64)(u32)nText << 32) >> 32);
                nText = (u32)(u64)(u32)nText;
                nFind = strlen(pFind->ptr);
                nFind = (u32)((s64)((u64)(u32)nFind << 32) >> 32);
                nFind = (u32)(u64)(u32)nFind;
                nRep = strlen(pRep->ptr);
                // fake match: EA tests nText != 0 && nFind != 0 && nText >= nFind. One test per
                // level, each followed by a same-value ?: on the condition just tested (always
                // true there, both arms equal): the ?: temps become three codeless allocator
                // neighbours of the text pointers and lengths, which gives EA's saved registers.
                if (nText != 0) {
                    nRep = nText != 0 ? nRep : (u32)(s32)nRep;
                    if (nFind != 0) {
                        nText = nFind != 0 ? nText : (u32)(s32)nText;
                        if (nText >= nFind) {
                            nRep = nText >= nFind ? nRep : (u32)(s32)nRep;
                            nGrow = nRep - nFind;
                            // fake match: nGrowCopy is nGrow, so every `nGrow | nGrowCopy` below is
                            // nGrow. The allocator never merges an OR: it is EA's kept copy
                            // (`mr r0,r7`), which the shift loops use while the tests use nGrow.
                            nGrowCopy = nGrow;
                            for (j = 0; j < nText - nFind + 1; j++) {
                                bMatch = 1;
                                for (k = j; k < j + nFind; k++) {
                                    if (k >= pText->length) {
                                        bMatch = 0;
                                        break;
                                    }
                                    if (pText->ptr[k] != pFind->ptr[k - j]) {
                                        bMatch = 0;
                                        break;
                                    }
                                }
                                if (bMatch) {
                                    // fake match: same-value ?: on the condition just tested (j
                                    // unchanged): their temps are codeless allocator neighbours
                                    // (the text pointers' saved registers, the second loop's k).
                                    j = bMatch ? j : (u32)(s32)j;
                                    if (nGrow > 0) {
                                        for (k = pText->length - 1; k >= j + (nGrow | nGrowCopy); k--) {
                                            pText->ptr[k] = pText->ptr[k - (nGrow | nGrowCopy)];
                                        }
                                    } else if (nGrow < 0) {
                                        k = j + nRep;
                                        j = nGrow < 0 ? j : (u32)(s32)j;
                                        for (; k <= pText->length + (nGrow | nGrowCopy); k++) {
                                            pText->ptr[k] = pText->ptr[k - (nGrow | nGrowCopy)];
                                        }
                                    }
                                    for (k = j; k < j + nRep && k < pText->length; k++) {
                                        pText->ptr[k] = pRep->ptr[k - j];
                                    }
                                    j += nRep - 1;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case 0x7D: {  // hand a text to the game (pScreenDrawDebugFnc)
            UISStringT* pText;

            pText = (UISStringT*)*--pStackState->pStack;
            if (pText != NULL && pInfo->pScreenDrawDebugFnc != NULL && pScreen != NULL) {
                // port: the text's address passed as the callback's word
                pInfo->pScreenDrawDebugFnc(pScreen->GroupID, pScreen->ScreenID, (s32)pText->ptr);
            }
            break;
        }
        }
    }
    return UISPROCESS_CONTINUE;
}

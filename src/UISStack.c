// UISStack.c (EA's file name: Madden NFL 2003's STABS, Source/Common/UIStudio\UISStack.c):
// the UI Studio's script interpreter, split from UIStudio.c.

#include "frontend/uistudio.h"

// The arguments a script's format opcode (0x4C) hands UISStringFormat: at most 20.
UISParamT uisFormatStringStack[20];

// name: Madden 2003 STABS (UISStack.c)
// fake match: EA's build inlines this into UISStackProcess, whose source here is over CW's default
// inline budget (7000), so the budget is raised (deferred inlining reads it at the end of the file).
#pragma inline_max_total_size(12000)
static inline u8* _UISPatchFncPC(UISInfoT* pStudio, UISScrDataT* pData, u32 uOffset) {
    u8* pRet;

    if ((uOffset & 0x80000000) == 0x80000000) {
        pRet = (u8*)pStudio->pGlobalScript->pScrData + (uOffset & 0x7FFFFFFF);
    } else {
        pRet = (u8*)pData + uOffset;
    }
    return pRet;
}

// Runs a screen's script from pFrame->pPC: a byte-code machine with a stack of 32-bit words
// (ints, floats and pointers) that grows up from pFrame->pStack. It stops at the script's end
// (returns 0) or when the script waits for another screen (returns 3; UISInternalUnloadModal resumes it
// from the ModalStack record it keeps). Immediates are big-endian; opcodes not listed do nothing.
// port: the stack keeps pointers in 32-bit words, like the rest of the studio.
s8 UISStackProcess(UISInfoT* pStudio, s32* p, UISStackInfoT* pFrame, UISScreenT* pScreen,
                   UISControlInfoT* pInfo) {
    s32* pTop;
    u8 uOp;
    UISThreadGroupInfoT data;

    while (pFrame->pPC != NULL) {
        uOp = *pFrame->pPC;
        pTop = pFrame->pStack;
        pFrame->pPC++;
        switch (uOp) {
        case 0x02:  // send event 0 (a call to screen group|screen<<16 below the arguments)
            // fake match: both opcodes share this event body.
            goto event0;
        case 0x71:
        event0: {
            u32 u;
            u8 nArgs;
            s32* pArgs;

            nArgs = *pFrame->pPC;
            pFrame->pPC++;
            pArgs = pFrame->pStack - nArgs;
            u = pArgs[-1];
            data.ScreenInfo.GroupID = u;
            data.ScreenInfo.ScreenID = u >> 16;
            data.ScreenInfo.ParentGroupID = pScreen->GroupID;
            data.ScreenInfo.ParentScreenID = pScreen->ScreenID;
            UISAddThreadAction(data.ScreenInfo.ParentGroupID, data.ScreenInfo.ParentScreenID, pStudio, 0,
                               &data, nArgs, pArgs);
            while (nArgs-- != 0) {
                pFrame->pStack--;
            }
            pFrame->pStack--;
            break;
        }
        case 0x03: {  // send event 1 to a screen (0xFFFF: this one) with a word
            u32 u;
            u16 uGroup;
            u16 uScreen;
            s32 n;
            u16* pA1;

            n = *--pFrame->pStack;
            u = *--pFrame->pStack;
            uGroup = u;
            uScreen = u >> 16;
            // fake match: join the event-word pointer through the existing branch.
            pA1 = (uGroup == 0xFFFF)
                       ? (uGroup = pScreen->GroupID, uScreen = pScreen->ScreenID, &data.ScreenInfo.ScreenID)
                       : &data.ScreenInfo.ScreenID;
            data.ScreenInfo.GroupID = uGroup;
            *pA1 = uScreen;
            data.ScreenInfo.iRetVal = n;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 1, &data, 0, NULL);
            break;
        }
        case 0x08: {  // send event 3 to a screen (0xFFFF: this one)
            u32 u;
            u16 uGroup;
            u16 uScreen;

            u = *--pFrame->pStack;
            uGroup = u;
            uScreen = u >> 16;
            if (uGroup == 0xFFFF) {
                uGroup = pScreen->GroupID;
                uScreen = pScreen->ScreenID;
            }
            data.ScreenInfo.GroupID = uGroup;
            data.ScreenInfo.ScreenID = uScreen;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 3, &data, 0, NULL);
            break;
        }
        case 0x58: {  // start a rate function (the byte: how many script addresses follow)
            // Script addresses: an offset into the studio's current UI file when the top bit is
            // set, otherwise into the screen's own file.
            UISControlInfoT* pNodeInfo;
            s32 n30;
            u32 nArgs;
            s32 nId;
            s32 nU20;
            f32 fTarget;
            s32 nTime;
            u8* pDoneScript = NULL;
            u8* pStepScript = NULL;
            u32 u;

            nArgs = *pFrame->pPC;
            pFrame->pPC++;
            pNodeInfo = (UISControlInfoT*)*--pFrame->pStack;
            n30 = *--pFrame->pStack;
            if (nArgs >= 5) {
                if (nArgs >= 6) {
                    u = *--pFrame->pStack;
                    pStepScript = _UISPatchFncPC(pStudio, pScreen->pScrData, u);
                }
                u = *--pFrame->pStack;
                pDoneScript = _UISPatchFncPC(pStudio, pScreen->pScrData, u);
            }
            nTime = *--pFrame->pStack;
            fTarget = *(f32*)--pFrame->pStack;
            nU20 = *--pFrame->pStack;
            nId = *--pFrame->pStack;
            UISLoadAdvRateFnc(pStudio, pScreen, pNodeInfo, n30, nId, pDoneScript, pStepScript, nTime, fTarget,
                              nU20);
            break;
        }
        case 0x06: {  // start a stepped rate function
            UISControlInfoT* pNodeInfo = (UISControlInfoT*)*--pFrame->pStack;
            s32 nU10 = *--pFrame->pStack;
            u8* pStepScript;
            s32 nId;
            u32 u;

            u = *--pFrame->pStack;
            pStepScript = _UISPatchFncPC(pStudio, pScreen->pScrData, u);
            nId = *--pFrame->pStack;
            UISLoadRateFnc(pStudio, pScreen, pNodeInfo, nId, pStepScript, nU10);
            break;
        }
        case 0x07: {  // stop a rate function
            UISControlInfoT* pNodeInfo = (UISControlInfoT*)*--pFrame->pStack;
            s32 nId = *--pFrame->pStack;

            UISUnloadRateFnc(pStudio, pNodeInfo, nId);
            break;
        }
        case 0x0A: {  // call one of the game's handlers with a variable of the screen file
            s32 n;
            s32 i;
            s32 n2;
            u32* pnOffset;
            void* pVar;
            s32* pArgs;

            n = *--pFrame->pStack;
            i = *--pFrame->pStack;
            n2 = *--pFrame->pStack;
            pnOffset = (u32*)*--pFrame->pStack;
            if (pnOffset != NULL) {
                // fake match: an integer sum, offset first (EA's add order)
                pVar = (void*)(*pnOffset + (uptr)pScreen->pScrData);
            } else {
                pVar = NULL;
            }
            pArgs = pFrame->pStack - n;
            // port: the last argument is the word below the arguments, passed as an address
            pStudio->Plugins[i].pFnc(pVar, n2, n, pArgs, (s32)(pArgs - 1));
            break;
        }
        case 0x76: {  // send event 9 with n words
            s32* pArgs;
            s32 n;

            n = *--pFrame->pStack;
            pFrame->pStack--;
            pArgs = pFrame->pStack - n;
            data.GenericInfo.Data[0] = pArgs[0];
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 9, &data, n, pArgs + 1);
            break;
        }
        case 0x0B: {  // a command to the game (pMessageFnc)
            u16 uGroup;
            u16 uScreen;
            s32* pArgs;
            s32 n;

            uGroup = pScreen->GroupID;
            uScreen = pScreen->ScreenID;
            n = *--pFrame->pStack;
            pFrame->pStack--;
            pArgs = pFrame->pStack - n;
            // port: pointers passed as the callback's words
            pStudio->pMessageFnc(pArgs[0], uGroup, uScreen, n, (s32)(pArgs + 1), (s32)(pArgs - 1));
            break;
        }
        case 0x0C: {  // send event 7 with two words and n more
            s32 n;

            n = *--pFrame->pStack;
            data.MessageInfo.Message = *--pFrame->pStack;
            data.MessageInfo.Controller = *--pFrame->pStack;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 7, &data, n, pFrame->pStack - n);
            break;
        }
        case 0x0D:  // push the screen file
            *pTop = (s32)pScreen->pScrData;
            pFrame->pStack++;
            break;
        case 0x0E: {  // copy a text
            UISStringT* pText;
            UISStringT* pFind;
            u32 u;

            pText = (UISStringT*)*--pFrame->pStack;
            pFind = (UISStringT*)*--pFrame->pStack;
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
        case 0x0F:  // push a word
        case 0x10: {
            u32 u;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *pFrame->pStack = u;
            pFrame->pStack++;
            break;
        }
        case 0x11: {  // push a float
            UISParamT word;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            word.u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *(f32*)pFrame->pStack = word.fValue;
            pFrame->pStack++;
            break;
        }
        case 0x12:  // float to int, top
            pTop[-1] = *(f32*)&pTop[-1];
            break;
        case 0x13:  // int to float, top
            *(f32*)&pTop[-1] = pTop[-1];
            break;
        case 0x14:  // float to int, second
            pTop[-2] = *(f32*)&pTop[-2];
            break;
        case 0x15:  // int to float, second
            *(f32*)&pTop[-2] = pTop[-2];
            break;
        case 0x6A:  // no-op opcode
            // fake match: retain this opcode's distinct jump-table destination.
            if (uOp) {
                break;
            }
            break;
        case 0x18:  // load through a pointer
            pTop[-1] = *(s32*)pTop[-1];
            break;
        case 0x19:  // store through a pointer
            *(s32*)pTop[-1] = pTop[-2];
            pFrame->pStack -= 2;
            break;
        case 0x69:  // the address of a local
            pTop[-1] = (s32)&pTop[pTop[-1] - 1];  // port: a stack word holds the pointer
            break;
        case 0x1A:  // load a local
            pTop[-1] = pTop[pTop[-1] - 1];
            break;
        case 0x1B: {  // store a local
            s32 n;

            n = pTop[-1] - 1;
            pTop[n] = pTop[-2];
            pFrame->pStack -= 2;
            break;
        }
        case 0x1C: {  // bitwise and
            s32 n;
            s32 n2;

            n = *--pFrame->pStack;
            n2 = *--pFrame->pStack;
            *pFrame->pStack = n & n2;
            pFrame->pStack++;
            break;
        }
        case 0x1D: {  // bitwise or
            s32 n;
            s32 n2;

            n = *--pFrame->pStack;
            n2 = *--pFrame->pStack;
            *pFrame->pStack = n | n2;
            pFrame->pStack++;
            break;
        }
        case 0x1E: {  // bitwise not
            s32 n;

            n = *--pFrame->pStack;
            *pFrame->pStack = ~n;
            pFrame->pStack++;
            break;
        }
        case 0x1F: {  // logical and
            s32 n;
            s32 n2;

            n = *--pFrame->pStack;
            n2 = *--pFrame->pStack;
            *pFrame->pStack = n != 0 && n2 != 0;
            pFrame->pStack++;
            break;
        }
        case 0x20: {  // logical or
            s32 n;
            s32 n2;

            n = *--pFrame->pStack;
            n2 = *--pFrame->pStack;
            *pFrame->pStack = n != 0 || n2 != 0;
            pFrame->pStack++;
            break;
        }
        case 0x21: {  // logical not
            s32 n;

            n = *--pFrame->pStack;
            *pFrame->pStack = n == 0;
            pFrame->pStack++;
            break;
        }
        case 0x22: {  // int abs
            s32 n;

            n = pTop[-1];
            pTop[-1] = (n < 0) ? -n : n;
            break;
        }
        case 0x23: {  // float abs
            f32 f;

            f = *(f32*)--pFrame->pStack;
            *(f32*)pFrame->pStack = (f < 0.0f) ? -f : f;
            pFrame->pStack++;
            break;
        }
        case 0x24:  // int negate
            pFrame->pStack[-1] = -pTop[-1];
            break;
        case 0x25:  // int add
            pTop[-2] = pTop[-2] + pTop[-1];
            pFrame->pStack--;
            break;
        case 0x26:  // int subtract
            pTop = pFrame->pStack;
            pTop[-2] = pTop[-2] - pTop[-1];
            pFrame->pStack--;
            break;
        case 0x27:  // int multiply
            pTop[-2] = pTop[-2] * pTop[-1];
            pFrame->pStack--;
            break;
        case 0x28:  // int divide
            pTop[-2] = pTop[-2] / pTop[-1];
            pFrame->pStack--;
            break;
        case 0x29: {  // float negate
            f32 f;

            f = *(f32*)--pFrame->pStack;
            *(f32*)pFrame->pStack = -f;
            pFrame->pStack++;
            break;
        }
        case 0x2A:  // float add
            *(f32*)&pTop[-2] = *(f32*)&pTop[-2] + *(f32*)&pTop[-1];
            pFrame->pStack--;
            break;
        case 0x2B:  // float subtract
            *(f32*)&pTop[-2] = *(f32*)&pTop[-2] - *(f32*)&pTop[-1];
            pFrame->pStack--;
            break;
        case 0x2C:  // float multiply
            *(f32*)&pTop[-2] = *(f32*)&pTop[-2] * *(f32*)&pTop[-1];
            pFrame->pStack--;
            break;
        case 0x2D:  // float divide (0 by zero)
            if (*(f32*)&pTop[-1] != 0.0f) {
                *(f32*)&pTop[-2] = *(f32*)&pTop[-2] / *(f32*)&pTop[-1];
            } else {
                *(f32*)&pTop[-2] = 0.0f;
            }
            pFrame->pStack--;
            break;
        case 0x2E:  // int increment
            pTop[-1] = pTop[-1] + 1;
            break;
        case 0x2F:  // int decrement
            pTop[-1] = pTop[-1] - 1;
            break;
        case 0x30:  // float increment
            *(f32*)&pTop[-1] += 1.0f;
            break;
        case 0x31:  // float decrement
            *(f32*)&pTop[-1] -= 1.0f;
            break;
        case 0x32:  // int >=
            pTop[-2] = pTop[-1] <= pTop[-2];
            pFrame->pStack--;
            break;
        case 0x33:  // int <=
            pTop[-2] = pTop[-1] >= pTop[-2];
            pFrame->pStack--;
            break;
        case 0x34:  // int >
            pTop[-2] = pTop[-1] < pTop[-2];
            pFrame->pStack--;
            break;
        case 0x35:  // int <
            pTop[-2] = pTop[-1] > pTop[-2];
            pFrame->pStack--;
            break;
        case 0x36:  // int ==
            pTop[-2] = pTop[-1] == pTop[-2];
            pFrame->pStack--;
            break;
        case 0x37:  // int !=
            pTop[-2] = pTop[-1] != pTop[-2];
            pFrame->pStack--;
            break;
        case 0x38:  // float >=
            pTop[-2] = *(f32*)&pTop[-1] <= *(f32*)&pTop[-2];
            pFrame->pStack--;
            break;
        case 0x39:  // float <=
            pTop[-2] = *(f32*)&pTop[-1] >= *(f32*)&pTop[-2];
            pFrame->pStack--;
            break;
        case 0x3A:  // float >
            pTop[-2] = *(f32*)&pTop[-1] < *(f32*)&pTop[-2];
            pFrame->pStack--;
            break;
        case 0x3B:  // float <
            pTop[-2] = *(f32*)&pTop[-1] > *(f32*)&pTop[-2];
            pFrame->pStack--;
            break;
        case 0x3C:  // float ==
            pTop[-2] = *(f32*)&pTop[-1] == *(f32*)&pTop[-2];
            pFrame->pStack--;
            break;
        case 0x3D:  // float !=
            pTop[-2] = *(f32*)&pTop[-1] != *(f32*)&pTop[-2];
            pFrame->pStack--;
            break;
        case 0x3E: {  // jump by an offset if true
            s32 n;
            u32 u;

            n = *--pFrame->pStack;
            u = *--pFrame->pStack;
            if (u != 0) {
                pFrame->pPC += n;
            }
            break;
        }
        case 0x3F: {  // jump by an offset if false
            s32 n;
            u32 u;

            n = *--pFrame->pStack;
            u = *--pFrame->pStack;
            if (u == 0) {
                pFrame->pPC += n;
            }
            break;
        }
        case 0x40: {  // jump by an offset
            s32 n;

            n = *--pFrame->pStack;
            pFrame->pPC += n;
            break;
        }
        case 0x41:  // duplicate the top
            pTop[0] = pTop[-1];
            pFrame->pStack++;
            break;
        case 0x42:  // drop the top
            pFrame->pStack--;
            break;
        case 0x43: {  // call a script address, pushing the return address
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            u32 u;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *pFrame->pStack = (s32)pFrame->pPC;
            pFrame->pStack++;
            pFrame->pPC = _UISPatchFncPC(pStudio, pScreen->pScrData, u);
            break;
        }
        case 0x44:  // return to the popped address
            pFrame->pPC = (u8*)*--pFrame->pStack;
            break;
        case 0x45: {  // send events 0 and 3 to the screen a file word names, with n words
            u32 u;
            s32 n;
            u8 nArgs;

            nArgs = *pFrame->pPC;
            pFrame->pPC++;
            n = *--pFrame->pStack;
            u = *(u32*)(*(u32*)n + (u32)pScreen->pScrData);  // port: EA sums the pointer as a u32
            if (u != 0xFFFFFFFF) {
                // fake match: a same-value ?: on the test just made (both arms equal, no
                // compare left): this address phi keeps &data.ScreenInfo.ParentScreenID in a
                // register through the script loop, as in EA's build.
                u16* pA3 = (u != 0xFFFFFFFF) ? &data.ScreenInfo.ParentScreenID
                                             : &data.ScreenInfo.ParentScreenID;

                data.ScreenInfo.GroupID = u;
                data.ScreenInfo.ScreenID = u >> 16;
                data.ScreenInfo.ParentGroupID = pScreen->GroupID;
                *pA3 = pScreen->ScreenID;
                UISAddThreadAction(data.ScreenInfo.ParentGroupID, data.ScreenInfo.ParentScreenID, pStudio, 0,
                                   &data, nArgs, pFrame->pStack - nArgs);
                UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 3, &data, 0, NULL);
                while (nArgs-- != 0) {
                    pFrame->pStack--;
                }
            }
            break;
        }
        case 0x46: {  // send event 6 for a node info
            s32 n;

            n = *--pFrame->pStack;
            data.ActivateInfo.iDir = 0;
            data.ActivateInfo.iProcessed = 0;
            data.ActivateInfo.pControlInfo = (UISControlInfoT*)n;
            data.ActivateInfo.pTableEntry = NULL;
            data.ActivateInfo.GroupID = pScreen->GroupID;
            data.ActivateInfo.ScreenID = pScreen->ScreenID;
            UISAddThreadAction(data.ActivateInfo.GroupID, data.ActivateInfo.ScreenID, pStudio, 6, &data, 0,
                               NULL);
            break;
        }
        case 0x7E: {  // send event 5 for a node info
            s32 n;

            n = *--pFrame->pStack;
            data.ActivateInfo.iDir = 0;
            data.ActivateInfo.iProcessed = 0;
            data.ActivateInfo.pControlInfo = (UISControlInfoT*)n;
            data.ActivateInfo.pTableEntry = NULL;
            data.ActivateInfo.GroupID = pScreen->GroupID;
            data.ActivateInfo.ScreenID = pScreen->ScreenID;
            UISAddThreadAction(data.ActivateInfo.GroupID, data.ActivateInfo.ScreenID, pStudio, 5, &data, 0,
                               NULL);
            break;
        }
        case 0x47: {  // send event 5 for entry n of a list of node infos, unless it is this one and set
            s32 nId = *--pFrame->pStack;
            s32* pList = (s32*)*--pFrame->pStack;
            s32* pnList;
            UISControlInfoT* pEntry;
            s32 nEntry;

            nEntry = *--pFrame->pStack;
            pnList = (s32*)(*pList + (u32)pScreen->pScrData);  // port: EA sums the pointer as a u32
            if (nEntry < pnList[0]) {
                // fake match: the same for &data.ActivateInfo.pTableEntry.
                s32** pA2 = (nEntry < pnList[0]) ? &data.ActivateInfo.pTableEntry
                                                 : &data.ActivateInfo.pTableEntry;
                s32 nEntryOffset = pnList[nEntry + 2];
                pEntry = (UISControlInfoT*)((uptr)nEntryOffset + (uptr)pScreen->pScrData);
                if (pInfo != pEntry || pEntry->IsEnabled == 0) {
                    data.ActivateInfo.iDir = nId;
                    data.ActivateInfo.iProcessed = 0;
                    data.ActivateInfo.pControlInfo = pEntry;
                    *pA2 = pnList;
                    data.ActivateInfo.GroupID = pScreen->GroupID;
                    data.ActivateInfo.ScreenID = pScreen->ScreenID;
                    UISAddThreadAction(data.ActivateInfo.GroupID, data.ActivateInfo.ScreenID, pStudio, 5,
                                       &data, 1, &nEntry);
                }
            }
            break;
        }
        case 0x48:  // send event 3 to the screen that made this one current
            data.ScreenInfo.GroupID = pScreen->ParentGroupID;
            data.ScreenInfo.ScreenID = pScreen->ParentScreenID;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 3, &data, 0, NULL);
            break;
        case 0x49: {  // a text's length
            UISStringT* pText;

            pText = (UISStringT*)*--pFrame->pStack;
            if (pText != NULL) {
                pFrame->pStack[-1] = strlen(pText->ptr);
            } else {
                pFrame->pStack[-1] = 0;
            }
            break;
        }
        case 0x4A: {  // a text's character n
            u32 u;
            UISStringT* pText;
            s32 c = 0;

            u = *--pFrame->pStack;
            pText = (UISStringT*)*--pFrame->pStack;
            if (pText != NULL && u < pText->length) {
                c = pText->ptr[u];
            }
            pFrame->pStack[-1] = c;
            break;
        }
        case 0x4B: {  // set a text's character n
            s32 n;
            u32 u;
            UISStringT* pText;

            n = *--pFrame->pStack;
            u = *--pFrame->pStack;
            pText = (UISStringT*)*--pFrame->pStack;
            if (pText != NULL && u < pText->length) {
                pText->ptr[u] = n;
            }
            break;
        }
        case 0x4C: {  // format a text (at most 20 arguments)
            s32 n;
            UISStringT* pFind;
            u32 k;
            UISStringT* pText;

            n = *--pFrame->pStack;
            for (k = 0; k < n; k++) {
                if (k < 20) {
                    uisFormatStringStack[n - k - 1].iValue = *--pFrame->pStack;
                } else {
                    pFrame->pStack--;
                }
            }
            pFind = (UISStringT*)*--pFrame->pStack;
            pText = (UISStringT*)*--pFrame->pStack;
            UISStringFormat((u32)pScreen->pScrData, pText, pFind, n, uisFormatStringStack);
            break;
        }
        case 0x77: {  // whether a group is this screen's
            u32 u;

            u = *--pFrame->pStack;
            if (u == pScreen->GroupID) {
                pFrame->pStack[-1] = 1;
            } else {
                pFrame->pStack[-1] = 0;
            }
            break;
        }
        case 0x78: {  // the current screen, group|screen<<16
            u16 uCurScreen;
            u16 uCurGroup;

            UISGetActiveScreen(pStudio, &uCurGroup, &uCurScreen);
            pFrame->pStack[-1] = ((u32)uCurScreen << 16) | uCurGroup;
            break;
        }
        case 0x4D: {  // send event 2 with a word
            s32 nArg;

            nArg = *--pFrame->pStack;
            data.GenericInfo.Data[0] = *--pFrame->pStack;
            UISAddThreadAction(pScreen->GroupID, pScreen->ScreenID, pStudio, 2, &data, 1, &nArg);
            break;
        }
        case 0x4E:  // skip a word
            pFrame->pPC += 4;
            break;
        case 0x52: {  // swap the top two
            s32 n;

            n = pFrame->pStack[-1];
            pFrame->pStack[-1] = pFrame->pStack[-2];
            pFrame->pStack[-2] = n;
            break;
        }
        case 0x4F:  // clear the screen's event mask
            pScreen->ControllerDisable = 0;
            break;
        case 0x50:  // set every bit of it
            pScreen->ControllerDisable = -1;
            break;
        case 0x51:  // call a screen and wait for it: the script pauses in a ModalStack record
        case 0x70: {
            u32 u;
            s32 n;
            u8 nArgs;
            UISModalStackT* pRec;
            s32* pArgs;

            nArgs = *pFrame->pPC;
            pFrame->pPC++;
            n = pStudio->NumModals;
            if (n < pStudio->MaxModals) {
                pArgs = pFrame->pStack - nArgs;
                u = pArgs[-1];
                data.ScreenInfo.GroupID = u;
                data.ScreenInfo.ScreenID = u >> 16;
                data.ScreenInfo.ParentGroupID = pScreen->GroupID;
                data.ScreenInfo.ParentScreenID = pScreen->ScreenID;
                UISAddThreadAction(data.ScreenInfo.ParentGroupID, data.ScreenInfo.ParentScreenID, pStudio, 0,
                                   &data, nArgs, pArgs);
                while (nArgs-- != 0) {
                    pFrame->pStack--;
                }
                pFrame->pStack--;
                pRec = &pStudio->ModalStack[n];
                pRec->StackState.pPC = pFrame->pPC;
                pRec->StackState.pStack = pFrame->pStack;
                pRec->StackState.pStackCurrent = pFrame->pStackCurrent;
                pRec->StackState.pStackEnd = pFrame->pStackEnd;
                pRec->StackState.pStackStart = pFrame->pStackStart;
                pRec->pRestoreStack = p;
                pRec->pRestoreState = pFrame;
                pFrame->pStackCurrent = pRec->StackState.pStack;
                pRec->GroupID = data.ScreenInfo.GroupID;
                pRec->ScreenID = data.ScreenInfo.ScreenID;
                pRec->pScreen = pScreen;
                pRec->pControlInfo = pInfo;
                pStudio->NumModals++;
                return 3;
            }
            while (nArgs-- != 0) {
                pFrame->pStack--;
            }
            break;
        }
        case 0x54: {  // send event 5 for a node info
            s32 n;

            n = *--pFrame->pStack;
            data.ActivateInfo.iDir = 0;
            data.ActivateInfo.iProcessed = 0;
            data.ActivateInfo.pControlInfo = (UISControlInfoT*)n;
            data.ActivateInfo.pTableEntry = NULL;
            data.ActivateInfo.GroupID = pScreen->GroupID;
            data.ActivateInfo.ScreenID = pScreen->ScreenID;
            UISAddThreadAction(data.ActivateInfo.GroupID, data.ActivateInfo.ScreenID, pStudio, 5, &data, 0,
                               NULL);
            break;
        }
        case 0x55:  // push a word at a local's pointer plus an offset (0x6B: its address)
        case 0x6B: {
            u32 uOffset;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s32 n;
            s32* pArgs;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            n = (uByte0 << 8) | uByte1;
            uOffset += pFrame->pStack[(s16)n];
            pArgs = (s32*)uOffset;
            if (uOp == 0x6B) {
                *pFrame->pStack = (s32)pArgs;  // port: a stack word holds the pointer
                pFrame->pStack++;
            } else {
                *pFrame->pStack = *pArgs;
                pFrame->pStack++;
            }
            break;
        }
        case 0x56: {  // store the top at a local's pointer plus an offset
            u32 uOffset;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s32 n;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            n = (uByte0 << 8) | uByte1;
            uOffset += pFrame->pStack[(s16)n];
            *(s32*)uOffset = pTop[-1];
            pFrame->pStack--;
            break;
        }
        case 0x57: {  // push a local's pointer plus an offset
            u32 uOffset;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s32 n;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            n = (uByte0 << 8) | uByte1;
            pTop = pFrame->pStack;
            uOffset += pTop[(s16)n];
            *pTop = uOffset;
            pFrame->pStack++;
            break;
        }
        case 0x5A:  // int remainder
            pTop[-2] = pTop[-2] % pTop[-1];
            pFrame->pStack--;
            break;
        case 0x5B:  // read an array element (0x6C-0x6E: its address); the array is a count of
        case 0x5D:  // dimensions, the dimensions, then the elements; indexes are clamped
        case 0x5F:
        case 0x61:
        case 0x6C:
        case 0x6D:
        case 0x6E: {
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
            switch (uOp) {
            case 0x61:
                bOnStack = 1;
                pArr = (s32*)pTop[pTop[-1] - 1];
                break;
            case 0x5B:
            case 0x6C:
                bOnStack = 1;
                pArr = &pTop[pTop[-1] - 1];
                break;
            case 0x5D:
            case 0x6D:
                pArr = (s32*)pTop[-1];
                bOnStack = 1;
                break;
            case 0x5F:
            case 0x6E: {
                u32 uOffset;
                u32 uByte0;
                u32 uByte1;
                u32 uByte2;
                u32 uByte3;
                s32 n;

                bOnStack = 0;
                uByte0 = *pFrame->pPC;
                pFrame->pPC++;
                uByte1 = *pFrame->pPC;
                pFrame->pPC++;
                uByte2 = *pFrame->pPC;
                pFrame->pPC++;
                uByte3 = *pFrame->pPC;
                pFrame->pPC++;
                uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pFrame->pPC;
                pFrame->pPC++;
                uByte1 = *pFrame->pPC;
                pFrame->pPC++;
                n = (uByte0 << 8) | uByte1;
                uOffset += pFrame->pStack[(s16)n];
                pArr = (s32*)uOffset;
                break;
            }
            }
            nDims = pArr[0];
            for (i = 1; i <= nDims; i++) {
                s32 n;
                s32 dimSize;  // name: Madden 2003 STABS

                n = pTop[-(i + bOnStack)];
                dimSize = pArr[i];
                if (n >= dimSize || n < 0) {
                    n = dimSize - 1;
                }
                nIndex += nMul * n;
                nMul *= dimSize;
            }
            if (uOp == 0x6C || uOp == 0x6D || uOp == 0x6E) {
                // port: a stack word holds the pointer
                pTop[-(nDims + bOnStack)] = (s32)&pArr[nDims + 1 + nIndex];
            } else {
                pTop[-(nDims + bOnStack)] = pArr[nDims + 1 + nIndex];
            }
            pFrame->pStack -= nDims - (1 - bOnStack);
            break;
        }
        case 0x5C:  // write an array element
        case 0x5E:
        case 0x60:
        case 0x62: {
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
            switch (uOp) {
            case 0x62:
                bOnStack = 1;
                pArr = (s32*)pTop[pTop[-1] - 1];
                break;
            case 0x5C:
                bOnStack = 1;
                pArr = &pTop[pTop[-1] - 1];
                break;
            case 0x5E:
                pArr = (s32*)pTop[-1];
                bOnStack = 1;
                break;
            case 0x60: {
                u32 uOffset;
                u32 uByte0;
                u32 uByte1;
                u32 uByte2;
                u32 uByte3;
                s32 n;

                bOnStack = 0;
                uByte0 = *pFrame->pPC;
                pFrame->pPC++;
                uByte1 = *pFrame->pPC;
                pFrame->pPC++;
                uByte2 = *pFrame->pPC;
                pFrame->pPC++;
                uByte3 = *pFrame->pPC;
                pFrame->pPC++;
                uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pFrame->pPC;
                pFrame->pPC++;
                uByte1 = *pFrame->pPC;
                pFrame->pPC++;
                n = (uByte0 << 8) | uByte1;
                uOffset += pFrame->pStack[(s16)n];
                pArr = (s32*)uOffset;
                break;
            }
            }
            nDims = pArr[0];
            for (i = 1; i <= nDims; i++) {
                s32 n;
                s32 dimSize;  // name: Madden 2003 STABS

                n = pTop[-(i + 1 + bOnStack)];
                dimSize = pArr[i];
                if (n >= dimSize || n < 0) {
                    n = dimSize - 1;
                }
                nIndex += nMul * n;
                nMul *= dimSize;
            }
            pArr[nDims + 1 + nIndex] = pTop[-1 - bOnStack];
            pFrame->pStack -= nDims + 1 + bOnStack;
            break;
        }
        case 0x63: {  // push n copies of the top
            s32 nCount;
            s32 i;
            u8 nByte0;
            u8 nByte1;
            u8 nByte2;
            u8 nByte3;

            nByte0 = *pFrame->pPC;
            pFrame->pPC++;
            nByte1 = *pFrame->pPC;
            pFrame->pPC++;
            nByte2 = *pFrame->pPC;
            pFrame->pPC++;
            nByte3 = *pFrame->pPC;
            pFrame->pPC++;
            nCount = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            for (i = 0; i < nCount; i++) {
                pTop[i] = pTop[-1];
            }
            pFrame->pStack += nCount;
            break;
        }
        case 0x64: {  // drop n words
            s32 nCount;
            u8 nByte0;
            u8 nByte1;
            u8 nByte2;
            u8 nByte3;

            nByte0 = *pFrame->pPC;
            pFrame->pPC++;
            nByte1 = *pFrame->pPC;
            pFrame->pPC++;
            nByte2 = *pFrame->pPC;
            pFrame->pPC++;
            nByte3 = *pFrame->pPC;
            pFrame->pPC++;
            nCount = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            pFrame->pStack -= nCount;
            break;
        }
        case 0x65: {  // push a copy of the top n words
            s32 i;
            s32 nCount;
            u8 nByte0;
            u8 nByte1;
            u8 nByte2;
            u8 nByte3;

            nByte0 = *pFrame->pPC;
            pFrame->pPC++;
            nByte1 = *pFrame->pPC;
            pFrame->pPC++;
            nByte2 = *pFrame->pPC;
            pFrame->pPC++;
            nByte3 = *pFrame->pPC;
            pFrame->pPC++;
            nCount = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            for (i = 0; i < nCount; i++) {
                pTop[i] = pTop[i - nCount];
            }
            pFrame->pStack += nCount;
            break;
        }
        case 0x66:  // fill an array with a value
        case 0x67:
        case 0x68: {
            int i2;  // an int: EA's loop guards compare it with the s32 bound
            s32* pArr;
            s32 nDims;
            s32 nMul;
            s32 bOnStack;

            pArr = NULL;
            nMul = 1;
            bOnStack = 0;
            switch (uOp) {
            case 0x66:
                bOnStack = 1;
                pArr = &pTop[pTop[-1] - 1];
                break;
            case 0x67:
                pArr = (s32*)pTop[-1];
                bOnStack = 1;
                break;
            case 0x68: {
                u32 uOffset;
                u32 uByte0;
                u32 uByte1;
                u32 uByte2;
                u32 uByte3;
                s32 n;

                bOnStack = 0;
                uByte0 = *pFrame->pPC;
                pFrame->pPC++;
                uByte1 = *pFrame->pPC;
                pFrame->pPC++;
                uByte2 = *pFrame->pPC;
                pFrame->pPC++;
                uByte3 = *pFrame->pPC;
                pFrame->pPC++;
                uOffset = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pFrame->pPC;
                pFrame->pPC++;
                uByte1 = *pFrame->pPC;
                pFrame->pPC++;
                n = (uByte0 << 8) | uByte1;
                uOffset += pFrame->pStack[(s16)n];
                pArr = (s32*)uOffset;
                break;
            }
            }
            nDims = pArr[0];
            for (i2 = 1; i2 <= nDims; i2++) {
                nMul *= pArr[i2];
            }
            for (i2 = 0; i2 < nMul; i2++) {
                pArr[nDims + 1 + i2] = pTop[-1 - bOnStack];
            }
            pFrame->pStack -= bOnStack + 1;
            break;
        }
        case 0x6F: {  // entry n of the screen file's third table (0 past its end)
            u32 u;

            u = pTop[-1];
            if (u < pScreen->pScrData->NumStrings) {
                pTop[-1] = (s32)&pScreen->pScrData->Strings[u];  // port: a stack word holds the pointer
            } else {
                pTop[-1] = 0;
            }
            break;
        }
        case 0x59: {  // switch a node on or off (UISUpdateVisibility)
            u32 u;
            u32 uByte0;
            u32 uByte1;
            u32 uByte2;
            u32 uByte3;
            s16 nIndex;

            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            uByte2 = *pFrame->pPC;
            pFrame->pPC++;
            uByte3 = *pFrame->pPC;
            pFrame->pPC++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            uByte1 = *pFrame->pPC;
            pFrame->pPC++;
            nIndex = (uByte0 << 8) | uByte1;
            uByte0 = *pFrame->pPC;
            pFrame->pPC++;
            u += pFrame->pStack[nIndex];  // port: the sum is a 32-bit address
            UISUpdateVisibility(pStudio, pScreen, uByte0, (void*)u, pTop[-1]);
            pFrame->pStack--;
            break;
        }
        case 0x73: {  // whether a rate function runs
            UISControlInfoT* pNodeInfo = (UISControlInfoT*)*--pFrame->pStack;
            s32 nId = *--pFrame->pStack;

            *pFrame->pStack = UISFindRateFnc(pStudio, pNodeInfo, nId) < pStudio->NumRateFncs;
            pFrame->pStack++;
            break;
        }
        case 0x72: {  // send event 8 to a screen with a word
            s32 n;
            u32 u;

            n = *--pFrame->pStack;
            u = *--pFrame->pStack;
            data.ScreenInfo.GroupID = u;
            data.ScreenInfo.ScreenID = u >> 16;
            data.ScreenInfo.ParentGroupID = pScreen->GroupID;
            data.ScreenInfo.ParentScreenID = pScreen->ScreenID;
            data.ScreenInfo.iDir = n;
            UISAddThreadAction(data.ScreenInfo.ParentGroupID, data.ScreenInfo.ParentScreenID, pStudio, 8,
                               &data, 0, NULL);
            break;
        }
        case 0x74: {  // set or clear a bit of the screen's event mask (-1: all)
            s32 n;
            s32 bOn;
            u32 uBits;

            bOn = *--pFrame->pStack;
            n = *--pFrame->pStack;
            uBits = n >= 0 ? 1 << n : -1;
            if (bOn != 0) {
                pScreen->ControllerDisable |= uBits;
            } else {
                pScreen->ControllerDisable &= ~uBits;
            }
            break;
        }
        case 0x75: {  // whether a bit of the event mask is clear
            s32 n;

            n = *--pFrame->pStack;
            *pFrame->pStack = (pScreen->ControllerDisable & (1 << n)) == 0;
            pFrame->pStack++;
            break;
        }
        case 0x79: {  // a text's buffer size, as a float
            UISStringT* pText;

            pText = (UISStringT*)*--pFrame->pStack;
            if (pText != NULL) {
                pFrame->pStack[-1] = pText->length;
            } else {
                pFrame->pStack[-1] = 0;
            }
            break;
        }
        case 0x7A: {  // a text to upper case
            UISStringT* pText;
            u32 j;

            pText = (UISStringT*)*--pFrame->pStack;
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

            pText = (UISStringT*)*--pFrame->pStack;
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

            pRep = (UISStringT*)*--pFrame->pStack;
            pFind = (UISStringT*)*--pFrame->pStack;
            pText = (UISStringT*)*--pFrame->pStack;
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

            pText = (UISStringT*)*--pFrame->pStack;
            if (pText != NULL && pStudio->pScreenDrawDebugFnc != NULL && pScreen != NULL) {
                // port: the text's address passed as the callback's word
                pStudio->pScreenDrawDebugFnc(pScreen->GroupID, pScreen->ScreenID, (s32)pText->ptr);
            }
            break;
        }
        }
    }
    return 0;
}

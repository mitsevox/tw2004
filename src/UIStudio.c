// UIStudio.c (EA's name, from its asserts): the core of EA's UI Studio library, the menu screens'
// runtime. It loads, activates and unloads screens and runs the events sent to them. Its own
// data: the "UIStudio.c" assert string and a warning in .rodata (0x801861A0-0x8018620C), three
// jump tables in .data and constants in .sdata2 (0x802851A8-0x802851B7).

#include "frontend/uistudio.h"

// The arguments a script's format opcode (0x4C) hands fn_8016B808: at most 20.
UISWord lbl_802805D8[20];

void fn_80168644(UIStudio* pStudio, UISScreen* pScreen, s32 nKind, void* p, s32 bOn);

// Runs a screen's script from pFrame->p10: a byte-code machine with a stack of 32-bit words
// (ints, floats and pointers) that grows up from pFrame->pC. It stops at the script's end
// (returns 0) or when the script waits for another screen (returns 3; fn_80169308 resumes it
// from the p60 record it keeps). Immediates are big-endian; opcodes not listed do nothing.
// port: the stack keeps pointers in 32-bit words, like the rest of the studio.
s8 fn_80166098(UIStudio* pStudio, s32* p, UISFrame* pFrame, UISScreen* pScreen, UISNodeInfo* pInfo) {
    UISEventData data;
    UISWord word;
    u16 uGroup;
    u16 uScreen;
    s32* pTop;
    s32* pArgs;
    s32* pArr;
    u8 uOp;
    u8 nArgs;
    u32 u;
    s32 n;
    s32 n2;
    s32 i;
    int i2;  // cases 0x66-0x68 count in an int: EA's loop guards compare it with the s32 bound
    s32 nDims;
    s32 nIndex;
    s32 nMul;
    s32 bOnStack;
    UISText* pText;
    UISText* pFind;
    UISText* pRep;
    UISRecord60* pRec;
    u32 nText;
    u32 nFind;
    u32 nRep;
    s32 nGrow;
    u32 j;
    u32 k;
    u8 bMatch;
    f32 f;
    // The script's big-endian immediates are read a byte at a time into these, then combined
    // (the three four-byte reads into n go through the u8 ones: the code masks those bytes).
    u32 uByte0;
    u32 uByte1;
    u32 uByte2;
    u32 uByte3;
    u8 nByte0;
    u8 nByte1;
    u8 nByte2;
    u8 nByte3;

    while (pFrame->p10 != NULL) {
        uOp = *pFrame->p10;
        pTop = pFrame->pC;
        pFrame->p10++;
        switch (uOp) {
        case 0x02:  // send event 0 (a call to screen group|screen<<16 below the arguments)
        case 0x71:
            nArgs = *pFrame->p10;
            pFrame->p10++;
            pArgs = pFrame->pC - nArgs;
            u = pArgs[-1];
            data.aw[0] = u;
            data.aw[1] = u >> 16;
            data.aw[2] = pScreen->uGroup;
            data.aw[3] = pScreen->uScreen;
            fn_80165B90(data.aw[2], data.aw[3], pStudio, 0, &data, nArgs, pArgs);
            while (nArgs-- != 0) {
                pFrame->pC--;
            }
            pFrame->pC--;
            break;
        case 0x03:  // send event 1 to a screen (0xFFFF: this one) with a word
            n = *--pFrame->pC;
            u = *--pFrame->pC;
            uGroup = u;
            uScreen = u >> 16;
            if (uGroup == 0xFFFF) {
                uGroup = pScreen->uGroup;
                uScreen = pScreen->uScreen;
            }
            data.aw[0] = uGroup;
            data.aw[1] = uScreen;
            data.au[2] = n;
            fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 1, &data, 0, NULL);
            break;
        case 0x08:  // send event 3 to a screen (0xFFFF: this one)
            u = *--pFrame->pC;
            uGroup = u;
            uScreen = u >> 16;
            if (uGroup == 0xFFFF) {
                uGroup = pScreen->uGroup;
                uScreen = pScreen->uScreen;
            }
            data.aw[0] = uGroup;
            data.aw[1] = uScreen;
            fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 3, &data, 0, NULL);
            break;
        case 0x58: {  // start a rate function (the byte: how many script addresses follow)
            // Script addresses: an offset into the studio's current UI file when the top bit is
            // set, otherwise into the screen's own file.
            UISNodeInfo* pNodeInfo;
            s32 n30;
            s32 nTime;
            f32 fTarget;
            s32 nU20;
            s32 nId;
            u8* pDoneScript = NULL;
            u8* pStepScript = NULL;

            nArgs = *pFrame->p10;
            pFrame->p10++;
            pNodeInfo = (UISNodeInfo*)*--pFrame->pC;
            n30 = *--pFrame->pC;
            if (nArgs >= 5) {
                if (nArgs >= 6) {
                    u = *--pFrame->pC;
                    // fake match: each script address is resolved through these temporaries, the
                    // file base read first and the result copied, as from an inline helper.
                    {
                        u8* pBase = (u8*)pScreen->pData;
                        u8* pRet;

                        if ((u & 0x80000000) == 0x80000000) {
                            pRet = (u8*)pStudio->pCurrent->p10 + (u & 0x7FFFFFFF);
                        } else {
                            pRet = pBase + u;
                        }
                        pStepScript = pRet;
                    }
                }
                u = *--pFrame->pC;
                {
                    u8* pBase = (u8*)pScreen->pData;
                    u8* pRet;

                    if ((u & 0x80000000) == 0x80000000) {
                        pRet = (u8*)pStudio->pCurrent->p10 + (u & 0x7FFFFFFF);
                    } else {
                        pRet = pBase + u;
                    }
                    pDoneScript = pRet;
                }
            }
            nTime = *--pFrame->pC;
            fTarget = *(f32*)--pFrame->pC;
            nU20 = *--pFrame->pC;
            nId = *--pFrame->pC;
            fn_80165E9C(pStudio, pScreen, pNodeInfo, n30, nId, pDoneScript, pStepScript, nTime, fTarget,
                        nU20);
            break;
        }
        case 0x06: {  // start a stepped rate function
            UISNodeInfo* pNodeInfo = (UISNodeInfo*)*--pFrame->pC;
            s32 nU10 = *--pFrame->pC;
            u8* pStepScript;
            s32 nId;

            u = *--pFrame->pC;
            {
                u8* pBase = (u8*)pScreen->pData;
                u8* pRet;

                if ((u & 0x80000000) == 0x80000000) {
                    pRet = (u8*)pStudio->pCurrent->p10 + (u & 0x7FFFFFFF);
                } else {
                    pRet = pBase + u;
                }
                pStepScript = pRet;
            }
            nId = *--pFrame->pC;
            fn_80165D90(pStudio, pScreen, pNodeInfo, nId, pStepScript, nU10);
            break;
        }
        case 0x07: {  // stop a rate function
            UISNodeInfo* pNodeInfo = (UISNodeInfo*)*--pFrame->pC;
            s32 nId = *--pFrame->pC;

            fn_80165D2C(pStudio, pNodeInfo, nId);
            break;
        }
        case 0x0A: {  // call one of the game's handlers with a variable of the screen file
            u32* pnOffset;
            void* pVar;

            n = *--pFrame->pC;
            i = *--pFrame->pC;
            n2 = *--pFrame->pC;
            pnOffset = (u32*)*--pFrame->pC;
            if (pnOffset != NULL) {
                // fake match: an integer sum, offset first (EA's add order)
                pVar = (void*)(*pnOffset + (uptr)pScreen->pData);
            } else {
                pVar = NULL;
            }
            pArgs = pFrame->pC - n;
            // port: the last argument is the word below the arguments, passed as an address
            pStudio->ppfnHandlers[i](pVar, n2, n, pArgs, (s32)(pArgs - 1));
            break;
        }
        case 0x76:  // send event 9 with n words
            n = *--pFrame->pC;
            pFrame->pC--;
            pArgs = pFrame->pC - n;
            data.au[0] = pArgs[0];
            fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 9, &data, n, pArgs + 1);
            break;
        case 0x0B:  // a command to the game (pfnCommand)
            uGroup = pScreen->uGroup;
            uScreen = pScreen->uScreen;
            n = *--pFrame->pC;
            pFrame->pC--;
            pArgs = pFrame->pC - n;
            // port: pointers passed as the callback's words
            pStudio->pfnCommand(pArgs[0], uGroup, uScreen, n, (s32)(pArgs + 1), (s32)(pArgs - 1));
            break;
        case 0x0C:  // send event 7 with two words and n more
            n = *--pFrame->pC;
            data.au[0] = *--pFrame->pC;
            data.au[1] = *--pFrame->pC;
            fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 7, &data, n, pFrame->pC - n);
            break;
        case 0x0D:  // push the screen file
            *pTop = (s32)pScreen->pData;
            pFrame->pC++;
            break;
        case 0x0E:  // copy a text
            pText = (UISText*)*--pFrame->pC;
            pFind = (UISText*)*--pFrame->pC;
            if (pText != NULL && pFind != NULL) {
                u = pFind->nSize;
                if (pText->nSize < u) {
                    u = pText->nSize;
                }
                strncpy(pText->szText, pFind->szText, u);
                pText->szText[u] = 0;
            }
            break;
        case 0x0F:  // push a word
        case 0x10:
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *pFrame->pC = u;
            pFrame->pC++;
            break;
        case 0x11:  // push a float
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            word.u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *(f32*)pFrame->pC = word.f;
            pFrame->pC++;
            break;
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
        case 0x18:  // load through a pointer
            pTop[-1] = *(s32*)pTop[-1];
            break;
        case 0x19:  // store through a pointer
            *(s32*)pTop[-1] = pTop[-2];
            pFrame->pC -= 2;
            break;
        case 0x69:  // the address of a local
            pTop[-1] = (s32)&pTop[pTop[-1] - 1];  // port: a stack word holds the pointer
            break;
        case 0x1A:  // load a local
            pTop[-1] = pTop[pTop[-1] - 1];
            break;
        case 0x1B:  // store a local
            n = pTop[-1] - 1;
            pTop[n] = pTop[-2];
            pFrame->pC -= 2;
            break;
        case 0x1C:  // bitwise and
            n = *--pFrame->pC;
            n2 = *--pFrame->pC;
            *pFrame->pC = n & n2;
            pFrame->pC++;
            break;
        case 0x1D:  // bitwise or
            n = *--pFrame->pC;
            n2 = *--pFrame->pC;
            *pFrame->pC = n | n2;
            pFrame->pC++;
            break;
        case 0x1E:  // bitwise not
            n = *--pFrame->pC;
            *pFrame->pC = ~n;
            pFrame->pC++;
            break;
        case 0x1F:  // logical and
            n = *--pFrame->pC;
            n2 = *--pFrame->pC;
            *pFrame->pC = n != 0 && n2 != 0;
            pFrame->pC++;
            break;
        case 0x20:  // logical or
            n = *--pFrame->pC;
            n2 = *--pFrame->pC;
            *pFrame->pC = n != 0 || n2 != 0;
            pFrame->pC++;
            break;
        case 0x21:  // logical not
            n = *--pFrame->pC;
            *pFrame->pC = n == 0;
            pFrame->pC++;
            break;
        case 0x22:  // int abs
            n = pTop[-1];
            pTop[-1] = (n < 0) ? -n : n;
            break;
        case 0x23:  // float abs
            f = *(f32*)--pFrame->pC;
            *(f32*)pFrame->pC = (f < 0.0f) ? -f : f;
            pFrame->pC++;
            break;
        case 0x24:  // int negate
            pFrame->pC[-1] = -pTop[-1];
            break;
        case 0x25:  // int add
            pTop[-2] = pTop[-2] + pTop[-1];
            pFrame->pC--;
            break;
        case 0x26:  // int subtract
            pTop = pFrame->pC;
            pTop[-2] = pTop[-2] - pTop[-1];
            pFrame->pC--;
            break;
        case 0x27:  // int multiply
            pTop[-2] = pTop[-2] * pTop[-1];
            pFrame->pC--;
            break;
        case 0x28:  // int divide
            pTop[-2] = pTop[-2] / pTop[-1];
            pFrame->pC--;
            break;
        case 0x29:  // float negate
            f = *(f32*)--pFrame->pC;
            *(f32*)pFrame->pC = -f;
            pFrame->pC++;
            break;
        case 0x2A:  // float add
            *(f32*)&pTop[-2] = *(f32*)&pTop[-2] + *(f32*)&pTop[-1];
            pFrame->pC--;
            break;
        case 0x2B:  // float subtract
            *(f32*)&pTop[-2] = *(f32*)&pTop[-2] - *(f32*)&pTop[-1];
            pFrame->pC--;
            break;
        case 0x2C:  // float multiply
            *(f32*)&pTop[-2] = *(f32*)&pTop[-2] * *(f32*)&pTop[-1];
            pFrame->pC--;
            break;
        case 0x2D:  // float divide (0 by zero)
            if (*(f32*)&pTop[-1] != 0.0f) {
                *(f32*)&pTop[-2] = *(f32*)&pTop[-2] / *(f32*)&pTop[-1];
            } else {
                *(f32*)&pTop[-2] = 0.0f;
            }
            pFrame->pC--;
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
            pFrame->pC--;
            break;
        case 0x33:  // int <=
            pTop[-2] = pTop[-1] >= pTop[-2];
            pFrame->pC--;
            break;
        case 0x34:  // int >
            pTop[-2] = pTop[-1] < pTop[-2];
            pFrame->pC--;
            break;
        case 0x35:  // int <
            pTop[-2] = pTop[-1] > pTop[-2];
            pFrame->pC--;
            break;
        case 0x36:  // int ==
            pTop[-2] = pTop[-1] == pTop[-2];
            pFrame->pC--;
            break;
        case 0x37:  // int !=
            pTop[-2] = pTop[-1] != pTop[-2];
            pFrame->pC--;
            break;
        case 0x38:  // float >=
            pTop[-2] = *(f32*)&pTop[-1] <= *(f32*)&pTop[-2];
            pFrame->pC--;
            break;
        case 0x39:  // float <=
            pTop[-2] = *(f32*)&pTop[-1] >= *(f32*)&pTop[-2];
            pFrame->pC--;
            break;
        case 0x3A:  // float >
            pTop[-2] = *(f32*)&pTop[-1] < *(f32*)&pTop[-2];
            pFrame->pC--;
            break;
        case 0x3B:  // float <
            pTop[-2] = *(f32*)&pTop[-1] > *(f32*)&pTop[-2];
            pFrame->pC--;
            break;
        case 0x3C:  // float ==
            pTop[-2] = *(f32*)&pTop[-1] == *(f32*)&pTop[-2];
            pFrame->pC--;
            break;
        case 0x3D:  // float !=
            pTop[-2] = *(f32*)&pTop[-1] != *(f32*)&pTop[-2];
            pFrame->pC--;
            break;
        case 0x3E:  // jump by an offset if true
            n = *--pFrame->pC;
            u = *--pFrame->pC;
            if (u != 0) {
                pFrame->p10 += n;
            }
            break;
        case 0x3F:  // jump by an offset if false
            n = *--pFrame->pC;
            u = *--pFrame->pC;
            if (u == 0) {
                pFrame->p10 += n;
            }
            break;
        case 0x40:  // jump by an offset
            n = *--pFrame->pC;
            pFrame->p10 += n;
            break;
        case 0x41:  // duplicate the top
            pTop[0] = pTop[-1];
            pFrame->pC++;
            break;
        case 0x42:  // drop the top
            pFrame->pC--;
            break;
        case 0x43:  // call a script address, pushing the return address
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            *pFrame->pC = (s32)pFrame->p10;
            pFrame->pC++;
            {
                u8* pBase = (u8*)pScreen->pData;
                u8* pRet;

                if ((u & 0x80000000) == 0x80000000) {
                    pRet = (u8*)pStudio->pCurrent->p10 + (u & 0x7FFFFFFF);
                } else {
                    pRet = pBase + u;
                }
                pFrame->p10 = pRet;
            }
            break;
        case 0x44:  // return to the popped address
            pFrame->p10 = (u8*)*--pFrame->pC;
            break;
        case 0x45:  // send events 0 and 3 to the screen a file word names, with n words
            nArgs = *pFrame->p10;
            pFrame->p10++;
            n = *--pFrame->pC;
            u = *(u32*)((u8*)pScreen->pData + *(u32*)n);
            if (u != 0xFFFFFFFF) {
                data.aw[0] = u;
                data.aw[1] = u >> 16;
                data.aw[2] = pScreen->uGroup;
                data.aw[3] = pScreen->uScreen;
                fn_80165B90(data.aw[2], data.aw[3], pStudio, 0, &data, nArgs, pFrame->pC - nArgs);
                fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 3, &data, 0, NULL);
                while (nArgs-- != 0) {
                    pFrame->pC--;
                }
            }
            break;
        case 0x46:  // send event 6 for a node info
            n = *--pFrame->pC;
            data.aw[0] = 0;
            data.aw[1] = 0;
            data.au[2] = n;
            data.au[1] = 0;
            data.aw[7] = pScreen->uGroup;
            data.aw[6] = pScreen->uScreen;
            fn_80165B90(data.aw[7], data.aw[6], pStudio, 6, &data, 0, NULL);
            break;
        case 0x7E:  // send event 5 for a node info
            n = *--pFrame->pC;
            data.aw[0] = 0;
            data.aw[1] = 0;
            data.au[2] = n;
            data.au[1] = 0;
            data.aw[7] = pScreen->uGroup;
            data.aw[6] = pScreen->uScreen;
            fn_80165B90(data.aw[7], data.aw[6], pStudio, 5, &data, 0, NULL);
            break;
        case 0x47: {  // send event 5 for entry n of a list of node infos, unless it is this one and set
            s32 nId = *--pFrame->pC;
            s32* pList = (s32*)*--pFrame->pC;
            s32* pnList;
            UISNodeInfo* pEntry;
            s32 nEntry;

            nEntry = *--pFrame->pC;
            pnList = (s32*)((u8*)pScreen->pData + *pList);
            if (nEntry < pnList[0]) {
                pEntry = (UISNodeInfo*)((u8*)pScreen->pData + pnList[nEntry + 2]);
                if (pInfo != pEntry || pEntry->u4 == 0) {
                    data.aw[0] = nId;
                    data.aw[1] = 0;
                    data.ap[2] = pEntry;
                    data.ap[1] = pnList;
                    data.aw[7] = pScreen->uGroup;
                    data.aw[6] = pScreen->uScreen;
                    fn_80165B90(data.aw[7], data.aw[6], pStudio, 5, &data, 1, &nEntry);
                }
            }
            break;
        }
        case 0x48:  // send event 3 to the screen that made this one current
            data.aw[0] = pScreen->uPrevGroup;
            data.aw[1] = pScreen->uPrevScreen;
            fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 3, &data, 0, NULL);
            break;
        case 0x49:  // a text's length
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL) {
                pFrame->pC[-1] = strlen(pText->szText);
            } else {
                pFrame->pC[-1] = 0;
            }
            break;
        case 0x4A: {  // a text's character n
            s32 c = 0;

            u = *--pFrame->pC;
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL && u < pText->nSize) {
                c = pText->szText[u];
            }
            pFrame->pC[-1] = c;
            break;
        }
        case 0x4B:  // set a text's character n
            n = *--pFrame->pC;
            u = *--pFrame->pC;
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL && u < pText->nSize) {
                pText->szText[u] = n;
            }
            break;
        case 0x4C:  // format a text (at most 20 arguments)
            pFrame->pC--;
            n = pTop[-1];
            for (k = 0; k < n; k++) {
                if (k < 20) {
                    lbl_802805D8[n - k - 1].n = *--pFrame->pC;
                } else {
                    pFrame->pC--;
                }
            }
            pFind = (UISText*)*--pFrame->pC;
            pText = (UISText*)*--pFrame->pC;
            fn_8016B808((u32)pScreen->pData, pText, pFind, n, lbl_802805D8);
            break;
        case 0x77:  // whether a group is this screen's
            u = *--pFrame->pC;
            if (u == pScreen->uGroup) {
                pFrame->pC[-1] = 1;
            } else {
                pFrame->pC[-1] = 0;
            }
            break;
        case 0x78: {  // the current screen, group|screen<<16
            u16 uCurScreen;
            u16 uCurGroup;

            fn_80168EE8(pStudio, &uCurGroup, &uCurScreen);
            pFrame->pC[-1] = uCurGroup | (uCurScreen << 16);
            break;
        }
        case 0x4D: {  // send event 2 with a word
            s32 nArg;

            nArg = *--pFrame->pC;
            data.au[0] = *--pFrame->pC;
            fn_80165B90(pScreen->uGroup, pScreen->uScreen, pStudio, 2, &data, 1, &nArg);
            break;
        }
        case 0x4E:  // skip a word
            pFrame->p10 += 4;
            break;
        case 0x52:  // swap the top two
            n = pFrame->pC[-1];
            pFrame->pC[-1] = pFrame->pC[-2];
            pFrame->pC[-2] = n;
            break;
        case 0x4F:  // clear the screen's event mask
            pScreen->uMask = 0;
            break;
        case 0x50:  // set every bit of it
            pScreen->uMask = -1;
            break;
        case 0x51:  // call a screen and wait for it: the script pauses in a p60 record
        case 0x70:
            nArgs = *pFrame->p10;
            pFrame->p10++;
            n = pStudio->n5C;
            if (n < pStudio->nMax60) {
                pArgs = pFrame->pC - nArgs;
                u = pArgs[-1];
                data.aw[0] = u;
                data.aw[1] = u >> 16;
                data.aw[2] = pScreen->uGroup;
                data.aw[3] = pScreen->uScreen;
                fn_80165B90(data.aw[2], data.aw[3], pStudio, 0, &data, nArgs, pArgs);
                while (nArgs-- != 0) {
                    pFrame->pC--;
                }
                pFrame->pC--;
                pRec = &pStudio->p60[n];
                pRec->frame.p10 = pFrame->p10;
                pRec->frame.pC = pFrame->pC;
                pRec->frame.p4 = pFrame->p4;
                pRec->frame.p8 = pFrame->p8;
                pRec->frame.p0 = pFrame->p0;
                pRec->p1C = p;
                pRec->pFrame = pFrame;
                pFrame->p4 = pRec->frame.pC;
                pRec->u26 = data.aw[0];
                pRec->u24 = data.aw[1];
                pRec->pScreen = pScreen;
                pRec->pInfo = pInfo;
                pStudio->n5C++;
                return 3;
            }
            while (nArgs-- != 0) {
                pFrame->pC--;
            }
            break;
        case 0x54:  // send event 5 for a node info
            n = *--pFrame->pC;
            data.aw[0] = 0;
            data.aw[1] = 0;
            data.au[2] = n;
            data.au[1] = 0;
            data.aw[7] = pScreen->uGroup;
            data.aw[6] = pScreen->uScreen;
            fn_80165B90(data.aw[7], data.aw[6], pStudio, 5, &data, 0, NULL);
            break;
        case 0x55:  // push a word at a local's pointer plus an offset (0x6B: its address)
        case 0x6B:
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            n = (uByte0 << 8) | uByte1;
            pArgs = (s32*)(u + pFrame->pC[(s16)n]);
            if (uOp == 0x6B) {
                *pFrame->pC = (s32)pArgs;  // port: a stack word holds the pointer
                pFrame->pC++;
            } else {
                *pFrame->pC = *pArgs;
                pFrame->pC++;
            }
            break;
        case 0x56:  // store the top at a local's pointer plus an offset
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            n = (uByte0 << 8) | uByte1;
            *(s32*)(u + pFrame->pC[(s16)n]) = pTop[-1];
            pFrame->pC--;
            break;
        case 0x57:  // push a local's pointer plus an offset
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            n = (uByte0 << 8) | uByte1;
            pTop = pFrame->pC;
            *pTop = u + pTop[(s16)n];
            pFrame->pC++;
            break;
        case 0x5A:  // int remainder
            pTop[-2] = pTop[-2] % pTop[-1];
            pFrame->pC--;
            break;
        case 0x5B:  // read an array element (0x6C-0x6E: its address); the array is a count of
        case 0x5D:  // dimensions, the dimensions, then the elements; indexes are clamped
        case 0x5F:
        case 0x61:
        case 0x6C:
        case 0x6D:
        case 0x6E:
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
            case 0x6E:
                bOnStack = 0;
                uByte0 = *pFrame->p10;
                pFrame->p10++;
                uByte1 = *pFrame->p10;
                pFrame->p10++;
                uByte2 = *pFrame->p10;
                pFrame->p10++;
                uByte3 = *pFrame->p10;
                pFrame->p10++;
                u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pFrame->p10;
                pFrame->p10++;
                uByte1 = *pFrame->p10;
                pFrame->p10++;
                n = (uByte0 << 8) | uByte1;
                pArr = (s32*)(u + pFrame->pC[(s16)n]);
                break;
            }
            nDims = pArr[0];
            for (i = 1; i <= nDims; i++) {
                n = pTop[-(i + bOnStack)];
                if (n >= pArr[i] || n < 0) {
                    n = pArr[i] - 1;
                }
                nIndex += nMul * n;
                nMul *= pArr[i];
            }
            if (uOp == 0x6C || uOp == 0x6D || uOp == 0x6E) {
                // port: a stack word holds the pointer
                pTop[-(nDims + bOnStack)] = (s32)&pArr[nDims + nIndex + 1];
            } else {
                pTop[-(nDims + bOnStack)] = pArr[nDims + nIndex + 1];
            }
            pFrame->pC -= nDims - (1 - bOnStack);
            break;
        case 0x5C:  // write an array element
        case 0x5E:
        case 0x60:
        case 0x62:
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
            case 0x60:
                bOnStack = 0;
                uByte0 = *pFrame->p10;
                pFrame->p10++;
                uByte1 = *pFrame->p10;
                pFrame->p10++;
                uByte2 = *pFrame->p10;
                pFrame->p10++;
                uByte3 = *pFrame->p10;
                pFrame->p10++;
                u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pFrame->p10;
                pFrame->p10++;
                uByte1 = *pFrame->p10;
                pFrame->p10++;
                n = (uByte0 << 8) | uByte1;
                pArr = (s32*)(u + pFrame->pC[(s16)n]);
                break;
            }
            nDims = pArr[0];
            for (i = 1; i <= nDims; i++) {
                n = pTop[-(i + bOnStack + 1)];
                if (n >= pArr[i] || n < 0) {
                    n = pArr[i] - 1;
                }
                nIndex += nMul * n;
                nMul *= pArr[i];
            }
            pArr[nDims + nIndex + 1] = pTop[-1 - bOnStack];
            pFrame->pC -= nDims + bOnStack + 1;
            break;
        case 0x63:  // push n copies of the top
            nByte0 = *pFrame->p10;
            pFrame->p10++;
            nByte1 = *pFrame->p10;
            pFrame->p10++;
            nByte2 = *pFrame->p10;
            pFrame->p10++;
            nByte3 = *pFrame->p10;
            pFrame->p10++;
            n = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            for (i = 0; i < n; i++) {
                pTop[i] = pTop[-1];
            }
            pFrame->pC += n;
            break;
        case 0x64:  // drop n words
            nByte0 = *pFrame->p10;
            pFrame->p10++;
            nByte1 = *pFrame->p10;
            pFrame->p10++;
            nByte2 = *pFrame->p10;
            pFrame->p10++;
            nByte3 = *pFrame->p10;
            pFrame->p10++;
            n = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            pFrame->pC -= n;
            break;
        case 0x65:  // push a copy of the top n words
            nByte0 = *pFrame->p10;
            pFrame->p10++;
            nByte1 = *pFrame->p10;
            pFrame->p10++;
            nByte2 = *pFrame->p10;
            pFrame->p10++;
            nByte3 = *pFrame->p10;
            pFrame->p10++;
            n = ((u32)nByte0 << 24) | (nByte1 << 16) | (nByte2 << 8) | nByte3;
            for (i = 0; i < n; i++) {
                pTop[i] = pTop[i - n];
            }
            pFrame->pC += n;
            break;
        case 0x66:  // fill an array with a value
        case 0x67:
        case 0x68:
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
            case 0x68:
                bOnStack = 0;
                uByte0 = *pFrame->p10;
                pFrame->p10++;
                uByte1 = *pFrame->p10;
                pFrame->p10++;
                uByte2 = *pFrame->p10;
                pFrame->p10++;
                uByte3 = *pFrame->p10;
                pFrame->p10++;
                u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
                uByte0 = *pFrame->p10;
                pFrame->p10++;
                uByte1 = *pFrame->p10;
                pFrame->p10++;
                n = (uByte0 << 8) | uByte1;
                pArr = (s32*)(u + pFrame->pC[(s16)n]);
                break;
            }
            nDims = pArr[0];
            for (i2 = 1; i2 <= nDims; i2++) {
                nMul *= pArr[i2];
            }
            for (i2 = 0; i2 < nMul; i2++) {
                pArr[nDims + i2 + 1] = pTop[-1 - bOnStack];
            }
            pFrame->pC -= bOnStack + 1;
            break;
        case 0x6F:  // entry n of the screen file's third table (0 past its end)
            u = pTop[-1];
            if (u < pScreen->pData->nEntriesC) {
                pTop[-1] = (s32)&pScreen->pData->pEntriesC[u];  // port: a stack word holds the pointer
            } else {
                pTop[-1] = 0;
            }
            break;
        case 0x59:  // switch a node on or off (fn_80168644)
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            uByte2 = *pFrame->p10;
            pFrame->p10++;
            uByte3 = *pFrame->p10;
            pFrame->p10++;
            u = (uByte0 << 24) | (uByte1 << 16) | (uByte2 << 8) | uByte3;
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            uByte1 = *pFrame->p10;
            pFrame->p10++;
            k = (uByte0 << 8) | uByte1;
            uByte0 = *pFrame->p10;
            pFrame->p10++;
            fn_80168644(pStudio, pScreen, uByte0, (void*)(u + pFrame->pC[(s16)k]), pTop[-1]);
            pFrame->pC--;
            break;
        case 0x73: {  // whether a rate function runs
            UISNodeInfo* pNodeInfo = (UISNodeInfo*)*--pFrame->pC;
            s32 nId = *--pFrame->pC;

            *pFrame->pC = fn_8016604C(pStudio, pNodeInfo, nId) < pStudio->nRateFns;
            pFrame->pC++;
            break;
        }
        case 0x72:  // send event 8 to a screen with a word
            n = *--pFrame->pC;
            u = *--pFrame->pC;
            data.aw[0] = u;
            data.aw[1] = u >> 16;
            data.aw[2] = pScreen->uGroup;
            data.aw[3] = pScreen->uScreen;
            data.au[3] = n;
            fn_80165B90(data.aw[2], data.aw[3], pStudio, 8, &data, 0, NULL);
            break;
        case 0x74: {  // set or clear a bit of the screen's event mask (-1: all)
            s32 bOn = *--pFrame->pC;
            u32 uBits = -1;

            n = *--pFrame->pC;
            if (n >= 0) {
                uBits = 1 << n;
            }
            if (bOn != 0) {
                pScreen->uMask |= uBits;
            } else {
                pScreen->uMask &= ~uBits;
            }
            break;
        }
        case 0x75:  // whether a bit of the event mask is clear
            n = *--pFrame->pC;
            *pFrame->pC = (pScreen->uMask & (1 << n)) == 0;
            pFrame->pC++;
            break;
        case 0x79:  // a text's buffer size, as a float
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL) {
                pFrame->pC[-1] = pText->nSize;
            } else {
                pFrame->pC[-1] = 0;
            }
            break;
        case 0x7A:  // a text to upper case
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL) {
                for (j = 0; j < pText->nSize; j++) {
                    if (pText->szText[j] >= 'a' && pText->szText[j] <= 'z') {
                        pText->szText[j] -= 0x20;
                    }
                }
            }
            break;
        case 0x7B:  // a text to lower case
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL) {
                for (j = 0; j < pText->nSize; j++) {
                    if (pText->szText[j] >= 'A' && pText->szText[j] <= 'Z') {
                        pText->szText[j] += 0x20;
                    }
                }
            }
            break;
        case 0x7C:  // replace every pFind in a text by pRep
            pRep = (UISText*)*--pFrame->pC;
            pFind = (UISText*)*--pFrame->pC;
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL && pFind != NULL && pRep != NULL) {
                nText = strlen(pText->szText);
                nFind = strlen(pFind->szText);
                nRep = strlen(pRep->szText);
                if (nText != 0 && nFind != 0 && nText >= nFind) {
                    nGrow = nRep - nFind;
                    for (j = 0; j < nText - nFind + 1; j++) {
                        bMatch = 1;
                        for (k = j; k < j + nFind; k++) {
                            if (k >= pText->nSize) {
                                bMatch = 0;
                                break;
                            }
                            if (pText->szText[k] != pFind->szText[k - j]) {
                                bMatch = 0;
                                break;
                            }
                        }
                        if (bMatch) {
                            if (nGrow > 0) {
                                for (k = pText->nSize - 1; k >= j + nGrow; k--) {
                                    pText->szText[k] = pText->szText[k - nGrow];
                                }
                            } else if (nGrow < 0) {
                                for (k = j + nRep; k <= pText->nSize + nGrow; k++) {
                                    pText->szText[k] = pText->szText[k - nGrow];
                                }
                            }
                            for (k = j; k < j + nRep && k < pText->nSize; k++) {
                                pText->szText[k] = pRep->szText[k - j];
                            }
                            j += nRep - 1;
                        }
                    }
                }
            }
            break;
        case 0x7D:  // hand a text to the game (pfnScreen28)
            pText = (UISText*)*--pFrame->pC;
            if (pText != NULL && pStudio->pfnScreen28 != NULL && pScreen != NULL) {
                // port: the text's address passed as the callback's word
                pStudio->pfnScreen28(pScreen->uGroup, pScreen->uScreen, (s32)pText->szText);
            }
            break;
        }
    }
    return 0;
}

// Runs fn_8016ABBC on p in pScreen, unless fn_8016AD54 finds p's switch already set as bOn
// asks (or not at all).
void fn_80168644(UIStudio* pStudio, UISScreen* pScreen, s32 nKind, void* p, s32 bOn) {
    s32 nFound;
    s32 nOn;

    if (pScreen == NULL || pScreen->pData == NULL || p == NULL) {
        return;
    }
    // EA passes the address of p itself as the info to look for; fn_8016AD54 writes the owner it
    // finds into it.
    nFound = fn_8016AD54(pScreen, (UISNodeInfo*)&p, 8, pScreen->pData->pNodes);
    nOn = bOn != 0;
    if (nFound != -1 && nFound != nOn) {
        fn_8016ABBC(pStudio, pScreen, nOn, nKind, p, 1);
    }
}

// Activates (bOn) a screen. With no p60 record open the current screen first gets event -5;
// then, unless every screen is being unloaded, the screen becomes current, its first node is
// switched on, the game hears of it (pfnScreen24) and it gets event -4. A screen waiting to be
// unloaded is refused with a warning.
void fn_801686F8(UIStudio* pStudio, u8 bOn, u16 uGroup, u16 uScreen) {
    char szMsg[512];
    UISScreen* pScreen;
    u32 nIndex;

    if (pStudio->n5C == 0) {
        pStudio->uFlags |= 2;
        UIStudio_Send(pStudio, &pStudio->stack64, 0, -5, 0, NULL, 0);
        pStudio->uFlags &= ~2;
    }
    if (bOn && !(u8)pStudio->bUnloadingAll) {
        nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
        if (nIndex < pStudio->nScreens) {
            pScreen = &pStudio->pScreens[nIndex];
            if (pScreen->bUnloading == 0) {
                pScreen->pData->pNodes[0].pInfo->u4 = 1;
                pScreen->pData->pNodes[0].pInfo->p0 = (void*)1;  // port: a marker, not an owner
                pStudio->nCurScreen = nIndex;
                if (pStudio->pfnScreen24 != NULL) {
                    pStudio->pfnScreen24(pScreen->uGroup, pScreen->uScreen);
                }
                pStudio->uFlags |= 2;
                UIStudio_Send(pStudio, &pStudio->stack64, 0, -4, 0, NULL, 0);
                pStudio->uFlags &= ~2;
            } else {
                sprintf(szMsg,
                        "Attempting to activate screen (Group ID: %d, Screen ID: %d) which is waiting to be "
                        "unloaded.\n",
                        uGroup, uScreen);
                lbl_80282A28(0, "UIStudio.c", 2942, szMsg);
                return;
            }
        }
    }
}

// Switches (bOn) the node with info pInfo in a screen on or off and runs its scripts for event
// -6 (on) or -7 (off), and those of the node that links to it by index. Switching one on first
// switches off the one fn_8016B6BC finds set. The scripts get nId and pInfo's place in the
// list p (a count, a word, then file offsets; -1 when not there).
void fn_80168918(UIStudio* pStudio, u8 bOn, s32 nId, UISNodeInfo* pInfo, s32* p, u16 uScreen, u16 uGroup) {
    s32 aArgs[2];
    UISScreenFile* pFile;
    UISNode* pNode;
    UISNode* pLinkNode;
    UISNode* pCheck;
    UISNodeInfo* pOther;
    u8* pScript;
    u16 nIndex;
    u32 nNode;
    u32 n;
    s32 i;
    s32 nSlot;
    UISScreen* pScreen;
    s32 nEvent;

    nSlot = -1;
    nIndex = fn_8016C6C4(pStudio, uGroup, uScreen);
    pNode = NULL;
    if (nIndex >= pStudio->nScreens) {
        return;
    }
    pScreen = &pStudio->pScreens[nIndex];
    nEvent = bOn == 1 ? -6 : -7;
    pFile = pScreen->pData;
    nNode = pFile->nNodes;
    while (nNode-- != 0) {
        pCheck = &pFile->pNodes[nNode];
        if (pCheck->pInfo == pInfo) {
            pNode = pCheck;
            break;
        }
    }
    pLinkNode = NULL;
    n = pFile->nNodes;
    while (n-- != 0) {
        pCheck = &pFile->pNodes[n];
        for (i = 0; i < (s32)pCheck->nHandlers; i++) {
            if (!(pCheck->pHandlers[i].uFlags & 0xC000)
                && (pCheck->pHandlers[i].uFlags & 0x2FFF) == (s16)nNode) {
                pLinkNode = pCheck;
                break;
            }
        }
    }
    if (bOn == 1) {
        pOther = fn_8016B6BC(pScreen, pInfo);
        if (pOther != NULL) {
            fn_80168918(pStudio, 0, 0, pOther, NULL, uScreen, uGroup);
        }
    }
    // EA bug: pNode is NULL when no node of the screen has pInfo; nothing checks it.
    pScript = fn_8016C674(pNode, nEvent);
    pNode->pInfo->u4 = bOn;
    if (p != NULL) {
        nSlot = p[0];
        while (nSlot-- != 0) {
            if (pInfo == (UISNodeInfo*)((u8*)pScreen->pData + p[nSlot + 2])) {
                break;
            }
        }
    }
    aArgs[0] = nId;
    aArgs[1] = nSlot;
    if (pScript != NULL) {
        fn_8016C270(pStudio, pScreen, pNode->pInfo, &pStudio->stack64, pScript, 2, aArgs, 0, NULL, 0, 0,
                    NULL);
    }
    if (pLinkNode != NULL) {
        pScript = fn_8016C614(pLinkNode, nNode, nEvent);
        if (pScript != NULL) {
            fn_8016C270(pStudio, pScreen, pLinkNode->pInfo, &pStudio->stack64, pScript, 2, aArgs, 0, NULL, 0,
                        0, NULL);
        }
    }
}

// Runs the queued events, then sends event uEvent to every screen.
void fn_80168B80(UIStudio* pStudio, u32 uEvent) {
    s32 i;
    s32 n;
    UISScreen* pScreen;
    u8 bOut;

    fn_80165528(pStudio, 0);
    n = pStudio->nScreens;
    for (i = 0; i < n; i++) {
        pScreen = &pStudio->pScreens[i];
        bOut = 0;
        pStudio->uFlags |= 2;
        fn_8016A2D4(pStudio, pScreen, &pStudio->stack64, 0, uEvent, -10, 0, NULL, &bOut);
        pStudio->uFlags &= ~2;
    }
}

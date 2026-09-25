// uiProcessInterface.c (EA's name, from its asserts; also in EA's 2002 source tree): runs the
// UI (start-up, the menus and a round's screens). Prepares the loaded UI file, reads the
// controllers for it, passes the UI's commands to whichever part of the game is running, fades
// the screen out, and formats numbers for it.

#include "golfer.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "camera.h"
#include "frontend/uistudio.h"

u8 lbl_80281F19;
u8 lbl_80281F1A;                // set: fn_8008FD60 passes events to the UI
u8 lbl_80281F1B;

void fn_8008F80C(s32 p0, s32 p1);
s32 fn_80092BC4();
s32 fn_800934F8();
s32 fn_800BA038();
void fn_80090400(FrontEnd* pFE);
void fn_80090664(void);
void fn_8008FE88(FrontEnd* pFE);
TexEntry* fn_80090904(TexBank* pBank, u64 uHash);
void fn_800E573C(void);         // GameMessages.c
void fn_800E5798(void);         // GameMessages.c
void fn_800E5708(void);         // GameMessages.c
void fn_80092BA0(void);         // uiText.c
void fn_8008EC30(void);         // uiLoadFile.c
void fn_800B9FF0(void);
void fn_80037FB4(u8 a, f32* pColor);    // a full-screen colour (GoPostFx.c)
void DEMO_Start(void);                 // BootCourse.c
void fn_80077340(void);                 // FE_Manager.c
void fn_80077344(void);                 // FE_Manager.c
void fn_80077348(void);                 // FE_Manager.c
void fn_80016B6C(f32 x, f32 y);
void FO_vSetCurrentAddMode(s32 nMode);
void fn_80012C54_SetWordWrap(s32 v);
void UFont_ResetContext(void);
void fn_800908D4(f32 x0);
void fn_80090890(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg);
void fn_80090894(u16 uGroup, u16 uScreen, s32 n);
void fn_800908BC(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void fn_800908C0(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void fn_800908C4(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void fn_800908C8(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void fn_800908CC(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void fn_800908D0(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);

u8 fn_8008F39C(void) {
    return lbl_80281F1B;
}

// Add uBase to n words, nStride words apart.
void fn_8008F3A4(u32* p, uptr uBase, int nStride, u32 n) {
    u32 i;

    for (i = 0; i < n; i++) {
        *p += uBase;
        p += nStride;
    }
}

// fake match: MWCC evaluates p8->nCount before p8->apTables unless apTables is evaluated in a
// boolean condition first
static inline void* fn_8008F488_Read(void* p) {
    if (!p && !p) {
    }
    return p;
}

// Turn the UI file's offsets into pointers.
// port: the file stores 32-bit offsets in its pointer fields, as the GameCube's pointers are.
void fn_8008F488(FrontEnd* pFE) {
    u32 i;
    uptr uBase = (uptr)pFE->pFile;

    pFE->pFile->p4 = (UIFilePairs*)((uptr)pFE->pFile->p4 + uBase);
    pFE->pFile->p8 = (UIFileTables*)((uptr)pFE->pFile->p8 + uBase);
    fn_8008F3A4((u32*)pFE->pFile->p4->aPairs, uBase, 1, pFE->pFile->p4->nCount * 2);
    fn_8008F3A4((u32*)fn_8008F488_Read(pFE->pFile->p8->apTables), uBase, 1, pFE->pFile->p8->nCount);
    for (i = 0; i < pFE->pFile->p8->nCount; i++) {
        fn_8008F3A4((u32*)pFE->pFile->p8->apTables[i]->apEntries, uBase, 1,
                    pFE->pFile->p8->apTables[i]->nCount);
    }
}

// Pass a UI command to the part of the game that is running: by the session's game type,
// start-up (1), the menus (3) or a round (4 to 8).
void fn_8008F568(s32 nCmd, s32 unused1, s32 unused2, s32 unused3, s32 a, s32 b) {
    // port: the studio passes the addresses of the command's values and answer as 32-bit words
    if (gSession.nGameType == 1) {
        fn_800B1D3C(nCmd, (MsgArg*)a, (MsgArg*)b);
    }
    if (gSession.nGameType == 3) {
        fn_80079E6C(nCmd, (MsgArg*)a, (MsgArg*)b);
    } else if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
        fn_800850E4(nCmd, (MsgArg*)a, (MsgArg*)b);
    }
}

// The studio's UISLoadFn: screen uScreen's data from the UI file's pairs (0 past the end). The
// group is ignored.
void* fn_8008F610(u16 uGroup, u16 uScreen) {
    UIFilePair* pPair;

    if (uScreen >= lbl_80281F1C->pFile->p4->nCount) {
        return NULL;
    }
    pPair = &lbl_80281F1C->pFile->p4->aPairs[uScreen];
    return pPair->p4;
}

// The studio's UISUnloadFn: nothing to do, the screens' data stays in the UI file.
void fn_8008F644(u16 uGroup, u16 uScreen, void* pData) {
}

// Run and draw the UI for nTicks (while lbl_80281F1A is set), then step lbl_801D880C: counting
// 0..2 up, and past 2 it is reset and its n4 sent to the menus (0x23) or the round (0x24).
void fn_8008F648(s32 nTicks) {
    s32 aArgs[1];

    if (lbl_80281F1A) {
        fn_8001425C(0);
        fn_80035118(4, 5);
        fn_80012F34(0);
        fn_80012F50(1, 6, 1);
        fn_80012EF8();
        if (lbl_80281370 && gSession.nGameType == 3) {
            fn_80091454();
        }
        fn_80014194(NULL);
        fn_80016B6C(1.0f / 512.0f, 1.0f / 448.0f);
        FO_vSetCurrentAddMode(1);
        fn_80012C54_SetWordWrap(1);
        fn_800908D4(0.85f);
        if (lbl_80281F1C != NULL) {
            fn_80168C24(lbl_80281F1C->pHandler, nTicks);
        }
        fn_800908D4(1.0f);
        fn_80012C54_SetWordWrap(0);
        fn_80016B6C(1.0f, 1.0f);
        fn_80012F34(1);
        fn_80012F50(1, 6, 0x80);
        fn_80012F18(3);
        fn_80012EF8();
        if (lbl_801D880C.n0 <= 2 && lbl_801D880C.n0 >= 0) {
            lbl_801D880C.n0++;
        } else if (lbl_801D880C.n0 > 2) {
            lbl_801D880C.n0 = -1;
            fn_80005AE8(aArgs, 0, sizeof(aArgs));
            aArgs[0] = lbl_801D880C.n4;
            if (gSession.nGameType == 3) {
                fn_8016B09C(lbl_80281F1C->pHandler, 0x23, 1, aArgs);
            }
            if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
                fn_8016B09C(lbl_80281F1C->pHandler, 0x24, 1, aArgs);
            }
        }
        UFont_ResetContext();
    }
}

void fn_8008F80C(s32 n, s32 b) {
    lbl_801D87C0.a30[n] = b;
}

// Read the controllers for the UI. The main stick works the D-pad in start-up, the menus and (when
// fn_800E415C says so) game type 6. In a round, nothing happens while a player's view runs a
// scripted camera (fn_80063C90, or script camera 3) other than camera 0x15. Each plugged-in
// controller's buttons (the stick's directions folded into the D-pad bits) are compared with last
// frame's, a held button repeating every 9 frames; each newly pressed button sends its
// lbl_80189B58 event to the UI (not during the fade or while a movie is queued). In the menus,
// event 0x34 (which stops that input) is sent with fewer than two controllers in mode 0x1A, or in
// mode 7 without CPU players; otherwise 0x2D while any is plugged in.
void fn_8008F820(void) {
    f32 fOne;
    s32 aArgs[1];
    u32 aPressed[8];            // fake match: 4 are used; the stack frame holds 8
    u32 aButtons[8];            // fake match: likewise
    View* pView;
    u32 uMask;
    int i;
    int j;
    int k;
    UIButtonEvent* pEvent;

    fOne = 1.0f;
    if (gSession.nGameType == 3 || gSession.nGameType == 1 ||
        (gSession.nGameType == 6 && fn_800E415C())) {
        fn_800130EC(1);
    } else {
        fn_800130EC(0);
    }
    if (gSession.nGameType >= 4 && gSession.nGameType <= 8 && gSession.nPaused == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pView = fn_80017028(gPlayers[i].nView[0]);
            if ((fn_80063C90(pView) || pView->script.nCamera == 3) && pView->nCurCamera != 0x15) {
                return;
            }
        }
    }
    lbl_801D87C0.n38 = 0;
    fn_80005AE8(aArgs, 0, sizeof(aArgs));
    for (i = 0; i < 4; i++) {
        if (fn_80013070(i)) {
            lbl_801D87C0.a1[i] = 1;
            lbl_801D87C0.n34 = 0;
            lbl_801D87C0.n38++;
        } else {
            lbl_801D87C0.a1[i] = 0;
        }
        if (lbl_801D87C0.a1[i]) {
            aButtons[i] = fn_800136DC(i);
            if (aButtons[i] & 0x40000) {
                aButtons[i] |= 4;
            }
            if (aButtons[i] & 0x80000) {
                aButtons[i] |= 8;
            }
            if (aButtons[i] & 0x20000) {
                aButtons[i] |= 2;
            }
            if (aButtons[i] & 0x10000) {
                aButtons[i] |= 1;
            }
            if (lbl_801D87C0.a8[i] != aButtons[i]) {
                lbl_801D87C0.a18[i] = 0;
            } else if (lbl_801D87C0.a18[i] > 8) {
                lbl_801D87C0.a18[i] = 0;
                lbl_801D87C0.a8[i] = 0;
            }
            lbl_801D87C0.a18[i]++;
            aPressed[i] = aButtons[i] & ~lbl_801D87C0.a8[i];
            lbl_801D87C0.a8[i] = aButtons[i];
        }
        lbl_801D87C0.a28[i] = lbl_801D87C0.a1[i];
    }
    if (lbl_801D87C0.n38 == 0) {
        lbl_801D87C0.n34++;
    }
    if (lbl_801D87C0.n38 > 0 && gSession.nGameType == 3 &&
        ((Game_GetMode() != 7 && Game_GetMode() != 0x1A) || lbl_801D87C0.n38 >= 2 ||
         lbl_801D7148.aCPU[0] || lbl_801D7148.aCPU[1])) {
        if (lbl_80281368 != -1) {
            lbl_80281EE0->b86 = lbl_80281368;
            lbl_80281368 = -1;
        }
        lbl_801D87C0.b49 = 0;
        fn_8016B09C(lbl_80281F1C->pHandler, 0x2D, 1, aArgs);
        lbl_801D87C0.b40 = 0;
    }
    if (((Game_GetMode() == 7 && !lbl_801D7148.aCPU[0] && !lbl_801D7148.aCPU[1]) ||
         Game_GetMode() == 0x1A) &&
        lbl_801D87C0.n38 < 2 && gSession.nGameType == 3) {
        if (lbl_80281368 == -1) {
            lbl_80281368 = lbl_80281EE0->b86;
        }
        fn_8016B09C(lbl_80281F1C->pHandler, 0x34, 1, aArgs);
        lbl_801D87C0.b40 = 1;
        lbl_801D87C0.b49 = 1;
    }
    aArgs[0] = 0;
    if (lbl_801D87C0.b0 == 0 && fn_80077148() && lbl_801D87C0.b40 == 0) {
        for (k = 0; k < 4; k++) {
            if (lbl_801D87C0.a1[k] && lbl_801D87C0.a30[k]) {
                if (gSession.nGameType != 6 || (gSession.nPaused != 2 && gSession.nPaused != 3)) {
                    pEvent = lbl_80189B58;
                    for (j = 0; j < UI_NUM_BUTTON_EVENTS; j++) {
                        if (pEvent->uMask & aPressed[k]) {
                            fn_80168DB0(lbl_80281F1C->pHandler, k, pEvent->nEvent, 1, &fOne, 0);
                        }
                        pEvent++;
                    }
                    if (aButtons[k] != 0 && gSession.nGameType == 3) {
                        fn_8016B09C(lbl_80281F1C->pHandler, 0x22, 1, aArgs);
                    }
                }
                if (gSession.nGameType == 6) {
                    fn_800E5240(k);
                }
                if (gSession.nGameType == 6) {
                    uMask = fn_800142AC(0x20, 1);
                    if (fn_800136DC(k) & uMask) {
                        lbl_80189B38[k]++;
                    } else {
                        lbl_80189B38[k] = 0;
                    }
                }
                if (lbl_80189B38[k] > 10) {
                    fn_800E4F88(k);
                    lbl_80189B38[k] = 0;
                }
            }
        }
    }
}

// Passes an event to the UI (while lbl_80281F1A is set). With lbl_80281F19 set it then shuts the UI
// down (fn_80090400) and sets lbl_80281F1B; otherwise fn_8008F820 runs.
void fn_8008FD60(u32 uEvent) {
    if (lbl_80281F1A) {
        if (lbl_80281F1C != NULL) {
            fn_80168B80(lbl_80281F1C->pHandler, uEvent);
        }
        if (lbl_80281F19) {
            fn_80090400(lbl_80281F1C);
            lbl_80281F19 = 0;
            lbl_80281F1A = 0;
            lbl_80281F1B = 1;
        } else if (lbl_80281F1C != NULL) {
            fn_8008F820();
        }
    }
}

// Find the UI file's table whose first entry is of kind 0x10 (the last, if several), keep it in
// p14 and turn its entries' p8 offsets into pointers; with none, p14 is NULL.
void fn_8008FDDC(FrontEnd* pFE) {
    uptr uBase = (uptr)pFE->pFile;
    UIColorTable* pTable;
    u8 bFound = 0;
    int nTables;
    int i;

    nTables = pFE->pFile->p8->nCount;
    for (i = 0; i < nTables; i++) {
        pTable = pFE->pFile->p8->apTables[i];

        // fake match: the original tests the count unsigned here (cmplwi), signed below
        if ((u32)pTable->nCount != 0 && pTable->apEntries[0]->u0 == 0x10) {
            pFE->p14 = pTable;
            bFound = 1;
        }
    }
    if (bFound) {
        for (i = 0; i < pFE->p14->nCount; i++) {
            pFE->p14->apEntries[i]->p8 = (u8*)((uptr)pFE->p14->apEntries[i]->p8 + uBase);
        }
    } else {
        pFE->p14 = NULL;
    }
}

// Resolve the UI file's entries by their names: kind 1 (except in game type 3) to the texture of
// that name in the texture bank fn_8008FFF0 picks (none for -1), kind 2 (with a name list in pC)
// to the same name in that list (p8 cleared; the table's index noted in lbl_801D87C0.n3C).
void fn_8008FE88(FrontEnd* pFE) {
    u32 k;
    UIColorTable* pTable;
    UIFileEntry* pEntry;
    u64 uHash;
    char* szName;
    int nBank;
    u32 i;
    u32 j;

    for (i = 0; i < pFE->pFile->p8->nCount; i++) {
        pTable = pFE->pFile->p8->apTables[i];
        for (j = 0; j < pTable->nCount; j++) {
            pEntry = pTable->apEntries[j];
            szName = pEntry->szC;
            if (pEntry->u0 == 1) {
                if (gSession.nGameType != 3) {
                    uHash = fn_8000BEE4(szName);
                    nBank = fn_8008FFF0(szName);
                    if (nBank != -1) {
                        pEntry->p4 = fn_80090904(lbl_80281F1C->p8->ap4[nBank], uHash);
                    }
                }
            } else if (pEntry->u0 == 2 && pFE->pC != NULL) {
                lbl_801D87C0.n3C = i;
                for (k = 0; k < pFE->pC->nCount; k++) {
                    if (strcmp(pEntry->szC, pFE->pC->apNames[k]) == 0) {
                        pEntry->p4 = pFE->pC->apNames[k];
                        pEntry->p8 = NULL;
                        break;
                    }
                }
            }
        }
    }
}

// A UI name starting "tu": 1 in a lesson, -1 otherwise; 0 for any other name, and
// always 0 when the game type is 1.
int fn_8008FFF0(const char* szName) {
    if (gSession.nGameType == 1) return 0;
    if (szName[0] == 't' && szName[1] == 'u') {
        if (fn_80100294()) return 1;
        return -1;
    }
    return 0;
}

// Start the front end with the UI set szSet: take what uiLoadFile.c loaded, resolve the UI file,
// set up the studio (its handlers 0 to 8 and the game's callbacks), hand it the file's
// "GlobalScript" screen if there is one and show the first screen (start-up passes it two zero
// words).
FrontEnd* fn_8009005C(char* szSet) {
    s32 aArgs[2];
    int i;

    lbl_80281F19 = 0;
    lbl_80281F1A = 1;
    lbl_80281F1B = 0;
    fn_8008EC60(szSet);
    lbl_80281F1C = fn_80009B34(sizeof(FrontEnd), 2, 16, "uiProcessInterface.c", 904);
    lbl_80281F1C->f18 = 0.0f;
    lbl_80281F1C->pFile = fn_8008F0C0(szSet);
    lbl_80281F1C->p8 = fn_8008F0F0(szSet);
    lbl_80281F1C->pC = fn_8008F15C(szSet);
    lbl_80281F1C->p10 = fn_8008F18C(szSet);
    fn_8009349C();
    lbl_801D87C0.a2C[0] = 0;
    lbl_801D87C0.a2C[1] = 0;
    lbl_801D87C0.a2C[2] = 0;
    lbl_801D87C0.a2C[3] = 0;
    lbl_801D87C0.b0 = 0;
    lbl_801D87C0.fFade = 0.0f;
    lbl_801D880C.n4 = 0;
    lbl_801D880C.n0 = -1;
    fn_8008F488(lbl_80281F1C);
    fn_8008FE88(lbl_80281F1C);
    fn_8008FDDC(lbl_80281F1C);
    lbl_80281F1C->pHandler = fn_80009B34(fn_80169D90(10, 9, 256, 2, 2048, 128), 2, 16,
                                         "uiProcessInterface.c", 943);
    fn_80169C0C(lbl_80281F1C->pHandler, 10, 9, 256, 2, 2048, 128, 16);
    fn_80169B0C(lbl_80281F1C->pHandler, 0, (UISHandlerFn)fn_800914DC);
    fn_80169B0C(lbl_80281F1C->pHandler, 1, fn_800908BC);
    fn_80169B0C(lbl_80281F1C->pHandler, 2, fn_800908C0);
    fn_80169B0C(lbl_80281F1C->pHandler, 3, fn_800908C4);
    fn_80169B0C(lbl_80281F1C->pHandler, 4, fn_800908C8);
    fn_80169B0C(lbl_80281F1C->pHandler, 5, fn_800908CC);
    fn_80169B0C(lbl_80281F1C->pHandler, 6, fn_800908D0);
    fn_80169B0C(lbl_80281F1C->pHandler, 7, (UISHandlerFn)fn_800929E4);
    fn_80169B0C(lbl_80281F1C->pHandler, 8, (UISHandlerFn)fn_80103684);
    fn_80169B30(lbl_80281F1C->pHandler, fn_8008F610, fn_8008F644);
    fn_80169B28(lbl_80281F1C->pHandler, (UISTransformFn)fn_80093280);
    fn_80169B44(lbl_80281F1C->pHandler, fn_8008F568);
    // fake match: the original compares the count signed here (cmpw), unsigned in fn_8008F610
    for (i = 0; i < (s32)lbl_80281F1C->pFile->p4->nCount; i++) {
        if (strcmp(lbl_80281F1C->pFile->p4->aPairs[i].p0, "GlobalScript") == 0) {
            fn_80169520(lbl_80281F1C->pHandler, lbl_80281F1C->pFile->p4->aPairs[i].p4);
            break;
        }
    }
    if (gSession.nGameType != 1) {
        fn_801694A0(lbl_80281F1C->pHandler, 0, 0, 0, NULL);
    } else {
        aArgs[0] = 0;
        aArgs[1] = 0;
        fn_801694A0(lbl_80281F1C->pHandler, 0, 0, 2, aArgs);
    }
    if (gSession.nGameType == 3) {
        fn_8008D8F4();
    }
    fn_80168F5C(lbl_80281F1C->pHandler, 0, 0);
    fn_80169B3C(lbl_80281F1C->pHandler, fn_80090894);
    fn_80165C6C(fn_80090890);
    return lbl_80281F1C;
}

// Shut the front end down: in game type 1 with no nC, fe_movies.c's fn_80091EE8; in game type 3,
// every lbl_801D8890 entry whose lbl_801D8ED0 word is set gets b0 set and b1 cleared, and the
// menus' data is brought back and freed (fn_8008F294, fn_8008F24C). Then everything the front
// end loaded is freed, and the front end itself.
void fn_80090400(FrontEnd* pFE) {
    int i;

    fn_80090B10();
    if (gSession.nGameType == 1 && gSession.nC == 0) {
        fn_80091EE8();
    } else if (gSession.nGameType == 3) {
        for (i = 0; i < FE_NUM_801D8890; i++) {
            if (lbl_801D8ED0[i] != 0) {
                lbl_801D8890[i].b0 = 1;
                lbl_801D8890[i].b1 = 0;
            }
        }
        lbl_80281370 = 1;
        fn_80091454();
        fn_8008F294();
        fn_8008F24C();
    }
    fn_80169B4C(lbl_80281F1C->pHandler);
    fn_8008F194(pFE->p10);
    fn_8008F164(pFE->pC);
    fn_8008F0FC(pFE->p8);
    fn_8008F0C8(pFE->pFile);
    fn_80009E70(lbl_80281F1C->pHandler);
    fn_80009E70(lbl_80281F1C);
    lbl_80281F1C = NULL;
}

// Resets the UI's controller state (all four controllers enabled); also calls fn_80092BA0,
// fn_8008EC30 (nothing loaded), fn_800B9FF0 and fn_800E5708.
void fn_800905A8(void) {
    s32 i;

    fn_80092BA0();
    fn_8008EC30();
    lbl_801D87C0.n34 = 0;
    lbl_801D87C0.n38 = 0;
    lbl_801D87C0.b49 = 0;
    lbl_801D87C0.b40 = 0;
    for (i = 0; i < 4; i++) {
        lbl_801D87C0.a18[i] = 0;
        lbl_801D87C0.a2C[i] = 0;
        lbl_801D87C0.a30[i] = 1;
    }
    lbl_801D87C0.b48 = 0;
    fn_800B9FF0();
    fn_800E5708();
}

// Sends the pending messages (GameMessages.c): game type 3 has its own set.
void fn_80090628(void) {
    if (gSession.nGameType == 3) {
        fn_800E573C();
    } else {
        fn_800E5798();
    }
}

void fn_80090664(void) {
    if (lbl_80281F1C != NULL) {
        fn_80090400(lbl_80281F1C);
    }
    fn_800934F8();
    fn_80092BC4();
    fn_800BA038();
}

// The fade to black (while lbl_801D87C0.b0 is set): draw it, 0.05 darker each frame. Once it is
// black, unpause; start-up (1) shuts the UI down (fn_8008FD60 sees lbl_80281F19) and sets
// gSession.b12, the menus (3) shut it down, start the demo when gSession.a8[0] is set and call
// FE_Manager.c's function for mode 0x17, 0x18 or 4, and other game types set gSession.b12.
void fn_8009069C(void) {
    f32 aColor[4];

    if (lbl_801D87C0.b0 == 0) return;
    aColor[0] = 0.0f;
    aColor[1] = 0.0f;
    aColor[2] = 0.0f;
    aColor[3] = lbl_801D87C0.fFade;
    fn_80037FB4(1, aColor);
    lbl_801D87C0.fFade += 0.05f;
    if (lbl_801D87C0.fFade >= 1.0f) {
        lbl_801D87C0.fFade = 1.0f;
        gSession.nPaused = 0;
        if (gSession.nGameType == 1) {
            lbl_80281F19 = 1;
            gSession.b12 = 1;
        } else if (gSession.nGameType == 3) {
            lbl_80281F19 = 1;
            if (gSession.a8[0] != 0) {
                DEMO_Start();
            }
            if (Game_GetMode() == 0x17) {
                fn_80077340();
            } else if (Game_GetMode() == 0x18) {
                fn_80077344();
            } else if (Game_GetMode() == 4) {
                fn_80077348();
            }
        } else {
            gSession.b12 = 1;
        }
    }
}

void fn_800907AC(int nValue, char* szOut) {
    char aBuf[128];
    int nDigit = 1;
    u32 nLen;
    u32 nCommas;
    u32 nTotal;
    int i;

    sprintf(szOut, "%d", nValue);
    nLen = strlen(szOut);
    nCommas = nLen / 3;
    if (nLen % 3 == 0) {
        nCommas--;
    }
    nTotal = nLen + nCommas;
    aBuf[nTotal] = '\0';
    for (i = nTotal - 1; i >= 0; i--) {
        if (nDigit % 4 == 0) {
            aBuf[i] = ',';
        } else {
            aBuf[i] = szOut[nLen - 1];
            nLen--;
        }
        nDigit++;
    }
    sprintf(szOut, aBuf);       // EA: the result is used as a format; it holds only digits, '-' and ','
}

// The studio's report callback (UISReportFn): the retail game prints nothing.
void fn_80090890(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg) {
}

// The studio's UISScreenDataFn: nothing to do.
void fn_80090894(u16 uGroup, u16 uScreen, s32 n) {
}

void fn_80090898(void) {
    fn_8008FE88(lbl_80281F1C);
}

// The studio's handlers 1 to 6 (fn_8009005C): they do nothing.
void fn_800908BC(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

void fn_800908C0(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

void fn_800908C4(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

void fn_800908C8(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

void fn_800908CC(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

void fn_800908D0(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// ---- sweep code (not yet cleaned up) ----

int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000
TexEntry* fn_800107E4(TexBank* pBank, int nTex);  // LLTexGrp.c

void fn_800908D4(f32 x0) {
    UFontContext* pCtx;
    pCtx = FO_spGetCurrentPacket();
    pCtx->fB4 = x0;
}

// The texture in pBank whose name hashes to uHash.
TexEntry* fn_80090904(TexBank* pBank, u64 uHash) {
    int nTex = fn_8001005C(pBank, uHash);

    return fn_800107E4(pBank, nTex);
}

// ---- end of sweep code ----

// uiProcessInterface.c (EA's name, from its asserts; also in EA's 2002 source tree; TW07's
// successor is ui_core's uiAptProcessInterface.c, which keeps UI_OpenInterface, UI_vInitModule,
// UI_vCloseModule, UI_ExitFade and UI_GetMoneyString): runs the UI of start-up, the menus and a
// round on EA's UI Studio library. It opens a UI set (UI_OpenInterface: the loaded UI file made
// ready, the studio set up with the game's callbacks), runs and draws it each frame
// (UI_UpdateInterface, UI_DrawInterface), reads the controllers for it (UI_ReadControllers), passes
// its commands to the part of the game that is running (UI_RunGameMessage: start-up's handlers,
// FE_RunGameMessage in the menus, IG_RunGameMessage in a round), fades the screen out when it is
// left (UI_ExitFade) and closes it (UI_CloseInterface). Also the comma formatting of amounts
// (UI_GetMoneyString).

#include "golfer.h"
#include "game.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "camera.h"
#include "frontend/uistudio.h"

// Defined here (declared in game/frontend.h); .sbss in reverse address order.
FrontEnd* gpFrontEnd;           // the open UI (UI_OpenInterface), NULL when none
u8 gbUIClosed;                  // the UI has shut down after its exit fade (UI_IsClosed)
u8 gbUIRunning;                 // the UI is open: UI_UpdateInterface and UI_DrawInterface run it
u8 gbUICloseRequested;          // the exit fade is black: UI_UpdateInterface closes the UI
u8 gbPausedWithoutScoreCard;    // GameUICommands.c GM_vPauseGame: the scorecard was not up; not read

// .bss, reverse address order (declared in frontend/fe.h)
UIDelayedHint gUIDelayedHint;   // a value a message hands back to the UI as a hint 3 frames later
UIState gUIState;               // the UI's controller input, exit fade and picture table

// Per controller: frames Controller_GetButtonMask(0x20, 1)'s button has been held in play; past 10
// UI_ReadControllers sends GUI_SendButtonHeld.
s32 gUIButtonHeldFrames[8] = {0};
// The UI event each controller button sends when pressed (UI_ReadControllers).
UIButtonEvent gUIButtonEvents[UI_NUM_BUTTON_EVENTS] = {
    {0x1000, 0x0}, {0x800, 0x6}, {0x100, 0x7}, {0x8, 0x2},
    {0x4, 0x3},    {0x1, 0x4},   {0x2, 0x5},   {0x200, 0x8},
    {0x400, 0x9},  {0x40, 0xA},  {0x10, 0xC},  {0x0, 0xE},
    {0x20, 0xB},   {0x10, 0xD},  {0x0, 0xF},   {0x0, 0x1},
};
s8 gSavedCrAPHidden = -1;      // gpCrAPState->bHidden put aside while hint 0x34 is up (-1: none)

void UI_SetControllerEnabled(s32 n, s32 b);
void UIText_SetFontDrawQueued(void);         // uiText.c
void UITransform_Shutdown(void);
void UI_EATraxFreeLogo(void);         // uiEATrax.c
void UI_CloseInterface(FrontEnd* pFE);
void UI_vCloseModule(void);
void UI_ResolveFileEntries(FrontEnd* pFE);
TexEntry* UI_FindTexture(TexBank* pBank, u64 uHash);
void GameMsg_SendPendingMenus(void);         // GameMessages.c
void GameMsg_SendPending(void);         // GameMessages.c
void GameMsg_ClearPending(void);         // GameMessages.c
void UIText_SetFontDrawAtOnce(void);         // uiText.c
void UI_ResetLoadedFiles(void);         // uiLoadFile.c
void UI_EATraxReset(void);
void fn_80037FB4(u8 a, f32* pColor);    // a full-screen colour (GoPostFx.c)
void DEMO_Start(void);                 // BootCourse.c
void FE_PlayPGATourMovie(void);         // FE_Manager.c
void FE_PlayRTEMovie(void);             // FE_Manager.c
void FE_PlayLadderMovie(void);          // FE_Manager.c
void fn_80016B6C(f32 x, f32 y);
void FO_vSetCurrentAddMode(s32 nMode);
void fn_80012C54_SetWordWrap(s32 v);
void UFont_ResetContext(void);
void UI_SetTextLineSpacing(f32 fSpacing);
int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000
TexEntry* fn_800107E4(TexBank* pBank, int nTex);  // LLTexGrp.c
void UI_ReportUISError(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg);
void UI_ScreenDrawDebug(u16 uGroup, u16 uScreen, s32 n);
void UI_BlankProcess1(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void UI_BlankProcess2(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void UI_BlankProcess3(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void UI_BlankProcess4(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void UI_BlankProcess5(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);
void UI_BlankProcess6(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4);

// Whether the UI has shut down after its exit fade (gbUIClosed, set by UI_UpdateInterface);
// gomainloop's main loop stops on it.
u8 UI_IsClosed(void) {
    return gbUIClosed;
}

// Relocate n offsets: add uBase to n words, nStride words apart (UI_RelocateFile turns the UI
// file's offsets into pointers with it).
void UI_RelocateOffsets(u32* p, uptr uBase, int nStride, u32 n) {
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

// Turn the UI file's offsets into pointers: its pair list and table list, both words of every pair,
// the table pointers and each table's entry pointers (the entries' own fields are done later by
// UI_FindColorTable and UI_ResolveFileEntries).
// port: the file stores 32-bit offsets in its pointer fields, as the GameCube's pointers are.
void UI_RelocateFile(FrontEnd* pFE) {
    u32 i;
    uptr uBase = (uptr)pFE->pFile;

    pFE->pFile->p4 = (UIFilePairs*)((uptr)pFE->pFile->p4 + uBase);
    pFE->pFile->p8 = (UIFileTables*)((uptr)pFE->pFile->p8 + uBase);
    UI_RelocateOffsets((u32*)pFE->pFile->p4->aPairs, uBase, 1, pFE->pFile->p4->nCount * 2);
    UI_RelocateOffsets((u32*)fn_8008F488_Read(pFE->pFile->p8->apTables), uBase, 1, pFE->pFile->p8->nCount);
    for (i = 0; i < pFE->pFile->p8->nCount; i++) {
        UI_RelocateOffsets((u32*)pFE->pFile->p8->apTables[i]->apEntries, uBase, 1,
                           pFE->pFile->p8->apTables[i]->nCount);
    }
}

// The studio's message callback (UISMessageFncT, registered by UI_OpenInterface): runs UI command
// nCmd with the addresses of its arguments and its answer, by the session's game type: start-up (1)
// through the start-up handlers (Startup_RunGameMessage), the menus (3) through FE_RunGameMessage, a round (4
// to 8) through IG_RunGameMessage; other game types drop it. The group, screen and argument count
// are not used.
void UI_RunGameMessage(s32 nCmd, s32 nGroup, s32 nScreen, s32 nParams, s32 nArgsAddr, s32 nResultAddr) {
    // port: the studio passes the addresses of the command's values and answer as 32-bit words
    if (gSession.nGameType == 1) {
        Startup_RunGameMessage(nCmd, (MsgArg*)nArgsAddr, (MsgArg*)nResultAddr);
    }
    if (gSession.nGameType == 3) {
        FE_RunGameMessage(nCmd, (MsgArg*)nArgsAddr, (MsgArg*)nResultAddr);
    } else if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
        IG_RunGameMessage(nCmd, (MsgArg*)nArgsAddr, (MsgArg*)nResultAddr);
    }
}

// The studio's UISResLoadFncT: screen uScreen's data from the UI file's pairs (0 past the end). The
// group is ignored.
void* UI_ResLoad(u16 uGroup, u16 uScreen) {
    UIFilePair* pPair;

    if (uScreen >= gpFrontEnd->pFile->p4->nCount) {
        return NULL;
    }
    pPair = &gpFrontEnd->pFile->p4->aPairs[uScreen];
    return pPair->p4;
}

// The studio's UISResUnloadFncT: nothing to do, the screens' data stays in the UI file.
void UI_ResUnload(u16 uGroup, u16 uScreen, void* pData) {
}

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283B78), before the 1.0f / 512.0f UI_DrawInterface uses first; its body is unknown.
static f32 uiProcessInterface_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Once a frame (gomainloop's frame loops, nTicks 1), while gbUIRunning: draw the UI
// (UISDrawObjects) with alpha blending, no depth writes and an alpha test, in the UI's 512 x 448
// coordinates (fn_80016B6C) and with word-wrapped text at 0.85 line spacing
// (UI_SetTextLineSpacing), then put the render state back. In the menus it first clears
// gbUIFirstMenuDraw (UI_ClearFirstMenuDraw). Then it steps gUIDelayedHint: nFrames counts 0 to 2,
// one a frame, and past 2 goes back to -1 while nValue is sent to the UI as hint 0x23 (the menus)
// or 0x24 (a round).
void UI_DrawInterface(s32 nTicks) {
    s32 aArgs[1];

    if (gbUIRunning) {
        RenderView_SetUseCurrentMatrices(0);
        RenderState_SetBlendFactors(4, 5);
        DS_vEnableZBufferUpdate(0);
        DS_vSetAlphaTestMode(1, 6, 1);
        RenderState_Flush();
        if (gbUIFirstMenuDraw && gSession.nGameType == 3) {
            UI_ClearFirstMenuDraw();
        }
        RenderView_SetColor(NULL);
        fn_80016B6C(1.0f / 512.0f, 1.0f / 448.0f);
        FO_vSetCurrentAddMode(1);
        fn_80012C54_SetWordWrap(1);
        UI_SetTextLineSpacing(0.85f);
        if (gpFrontEnd != NULL) {
            UISDrawObjects(gpFrontEnd->pHandler, nTicks);
        }
        UI_SetTextLineSpacing(1.0f);
        fn_80012C54_SetWordWrap(0);
        fn_80016B6C(1.0f, 1.0f);
        DS_vEnableZBufferUpdate(1);
        DS_vSetAlphaTestMode(1, 6, 0x80);
        DS_vSetZBufferMode(3);
        RenderState_Flush();
        if (gUIDelayedHint.nFrames <= 2 && gUIDelayedHint.nFrames >= 0) {
            gUIDelayedHint.nFrames++;
        } else if (gUIDelayedHint.nFrames > 2) {
            gUIDelayedHint.nFrames = -1;
            Mem_set(aArgs, 0, sizeof(aArgs));
            aArgs[0] = gUIDelayedHint.nValue;
            if (gSession.nGameType == 3) {
                UISProcessHint(gpFrontEnd->pHandler, 0x23, 1, aArgs);
            }
            if (gSession.nGameType >= 4 && gSession.nGameType <= 8) {
                UISProcessHint(gpFrontEnd->pHandler, 0x24, 1, aArgs);
            }
        }
        UFont_ResetContext();
    }
}

// Let controller n's buttons reach the UI (b 1) or not (0): gUIState.abInputEnabled (front-end
// message 32, GM_vSetControllerInputEnabled; UI_vInitModule enables all four).
void UI_SetControllerEnabled(s32 n, s32 b) {
    gUIState.abInputEnabled[n] = b;
}

// Once a frame (from UI_UpdateInterface): read the controllers for the UI. The main stick works the
// D-pad in start-up, the menus and, while the pause menu is open, in play (game type 6). In a round
// (4 to 8, not paused) nothing is read while any player's view has a script fade running or held
// (script.nFade 1 to 4) with a camera other than 0x15. Each plugged-in controller's buttons (the
// stick's directions folded into the D-pad bits) are compared with last frame's: a button held 9
// frames counts as pressed again. gUIState.nNumPluggedIn counts the controllers plugged in,
// nFramesNoPad the frames with none. In the menus, the two-player modes 7 (speed golf on points)
// and 26 (the long-drive race) with fewer than two controllers (mode 7: and no CPU player in slot 0
// or 1) send hint 0x34, block the buttons (bButtonsBlocked) and hide the menu golfer
// (bHideMenuGolfer), putting gpCrAPState->bHidden aside in
// gSavedCrAPHidden; otherwise, with any controller in, hint 0x2D lifts that and puts bHidden back.
// Unless blocked, fading out (bFadeToBlack) or a movie is queued, each enabled controller's newly
// pressed buttons send their gUIButtonEvents event (UISProcessEvent; in play not while nPaused is 2
// or 3), and in the menus any held button sends hint 0x22; in play each such controller is also
// reported present (GUI_OnControllerPresent), and holding Controller_GetButtonMask(0x20, 1)'s
// button over 10 frames sends GUI_SendButtonHeld.
void UI_ReadControllers(void) {
    f32 fOne;
    s32 aArgs[1];
    u32 aButtons[8];            // fake match: 4 are used; the stack frame holds 8
    u32 aPressed[8];            // fake match: likewise
    View* pView;
    u32 uMask;
    u32 uButtons; // fake match: preserves the original mask-test operand order
    int i;
    int j;
    int k;
    UIButtonEvent* pEvent;
    u32* pPressed;
    u32* pButtons;

    fOne = 1.0f;
    if (gSession.nGameType == 3 || gSession.nGameType == 1 ||
        (gSession.nGameType == 6 && GUI_IsPauseMenuOpen())) {
        Input_vEmulateDPad(1);
    } else {
        Input_vEmulateDPad(0);
    }
    if (gSession.nGameType >= 4 && gSession.nGameType <= 8 && gSession.nPaused == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pView = ViewController_GetCameraControl(gPlayers[i].nView[0]);
            if ((fn_80063C90(pView) || pView->script.nFade == 3) && pView->nCurCamera != 0x15) {
                return;
            }
        }
    }
    // fake match: nNumPluggedIn is cleared through pPressed and pButtons (the same single store).
    // This first use makes the frontend give their array uses below temps numbered after the input
    // loop counter's (EA's r22/r23/r24), which the late loop's indexing then shares; the void*
    // copy keeps pButtons's use from being propagated away.
    pPressed = (u32*)&gUIState.nNumPluggedIn;
    pButtons = (u32*)(void*)pPressed;
    *pButtons = 0;
    Mem_set(aArgs, 0, sizeof(aArgs));
    pButtons = aButtons;
    pPressed = aPressed;
    for (i = 0; i < 4; i++) {
        if (Input_bDoesPadExist(i)) {
            gUIState.abPluggedIn[i] = 1;
            gUIState.nFramesNoPad = 0;
            gUIState.nNumPluggedIn++;
        } else {
            gUIState.abPluggedIn[i] = 0;
        }
        if (gUIState.abPluggedIn[i]) {
            pButtons[i] = Input_ReadControlPad(i);
            if (pButtons[i] & 0x40000) {
                pButtons[i] |= 4;
            }
            if (pButtons[i] & 0x80000) {
                pButtons[i] |= 8;
            }
            if (pButtons[i] & 0x20000) {
                pButtons[i] |= 2;
            }
            if (pButtons[i] & 0x10000) {
                pButtons[i] |= 1;
            }
            if (gUIState.auLastButtons[i] != pButtons[i]) {
                gUIState.anHeldFrames[i] = 0;
            } else if (gUIState.anHeldFrames[i] > 8) {
                gUIState.anHeldFrames[i] = 0;
                gUIState.auLastButtons[i] = 0;
            }
            gUIState.anHeldFrames[i]++;
            pPressed[i] = pButtons[i] & ~gUIState.auLastButtons[i];
            gUIState.auLastButtons[i] = pButtons[i];
        }
        gUIState.abPluggedInLast[i] = gUIState.abPluggedIn[i];
    }
    if (gUIState.nNumPluggedIn == 0) {
        gUIState.nFramesNoPad++;
    }
    if (gUIState.nNumPluggedIn > 0 && gSession.nGameType == 3 &&
        ((Game_GetMode() != 7 && Game_GetMode() != 0x1A) || gUIState.nNumPluggedIn >= 2 ||
         gFEState.aCPU[0] || gFEState.aCPU[1])) {
        if (gSavedCrAPHidden != -1) {
            gpCrAPState->bHidden = gSavedCrAPHidden;
            gSavedCrAPHidden = -1;
        }
        gUIState.bHideMenuGolfer = 0;
        UISProcessHint(gpFrontEnd->pHandler, 0x2D, 1, aArgs);
        gUIState.bButtonsBlocked = 0;
    }
    if (((Game_GetMode() == 7 && !gFEState.aCPU[0] && !gFEState.aCPU[1]) ||
         Game_GetMode() == 0x1A) &&
        gUIState.nNumPluggedIn < 2 && gSession.nGameType == 3) {
        if (gSavedCrAPHidden == -1) {
            gSavedCrAPHidden = gpCrAPState->bHidden;
        }
        UISProcessHint(gpFrontEnd->pHandler, 0x34, 1, aArgs);
        gUIState.bButtonsBlocked = 1;
        gUIState.bHideMenuGolfer = 1;
    }
    aArgs[0] = 0;
    if (gUIState.bFadeToBlack == 0 && FE_movieIsQueueEmpty() && gUIState.bButtonsBlocked == 0) {
        for (k = 0; k < 4; k++) {
            if (gUIState.abPluggedIn[k] && gUIState.abInputEnabled[k]) {
                if (gSession.nGameType != 6 || (gSession.nPaused != 2 && gSession.nPaused != 3)) {
                    pEvent = gUIButtonEvents;
                    for (j = 0; j < UI_NUM_BUTTON_EVENTS; j++) {
                        if (pEvent->uMask & pPressed[k]) {
                            UISProcessEvent(gpFrontEnd->pHandler, k, pEvent->nEvent, 1, &fOne, 0);
                        }
                        pEvent++;
                    }
                    if (pButtons[k] != 0 && gSession.nGameType == 3) {
                        UISProcessHint(gpFrontEnd->pHandler, 0x22, 1, aArgs);
                    }
                }
                if (gSession.nGameType == 6) {
                    GUI_OnControllerPresent(k);
                }
                if (gSession.nGameType == 6) {
                    uMask = Controller_GetButtonMask(0x20, 1);
                    uButtons = Input_ReadControlPad(k);
                    if (uButtons & uMask) {
                        gUIButtonHeldFrames[k]++;
                    } else {
                        gUIButtonHeldFrames[k] = 0;
                    }
                }
                if (gUIButtonHeldFrames[k] > 10) {
                    GUI_SendButtonHeld(k);
                    gUIButtonHeldFrames[k] = 0;
                }
            }
        }
    }
}

// Once a frame (gomainloop's frame loops, uEvent 1), while gbUIRunning: run the UI
// (UISIdleProcess). When the exit fade has asked for it (gbUICloseRequested), shut the UI down
// (UI_CloseInterface), clear gbUIRunning and set gbUIClosed; otherwise read the controllers
// (UI_ReadControllers).
void UI_UpdateInterface(u32 uEvent) {
    if (gbUIRunning) {
        if (gpFrontEnd != NULL) {
            UISIdleProcess(gpFrontEnd->pHandler, uEvent);
        }
        if (gbUICloseRequested) {
            UI_CloseInterface(gpFrontEnd);
            gbUICloseRequested = 0;
            gbUIRunning = 0;
            gbUIClosed = 1;
        } else if (gpFrontEnd != NULL) {
            UI_ReadControllers();
        }
    }
}

// Find the UI file's colour table, the table whose first entry is of kind 0x10 (the last, if
// several): keep it in pFE->p14 (the colours uiText.c draws text in) and turn its entries' p8
// offsets into pointers; with none, p14 is NULL.
void UI_FindColorTable(FrontEnd* pFE) {
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

// Resolve the UI file's entries by name. Kind 1, outside the menus: the texture of that name (p4)
// in the texture bank UI_GetTextureBankIndex picks, left alone when that is -1. Kind 2, while the
// picture list (pFE->pC) is loaded: the picture record of the same name (p4), its decoded picture
// (p8) cleared, and gUIState.nPictureTable notes that table.
void UI_ResolveFileEntries(FrontEnd* pFE) {
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
                    nBank = UI_GetTextureBankIndex(szName);
                    if (nBank != -1) {
                        pEntry->p4 = UI_FindTexture(gpFrontEnd->p8->ap4[nBank], uHash);
                    }
                }
            } else if (pEntry->u0 == 2 && pFE->pC != NULL) {
                gUIState.nPictureTable = i;
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

// The texture bank (an index into gpFrontEnd->p8->ap4) a UI texture name is in: a name starting
// "tu" is in bank 1 while a lesson runs and in none (-1) otherwise; every other name, and every
// name in start-up (game type 1), is in bank 0.
int UI_GetTextureBankIndex(const char* szName) {
    if (gSession.nGameType == 1) return 0;
    if (szName[0] == 't' && szName[1] == 'u') {
        if (Lessons_IsRunning()) return 1;
        return -1;
    }
    return 0;
}

// Open the UI set szSet ("startup", "frontend" or "ingame"): allocate the front end (gpFrontEnd),
// take what uiLoadFile.c loaded (the UI file, texture banks, picture list and fonts), allocate the
// transform stack, clear the exit fade and the delayed hint, resolve the UI file (UI_RelocateFile,
// UI_ResolveFileEntries, UI_FindColorTable), set up the studio (10 screens, 9 plugins, 256 rate
// functions, 2 modals, a 2048-word stack) with its plugins 0 to 8 and the game's callbacks, hand it
// the file's "GlobalScript" screen if there is one, and load and activate screen 0 (start-up passes
// it two zero words). In the menus the golfer textures are set up too (FE_InitGolferTextures).
// Returns gpFrontEnd.
FrontEnd* UI_OpenInterface(char* szSet) {
    s32 aArgs[2];
    int i;

    gbUICloseRequested = 0;
    gbUIRunning = 1;
    gbUIClosed = 0;
    UI_SetInterfaceName(szSet);
    gpFrontEnd = StaticMem_Alloc(sizeof(FrontEnd), 2, 16, "uiProcessInterface.c", 904);
    gpFrontEnd->f18 = 0.0f;
    gpFrontEnd->pFile = UI_GetFileData(szSet);
    gpFrontEnd->p8 = UI_GetTextureBanks(szSet);
    gpFrontEnd->pC = UI_GetPictureList(szSet);
    gpFrontEnd->p10 = UI_GetFonts(szSet);
    UITransform_Init();
    gUIState.abAssigned[0] = 0;
    gUIState.abAssigned[1] = 0;
    gUIState.abAssigned[2] = 0;
    gUIState.abAssigned[3] = 0;
    gUIState.bFadeToBlack = 0;
    gUIState.fFade = 0.0f;
    gUIDelayedHint.nValue = 0;
    gUIDelayedHint.nFrames = -1;
    UI_RelocateFile(gpFrontEnd);
    UI_ResolveFileEntries(gpFrontEnd);
    UI_FindColorTable(gpFrontEnd);
    gpFrontEnd->pHandler = StaticMem_Alloc(UISGetMemSize(10, 9, 256, 2, 2048, 128), 2, 16,
                                         "uiProcessInterface.c", 943);
    UISInit(gpFrontEnd->pHandler, 10, 9, 256, 2, 2048, 128, 16);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 0, (UISPluginFncT*)UIPoly_ProcessMessage);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 1, UI_BlankProcess1);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 2, UI_BlankProcess2);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 3, UI_BlankProcess3);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 4, UI_BlankProcess4);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 5, UI_BlankProcess5);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 6, UI_BlankProcess6);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 7, (UISPluginFncT*)UIText_ProcessMessage);
    UISRegisterPluginFnc(gpFrontEnd->pHandler, 8, (UISPluginFncT*)UIArc_ProcessMessage);
    UISRegisterResourceFncs(gpFrontEnd->pHandler, UI_ResLoad, UI_ResUnload);
    UISRegisterTransformFncs(gpFrontEnd->pHandler, (UISTransformFncT*)UITransform_HandleOp);
    UISRegisterMessageFnc(gpFrontEnd->pHandler, UI_RunGameMessage);
    // fake match: the original compares the count signed here (cmpw), unsigned in UI_ResLoad
    for (i = 0; i < (s32)gpFrontEnd->pFile->p4->nCount; i++) {
        if (strcmp(gpFrontEnd->pFile->p4->aPairs[i].p0, "GlobalScript") == 0) {
            UISSetGlobalScript(gpFrontEnd->pHandler, gpFrontEnd->pFile->p4->aPairs[i].p4);
            break;
        }
    }
    if (gSession.nGameType != 1) {
        UISLoadScreen(gpFrontEnd->pHandler, 0, 0, 0, NULL);
    } else {
        aArgs[0] = 0;
        aArgs[1] = 0;
        UISLoadScreen(gpFrontEnd->pHandler, 0, 0, 2, aArgs);
    }
    if (gSession.nGameType == 3) {
        FE_InitGolferTextures();
    }
    UISSetScreenActive(gpFrontEnd->pHandler, 0, 0);
    UISRegisterScreenDrawDebugFnc(gpFrontEnd->pHandler, UI_ScreenDrawDebug);
    UISRegisterRuntimeErrorFnc(UI_ReportUISError);
    return gpFrontEnd;
}

// Shut the UI pFE down. First the movie entries' pictures are freed (UI_FreeAllEntryPictures);
// leaving start-up with no nC plays the start-up movies and legal screen (UI_PlayStartUpMovies); leaving the
// menus sets b0 and clears b1 of every gUITxf2BankState entry whose gUITxf2BankMarkOnExit word is
// set, clears gbUIFirstMenuDraw (set, then cleared by UI_ClearFirstMenuDraw), and brings the
// picture list back from ARAM and frees it (UI_RestoreMenuPictures, UI_FreeMenuPictures). Then the
// studio is shut down, the fonts, picture list, texture banks and UI file are freed, and the studio
// and the front end with them; gpFrontEnd is NULL after.
void UI_CloseInterface(FrontEnd* pFE) {
    int i;

    UI_FreeAllEntryPictures();
    if (gSession.nGameType == 1 && gSession.nC == 0) {
        UI_PlayStartUpMovies();
    } else if (gSession.nGameType == 3) {
        for (i = 0; i < UI_NUM_TXF2_BANKS; i++) {
            if (gUITxf2BankMarkOnExit[i] != 0) {
                gUITxf2BankState[i].b0 = 1;
                gUITxf2BankState[i].b1 = 0;
            }
        }
        gbUIFirstMenuDraw = 1;
        UI_ClearFirstMenuDraw();
        UI_RestoreMenuPictures();
        UI_FreeMenuPictures();
    }
    UISShutdown(gpFrontEnd->pHandler);
    UI_FreeFonts(pFE->p10);
    UI_FreePictureList(pFE->pC);
    UI_FreeTextureBanks(pFE->p8);
    UI_FreeFileData(pFE->pFile);
    StaticMem_Free(gpFrontEnd->pHandler);
    StaticMem_Free(gpFrontEnd);
    gpFrontEnd = NULL;
}

// Start the UI module (GO_vInitFE, GO_vInitIG, start-up's gomainloop fn_8006CEFC): the font add
// mode set to 1 (UIText_SetFontDrawAtOnce), nothing loaded (UI_ResetLoadedFiles), the controller
// state cleared with all four controllers enabled, the EA Trax display reset (UI_EATraxReset) and the
// pending UI messages dropped (GameMsg_ClearPending).
void UI_vInitModule(void) {
    s32 i;

    UIText_SetFontDrawAtOnce();
    UI_ResetLoadedFiles();
    gUIState.nFramesNoPad = 0;
    gUIState.nNumPluggedIn = 0;
    gUIState.bHideMenuGolfer = 0;
    gUIState.bButtonsBlocked = 0;
    for (i = 0; i < 4; i++) {
        gUIState.anHeldFrames[i] = 0;
        gUIState.abAssigned[i] = 0;
        gUIState.abInputEnabled[i] = 1;
    }
    gUIState.b48 = 0;
    UI_EATraxReset();
    GameMsg_ClearPending();
}

// Once a main-loop frame (gomainloop fn_8006D8E8): send the messages queued for the UI
// (GameMessages.c), the menus' own set in game type 3 (GameMsg_SendPendingMenus), else
// GameMsg_SendPending.
void UI_SendPendingMessages(void) {
    if (gSession.nGameType == 3) {
        GameMsg_SendPendingMenus();
    } else {
        GameMsg_SendPending();
    }
}

// End the UI module (gomainloop's shut-down steps for the menus, a round and start-up): shut the UI
// down if it is still open (UI_CloseInterface), free the transform stack (UITransform_Shutdown),
// set the font add mode back to 0 (UIText_SetFontDrawQueued) and free the EA Trax logo (UI_EATraxFreeLogo).
void UI_vCloseModule(void) {
    if (gpFrontEnd != NULL) {
        UI_CloseInterface(gpFrontEnd);
    }
    UITransform_Shutdown();
    UIText_SetFontDrawQueued();
    UI_EATraxFreeLogo();
}

// The fade to black when the UI is left (while gUIState.bFadeToBlack is set): draw it, 0.05 darker
// each frame. Once it is black, unpause; start-up (1) asks for the UI to close (gbUICloseRequested)
// and ends the loop (gSession.bEndLoop); the menus (3) ask for it to close, start the demo when
// gSession.bDemo is set and call FE_Manager.c's (empty) function for game mode 23 (the PGA TOUR),
// 24 (a real-time event) or 4 (the ladder); other game types end the loop.
void UI_ExitFade(void) {
    f32 aColor[4];

    if (gUIState.bFadeToBlack == 0) return;
    aColor[0] = 0.0f;
    aColor[1] = 0.0f;
    aColor[2] = 0.0f;
    aColor[3] = gUIState.fFade;
    fn_80037FB4(1, aColor);
    gUIState.fFade += 0.05f;
    if (gUIState.fFade >= 1.0f) {
        gUIState.fFade = 1.0f;
        gSession.nPaused = 0;
        if (gSession.nGameType == 1) {
            gbUICloseRequested = 1;
            gSession.bEndLoop = 1;
        } else if (gSession.nGameType == 3) {
            gbUICloseRequested = 1;
            if (gSession.bDemo != 0) {
                DEMO_Start();
            }
            if (Game_GetMode() == 0x17) {
                FE_PlayPGATourMovie();
            } else if (Game_GetMode() == 0x18) {
                FE_PlayRTEMovie();
            } else if (Game_GetMode() == 4) {
                FE_PlayLadderMovie();
            }
        } else {
            gSession.bEndLoop = 1;
        }
    }
}

// Print nValue into szOut with a comma every three digits ("1234567" becomes "1,234,567"), through
// a 128-character buffer. Meant for amounts of money: the '-' of a negative value counts as a digit
// ("-123" becomes "-,123").
void UI_GetMoneyString(int nValue, char* szOut) {
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

// The studio's runtime-error callback (UISRuntimeErrorFncT, UI_OpenInterface): the retail game
// prints nothing, as Madden 2003's ReportUISError.
void UI_ReportUISError(s32 nLevel, const char* szFile, s32 nLine, const char* szMsg) {
}

// The studio's screen debug-draw callback (UISScreenDrawDebugFncT, UI_OpenInterface): nothing to
// do.
void UI_ScreenDrawDebug(u16 uGroup, u16 uScreen, s32 n) {
}

// Resolve the open UI file's entries again (UI_ResolveFileEntries on gpFrontEnd), once UI_RestoreMenuPictures
// has brought the picture list back from ARAM.
void UI_RefreshFileEntries(void) {
    UI_ResolveFileEntries(gpFrontEnd);
}

// The studio's plugin 1 (UISPluginFncT, UI_OpenInterface): empty, as Madden 2003's _BlankProcess.
void UI_BlankProcess1(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// The studio's plugin 2 (UISPluginFncT, UI_OpenInterface): empty, as Madden 2003's _BlankProcess.
void UI_BlankProcess2(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// The studio's plugin 3 (UISPluginFncT, UI_OpenInterface): empty, as Madden 2003's _BlankProcess.
void UI_BlankProcess3(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// The studio's plugin 4 (UISPluginFncT, UI_OpenInterface): empty, as Madden 2003's _BlankProcess.
void UI_BlankProcess4(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// The studio's plugin 5 (UISPluginFncT, UI_OpenInterface): empty, as Madden 2003's _BlankProcess.
void UI_BlankProcess5(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// The studio's plugin 6 (UISPluginFncT, UI_OpenInterface): empty, as Madden 2003's _BlankProcess.
void UI_BlankProcess6(void* pVar, s32 nMsg, s32 n2, s32* pn3, s32 n4) {
}

// Set the current text context's line spacing (UFontContext.fLineSpacing): word-wrapped lines are
// that many times the font's height apart (LLFont.c fn_80011D0C); 1 is normal, UI_DrawInterface
// draws the UI at 0.85.
void UI_SetTextLineSpacing(f32 fSpacing) {
    UFontContext* pCtx;
    pCtx = FO_spGetCurrentPacket();
    pCtx->fLineSpacing = fSpacing;
}

// The texture in pBank whose name hashes to uHash. A name not in the bank gives index 0x80000000,
// which fn_800107E4's 32-bit multiply by the entry size wraps to the bank's first texture.
// port: on 64 bits that index points far outside the bank.
TexEntry* UI_FindTexture(TexBank* pBank, u64 uHash) {
    int nTex = fn_8001005C(pBank, uHash);

    return fn_800107E4(pBank, nTex);
}

// Code800B1D3C.c (a placeholder named by its address: no EA file name is known; TW06 and TW07 have
// no start-up command table): the UI commands while the game starts up (session game type 1).
// uiProcessInterface.c sends each through Startup_RunGameMessage to the handler table
// Startup_InitGameMessages fills (gStartupMessageHandlers); the handlers run startUp.c's
// memory-card checks, its built-in sounds and the disc change. A unit of its own: UAudVector.c and
// the ball-against-object test (Code800B1AA8.c) lie between it and startUp.c. Its only data is the
// table (.bss).

#include "core/startup.h"
#include "core/memcard.h"
#include "core/gameaudio.h"
#include "game/frontend.h"
#include "frontend/uistudio.h"
#include "frontend/fe.h"

void   GM_vStartupGetBlocksNeeded(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupCheckCards(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFindFirstCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFindNextCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFadeToBlack(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupLoadFromCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupSkipCardLoad(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFormatCard(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetNextCardStatus(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetCurrentCardStatus(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupPlaySound(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupFormatHadIOError(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage11_Return0(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage12_Empty(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage13_Empty(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupMessage14_Return1(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetFilesNeeded(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupEndGameLoop(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupDeleteSaveGame(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupLoadOptionsCheckDisc(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupChangeDisc(MsgArg* pArgs, MsgArg* pResult);
void   GM_vStartupGetDiscChangeStatus(MsgArg* pArgs, MsgArg* pResult);

// The start-up message handlers (Startup_InitGameMessages fills 0..22, all but 4; the last 7 are never set).
MsgHandler gStartupMessageHandlers[30];

// The UI commands while the session's game type is 1 (start-up): run command nCmd's handler.
void Startup_RunGameMessage(int nCmd, MsgArg* pArgs, MsgArg* pResult) {
    gStartupMessageHandlers[nCmd](pArgs, pResult);
}

// Fill in the start-up UI command table (gStartupMessageHandlers): commands 0..22, command 4 left empty
// (NULL). Called once when start-up begins (gomainloop.c fn_8006CEFC).
void Startup_InitGameMessages(void) {
    int i;
    for (i = 0; i < 23; i++) {
        gStartupMessageHandlers[i] = NULL;
    }
    gStartupMessageHandlers[0] = GM_vStartupGetBlocksNeeded;
    gStartupMessageHandlers[1] = GM_vStartupCheckCards;
    gStartupMessageHandlers[2] = GM_vStartupFindFirstCard;
    gStartupMessageHandlers[3] = GM_vStartupFindNextCard;
    gStartupMessageHandlers[5] = GM_vStartupFadeToBlack;
    gStartupMessageHandlers[6] = GM_vStartupLoadFromCard;
    gStartupMessageHandlers[7] = GM_vStartupFormatCard;
    gStartupMessageHandlers[8] = GM_vStartupGetNextCardStatus;
    gStartupMessageHandlers[9] = GM_vStartupGetCurrentCardStatus;
    gStartupMessageHandlers[10] = GM_vStartupPlaySound;
    gStartupMessageHandlers[11] = GM_vStartupMessage11_Return0;
    gStartupMessageHandlers[12] = GM_vStartupMessage12_Empty;
    gStartupMessageHandlers[13] = GM_vStartupMessage13_Empty;
    gStartupMessageHandlers[14] = GM_vStartupMessage14_Return1;
    gStartupMessageHandlers[15] = GM_vStartupGetFilesNeeded;
    gStartupMessageHandlers[16] = GM_vStartupEndGameLoop;
    gStartupMessageHandlers[17] = GM_vStartupDeleteSaveGame;
    gStartupMessageHandlers[18] = GM_vStartupFormatHadIOError;
    gStartupMessageHandlers[19] = GM_vStartupSkipCardLoad;
    gStartupMessageHandlers[20] = GM_vStartupLoadOptionsCheckDisc;
    gStartupMessageHandlers[21] = GM_vStartupChangeDisc;
    gStartupMessageHandlers[22] = GM_vStartupGetDiscChangeStatus;
}

// Command 0: the card space the game's save and the EA Sports Bio need (fn_8009D390) on the card at
// port pArgs[0], slot pArgs[1]. The slot counts from 1 here; both are kept at 0 or above.
void GM_vStartupGetBlocksNeeded(MsgArg* pArgs, MsgArg* pResult) {
    s32 nPort = pArgs[0].i;
    s32 nSlot = pArgs[1].i;
    if (nSlot > 0) {
        nSlot--;
    }
    if (nPort < 0) {
        nPort = 0;
    }
    if (nSlot < 0) {
        nSlot = 0;
    }
    MC_Connect();
    pResult->i = fn_8009D390(nPort, nSlot);
    MC_Disconnect();
}

// Command 1: the start-up card check (Startup_CheckCards): build the card status table and send the
// start-up UI the message for the first card that needs one.
void GM_vStartupCheckCards(MsgArg* pArgs, MsgArg* pResult) {
    Startup_CheckCards();
}

// Command 2: start the card search (Startup_FindFirstCardWithStatus). The port and slot found go to
// the addresses in pArgs[0] and pArgs[1]; answers whether one was found.
void GM_vStartupFindFirstCard(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)Startup_FindFirstCardWithStatus(pArgs[0].p, pArgs[1].p);
}

// Command 3: continue the card search (Startup_FindNextCardWithStatus), as command 2.
void GM_vStartupFindNextCard(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (u8)Startup_FindNextCardWithStatus(pArgs[0].p, pArgs[1].p);
}

// Command 5: start the fade to black (gUIState.bFadeToBlack).
void GM_vStartupFadeToBlack(MsgArg* pArgs, MsgArg* pResult) {
    gUIState.bFadeToBlack = 1;
}

// Command 6 (Startup_LoadFromCard): load the options from the first card with a good save, then the last
// user (MC_LoadInitialUser). When no card status has been read since command 19, it only sends the
// start-up UI message 0x86 (no save found).
void GM_vStartupLoadFromCard(MsgArg* pArgs, MsgArg* pResult) {
    Startup_LoadFromCard();
}

// Command 19 (Startup_SkipCardLoad): make command 6 skip the card and only report no save found (message
// 0x86), until a card's status is read again.
void GM_vStartupSkipCardLoad(MsgArg* pArgs, MsgArg* pResult) {
    Startup_SkipCardLoad();
}

// Command 7: format the card at port pArgs[0], slot pArgs[1] (Startup_FormatCard; the result goes
// to the UI as message 0x84).
void GM_vStartupFormatCard(MsgArg* pArgs, MsgArg* pResult) {
    Startup_FormatCard(pArgs[0].i, pArgs[1].i);
}

// Command 8: report the next card status not yet reported (Startup_GetNextCardStatus). Its port and
// slot go to the addresses in pArgs[0] and pArgs[1].
void GM_vStartupGetNextCardStatus(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
    pResult->i = Startup_GetNextCardStatus(pArgs[0].p, pArgs[1].p);
    MC_Disconnect();
}

// Command 9: report again the card the reports reached (Startup_GetCurrentCardStatus). Its port and
// slot go to the addresses in pArgs[0] and pArgs[1].
void GM_vStartupGetCurrentCardStatus(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
    pResult->i = Startup_GetCurrentCardStatus(pArgs[0].p, pArgs[1].p);
    MC_Disconnect();
}

// Command 10: play built-in sound 0 when pArgs[0] is 2, else built-in sound 1
// (Aud_PlayBuiltInSound).
void GM_vStartupPlaySound(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 2) {
        Aud_PlayBuiltInSound(0);
        return;
    }
    Aud_PlayBuiltInSound(1);
}

// Command 18: whether a format of the card at port pArgs[0], slot pArgs[1] failed with an I/O error
// (MCCardState.b94).
void GM_vStartupFormatHadIOError(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;
    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.b94;
}

// Command 11 of the start-up table (Startup_InitGameMessages): answers 0.
void GM_vStartupMessage11_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Command 12 of the start-up table (Startup_InitGameMessages): empty.
void GM_vStartupMessage12_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 13 of the start-up table (Startup_InitGameMessages): empty.
void GM_vStartupMessage13_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 14 of the start-up table (Startup_InitGameMessages): answers 1.
void GM_vStartupMessage14_Return1(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

// Command 15: how many new files a save of the game needs on the card at port pArgs[0], slot
// pArgs[1] (fn_8009D3DC: 1 when the save file or its backup is not on it yet, else 0).
void GM_vStartupGetFilesNeeded(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
    pResult->i = fn_8009D3DC(pArgs[0].i, pArgs[1].i);
    MC_Disconnect();
}

// Command 16: end start-up's main loop (gSession.nC 2, which gomainloop.c fn_8006D01C checks each
// frame), as the menus' GM_vEndGameLoop.
void GM_vStartupEndGameLoop(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

// Command 17: delete the game's save from the card at port pArgs[0], slot pArgs[1]
// (Startup_DeleteSaveGame; the result goes to the UI as message 0x8D).
void GM_vStartupDeleteSaveGame(MsgArg* pArgs, MsgArg* pResult) {
    Startup_DeleteSaveGame(pArgs[0].i, pArgs[1].i);
}

// Command 20: load the options from a card (Startup_LoadOptionsFromCard), then answer 1 when the
// disc in the drive is not disc 1 (fn_8011027C) and no options were loaded (fn_80110460 answers the
// flag Startup_LoadOptionsFromCard left), else 0.
void GM_vStartupLoadOptionsCheckDisc(MsgArg* pArgs, MsgArg* pResult) {
    Startup_LoadOptionsFromCard();
    if (fn_8011027C() && !fn_80110460()) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

// Command 21: ask for the other disc and wait for it.
void GM_vStartupChangeDisc(MsgArg* pArgs, MsgArg* pResult) {
    fn_801102AC();
}

// Command 22: the disc change's progress, as the menus' GM_vGetDiscChangeStatus answers it.
void GM_vStartupGetDiscChangeStatus(MsgArg* pArgs, MsgArg* pResult) {
    GM_vGetDiscChangeStatus(pArgs, pResult);
}

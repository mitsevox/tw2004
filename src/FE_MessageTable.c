// FE_MessageTable.c (our name): the front end's messages, the questions and orders the menu UI
// sends while the menus run (game type 3; uiProcessInterface.c routes them to FE_RunGameMessage).
// FE_InitGameMessages fills gFEMessageHandlers, 612 of 770 slots, with this file's handlers and those
// of the other front-end files (Create-A-Player, the logo editor, the PGA TOUR screens, the
// calendar, the ladder, the trophy room). This file's handlers set up the game the menus start
// (mode, players, golfers, controllers, course, holes, tees, weather, custom rounds), work the
// memory card screens (card state, loading and saving profiles, options and replays, profile
// backups), set the options (course conditions, swing and putting aids, commentary, music and
// sounds, vibration), and read the save profiles' stats and records for the menus; others run the
// EA Sports Bio, the Game Boy Advance link, the disc swap and the Play Now groups. The handlers are
// EA's GM_v... message functions: TW07's UI_Core/frontend/GameMessages/APT_FE_GameMessages.c
// (TW06's apt_fe_gamemessages.c) has GetGolferName, GM_vGetGolferName, GM_vGetGameMode,
// GM_vHideCharacter, GM_vSetCharState, GM_vSaveGolferModel, GM_vCharStream, GM_vSetMulligan,
// GM_vSetMCRewardMoney, GM_vTrophyBallsWon, GM_vPar5Eagles and the options' GM_vSet/Get...Option
// pairs (commentary, tap-ins, green grid, caddie tips, break line, power boost, spin control, swing
// aid, music volume, sound effects), GM_vSet/GetVibration, and TW07's MC.h the MC_CallActionFn... and
// MC_SetCurrentFileType helpers at the file's end, but in another order and mostly with other
// handlers, so most names here are read from the code; a handler that is empty or answers a
// constant is named after its message number (GM_vFEMessage<N>_Empty / _Return<value>). The
// round's twin is GameUICommands.c (IG_RunGameMessage).

#include "game.h"
#include "camera.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "core/memcard.h"
#include "charstate.h"
#include "trax.h"
#include "core/gameaudio.h"
#include "unsorted/cull.h"
#include "core/easb.h"
#include "game/earnings.h"
#include "game/modes/ladder.h"

// Outside this file.
void fn_800142A4(s8 n);                 // sets lbl_80281C98
void SaveProfile_InitNew(SaveProfile* pProfile);
void FE_OnGolferHiddenChanged(void);                 // FEgolferanim.c
void UI_SetControllerEnabled(s32 p0, s32 p1);       // uiProcessInterface.c
void FE_SetOffscreenBufferRender(u8 bOn);               // FEgolferanim.c
s32  MC_LoadUser(MCCardPosStr* pPos);   // MC.c
s32  MC_LoadReplay(MCCardPos* pPos);      // MC.c: load a replay from the card
void MC_ConnectCard(s32 nPort, s32 nSlot); // MC_Gc.c
s32  MC_NumEASaveGames(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_800A1164(s32 nPort, s32 nSlot, char* pName, s32 n);     // MC.c
s32  MC_GetUser(s32 nPort, s32 nSlot, s32 n, char* szOut);     // MC.c: clears szOut first
void FE_PlayTrophyBallHighlight(Replay* pReplay);      // FE_Manager.c
f32  GM_GetBonusProgress(SaveProfile* pProfile);    // GameMode.c
void GM_PgaTourSim_ClearAllSeasons(TourSeason* pTour);    // PGATourSimulation.c
int  GameMode4_GetCurrentEventHoles(void);                 // LadderedMode.c: the current ladder event's holes
int  GameMode4_GetCurrentEvent(void);                 // LadderedMode.c: the current ladder event
s32  PlayNow_GetNumOpponents(int i);                // PlayNowMode.c: challenge i's opponent count
s32  PlayNow_GetOpponent(int i, int k);         // PlayNowMode.c: its opponent k
u8   GM_UserHasEagledHole(int nSlot, int a, int b);      // GameRound.c
int  GM_GetPar5EagleDate(int nSlot, int a, int b);      // GameRound.c
int  GM_GetGolferMoneyRating(int nGolfer);          // Earnings.c: the golfer's rating
int  GM_GetMinPlayersForMode(int nMode);            // GameRound.c
void GM_SetSplitScreenForMode(void);                 // GameRound.c
void GM_BuildRandom18(void);                 // GameRound.c: builds the random mixed round
void Lessons_StartFromMenu(void);                 // GameMode11.c
void GameModeDriverPGATour_PrepareForTeeOff(void);                 // GameModeDriverPGATour.c
u8*  GameMode26_StartEvent(void);                 // CharSliders.c
void GameMode22_StartEvent(void);                 // GameMode22.c
s32  Gba_GetState(void);                 // gbacable.c
void Gba_MarkUnlocksGranted(void);                 // gbacable.c
void Gba_UnlockProfileRewards(void);                 // gbacable.c
s32  fn_801255C4(s32* pPos);            // EASportsBio.c
u8   PasswordManager_TestPassword(char* szCode);  // PasswordManager.c
void GameMode22_SetNumDrives(s32 n);                // GameMode22.c: sets gGameMode22.nDrives
void GameMode22_SetVariant(s32 n);                // GameMode22.c: sets gGameMode22.nVariant
void GM_SetupCustomHoleSelection(void); // GameMode.c
int  GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile);  // GameMode.c
void PlayNow_SelectGroup(int nId);              // PlayNowMode.c
s32  PlayNow_GetNumGroups(void);                 // PlayNowMode.c
char* PlayNow_GetGroupName(int nId);             // PlayNowMode.c
char* PlayNow_GetGroupDescription(int nId);             // PlayNowMode.c
void PlayNow_GetRewards(int i, s32* pA, s32* pB, s32* pC);     // PlayNowMode.c
int  GameMode4_GetNumEventsWon(void);                 // LadderedMode.c
void GameMode4_SetEventBonus(s32 n);                // LadderedMode.c
void GameMode26_SetTargetScore(s32 v);                // CharSliders.c
void FE_SetProfileLeftHanded(int nSlot, int n);
s32  MC_LoadOptions(MCCardPos* pPos);      // MC.c: load the save from the card
s32  MC_SaveOptions(MCCardPos* pPos);      // } MC.c, in lbl_8018C7D8 (sets 0, 2, 1, 1)
s32  fn_800A09EC(MCCardPos* pPos);      // }
s32  MC_SaveUser(MCCardPos* pPos);      // }
s32  MC_GetNumUser(MCCardPos* pPos);      // }
void Gaud_ExitFE(void);
void Gaud_PlayUISound(int n);
void TrophyRoom_GetTourWinStatus(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetPlayerOfMonthStatus(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetRTEAwardStatus(MsgArg* pArgs, MsgArg* pResult);
void Gba_Init(void);
void Gba_SetState(s32 v);
s32  Gba_GetCashToMove(void);
void Gba_SetCashToMove(s32 n);
s32  Gba_GetCashOnGba(void);
s32  Gba_GetGbaStat(void);
s32  Gba_IsReadPending(void);
void Gba_SetReadPending(s32 v);
void Gba_SetUndoTransfer(s32 v);
s32  Gba_IsUndoTransfer(void);
void Gba_StepPorts(s32 a, s32 b);

// The other files' message handlers in the table (the Create-A-Player screens, the logo editor,
// the PGA TOUR screens, the stats screen, the EA Sports Bio...).
void GM_vGetNumCrAPItems(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemValue(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemColor(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage404_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage405_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCRAPSlider(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCRAPItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage408_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCRAPSlider(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumCrAPGeometries(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage411_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemInfo(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemAttributes(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemPriceAndLock(MsgArg* pArgs, MsgArg* pResult);
void GM_vCRAPTryOnItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vPreviewItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumCrAPSubcategories(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPGolferInfo(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPGolferInfo(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetUseProfileCopy(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemLocked(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemOwned(MsgArg* pArgs, MsgArg* pResult);
void GM_vPurchaseCrAPItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPCategoryWorn(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemEquipped(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPSaleItems(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPSaleSubcategories(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemOnSale(MsgArg* pArgs, MsgArg* pResult);
void GM_vSelectLogo(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLogoEditorShape(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoPaletteColor(MsgArg* pArgs, MsgArg* pResult);
void GM_vLoadLogoFromTexture(MsgArg* pArgs, MsgArg* pResult);
void GM_vMarkLogoChanged(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLogoPixel(MsgArg* pArgs, MsgArg* pResult);
void GM_vCheckCrAPUnlocks(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemNew(MsgArg* pArgs, MsgArg* pResult);
void GM_vMarkNewCrAPItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearMarkedNewCrAPItems(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsAnyPadHoldingA(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetDPadHeld(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetProfileFlag(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileFlag(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNewCrAPItemsText(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTodaysDate(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRandom1To99(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoName(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLogoName(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsProfileLogoMade(MsgArg* pArgs, MsgArg* pResult);
void GM_vSaveLogo(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferAttributeTier(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPSubcategoryName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPColorName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemUnlockText(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage534_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPCameraIdleState(MsgArg* pArgs, MsgArg* pResult);
void GM_vHasMenuGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsMenuGolferReady(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsGolferLoaderIdle(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage539_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetUseProfileCopy(MsgArg* pArgs, MsgArg* pResult);
void GM_vCRAPCreatingLogo(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoPaletteEntry(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoPixel(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLogoShape(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoShape(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage609_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPTriggerAnims(MsgArg* pArgs, MsgArg* pResult);
void GM_vRestartCrAPAnim(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPRenderStateForSubcategory(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPClub(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearGolferCache(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage598_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPOutfit(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPGolferInIdleShot(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPLookInFaceShot(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage566_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSellCrAPItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPCategoryCounts(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumCrAPSaleItemsOwned(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPBody(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearQueuedCrAPAnim(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEquippedCrAPItemInSubcategory(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPAnimInGolferLib(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vRestoreAfterPreview(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage741_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsLeapYear(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemRemovable(MsgArg* pArgs, MsgArg* pResult);
void PGALeaderboard_GetRow(MsgArg* pArgs, MsgArg* pResult);
void PGALeaderboard_GetNumRows(MsgArg* pArgs, MsgArg* pResult);
void PGASchedule_GetRow(MsgArg* pArgs, MsgArg* pResult);
void PGASchedule_Build(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_GetLastEventLine(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_IsSeasonOver(MsgArg* pArgs, MsgArg* pResult);
void PGASeasonWrapUp_GetLine(MsgArg* pArgs, MsgArg* pResult);
void PGADriver_ShowCalendar_AdvanceSeason(void);       // FE_PGATourMessages.c: message 553
void PGATourMsg_GetTestText(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage560_Return25(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_SignNext(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_GetItemBonus(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_PickStartingSponsor(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_CollectItems(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_GetItem(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_GetName(MsgArg* pArgs, MsgArg* pResult);
void PGATourWins_GetDetails(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_CheckAdvanceTournament(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_SetStatsDirty(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_SetScoresDirty(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_DidUserQuit(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_GetSlot(MsgArg* pArgs, MsgArg* pResult);
void PGASponsor_GetTotalItemBonus(MsgArg* pArgs, MsgArg* pResult);
void Calendar_FillCell(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetLine(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetBottomLine(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetMonthShown(MsgArg* pArgs, MsgArg* pResult);
void Calendar_PrevMonth(MsgArg* pArgs, MsgArg* pResult);
void Calendar_NextMonth(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetTodayCell(MsgArg* pArgs, MsgArg* pResult);
void Calendar_SetDriver(MsgArg* pArgs, MsgArg* pResult);
void Calendar_SelectCell(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetPopupRow(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage511_Empty(MsgArg* pArgs, MsgArg* pResult);
void Calendar_Play(MsgArg* pArgs, MsgArg* pResult);
void Calendar_IsSimulationNecessary(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetRTEDescription(MsgArg* pArgs, MsgArg* pResult);
void FE_Sqrt(MsgArg* pArgs, MsgArg* pResult);
void Calendar_IsSherwoodTargetEntered(MsgArg* pArgs, MsgArg* pResult);
void Calendar_SetPlayNowFlag(MsgArg* pArgs, MsgArg* pResult);
void Calendar_GetPlayNowFlag(MsgArg* pArgs, MsgArg* pResult);
void UIStatsRankings_GetRow(MsgArg* pArgs, MsgArg* pResult);
void UIStatsRankings_GetProfileName(MsgArg* pArgs, MsgArg* pResult);
void UIStatsRankings_SetActiveStat(MsgArg* pArgs, MsgArg* pResult);
void UIStatsRankings_GetIndStatsRow(MsgArg* pArgs, MsgArg* pResult);
void UIStatsRankings_GetIndStatsNumRows(MsgArg* pArgs, MsgArg* pResult);
void UIStatsRankings_GetNumRows(MsgArg* pArgs, MsgArg* pResult);
void FE_GetNextRealtimeEventInfo(MsgArg* pArgs, MsgArg* pResult);
void FE_GetDateTimeIfClockEarly(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_SetNodePos(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetNodePos(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetEventText(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_MoveCursor(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetNodeEventN0(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetFirstAndCursorNodePos(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_StartEvent(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetAngleBetween(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetNodeState(MsgArg* pArgs, MsgArg* pResult);
void LadderMenu_GetEventName(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetTourTrophy(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetTourTrophyText(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_CountEventsInMonth(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetPlaceholderText(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetIndexMod4(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetMedalDate(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetLadderAward(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetLadderEventCourse(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetRTEAwardIcon(MsgArg* pArgs, MsgArg* pResult);
void TrophyRoom_GetAwardEarnedText(MsgArg* pArgs, MsgArg* pResult);

// This file.
void GetGolferName(int nGolfer, char* szName);
void FE_GetRecordEntry(int nKind, MsgArg* pArgs, MsgArg* pResult);
s32  MC_CallActionFnMemoryRequired(void* pArg);
s32  MC_CallActionFnNumFilesOnCard(void* pArg);
s32  MC_CallActionFnDataCorrupt(void* pArg);
s32  MC_CallActionFnLoad(void* pArg);
s32  MC_CallActionFnSave(void* pArg);

// This file's message handlers, in address order.
void GM_vGetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage4_GetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage6_GetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetGameMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetNumPlayers(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage8_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vStartDemo(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage10_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCourse(MsgArg* pArgs, MsgArg* pResult);
void GM_vSelectSingleHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetHoleSet(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferAttribute(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferName(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage17_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vInitCustomRound(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetDemoSetupFlag(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumPlayers(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerController(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGameMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage25_Return1(MsgArg* pArgs, MsgArg* pResult);
void GM_vHideCharacter(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCharState(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferLastName(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetUserName(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetUserNames(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsCardPresent(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage31_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetControllerInputEnabled(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCConnect(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCDisconnect(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCLoadUser(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCSaveUser(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayMenuSound(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetWeatherOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCFormatCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetTeeSet(MsgArg* pArgs, MsgArg* pResult);
void GM_vProfileHasCreatedGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFirstCreatedGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage43_Return1(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCreatedGolferIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage45_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsGolferAvailable(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsControllerPluggedIn(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsCardFormatted(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsWrongDevice(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetFreeBlocks(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCOptionsMemoryRequired(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage326_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsMultitapPluggedIn(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCfunction(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCSaveOptions(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCLoadOptions(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOptionFlags1And2(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCommentaryOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetTapinsOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOptionFlags1And2(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCommentaryOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTapinsOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetStringWidth(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCUserMemoryRequired(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage325_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSelectFirstChallengeOfGroup(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage64_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferModelID(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLadderEventBonus(MsgArg* pArgs, MsgArg* pResult);
void GM_vFindGolferBio(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetBioTexts(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetBioLines(MsgArg* pArgs, MsgArg* pResult);
void GM_vSaveGolferModel(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileSecondName(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetProfileSecondName(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage287_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage299_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage302_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage292_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage312_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetAllGolfersPickable(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetAllGolfersPickable(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage314_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vHasProfileSecondName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileCash(MsgArg* pArgs, MsgArg* pResult);
void GM_vResetProfile(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumLadderEventsWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage218_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage217_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage216_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage222_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage301_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage75_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage76_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage77_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsGolferUnlocked(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCourseUnlockedOnAnyProfile(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage80_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage322_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCreatedGolferIndexOr0(MsgArg* pArgs, MsgArg* pResult);
void GM_vLookUpEarningsRange(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetProfileCash(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCreatedGolferAttribute(MsgArg* pArgs, MsgArg* pResult);
void GM_vQueueMovieKind2(MsgArg* pArgs, MsgArg* pResult);
void GM_vCharStream(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetSplitScreen(MsgArg* pArgs, MsgArg* pResult);
void GM_vAddHoleToRound(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage89_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCurrentProfileSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCurrentProfileSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vNextActiveProfileSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileName(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsProfileActive(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage94_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumControllersPluggedIn(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsControllerAssigned(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearControllerAssigned(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage98_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage99_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage100_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage101_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage102_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileRounds(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileStrokeAverage(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfilePuttsPerHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileAverageDrive(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileFairwayPercent(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileGreenPercent(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileLongestDrive(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileLongestPutt(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileHolesInOne(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileAlbatrosses(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileEagles(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileBirdies(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfilePars(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileBogeys(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileDoubleBogeys(MsgArg* pArgs, MsgArg* pResult);
void GM_vNextShirt(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPinSet(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetButtonConfig(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsTrophyBallWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsBonusTrophyBallWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEarningsTableA9B4(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetMulligan(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage124_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferClubAvailable(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage126_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vToggleClub(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage305_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetWindOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetVibration(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLowRoundRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLongestDriveRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLongestPuttRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGreensInRegRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFewestPuttsRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFairwaysHitRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEaglesRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetBirdiesRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCLoadReplay(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage139_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage140_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vCountMedalsInARow(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage142_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsUserNameOnCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetNumUsers(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCOverwriteUser(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage146_Return1(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage147_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsProfileLoaded(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFirstTimeInFE(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetFirstTimeInFE(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage151_Return30Or60(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetGolferDimmed(MsgArg* pArgs, MsgArg* pResult);
void GM_vTestPassword(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage154_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage155_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetDemoStarting(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsReplaySaved(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetMusicVolumeOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetSFXOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetMusicVolumeOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSFXOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetWeatherOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetWindOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetVibration(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage166_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetEditedGolferModelID(MsgArg* pArgs, MsgArg* pResult);
void GM_vStoreProfileInSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileMoney(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage169_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vTrophyBallsWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vBonusTrophyBallsWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileRecordsHeld(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileProgress(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileStats(MsgArg* pArgs, MsgArg* pResult);
void GM_vPar5Eagles(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetProfileNumLadderEventsWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vCountMedalsAndTourCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vCountMedal2(MsgArg* pArgs, MsgArg* pResult);
void GM_vCountMedal1(MsgArg* pArgs, MsgArg* pResult);
void GM_vCountBestMedalsAndTourCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTourCardLevel(MsgArg* pArgs, MsgArg* pResult);
void GM_vShowAwardReplay(MsgArg* pArgs, MsgArg* pResult);
void GM_vSaveCreatedPlayerToSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetMenuGameMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetMenuGameMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsProfileChanged(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetMostRewardsUnlocked(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEFormatWithCommas(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTourCardWithheld(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetTourCardWithheld(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCharacterHidden(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerController(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCoursePrice(MsgArg* pArgs, MsgArg* pResult);
void GM_vSaveProfileWithDefaultName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetHolePar(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetReplayCourseName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetReplayHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetReplayGolferLastName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferPrize(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage198_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetChallengeMedal(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCreatedGolferModelID(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCCheckCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsGolferUnlockedByDefault(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage203_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vUserHasEagledHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPar5EagleDate(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayCredits(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage206_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCourseUnlocked(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetHighestRewardUnlocked(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage209_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCustomRoundUsed(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCustomRoundName(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCustomRoundName(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCustomRoundHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCustomRoundN15(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFirstChar(MsgArg* pArgs, MsgArg* pResult);
void GM_vRotatePoint2D(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCustomRoundHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCustomRoundN15(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCustomRoundIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCustomRoundIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFEProfileN5(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetFEProfileN5(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage227_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetRandomCustomRoundHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerIsCPU(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerIsCPU(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOptionN14(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetGreenSpeedOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetFairwaySpeedOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetGreenGridOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCaddieTipsOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPuttingTipOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetBreakLineOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOnOffOption3(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOnOffOption4(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOnOffOption5(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOnOffOption6(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetSwingAidOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPowerBoostOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetSpinControlOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOptionLevel2(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOptionN14(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGreenSpeedOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFairwaySpeedOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGreenGridOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCaddieTipsOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPuttingTipOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetBreakLineOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOnOffOption3(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOnOffOption4(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOnOffOption5(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOnOffOption6(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSwingAidOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPowerBoostOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpinControlOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOptionLevel2(MsgArg* pArgs, MsgArg* pResult);
void GM_vSelectCustomRound(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsBackupProfileUnused(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetBackupProfileName(MsgArg* pArgs, MsgArg* pResult);
void GM_vLoadBackupProfile(MsgArg* pArgs, MsgArg* pResult);
void GM_vBackupAllProfiles(MsgArg* pArgs, MsgArg* pResult);
void GM_vBackupProfileClaimRow(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetRoughOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRoughOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage263_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCreatedGolferBallAndGlove(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCreatedGolferBallAndGlove(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCreatedGolferAttributeLevels(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetFEProfileN1(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFEProfileN1(MsgArg* pArgs, MsgArg* pResult);
void GM_vBuildAttributeLevelUps(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumAttributeLevelUps(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetAttributeLevelUp(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearAttributeLevelUps(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage273_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCHasSLUS20572Save(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCNumEASaveGames(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCEASaveExists(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetEASaveName(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetMCRewardMoney(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage278_Return2And1(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage279_Return7(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetCardErrors(MsgArg* pArgs, MsgArg* pResult);
void GM_vEndGameLoop(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCourseName(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage284_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage285_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage288_ReturnArg(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage290_Return2(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage291_Return4(MsgArg* pArgs, MsgArg* pResult);
void GM_vBackupProfile(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage295_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage296_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage297_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage298_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCFreeFiles(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSkillZoneOrLongDriveRecord(MsgArg* pArgs, MsgArg* pResult);
void GM_vEmptyProfileSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage307_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetChallengeRewards(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEGetStringLength(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetEATraxTrack(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEATraxTrack(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCustomRoundSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGameName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEATraxOptions(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetEATraxOptions(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCourseChoiceUnlocked(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLadderEventMaxSkins(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage324_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vQueueMovie(MsgArg* pArgs, MsgArg* pResult);
void GM_vStopMusic(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage328_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerCreatedGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearBackupRows(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearBackupRow(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsWaveBird(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetFEStateB10(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetFEStateB10(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage336_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetEASBSaveNeeds(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage338_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage339_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage340_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerBag(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearSavedRound(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetSavedRoundInUse(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage344_Return150(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage345_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage346_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage347_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage348_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage349_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage350_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage351_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage352_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage353_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage354_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vCompareStrings(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage356_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage357_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage358_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage359_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage360_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage361_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage362_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage363_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage364_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage365_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage366_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage367_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLeftHanded(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage369_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage370_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetThreeLevelsOneRaised(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage372_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage380_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vMaskString(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLongDriveTargetScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vSwapDisc(MsgArg* pArgs, MsgArg* pResult);
void GM_vStartEventCheckDisc(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaStartLink(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaTakeCash(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaSwapStats(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage595_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaCancelLink(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaGrantUnlocks(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage630_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaGetLinkState(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaReadCashAndStats(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaAddCashToMove(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage618_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage619_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaIsReadPending(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaGetGbaCash(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaIsLinkFailed(MsgArg* pArgs, MsgArg* pResult);
void GM_vGbaResumeLink(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioListProducts(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioListAccomplishments(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioShowSummary(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioShowProductDetails(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioLoad(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioSave(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioIsOnCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioMemoryRequired(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioLoadProducts(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioIsLoaded(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioIsNotWrongFile(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioIsReplacingOldest(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioIsBadData(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioDelete(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage655_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioGetLevel(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioCheckReward(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage667_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioUnloadProducts(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioGetLevelProgress(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCSetCurrentFileType(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCCallActionFn(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage675_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayNowGetNumGroups(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayNowSelectGroup(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayNowGetGroupName(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayNowGetGroupDescription(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayNowGetGroupMedal(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCourseFindDisc(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLongDriveOptions(MsgArg* pArgs, MsgArg* pResult);
void GM_vTrophyRoomGetStatus(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage715_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vControlMusic(MsgArg* pArgs, MsgArg* pResult);
void GM_vDefaultBlankUserName(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCIsUserOnCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vShortenString12(MsgArg* pArgs, MsgArg* pResult);
void GM_vShortenString32(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioGetLastError(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioCheck(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetBlocksNeeded(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage762_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage763_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetLastSavedUser(MsgArg* pArgs, MsgArg* pResult);
void GM_vFEMessage765_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioCreate(MsgArg* pArgs, MsgArg* pResult);
void GM_vEASBioGetRewardMessage(MsgArg* pArgs, MsgArg* pResult);

// The front end's message handlers by message number (FE_InitGameMessages fills 612 of the 770
// slots; the rest stay NULL). GameUICommands.c's gIGMessageHandlers is the round's twin.
#define FE_NUM_MESSAGES 770
MsgHandler gFEMessageHandlers[FE_NUM_MESSAGES];

// Runs front-end message nMsg: the handler in gFEMessageHandlers[nMsg] gets the message's values
// (pArgs) and its answer (pResult). uiProcessInterface.c sends the menu UI's messages here while
// the front end runs (game type 3). The slot is not checked: slots without a handler are NULL.
void FE_RunGameMessage(int nMsg, MsgArg* pArgs, MsgArg* pResult) {
    gFEMessageHandlers[nMsg](pArgs, pResult);
}

// Fills gFEMessageHandlers, the front end's messages by number (called from FE_Manager.c's
// FE_vInitModule as the front end starts): every slot NULL, then 612 handlers of this file and the
// other front-end files (Create-A-Player, the logo editor, the PGA TOUR screens, the calendar, the
// ladder, the trophy room...); the rest, message 1 among them, stay NULL.
void FE_InitGameMessages(void) {
    memset(gFEMessageHandlers, 0, sizeof(gFEMessageHandlers));
    gFEMessageHandlers[1] = NULL;
    gFEMessageHandlers[2] = GM_vGetMinPlayersForMode;
    gFEMessageHandlers[3] = GM_vSetupPlayers;
    gFEMessageHandlers[4] = GM_vMessage4_GetMinPlayersForMode;
    gFEMessageHandlers[5] = GM_vSetGameMode;
    gFEMessageHandlers[6] = GM_vMessage6_GetMinPlayersForMode;
    gFEMessageHandlers[7] = GM_vSetNumPlayers;
    gFEMessageHandlers[8] = GM_vFEMessage8_Empty;
    gFEMessageHandlers[9] = GM_vStartDemo;
    gFEMessageHandlers[10] = GM_vFEMessage10_Empty;
    gFEMessageHandlers[12] = GM_vSetCourse;
    gFEMessageHandlers[13] = GM_vSelectSingleHole;
    gFEMessageHandlers[14] = GM_vSetHoleSet;
    gFEMessageHandlers[15] = GM_vGetGolferAttribute;
    gFEMessageHandlers[16] = GM_vGetGolferName;
    gFEMessageHandlers[17] = GM_vFEMessage17_Empty;
    gFEMessageHandlers[18] = GM_vInitCustomRound;
    gFEMessageHandlers[19] = GM_vGetDemoSetupFlag;
    gFEMessageHandlers[20] = GM_vSetPlayerGolfer;
    gFEMessageHandlers[21] = GM_vGetNumPlayers;
    gFEMessageHandlers[22] = GM_vSetPlayerController;
    gFEMessageHandlers[23] = GM_vGetPlayerGolfer;
    gFEMessageHandlers[24] = GM_vGetGameMode;
    gFEMessageHandlers[25] = GM_vFEMessage25_Return1;
    gFEMessageHandlers[26] = GM_vHideCharacter;
    gFEMessageHandlers[27] = GM_vSetCharState;
    gFEMessageHandlers[28] = GM_vGetGolferLastName;
    gFEMessageHandlers[29] = GM_vMCGetUserName;
    gFEMessageHandlers[30] = GM_vMCIsCardPresent;
    gFEMessageHandlers[31] = GM_vFEMessage31_Empty;
    gFEMessageHandlers[32] = GM_vSetControllerInputEnabled;
    gFEMessageHandlers[33] = GM_vMCConnect;
    gFEMessageHandlers[34] = GM_vMCDisconnect;
    gFEMessageHandlers[35] = GM_vMCLoadUser;
    gFEMessageHandlers[36] = GM_vMCSaveUser;
    gFEMessageHandlers[37] = GM_vPlayMenuSound;
    gFEMessageHandlers[38] = GM_vSetWeatherOption;
    gFEMessageHandlers[39] = GM_vGetFirstCreatedGolfer;
    gFEMessageHandlers[40] = GM_vMCFormatCard;
    gFEMessageHandlers[41] = GM_vSetTeeSet;
    gFEMessageHandlers[42] = GM_vProfileHasCreatedGolfer;
    gFEMessageHandlers[43] = GM_vFEMessage43_Return1;
    gFEMessageHandlers[44] = GM_vGetCreatedGolferIndex;
    gFEMessageHandlers[45] = GM_vFEMessage45_Empty;
    gFEMessageHandlers[46] = LadderMenu_StartEvent;
    gFEMessageHandlers[47] = GM_vIsGolferAvailable;
    gFEMessageHandlers[48] = GM_vIsControllerPluggedIn;
    gFEMessageHandlers[49] = GM_vMCIsCardFormatted;
    gFEMessageHandlers[50] = GM_vMCGetFreeBlocks;
    gFEMessageHandlers[51] = GM_vMCOptionsMemoryRequired;
    gFEMessageHandlers[52] = GM_vMCIsMultitapPluggedIn;
    gFEMessageHandlers[53] = GM_vMCfunction;
    gFEMessageHandlers[54] = GM_vMCSaveOptions;
    gFEMessageHandlers[537] = GM_vMCLoadOptions;
    gFEMessageHandlers[55] = GM_vSetOptionFlags1And2;
    gFEMessageHandlers[56] = GM_vSetCommentaryOption;
    gFEMessageHandlers[57] = GM_vSetTapinsOption;
    gFEMessageHandlers[58] = GM_vGetOptionFlags1And2;
    gFEMessageHandlers[59] = GM_vGetCommentaryOption;
    gFEMessageHandlers[60] = GM_vGetTapinsOption;
    gFEMessageHandlers[61] = GM_vGetStringWidth;
    gFEMessageHandlers[62] = GM_vMCUserMemoryRequired;
    gFEMessageHandlers[63] = GM_vSelectFirstChallengeOfGroup;
    gFEMessageHandlers[64] = GM_vFEMessage64_Empty;
    gFEMessageHandlers[65] = GM_vGetGolferModelID;
    gFEMessageHandlers[66] = GM_vSetLadderEventBonus;
    gFEMessageHandlers[67] = GM_vFindGolferBio;
    gFEMessageHandlers[68] = GM_vGetBioTexts;
    gFEMessageHandlers[69] = GM_vSaveGolferModel;
    gFEMessageHandlers[70] = GM_vGetProfileSecondName;
    gFEMessageHandlers[71] = GM_vSetProfileSecondName;
    gFEMessageHandlers[72] = GM_vGetProfileCash;
    gFEMessageHandlers[73] = GM_vResetProfile;
    gFEMessageHandlers[74] = GM_vGetNumLadderEventsWon;
    gFEMessageHandlers[75] = GM_vFEMessage75_Empty;
    gFEMessageHandlers[76] = GM_vFEMessage76_Empty;
    gFEMessageHandlers[77] = GM_vFEMessage77_Empty;
    gFEMessageHandlers[78] = GM_vIsGolferUnlocked;
    gFEMessageHandlers[79] = GM_vIsCourseUnlockedOnAnyProfile;
    gFEMessageHandlers[80] = GM_vFEMessage80_Empty;
    gFEMessageHandlers[81] = GM_vGetCreatedGolferIndexOr0;
    gFEMessageHandlers[82] = GM_vLookUpEarningsRange;
    gFEMessageHandlers[83] = GM_vSetProfileCash;
    gFEMessageHandlers[84] = GM_vSetCreatedGolferAttribute;
    gFEMessageHandlers[85] = GM_vQueueMovieKind2;
    gFEMessageHandlers[86] = GM_vCharStream;
    gFEMessageHandlers[87] = GM_vSetSplitScreen;
    gFEMessageHandlers[88] = GM_vAddHoleToRound;
    gFEMessageHandlers[89] = GM_vFEMessage89_Empty;
    gFEMessageHandlers[90] = GM_vSetCurrentProfileSlot;
    gFEMessageHandlers[91] = GM_vGetCurrentProfileSlot;
    gFEMessageHandlers[92] = GM_vGetProfileName;
    gFEMessageHandlers[94] = GM_vFEMessage94_Empty;
    gFEMessageHandlers[95] = GM_vGetNumControllersPluggedIn;
    gFEMessageHandlers[96] = GM_vIsControllerAssigned;
    gFEMessageHandlers[97] = GM_vClearControllerAssigned;
    gFEMessageHandlers[98] = GM_vFEMessage98_Empty;
    gFEMessageHandlers[99] = GM_vFEMessage99_Empty;
    gFEMessageHandlers[100] = GM_vFEMessage100_Empty;
    gFEMessageHandlers[101] = GM_vFEMessage101_Empty;
    gFEMessageHandlers[102] = GM_vFEMessage102_Empty;
    gFEMessageHandlers[103] = GM_vGetProfileRounds;
    gFEMessageHandlers[104] = GM_vGetProfileStrokeAverage;
    gFEMessageHandlers[105] = GM_vGetProfilePuttsPerHole;
    gFEMessageHandlers[106] = GM_vGetProfileAverageDrive;
    gFEMessageHandlers[107] = GM_vGetProfileFairwayPercent;
    gFEMessageHandlers[108] = GM_vGetProfileGreenPercent;
    gFEMessageHandlers[109] = GM_vGetProfileLongestDrive;
    gFEMessageHandlers[110] = GM_vGetProfileLongestPutt;
    gFEMessageHandlers[111] = GM_vGetProfileHolesInOne;
    gFEMessageHandlers[112] = GM_vGetProfileAlbatrosses;
    gFEMessageHandlers[113] = GM_vGetProfileEagles;
    gFEMessageHandlers[114] = GM_vGetProfileBirdies;
    gFEMessageHandlers[115] = GM_vGetProfilePars;
    gFEMessageHandlers[116] = GM_vGetProfileBogeys;
    gFEMessageHandlers[117] = GM_vGetProfileDoubleBogeys;
    gFEMessageHandlers[118] = GM_vNextShirt;
    gFEMessageHandlers[119] = GM_vSetPinSet;
    gFEMessageHandlers[120] = GM_vSetButtonConfig;
    gFEMessageHandlers[121] = GM_vIsTrophyBallWon;
    gFEMessageHandlers[122] = GM_vGetEarningsTableA9B4;
    gFEMessageHandlers[123] = GM_vSetMulligan;
    gFEMessageHandlers[124] = GM_vFEMessage124_Empty;
    gFEMessageHandlers[125] = GM_vGetGolferClubAvailable;
    gFEMessageHandlers[126] = GM_vFEMessage126_Empty;
    gFEMessageHandlers[127] = GM_vToggleClub;
    gFEMessageHandlers[128] = GM_vSetWindOption;
    gFEMessageHandlers[129] = GM_vSetVibration;
    gFEMessageHandlers[130] = GM_vGetLowRoundRecord;
    gFEMessageHandlers[131] = GM_vGetLongestDriveRecord;
    gFEMessageHandlers[132] = GM_vGetLongestPuttRecord;
    gFEMessageHandlers[133] = GM_vGetGreensInRegRecord;
    gFEMessageHandlers[134] = GM_vGetFewestPuttsRecord;
    gFEMessageHandlers[135] = GM_vGetFairwaysHitRecord;
    gFEMessageHandlers[136] = GM_vGetEaglesRecord;
    gFEMessageHandlers[137] = GM_vGetBirdiesRecord;
    gFEMessageHandlers[138] = GM_vMCLoadReplay;
    gFEMessageHandlers[139] = GM_vFEMessage139_Empty;
    gFEMessageHandlers[140] = GM_vFEMessage140_Empty;
    gFEMessageHandlers[141] = GM_vCountMedalsInARow;
    gFEMessageHandlers[142] = GM_vFEMessage142_Empty;
    gFEMessageHandlers[143] = GM_vMCIsUserNameOnCard;
    gFEMessageHandlers[144] = GM_vMCGetNumUsers;
    gFEMessageHandlers[145] = GM_vMCOverwriteUser;
    gFEMessageHandlers[146] = GM_vFEMessage146_Return1;
    gFEMessageHandlers[147] = GM_vFEMessage147_Empty;
    gFEMessageHandlers[148] = GM_vIsProfileLoaded;
    gFEMessageHandlers[149] = GM_vGetFirstTimeInFE;
    gFEMessageHandlers[150] = GM_vSetFirstTimeInFE;
    gFEMessageHandlers[152] = GM_vSetGolferDimmed;
    gFEMessageHandlers[153] = GM_vTestPassword;
    gFEMessageHandlers[154] = GM_vFEMessage154_Empty;
    gFEMessageHandlers[155] = GM_vFEMessage155_Empty;
    gFEMessageHandlers[156] = GM_vGetDemoStarting;
    gFEMessageHandlers[157] = GM_vMCGetNumReplays;
    gFEMessageHandlers[158] = GM_vMCIsReplaySaved;
    gFEMessageHandlers[159] = GM_vSetMusicVolumeOption;
    gFEMessageHandlers[160] = GM_vSetSFXOption;
    gFEMessageHandlers[161] = GM_vGetMusicVolumeOption;
    gFEMessageHandlers[162] = GM_vGetSFXOption;
    gFEMessageHandlers[163] = GM_vGetWeatherOption;
    gFEMessageHandlers[164] = GM_vGetWindOption;
    gFEMessageHandlers[165] = GM_vGetVibration;
    gFEMessageHandlers[166] = GM_vFEMessage166_Empty;
    gFEMessageHandlers[167] = GM_vStoreProfileInSlot;
    gFEMessageHandlers[168] = GM_vGetProfileMoney;
    gFEMessageHandlers[169] = GM_vFEMessage169_Empty;
    gFEMessageHandlers[170] = GM_vTrophyBallsWon;
    gFEMessageHandlers[171] = GM_vGetProfileRecordsHeld;
    gFEMessageHandlers[172] = GM_vGetProfileStats;
    gFEMessageHandlers[173] = GM_vPar5Eagles;
    gFEMessageHandlers[174] = GM_vGetProfileNumLadderEventsWon;
    gFEMessageHandlers[175] = GM_vCountMedalsAndTourCard;
    gFEMessageHandlers[176] = GM_vCountMedal2;
    gFEMessageHandlers[177] = GM_vCountMedal1;
    gFEMessageHandlers[178] = GM_vCountBestMedalsAndTourCard;
    gFEMessageHandlers[179] = GM_vGetTourCardLevel;
    gFEMessageHandlers[180] = GM_vShowAwardReplay;
    gFEMessageHandlers[181] = GM_vSaveCreatedPlayerToSlot;
    gFEMessageHandlers[182] = GM_vGetMenuGameMode;
    gFEMessageHandlers[183] = GM_vSetMenuGameMode;
    gFEMessageHandlers[184] = GM_vIsProfileChanged;
    gFEMessageHandlers[185] = GM_vGetMostRewardsUnlocked;
    gFEMessageHandlers[186] = GM_vFEFormatWithCommas;
    gFEMessageHandlers[187] = GM_vGetTourCardWithheld;
    gFEMessageHandlers[188] = GM_vSetTourCardWithheld;
    gFEMessageHandlers[189] = GM_vIsCharacterHidden;
    gFEMessageHandlers[190] = GM_vGetPlayerController;
    gFEMessageHandlers[191] = GM_vGetCoursePrice;
    gFEMessageHandlers[192] = GM_vSaveProfileWithDefaultName;
    gFEMessageHandlers[193] = GM_vGetHolePar;
    gFEMessageHandlers[194] = GM_vGetReplayCourseName;
    gFEMessageHandlers[195] = GM_vGetReplayHole;
    gFEMessageHandlers[196] = GM_vGetReplayGolferLastName;
    gFEMessageHandlers[197] = GM_vGetGolferPrize;
    gFEMessageHandlers[198] = GM_vFEMessage198_Empty;
    gFEMessageHandlers[199] = GM_vGetChallengeMedal;
    gFEMessageHandlers[200] = GM_vGetCreatedGolferModelID;
    gFEMessageHandlers[201] = GM_vMCCheckCard;
    gFEMessageHandlers[202] = GM_vIsGolferUnlockedByDefault;
    gFEMessageHandlers[203] = GM_vFEMessage203_Return0;
    gFEMessageHandlers[204] = GM_vUserHasEagledHole;
    gFEMessageHandlers[205] = GM_vPlayCredits;
    gFEMessageHandlers[206] = GM_vFEMessage206_Empty;
    gFEMessageHandlers[207] = GM_vIsCourseUnlocked;
    gFEMessageHandlers[208] = GM_vGetHighestRewardUnlocked;
    gFEMessageHandlers[209] = GM_vFEMessage209_Return0;
    gFEMessageHandlers[151] = GM_vFEMessage151_Return30Or60;
    gFEMessageHandlers[93] = GM_vIsProfileActive;
    gFEMessageHandlers[210] = GM_vIsCustomRoundUsed;
    gFEMessageHandlers[211] = GM_vGetCustomRoundName;
    gFEMessageHandlers[212] = GM_vSetCustomRoundName;
    gFEMessageHandlers[213] = GM_vSetCustomRoundHole;
    gFEMessageHandlers[214] = GM_vSetCustomRoundN15;
    gFEMessageHandlers[215] = GM_vGetFirstChar;
    gFEMessageHandlers[216] = GM_vFEMessage216_Empty;
    gFEMessageHandlers[217] = GM_vFEMessage217_Empty;
    gFEMessageHandlers[218] = GM_vFEMessage218_Empty;
    gFEMessageHandlers[219] = GM_vRotatePoint2D;
    gFEMessageHandlers[220] = GM_vGetCustomRoundHole;
    gFEMessageHandlers[221] = GM_vGetCustomRoundN15;
    gFEMessageHandlers[222] = GM_vFEMessage222_Empty;
    gFEMessageHandlers[223] = GM_vGetCustomRoundIndex;
    gFEMessageHandlers[224] = GM_vSetCustomRoundIndex;
    gFEMessageHandlers[225] = GM_vGetFEProfileN5;
    gFEMessageHandlers[226] = GM_vSetFEProfileN5;
    gFEMessageHandlers[227] = GM_vFEMessage227_Return0;
    gFEMessageHandlers[228] = GM_vSetRandomCustomRoundHole;
    gFEMessageHandlers[229] = GM_vSetPlayerIsCPU;
    gFEMessageHandlers[230] = GM_vGetPlayerIsCPU;
    gFEMessageHandlers[231] = GM_vSetOptionN14;
    gFEMessageHandlers[232] = GM_vSetGreenSpeedOption;
    gFEMessageHandlers[233] = GM_vSetCaddieTipsOption;
    gFEMessageHandlers[234] = GM_vSetBreakLineOption;
    gFEMessageHandlers[235] = GM_vSetOnOffOption3;
    gFEMessageHandlers[236] = GM_vSetOnOffOption4;
    gFEMessageHandlers[237] = GM_vSetOnOffOption5;
    gFEMessageHandlers[238] = GM_vSetOnOffOption6;
    gFEMessageHandlers[239] = GM_vSetSwingAidOption;
    gFEMessageHandlers[240] = GM_vSetPowerBoostOption;
    gFEMessageHandlers[241] = GM_vSetSpinControlOption;
    gFEMessageHandlers[242] = GM_vSetOptionLevel2;
    gFEMessageHandlers[243] = GM_vGetOptionN14;
    gFEMessageHandlers[244] = GM_vGetGreenSpeedOption;
    gFEMessageHandlers[245] = GM_vGetCaddieTipsOption;
    gFEMessageHandlers[246] = GM_vGetBreakLineOption;
    gFEMessageHandlers[247] = GM_vGetOnOffOption3;
    gFEMessageHandlers[248] = GM_vGetOnOffOption4;
    gFEMessageHandlers[249] = GM_vGetOnOffOption5;
    gFEMessageHandlers[250] = GM_vGetOnOffOption6;
    gFEMessageHandlers[251] = GM_vGetSwingAidOption;
    gFEMessageHandlers[252] = GM_vGetPowerBoostOption;
    gFEMessageHandlers[253] = GM_vGetSpinControlOption;
    gFEMessageHandlers[254] = GM_vGetOptionLevel2;
    gFEMessageHandlers[255] = GM_vSelectCustomRound;
    gFEMessageHandlers[256] = GM_vIsBackupProfileUnused;
    gFEMessageHandlers[257] = GM_vGetBackupProfileName;
    gFEMessageHandlers[258] = GM_vLoadBackupProfile;
    gFEMessageHandlers[259] = GM_vBackupAllProfiles;
    gFEMessageHandlers[260] = GM_vBackupProfileClaimRow;
    gFEMessageHandlers[261] = GM_vSetRoughOption;
    gFEMessageHandlers[262] = GM_vGetRoughOption;
    gFEMessageHandlers[263] = GM_vFEMessage263_Empty;
    gFEMessageHandlers[264] = GM_vSetCreatedGolferBallAndGlove;
    gFEMessageHandlers[265] = GM_vGetCreatedGolferBallAndGlove;
    gFEMessageHandlers[266] = GM_vGetCreatedGolferAttributeLevels;
    gFEMessageHandlers[267] = GM_vSetFEProfileN1;
    gFEMessageHandlers[268] = GM_vGetFEProfileN1;
    gFEMessageHandlers[269] = GM_vBuildAttributeLevelUps;
    gFEMessageHandlers[270] = GM_vGetNumAttributeLevelUps;
    gFEMessageHandlers[271] = GM_vGetAttributeLevelUp;
    gFEMessageHandlers[272] = GM_vClearAttributeLevelUps;
    gFEMessageHandlers[273] = GM_vFEMessage273_Return0;
    gFEMessageHandlers[274] = GM_vMCNumEASaveGames;
    gFEMessageHandlers[275] = GM_vMCEASaveExists;
    gFEMessageHandlers[276] = GM_vMCGetEASaveName;
    gFEMessageHandlers[277] = GM_vSetMCRewardMoney;
    gFEMessageHandlers[278] = GM_vFEMessage278_Return2And1;
    gFEMessageHandlers[279] = GM_vFEMessage279_Return7;
    gFEMessageHandlers[280] = GM_vMCGetCardErrors;
    gFEMessageHandlers[281] = GM_vEndGameLoop;
    gFEMessageHandlers[282] = GM_vGetCourseName;
    gFEMessageHandlers[283] = GM_vHasProfileSecondName;
    gFEMessageHandlers[284] = GM_vFEMessage284_Empty;
    gFEMessageHandlers[285] = GM_vFEMessage285_Empty;
    gFEMessageHandlers[286] = GM_vSetEditedGolferModelID;
    gFEMessageHandlers[287] = GM_vFEMessage287_Empty;
    gFEMessageHandlers[288] = GM_vFEMessage288_ReturnArg;
    gFEMessageHandlers[289] = GM_vFEGetLetter;
    gFEMessageHandlers[290] = GM_vFEMessage290_Return2;
    gFEMessageHandlers[291] = GM_vFEMessage291_Return4;
    gFEMessageHandlers[292] = GM_vFEMessage292_Empty;
    gFEMessageHandlers[293] = GM_vGetBioLines;
    gFEMessageHandlers[294] = GM_vBackupProfile;
    gFEMessageHandlers[295] = GM_vFEMessage295_Return0;
    gFEMessageHandlers[296] = GM_vFEMessage296_Empty;
    gFEMessageHandlers[297] = GM_vFEMessage297_Empty;
    gFEMessageHandlers[298] = GM_vFEMessage298_Empty;
    gFEMessageHandlers[299] = GM_vFEMessage299_Empty;
    gFEMessageHandlers[300] = GM_vMCFreeFiles;
    gFEMessageHandlers[301] = GM_vFEMessage301_Empty;
    gFEMessageHandlers[302] = GM_vFEMessage302_Empty;
    gFEMessageHandlers[303] = GM_vSetAllGolfersPickable;
    gFEMessageHandlers[304] = GM_vGetSkillZoneOrLongDriveRecord;
    gFEMessageHandlers[305] = GM_vFEMessage305_Empty;
    gFEMessageHandlers[306] = GM_vEmptyProfileSlot;
    gFEMessageHandlers[307] = GM_vFEMessage307_Empty;
    gFEMessageHandlers[308] = GM_vGetChallengeRewards;
    gFEMessageHandlers[309] = GM_vFEGetStringLength;
    gFEMessageHandlers[310] = GM_vSetEATraxTrack;
    gFEMessageHandlers[311] = GM_vGetEATraxTrack;
    gFEMessageHandlers[312] = GM_vFEMessage312_Empty;
    gFEMessageHandlers[313] = GM_vGetAllGolfersPickable;
    gFEMessageHandlers[314] = GM_vFEMessage314_Empty;
    gFEMessageHandlers[315] = GM_vSetCustomRoundSlot;
    gFEMessageHandlers[316] = GM_vGetGameName;
    gFEMessageHandlers[317] = GM_vMCIsSaveCorrupt;
    gFEMessageHandlers[318] = GM_vMCDeleteSave;
    gFEMessageHandlers[319] = GM_vGetEATraxOptions;
    gFEMessageHandlers[320] = GM_vSetEATraxOptions;
    gFEMessageHandlers[321] = GM_vIsCourseChoiceUnlocked;
    gFEMessageHandlers[322] = GM_vFEMessage322_Empty;
    gFEMessageHandlers[323] = GM_vGetLadderEventMaxSkins;
    gFEMessageHandlers[324] = GM_vFEMessage324_Empty;
    gFEMessageHandlers[325] = GM_vFEMessage325_Empty;
    gFEMessageHandlers[326] = GM_vFEMessage326_Empty;
    gFEMessageHandlers[327] = GM_vQueueMovie;
    gFEMessageHandlers[328] = GM_vFEMessage328_Return0;
    gFEMessageHandlers[329] = GM_vSetPlayerCreatedGolfer;
    gFEMessageHandlers[330] = GM_vClearBackupRows;
    gFEMessageHandlers[331] = GM_vClearBackupRow;
    gFEMessageHandlers[332] = GM_vIsWaveBird;
    gFEMessageHandlers[333] = GM_vGetFEStateB10;
    gFEMessageHandlers[334] = GM_vSetFEStateB10;
    gFEMessageHandlers[335] = GM_vMCHadIOError;
    gFEMessageHandlers[336] = GM_vFEMessage336_Return0;
    gFEMessageHandlers[337] = GM_vMCGetSaveNeeds;
    gFEMessageHandlers[766] = GM_vMCGetEASBSaveNeeds;
    gFEMessageHandlers[338] = GM_vFEMessage338_Empty;
    gFEMessageHandlers[339] = GM_vFEMessage339_Return0;
    gFEMessageHandlers[340] = GM_vFEMessage340_Return0;
    gFEMessageHandlers[341] = GM_vSetPlayerBag;
    gFEMessageHandlers[342] = GM_vClearSavedRound;
    gFEMessageHandlers[343] = GM_vSetSavedRoundInUse;
    gFEMessageHandlers[344] = GM_vFEMessage344_Return150;
    gFEMessageHandlers[345] = GM_vFEMessage345_Empty;
    gFEMessageHandlers[346] = GM_vFEMessage346_Empty;
    gFEMessageHandlers[347] = GM_vFEMessage347_Empty;
    gFEMessageHandlers[348] = GM_vFEMessage348_Empty;
    gFEMessageHandlers[349] = GM_vFEMessage349_Empty;
    gFEMessageHandlers[350] = GM_vFEMessage350_Empty;
    gFEMessageHandlers[351] = GM_vFEMessage351_Empty;
    gFEMessageHandlers[352] = GM_vFEMessage352_Empty;
    gFEMessageHandlers[353] = GM_vFEMessage353_Empty;
    gFEMessageHandlers[354] = GM_vFEMessage354_Empty;
    gFEMessageHandlers[355] = GM_vCompareStrings;
    gFEMessageHandlers[356] = GM_vFEMessage356_Empty;
    gFEMessageHandlers[357] = GM_vFEMessage357_Empty;
    gFEMessageHandlers[358] = GM_vFEMessage358_Empty;
    gFEMessageHandlers[359] = GM_vFEMessage359_Empty;
    gFEMessageHandlers[360] = GM_vFEMessage360_Empty;
    gFEMessageHandlers[361] = GM_vFEMessage361_Empty;
    gFEMessageHandlers[362] = GM_vFEMessage362_Empty;
    gFEMessageHandlers[363] = GM_vFEMessage363_Empty;
    gFEMessageHandlers[364] = GM_vFEMessage364_Empty;
    gFEMessageHandlers[365] = GM_vFEMessage365_Empty;
    gFEMessageHandlers[366] = GM_vFEMessage366_Empty;
    gFEMessageHandlers[367] = GM_vFEMessage367_Empty;
    gFEMessageHandlers[368] = GM_vSetLeftHanded;
    gFEMessageHandlers[369] = GM_vFEMessage369_Return0;
    gFEMessageHandlers[370] = GM_vFEMessage370_Empty;
    gFEMessageHandlers[371] = GM_vGetThreeLevelsOneRaised;
    gFEMessageHandlers[372] = GM_vFEMessage372_Empty;
    gFEMessageHandlers[374] = GM_vGetFairwaySpeedOption;
    gFEMessageHandlers[375] = GM_vSetFairwaySpeedOption;
    gFEMessageHandlers[380] = GM_vFEMessage380_Empty;
    gFEMessageHandlers[381] = GM_vGetNumCrAPItems;
    gFEMessageHandlers[382] = GM_vGetCrAPItemValue;
    gFEMessageHandlers[383] = GM_vGetCrAPItemColor;
    gFEMessageHandlers[399] = GM_vMaskString;
    gFEMessageHandlers[404] = GM_vFEMessage404_Empty;
    gFEMessageHandlers[405] = GM_vFEMessage405_Empty;
    gFEMessageHandlers[406] = GM_vSetCRAPSlider;
    gFEMessageHandlers[407] = GM_vGetCRAPItem;
    gFEMessageHandlers[408] = GM_vFEMessage408_Empty;
    gFEMessageHandlers[409] = GM_vGetCRAPSlider;
    gFEMessageHandlers[410] = GM_vGetNumCrAPGeometries;
    gFEMessageHandlers[411] = GM_vFEMessage411_Return0;
    gFEMessageHandlers[412] = GM_vGetCrAPItemInfo;
    gFEMessageHandlers[451] = GM_vGetCrAPItemAttributes;
    gFEMessageHandlers[452] = GM_vGetCrAPItemPriceAndLock;
    gFEMessageHandlers[413] = GM_vCRAPTryOnItem;
    gFEMessageHandlers[441] = GM_vGetNumCrAPSubcategories;
    gFEMessageHandlers[453] = GM_vSetCrAPGolferInfo;
    gFEMessageHandlers[454] = GM_vGetCrAPGolferInfo;
    gFEMessageHandlers[457] = GM_vSetUseProfileCopy;
    gFEMessageHandlers[460] = GM_vIsCrAPItemLocked;
    gFEMessageHandlers[461] = GM_vIsCrAPItemOwned;
    gFEMessageHandlers[462] = GM_vPurchaseCrAPItem;
    gFEMessageHandlers[470] = GM_vIsCrAPCategoryWorn;
    gFEMessageHandlers[474] = GM_vIsCrAPItemEquipped;
    gFEMessageHandlers[475] = GM_vGetCrAPSaleItems;
    gFEMessageHandlers[476] = GM_vGetCrAPSaleSubcategories;
    gFEMessageHandlers[477] = GM_vIsCrAPItemOnSale;
    gFEMessageHandlers[498] = GM_vCheckCrAPUnlocks;
    gFEMessageHandlers[499] = GM_vIsCrAPItemNew;
    gFEMessageHandlers[500] = GM_vMarkNewCrAPItem;
    gFEMessageHandlers[501] = GM_vClearMarkedNewCrAPItems;
    gFEMessageHandlers[503] = GM_vSetProfileFlag;
    gFEMessageHandlers[504] = GM_vGetProfileFlag;
    gFEMessageHandlers[507] = GM_vGetNewCrAPItemsText;
    gFEMessageHandlers[509] = GM_vGetTodaysDate;
    gFEMessageHandlers[512] = GM_vGetRandom1To99;
    gFEMessageHandlers[515] = GM_vGetGolferAttributeTier;
    gFEMessageHandlers[527] = GM_vGetCrAPSubcategoryName;
    gFEMessageHandlers[528] = GM_vGetCrAPColorName;
    gFEMessageHandlers[529] = GM_vGetCrAPItemUnlockText;
    gFEMessageHandlers[534] = GM_vFEMessage534_Empty;
    gFEMessageHandlers[535] = GM_vSetCrAPCameraIdleState;
    gFEMessageHandlers[538] = GM_vHasMenuGolfer;
    gFEMessageHandlers[539] = GM_vFEMessage539_Empty;
    gFEMessageHandlers[542] = GM_vGetUseProfileCopy;
    gFEMessageHandlers[547] = GM_vRandomizeCrAPGolferInIdleShot;
    gFEMessageHandlers[554] = GM_vRandomizeCrAPLookInFaceShot;
    gFEMessageHandlers[557] = GM_vGetDPadHeld;
    gFEMessageHandlers[562] = GM_vIsMenuGolferReady;
    gFEMessageHandlers[687] = GM_vIsGolferLoaderIdle;
    gFEMessageHandlers[566] = GM_vFEMessage566_Empty;
    gFEMessageHandlers[458] = PGALeaderboard_GetRow;
    gFEMessageHandlers[459] = PGALeaderboard_GetNumRows;
    gFEMessageHandlers[466] = PGASchedule_GetRow;
    gFEMessageHandlers[467] = PGASchedule_Build;
    gFEMessageHandlers[520] = PGATourMsg_GetLastEventLine;
    gFEMessageHandlers[551] = PGATourMsg_IsSeasonOver;
    gFEMessageHandlers[552] = PGASeasonWrapUp_GetLine;
    // port: EA passes two arguments PGADriver_ShowCalendar_AdvanceSeason ignores (defined (void))
    gFEMessageHandlers[553] = (MsgHandler)PGADriver_ShowCalendar_AdvanceSeason;
    gFEMessageHandlers[559] = PGATourMsg_GetTestText;
    gFEMessageHandlers[560] = GM_vFEMessage560_Return25;
    gFEMessageHandlers[563] = PGASponsor_SignNext;
    gFEMessageHandlers[742] = PGATourMsg_SetStatsDirty;
    gFEMessageHandlers[743] = PGATourMsg_SetScoresDirty;
    gFEMessageHandlers[472] = GM_vStartEventCheckDisc;
    gFEMessageHandlers[465] = GM_vSwapDisc;
    gFEMessageHandlers[473] = GM_vGetDiscChangeStatus;
    gFEMessageHandlers[468] = Calendar_FillCell;
    gFEMessageHandlers[469] = Calendar_GetLine;
    gFEMessageHandlers[483] = Calendar_GetBottomLine;
    gFEMessageHandlers[484] = Calendar_GetMonthShown;
    gFEMessageHandlers[485] = Calendar_PrevMonth;
    gFEMessageHandlers[486] = Calendar_NextMonth;
    gFEMessageHandlers[487] = Calendar_GetTodayCell;
    gFEMessageHandlers[488] = Calendar_SetDriver;
    gFEMessageHandlers[496] = Calendar_SelectCell;
    gFEMessageHandlers[497] = Calendar_GetPopupRow;
    gFEMessageHandlers[511] = GM_vFEMessage511_Empty;
    gFEMessageHandlers[519] = Calendar_Play;
    gFEMessageHandlers[549] = Calendar_IsSimulationNecessary;
    gFEMessageHandlers[521] = UIStatsRankings_GetRow;
    gFEMessageHandlers[522] = UIStatsRankings_GetProfileName;
    gFEMessageHandlers[523] = UIStatsRankings_SetActiveStat;
    gFEMessageHandlers[524] = UIStatsRankings_GetIndStatsRow;
    gFEMessageHandlers[525] = UIStatsRankings_GetIndStatsNumRows;
    gFEMessageHandlers[526] = UIStatsRankings_GetNumRows;
    gFEMessageHandlers[489] = GM_vSelectLogo;
    gFEMessageHandlers[490] = GM_vSetLogoEditorShape;
    gFEMessageHandlers[491] = GM_vGetLogoPaletteColor;
    gFEMessageHandlers[492] = GM_vLoadLogoFromTexture;
    gFEMessageHandlers[493] = GM_vMarkLogoChanged;
    gFEMessageHandlers[494] = GM_vSetLogoPixel;
    gFEMessageHandlers[502] = GM_vIsAnyPadHoldingA;
    gFEMessageHandlers[513] = GM_vGetLogoName;
    gFEMessageHandlers[514] = GM_vSetLogoName;
    gFEMessageHandlers[583] = GM_vGetLogoPaletteEntry;
    gFEMessageHandlers[584] = GM_vGetLogoPixel;
    gFEMessageHandlers[518] = GM_vSetLongDriveTargetScore;
    gFEMessageHandlers[536] = FE_GetNextRealtimeEventInfo;
    gFEMessageHandlers[544] = FE_GetDateTimeIfClockEarly;
    gFEMessageHandlers[543] = GM_vCRAPCreatingLogo;
    gFEMessageHandlers[587] = GM_vSetLogoShape;
    gFEMessageHandlers[588] = GM_vGetLogoShape;
    gFEMessageHandlers[598] = GM_vFEMessage598_Empty;
    gFEMessageHandlers[565] = GM_vNextActiveProfileSlot;
    gFEMessageHandlers[567] = GM_vMCGetUserNames;
    gFEMessageHandlers[568] = FE_Sqrt;
    gFEMessageHandlers[569] = LadderMenu_SetNodePos;
    gFEMessageHandlers[573] = LadderMenu_GetNodePos;
    gFEMessageHandlers[575] = LadderMenu_GetEventText;
    gFEMessageHandlers[576] = LadderMenu_MoveCursor;
    gFEMessageHandlers[579] = LadderMenu_GetNodeEventN0;
    gFEMessageHandlers[580] = LadderMenu_GetFirstAndCursorNodePos;
    gFEMessageHandlers[592] = GM_vGbaStartLink;
    gFEMessageHandlers[593] = GM_vGbaTakeCash;
    gFEMessageHandlers[594] = GM_vGbaSwapStats;
    gFEMessageHandlers[595] = GM_vFEMessage595_Empty;
    gFEMessageHandlers[596] = LadderMenu_GetAngleBetween;
    gFEMessageHandlers[599] = GM_vGetGreenGridOption;
    gFEMessageHandlers[600] = GM_vSetGreenGridOption;
    gFEMessageHandlers[605] = TrophyRoom_GetTourTrophy;
    gFEMessageHandlers[606] = TrophyRoom_GetTourTrophyText;
    gFEMessageHandlers[608] = GM_vGetProfileProgress;
    gFEMessageHandlers[609] = GM_vFEMessage609_Empty;
    gFEMessageHandlers[610] = GM_vIsBonusTrophyBallWon;
    gFEMessageHandlers[611] = GM_vGbaCancelLink;
    gFEMessageHandlers[612] = GM_vGbaGetLinkState;
    gFEMessageHandlers[623] = GM_vGbaReadCashAndStats;
    gFEMessageHandlers[624] = GM_vGbaAddCashToMove;
    gFEMessageHandlers[630] = GM_vFEMessage630_Return0;
    gFEMessageHandlers[629] = GM_vGbaGrantUnlocks;
    gFEMessageHandlers[633] = GM_vGbaIsReadPending;
    gFEMessageHandlers[653] = GM_vGbaGetGbaCash;
    gFEMessageHandlers[654] = GM_vGbaIsLinkFailed;
    gFEMessageHandlers[748] = GM_vGbaResumeLink;
    gFEMessageHandlers[618] = GM_vFEMessage618_Empty;
    gFEMessageHandlers[619] = GM_vFEMessage619_Empty;
    gFEMessageHandlers[607] = TrophyRoom_CountEventsInMonth;
    gFEMessageHandlers[613] = TrophyRoom_GetPlaceholderText;
    gFEMessageHandlers[614] = GM_vEASBioListProducts;
    gFEMessageHandlers[615] = GM_vEASBioListAccomplishments;
    gFEMessageHandlers[616] = GM_vEASBioShowSummary;
    gFEMessageHandlers[617] = GM_vEASBioShowProductDetails;
    gFEMessageHandlers[620] = GM_vGetPar5EagleDate;
    gFEMessageHandlers[621] = GM_vIsProfileLogoMade;
    gFEMessageHandlers[622] = GM_vSaveLogo;
    gFEMessageHandlers[627] = GM_vEASBioLoad;
    gFEMessageHandlers[628] = GM_vEASBioSave;
    gFEMessageHandlers[632] = GM_vMCHasSLUS20572Save;
    gFEMessageHandlers[637] = TrophyRoom_GetIndexMod4;
    gFEMessageHandlers[638] = TrophyRoom_GetMedalDate;
    gFEMessageHandlers[640] = TrophyRoom_GetLadderAward;
    gFEMessageHandlers[641] = TrophyRoom_GetLadderEventCourse;
    gFEMessageHandlers[643] = GM_vEASBioIsOnCard;
    gFEMessageHandlers[644] = GM_vEASBioMemoryRequired;
    gFEMessageHandlers[645] = GM_vEASBioLoadProducts;
    gFEMessageHandlers[646] = GM_vEASBioIsLoaded;
    gFEMessageHandlers[647] = GM_vEASBioIsNotWrongFile;
    gFEMessageHandlers[648] = GM_vEASBioIsReplacingOldest;
    gFEMessageHandlers[649] = GM_vEASBioIsBadData;
    gFEMessageHandlers[650] = GM_vEASBioDelete;
    gFEMessageHandlers[655] = GM_vFEMessage655_Return0;
    gFEMessageHandlers[657] = GM_vSetCrAPTriggerAnims;
    gFEMessageHandlers[658] = GM_vEASBioCheckReward;
    gFEMessageHandlers[663] = GM_vRestartCrAPAnim;
    gFEMessageHandlers[664] = GM_vSetCrAPRenderStateForSubcategory;
    gFEMessageHandlers[665] = GM_vSetCrAPClub;
    gFEMessageHandlers[666] = GM_vEASBioGetLevelProgress;
    gFEMessageHandlers[667] = GM_vFEMessage667_Empty;
    gFEMessageHandlers[668] = GM_vEASBioUnloadProducts;
    gFEMessageHandlers[671] = GM_vMCSetCurrentFileType;
    gFEMessageHandlers[672] = GM_vMCCallActionFn;
    gFEMessageHandlers[670] = TrophyRoom_GetRTEAwardIcon;
    gFEMessageHandlers[674] = TrophyRoom_GetAwardEarnedText;
    gFEMessageHandlers[675] = GM_vFEMessage675_Empty;
    gFEMessageHandlers[678] = GM_vPlayNowGetNumGroups;
    gFEMessageHandlers[679] = GM_vPlayNowSelectGroup;
    gFEMessageHandlers[680] = GM_vPlayNowGetGroupName;
    gFEMessageHandlers[681] = GM_vPlayNowGetGroupDescription;
    gFEMessageHandlers[682] = GM_vPlayNowGetGroupMedal;
    gFEMessageHandlers[683] = PGASponsor_GetItemBonus;
    gFEMessageHandlers[684] = GM_vSetCourseFindDisc;
    gFEMessageHandlers[685] = GM_vClearGolferCache;
    gFEMessageHandlers[688] = GM_vEASBioGetLevel;
    gFEMessageHandlers[689] = GM_vRandomizeCrAPGolfer;
    gFEMessageHandlers[690] = GM_vRandomizeCrAPOutfit;
    gFEMessageHandlers[686] = GM_vMCIsWrongDevice;
    gFEMessageHandlers[691] = LadderMenu_GetNodeState;
    gFEMessageHandlers[692] = GM_vSetLongDriveOptions;
    gFEMessageHandlers[693] = Calendar_IsSherwoodTargetEntered;
    gFEMessageHandlers[694] = Calendar_SetPlayNowFlag;
    gFEMessageHandlers[695] = Calendar_GetPlayNowFlag;
    gFEMessageHandlers[696] = GM_vTrophyRoomGetStatus;
    gFEMessageHandlers[697] = PGASponsor_PickStartingSponsor;
    gFEMessageHandlers[698] = PGASponsor_CollectItems;
    gFEMessageHandlers[699] = PGASponsor_GetItem;
    gFEMessageHandlers[700] = PGASponsor_GetName;
    gFEMessageHandlers[701] = PGATourWins_GetDetails;
    gFEMessageHandlers[704] = GM_vStopMusic;
    gFEMessageHandlers[708] = Calendar_GetRTEDescription;
    gFEMessageHandlers[711] = GM_vSellCrAPItem;
    gFEMessageHandlers[712] = GM_vGetCrAPCategoryCounts;
    gFEMessageHandlers[713] = GM_vGetNumCrAPSaleItemsOwned;
    gFEMessageHandlers[714] = GM_vRandomizeCrAPBody;
    gFEMessageHandlers[715] = GM_vFEMessage715_Empty;
    gFEMessageHandlers[718] = PGATourMsg_CheckAdvanceTournament;
    gFEMessageHandlers[720] = LadderMenu_GetEventName;
    gFEMessageHandlers[721] = GM_vGetPuttingTipOption;
    gFEMessageHandlers[722] = GM_vSetPuttingTipOption;
    gFEMessageHandlers[723] = GM_vBonusTrophyBallsWon;
    gFEMessageHandlers[727] = GM_vControlMusic;
    gFEMessageHandlers[725] = GM_vClearQueuedCrAPAnim;
    gFEMessageHandlers[726] = GM_vDefaultBlankUserName;
    gFEMessageHandlers[728] = GM_vGetEquippedCrAPItemInSubcategory;
    gFEMessageHandlers[730] = GM_vIsCrAPAnimInGolferLib;
    gFEMessageHandlers[736] = GM_vMCIsUserOnCard;
    gFEMessageHandlers[737] = GM_vShortenString12;
    gFEMessageHandlers[739] = GM_vGetCrAPItemSlot;
    gFEMessageHandlers[740] = GM_vRestoreAfterPreview;
    gFEMessageHandlers[741] = GM_vFEMessage741_Empty;
    gFEMessageHandlers[746] = GM_vShortenString32;
    gFEMessageHandlers[749] = PGATourMsg_DidUserQuit;
    gFEMessageHandlers[751] = GM_vEASBioGetLastError;
    gFEMessageHandlers[754] = PGASponsor_GetSlot;
    gFEMessageHandlers[755] = PGASponsor_GetTotalItemBonus;
    gFEMessageHandlers[756] = GM_vIsLeapYear;
    gFEMessageHandlers[757] = GM_vIsCrAPItemRemovable;
    gFEMessageHandlers[760] = GM_vPreviewItem;
    gFEMessageHandlers[759] = GM_vEASBioCheck;
    gFEMessageHandlers[761] = GM_vMCGetBlocksNeeded;
    gFEMessageHandlers[762] = GM_vFEMessage762_Empty;
    gFEMessageHandlers[763] = GM_vFEMessage763_Empty;
    gFEMessageHandlers[764] = GM_vMCGetLastSavedUser;
    gFEMessageHandlers[765] = GM_vFEMessage765_Empty;
    gFEMessageHandlers[768] = GM_vEASBioCreate;
    gFEMessageHandlers[769] = GM_vEASBioGetRewardMessage;
}

// Front-end message 2: the fewest players game mode pArgs[0] takes (GM_GetMinPlayersForMode).
// Messages 4 and 6 run copies of it.
void GM_vGetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetMinPlayersForMode((u8)pArgs[0].i);
}

// Front-end message 4: a copy of GM_vGetMinPlayersForMode (message 2).
void GM_vMessage4_GetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetMinPlayersForMode((u8)pArgs[0].i);
}

// Front-end message 6: a copy of GM_vGetMinPlayersForMode (message 2).
void GM_vMessage6_GetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetMinPlayersForMode((u8)pArgs[0].i);
}

// Front-end message 5: game mode pArgs[0] (GM_SetModeType sets up its rules), then split screen as
// that mode has it (GM_SetSplitScreenForMode).
void GM_vSetGameMode(MsgArg* pArgs, MsgArg* pResult) {
    GM_SetModeType((u8)pArgs[0].i);
    GM_SetSplitScreenForMode();
}

// Front-end message 7: pArgs[0] players (Session_SetNumPlayers), then split screen as the game mode
// has it (GM_SetSplitScreenForMode).
void GM_vSetNumPlayers(MsgArg* pArgs, MsgArg* pResult) {
    Session_SetNumPlayers((u8)pArgs[0].i);
    GM_SetSplitScreenForMode();
}

// Front-end message 8: empty in this build.
void GM_vFEMessage8_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 3: sets up the session's players for the game the menus start (FE_vExitUI: CPU
// players, loaded profiles, golfers and bags). DiscCheck.c also calls it directly, with no values.
void GM_vSetupPlayers(MsgArg* pArgs, MsgArg* pResult) {
    FE_vExitUI();
}

// Front-end message 9: the menus start the demo (gSession.bDemo) in game mode pArgs[0]:
// gFEState.b11 set, the mode set up (GM_SetModeType), the fade to black started and the front
// end's audio stopped (Gaud_ExitFE).
void GM_vStartDemo(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.b11 = 1;
    GM_SetModeType((u8)pArgs[0].i);
    gSession.bDemo = 1;
    gUIState.bFadeToBlack = 1;
    Gaud_ExitFE();
}

// Front-end message 10: empty in this build.
void GM_vFEMessage10_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 12: the round's course is pArgs[0] (GM_SetCurrentCourse).
void GM_vSetCourse(MsgArg* pArgs, MsgArg* pResult) {
    GM_SetCurrentCourse((u8)pArgs[0].i);
}

// Front-end message 13: the round is the one hole pArgs[0] (1..18): every hole unselected
// (GM_SelectHoleSet(0)), then that one selected and made current (GM_SelectSingleHole).
void GM_vSelectSingleHole(MsgArg* pArgs, MsgArg* pResult) {
    GM_SelectHoleSet(0);
    GM_SelectSingleHole((u8)pArgs[0].i - 1);
}

// Front-end message 14: the round's holes by preset pArgs[0] (GM_SelectHoleSet: 1 all 18, 2 the
// front nine, 3 the back nine, 4/5/6 the par 5s/4s/3s...).
void GM_vSetHoleSet(MsgArg* pArgs, MsgArg* pResult) {
    GM_SelectHoleSet((u8)pArgs[0].i);
}

// Front-end message 15: attribute pArgs[1] of golfer pArgs[0] (GolferRecord.attr; a created
// golfer's from the current profile, FE_spGetGolfer).
void GM_vGetGolferAttribute(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord = FE_spGetGolfer(pArgs[0].i);

    pResult->i = pRecord->attr[pArgs[1].i];
}

// Front-end message 16: golfer pArgs[0]'s full name (GetGolferName) into the string pArgs[1].
void GM_vGetGolferName(MsgArg* pArgs, MsgArg* pResult) {
    GetGolferName(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// Golfer nGolfer's full name into szName: "First "Nick" Last" when the golfer's record has a
// nickname (longer than one character and not "NA"), else "First Last". Golfer 18 never shows a
// nickname. No buffer size (TW07's takes one).
void GetGolferName(int nGolfer, char* szName) {
    int bNick;
    GolferRecord* pRecord;

    pRecord = FE_spGetGolfer(nGolfer);
    bNick = strcmp(pRecord->szNick, "NA") != 0 && strlen(pRecord->szNick) > 1 && nGolfer != 18;
    if (bNick != 0) {
        sprintf(szName, "%s \"%s\" %s", pRecord->szFirst, pRecord->szNick, pRecord->szLast);
    } else {
        sprintf(szName, "%s %s", pRecord->szFirst, pRecord->szLast);
    }
}

// Front-end message 17: empty in this build.
void GM_vFEMessage17_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 18: a custom round (gpGame->bCustomRound) of four holes: course 8's hole 17,
// course 5's hole 4, course 17's hole 2 and course 18's hole 9, the rest unselected. With both demo
// session flags (0x4000 and 0x8000) set instead: commentary off (options.a0[4] 0) and all 18 slots
// course 0's hole 18.
void GM_vInitCustomRound(MsgArg* pArgs, MsgArg* pResult) {
    s32 i;

    gpGame->nHoleCourse[0] = 8;
    gpGame->nHoleNum[0] = 16;
    gpGame->nHoleCourse[1] = 5;
    gpGame->nHoleNum[1] = 3;
    gpGame->nHoleCourse[2] = 17;
    gpGame->nHoleNum[2] = 1;
    gpGame->nHoleCourse[3] = 18;
    gpGame->nHoleNum[3] = 8;
    for (i = 0; i < 18; i++) {
        gpGame->bHoleSelected[i] = 0;
    }
    gpGame->bHoleSelected[0] = 1;
    gpGame->bHoleSelected[1] = 1;
    gpGame->bHoleSelected[2] = 1;
    gpGame->bHoleSelected[3] = 1;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        gSession.options.a0[4] = 0;
        for (i = 0; i < 18; i++) {
            gpGame->nHoleCourse[i] = 0;
            gpGame->nHoleNum[i] = 17;
            gpGame->bHoleSelected[i] = 1;
        }
    }
    gpGame->bCustomRound = 1;
}

// Front-end message 19: 1 when session flag 0x4000 (the demo set-up) is set.
void GM_vGetDemoSetupFlag(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (gSession.uFlags >> 14) & 1;
}

// Front-end message 20: player pArgs[0] plays golfer pArgs[1] (Session_SetGolfer). In Play Now
// (game mode 5) it first sets gpFEProfile->b11703, which keeps FE_vExitUI from giving player 0
// the created golfer.
void GM_vSetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 5) {
        gpFEProfile->b11703 = 1;
    }
    Session_SetGolfer(pArgs[1].i, pArgs[0].i);
}

// Front-end message 21: the session's number of players.
void GM_vGetNumPlayers(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nNumPlayers;
}

// Front-end message 22: player pArgs[0] uses controller pArgs[1]. -1 and 9 give 9 (CONTROLLER_CPU);
// a real controller is also marked in gUIState.a2C.
void GM_vSetPlayerController(MsgArg* pArgs, MsgArg* pResult) {
    s32 nController;

    nController = pArgs[1].i;
    if (nController == -1 || nController == 9) {
        gSession.nController[pArgs[0].i] = 9;
        return;
    }
    gSession.nController[pArgs[0].i] = nController;
    gUIState.a2C[pArgs[1].i] = 1;
}

// Front-end message 23: the golfer player pArgs[0] plays.
void GM_vGetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nGolfer[pArgs[0].i];
}

// Front-end message 24: the session's game mode (Game_GetMode).
void GM_vGetGameMode(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_GetMode();
}

// Front-end message 25: always answers 1 in this build.
void GM_vFEMessage25_Return1(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

// Front-end message 26: hides the menu golfer (pArgs[0] nonzero) or shows him again
// (gpCrAPState->bHidden). A change while the CrAP screen shows him (screen kind 3) calls
// FE_OnGolferHiddenChanged.
void GM_vHideCharacter(MsgArg* pArgs, MsgArg* pResult) {
    u8 bOld;

    bOld = gpCrAPState->bHidden;
    gpCrAPState->bHidden = pArgs[0].i;
    if (bOld != gpCrAPState->bHidden && gpCrAPState->nScreenKind == 3) {
        FE_OnGolferHiddenChanged();
    }
}

// Front-end message 27: the menu screen showing the golfer is now pArgs[0]
// (gpCrAPState->nScreenKind: 0 golfers in turn, 3 the CrAP screen, 4 none). Going to 0 from another
// kind, or to 3 from another kind, clears the golfer cache (FE_vClearGolferCache); the golfer is
// drawn through the offscreen buffer on kind 0 only (FE_SetOffscreenBufferRender).
void GM_vSetCharState(MsgArg* pArgs, MsgArg* pResult) {
    s32 nOld;

    nOld = gpCrAPState->nScreenKind;
    gpCrAPState->nScreenKind = pArgs[0].i;
    if (nOld != 0 && gpCrAPState->nScreenKind == 0) {
        FE_vClearGolferCache();
    }
    if (gpCrAPState->nScreenKind == 3 && nOld != 3) {
        FE_vClearGolferCache();
    }
    if (gpCrAPState->nScreenKind == 0) {
        FE_SetOffscreenBufferRender(1);
    } else {
        FE_SetOffscreenBufferRender(0);
    }
}

// Front-end message 28: golfer pArgs[0]'s last name into the string pResult (a created golfer's
// from the current profile, FE_spGetGolfer).
void GM_vGetGolferLastName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pResult->p)->pStr, FE_spGetGolfer(pArgs[0].i)->szLast);
}

// Front-end message 29: profile name pArgs[3] (0..3) of the save on the memory card in port
// pArgs[1], slot pArgs[2] (MCCardState.aszName, from MC_GetMC) into the string pArgs[0].
void GM_vMCGetUserName(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[1].i, pArgs[2].i);
    strcpy(((MsgString*)pArgs[0].p)->pStr, state.aszName[pArgs[3].i]);
}

// Front-end message 567: the four profile names of the save on the memory card in port pArgs[0],
// slot pArgs[1] (MCCardState.aszName, from MC_GetMC) into the strings pArgs[2] to pArgs[5].
void GM_vMCGetUserNames(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    strcpy(((MsgString*)pArgs[2].p)->pStr, state.aszName[0]);
    strcpy(((MsgString*)pArgs[3].p)->pStr, state.aszName[1]);
    strcpy(((MsgString*)pArgs[4].p)->pStr, state.aszName[2]);
    strcpy(((MsgString*)pArgs[5].p)->pStr, state.aszName[3]);
}

// Front-end message 30: 1 when a memory card is in port pArgs[0], slot pArgs[1] (MC_CARD_PRESENT in
// its MCCardState, from MC_GetMC).
void GM_vMCIsCardPresent(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags & MC_CARD_PRESENT) >> 1;
}

// Front-end message 31: empty in this build.
void GM_vFEMessage31_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 32: whether controller pArgs[0]'s buttons reach the menu UI (pArgs[1] nonzero:
// yes), through gUIState.a30 (UI_SetControllerEnabled), which uiProcessInterface.c's
// UI_ReadControllers tests.
void GM_vSetControllerInputEnabled(MsgArg* pArgs, MsgArg* pResult) {
    UI_SetControllerEnabled(pArgs[0].i, (u8)pArgs[1].i);
}

// Front-end message 33: starts the memory card screens' work (MC_Connect: both ports reset and
// looked at once).
void GM_vMCConnect(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
}

// Front-end message 34: ends the memory card work (MC_Disconnect, empty on the GameCube).
void GM_vMCDisconnect(MsgArg* pArgs, MsgArg* pResult) {
    MC_Disconnect();
}

// Front-end message 35: loads the profile named by the string pArgs[3] from the save on the memory
// card in port pArgs[0], slot pArgs[1] into profile slot pArgs[2] (MC_LoadUser). Answers 1, or
// MC_LoadUser's error. Then the slot's profile is backed up (FE_BackupProfileClaimRow) and marked loaded
// (gFEState.aLoaded).
// EA bug: the answer is never 0, so the backup and the loaded mark also happen when the load
// failed.
void GM_vMCLoadUser(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPosStr pos;
    s32 nError;
    s32 n;

    pos.pos.nPort = pArgs[0].i;
    pos.pos.nSlot = pArgs[1].i;
    pos.pos.n8 = pArgs[2].i;
    pos.szC = ((MsgString*)pArgs[3].p)->pStr;
    nError = MC_LoadUser(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
    if (pResult->i != 0) {
        FE_BackupProfileClaimRow(pArgs[2].i);
        gFEState.aLoaded[pArgs[2].i] = 1;
    }
}

// Front-end message 36: saves profile slot pArgs[2] to the memory card in port pArgs[0], slot
// pArgs[1] (MC_SaveUser). Answers 1, or MC_SaveUser's error.
void GM_vMCSaveUser(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nError;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pos.n8 = pArgs[2].i;
    nError = MC_SaveUser(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Front-end message 37: plays menu sound pArgs[0] (Gaud_PlayUISound); sound 11 stands for a random
// one of sounds 11 to 18.
void GM_vPlayMenuSound(MsgArg* pArgs, MsgArg* pResult) {
    int n;

    n = pArgs[0].i;
    if (n == 11) {
        Gaud_PlayUISound((Misc_RandFunc(0) & 7) + 11);
    } else {
        Gaud_PlayUISound(n);
    }
}

// Front-end message 38: the weather option (options.nWeather) from the menu's choice pArgs[0]: 1
// gives 2 (a pick kept for several holes), 2 gives 3, 3 gives 0 (clear); other values change
// nothing.
void GM_vSetWeatherOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.nWeather = 2;
        return;
    case 2:
        gSession.options.nWeather = 3;
        return;
    case 3:
        gSession.options.nWeather = 0;
        return;
    }
}

// Front-end message 40: formats the memory card in port pArgs[0], slot pArgs[1] (MC_FormatCard).
// Answers 1, or MC_FormatCard's error.
void GM_vMCFormatCard(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = MC_FormatCard(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Front-end message 41: every player's tee set (all five gSession.nTeeSet) from the menu's choice
// pArgs[1]: 1 gives tee set 2, 2 gives 1, 3 gives 0; other values change nothing. pArgs[0] is not
// read.
void GM_vSetTeeSet(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 5; i++) {
        switch (pArgs[1].i) {
        case 1:
            gSession.nTeeSet[i] = 2;
            break;
        case 2:
            gSession.nTeeSet[i] = 1;
            break;
        case 3:
            gSession.nTeeSet[i] = 0;
            break;
        }
    }
}

// Front-end message 42: 1 when save profile pArgs[0] holds a created golfer
// (createdGolfer.bAvailable).
void GM_vProfileHasCreatedGolfer(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)gpSaveData[pArgs[0].i].createdGolfer.bAvailable != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Front-end message 39: 30, the golfer number of the first created golfer (FIRST_CREATED_GOLFER;
// 0..29 are the table golfers).
void GM_vGetFirstCreatedGolfer(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 30;
}

// Front-end message 43: always answers 1 in this build.
void GM_vFEMessage43_Return1(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

// Front-end message 44: the golfer number of created golfer pArgs[0]: pArgs[0] + 30
// (FIRST_CREATED_GOLFER).
void GM_vGetCreatedGolferIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = pArgs[0].i + 30;
}

// Front-end message 45: empty in this build.
void GM_vFEMessage45_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 47: golfer pArgs[0]'s bAvailable (a created golfer's from the current profile,
// FE_spGetGolfer).
void GM_vIsGolferAvailable(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (s8)FE_spGetGolfer(pArgs[0].i)->bAvailable;
}

// Front-end message 48: whether controller pArgs[0] is plugged in (gUIState.a1); 9
// (CONTROLLER_CPU) always answers 1.
void GM_vIsControllerPluggedIn(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    if (n == 9) {
        pResult->i = 1;
        return;
    }
    pResult->i = gUIState.a1[n];
}

// Front-end message 49: 1 when the memory card in port pArgs[0], slot pArgs[1] is formatted
// (MC_CARD_FORMATTED in its MCCardState).
void GM_vMCIsCardFormatted(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags & MC_CARD_FORMATTED) >> 3;
}

// Front-end message 686: 1 when the device in port pArgs[0], slot pArgs[1] is not a memory card
// (MC_CARD_WRONGDEVICE in its MCCardState).
void GM_vMCIsWrongDevice(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags & MC_CARD_WRONGDEVICE) >> 4;
}

// Front-end message 50: the free space on the memory card in port pArgs[0], slot pArgs[1], in
// blocks (MCCardState.nFreeBlocks).
void GM_vMCGetFreeBlocks(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.nFreeBlocks;
}

// Front-end message 51: the space the options save needs on the memory card in port pArgs[0], slot
// pArgs[1]: file type 0, the options (MC_SetCurrentFileType), and its memory-required operation
// (MC_CallActionFnMemoryRequired, which runs MC_MemoryRequiredForOptions), between MC_ConnectCard
// and MC_Disconnect.
void GM_vMCOptionsMemoryRequired(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_SetCurrentFileType(0);
    MC_ConnectCard(pos.nPort, pos.nSlot);
    pResult->i = MC_CallActionFnMemoryRequired(&pos);
    MC_Disconnect();
}

// Front-end message 326: empty in this build.
void GM_vFEMessage326_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 52: whether port pArgs[0] has a multitap (MC_IsMultitapPluggedIn).
void GM_vMCIsMultitapPluggedIn(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_IsMultitapPluggedIn(pArgs[0].i);
}

// Front-end message 53: asks for pArgs[0] to be handed back to the menu UI a little later:
// uiProcessInterface.c counts three UI updates (gUIDelayedHint.n0) and then sends it as hint 0x23.
// The menus' twin of GM_vIG_MCfunction.
void GM_vMCfunction(MsgArg* pArgs, MsgArg* pResult) {
    gUIDelayedHint.n4 = pArgs[0].i;
    gUIDelayedHint.n0 = 0;
}

// Front-end message 54: saves the options and records to the memory card in port pArgs[0], slot
// pArgs[1] (MC_SaveOptions). Answers 1, or MC_SaveOptions' error.
void GM_vMCSaveOptions(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nError;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    nError = MC_SaveOptions(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Front-end message 537: loads the options and records from the memory card in port pArgs[0], slot
// pArgs[1] (MC_LoadOptions). Answers 1, or MC_LoadOptions' error.
void GM_vMCLoadOptions(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nError;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    nError = MC_LoadOptions(&pos);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Front-end message 55: two on/off options, options.a7[1] from pArgs[0] and a7[2] from pArgs[1]:
// the menus send 1 for on and 2 for off; other values change nothing. GM_vGetOptionFlags1And2 reads
// them back; no other code reads them.
void GM_vSetOptionFlags1And2(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a7[1] = 1;
        break;
    case 2:
        gSession.options.a7[1] = 0;
        break;
    }
    switch (pArgs[1].i) {
    case 1:
        gSession.options.a7[2] = 1;
        return;
    case 2:
        gSession.options.a7[2] = 0;
        return;
    }
}

// Front-end message 56: the commentary volume (options.a0[4], 0..5) from the menu's choice
// pArgs[0]: 1 gives 5, 2..6 give 0..4; the mixer gets 0.2 times the level (Gaud_SetCommentLevel).
void GM_vSetCommentaryOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a0[4] = 5;
        break;
    case 2:
        gSession.options.a0[4] = 0;
        break;
    case 3:
        gSession.options.a0[4] = 1;
        break;
    case 4:
        gSession.options.a0[4] = 2;
        break;
    case 5:
        gSession.options.a0[4] = 3;
        break;
    case 6:
        gSession.options.a0[4] = 4;
        break;
    }
    Gaud_SetCommentLevel(0.2f * (s8)gSession.options.a0[4]);
}

// Front-end message 57: the gimmes option (options.bGimmes; EA's tap-ins) from the menu's choice
// pArgs[0]: 1 on, 2 off; other values change nothing. GM_vGetTapinsOption reads it back.
void GM_vSetTapinsOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bGimmes = 1;
        return;
    case 2:
        gSession.options.bGimmes = 0;
        return;
    }
}

// Front-end message 58: the two on/off options GM_vSetOptionFlags1And2 sets, options.a7[1] into the
// int pArgs[0] points to and a7[2] into pArgs[1]'s: 1 when on, 2 when off (another stored value
// leaves the int as it was).
void GM_vGetOptionFlags1And2(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a7[1]) {
    case 1:
        *(s32*)pArgs[0].p = 1;
        break;
    case 0:
        *(s32*)pArgs[0].p = 2;
        break;
    }
    switch (gSession.options.a7[2]) {
    case 1:
        *(s32*)pArgs[1].p = 1;
        return;
    case 0:
        *(s32*)pArgs[1].p = 2;
        return;
    }
}

// Front-end message 59: the commentary volume (options.a0[4]) as the menu's choice
// GM_vSetCommentaryOption takes: level 5 answers 1, levels 0..4 answer 2..6; any other level leaves
// pResult unset.
void GM_vGetCommentaryOption(MsgArg* pArgs, MsgArg* pResult) {
    switch ((s8)gSession.options.a0[4]) {
    case 0:
        pResult->i = 2;
        return;
    case 1:
        pResult->i = 3;
        return;
    case 2:
        pResult->i = 4;
        return;
    case 3:
        pResult->i = 5;
        return;
    case 4:
        pResult->i = 6;
        return;
    case 5:
        pResult->i = 1;
        return;
    }
}

// Front-end message 60: the gimmes option (options.bGimmes) as the menu's choice: 1 when on, 2 when
// off; any other value leaves pResult unset.
void GM_vGetTapinsOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bGimmes) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 61: the width of string pArgs[0] in font slot 4 (fn_80012C30,
// UFont_GetStringWidth), times 512, as a float.
void GM_vGetStringWidth(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

// Front-end message 62: the space a profile save (save kind 1, the MC_SaveUser set of lbl_8018C7D8)
// needs on the memory card in port pArgs[0], slot pArgs[1]: its memory-required operation
// (MC_CallActionFnMemoryRequired, which runs fn_800A270C: MC_BlocksNeededForSave kind 1), between
// MC_ConnectCard and MC_Disconnect. The twin of GM_vMCOptionsMemoryRequired.
void GM_vMCUserMemoryRequired(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_ConnectCard(pos.nPort, pos.nSlot);
    MC_SetCurrentFileType(1);
    pResult->i = MC_CallActionFnMemoryRequired(&pos);
    MC_Disconnect();
}

// Front-end message 325: empty in this build.
void GM_vFEMessage325_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 63: makes the first challenge of challenge group pArgs[0] the current one
// (PlayNow_GetGroupFirstChallenge, PlayNow_SelectChallenge; challenge 0 when the group has none).
void GM_vSelectFirstChallengeOfGroup(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_SelectChallenge(PlayNow_GetGroupFirstChallenge(pArgs[0].i));
}

// Front-end message 64: empty in this build.
void GM_vFEMessage64_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 65: golfer pArgs[0]'s nModelID (a created golfer's from the current profile,
// FE_spGetGolfer).
void GM_vGetGolferModelID(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_spGetGolfer(pArgs[0].i)->nModelID;
}

// Front-end message 66: the bonus a won ladder event pays (GameMode4_SetEventBonus: not 0 doubles
// the event's money).
void GM_vSetLadderEventBonus(MsgArg* pArgs, MsgArg* pResult) {
    GameMode4_SetEventBonus(pArgs[0].i);
}

// Front-end message 67: finds golfer pArgs[0]'s bio in the 'BIO ' table (gpFEBios; golfer 1
// uses golfer 0's) and answers its index; its six numbers (a34) go into the ints pArgs[1] to
// pArgs[6] point to and n68 into pArgs[7]'s. No bio with that id gives index 29 (FE_NUM_BIOS), and
// the numbers are then read from one entry past the table.
void GM_vFindGolferBio(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    s32 nId;

    nId = pArgs[0].i;
    if (nId == 1) {
        nId = 0;
    }
    for (i = 0; i < FE_NUM_BIOS; i++) {
        if (nId == gpFEBios[i].nId) break;
    }
    pResult->i = i;
    *(s32*)pArgs[1].p = gpFEBios[i].a34[0];
    *(s32*)pArgs[2].p = gpFEBios[i].a34[1];
    *(s32*)pArgs[3].p = gpFEBios[i].a34[2];
    *(s32*)pArgs[4].p = gpFEBios[i].a34[3];
    *(s32*)pArgs[5].p = gpFEBios[i].a34[4];
    *(s32*)pArgs[6].p = gpFEBios[i].a34[5];
    *(s32*)pArgs[7].p = gpFEBios[i].n68;
}

// Front-end message 68: bio pArgs[0]'s four texts (FEBio sz4, sz24, sz4C, sz70) into the strings
// pArgs[1] to pArgs[4], and the name of its course (lbl_80191990[nCourse]) into pArgs[5], "N/A"
// when nCourse is -1.
void GM_vGetBioTexts(MsgArg* pArgs, MsgArg* pResult) {
    s32 nBio = pArgs[0].i;

    strcpy(((MsgString*)pArgs[1].p)->pStr, gpFEBios[nBio].sz4);
    strcpy(((MsgString*)pArgs[2].p)->pStr, gpFEBios[nBio].sz24);
    strcpy(((MsgString*)pArgs[3].p)->pStr, gpFEBios[nBio].sz4C);
    strcpy(((MsgString*)pArgs[4].p)->pStr, gpFEBios[nBio].sz70);
    if (gpFEBios[nBio].nCourse == -1) {
        strcpy(((MsgString*)pArgs[5].p)->pStr, "N/A");
        return;
    }
    strcpy(((MsgString*)pArgs[5].p)->pStr, lbl_80191990[gpFEBios[nBio].nCourse]);
}

// Front-end message 293: bio pArgs[0]'s long text (FEBio sz98) split at its newlines into the
// strings pArgs[1] to pArgs[5]; lines it does not have are " " (two spaces).
void GM_vGetBioLines(MsgArg* pArgs, MsgArg* pResult) {
    char szText[sizeof(gpFEBios->sz98)];
    int i;
    char* pLine;

    strcpy(szText, gpFEBios[pArgs[0].i].sz98);
    pLine = strtok(szText, "\n");
    for (i = 1; i <= 5; i++) {
        if (pLine != NULL) {
            strcpy(((MsgString*)pArgs[i].p)->pStr, pLine);
            pLine = strtok(NULL, "\n");
        } else {
            strcpy(((MsgString*)pArgs[i].p)->pStr, "  ");
        }
    }
}

// Front-end message 69: gives the created golfer being edited model pArgs[1]: the current profile's
// createdGolfer.nModelID; the player of the profile slot being worked on (gpFEProfile->nSlot)
// plays that slot's created golfer (FIRST_CREATED_GOLFER + slot), whose gGolferTable record gets
// the model too. pArgs[0] is not read.
void GM_vSaveGolferModel(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nSlot = gpFEProfile->nSlot;

    pProfile->createdGolfer.nModelID = pArgs[1].i;
    // fake match: reads nSlot again rather than using the local
    gSession.nGolfer[nSlot] = (u8)(gpFEProfile->nSlot + FIRST_CREATED_GOLFER);
    gGolferTable[gSession.nGolfer[nSlot]].nModelID = pProfile->createdGolfer.nModelID;
}

// Front-end message 70: the second string of slot pArgs[0]'s profile, the one stored at szName + 10
// after the name's first ten bytes (the profile setup, SaveProfile_InitNew, leaves it empty), into the
// string pArgs[1]. What the menus keep there is not known.
void GM_vGetProfileSecondName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, &gpSaveData[pArgs[0].i].szName[10]);
}

// Front-end message 71: sets the second string of slot pArgs[0]'s profile (szName + 10, see
// GM_vGetProfileSecondName) to the string pArgs[1], less its trailing spaces. An empty string makes
// the loop test the byte before it (szName[9]).
void GM_vSetProfileSecondName(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    strcpy(&gpSaveData[pArgs[0].i].szName[10], ((MsgString*)pArgs[1].p)->pStr);
    i = strlen(&gpSaveData[pArgs[0].i].szName[10]) - 1;
    while (gpSaveData[pArgs[0].i].szName[10 + i] == ' ') {
        i--;
    }
    gpSaveData[pArgs[0].i].szName[10 + i + 1] = '\0';
}

// Front-end message 287: empty in this build.
void GM_vFEMessage287_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 299: empty in this build.
void GM_vFEMessage299_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 302: empty in this build.
void GM_vFEMessage302_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 292: empty in this build.
void GM_vFEMessage292_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 312: empty in this build.
void GM_vFEMessage312_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 303: while pArgs[0] is nonzero every golfer counts as unlocked
// (gpFEProfile->b11702, which GM_vIsGolferUnlocked tests; FE_Manager.c clears it).
void GM_vSetAllGolfersPickable(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        gpFEProfile->b11702 = 1;
    } else {
        gpFEProfile->b11702 = 0;
    }
}

// Front-end message 313: 1 while every golfer counts as unlocked (gpFEProfile->b11702, set by
// GM_vSetAllGolfersPickable), else 0.
void GM_vGetAllGolfersPickable(MsgArg* pArgs, MsgArg* pResult) {
    if (gpFEProfile->b11702 != 0) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

// Front-end message 314: empty in this build.
void GM_vFEMessage314_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 283: 1 when the second string of slot pArgs[0]'s profile (szName + 10, see
// GM_vGetProfileSecondName) has anything but spaces, else 0.
void GM_vHasProfileSecondName(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    int nLen = strlen(&gpSaveData[pArgs[0].i].szName[10]);

    pResult->i = 0;
    for (i = 0; i < nLen; i++) {
        if (gpSaveData[pArgs[0].i].szName[10 + i] != ' ') {
            pResult->i = 1;
            return;
        }
    }
}

// Front-end message 72: the current profile's money to spend (nCurrentCash).
void GM_vGetProfileCash(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_GetCurrentProfile()->nCurrentCash;
}

// Front-end message 73: sets the current profile up as a new one (SaveProfile_InitNew: cleared, named "User
// <slot>", the starting golfers, courses and money).
void GM_vResetProfile(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile_InitNew(FE_GetCurrentProfile());
}

// Front-end message 74: how many of the 25 ladder events player 0's profile has won
// (GameMode4_GetNumEventsWon).
void GM_vGetNumLadderEventsWon(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameMode4_GetNumEventsWon();
}

// Front-end message 218: empty in this build.
void GM_vFEMessage218_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 217: empty in this build.
void GM_vFEMessage217_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 216: empty in this build.
void GM_vFEMessage216_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 222: empty in this build.
void GM_vFEMessage222_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 301: empty in this build.
void GM_vFEMessage301_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 75: empty in this build.
void GM_vFEMessage75_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 76: empty in this build.
void GM_vFEMessage76_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 77: empty in this build.
void GM_vFEMessage77_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 78: whether golfer pArgs[1] can be picked: 1 when it is unlocked (in any of the
// five save profiles, loaded or not, or by a cheat code in lbl_80281DF4) or is a created golfer
// (FIRST_CREATED_GOLFER on), 0 when it is locked, -1 when its gGolferTable record's bAvailable is
// -1 (not in the game); always 1 while GM_vSetAllGolfersPickable's flag (gpFEProfile->b11702) is
// set. pArgs[0] is not read. It also sets fe_movies.c's lbl_80281374 to 0 (answer not 0) or 0.2
// (locked); no code reads that value. For a created golfer (30..33) the unlock tests read past
// aGolferUnlocked[30]; the answer is already 1 then.
void GM_vIsGolferUnlocked(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    if (gpFEProfile->b11702 != 0) {
        pResult->i = 1;
    } else if ((s8)gGolferTable[pArgs[1].i].bAvailable != -1) {
        pResult->i = 0;
        if (pArgs[1].i >= FIRST_CREATED_GOLFER) {
            pResult->i = 1;
        }
        for (i = 0; i < 5; i++) {
            if (gpSaveData[i].aGolferUnlocked[pArgs[1].i] != 0) {
                pResult->i = 1;
            }
        }
        if (lbl_80281DF4->aGolferUnlocked[pArgs[1].i] != 0) {
            pResult->i = 1;
        }
    } else {
        pResult->i = -1;
    }
    if (pResult->i != 0) {
        lbl_80281374 = 0.0f;
    } else {
        lbl_80281374 = 0.2f;
    }
}

// Front-end message 79: whether course pArgs[1] can be picked: 1 when a loaded save profile
// (bActive) or a cheat code (lbl_80281DF4) has unlocked it, and always for course 23, else 0.
// pArgs[0] is not read. For course 23 the tests read aCourseUnlocked[23], one past its 23 entries;
// the answer is already 1 then.
void GM_vIsCourseUnlockedOnAnyProfile(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    pResult->i = 0;
    if (pArgs[1].i == 23) {
        pResult->i = 1;
    }
    for (i = 0; i < 5; i++) {
        if (gpSaveData[i].aCourseUnlocked[pArgs[1].i] != 0 && gpSaveData[i].bActive != 0) {
            pResult->i = 1;
        }
    }
    if (lbl_80281DF4->aCourseUnlocked[pArgs[1].i] != 0) {
        pResult->i = 1;
    }
}

// Front-end message 80: empty in this build.
void GM_vFEMessage80_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 322: empty in this build.
void GM_vFEMessage322_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 81: the golfer number of created golfer pArgs[0] (pArgs[0] + 30,
// FIRST_CREATED_GOLFER) when pArgs[1] is 0; golfer 0 when pArgs[1] is not 0.
void GM_vGetCreatedGolferIndexOr0(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        pResult->i = 0;
    } else {
        pResult->i = pArgs[0].i + 30;
    }
}

// Front-end message 82: looks value pArgs[0] up in the earnings table's ranges
// (gEarningsTable.aRange, loaded from the 'ERN ' stream): the first row with n0 - 1 <= value <= n4
// answers its n8; -1 when no row holds it. What the ranges stand for is not known.
void GM_vLookUpEarningsRange(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    int n;
    int nValue;

    nValue = pArgs[0].i;
    n = -1;
    for (i = 0; i < NUM_EARNINGS_RANGES; i++) {
        if (nValue >= gEarningsTable.aRange[i].n0 - 1 && nValue <= gEarningsTable.aRange[i].n4) {
            n = gEarningsTable.aRange[i].n8;
            break;
        }
    }
    pResult->i = n;
}

// Front-end message 83: sets the current profile's money to spend (nCurrentCash) to pArgs[1].
// pArgs[0] is not read.
void GM_vSetProfileCash(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;

    pProfile = FE_GetCurrentProfile();
    pProfile->nCurrentCash = pArgs[1].i;
}

// Front-end message 84: sets attribute pArgs[1] (GolferRecord.attr) of the current profile's
// created golfer to pArgs[2]. pArgs[0] is not read.
void GM_vSetCreatedGolferAttribute(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;
    int nAttr;

    nAttr = pArgs[1].i;
    pProfile = FE_GetCurrentProfile();
    pProfile->createdGolfer.attr[nAttr] = pArgs[2].i;
}

// Front-end message 85: queues a movie of kind 2 (FE_movieGetFreeEntry) and stops the music
// (Gaud_StopMusic). FE_movieFade plays only kinds 1 (the credits) and 3 (a bio), so kind 2 gives
// the fade to black, the golfers put away and set up again and the menu music restarted, with no
// movie.
void GM_vQueueMovieKind2(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = FE_movieGetFreeEntry();
    pMovie->nKind = 2;
    Gaud_StopMusic();
}

// Front-end message 86: shows golfer pArgs[0] (FE_setupStreaming, with pArgs[1] and pArgs[2] as its
// other two golfers), then picks a shirt for the player of the profile slot being worked on
// (gpFEProfile->nSlot), unless it is slot 0: of the four shirts (PlayerProfile.n0, 0..3;
// Character_SetClubsAndClothes dresses "shirt<n>"), the first that no player 0..nSlot (itself
// included) whose golfer has the same model wears. Then Session_SetupProfiles. Nothing changes when
// all four are taken.
void GM_vCharStream(MsgArg* pArgs, MsgArg* pResult) {
    u8 abFree[4] = {1, 1, 1, 1};
    int i;
    u32 n;
    GolferRecord* pMine;
    GolferRecord* pOther;

    FE_GetCurrentProfile();
    FE_setupStreaming(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    for (i = 0; i <= gpFEProfile->nSlot; i++) {
        pMine = FE_spGetGolfer(gSession.nGolfer[gpFEProfile->nSlot]);
        pOther = FE_spGetGolfer(gSession.nGolfer[i]);
        if (pMine->nModelID == pOther->nModelID) {
            abFree[gSession.aProfile[i].n0] = 0;
        }
    }
    for (n = 0; n < 4; n++) {
        if (abFree[n] && gpFEProfile->nSlot > 0) {
            gSession.aProfile[gpFEProfile->nSlot].n0 = n;
            break;
        }
    }
    Session_SetupProfiles();
}

// Front-end message 87: the split-screen setting (gSession.nSplitScreen: 0 one view, else split, 2
// side by side).
void GM_vSetSplitScreen(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nSplitScreen = pArgs[0].i;
}

// Front-end message 88: adds hole pArgs[0] (1..18) to the round's selected holes and moves to the
// round's first hole (GM_SelectSingleHole); unlike GM_vSelectSingleHole, the holes already selected
// stay.
void GM_vAddHoleToRound(MsgArg* pArgs, MsgArg* pResult) {
    GM_SelectSingleHole((u8)pArgs[0].i - 1);
}

// Front-end message 89: empty in this build.
void GM_vFEMessage89_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 90: the menus now work on profile slot pArgs[0] (gpFEProfile->nSlot,
// FE_GetCurrentProfile's slot). When the golfer shown is golfer 7 or 29, the models that wear a
// created golfer's look, it takes that profile's look (Character_ApplyCrAPSettings).
void GM_vSetCurrentProfileSlot(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->nSlot = pArgs[0].i;
    if (gpCrAPState->pB4->pChar != NULL &&
        (gpCrAPState->pB4->pChar->nGolferId == 7 || gpCrAPState->pB4->pChar->nGolferId == 29)) {
        Character_ApplyCrAPSettings(gpCrAPState->pB4->pChar, &FE_GetCurrentProfile()->choices);
    }
}

// Front-end message 91: the profile slot the menus work on (gpFEProfile->nSlot).
void GM_vGetCurrentProfileSlot(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->nSlot;
}

// Front-end message 565: moves the menus on to the next profile slot holding a loaded profile
// (bActive) and answers it; -1 when that runs past slot 3 or past the number of players.
// gpFEProfile->nSlot keeps the advanced value either way.
void GM_vNextActiveProfileSlot(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->nSlot++;
    while (gpFEProfile->nSlot < 4 && gpSaveData[gpFEProfile->nSlot].bActive == 0) {
        gpFEProfile->nSlot++;
    }
    if (gpFEProfile->nSlot >= 4 || gpFEProfile->nSlot + 1 > gSession.nNumPlayers) {
        pResult->i = -1;
        return;
    }
    pResult->i = gpFEProfile->nSlot;
}

// Front-end message 92: slot pArgs[0]'s profile name (szName) into the string pArgs[1].
void GM_vGetProfileName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[pArgs[0].i].szName);
}

// Front-end message 93: whether slot pArgs[0] holds a profile (bActive).
void GM_vIsProfileActive(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].bActive;
}

// Front-end message 94: empty in this build.
void GM_vFEMessage94_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 95: how many controllers are plugged in (gUIState.n38, counted every menu
// update by uiProcessInterface.c).
void GM_vGetNumControllersPluggedIn(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gUIState.n38;
}

// Front-end message 96: whether controller pArgs[0] has been given to a player (gUIState.a2C,
// set by GM_vSetPlayerController, cleared by GM_vClearControllerAssigned and when the menus start).
void GM_vIsControllerAssigned(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gUIState.a2C[pArgs[0].i];
}

// Front-end message 97: clears the mark that controller pArgs[0] (0..3; others are ignored) has
// been given to a player (gUIState.a2C).
void GM_vClearControllerAssigned(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    if (n < 4) {
        gUIState.a2C[n] = 0;
    }
}

// Front-end message 98: empty in this build.
void GM_vFEMessage98_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 99: empty in this build.
void GM_vFEMessage99_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 100: empty in this build.
void GM_vFEMessage100_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 101: empty in this build.
void GM_vFEMessage101_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 102: empty in this build.
void GM_vFEMessage102_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 103: the full rounds slot pArgs[0]'s profile has played (nRounds). Messages 103
// to 117 give the stats screen a profile's statistics.
void GM_vGetProfileRounds(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nRounds;
}

// Front-end message 104: slot pArgs[0]'s profile's average strokes per stroke-play round
// (nStrokeRoundStrokes / nStrokeRounds), as a float; 0 before its first.
void GM_vGetProfileStrokeAverage(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nStrokeRounds != 0) {
        pResult->f = (f32)pProfile->nStrokeRoundStrokes / (f32)pProfile->nStrokeRounds;
        return;
    }
    pResult->f = 0.0f;
}

// Front-end message 105: slot pArgs[0]'s profile's putts per hole (nPutts / nPuttHoles; holes with
// 10 putts or more are not counted), as a float; 0 before its first.
void GM_vGetProfilePuttsPerHole(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nPuttHoles != 0) {
        pResult->f = (f32)pProfile->nPutts / (f32)pProfile->nPuttHoles;
        return;
    }
    pResult->f = 0.0f;
}

// Front-end message 106: slot pArgs[0]'s profile's average drive in yards (nDriveDistance /
// nDrives, cut to a whole number); 0 before its first.
void GM_vGetProfileAverageDrive(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nDrives != 0) {
        pResult->i = (f32)pProfile->nDriveDistance / (f32)pProfile->nDrives;
        return;
    }
    pResult->i = 0;
}

// Front-end message 107: the percentage of fairways slot pArgs[0]'s profile has hit (100 *
// nFairwaysHit / nFairways, cut to a whole number); 0 before its first.
void GM_vGetProfileFairwayPercent(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nFairways != 0) {
        pResult->i = 100.0f * (f32)pProfile->nFairwaysHit / (f32)pProfile->nFairways;
        return;
    }
    pResult->i = 0;
}

// Front-end message 108: the percentage of greens in regulation slot pArgs[0]'s profile has hit
// (100 * nGreensHit / nHoles, cut to a whole number); 0 before its first.
void GM_vGetProfileGreenPercent(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nHoles != 0) {
        pResult->i = 100.0f * ((f32)pProfile->nGreensHit / (f32)pProfile->nHoles);
        return;
    }
    pResult->i = 0;
}

// Front-end message 109: slot pArgs[0]'s profile's longest drive, in yards (nLongestDrive).
void GM_vGetProfileLongestDrive(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nLongestDrive;
}

// Front-end message 110: slot pArgs[0]'s profile's longest putt holed, in feet (nLongestPutt).
void GM_vGetProfileLongestPutt(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nLongestPutt;
}

void GM_vGetProfileHolesInOne(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nHolesInOne;
}

void GM_vGetProfileAlbatrosses(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nAlbatrosses;
}

void GM_vGetProfileEagles(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nEagles;
}

void GM_vGetProfileBirdies(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nBirdies;
}

void GM_vGetProfilePars(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nPars;
}

void GM_vGetProfileBogeys(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nBogeys;
}

// Front-end message 117: how many holes slot pArgs[0]'s profile has played 2 or more over par
// (nDoubleBogeys).
void GM_vGetProfileDoubleBogeys(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nDoubleBogeys;
}

// Front-end message 118: gives the player of the profile slot being worked on (gpFEProfile->nSlot)
// the next shirt (PlayerProfile.n0, 0..3, wrapping) that no player 0..nSlot (itself included) whose
// golfer has the same model wears, as GM_vCharStream does; it stops where it started when all four
// are taken. Then the golfer shown is dressed again (Character_RequestClothesUpdateFE).
void GM_vNextShirt(MsgArg* pArgs, MsgArg* pResult) {
    u8 abFree[4] = {1, 1, 1, 1};
    int i;
    s8 nStart;
    GolferRecord* pMine;
    GolferRecord* pOther;

    for (i = 0; i <= gpFEProfile->nSlot; i++) {
        pMine = FE_spGetGolfer(gSession.nGolfer[gpFEProfile->nSlot]);
        pOther = FE_spGetGolfer(gSession.nGolfer[i]);
        if (pMine->nModelID == pOther->nModelID) {
            abFree[gSession.aProfile[i].n0] = 0;
        }
    }
    nStart = gSession.aProfile[gpFEProfile->nSlot].n0;
    gSession.aProfile[gpFEProfile->nSlot].n0++;
    if (gSession.aProfile[gpFEProfile->nSlot].n0 > 3) {
        gSession.aProfile[gpFEProfile->nSlot].n0 = 0;
    }
    while (!abFree[gSession.aProfile[gpFEProfile->nSlot].n0]) {
        gSession.aProfile[gpFEProfile->nSlot].n0++;
        if (gSession.aProfile[gpFEProfile->nSlot].n0 > 3) {
            gSession.aProfile[gpFEProfile->nSlot].n0 = 0;
        }
        if (gSession.aProfile[gpFEProfile->nSlot].n0 == nStart) break;
    }
    Character_RequestClothesUpdateFE(gpCrAPState->pB4->nIndex);
}

// Front-end message 119: the pin position every hole uses (gSession.nPinSet, 0..3; -1 counts as 0).
void GM_vSetPinSet(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nPinSet = pArgs[0].i;
}

// Front-end message 120: selects row pArgs[0] of the button-mask table (lbl_80186AF0) that
// Controller_GetButtonMask reads (fn_800142A4 keeps it in lbl_80281C98, an s8). The table has only
// row 0 in this build.
void GM_vSetButtonConfig(MsgArg* pArgs, MsgArg* pResult) {
    fn_800142A4(pArgs[0].i);
}

// Front-end message 121: whether save profile pArgs[0] has won trophy ball pArgs[1] (its
// aAward[pArgs[1]].bWon; 0..38, Earnings.c GM_Earnings_AwardTrophyBall).
void GM_vIsTrophyBallWon(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aAward[pArgs[1].i].bWon;
}

// Front-end message 610: whether the current profile (FE_GetCurrentProfile) has won trophy ball
// pArgs[0] + 23: aAward[23..38], the ones GM_GetBonusProgress counts.
void GM_vIsBonusTrophyBallWon(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

    pResult->i = pProfile->aAward[pArgs[0].i + 23].bWon;
}

// Front-end message 122: entry pArgs[0] (0..11) of gEarningsTable.a9B4, twelve values of the 'ERN '
// earnings data that no other code reads.
void GM_vGetEarningsTableA9B4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gEarningsTable.a9B4[pArgs[0].i];
}

// Front-end message 123: the round's mulligan rule (gpGame->nMulligans: 0 none, 1 any number, 2 one
// per player per nine) is pArgs[0], but none in game mode 7 and any number in mode 9 (practice).
void GM_vSetMulligan(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 7) {
        gpGame->nMulligans = 0;
        return;
    }
    if (Game_GetMode() == 9) {
        gpGame->nMulligans = 1;
        return;
    }
    gpGame->nMulligans = pArgs[0].i;
}

// Front-end message 124: empty in this build.
void GM_vFEMessage124_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 125: whether club pArgs[1] is in the bag of the golfer player pArgs[0] plays
// (GolferRecord.uBagMask; a created golfer's from the current profile, FE_spGetGolfer): answers the
// club's bit (1 << pArgs[1]) when it is, 0 when not. The player's bag (gSession.uBag) is also set
// to the golfer's.
void GM_vGetGolferClubAvailable(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord = FE_spGetGolfer(gSession.nGolfer[pArgs[0].i]);
    s32 n = pArgs[1].i;

    n = pRecord->uBagMask & (1 << n);   // fake match: one local for the club and the result (register order)
    pResult->i = n;
    gSession.uBag[pArgs[0].i] = pRecord->uBagMask;
}

// Front-end message 126: empty in this build.
void GM_vFEMessage126_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 127: puts club pArgs[1] into player pArgs[0]'s bag (gSession.uBag) or takes it
// out, except in the demo set-up (session flag 0x4000). For a created golfer, or any golfer in game
// mode 4 (the ladder), the golfer's record (uBagMask) keeps the new bag.
void GM_vToggleClub(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord;
    s32 nClub;

    pRecord = FE_spGetGolfer(gSession.nGolfer[pArgs[0].i]);
    nClub = pArgs[1].i;
    if (!(gSession.uFlags & 0x4000)) {
        gSession.uBag[pArgs[0].i] ^= 1 << nClub;
        if (gSession.nGolfer[pArgs[0].i] >= FIRST_CREATED_GOLFER || Game_GetMode() == 4) {
            Game_GetMode();     // the original calls it again and ignores the result
            pRecord->uBagMask = gSession.uBag[pArgs[0].i];
        }
    }
}

// Front-end message 305: empty in this build.
void GM_vFEMessage305_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 128: the wind option (options.nWind) from the menu's choice pArgs[0]: 1..4 give
// 0..3 (calm to gusty); other values change nothing.
void GM_vSetWindOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.nWind = 0;
        return;
    case 2:
        gSession.options.nWind = 1;
        return;
    case 3:
        gSession.options.nWind = 2;
        return;
    case 4:
        gSession.options.nWind = 3;
        return;
    }
}

// Front-end message 129: the vibration option (options.a7[0]) from the menu's choice pArgs[0]: 1
// on, 2 off (fn_8002EBA4 stores it and sets all four pads' rumble); other values change nothing.
void GM_vSetVibration(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        fn_8002EBA4((u8*)&gSession.options, 1);
        gSession.options.a7[0] = 1;
        return;
    case 2:
        fn_8002EBA4((u8*)&gSession.options, 0);
        gSession.options.a7[0] = 0;
        return;
    }
}

// Record kind nKind (0 the round's strokes, 1 the longest drive, 2 the longest putt, 3 greens in
// regulation, 4 putts, 5 fairways hit, 6 eagles or better, 7 birdies or better: the kinds
// HighScoreRecords_CheckRecord keeps), place pArgs[1] (0..4): answers its value and copies its
// holder's name into the string pArgs[2]. pArgs[0] is the course (gSession.aCourseRecord); from
// NUM_COURSE_RECORDS on it means the all-time records (recA). Front-end messages 130..137 run it
// for kinds 0..7.
void FE_GetRecordEntry(int nKind, MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i < NUM_COURSE_RECORDS) {
        pResult->i = gSession.aCourseRecord[pArgs[0].i].aRecord[nKind][pArgs[1].i].nValue;
        strcpy(((MsgString*)pArgs[2].p)->pStr,
               gSession.aCourseRecord[pArgs[0].i].aRecord[nKind][pArgs[1].i].szName);
        return;
    }
    pResult->i = gSession.recA[nKind][pArgs[1].i].nValue;
    strcpy(((MsgString*)pArgs[2].p)->pStr, gSession.recA[nKind][pArgs[1].i].szName);
}

// Front-end message 130: a record of kind 0, the round's strokes (FE_GetRecordEntry).
void GM_vGetLowRoundRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(0, pArgs, pResult);
}

// Front-end message 131: a record of kind 1, the longest drive (FE_GetRecordEntry).
void GM_vGetLongestDriveRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(1, pArgs, pResult);
}

// Front-end message 132: a record of kind 2, the longest putt (FE_GetRecordEntry).
void GM_vGetLongestPuttRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(2, pArgs, pResult);
}

// Front-end message 133: a record of kind 3, the round's greens in regulation (FE_GetRecordEntry).
void GM_vGetGreensInRegRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(3, pArgs, pResult);
}

// Front-end message 134: a record of kind 4, the fewest putts in a round (FE_GetRecordEntry).
void GM_vGetFewestPuttsRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(4, pArgs, pResult);
}

// Front-end message 135: a record of kind 5, the round's fairways hit (FE_GetRecordEntry).
void GM_vGetFairwaysHitRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(5, pArgs, pResult);
}

// Front-end message 136: a record of kind 6, the round's eagles or better (FE_GetRecordEntry).
void GM_vGetEaglesRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(6, pArgs, pResult);
}

// Front-end message 137: a record of kind 7, the round's birdies or better (FE_GetRecordEntry).
void GM_vGetBirdiesRecord(MsgArg* pArgs, MsgArg* pResult) {
    FE_GetRecordEntry(7, pArgs, pResult);
}

// Front-end message 138: loads replay pArgs[2] from the memory card in port pArgs[0], slot pArgs[1]
// (MC_LoadReplay into gReplayData); answers 1 when it loaded, else 0. Then player 0 plays slot 0's
// created golfer (FIRST_CREATED_GOLFER) when that slot has a profile loaded, else the replay's
// golfer (golfer 0 when that was a created one), the replay's course is set, and gpFEProfile->b0
// is cleared (a card replay, not a profile's award replay: GM_vShowAwardReplay).
void GM_vMCLoadReplay(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pos.n8 = pArgs[2].i;
    pResult->i = MC_LoadReplay(&pos) == 0;
    if (pResult->i != 0) {
        if (gFEState.aLoaded[0] == 1) {
            Session_SetGolfer(FIRST_CREATED_GOLFER, 0);
        } else if (gReplayData.player.golfer.nIndex >= FIRST_CREATED_GOLFER) {
            Session_SetGolfer(0, 0);
        } else {
            Session_SetGolfer(gReplayData.player.golfer.nIndex, 0);
        }
        GM_SetCurrentCourse(gReplayData.nCourse);
        gpFEProfile->b0 = 0;
    }
}

// Front-end message 139: empty in this build.
void GM_vFEMessage139_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 140: empty in this build.
void GM_vFEMessage140_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 141: how many challenge groups in a row, from the first, save profile pArgs[0]
// has a medal in (aMedal not 3): the count stops at the first group without one (29 at most).
void GM_vCountMedalsInARow(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 29; i++, n++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] == 3) break;
    }
    pResult->i = n;
}

// Front-end message 142: empty in this build.
void GM_vFEMessage142_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 143: 1 when one of the four profile names on the memory card in port pArgs[0],
// slot pArgs[1] (MCCardState.aszName) is the string pArgs[2], else 0.
void GM_vMCIsUserNameOnCard(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;
    int i;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = 0;
    for (i = 0; i < 4; i++) {
        if (strcmp(state.aszName[i], ((MsgString*)pArgs[2].p)->pStr) == 0) {
            pResult->i = 1;
        }
    }
}

// Front-end message 144: how many profiles the save on the memory card in port pArgs[0], slot
// pArgs[1] holds (MC_GetNumUser).
void GM_vMCGetNumUsers(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pResult->i = MC_GetNumUser(&pos);
}

// Front-end message 145: saves profile pArgs[3] (gpSaveData) over the saved profile named pArgs[2]
// on the memory card in port pArgs[0], slot pArgs[1], with the options and the records
// (fn_800A1164). Answers 1, or fn_800A1164's error.
void GM_vMCOverwriteUser(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A1164(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr, pArgs[3].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Front-end message 146: always answers 1 in this build.
void GM_vFEMessage146_Return1(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

// Front-end message 147: empty in this build.
void GM_vFEMessage147_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 148: whether player slot pArgs[0] has a profile loaded (gFEState.aLoaded).
void GM_vIsProfileLoaded(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.aLoaded[pArgs[0].i];
}

// Front-end message 149: gFEState.b0F, set by the front end's setup (FE_vOpenONCE): while it is
// set, entering the menus plays the intro movie (GoEntry.c, which also passes it to
// Gaud_StartFEMusic, TW07's firstTime) and the front end's files load without the loading screen
// (fn_80014718).
void GM_vGetFirstTimeInFE(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.b0F;
}

// Front-end message 150: sets gFEState.b0F (GM_vGetFirstTimeInFE) to pArgs[0].
void GM_vSetFirstTimeInFE(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.b0F = pArgs[0].i;
}

// Front-end message 151: answers 60 in the demo set-up (session flag 0x4000), else 30. Nothing in
// the code says what the menus use the number for.
void GM_vFEMessage151_Return30Or60(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.uFlags & 0x4000) {
        pResult->i = 60;
        return;
    }
    pResult->i = 30;
}

// Front-end message 152: sets the menu golfer's b83 (gpCrAPState) to pArgs[0]: while it is set
// FEgolferanim.c dims him (his alpha at most gFEDimAlphaMax; on screen kind 1 all-zero lighting).
// FEgolferanim.c also clears it.
void GM_vSetGolferDimmed(MsgArg* pArgs, MsgArg* pResult) {
    gpCrAPState->b83 = pArgs[0].i;
}

// Front-end message 153: tests the code typed in, the first pArgs[1] characters of the string
// pArgs[0] (PasswordManager_TestPassword): 1 when it is a cheat code, whose unlocks are then set.
void GM_vTestPassword(MsgArg* pArgs, MsgArg* pResult) {
    char szCode[0x20];          // the size is unknown: the frame leaves 0x20 bytes for it

    strncpy(szCode, ((MsgString*)pArgs[0].p)->pStr, pArgs[1].i);
    szCode[pArgs[1].i] = '\0';
    pResult->i = PasswordManager_TestPassword(szCode);
}

// Front-end message 154: empty in this build.
void GM_vFEMessage154_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 155: empty in this build.
void GM_vFEMessage155_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 156: gFEState.b11, set when the menus start the demo (GM_vStartDemo) and
// cleared when they start a game (FE_vExitUI) or the front end is set up (FE_vOpenONCE).
void GM_vGetDemoStarting(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.b11;
}

// Front-end message 157: how many replays the save on the memory card in port pArgs[0], slot
// pArgs[1] holds (fn_800A09EC), 0 on an error. The round's GM_vIG_MCGetNumReplays runs it too.
void GM_vMCGetNumReplays(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 n;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    n = fn_800A09EC(&pos);
    if (n >= 0) {
        pResult->i = n;
        return;
    }
    pResult->i = 0;
}

// Front-end message 158: whether replay pArgs[2] is saved on the memory card in port pArgs[0], slot
// pArgs[1] (its bit in MCCardState.aReplayUsed).
void GM_vMCIsReplaySaved(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;
    u32 uBit = pArgs[2].i;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = BitArray_TestBit(state.aReplayUsed, uBit);
}

// Front-end message 159: the music volume (options.a0[1], 0..5) from the menu's choice pArgs[0]: 1
// gives 5, 2..6 give 0..4; the mixer gets 0.2 times the level (Gaud_SetMusicLevel).
void GM_vSetMusicVolumeOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a0[1] = 5;
        break;
    case 2:
        gSession.options.a0[1] = 0;
        break;
    case 3:
        gSession.options.a0[1] = 1;
        break;
    case 4:
        gSession.options.a0[1] = 2;
        break;
    case 5:
        gSession.options.a0[1] = 3;
        break;
    case 6:
        gSession.options.a0[1] = 4;
        break;
    }
    Gaud_SetMusicLevel(0.2f * (s8)gSession.options.a0[1]);
}

// Front-end message 160: the sound effects volume (options.a0[0], 0..5) from the menu's choice
// pArgs[0]: 1 gives 5, 2..6 give 0..4; the mixer gets 0.2 times the level (Gaud_SetSfxLevel).
void GM_vSetSFXOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a0[0] = 5;
        break;
    case 2:
        gSession.options.a0[0] = 0;
        break;
    case 3:
        gSession.options.a0[0] = 1;
        break;
    case 4:
        gSession.options.a0[0] = 2;
        break;
    case 5:
        gSession.options.a0[0] = 3;
        break;
    case 6:
        gSession.options.a0[0] = 4;
        break;
    }
    Gaud_SetSfxLevel(0.2f * (s8)gSession.options.a0[0]);
}

// Front-end message 161: the music volume (options.a0[1]) as the menu's choice,
// GM_vSetMusicVolumeOption's inverse: 5 gives 1, 0..4 give 2..6; another value leaves pResult
// unset.
void GM_vGetMusicVolumeOption(MsgArg* pArgs, MsgArg* pResult) {
    switch ((s8)gSession.options.a0[1]) {
    case 5:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    case 1:
        pResult->i = 3;
        return;
    case 2:
        pResult->i = 4;
        return;
    case 3:
        pResult->i = 5;
        return;
    case 4:
        pResult->i = 6;
        return;
    }
}

// Front-end message 162: the sound effects volume (options.a0[0]) as the menu's choice,
// GM_vSetSFXOption's inverse: 5 gives 1, 0..4 give 2..6; another value leaves pResult unset.
void GM_vGetSFXOption(MsgArg* pArgs, MsgArg* pResult) {
    switch ((s8)gSession.options.a0[0]) {
    case 5:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    case 1:
        pResult->i = 3;
        return;
    case 2:
        pResult->i = 4;
        return;
    case 3:
        pResult->i = 5;
        return;
    case 4:
        pResult->i = 6;
        return;
    }
}

// Front-end message 163: the weather option (options.nWeather) as the menu's choice,
// GM_vSetWeatherOption's inverse: 2 gives 1, 3 gives 2, 0 gives 3; 1 and 4 (which the menu never
// sets) leave pResult unset.
void GM_vGetWeatherOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.nWeather) {
    case 2:
        pResult->i = 1;
        return;
    case 3:
        pResult->i = 2;
        return;
    case 0:
        pResult->i = 3;
        return;
    }
}

// Front-end message 164: the wind option (options.nWind) as the menu's choice, GM_vSetWindOption's
// inverse: 0..3 (calm to gusty) give 1..4; 4 and up (no wind) leave pResult unset.
void GM_vGetWindOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.nWind) {
    case 0:
        pResult->i = 1;
        return;
    case 1:
        pResult->i = 2;
        return;
    case 2:
        pResult->i = 3;
        return;
    case 3:
        pResult->i = 4;
        return;
    }
}

// Front-end message 165: the vibration option (options.a7[0]) as the menu's choice,
// GM_vSetVibration's inverse: on gives 1, off gives 2.
void GM_vGetVibration(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a7[0]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 166: empty in this build.
void GM_vFEMessage166_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 286: the created golfer of the profile being worked on (gpFEProfile) gets
// model pArgs[1] (GolferRecord.nModelID); pArgs[0] is not read.
void GM_vSetEditedGolferModelID(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->profile.createdGolfer.nModelID = pArgs[1].i;
}

// Front-end message 167: copies the profile being worked on (gpFEProfile) into save slot pArgs[0],
// marks it active and the slot loaded, and backs the slot up (FE_BackupProfileClaimRow); in the demo set-up
// (session flag 0x4000) only into a slot with no profile loaded. A profile without a TOUR card
// (level 0) gets level 1 unless gFEState.b18 is set (front-end message 188 sets it).
void GM_vStoreProfileInSlot(MsgArg* pArgs, MsgArg* pResult) {
    s32 nSlot = pArgs[0].i;

    if (!(gSession.uFlags & 0x4000) || gFEState.aLoaded[nSlot] == 0) {
        Mem_cpy(&gpSaveData[nSlot], &gpFEProfile->profile, sizeof(SaveProfile));
        gpSaveData[nSlot].bActive = 1;
        if (gpSaveData[nSlot].nTourCardLevel == 0 && gFEState.b18 == 0) {
            gpSaveData[nSlot].nTourCardLevel = 1;
        }
        gFEState.aLoaded[nSlot] = 1;
        FE_BackupProfileClaimRow(nSlot);
    }
}

// Front-end message 168: save profile pArgs[0]'s money, into the values pArgs[1] and pArgs[2] point
// at: all the money it has won (nTotalCash) and its golfer's PGA TOUR career winnings
// (tour.aStats[PGA_USER_GOLFER]).
void GM_vGetProfileMoney(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = gpSaveData[pArgs[0].i].nTotalCash;
    *(u32*)pArgs[2].p = gpSaveData[pArgs[0].i].tour.aStats[PGA_USER_GOLFER].nCareerWinnings;
}

// Front-end message 169: empty in this build.
void GM_vFEMessage169_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 170: how many of trophy balls 0..22 (aAward, the ones GM_GetGameProgress
// counts) save profile pArgs[0] has won.
void GM_vTrophyBallsWon(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 23; i++) {
        if (gpSaveData[pArgs[0].i].aAward[i].bWon == 1) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 723: how many of the bonus trophy balls, aAward[23..38] (the ones
// GM_GetBonusProgress counts), save profile pArgs[0] has won; the loop runs on past them (the EA
// bug below).
void GM_vBonusTrophyBallsWon(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    // EA bug: counts 39 awards from award 23, so aAward[39..61] read the first saved replay's
    // bytes (aReplay[0]); the 16 real ones end at aAward[38].
    for (i = 23; i < 62; i++) {
        if (gpSaveData[pArgs[0].i].aAward[i].bWon == 1) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 171: how many all-time records save profile pArgs[0] holds
// (GM_vGetAllTimeRecordsHeld).
void GM_vGetProfileRecordsHeld(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_vGetAllTimeRecordsHeld(&gpSaveData[pArgs[0].i]);
}

// Front-end message 608: save profile pArgs[0]'s progress, into the values pArgs[1..7] point at:
// [1] ladder events won (aLadderAward), [2] PGA TOUR tournaments won (aC8), [3] the bonus progress
// (an f32, GM_GetBonusProgress), [4] the Create-A-Player unlock bits set (aAssetOwned, bits
// 0..2999), [6] real-time events won (aRTEAward) and [7] sponsorship slots signed (aSponsor).
// pArgs[5] is not used.
void GM_vGetProfileProgress(MsgArg* pArgs, MsgArg* pResult) {
    int  nProfile = pArgs[0].i;
    s32* pnLadder = pArgs[1].p;
    s32* pnTour = pArgs[2].p;
    s32* pnBits = pArgs[4].p;
    s32* pnRTE = pArgs[6].p;
    s32* pnLocks = pArgs[7].p;
    int  i;
    int  n;

    n = 0;
    for (i = 0; i < 25; i++) {
        if (gpSaveData[nProfile].aLadderAward[i].bWon) {
            n++;
        }
    }
    *pnLadder = n;

    n = 0;
    for (i = 0; i < 31; i++) {
        if (gpSaveData[nProfile].aC8[i].award.bWon) {
            n++;
        }
    }
    *pnTour = n;

    *(f32*)pArgs[3].p = GM_GetBonusProgress(&gpSaveData[nProfile]);

    n = 0;
    for (i = 0; i < 3000; i++) {
        if (BitArray_TestBit(gpSaveData[nProfile].aAssetOwned, i)) {
            n++;
        }
    }
    *pnBits = n;

    n = 0;
    for (i = 0; i < 75; i++) {
        if (gpSaveData[nProfile].aRTEAward[i].bWon) {
            n++;
        }
    }
    *pnRTE = n;

    n = 0;
    for (i = 0; i < 11; i++) {
        if (gpSaveData[nProfile].aSponsor[i].bSigned) {
            n++;
        }
    }
    *pnLocks = n;
}

// Front-end message 172: save profile pArgs[0]'s stats, into the values pArgs[1..9] point at: [1]
// the best round, [2] the longest drive (yards), [3] the longest putt (feet), [4] holes in one, [5]
// and [6] 0, [7] the game progress (an f32, GM_GetGameProgress), [8] the golfers unlocked (of 30)
// and [9] the courses unlocked. The courses counted are 0..20 but 4 and 7, with course 0 listed
// twice and one taken off again; one more is added when all 18 rewards are unlocked.
void GM_vGetProfileStats(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfers = 0;
    int nCourses = 0;
    int nRewards = 0;
    int nProfile = pArgs[0].i;
    int aCourses[20] = {0, 1, 2, 3, 0, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    int i;

    *(s32*)pArgs[1].p = gpSaveData[nProfile].nBestRound;
    *(s32*)pArgs[2].p = gpSaveData[nProfile].nLongestDrive;
    *(s32*)pArgs[3].p = gpSaveData[nProfile].nLongestPutt;
    *(s32*)pArgs[4].p = gpSaveData[nProfile].nHolesInOne;
    *(s32*)pArgs[5].p = 0;
    *(s32*)pArgs[6].p = 0;
    *(f32*)pArgs[7].p = GM_GetGameProgress(&gpSaveData[nProfile]);
    for (i = 0; i < 30; i++) {
        if (gpSaveData[nProfile].aGolferUnlocked[i]) {
            nGolfers++;
        }
    }
    *(s32*)pArgs[8].p = nGolfers;
    for (i = 0; i < 20; i++) {
        if (gpSaveData[nProfile].aCourseUnlocked[aCourses[i]]) {
            nCourses++;
        }
    }
    nCourses--;
    for (i = 0; i < 18; i++) {
        if (gpSaveData[nProfile].aRewardUnlocked[i]) {
            nRewards++;
        }
    }
    if (nRewards == 18) {
        nCourses++;
    }
    *(s32*)pArgs[9].p = nCourses;
}

// Front-end message 173: how many of par-5 holes 0..70 save profile pArgs[0] has eagled
// (UserInfo_GetPar5EagleStat's kind 0, a5004); holes 71..74 (a10578) are not counted.
void GM_vPar5Eagles(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 71; i++) {
        if (UserInfo_GetPar5EagleStat(&gpSaveData[pArgs[0].i], 0, i) == 1) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 174: how many of the 25 ladder events save profile pArgs[0] has won
// (aLadderAward).
void GM_vGetProfileNumLadderEventsWon(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 25; i++) {
        if (gpSaveData[pArgs[0].i].aLadderAward[i].bWon != 0) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 175: one per challenge group save profile pArgs[0] has any medal in (aMedal not
// 3), plus one when it has a TOUR card (nTourCardLevel 1 or more).
void GM_vCountMedalsAndTourCard(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    if (gpSaveData[pArgs[0].i].nTourCardLevel >= 1) {
        n = 1;
    }
    for (i = 0; i < 29; i++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] != 3) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 176: how many challenge groups save profile pArgs[0] has medal 2 in (aMedal: 0
// the best, 3 none).
void GM_vCountMedal2(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 29; i++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] == 2) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 177: how many challenge groups save profile pArgs[0] has medal 1 in (aMedal: 0
// the best, 3 none).
void GM_vCountMedal1(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 29; i++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] == 1) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 178: how many challenge groups save profile pArgs[0] has the best medal (0) in,
// plus one when it has a TOUR card (nTourCardLevel 1 or more).
void GM_vCountBestMedalsAndTourCard(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    if (gpSaveData[pArgs[0].i].nTourCardLevel >= 1) {
        n = 1;
    }
    for (i = 0; i < 29; i++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] == 0) {
            n++;
        }
    }
    pResult->i = n;
}

// Front-end message 179: save profile pArgs[0]'s TOUR card level (nTourCardLevel: 0 none, 1..6).
void GM_vGetTourCardLevel(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nTourCardLevel;
}

// Front-end message 180: plays the replay save profile pArgs[0] kept with trophy ball pArgs[1]
// (aReplay[0..4] for awards 0, 6, 9, 3 and 13; another award only sets the flags): gpFEProfile->b0
// is set (FE_vExitUI then records game mode 27 for the menus) and n1061C keeps the award;
// FE_PlayTrophyBallHighlight copies the replay into gReplayData and sets up game mode 10 with its golfer and
// course. SaveProfile.aReplay is Replay[5]: copying the struct member gives the original's copy
// order.
void GM_vShowAwardReplay(MsgArg* pArgs, MsgArg* pResult) {
    Replay replay0;
    Replay replay6;
    Replay replay9;
    Replay replay3;
    Replay replay13;

    gpFEProfile->b0 = 1;
    gpFEProfile->n1061C = pArgs[1].i;
    switch (pArgs[1].i) {
    case 0:
        replay0 = gpSaveData[pArgs[0].i].aReplay[0];
        FE_PlayTrophyBallHighlight(&replay0);
        break;
    case 6:
        replay6 = gpSaveData[pArgs[0].i].aReplay[1];
        FE_PlayTrophyBallHighlight(&replay6);
        break;
    case 9:
        replay9 = gpSaveData[pArgs[0].i].aReplay[2];
        FE_PlayTrophyBallHighlight(&replay9);
        break;
    case 3:
        replay3 = gpSaveData[pArgs[0].i].aReplay[3];
        FE_PlayTrophyBallHighlight(&replay3);
        break;
    case 13:
        replay13 = gpSaveData[pArgs[0].i].aReplay[4];
        FE_PlayTrophyBallHighlight(&replay13);
        break;
    }
}

// Front-end message 181: saves part of the profile being worked on (gpFEProfile) into save slot
// pArgs[0]: its name, its created golfer (createdGolfer and bytes 0x54C0..0x5500) and its looks,
// dates, assets and unlock bits (0x5500 up to tour); the slot keeps its own stats, awards and
// money. A slot with no profile loaded gets its money plus gFEState.n1C plus 25,000. The slot
// is marked active and loaded and backed up (FE_BackupProfileClaimRow), gets TOUR card level 1 if
// it has none, and its PGA TOUR seasons are cleared (GM_PgaTourSim_ClearAllSeasons). Its saved
// replays' golfer and the all-time records held under its old name take the new name; the course
// records were meant to as well (the EA bug below).
void GM_vSaveCreatedPlayerToSlot(MsgArg* pArgs, MsgArg* pResult) {
    char szOld[0x20];           // the size is unknown (0x20 gives the original's frame)
    int  nSlot;
    int  nMoney;
    int  k;
    int  j;
    int  i;

    nMoney = 0;
    nSlot = pArgs[0].i;
    if (!gFEState.aLoaded[nSlot]) {
        nMoney = gpSaveData[nSlot].nCurrentCash;
    }
    strcpy(szOld, gpSaveData[nSlot].szName);
    strcpy(gpSaveData[nSlot].szName, gpFEProfile->profile.szName);
    memcpy(&gpSaveData[nSlot].createdGolfer, &gpFEProfile->profile.createdGolfer, sizeof(GolferRecord));
    memcpy(gpSaveData[nSlot].unk54C0, gpFEProfile->profile.unk54C0, 0x5500 - 0x54C0);
    memcpy(&gpSaveData[nSlot].choices, &gpFEProfile->profile.choices, 0xB634 - 0x5500);
    if (!gFEState.aLoaded[nSlot]) {
        nMoney = gFEState.n1C + nMoney;
        gpSaveData[nSlot].nCurrentCash = nMoney + 25000;
    }
    gpSaveData[nSlot].bActive = 1;
    if (gpSaveData[nSlot].nTourCardLevel == 0) {
        gpSaveData[nSlot].nTourCardLevel = 1;
    }
    gFEState.aLoaded[nSlot] = 1;
    FE_BackupProfileClaimRow(nSlot);

    for (k = 0; k < 5; k++) {
        strcpy(gpSaveData[pArgs[0].i].aReplay[k].player.golfer.szLast, gpSaveData[nSlot].szName);
    }
    for (j = 0; j < 8; j++) {
        for (k = 0; k < 5; k++) {
            if (strcmp(gSession.recA[j][k].szName, szOld) == 0) {
                strcpy(gSession.recA[j][k].szName, gpSaveData[nSlot].szName);
            }
        }
    }
    // EA bug: k is still 5 here, so each check reads the entry after [j][4] (the next kind's
    // first; past the course's records for the last kind)
    for (i = 0; i < NUM_COURSE_RECORDS; i++) {
        for (j = 0; j < 8; j++) {
            if (strcmp(gSession.aCourseRecord[i].aRecord[j][k].szName, szOld) == 0) {
                strcpy(gSession.aCourseRecord[i].aRecord[j][k].szName, gpSaveData[nSlot].szName);
            }
        }
    }
    GM_PgaTourSim_ClearAllSeasons(&gpSaveData[nSlot].tour);
}

// Front-end message 182: gFEState.nMode, the game mode FE_vExitUI records as the menus start a
// game (the session's, or 4 a ladder event, 23 the PGA TOUR, 27 a trophy ball's replay, 28 the
// lessons with a TOUR card); -1 after the front end's setup.
void GM_vGetMenuGameMode(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.nMode;
}

// Front-end message 183: sets gFEState.nMode (GM_vGetMenuGameMode) to pArgs[0].
void GM_vSetMenuGameMode(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.nMode = pArgs[0].i;
}

// Front-end message 184: whether save profile pArgs[0] has changed (bChanged: a stat, an award or
// its money changed or a challenge started since a round was last set up).
void GM_vIsProfileChanged(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].bChanged;
}

// Front-end message 185: the most rewards (of the first 18, aRewardUnlocked) any of the five save
// profiles has unlocked, or the cheat codes' unlocks (lbl_80281DF4) have, if that is more.
void GM_vGetMostRewardsUnlocked(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    int j;
    int nCount;
    int nMax;

    nMax = 0;
    for (i = 0; i < 5; i++) {
        nCount = 0;
        for (j = 0; j < 18; j++) {
            if (gpSaveData[i].aRewardUnlocked[j] != 0) {
                nCount++;
            }
        }
        if (nCount > nMax) {
            nMax = nCount;
        }
    }
    nCount = 0;
    for (j = 0; j < 18; j++) {
        if (lbl_80281DF4->aRewardUnlocked[j] != 0) {
            nCount++;
        }
    }
    if (nCount > nMax) {
        nMax = nCount;
    }
    pResult->i = nMax;
}

// Front-end message 186: pArgs[0] printed into the string pArgs[1] with thousands commas, "12,345"
// (UI_GetMoneyString); the menus' twin of the round's GM_vFormatWithCommas.
void GM_vFEFormatWithCommas(MsgArg* pArgs, MsgArg* pResult) {
    UI_GetMoneyString(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// Front-end message 187: gFEState.b18: set by the front end's setup (FE_vOpenONCE), cleared by
// the "THEKITCHENSINK" cheat code and by front-end message 188. While it is clear, a profile stored
// without a TOUR card gets level 1 (GM_vStoreProfileInSlot).
void GM_vGetTourCardWithheld(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.b18;
}

// Front-end message 188: sets gFEState.b18 (GM_vGetTourCardWithheld) to pArgs[0]; clearing it
// gives save profile 0 TOUR card level 1 when it has none, and its backup (p658[0]) too when that
// has none.
void GM_vSetTourCardWithheld(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.b18 = pArgs[0].i;
    if (gFEState.b18 == 0 && gpSaveData[0].nTourCardLevel == 0) {
        if (gFEState.p658[0].nTourCardLevel < 1) {
            gFEState.p658[0].nTourCardLevel = 1;
        }
        gpSaveData[0].nTourCardLevel = 1;
    }
}

// Front-end message 189: whether the menu golfer is hidden (gpCrAPState->bHidden, which
// GM_vHideCharacter sets).
void GM_vIsCharacterHidden(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpCrAPState->bHidden;
}

// Front-end message 190: the controller player pArgs[0] uses (gSession.nController; 9 is
// CONTROLLER_CPU), as GM_vSetPlayerController sets it.
void GM_vGetPlayerController(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nController[pArgs[0].i];
}

// Front-end message 191: the money that unlocks course pArgs[0] (gEarningsTable.aCoursePrice: a
// profile's nTotalCash must reach it; 0 not for sale).
void GM_vGetCoursePrice(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gEarningsTable.aCoursePrice[pArgs[0].i].nPrice;
}

// Front-end message 192: names the profile being worked on (gpFEProfile) "USER<n>", n being slot
// pArgs[0] + 1 (SaveProfile_SetName: its name and its created golfer's last name), and stores it in that
// save slot: when the slot has no profile loaded, gFEState.n1C plus 25,000 is first added to
// its money; it is marked active, gets TOUR card level 1 if it has none, the slot is marked loaded
// and backed up (FE_BackupProfileClaimRow).
void GM_vSaveProfileWithDefaultName(MsgArg* pArgs, MsgArg* pResult) {
    s32 nSlot = pArgs[0].i;
    char szName[16];

    sprintf(szName, "USER%d", nSlot + 1);
    SaveProfile_SetName(&gpFEProfile->profile, szName);
    if (gFEState.aLoaded[nSlot] == 0) {
        gpFEProfile->profile.nCurrentCash += gFEState.n1C + 25000;
    }
    gpFEProfile->profile.bActive = 1;
    if (gpFEProfile->profile.nTourCardLevel == 0) {
        gpFEProfile->profile.nTourCardLevel = 1;
    }
    gFEState.aLoaded[nSlot] = 1;
    Mem_cpy(&gpSaveData[nSlot], &gpFEProfile->profile, sizeof(SaveProfile));
    FE_BackupProfileClaimRow(nSlot);
}

// Front-end message 193: the par of hole pArgs[1] (0-based) of course pArgs[0] (fn_800D2ABC). Below
// 0 it is the saved round being edited (save slot gpFEProfile->n3, round n4: that entry's course
// and hole); 22 and 24..29 are built rounds, whose holes come from other courses (fn_800D3118,
// fn_800D315C).
void GM_vGetHolePar(MsgArg* pArgs, MsgArg* pResult) {
    int nCourse;
    int nCourseArg = pArgs[0].i;

    if (nCourseArg <= -1) {
        nCourse = gpSaveData[gpFEProfile->n3].aSavedRound[gpFEProfile->n4].nCourse[pArgs[1].i];
        pResult->i = fn_800D2ABC(
            nCourse, gpSaveData[gpFEProfile->n3].aSavedRound[gpFEProfile->n4].nHoleNum[pArgs[1].i]);
        return;
    }
    if (nCourseArg == 22) {
        nCourse = fn_800D3118(22, pArgs[1].i);
        pResult->i = fn_800D2ABC(nCourse, fn_800D315C(22, pArgs[1].i) - 1);
        return;
    }
    if (nCourseArg >= 24 && nCourseArg < 30) {
        // the argument is read again here (the original reloads it after the call)
        nCourse = fn_800D3118(pArgs[0].i, pArgs[1].i);
        pResult->i = fn_800D2ABC(nCourse, fn_800D315C(pArgs[0].i, pArgs[1].i) - 1);
        return;
    }
    pResult->i = fn_800D2ABC(nCourseArg, pArgs[1].i);
}

// Front-end message 194: the name of the replay's course (lbl_80191990[gReplayData.nCourse]) into
// the string pArgs[0].
void GM_vGetReplayCourseName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, lbl_80191990[gReplayData.nCourse]);
}

// Front-end message 195: the saved replay's hole (gReplayData.nHole). Messages 194 and 196 give its
// course's name and its golfer's last name.
void GM_vGetReplayHole(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gReplayData.nHole;
}

// Front-end message 196: the last name of the saved replay's golfer (gReplayData.player) into the
// string pArgs[0].
void GM_vGetReplayGolferLastName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, gReplayData.player.golfer.szLast);
}

// Front-end message 197: the money golfer pArgs[0] is worth in the current game mode. Modes 0 and
// 1: the stroke-play prize for beating a golfer of his money rating (GM_GetGolferMoneyRating picks
// the aStrokePrize row); mode 2: that rating's skins row, its n10; mode 4 (the ladder): the current
// ladder event's prize (gLadderMap.nEvent), or the last event's once all 25 are won
// (GameMode4_GetNumEventsWon), whatever the golfer. Other modes leave pResult alone.
void GM_vGetGolferPrize(MsgArg* pArgs, MsgArg* pResult) {
    int nEvent = GameMode4_GetNumEventsWon();

    switch (Game_GetMode()) {
    case 4:
        if (nEvent >= 25) {
            pResult->i = gEarningsTable.aLadderPrize[24].nBase;
            return;
        }
        pResult->i = gEarningsTable.aLadderPrize[gLadderMap.nEvent].nBase;
        return;
    case 0:
    case 1:
        pResult->i = gEarningsTable.aStrokePrize[GM_GetGolferMoneyRating(pArgs[0].i)].nBase;
        return;
    case 2:
        pResult->i = gEarningsTable.aSkins[GM_GetGolferMoneyRating(pArgs[0].i)].n10;
        return;
    case 3:
        return;
    }
}

// Front-end message 198: empty in this build.
void GM_vFEMessage198_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 199: the best medal save slot pArgs[0]'s profile has won in challenge group
// pArgs[1] (SaveProfile.aMedal: 0 the best, 3 none).
void GM_vGetChallengeMedal(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aMedal[pArgs[1].i];
}

// Front-end message 200: the model id (GolferRecord.nModelID) of save slot pArgs[0]'s created
// golfer.
void GM_vGetCreatedGolferModelID(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].createdGolfer.nModelID;
}

// Front-end message 201: looks at the memory card in port pArgs[0], slot pArgs[1] once so its noted
// state is fresh (MC_ConnectCard), then ends the card work (MC_Disconnect, empty on the GameCube).
void GM_vMCCheckCard(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    MC_Disconnect();
}

// Front-end message 202: whether golfer pArgs[1] can be picked without any profile's unlocks: -1
// when the golfer is not available at all (gGolferTable's bAvailable is -1), else 1 when he is a
// created golfer, one of the 16 golfers of gStartUnlockedGolfers or unlocked by a cheat code (lbl_80281DF4),
// 0 when locked. lbl_80281374 is set to 0.2 for a locked golfer, else 0. Message 78 makes the same
// test with the save profiles' unlocks instead of the list (and answers 1 for any golfer while
// gpFEProfile->b11702 is set).
void GM_vIsGolferUnlockedByDefault(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    if ((s8)gGolferTable[pArgs[1].i].bAvailable != -1) {
        pResult->i = 0;
        if (pArgs[1].i >= FIRST_CREATED_GOLFER) {
            pResult->i = 1;
        }
        for (i = 0; i < 16; i++) {
            if (pArgs[1].i == gStartUnlockedGolfers[i]) {
                pResult->i = 1;
            }
        }
        if (lbl_80281DF4->aGolferUnlocked[pArgs[1].i] != 0) {
            pResult->i = 1;
        }
    } else {
        pResult->i = -1;
    }
    if (pResult->i != 0) {
        lbl_80281374 = 0.0f;
    } else {
        lbl_80281374 = 0.2f;
    }
}

// Front-end message 203: always answers 0 in this build.
void GM_vFEMessage203_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 204: whether save slot pArgs[0]'s profile has eagled hole pArgs[2] (1..18) of
// course pArgs[1] (GM_UserHasEagledHole; 0 when that hole is not a par 5).
void GM_vUserHasEagledHole(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_UserHasEagledHole(pArgs[0].i, pArgs[1].i, pArgs[2].i - 1);
}

// Front-end message 620: the date the profile being worked on (its slot, gpFEProfile->nSlot)
// eagled hole pArgs[1] (1..18) of course pArgs[0] (GM_GetPar5EagleDate), unpacked by FE_IntToDate
// into the words pArgs[2], pArgs[3] and pArgs[4] point at (month, day, year); all three 0 when it
// has not eagled that hole or it is not a par 5.
void GM_vGetPar5EagleDate(MsgArg* pArgs, MsgArg* pResult) {
    int nA = pArgs[0].i;
    int nB = pArgs[1].i - 1;
    int* pA = pArgs[2].p;
    int* pB = pArgs[3].p;
    int* pC = pArgs[4].p;

    if (GM_UserHasEagledHole(gpFEProfile->nSlot, nA, nB)) {
        FE_IntToDate(GM_GetPar5EagleDate(gpFEProfile->nSlot, nA, nB), pA, pB, pC);
        return;
    }
    *pA = 0;
    *pB = 0;
    *pC = 0;
}

// Front-end message 205: queues the credits movie (FE_movieGetFreeEntry, kind FE_MOVIE_CREDITS) and stops
// the music (Gaud_StopMusic).
void GM_vPlayCredits(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = FE_movieGetFreeEntry();
    pMovie->nKind = FE_MOVIE_CREDITS;
    Gaud_StopMusic();
}

// Front-end message 206: empty in this build.
void GM_vFEMessage206_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 207: 1 when save slot pArgs[0]'s profile or a cheat code (lbl_80281DF4) has
// unlocked course pArgs[1], else 0.
void GM_vIsCourseUnlocked(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    if (gpSaveData[pArgs[0].i].aCourseUnlocked[pArgs[1].i] != 0) {
        pResult->i = 1;
    }
    if (lbl_80281DF4->aCourseUnlocked[pArgs[1].i] != 0) {
        pResult->i = 1;
    }
}

// Front-end message 208: how far the rewards go for save slot pArgs[0]: the number (1..18) of the
// last reward its profile or a cheat code (lbl_80281DF4) has unlocked, 0 for none. Earlier rewards
// that are still locked are not counted.
void GM_vGetHighestRewardUnlocked(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    int n = 0;

    for (i = 0; i < 18; i++) {
        if (gpSaveData[pArgs[0].i].aRewardUnlocked[i] != 0 && i + 1 > n) {
            n = i + 1;
        }
    }
    for (i = 0; i < 18; i++) {
        if (lbl_80281DF4->aRewardUnlocked[i] != 0 && i + 1 > n) {
            n = i + 1;
        }
    }
    pResult->i = n;
}

// Front-end message 209: always answers 0 in this build.
void GM_vFEMessage209_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 210: 1 when save slot pArgs[0] holds a profile (bActive) whose custom round
// pArgs[1] (0..2) is in use (SavedRound.n0), else 0.
void GM_vIsCustomRoundUsed(MsgArg* pArgs, MsgArg* pResult) {
    int b = 0;

    if (gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 != 0 && gpSaveData[pArgs[0].i].bActive != 0) {
        b = 1;
    }
    pResult->i = b;
}

// Front-end message 211: the name of save slot pArgs[0]'s custom round pArgs[1] into the string
// pArgs[2].
void GM_vGetCustomRoundName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[2].p)->pStr, gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].szName);
}

// Front-end message 212: names save slot pArgs[0]'s custom round pArgs[1]: its 20 characters are
// blanked, then the first pArgs[3] characters of the string pArgs[2] are copied in (each place
// blanked again first) and a '\0' is put after them. pArgs[3] is not checked against the name's 20
// bytes.
void GM_vSetCustomRoundName(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 20; i++) {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].szName[i] = ' ';
    }
    for (i = 0; i < pArgs[3].i; i++) {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].szName[i] = ' ';
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].szName[i] = ((MsgString*)pArgs[2].p)->pStr[i];
    }
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].szName[i] = '\0';
}

// Front-end message 213: entry pArgs[2] (0..17) of save slot pArgs[0]'s custom round pArgs[1] is
// hole number pArgs[4] of course pArgs[3]. A course of -1 instead marks the whole round unused
// (SavedRound.n0 cleared) and leaves the entry's course as it was; the hole number is stored either
// way. GM_vGetCustomRoundHole reads an entry back.
void GM_vSetCustomRoundHole(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[3].i != -1) {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nCourse[pArgs[2].i] = pArgs[3].i;
    } else {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = 0;
    }
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nHoleNum[pArgs[2].i] = pArgs[4].i;
}

// Front-end message 214: sets byte n15 of save slot pArgs[0]'s custom round pArgs[1] to pArgs[2]
// (the profile setup sets it to 1; message 221, GM_vGetCustomRoundN15, reads it back; nothing else
// reads it).
void GM_vSetCustomRoundN15(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n15 = pArgs[2].i;
}

// Front-end message 215: the first character of the string pArgs[0] (0 for an empty string).
void GM_vGetFirstChar(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = ((MsgString*)pArgs[0].p)->pStr[0];
}

// Front-end message 219: turns the point (x, y) held in the floats pArgs[1] and pArgs[2] point at
// by pArgs[0] degrees about the origin (x' = x cos - y sin, y' = x sin + y cos, through
// LLMath_mat44fltMultiply) and writes it back.
void GM_vRotatePoint2D(MsgArg* pArgs, MsgArg* pResult) {
    Vec4 v;
    f32 mtx[4][4];              // EA bug: only the 2x2 rotation is set; the rest is left unset
                                // (z and w are 0, so it only matters if it holds a NaN)
    f32 fAngle = pArgs[0].f * PI / 180.0f;
    f32 fSin;
    f32 fCos;

    v.x = *(f32*)pArgs[1].p;
    v.y = *(f32*)pArgs[2].p;
    v.z = 0.0f;
    v.w = 0.0f;
    fSin = Math_Sin(fAngle);
    fCos = Math_Cos(fAngle);
    mtx[0][0] = fCos;
    mtx[0][1] = fSin;
    mtx[1][0] = -fSin;
    mtx[1][1] = fCos;
    LLMath_mat44fltMultiply(mtx, &v, &v);
    *(f32*)pArgs[1].p = v.x;
    *(f32*)pArgs[2].p = v.y;
}

// Front-end message 220: entry pArgs[2] (0..17) of save slot pArgs[0]'s custom round pArgs[1]: its
// course into the word pArgs[3] points at, its hole number (-1: none) into the word pArgs[4] points
// at. GM_vSetCustomRoundHole sets it.
void GM_vGetCustomRoundHole(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[3].p = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nCourse[pArgs[2].i];
    *(s32*)pArgs[4].p = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nHoleNum[pArgs[2].i];
}

// Front-end message 221: byte n15 of save slot pArgs[0]'s custom round pArgs[1], read signed
// (GM_vSetCustomRoundN15 sets it).
void GM_vGetCustomRoundN15(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n15;
}

// Front-end message 223: which custom round (0..2) of save slot gpFEProfile->n3 the menus are
// working on (gpFEProfile->n4; message 193 reads that round's pars). Message 224 sets it.
void GM_vGetCustomRoundIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->n4;
}

// Front-end message 224: the menus now work on custom round pArgs[0] (0..2) of save slot
// gpFEProfile->n3 (gpFEProfile->n4; GM_vGetCustomRoundIndex reads it).
void GM_vSetCustomRoundIndex(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->n4 = pArgs[0].i;
}

// Front-end message 225: byte n5 of the menus' working profile (gpFEProfile), read signed; message
// 226 sets it and no other code reads it.
void GM_vGetFEProfileN5(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->n5;
}

// Front-end message 226: sets byte n5 of the menus' working profile (gpFEProfile) to pArgs[0];
// only message 225 (GM_vGetFEProfileN5) reads it.
void GM_vSetFEProfileN5(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->n5 = pArgs[0].i;
}

// Front-end message 227: answers 0 in the three words pArgs[2], pArgs[3] and pArgs[4] point at
// (where message 620 puts a month, day and year), whatever it is asked.
void GM_vFEMessage227_Return0(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[2].p = 0;
    *(s32*)pArgs[3].p = 0;
    *(s32*)pArgs[4].p = 0;
}

// Front-end message 228: fills entry pArgs[2] of save slot pArgs[0]'s custom round pArgs[1] with a
// random hole: a random course from a list of 20 (course 0 twice, so twice as likely; no 4, 7, 21
// or 22) among those this profile or a cheat code (lbl_80281DF4) has unlocked, and a random hole
// number 0..17, both drawn again while the round already holds that course and hole (the entry
// being filled counts too).
void GM_vSetRandomCustomRoundHole(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    int nSlot = pArgs[0].i;
    int nRound = pArgs[1].i;
    int nEntry = pArgs[2].i;
    s32 aCourses[20] = {0, 1, 2, 3, 0, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    int nCourse = 0;
    int nUnlocked = 0;
    u32 nPick;
    u32 nFound;
    s8 nHoleNum;

    for (i = 0; i < 20; i++) {
        if (gpSaveData[nSlot].aCourseUnlocked[aCourses[i]] ||
            lbl_80281DF4->aCourseUnlocked[aCourses[i]]) {
            nUnlocked++;
        }
    }
retry:
    nPick = Misc_RandFunc(1) % nUnlocked + 1;
    nFound = 0;
    for (i = 0; i < 20; i++) {
        if ((gpSaveData[nSlot].aCourseUnlocked[aCourses[i]] ||
             lbl_80281DF4->aCourseUnlocked[aCourses[i]]) &&
            ++nFound == nPick) {
            nCourse = aCourses[i];
            break;
        }
    }
    nHoleNum = Misc_RandFunc(1) % 18;
    for (i = 0; i < 18; i++) {
        if (nCourse == gpSaveData[nSlot].aSavedRound[nRound].nCourse[i] &&
            nHoleNum == gpSaveData[nSlot].aSavedRound[nRound].nHoleNum[i]) {
            goto retry;  // fake match: the original jumps back to the draw (a do-while: 97.0%)
        }
    }
    gpSaveData[nSlot].aSavedRound[nRound].nCourse[nEntry] = nCourse;
    gpSaveData[nSlot].aSavedRound[nRound].nHoleNum[nEntry] = nHoleNum;
}

// Front-end message 229: player slot pArgs[0] is a CPU player (pArgs[1] nonzero) or not
// (gFEState.aCPU; the players' setup, FE_vExitUI, gives a CPU player CONTROLLER_CPU and no
// profile). Message 230 reads it back.
void GM_vSetPlayerIsCPU(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.aCPU[pArgs[0].i] = pArgs[1].i;
}

// Front-end message 230: whether player slot pArgs[0] is a CPU player (gFEState.aCPU;
// GM_vSetPlayerIsCPU sets it).
void GM_vGetPlayerIsCPU(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.aCPU[pArgs[0].i];
}

// Front-end message 231: game option n14 from the menu's choice pArgs[0]: 1, 2, 3 give 0, 1, 2;
// another choice changes nothing. Message 243 reads it back; no other code reads it
// (Options_SetDefaults sets it to 0).
void GM_vSetOptionN14(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.n14 = 0;
        return;
    case 2:
        gSession.options.n14 = 1;
        return;
    case 3:
        gSession.options.n14 = 2;
        return;
    }
}

// Front-end message 232: the green speed option (options.n18) from the menu's choice pArgs[0]: 1,
// 2, 3 give 0, 1, 2 (another choice keeps the old value), then applied at once (fn_80055C40 sets
// gGreenSpeedSetting, which the ball's roll reads). Message 244 reads it back.
void GM_vSetGreenSpeedOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.n18 = 0;
        break;
    case 2:
        gSession.options.n18 = 1;
        break;
    case 3:
        gSession.options.n18 = 2;
        break;
    }
    fn_80055C40(gSession.options.n18);
}

// Front-end message 375: the fairway speed option (options.n20) from the menu's choice pArgs[0]: 1,
// 2, 3 give 0, 1, 2 (another choice keeps the old value), then applied at once (fn_80055CAC sets
// gFairwaySetting, which the ball's roll reads). Message 374 reads it back.
void GM_vSetFairwaySpeedOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.n20 = 0;
        break;
    case 2:
        gSession.options.n20 = 1;
        break;
    case 3:
        gSession.options.n20 = 2;
        break;
    }
    fn_80055CAC(gSession.options.n20);
}

// Front-end message 600: the green grid option (options.bPuttingGrid: the grid on the green shows
// with the putter, GoGreenGrid.c) from the menu's choice pArgs[0]: 1 on, 2 off; another choice
// changes nothing. Message 599 reads it back.
void GM_vSetGreenGridOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bPuttingGrid = 1;
        return;
    case 2:
        gSession.options.bPuttingGrid = 0;
        return;
    }
}

// Front-end message 233: the caddie tips option (options.a24[0]; CTIP_ShowCaddieTip shows no tip
// while it is off) from the menu's choice pArgs[0]: 1 on, 2 off; another choice changes nothing.
// Message 245 reads it back.
void GM_vSetCaddieTipsOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[0] = 1;
        return;
    case 2:
        gSession.options.a24[0] = 0;
        return;
    }
}

// Front-end message 722: the putting tip option (options.a24[1]; the round's UI asks it with
// IG_vShow_Putting_Tip) from the menu's choice pArgs[0]: 1 on, 2 off; another choice changes
// nothing. Message 721 reads it back.
void GM_vSetPuttingTipOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[1] = 1;
        return;
    case 2:
        gSession.options.a24[1] = 0;
        return;
    }
}

// Front-end message 234: the break line option (options.a24[2]; GoBreakLine.c draws the putt's
// break line only while it is on) from the menu's choice pArgs[0]: 1 on, 2 off; another choice
// changes nothing. Message 246 reads it back.
void GM_vSetBreakLineOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[2] = 1;
        return;
    case 2:
        gSession.options.a24[2] = 0;
        return;
    }
}

// Front-end message 235: on/off game option a24[3] from the menu's choice pArgs[0]: 1 on, 2 off;
// another choice changes nothing. Only message 247 reads it back; the game never tests it.
void GM_vSetOnOffOption3(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[3] = 1;
        return;
    case 2:
        gSession.options.a24[3] = 0;
        return;
    }
}

// Front-end message 236: on/off game option a24[4] from the menu's choice pArgs[0]: 1 on, 2 off;
// another choice changes nothing. Message 248 reads it back, and the round's menu as option 6 of
// GM_vGetOption; the game itself never tests it.
void GM_vSetOnOffOption4(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[4] = 1;
        return;
    case 2:
        gSession.options.a24[4] = 0;
        return;
    }
}

// Front-end message 237: on/off game option a24[5] from the menu's choice pArgs[0]: 1 on, 2 off;
// another choice changes nothing. Only message 249 reads it back; the game never tests it.
void GM_vSetOnOffOption5(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[5] = 1;
        return;
    case 2:
        gSession.options.a24[5] = 0;
        return;
    }
}

// Front-end message 238: on/off game option a24[6] from the menu's choice pArgs[0]: 1 on, 2 off;
// another choice changes nothing. Message 250 reads it back, the round's menu as option 5 of
// GM_vGetOption; while it is off and the wind option is on (nWind 1 or more),
// CharacterState_UpdateSKAState plays clip group 16 for the golfer whose ball is on the tee.
void GM_vSetOnOffOption6(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[6] = 1;
        return;
    case 2:
        gSession.options.a24[6] = 0;
        return;
    }
}

// Front-end message 239: the swing aid option (options.a24[7]: Swing.c draws the club's trail on
// the backswing and downswing only while it is on) from the menu's choice pArgs[0]: 1 on, 2 off;
// another choice changes nothing. Message 251 reads it back.
void GM_vSetSwingAidOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[7] = 1;
        return;
    case 2:
        gSession.options.a24[7] = 0;
        return;
    }
}

// Front-end message 240: the power boost option (options.bBoostEnabled; SW_vCheckForSwingBoost does
// nothing while it is off) from the menu's choice pArgs[0]: 1 on, 2 off; another choice changes
// nothing. Message 252 reads it back.
void GM_vSetPowerBoostOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bBoostEnabled = 1;
        return;
    case 2:
        gSession.options.bBoostEnabled = 0;
        return;
    }
}

// Front-end message 241: the spin control option (options.bSpinEnabled; SW_vUpdateSpinControl does
// nothing while it is off) from the menu's choice pArgs[0]: 1 on, 2 off; another choice changes
// nothing. Message 253 reads it back.
void GM_vSetSpinControlOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bSpinEnabled = 1;
        return;
    case 2:
        gSession.options.bSpinEnabled = 0;
        return;
    }
}

// Front-end message 242: level option a0[2] (0..5, default 5) from the menu's choice pArgs[0]: 1
// gives 5, 2..6 give 0..4, as the commentary volume's (GM_vSetCommentaryOption); another choice
// changes nothing. Unlike the effects, music and commentary levels it goes to no mixer: only
// message 254 reads it back.
void GM_vSetOptionLevel2(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a0[2] = 5;
        return;
    case 2:
        gSession.options.a0[2] = 0;
        return;
    case 3:
        gSession.options.a0[2] = 1;
        return;
    case 4:
        gSession.options.a0[2] = 2;
        return;
    case 5:
        gSession.options.a0[2] = 3;
        return;
    case 6:
        gSession.options.a0[2] = 4;
        return;
    }
}

// Front-end message 243: game option n14 as the menu's choice: 0, 1, 2 answer 1, 2, 3 (another
// value leaves pResult alone). GM_vSetOptionN14 sets it.
void GM_vGetOptionN14(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.n14) {
    case 0:
        pResult->i = 1;
        return;
    case 1:
        pResult->i = 2;
        return;
    case 2:
        pResult->i = 3;
        return;
    }
}

// Front-end message 244: the green speed option (options.n18) as the menu's choice: 0, 1, 2 answer
// 1, 2, 3 (another value leaves pResult alone). GM_vSetGreenSpeedOption sets it.
void GM_vGetGreenSpeedOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.n18) {
    case 0:
        pResult->i = 1;
        return;
    case 1:
        pResult->i = 2;
        return;
    case 2:
        pResult->i = 3;
        return;
    }
}

// Front-end message 374: the fairway speed option (options.n20) as the menu's choice: 0, 1, 2
// answer 1, 2, 3 (another value leaves pResult alone). GM_vSetFairwaySpeedOption sets it.
void GM_vGetFairwaySpeedOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.n20) {
    case 0:
        pResult->i = 1;
        return;
    case 1:
        pResult->i = 2;
        return;
    case 2:
        pResult->i = 3;
        return;
    }
}

// Front-end message 599: the green grid option (options.bPuttingGrid) as the menu's choice: 1 on, 2
// off (another value leaves pResult alone). GM_vSetGreenGridOption sets it.
void GM_vGetGreenGridOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bPuttingGrid) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 245: the caddie tips option (options.a24[0]) as the menu's choice: 1 on, 2 off
// (another value leaves pResult alone). GM_vSetCaddieTipsOption sets it.
void GM_vGetCaddieTipsOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[0]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 721: the putting tip option (options.a24[1]) as the menu's choice: 1 on, 2 off
// (another value leaves pResult alone). GM_vSetPuttingTipOption sets it.
void GM_vGetPuttingTipOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[1]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 246: the break line option (options.a24[2]) as the menu's choice: 1 on, 2 off
// (another value leaves pResult alone). GM_vSetBreakLineOption sets it.
void GM_vGetBreakLineOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[2]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 247: on/off game option a24[3] as the menu's choice: 1 on, 2 off (another value
// leaves pResult alone). GM_vSetOnOffOption3 sets it.
void GM_vGetOnOffOption3(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[3]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 248: on/off game option a24[4] as the menu's choice: 1 on, 2 off (another value
// leaves pResult alone). GM_vSetOnOffOption4 sets it.
void GM_vGetOnOffOption4(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[4]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 249: on/off game option a24[5] as the menu's choice: 1 on, 2 off (another value
// leaves pResult alone). GM_vSetOnOffOption5 sets it.
void GM_vGetOnOffOption5(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[5]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 250: on/off game option a24[6] as the menu's choice: 1 on, 2 off (another value
// leaves pResult alone). GM_vSetOnOffOption6 sets it.
void GM_vGetOnOffOption6(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[6]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 251: the swing aid option (options.a24[7], the swing trail) as the menu's
// choice: 1 on, 2 off (another value leaves pResult alone). GM_vSetSwingAidOption sets it.
void GM_vGetSwingAidOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[7]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 252: the power boost option (options.bBoostEnabled) as the menu's choice: 1 on,
// 2 off (another value leaves pResult alone). GM_vSetPowerBoostOption sets it.
void GM_vGetPowerBoostOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bBoostEnabled) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 253: the spin control option (options.bSpinEnabled) as the menu's choice: 1 on,
// 2 off (another value leaves pResult alone). GM_vSetSpinControlOption sets it.
void GM_vGetSpinControlOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bSpinEnabled) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Front-end message 254: level option a0[2] as the menu's choice: level 5 answers 1, levels 0..4
// answer 2..6 (another value leaves pResult alone). GM_vSetOptionLevel2 sets it.
void GM_vGetOptionLevel2(MsgArg* pArgs, MsgArg* pResult) {
    switch ((s8)gSession.options.a0[2]) {
    case 5:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    case 1:
        pResult->i = 3;
        return;
    case 2:
        pResult->i = 4;
        return;
    case 3:
        pResult->i = 5;
        return;
    case 4:
        pResult->i = 6;
        return;
    }
}

// Front-end message 255: whether the round is a custom round (pArgs[0] nonzero;
// gpGame->bCustomRound), the one saved as custom round pArgs[2] (gpGame->nSaveCourse) of save slot
// pArgs[1] (gpGame->nSaveSlot). For a custom round its 18 holes are copied in
// (GM_SetupCustomHoleSelection) and the current hole set to the first selected one.
void GM_vSelectCustomRound(MsgArg* pArgs, MsgArg* pResult) {
    gpGame->bCustomRound = pArgs[0].i;
    gpGame->nSaveSlot = pArgs[1].i;
    gpGame->nSaveCourse = pArgs[2].i;
    if (gpGame->bCustomRound != 0) {
        GM_SetupCustomHoleSelection();
        GM_InitializeCurrentHoleToFirstSelected();
    }
}

// Front-end message 256: whether backup row pArgs[0] (gFEState.p658) holds a profile (bActive)
// that none of player slots 0..3 is using as its backup (aBackup); 0 when one is.
void GM_vIsBackupProfileUnused(MsgArg* pArgs, MsgArg* pResult) {
    u8 bUsed = 0;
    int i;

    for (i = 0; i < 4; i++) {
        if (gFEState.aBackup[i] == pArgs[0].i) {
            bUsed = 1;
        }
    }
    if (bUsed) {
        pResult->i = 0;
        return;
    }
    pResult->i = gFEState.p658[pArgs[0].i].bActive;
}

// Front-end message 257: the name of the profile in backup row pArgs[0] (gFEState.p658) into
// the string pArgs[1].
void GM_vGetBackupProfileName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, gFEState.p658[pArgs[0].i].szName);
}

// Front-end message 258: player slot pArgs[1] takes the profile in backup row pArgs[0]: the slot is
// marked loaded (gFEState.aLoaded), the two rows pArgs[0] and pArgs[1] are swapped when they
// differ (FE_SwapBackupRows) so the profile sits in the slot's own row, that row is copied into the
// slot's profile (gpSaveData) and becomes the slot's backup row (aBackup).
void GM_vLoadBackupProfile(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRow = pArgs[0].i;
    s32 nSlot = pArgs[1].i;

    gFEState.aLoaded[nSlot] = 1;
    if (nSlot != nRow) {
        FE_SwapBackupRows(nSlot, nRow);
    }
    Mem_cpy(&gpSaveData[nSlot], &gFEState.p658[nSlot], sizeof(SaveProfile));
    gFEState.aBackup[nSlot] = nSlot;
}

// Front-end message 259: backs up every player slot's profile that is active into the slot's own
// backup row (FE_BackupAllProfiles).
void GM_vBackupAllProfiles(MsgArg* pArgs, MsgArg* pResult) {
    FE_BackupAllProfiles();
}

// Front-end message 260: backs up player slot pArgs[0]'s profile (FE_BackupProfileClaimRow: into
// its backup row, giving the slot one first when it has none).
void GM_vBackupProfileClaimRow(MsgArg* pArgs, MsgArg* pResult) {
    FE_BackupProfileClaimRow(pArgs[0].i);
}

// Front-end message 261: the rough option (options.n1C) from the menu's choice pArgs[0]: 1, 2, 3
// give 0, 1, 2 (another choice keeps the old value), then applied at once (fn_80055CD0 sets
// gRoughSetting, which the ball's roll reads). Message 262 reads it back.
void GM_vSetRoughOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.n1C = 0;
        break;
    case 2:
        gSession.options.n1C = 1;
        break;
    case 3:
        gSession.options.n1C = 2;
        break;
    }
    fn_80055CD0(gSession.options.n1C);
}

// Front-end message 262: the rough option (options.n1C) as the menu's choice: 0, 1, 2 answer 1, 2,
// 3 (another value leaves pResult alone). GM_vSetRoughOption sets it.
void GM_vGetRoughOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.n1C) {
    case 0:
        pResult->i = 1;
        return;
    case 1:
        pResult->i = 2;
        return;
    case 2:
        pResult->i = 3;
        return;
    }
}

// Front-end message 263: empty in this build.
void GM_vFEMessage263_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 264: save slot pArgs[0]'s created golfer's ball type (nGolferBallType) from
// pArgs[4] and its glove variant (n54C2, which the session's PlayerProfile.n2 gets) from pArgs[5];
// a value below 0 keeps the old one. pArgs[1..3] are not read. Message 265 reads them back.
void GM_vSetCreatedGolferBallAndGlove(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[4].i >= 0) {
        gpSaveData[pArgs[0].i].nGolferBallType = pArgs[4].i;
    }
    if (pArgs[5].i >= 0) {
        gpSaveData[pArgs[0].i].n54C2 = pArgs[5].i;
    }
}

// Front-end message 265: save slot pArgs[0]'s created golfer's ball type (nGolferBallType) and
// glove variant (n54C2), read signed, into the words pArgs[4] and pArgs[5] point at; the words
// pArgs[1..3] point at get 0. GM_vSetCreatedGolferBallAndGlove sets them.
void GM_vGetCreatedGolferBallAndGlove(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = 0;
    *(s32*)pArgs[2].p = 0;
    *(s32*)pArgs[3].p = 0;
    *(s32*)pArgs[4].p = (s8)gpSaveData[pArgs[0].i].nGolferBallType;
    *(s32*)pArgs[5].p = (s8)gpSaveData[pArgs[0].i].n54C2;
}

// Front-end message 266: save slot pArgs[0]'s created golfer's level (1..4) in each attribute
// group, into the words pArgs[1..5] point at: 1 below 50, 2 from 50, 3 from 75, 4 from 100. Groups:
// power (4 only once the profile's tour card is at level 6, whatever the attribute), ball striking
// and approach (the lower of the two decides), putting, spin, recovery. Message 269 lists the
// levels newly reached.
void GM_vGetCreatedGolferAttributeLevels(MsgArg* pArgs, MsgArg* pResult) {
    s8 nPower = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_POWER];
    s8 nStriking = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_BALL_STRIKING];
    s8 nApproach = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_APPROACH];
    s8 nPutting = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_PUTTING];
    s8 nSpin = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_SPIN];
    s8 nRecovery = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_RECOVERY];

    *(s32*)pArgs[1].p = (gpSaveData[pArgs[0].i].nTourCardLevel == 6) ? 4
                      : (nPower >= 75)                                ? 3
                      : (nPower >= 50)                                ? 2
                                                                      : 1;
    *(s32*)pArgs[2].p = (nStriking >= 100 && nApproach >= 100) ? 4
                      : (nStriking >= 75 && nApproach >= 75)   ? 3
                      : (nStriking >= 50 && nApproach >= 50)   ? 2
                                                               : 1;
    *(s32*)pArgs[3].p = (nPutting >= 100) ? 4 : (nPutting >= 75) ? 3 : (nPutting >= 50) ? 2 : 1;
    *(s32*)pArgs[4].p = (nSpin >= 100) ? 4 : (nSpin >= 75) ? 3 : (nSpin >= 50) ? 2 : 1;
    *(s32*)pArgs[5].p = (nRecovery >= 100) ? 4 : (nRecovery >= 75) ? 3 : (nRecovery >= 50) ? 2 : 1;
}

// Front-end message 267: sets byte n1 of the menus' working profile (gpFEProfile) to pArgs[0] (-1
// when the working profile is set up, FE_Manager.c); only message 268 reads it.
void GM_vSetFEProfileN1(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->n1 = pArgs[0].i;
}

// Front-end message 268: byte n1 of the menus' working profile (gpFEProfile), read signed
// (GM_vSetFEProfileN1 sets it).
void GM_vGetFEProfileN1(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->n1;
}

// Front-end message 269: lists the attribute levels save slot pArgs[0]'s created golfer has reached
// that the six values pArgs[1..6] (floats: power, ball striking, approach, putting, spin, recovery)
// have not, as pairs (group, level) in gpFEProfile->a10621, counted in n10620. A level is 2, 3 or
// 4 for a saved attribute of 50, 75 or 100 whose value passed is under that mark. Groups as in
// message 266: 1 power (levels 2 and 3 only), 2 ball striking and approach (both reached, either
// value under), 3 putting, 4 spin, 5 recovery. Messages 270 and 271 read the list back, 272 empties
// it.
void GM_vBuildAttributeLevelUps(MsgArg* pArgs, MsgArg* pResult) {
    int n = 0;
    int i;
    f32 fPower = pArgs[1].f;
    f32 fStriking = pArgs[2].f;
    f32 fApproach = pArgs[3].f;
    f32 fPutting = pArgs[4].f;
    f32 fSpin = pArgs[5].f;
    f32 fRecovery = pArgs[6].f;
    s8 nPower = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_POWER];
    s8 nStriking = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_BALL_STRIKING];
    s8 nApproach = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_APPROACH];
    s8 nPutting = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_PUTTING];
    s8 nSpin = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_SPIN];
    s8 nRecovery = gpSaveData[pArgs[0].i].createdGolfer.attr[ATTR_RECOVERY];

    gpFEProfile->n10620 = 0;
    for (i = 0; i < 15; i++) {
        gpFEProfile->a10621[i][0] = -1;
        gpFEProfile->a10621[i][1] = -1;
    }
    if (fPower < 50.0f && nPower >= 50) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 1;
        gpFEProfile->a10621[n][1] = 2;
        n++;
    }
    if (fPower < 75.0f && nPower >= 75) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 1;
        gpFEProfile->a10621[n][1] = 3;
        n++;
    }
    if (nStriking >= 50 && nApproach >= 50 && (fStriking < 50.0f || fApproach < 50.0f)) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 2;
        gpFEProfile->a10621[n][1] = 2;
        n++;
    }
    if (nStriking >= 75 && nApproach >= 75 && (fStriking < 75.0f || fApproach < 75.0f)) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 2;
        gpFEProfile->a10621[n][1] = 3;
        n++;
    }
    if (nStriking >= 100 && nApproach >= 100 && (fStriking < 100.0f || fApproach < 100.0f)) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 2;
        gpFEProfile->a10621[n][1] = 4;
        n++;
    }
    if (fPutting < 50.0f && nPutting >= 50) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 3;
        gpFEProfile->a10621[n][1] = 2;
        n++;
    }
    if (fPutting < 75.0f && nPutting >= 75) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 3;
        gpFEProfile->a10621[n][1] = 3;
        n++;
    }
    if (fPutting < 100.0f && nPutting >= 100) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 3;
        gpFEProfile->a10621[n][1] = 4;
        n++;
    }
    if (fSpin < 50.0f && nSpin >= 50) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 4;
        gpFEProfile->a10621[n][1] = 2;
        n++;
    }
    if (fSpin < 75.0f && nSpin >= 75) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 4;
        gpFEProfile->a10621[n][1] = 3;
        n++;
    }
    if (fSpin < 100.0f && nSpin >= 100) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 4;
        gpFEProfile->a10621[n][1] = 4;
        n++;
    }
    if (fRecovery < 50.0f && nRecovery >= 50) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 5;
        gpFEProfile->a10621[n][1] = 2;
        n++;
    }
    if (fRecovery < 75.0f && nRecovery >= 75) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 5;
        gpFEProfile->a10621[n][1] = 3;
        n++;
    }
    if (fRecovery < 100.0f && nRecovery >= 100) {
        gpFEProfile->n10620++;
        gpFEProfile->a10621[n][0] = 5;
        gpFEProfile->a10621[n][1] = 4;
        n++;
    }
}

// Front-end message 270: how many pairs GM_vBuildAttributeLevelUps listed (gpFEProfile->n10620).
void GM_vGetNumAttributeLevelUps(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->n10620;
}

// Front-end message 271: pair pArgs[0] of the list GM_vBuildAttributeLevelUps made: the attribute
// group into the word pArgs[1] points at, the level reached into the word pArgs[2] points at (-1
// and -1 past the list's end).
void GM_vGetAttributeLevelUp(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = gpFEProfile->a10621[pArgs[0].i][0];
    *(s32*)pArgs[2].p = gpFEProfile->a10621[pArgs[0].i][1];
}

// Front-end message 272: empties the list GM_vBuildAttributeLevelUps made (its count,
// gpFEProfile->n10620, set to 0; the pairs stay).
void GM_vClearAttributeLevelUps(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->n10620 = 0;
}

// Front-end message 273: whether MC.c's fn_800A218C finds its file on the card in port pArgs[0],
// slot pArgs[1]. fn_800A218C always returns MC_ERR_NOFILE in this build (the stub twin of
// fn_800A2194's search), so the answer is always 0.
void GM_vFEMessage273_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A218C(pArgs[0].i, pArgs[1].i) == 0;
}

// Front-end message 632: 1 when the card in port pArgs[0], slot pArgs[1] holds a file whose name
// contains "BASLUS-20572" (fn_800A2194: another PlayStation 2 product code than this game's own
// save, MC_FILE_NAME "BASLUS-20757"), else 0 (also when the card cannot be read).
void GM_vMCHasSLUS20572Save(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2194(pArgs[0].i, pArgs[1].i) == 0;
}

// Front-end message 274: how many of the EA titles in the 'eagm' list have a save on the card in
// port pArgs[0], slot pArgs[1] (MC_NumEASaveGames: it marks them, and returns a card error instead
// when the card cannot be read), and into *pArgs[2] how many titles the list holds
// (MC_GetNumEATitles).
void GM_vMCNumEASaveGames(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_NumEASaveGames(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = MC_GetNumEATitles();
}

// Front-end message 275: whether EA title pArgs[0] of the 'eagm' list was found on the card by the
// last GM_vMCNumEASaveGames (MC_EASaveExists).
void GM_vMCEASaveExists(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_EASaveExists(pArgs[0].i);
}

// Front-end message 276: the name of EA title pArgs[0] of the 'eagm' list (MC_GetEASaveName),
// copied into the string pArgs[1].
void GM_vMCGetEASaveName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, MC_GetEASaveName(pArgs[0].i));
}

// Front-end message 277: the money the memory card rewards (TW07: GM_vSetMCRewardMoney(money,
// oldUserLoaded)). pArgs[0] is kept in gFEState.n1C, which a new profile gets on top of its
// 25000 start; when pArgs[1] is set (a profile is already loaded) it is also added now to player
// slot 0's money to spend (nCurrentCash).
void GM_vSetMCRewardMoney(MsgArg* pArgs, MsgArg* pResult) {
    s32 nAmount;

    nAmount = pArgs[0].i;
    gFEState.n1C = nAmount;
    if (pArgs[1].i != 0) {
        gpSaveData->nCurrentCash += nAmount;
    }
}

// Front-end message 278: always writes 2 into *pArgs[0] and 1 into *pArgs[1] in this build (the
// round's GM_vIGMessage139_Return2And1 does the same).
void GM_vFEMessage278_Return2And1(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

// Front-end message 279: always answers 7 in this build.
void GM_vFEMessage279_Return7(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

// Front-end message 280: for the card in port pArgs[0], slot pArgs[1] (MC_GetMC), 1 or 0 into
// *pArgs[2] when its sectors are not 8 KB, and into *pArgs[3..6] its flags: bad encoding
// (MC_CARD_ENCODING), not a memory card (MC_CARD_WRONGDEVICE), I/O error (MC_CARD_IOERROR) and
// broken (MC_CARD_BROKEN). The round's GM_vIG_MCGetCardErrors is the same.
void GM_vMCGetCardErrors(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    if (state.nSectorSize != 0x2000) {
        *(s32*)pArgs[2].p = 1;
    } else {
        *(s32*)pArgs[2].p = 0;
    }
    if (state.uFlags & MC_CARD_ENCODING) {
        *(s32*)pArgs[3].p = 1;
    } else {
        *(s32*)pArgs[3].p = 0;
    }
    if (state.uFlags & MC_CARD_WRONGDEVICE) {
        *(s32*)pArgs[4].p = 1;
    } else {
        *(s32*)pArgs[4].p = 0;
    }
    if (state.uFlags & MC_CARD_IOERROR) {
        *(s32*)pArgs[5].p = 1;
    } else {
        *(s32*)pArgs[5].p = 0;
    }
    if (state.uFlags & MC_CARD_BROKEN) {
        *(s32*)pArgs[6].p = 1;
        return;
    }
    *(s32*)pArgs[6].p = 0;
}

// Front-end message 281: ends the menus' main loop: gSession.nC 2, which gomainloop.c fn_8006D01C
// checks each frame (the round's IG_vEndGameLoop is the same).
void GM_vEndGameLoop(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

// Front-end message 282: course pArgs[0]'s name (lbl_80191990: "Pebble Beach", ...), copied into
// the string pArgs[1].
void GM_vGetCourseName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[pArgs[0].i]);
}

// Front-end message 284: empty in this build.
void GM_vFEMessage284_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 285: empty in this build.
void GM_vFEMessage285_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 288: writes pArgs[0] back into *pArgs[1] and 0 into *pArgs[2] (the round's
// GM_vIGMessage141_ReturnArg does the same).
void GM_vFEMessage288_ReturnArg(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

// Front-end message 289: the letter for number pArgs[0] (0 is "A") into the string pArgs[2];
// pArgs[1] is not used. The round's GM_vGetLetter runs it with its own arguments.
void GM_vFEGetLetter(MsgArg* pArgs, MsgArg* pResult) {
    sprintf(((MsgString*)pArgs[2].p)->pStr, "%c", pArgs[0].i + 'A');
}

// Front-end message 290: always answers 2 in this build.
void GM_vFEMessage290_Return2(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

// Front-end message 291: always answers 4 in this build.
void GM_vFEMessage291_Return4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 4;
}

// Front-end message 294: copies player slot pArgs[0]'s profile into its backup row (FE_Manager.c
// FE_BackupProfile; gFEState.p658[aBackup[slot]]).
void GM_vBackupProfile(MsgArg* pArgs, MsgArg* pResult) {
    FE_BackupProfile(pArgs[0].i);
}

// Front-end message 295: always answers 0 and an empty string (into the string pArgs[0]) in this
// build.
void GM_vFEMessage295_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    strcpy(((MsgString*)pArgs[0].p)->pStr, "");
}

// Front-end message 296: empty in this build.
void GM_vFEMessage296_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 297: empty in this build.
void GM_vFEMessage297_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 298: empty in this build.
void GM_vFEMessage298_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 300: how many more files the card in port pArgs[0], slot pArgs[1] has room for
// in its directory (MCCardState.nFreeFiles). The round's GM_vIG_MCFreeFiles is the same.
void GM_vMCFreeFiles(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.nFreeFiles;
}

// Front-end message 304: place pArgs[2] (0 best .. 4) of a skill-zone or long-drive record table,
// its value answered and its holder's name copied into the string pArgs[3]. pArgs[1] picks the
// table: game mode 16, 17 or 13 the skill-zone records (gSession.recB[pArgs[0]][0..2], pArgs[0] the
// hole), 0 or 1 the long-drive contest variant's records (recC[pArgs[0]][0..1], pArgs[0] the hole's
// record index); anything else leaves the result alone.
void GM_vGetSkillZoneOrLongDriveRecord(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 13:
        pResult->i = gSession.recB[pArgs[0].i][2][pArgs[2].i].nValue;
        strcpy(((MsgString*)pArgs[3].p)->pStr, gSession.recB[pArgs[0].i][2][pArgs[2].i].szName);
        break;
    case 16:
        pResult->i = gSession.recB[pArgs[0].i][0][pArgs[2].i].nValue;
        strcpy(((MsgString*)pArgs[3].p)->pStr, gSession.recB[pArgs[0].i][0][pArgs[2].i].szName);
        break;
    case 17:
        pResult->i = gSession.recB[pArgs[0].i][1][pArgs[2].i].nValue;
        strcpy(((MsgString*)pArgs[3].p)->pStr, gSession.recB[pArgs[0].i][1][pArgs[2].i].szName);
        break;
    case 0:
        pResult->i = gSession.recC[pArgs[0].i][0][pArgs[2].i].nValue;
        strcpy(((MsgString*)pArgs[3].p)->pStr, gSession.recC[pArgs[0].i][0][pArgs[2].i].szName);
        break;
    case 1:
        pResult->i = gSession.recC[pArgs[0].i][1][pArgs[2].i].nValue;
        strcpy(((MsgString*)pArgs[3].p)->pStr, gSession.recC[pArgs[0].i][1][pArgs[2].i].szName);
        break;
    }
}

// Front-end message 306: empties player slot pArgs[0]: no profile in it (SaveProfile.bActive 0) and
// none loaded (gFEState.aLoaded).
void GM_vEmptyProfileSlot(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].bActive = 0;
    gFEState.aLoaded[pArgs[0].i] = 0;
}

// Front-end message 307: empty in this build.
void GM_vFEMessage307_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 308: challenge pArgs[0]'s three medal rewards (PlayNow_GetRewards) into
// *pArgs[1] (the lowest medal's), *pArgs[2] and *pArgs[3] (the best's).
void GM_vGetChallengeRewards(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_GetRewards(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p);
}

// Front-end message 309: the length of the string pArgs[0] (strlen). The round's
// GM_vGetStringLength is the same.
void GM_vFEGetStringLength(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

// Front-end message 310: switches EA Trax track pArgs[1] (0..18) on or off (pArgs[2]) in music row
// pArgs[0] (gSession.options.rows; StartBackgroundMusic plays the row's tracks that are on).
void GM_vSetEATraxTrack(MsgArg* pArgs, MsgArg* pResult) {
    gSession.options.rows[pArgs[0].i][pArgs[1].i] = pArgs[2].i;
}

// Front-end message 311: whether EA Trax track pArgs[1] is on in music row pArgs[0]
// (gSession.options.rows), and the track's two lines of text: the strings pArgs[2] and pArgs[3] are
// pointed at its sz0 and its song name (Trax.c's lbl_801F846C).
void GM_vGetEATraxTrack(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.options.rows[pArgs[0].i][pArgs[1].i];
    ((MsgString*)pArgs[2].p)->pStr = lbl_801F846C[pArgs[1].i].sz0;
    ((MsgString*)pArgs[3].p)->pStr = lbl_801F846C[pArgs[1].i].szSong;
}

// Front-end message 315: picks player slot pArgs[0] as the one whose saved custom round the menus
// edit (gpFEProfile->n3; n4 is the round), as the hole-par message GM_vGetHolePar reads it.
void GM_vSetCustomRoundSlot(MsgArg* pArgs, MsgArg* pResult) {
    gpFEProfile->n3 = pArgs[0].i;
}

// Front-end message 316: the game's title, "TIGER WOODS PGA TOUR(R) 2004": the string pArgs[0] is
// pointed at it (the round's GM_vIG_GetGameName is the same).
void GM_vGetGameName(MsgArg* pArgs, MsgArg* pResult) {
    ((MsgString*)pArgs[0].p)->pStr = "TIGER WOODS PGA TOUR\xAE 2004";
}

// Front-end message 317: 1 when the save on the card in port pArgs[0], slot pArgs[1] is bad data
// (fn_8009EE28 loads and checks the save file and its backup: MC_ERR_BADDATA), else 0, also when
// there is no card or no save. The round's GM_vIG_MCIsSaveCorrupt runs it.
void GM_vMCIsSaveCorrupt(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_8009EE28(pArgs[0].i, pArgs[1].i) == MC_ERR_BADDATA) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

// Front-end message 318: deletes the game's save from the card in port pArgs[0], slot pArgs[1]
// (MC_DeleteSaveGame). Answers 1 once it is gone, else MC_DeleteSaveGame's error. The round's
// GM_vIG_MCDeleteSave runs it.
void GM_vMCDeleteSave(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = MC_DeleteSaveGame(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Front-end message 319: the music options as the menus' choices: each of the four music rows'
// switches (gSession.options.abRowOn) into *pArgs[0..3] as 1 on or 2 off, option b7E the same way
// into *pArgs[4], and option n80 into *pArgs[5].
void GM_vGetEATraxOptions(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 4; i++) {
        switch (gSession.options.abRowOn[i]) {
        case 1:
            *(s32*)pArgs[i].p = 1;
            break;
        case 0:
            *(s32*)pArgs[i].p = 2;
            break;
        }
    }
    *(s32*)pArgs[4].p = gSession.options.b7E ? 1 : 2;
    *(s32*)pArgs[5].p = gSession.options.n80;
}

// Front-end message 320: sets the music options from the menus' choices: the four music rows'
// switches (gSession.options.abRowOn) from pArgs[0..3] (1 on, 2 off; any other value leaves the row
// as it is), option b7E from pArgs[4] (1 on) and option n80 from pArgs[5]; then the music is picked
// again (Gaud_SetStreamingContext).
void GM_vSetEATraxOptions(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 4; i++) {
        switch (pArgs[i].i) {
        case 1:
            gSession.options.abRowOn[i] = 1;
            break;
        case 2:
            gSession.options.abRowOn[i] = 0;
            break;
        }
    }
    gSession.options.b7E = pArgs[4].i == 1;
    gSession.options.n80 = pArgs[5].i;
    Gaud_SetStreamingContext();
}

// Front-end message 321: whether choice pArgs[0] can be picked: 1 always; 2 once a loaded profile
// or a cheat code (lbl_80281DF4) has course unlock 21, 3 the same for course unlock 22
// (GM_Earnings_CheckUnlockCourses buys them with prices 23 and 21; the "ALLTHETRACKS" code sets
// both); any other choice 0.
void GM_vIsCourseChoiceUnlocked(MsgArg* pArgs, MsgArg* pResult) {
    s32 nChoice = pArgs[0].i;
    int b = 0;
    int i;

    for (i = 0; i < 5; i++) {
        switch (nChoice) {
        case 1:
            b = 1;
            break;
        case 2:
            if (gpSaveData[i].bActive != 0 && gpSaveData[i].aCourseUnlocked[21] != 0) {
                b = 1;
            }
            if (lbl_80281DF4->aCourseUnlocked[21] != 0) {
                b = 1;
            }
            break;
        case 3:
            if (gpSaveData[i].bActive != 0 && gpSaveData[i].aCourseUnlocked[22] != 0) {
                b = 1;
            }
            if (lbl_80281DF4->aCourseUnlocked[22] != 0) {
                b = 1;
            }
            break;
        }
    }
    pResult->i = b;
}

// Front-end message 323: for the current ladder event (GameMode4_GetCurrentEvent, its challenge in
// PlayNowMode's list), the most a single opponent's skins are worth over the holes played
// (GameMode4_GetCurrentEventHoles: 1 all 18, 2 the front nine, 3 the back nine; each hole's skin
// from gEarningsTable.aSkins at the opponent's money rating). Then each opponent playing the same
// golfer model as the player's own golfer gets the look after the player's (gSession.aProfile[].n0,
// 0..3, wrapping).
void GM_vGetLadderEventMaxSkins(MsgArg* pArgs, MsgArg* pResult) {
    int  nChallenge = GameMode4_GetCurrentEvent() - 1;
    s32  nOpponents = PlayNow_GetNumOpponents(nChallenge);
    s32  nMax = 0;
    s32  nHoles = GameMode4_GetCurrentEventHoles();
    s32  nGolfer;
    s32  nSum;
    s32  nLook;
    s32  i;
    s32  h;
    GolferRecord* pRecord;
    GolferRecord* pOther;

    for (i = 0; i < nOpponents; i++) {
        nGolfer = PlayNow_GetOpponent(nChallenge, i);
        nSum = 0;
        for (h = 0; h < 6; h++) {
            if (nHoles == 2 || nHoles == 1) {
                nSum += gEarningsTable.aSkins[GM_GetGolferMoneyRating(nGolfer)].aValue[0];
            }
        }
        for (h = 6; h < 9; h++) {
            if (nHoles == 2 || nHoles == 1) {
                nSum += gEarningsTable.aSkins[GM_GetGolferMoneyRating(nGolfer)].aValue[1];
            }
        }
        for (h = 9; h < 12; h++) {
            if (nHoles == 3 || nHoles == 1) {
                nSum += gEarningsTable.aSkins[GM_GetGolferMoneyRating(nGolfer)].aValue[1];
            }
        }
        for (h = 12; h < 17; h++) {
            if (nHoles == 3 || nHoles == 1) {
                nSum += gEarningsTable.aSkins[GM_GetGolferMoneyRating(nGolfer)].aValue[2];
            }
        }
        for (h = 17; h < 18; h++) {
            if (nHoles == 3 || nHoles == 1) {
                nSum += gEarningsTable.aSkins[GM_GetGolferMoneyRating(nGolfer)].aValue[3];
            }
        }
        nMax = (nSum > nMax) ? nSum : nMax;
    }
    pResult->i = nMax;

    pRecord = FE_spGetGolfer(gSession.nGolfer[gpFEProfile->nSlot]);
    for (i = 0; i < nOpponents; i++) {
        pOther = FE_spGetGolfer(PlayNow_GetOpponent(nChallenge, i));
        if (pRecord->nModelID == pOther->nModelID) {
            nLook = gSession.aProfile[gpFEProfile->nSlot].n0 + 1;
            if (nLook == 4) {
                nLook = 0;
            }
            gSession.aProfile[i + 1].n0 = nLook;
        }
    }
}

// Front-end message 324: empty in this build.
void GM_vFEMessage324_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 327: queues a movie (FE_movieGetFreeEntry; FE_movieFade fades to black and plays it):
// the credits when pArgs[0] is -1, else a bio movie; the music stops (Gaud_StopMusic). pArgs[0] is
// not stored as the bio's number: nothing in this build writes FEMovie.nBio, so a bio movie always
// plays bios/bio01.
void GM_vQueueMovie(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = FE_movieGetFreeEntry();
    pMovie->nKind = FE_MOVIE_BIO;
    if (pArgs[0].i == -1) {
        pMovie->nKind = FE_MOVIE_CREDITS;
    }
    Gaud_StopMusic();
}

// Front-end message 704: stops the music (Gaud_StopMusic).
void GM_vStopMusic(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_StopMusic();
}

// Front-end message 328: always answers 0 in this build.
void GM_vFEMessage328_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 329: player pArgs[0] plays the created golfer of the same slot (its
// gSession.nGolfer becomes FIRST_CREATED_GOLFER, 30, plus the player's number).
void GM_vSetPlayerCreatedGolfer(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    gSession.nGolfer[n] = (u8)(n + 30);
}

// Front-end message 330: player slots 0..3 lose their backup rows (gFEState.aBackup -1; slot
// 4's is left).
void GM_vClearBackupRows(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.aBackup[0] = -1;
    gFEState.aBackup[1] = -1;
    gFEState.aBackup[2] = -1;
    gFEState.aBackup[3] = -1;
}

// Front-end message 331: player slot pArgs[0] loses its backup row (gFEState.aBackup -1).
void GM_vClearBackupRow(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.aBackup[pArgs[0].i] = -1;
}

// Front-end message 332: 1 when the controller in port pArgs[0] is a WaveBird (Input_iGetPadType
// 0x8B100000), else 0.
void GM_vIsWaveBird(MsgArg* pArgs, MsgArg* pResult) {
    if (Input_iGetPadType(pArgs[0].i) == 0x8B100000) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

// Front-end message 333: the front end's flag gFEState.b10 (1 after the front end's set-up,
// FE_vOpenONCE; slot 334 sets it; nothing else reads it).
void GM_vGetFEStateB10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gFEState.b10;
}

// Front-end message 334: sets the front end's flag gFEState.b10 (read by slot 333).
void GM_vSetFEStateB10(MsgArg* pArgs, MsgArg* pResult) {
    gFEState.b10 = pArgs[0].i;
}

// Front-end message 335, also the round's command 164: whether the card in port pArgs[0] has had an
// I/O error (fn_8009F728: lbl_80281FD0, set on CARD_RESULT_IOERROR).
void GM_vMCHadIOError(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F728(pArgs[0].i);
}

// Front-end message 336: always answers 0 in this build.
void GM_vFEMessage336_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 337: for the card in port pArgs[0], slot pArgs[1] (connected for the look), the
// new files a save of the game needs (fn_8009D3DC: 1 when the save file or its backup is not on the
// card yet) into *pArgs[2], and the blocks it needs (MC_BlocksNeededForSave, kind 0: 40 when the
// file is not there yet) into *pArgs[3]. The round's GM_vIG_MCGetSaveNeeds runs it.
void GM_vMCGetSaveNeeds(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = fn_8009D3DC(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[3].p = MC_BlocksNeededForSave(pArgs[0].i, pArgs[1].i, 0, 0);
    MC_Disconnect();
}

// Front-end message 766: GM_vMCGetSaveNeeds for the EA Sports Bio: the new files its save needs
// (fn_8009D50C: 1 when there is no "EASB" file on the card yet) into *pArgs[2], and its blocks
// (MC_BlocksNeededForSave kind 3) into *pArgs[3].
void GM_vMCGetEASBSaveNeeds(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = fn_8009D50C(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[3].p = MC_BlocksNeededForSave(pArgs[0].i, pArgs[1].i, 0, 3);
    MC_Disconnect();
}

// Front-end message 338: empty in this build.
void GM_vFEMessage338_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 339: always answers 0 in this build.
void GM_vFEMessage339_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 340: always answers 0 in this build.
void GM_vFEMessage340_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 341: gives player pArgs[0] its golfer's clubs (gSession.uBag, a bit per club):
// a created golfer's own bag (GolferRecord.uBagMask), any other golfer the default bag 0x02A7FC44.
void GM_vSetPlayerBag(MsgArg* pArgs, MsgArg* pResult) {
    s32 nPlayer = pArgs[0].i;
    GolferRecord* pRecord = FE_spGetGolfer(gSession.nGolfer[nPlayer]);

    if (gSession.nGolfer[nPlayer] < FIRST_CREATED_GOLFER) {
        gSession.uBag[nPlayer] = 0x02A7FC44;
        return;
    }
    gSession.uBag[nPlayer] = pRecord->uBagMask;
}

// Front-end message 342: empties profile pArgs[0]'s saved custom round pArgs[1]: every hole none
// (nHoleNum -1, nCourse 0), and the round no longer in use (n0 0).
void GM_vClearSavedRound(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 18; i++) {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nHoleNum[i] = -1;
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nCourse[i] = 0;
    }
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = 0;
}

// Front-end message 343: sets whether profile pArgs[0]'s saved custom round pArgs[1] is in use
// (SavedRound.n0 = pArgs[2]).
void GM_vSetSavedRoundInUse(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = pArgs[2].i;
}

// Front-end message 344: always answers 150 in this build.
void GM_vFEMessage344_Return150(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 150;
}

// Front-end message 345: empty in this build.
void GM_vFEMessage345_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 346: empty in this build.
void GM_vFEMessage346_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 347: empty in this build.
void GM_vFEMessage347_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 348: empty in this build.
void GM_vFEMessage348_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 349: empty in this build.
void GM_vFEMessage349_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 350: empty in this build.
void GM_vFEMessage350_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 351: empty in this build.
void GM_vFEMessage351_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 352: empty in this build.
void GM_vFEMessage352_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 353: empty in this build.
void GM_vFEMessage353_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 354: empty in this build.
void GM_vFEMessage354_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 355: compares the strings pArgs[0] and pArgs[1] (strcmp: 0 when equal, below 0
// when the first sorts first, above 0 when it sorts after).
void GM_vCompareStrings(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strcmp(((MsgString*)pArgs[0].p)->pStr, ((MsgString*)pArgs[1].p)->pStr);
}

// Front-end message 356: empty in this build.
void GM_vFEMessage356_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 357: empty in this build.
void GM_vFEMessage357_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 358: empty in this build.
void GM_vFEMessage358_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 359: empty in this build.
void GM_vFEMessage359_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 360: empty in this build.
void GM_vFEMessage360_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 361: empty in this build.
void GM_vFEMessage361_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 362: empty in this build.
void GM_vFEMessage362_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 363: empty in this build.
void GM_vFEMessage363_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 364: empty in this build.
void GM_vFEMessage364_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 365: empty in this build.
void GM_vFEMessage365_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 366: empty in this build.
void GM_vFEMessage366_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 367: empty in this build.
void GM_vFEMessage367_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 368: the working profile's created golfer is left-handed when pArgs[0] is set
// (FE_SetProfileLeftHanded).
void GM_vSetLeftHanded(MsgArg* pArgs, MsgArg* pResult) {
    FE_SetProfileLeftHanded(gpFEProfile->nSlot, pArgs[0].i);
}

// Front-end message 369: answers 0.0 (as a float).
void GM_vFEMessage369_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 0.0f;
}

// Front-end message 370: empty in this build.
void GM_vFEMessage370_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 371: three values out through the words pArgs[4], pArgs[5] and pArgs[6] point
// at: two are 50, and the one pArgs[2] % 3 picks (0, 1, 2) is a level of 25..250, (pArgs[3] % 10 +
// 1) * 25. What the menus use the three for is not known.
void GM_vGetThreeLevelsOneRaised(MsgArg* pArgs, MsgArg* pResult) {
    s32 nWhich;
    s32 nLevel;

    nWhich = pArgs[2].i % 3;
    nLevel = (pArgs[3].i % 10 + 1) * 25;
    switch (nWhich) {
    case 0:
        *(s32*)pArgs[4].p = nLevel;
        *(s32*)pArgs[5].p = 50;
        *(s32*)pArgs[6].p = 50;
        return;
    case 1:
        *(s32*)pArgs[4].p = 50;
        *(s32*)pArgs[5].p = nLevel;
        *(s32*)pArgs[6].p = 50;
        return;
    case 2:
        *(s32*)pArgs[4].p = 50;
        *(s32*)pArgs[5].p = 50;
        *(s32*)pArgs[6].p = nLevel;
        return;
    }
}

// Front-end message 372: empty in this build.
void GM_vFEMessage372_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 380: empty in this build.
void GM_vFEMessage380_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 399: the string pArgs[0] becomes pArgs[1] asterisks (a hidden entry).
void GM_vMaskString(MsgArg* pArgs, MsgArg* pResult) {
    char szStars[64] = "";
    int i;
    int nLen;

    nLen = pArgs[1].i;
    for (i = 0; i < nLen; i++) {
        szStars[i] = '*';
    }
    // EA bug: the terminator goes one past the stars (the buffer is zeroed, so the string still
    // ends); for pArgs[1] of 63 it writes szStars[64], past the buffer (64 or more: the loop does)
    szStars[i + 1] = '\0';
    strcpy(((MsgString*)pArgs[0].p)->pStr, szStars);
}

// Front-end message 518: the two-player long-drive race's (game mode 26) target score is pArgs[0]
// (GameMode26_SetTargetScore; 10000 until set).
void GM_vSetLongDriveTargetScore(MsgArg* pArgs, MsgArg* pResult) {
    GameMode26_SetTargetScore(pArgs[0].i);
}

// Front-end message 465: asks for the other disc (fn_801102AC: in the menus the menu golfer's
// streaming stops and pauses and the disc change is started later by fn_80110390). Always answers
// 0.
void GM_vSwapDisc(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    if (pResult->i == 0) {
        fn_801102AC();
    }
}

// Front-end message 472: starts the current game mode's event: the lesson (mode 11), the Play Now
// challenge (5), the PGA TOUR tee-off (23), the long-drive race (26), the long-drive contest (22)
// or the real-time event (24). In modes 11 and 5 player 0 first gets golfer 0, or, with profile 0
// loaded, row 0 of p658 (gFEState.aBackup[0]) and the created golfer (FIRST_CREATED_GOLFER; not
// when the working profile's b11703 is set). Answers whether the course is on the disc in the drive
// (fn_80110180, its hole-file check turned on while it runs).
void GM_vStartEventCheckDisc(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 11) {
        if (Game_GetMode() == 5 || Game_GetMode() == 11) {
            if (gpSaveData[0].bActive) {
                gFEState.aBackup[0] = 0;
                if (gpFEProfile->b11703 == 0) {
                    Session_SetGolfer(FIRST_CREATED_GOLFER, 0);
                }
            } else {
                Session_SetGolfer(0, 0);
            }
        }
        Lessons_StartFromMenu();
        fn_80110178(1);
    } else if (Game_GetMode() == 5) {
        if (Game_GetMode() == 5 || Game_GetMode() == 11) {
            if (gpSaveData[0].bActive) {
                gFEState.aBackup[0] = 0;
                if (gpFEProfile->b11703 == 0) {
                    Session_SetGolfer(FIRST_CREATED_GOLFER, 0);
                }
            } else {
                Session_SetGolfer(0, 0);
            }
        }
        PlayNow_StartChallenge();
        fn_80110178(1);
    } else if (Game_GetMode() == 23) {
        GameModeDriverPGATour_PrepareForTeeOff();
    } else if (Game_GetMode() == 26) {
        GameMode26_StartEvent();
    } else if (Game_GetMode() == 22) {
        GameMode22_StartEvent();
    } else if (Game_GetMode() == 24) {
        GameModeDriverRTE_StartEvent();
    }
    pResult->i = fn_80110180();
    fn_80110178(0);
}

// Front-end message 473: the state of the disc change the menus asked for (DVDGetCommandBlockStatus
// of DiscCheck.c's command block, fn_801104A0), numbered as GameUICommands.c's
// IG_vGetDiscDriveStatus numbers the drive's: motor stopped 0 (disc 2 in the drive) or 1 (disc 1),
// wrong disc 2 (disc 2) or 3 (disc 1), busy 4, anything else 5; 100 once the disc change has
// finished (fn_80110450).
void GM_vGetDiscChangeStatus(MsgArg* pArgs, MsgArg* pResult) {
    switch (DVDGetCommandBlockStatus(fn_801104A0())) {
    case 7:
        if (fn_8011027C() != 0) {
            pResult->i = 0;
        } else {
            pResult->i = 1;
        }
        break;
    case 6:
        if (fn_8011027C() != 0) {
            pResult->i = 2;
        } else {
            pResult->i = 3;
        }
        break;
    case 1:
        pResult->i = 4;
        break;
    default:
        pResult->i = 5;
        break;
    }
    if (fn_80110450() != 0) {
        pResult->i = 100;
    }
}

// Front-end message 592: starts the Game Boy Advance link: the link code is set up (Gba_Init:
// the disc's ID, the start tick, GBAInit) and the link state goes to 0, from which Gba_UpdateLinkState
// starts looking for a GBA.
void GM_vGbaStartLink(MsgArg* pArgs, MsgArg* pResult) {
    Gba_Init();
    Gba_SetState(0);
}

// Front-end message 593: link state 6: Gba_UpdateLinkState then takes the cash from the Game Boy Advance
// into the current profile, has the GBA save its cash and goes to state 7.
void GM_vGbaTakeCash(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(6);
}

// Front-end message 594: link state 8: Gba_UpdateLinkState then swaps stats with the Game Boy Advance (the
// best round, holes in one, longest drive and longest putt, the better of each kept in the current
// profile), has it save them and goes to state 9.
void GM_vGbaSwapStats(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(8);
}

// Front-end message 595: empty in this build.
void GM_vFEMessage595_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 611: puts the Game Boy Advance link in state 18 (0x12), the state a failed
// command leaves it in: Gba_UpdateLinkState stops working the link and undoes what a pending request did to
// the profile.
void GM_vGbaCancelLink(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(18);
}

// Front-end message 629: the Game Boy Advance link's unlocks, once per profile. When bit 1 of the
// current profile's a10548 is still clear (the bit also unlocks the Create-A-Player items of lock
// kind 2, FE_Manager.c FE_CrAP_IsItemLocked): the last course (22) and rewards 0..17 are unlocked
// (Gba_UnlockProfileRewards), gbacable.c's gGbaUnlocksGranted is set (Gba_MarkUnlocksGranted), the
// bit is set and the answer is 1. Else 0.
void GM_vGbaGrantUnlocks(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

    if (!UserInfo_GetUserFlag(pProfile, 1)) {
        Gba_UnlockProfileRewards();
        Gba_MarkUnlocksGranted();
        UserInfo_SetUserFlag(pProfile, 1, 1);
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Front-end message 630: answers 0.
void GM_vFEMessage630_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 612: the Game Boy Advance link's state (Gba_GetState) as the menus number it: 0
// looking for a GBA (state 1), 1 linked (5), 3 moving cash or stats (6, 8), 4 done moving them (7,
// 9), 6 the GBA's context could not be read (3), 7 the contexts differ (16), 8 no GBA answered in
// time (17), 9 a command failed (18). Other states leave the result as it was.
void GM_vGbaGetLinkState(MsgArg* pArgs, MsgArg* pResult) {
    switch (Gba_GetState()) {
    case 1:
        pResult->i = 0;
        break;
    case 5:
        pResult->i = 1;
        break;
    case 6:
    case 8:
        pResult->i = 3;
        break;
    case 7:
    case 9:
        pResult->i = 4;
        break;
    case 3:
        pResult->i = 6;
        break;
    case 16:
        pResult->i = 7;
        break;
    case 17:
        pResult->i = 8;
        break;
    case 18:
        pResult->i = 9;
        break;
    }
}

// Front-end message 623: reads what the Game Boy Advance holds into the words pArgs[0..4] point at:
// its cash (request 0x70, GbaChannel.u68), then its four stats (requests 0xB0 with 0..3: the best
// round, holes in one, longest drive, longest putt; GbaChannel.n70). It stops once a command fails
// (link state 18) or no link state is set (-1). Then gbacable.c's gGbaReadPending is cleared, and if
// gGbaUndoTransfer is set a link state of 7 becomes 12 (send the cash back) and 9 becomes 13 (copy the
// stats into the profile) and gGbaUndoTransfer is cleared; nothing in this build sets gGbaUndoTransfer.
void GM_vGbaReadCashAndStats(MsgArg* pArgs, MsgArg* pResult) {
    u32 i;

    if (Gba_GetState() != 18 && Gba_GetState() != -1) {
        Gba_StepPorts(0x70, 0);
        if (Gba_GetState() != 18 && Gba_GetState() != -1) {
            *(s32*)pArgs[0].p = Gba_GetCashOnGba();
            for (i = 0; i < 4; i++) {
                Gba_StepPorts(0xB0, i);
                if (Gba_GetState() == 18 || Gba_GetState() == -1) break;
                *(s32*)pArgs[1 + i].p = Gba_GetGbaStat();
            }
        }
    }
    Gba_SetReadPending(0);
    if (Gba_IsUndoTransfer()) {
        if (Gba_GetState() == 7) {
            Gba_SetState(12);
        } else if (Gba_GetState() == 9) {
            Gba_SetState(13);
        }
        Gba_SetUndoTransfer(0);
    }
}

// Front-end message 624: adds the word pArgs[0] points at to the cash to move between the Game Boy
// Advance and the profile (GbaChannel.n6C of the port in use) and answers the new amount.
void GM_vGbaAddCashToMove(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = *(s32*)pArgs[0].p;
    pResult->i = n + Gba_GetCashToMove();
    Gba_SetCashToMove(pResult->i);
}

// Front-end message 618: empty in this build.
void GM_vFEMessage618_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 619: empty in this build.
void GM_vFEMessage619_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 633: answers gbacable.c's flag gGbaReadPending (Gba_IsReadPending).
// GM_vGbaReadCashAndStats clears it after reading the Game Boy Advance, and nothing in this build
// sets it, so the answer is always 0.
void GM_vGbaIsReadPending(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Gba_IsReadPending();
}

// Front-end message 653: the cash the Game Boy Advance holds, as last read (GbaChannel.u68 of the
// port in use).
void GM_vGbaGetGbaCash(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Gba_GetCashOnGba();
}

// Front-end message 654: 1 when the Game Boy Advance link is in state 18 (0x12: a command failed,
// or GM_vGbaCancelLink), else 0.
void GM_vGbaIsLinkFailed(MsgArg* pArgs, MsgArg* pResult) {
    if (Gba_GetState() == 18) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Front-end message 748: puts the Game Boy Advance link back in state 5, linked: Gba_UpdateLinkState polls
// the GBA and sends any pending request.
void GM_vGbaResumeLink(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(5);
}

// ---- the EA Sports Bio screens (EASportsBio.c does the work) ----

// Front-end message 614: asks EASportsBio.c for the EA Sports Bio's list of games, from game
// pArgs[0], with pArgs[1] as the game picked (fn_80125600); fn_80124C10 sends the list to the menus
// on its next frame.
void GM_vEASBioListProducts(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125600(pArgs[0].i, pArgs[1].i);
}

// Front-end message 615: asks EASportsBio.c for the accomplishments of the game
// GM_vEASBioListProducts picked, from entry pArgs[0] (pArgs[1] is kept but not used), sorted by
// time unless pArgs[2] is set (fn_80125648); fn_80124C10 sends them on its next frame.
void GM_vEASBioListAccomplishments(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125648(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// Front-end message 616: while pArgs[0] is set, fn_80124C10 sends the menus the EA Sports Bio's
// summary (its level, the number of games, the time played; hint 0xB1) every 16 frames
// (fn_8012566C).
void GM_vEASBioShowSummary(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012566C(pArgs[0].i);
}

// Front-end message 617: while pArgs[0] is set, fn_80124C10 sends the menus the details of the game
// GM_vEASBioListProducts picked (its name, time played, last played date, games played and the
// share won, level; hints 0xB3, 0xBF, 0xB2) every 16 frames (fn_80125680).
void GM_vEASBioShowProductDetails(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125680(pArgs[0].i);
}

// Front-end message 627: loads the EA Sports Bio from the card: opens it (fn_80125354, which marks
// the Bio loaded and notes its level) and closes it again (fn_801253F0). 1 when both succeed, else
// 0; the working profile's n11704 keeps the error. pArgs[0], pArgs[1] (the card) are handed on but
// not used.
void GM_vEASBioLoad(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125354(pArgs[0].i, pArgs[1].i);
    gpFEProfile->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    nError = fn_801253F0(pArgs[0].i, pArgs[1].i);
    gpFEProfile->n11704 = nError;
    pResult->i = nError == 0;
}

// Front-end message 628: saves this game's record into the EA Sports Bio on the card. The Bio is
// opened and closed first (fn_80125194): when that fails with anything but -18 (MC_ERR_BADDATA,
// which most library errors become), a new Bio file is made (fn_801252D0), after deleting the old
// one (fn_80125280) when the error was -43 (EASB_ERROR_WRONG_FILE) or -44
// (EASB_ERROR_CANNOT_REOPEN); then the record is written (fn_801251EC). The answer is 1, or the
// first error; the working profile's n11704 keeps the error.
void GM_vEASBioSave(MsgArg* pArgs, MsgArg* pResult) {
    s32 aPos[2];
    s32 nError;

    aPos[0] = pArgs[0].i;
    aPos[1] = pArgs[1].i;
    nError = fn_80125194(aPos[0], aPos[1]);
    if (nError != 0 && nError != -18) {
        if (nError == -43 || nError == -44) {
            nError = fn_80125280(pArgs[0].i, pArgs[1].i);
            gpFEProfile->n11704 = nError;
            if (nError != 0) {
                pResult->i = (nError != 0) ? nError : 1;
                return;
            }
        }
        nError = fn_801252D0(pArgs[0].i, pArgs[1].i);
        gpFEProfile->n11704 = nError;
        if (nError != 0) {
            pResult->i = (nError != 0) ? nError : 1;
            gpFEProfile->n11704 = nError;
            return;
        }
    }
    nError = fn_801251EC(aPos);
    pResult->i = (nError != 0) ? nError : 1;
    gpFEProfile->n11704 = nError;
}

// Front-end message 643: whether an EA Sports Bio file is on the card in port pArgs[0], slot
// pArgs[1] (fn_80125118): TRUE when it opens (it is closed again) or fails to with -18
// (MC_ERR_BADDATA, a Bio that cannot be read).
void GM_vEASBioIsOnCard(MsgArg* pArgs, MsgArg* pResult) {
    s32 aPos[2];

    aPos[0] = pArgs[0].i;
    aPos[1] = pArgs[1].i;
    pResult->i = fn_80125118(aPos);
}

// Front-end message 644: the space the EA Sports Bio needs on the card in port pArgs[0], slot
// pArgs[1] (fn_801255C4: MC_BlocksNeededForSave for save kind 3).
void GM_vEASBioMemoryRequired(MsgArg* pArgs, MsgArg* pResult) {
    s32 aPos[2];

    aPos[0] = pArgs[0].i;
    aPos[1] = pArgs[1].i;
    pResult->i = fn_801255C4(aPos);
}

// Front-end message 645: loads the EA Sports Bio's game records (fn_80125434: opened, every PROD
// record read, closed). 1 when it succeeds, else 0; the working profile's n11704 keeps the error.
void GM_vEASBioLoadProducts(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125434(pArgs[0].i, pArgs[1].i);
    gpFEProfile->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    pResult->i = nError == 0;
}

void GM_vEASBioIsLoaded(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_IsBioLoaded();
}

// Front-end message 647: opens the EA Sports Bio on the card (fn_80125354): 0 when that fails with
// -43 (EASB_ERROR_WRONG_FILE: the tag-file library's error 9), else 1. The Bio is closed afterwards
// (fn_801253F0) even when the open failed.
void GM_vEASBioIsNotWrongFile(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_80125354(0, 0) == -43) {
        pResult->i = 0;
    } else {
        pResult->i = 1;
    }
    fn_801253F0(0, 0);
}

// Front-end message 648: 1 when the EA Sports Bio had no record of this game and no empty one, so
// this game takes the slot of the oldest game (fn_80125528: EASBStorage.n94 is 1). pArgs[0],
// pArgs[1] are handed on but not used.
void GM_vEASBioIsReplacingOldest(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125528(pArgs[0].i, pArgs[1].i);
}

// Front-end message 649: 1 when opening the EA Sports Bio on the card fails with -18
// (MC_ERR_BADDATA: most library errors, a corrupt file among them), else 0. When it opens, it is
// closed again (fn_801253F0).
void GM_vEASBioIsBadData(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError = fn_80125354(0, 0);

    if (nError == -18) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
    if (nError == 0) {
        fn_801253F0(0, 0);
    }
}

// Front-end message 650: deletes the EA Sports Bio file from the card (fn_80125280; the Bio is then
// no longer loaded). 1 when it succeeds, else 0; the working profile's n11704 keeps the error.
void GM_vEASBioDelete(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125280(pArgs[0].i, pArgs[1].i);
    gpFEProfile->n11704 = nError;
    pResult->i = nError == 0;
}

// Front-end message 655: answers 0.
void GM_vFEMessage655_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Front-end message 688: the EA Sports Bio's level (fn_80125928; 0 when no Bio is loaded) into the
// word pArgs[0] points at.
void GM_vEASBioGetLevel(MsgArg* pArgs, MsgArg* pResult) {
    s32* pN;

    pN = pArgs[0].p;
    *pN = fn_80125928();
}

// Front-end message 658: whether an EA Sports Bio reward is waiting (fn_801256B8: a new
// accomplishment, a level-up, or a level-up that unlocked something); it becomes the current reward
// message either way (EASBio_eReward_None when there is none).
void GM_vEASBioCheckReward(MsgArg* pArgs, MsgArg* pResult) {
    EASBio_eReward eReward = fn_801256B8();

    if (eReward != -1) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
    EASBio_SetCurrentRewardMessage(eReward);
}

// Front-end message 667: empty in this build.
void GM_vFEMessage667_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 668: frees the EA Sports Bio's loaded game records (fn_801254EC). The answer is
// then replaced by fn_801254B8's, which is always -18 (pictures are not supported on this
// platform).
void GM_vEASBioUnloadProducts(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801254EC();
    pResult->i = fn_801254B8();
}

// Front-end message 666: the EA Sports Bio's progress to its next level as a percentage
// (fn_80124BDC), as a float.
void GM_vEASBioGetLevelProgress(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_80124BDC();
}

// Front-end message 671: the memory-card screens' save kind is pArgs[0] (MC_SetCurrentFileType: 0
// the options, 1 a profile, 2 a replay, 3 the EA Sports Bio).
void GM_vMCSetCurrentFileType(MsgArg* pArgs, MsgArg* pResult) {
    MC_SetCurrentFileType(pArgs[0].i);
}

// Front-end message 672: runs memory-card operation pArgs[0] of the save kind MC_SetCurrentFileType
// picked, on the card in port pArgs[1], slot pArgs[2], each given its own payload (core/memcard.h):
// 0 save and 1 load answer 1 when they succeed, else 0; 2 the files on the card (a count, or
// whether the file is there), 3 whether the file there is bad data and 4 the space it needs answer
// the operation's own result. Operations 0, 1 and 3 also get pArgs[3] (MCCardPos.n8), 1 and 4 the
// name in string pArgs[4].
void GM_vMCCallActionFn(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos    pos0;
    MCCardPosStr posStr;
    MCCardPos    pos3;
    MCOpCardName cardName;
    MCOpCard     card;
    int   nOp = pArgs[0].i;
    s32   nPort = pArgs[1].i;
    s32   nSlot = pArgs[2].i;
    s32   n8 = pArgs[3].i;
    char* szName = ((MsgString*)pArgs[4].p)->pStr;

    switch (nOp) {
    case 0:
        pos0.nPort = nPort;
        pos0.nSlot = nSlot;
        pos0.n8 = n8;
        pResult->i = MC_CallActionFnSave(&pos0) == 0;
        break;
    case 1:
        posStr.pos.nPort = nPort;
        posStr.pos.nSlot = nSlot;
        posStr.pos.n8 = n8;
        posStr.szC = szName;
        pResult->i = MC_CallActionFnLoad(&posStr) == 0;
        break;
    case 3:
        pos3.nPort = nPort;
        pos3.nSlot = nSlot;
        pos3.n8 = n8;
        pResult->i = MC_CallActionFnDataCorrupt(&pos3);
        break;
    case 2:
        card.nPort = nPort;
        card.nSlot = nSlot;
        pResult->i = MC_CallActionFnNumFilesOnCard(&card);
        break;
    case 4:
        cardName.nPort = nPort;
        cardName.nSlot = nSlot;
        cardName.szName = szName;
        pResult->i = MC_CallActionFnMemoryRequired(&cardName);
        break;
    }
}

// Front-end message 675: empty in this build.
void GM_vFEMessage675_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 678: the number of Play Now challenge groups (PlayNow_GetNumGroups).
void GM_vPlayNowGetNumGroups(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetNumGroups();
}

// Front-end message 679: picks Play Now challenge group pArgs[0], counted from 1
// (PlayNow_SelectGroup).
void GM_vPlayNowSelectGroup(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_SelectGroup(pArgs[0].i - 1);
}

// Front-end message 680: the name of Play Now challenge group pArgs[1], counted from 1, into the
// string pArgs[0].
void GM_vPlayNowGetGroupName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupName(pArgs[1].i - 1));
}

// Front-end message 681: the description of Play Now challenge group pArgs[1], counted from 1, into
// the string pArgs[0].
void GM_vPlayNowGetGroupDescription(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupDescription(pArgs[1].i - 1));
}

// Front-end message 682: profile pArgs[0]'s best medal in Play Now challenge group pArgs[1],
// counted from 1 (SaveProfile.aMedal: 0 the best, 3 none).
void GM_vPlayNowGetGroupMedal(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aMedal[pArgs[1].i - 1];
}

// Front-end message 684: the round's course is pArgs[0] (GM_SetCurrentCourse and its first selected
// hole), or 10000 for the random mixed round (built by GM_BuildRandom18 only the first time in a
// row; gpGame->bRandom18 remembers it). Answers which disc the course is on: whether it is on the
// disc in the drive (fn_80110180), inverted when disc 2 is in the drive (fn_8011027C), so 1 for
// disc 1 and 0 for disc 2.
void GM_vSetCourseFindDisc(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 10000) {
        if (gpGame->bRandom18 == 0) {
            gpGame->bRandom18 = 1;
            GM_BuildRandom18();
        }
        pResult->i = fn_80110180();
        if (fn_8011027C() != 0) {
            pResult->i = pResult->i == 0;
        }
    } else {
        GM_SetCurrentCourse(pArgs[0].i);
        GM_InitializeCurrentHoleToFirstSelected();
        pResult->i = fn_80110180();
        if (fn_8011027C() != 0) {
            pResult->i = pResult->i == 0;
        }
        gpGame->bRandom18 = 0;
    }
}

// Front-end message 692, the long-drive menu. pArgs[0] 0 picks the game by pArgs[1]: 0 (or anything
// else) the two-player race (game mode 26), 1 the contest (mode 22) where every drive's points add
// up, 2 the contest where only the best drive counts (GameMode22_SetVariant); the mode is set up
// and split screen set for it. pArgs[0] 1 sets the contest's drives per player by pArgs[1]: 5 (0 or
// anything else), 10 (1) or 15 (2).
void GM_vSetLongDriveOptions(MsgArg* pArgs, MsgArg* pResult) {
    s32 nMode;
    s32 n;

    switch (pArgs[0].i) {
    case 0:
        switch (pArgs[1].i) {
        case 0:
        default:
            nMode = 26;
            n = 0;
            break;
        case 1:
            nMode = 22;
            n = 0;
            break;
        case 2:
            nMode = 22;
            n = 1;
            break;
        }
        GM_SetModeType(nMode);
        GM_SetSplitScreenForMode();
        if (nMode == 22) {
            GameMode22_SetVariant(n);
        }
        return;
    case 1:
        switch (pArgs[1].i) {
        case 0:
        default:
            n = 5;
            break;
        case 1:
            n = 10;
            break;
        case 2:
            n = 15;
            break;
        }
        GameMode22_SetNumDrives(n);
        break;
    }
}

// Front-end message 696: a trophy room status question, passed on whole by pArgs[0]: 0 a tour win
// (TrophyRoom_GetTourWinStatus), 1 a player of the month award (TrophyRoom_GetPlayerOfMonthStatus),
// 3 a real-time event award (TrophyRoom_GetRTEAwardStatus); 2 is not answered.
void GM_vTrophyRoomGetStatus(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        TrophyRoom_GetTourWinStatus(pArgs, pResult);
        return;
    case 1:
        TrophyRoom_GetPlayerOfMonthStatus(pArgs, pResult);
        return;
    case 3:
        TrophyRoom_GetRTEAwardStatus(pArgs, pResult);
        return;
    case 2:
        return;
    }
}

// Front-end message 715: empty in this build.
void GM_vFEMessage715_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 727, music commands by pArgs[0]: 0 switches music row 0 and its track pArgs[1]
// on and plays that track (Gaud_StartMusic, play list 13) after re-picking the stream; 1 stops the
// music; 2 restarts it (Gaud_RestartMusic) unless music is playing.
void GM_vControlMusic(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        gSession.options.abRowOn[0] = 1;
        gSession.options.rows[0][pArgs[1].i] = 1;
        Gaud_StopMusic();
        Gaud_SetStreamingContext();
        Gaud_StartMusic(13, pArgs[1].i);
        break;
    case 1:
        Gaud_StopMusic();
        break;
    case 2:
        if (!Gaud_GetMusicStatus()) {
            Gaud_RestartMusic();
        }
        break;
    }
}

// Front-end message 726: a profile name typed as nothing but spaces (or nothing) becomes "User <n>"
// in place in the string pArgs[0], n the working profile's slot + 1.
void GM_vDefaultBlankUserName(MsgArg* pArgs, MsgArg* pResult) {
    char* szName = ((MsgString*)pArgs[0].p)->pStr;
    int n = 0;
    int i;

    for (i = 0; szName[i] != '\0'; i++) {
        if (szName[i] != ' ') {
            n++;
        }
    }
    if (n == 0) {
        sprintf(szName, "User %d", gpFEProfile->nSlot + 1);
    }
}

// Front-end message 736: whether profile pArgs[2] is in the save on the card in port pArgs[0], slot
// pArgs[1] (MC_GetUser; the name it reads is dropped): 1 when it is, -1 when the save read back is
// not a good one (MC_ERR_BADDATA), else 0.
void GM_vMCIsUserOnCard(MsgArg* pArgs, MsgArg* pResult) {
    char sz[0x20];              // the size is unknown (0x20 gives the original's frame)
    s32  nResult = MC_GetUser(pArgs[0].i, pArgs[1].i, pArgs[2].i, sz);

    if (nResult == 0) {
        pResult->i = 1;
    } else if (nResult == MC_ERR_BADDATA) {
        pResult->i = -1;
    } else {
        pResult->i = 0;
    }
}

// Front-end message 737: copies the string pArgs[0] into the string pArgs[1], cut to its first 8
// characters and "..." when it is longer than 12.
void GM_vShortenString12(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 12) {
        ((MsgString*)pArgs[1].p)->pStr[11] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[10] = '.';
        ((MsgString*)pArgs[1].p)->pStr[9] = '.';
        ((MsgString*)pArgs[1].p)->pStr[8] = '.';
    }
}

// Front-end message 746: copies the string pArgs[0] into the string pArgs[1], cut to its first 28
// characters and "..." when it is longer than 32.
void GM_vShortenString32(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 32) {
        ((MsgString*)pArgs[1].p)->pStr[31] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[30] = '.';
        ((MsgString*)pArgs[1].p)->pStr[29] = '.';
        ((MsgString*)pArgs[1].p)->pStr[28] = '.';
    }
}

// Front-end message 751: the last EA Sports Bio error (the working profile's n11704, kept by
// GM_vEASBioLoad, GM_vEASBioSave, GM_vEASBioLoadProducts and GM_vEASBioDelete; 0 none).
void GM_vEASBioGetLastError(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpFEProfile->n11704;
}

// Front-end message 759: opens the EA Sports Bio on the card and closes it again (fn_80125194);
// answers the open's error (0 when it opened).
void GM_vEASBioCheck(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125194(0, 0);
}

// Front-end message 761: the space the game's save and the EA Sports Bio need together on the card
// in port pArgs[0], slot pArgs[1] (fn_8009D390).
void GM_vMCGetBlocksNeeded(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009D390(pArgs[0].i, pArgs[1].i);
}

// Front-end message 762: empty in this build.
void GM_vFEMessage762_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 763: empty in this build.
void GM_vFEMessage763_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 764: which profile of the card's save was saved last (fn_800A27F4: the save's
// n4D0C0 as MC_LoadOptions last read it; 0 when the save has none).
void GM_vMCGetLastSavedUser(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A27F4();
}

// Front-end message 765: empty in this build.
void GM_vFEMessage765_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 768: makes a new EA Sports Bio file on the card (fn_801252D0); 1 when it
// succeeds, else 0. pArgs[0], pArgs[1] (the card) are handed on but not used.
void GM_vEASBioCreate(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801252D0(pArgs[0].i, pArgs[1].i) == 0;
}

// Front-end message 769: the EA Sports Bio reward message to show (EASBio_eReward, as
// GM_vEASBioCheckReward set it; -1 none).
void GM_vEASBioGetRewardMessage(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_GetCurrentRewardMessage();
}

// Operation 4 of the save kind MC_SetCurrentFileType picked: the space it needs on the card
// (MC_BlocksNeededForSave for that kind: MC_MemoryRequiredForOptions, fn_800A270C, fn_800A2740,
// fn_801255C4). The payload starts with the card's port and slot (core/memcard.h).
s32 MC_CallActionFnMemoryRequired(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[4](pArg);
}

// Picks the save kind the MC_CallActionFn* functions work on (the row of lbl_8018C7D8): 0 the
// options, 1 a profile, 2 a replay, 3 the EA Sports Bio.
void MC_SetCurrentFileType(int n) {
    lbl_80281FFC = n;
}

// Operation 2 of the save kind MC_SetCurrentFileType picked: its files on the card (a count, or
// whether the file is there: fn_800A1758, MC_GetNumUser, fn_800A09EC, fn_80125118). The payload is
// the card (MCOpCard).
s32 MC_CallActionFnNumFilesOnCard(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[2](pArg);
}

// Operation 3 of the save kind MC_SetCurrentFileType picked: whether its file on the card is bad
// data (fn_800A2630, fn_800A26A0, fn_800A2668, fn_8012555C). The payload is an MCCardPos.
s32 MC_CallActionFnDataCorrupt(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[3](pArg);
}

// Operation 1 of the save kind MC_SetCurrentFileType picked: load it from the card (MC_LoadOptions,
// MC_LoadUser, MC_LoadReplay; 0 when it succeeds). The EA Sports Bio's row has none (NULL): kind 3
// must not be loaded this way. The payload is an MCCardPosStr (the card and a name).
s32 MC_CallActionFnLoad(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[1](pArg);
}

// Operation 0 of the save kind MC_SetCurrentFileType picked: save it to the card (MC_SaveOptions,
// MC_SaveUser, MC_SaveReplay, or the EA Sports Bio's record, fn_801251EC; 0 when it succeeds). The
// payload is an MCCardPos.
s32 MC_CallActionFnSave(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[0](pArg);
}

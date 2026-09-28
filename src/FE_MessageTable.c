// FE_MessageTable.c (our name): the menus' message table. The menu UI (uiProcessInterface.c) sends
// its messages here while the front end runs (game type 3): FE_InitGameMessages fills 612 of a table of
// 770 slots with handlers and FE_RunGameMessage calls the one for a message's number with its arguments
// and results. The handlers read and set what the menus show: golfer names, the session's setup,
// the save profile's stats and records, the Create-A-Player choices. TW06 has GetGolferName in
// apt_fe_gamemessages.c. Rounds have their own table (IG_RunGameMessage).

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
void fn_80057438(SaveProfile* pProfile);
void FE_OnGolferHiddenChanged(void);                 // FEgolferanim.c
void fn_8008F80C(s32 p0, s32 p1);       // uiProcessInterface.c
void FE_SetOffscreenBufferRender(u8 bOn);               // FEgolferanim.c
s32  MC_LoadUser(MCCardPosStr* pPos);   // MC.c
s32  MC_LoadReplay(MCCardPos* pPos);      // MC.c: load a replay from the card
void MC_ConnectCard(s32 nPort, s32 nSlot); // MC_Gc.c
s32  MC_NumEASaveGames(s32 nPort, s32 nSlot); // MC_Gc.c
s32  fn_800A1164(s32 nPort, s32 nSlot, char* pName, s32 n);     // MC.c
s32  MC_GetUser(s32 nPort, s32 nSlot, s32 n, char* szOut);     // MC.c: clears szOut first
void fn_8007739C(Replay* pReplay);      // FE_Manager.c
f32  GM_GetBonusProgress(SaveProfile* pProfile);    // GameManager.c
void GM_PgaTourSim_ClearAllSeasons(TourSeason* pTour);    // PGATourSimulation.c
int  GameMode4_GetCurrentEventHoles(void);                 // GameMode4.c: the current ladder event's holes
int  GameMode4_GetCurrentEvent(void);                 // GameMode4.c: the current ladder event
s32  PlayNow_GetNumOpponents(int i);                // GameMode5.c: challenge i's opponent count
s32  PlayNow_GetOpponent(int i, int k);         // GameMode5.c: its opponent k
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
void fn_8012409C(void);                 // gbacable.c
void fn_801240A8(void);                 // gbacable.c
s32  fn_801255C4(s32* pPos);            // EASportsBio.c
u8   PasswordManager_TestPassword(char* szCode);  // PasswordManager.c
void GameMode22_SetNumDrives(s32 n);                // GameMode22.c: sets gGameMode22.nDrives
void GameMode22_SetVariant(s32 n);                // GameMode22.c: sets gGameMode22.nVariant
void GM_SetupCustomHoleSelection(void); // GameManager.c
int  GM_vGetAllTimeRecordsHeld(SaveProfile* pProfile);  // GameManager.c
void PlayNow_SelectGroup(int nId);              // GameMode5.c
s32  PlayNow_GetNumGroups(void);                 // GameMode5.c
char* PlayNow_GetGroupName(int nId);             // GameMode5.c
char* PlayNow_GetGroupDescription(int nId);             // GameMode5.c
void PlayNow_GetRewards(int i, s32* pA, s32* pB, s32* pC);     // GameMode5.c
int  GameMode4_GetNumEventsWon(void);                 // GameMode4.c
void GameMode4_SetEventBonus(s32 n);                // GameMode4.c
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
void fn_80123FF8(void);
void Gba_SetState(s32 v);
s32  fn_8012411C(void);
void fn_80124138(s32 n);
s32  fn_80124174(void);
s32  fn_80124190(void);
s32  fn_801241CC(void);
void fn_801241D4(s32 v);
void fn_8012421C(s32 v);
s32  fn_80124224(void);
void fn_80123CBC(s32 a, s32 b);

// The other files' message handlers in the table (the Create-A-Player screens, the logo editor,
// the PGA TOUR screens, the stats screen, the EA Sports Bio...).
void GM_vGetNumCrAPItems(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemValue(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemColor(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage404_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage405_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCRAPSlider(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCRAPItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage408_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCRAPSlider(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumCrAPGeometries(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage411_Zero(MsgArg* pArgs, MsgArg* pResult);
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
void GM_vCrAPMessage534_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPCameraIdleState(MsgArg* pArgs, MsgArg* pResult);
void GM_vHasMenuGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsMenuGolferReady(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsGolferLoaderIdle(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage539_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetUseProfileCopy(MsgArg* pArgs, MsgArg* pResult);
void GM_vCRAPCreatingLogo(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoPaletteEntry(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoPixel(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetLogoShape(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLogoShape(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage609_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPTriggerAnims(MsgArg* pArgs, MsgArg* pResult);
void GM_vRestartCrAPAnim(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPRenderStateForSubcategory(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCrAPClub(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearGolferCache(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage598_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPOutfit(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPGolferInIdleShot(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPLookInFaceShot(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage566_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSellCrAPItem(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPCategoryCounts(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumCrAPSaleItemsOwned(MsgArg* pArgs, MsgArg* pResult);
void GM_vRandomizeCrAPBody(MsgArg* pArgs, MsgArg* pResult);
void GM_vClearQueuedCrAPAnim(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEquippedCrAPItemInSubcategory(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPAnimInGolferLib(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCrAPItemSlot(MsgArg* pArgs, MsgArg* pResult);
void GM_vRestoreAfterPreview(MsgArg* pArgs, MsgArg* pResult);
void GM_vCrAPMessage741_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsLeapYear(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsCrAPItemRemovable(MsgArg* pArgs, MsgArg* pResult);
void PGALeaderboard_GetRow(MsgArg* pArgs, MsgArg* pResult);
void PGALeaderboard_GetNumRows(MsgArg* pArgs, MsgArg* pResult);
void PGASchedule_GetRow(MsgArg* pArgs, MsgArg* pResult);
void PGASchedule_Build(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_GetLastEventLine(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_IsSeasonOver(MsgArg* pArgs, MsgArg* pResult);
void PGASeasonWrapUp_GetLine(MsgArg* pArgs, MsgArg* pResult);
void PGADriver_ShowCalendar_AdvanceSeason(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_GetTestText(MsgArg* pArgs, MsgArg* pResult);
void PGATourMsg_Get25(MsgArg* pArgs, MsgArg* pResult);
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
void Calendar_DoNothing(MsgArg* pArgs, MsgArg* pResult);
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
void fn_8007E458(int n, MsgArg* pArgs, MsgArg* pResult);
s32  fn_80084FB4(void* pArg);
s32  fn_80084FF8(void* pArg);
s32  fn_80085034(void* pArg);
s32  fn_80085070(void* pArg);
s32  fn_800850AC(void* pArg);

// This file's message handlers, in address order.
void GM_vGetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage4_GetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage6_GetMinPlayersForMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetGameMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetNumPlayers(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage8_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vStartDemo(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage10_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCourse(MsgArg* pArgs, MsgArg* pResult);
void GM_vSelectSingleHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetHoleSet(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferAttribute(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferName(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage17_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vInitCustomRound(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetDemoSetupFlag(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumPlayers(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetPlayerController(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGameMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vMessage25_Returns1(MsgArg* pArgs, MsgArg* pResult);
void GM_vHideCharacter(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetCharState(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferLastName(MsgArg* pArgs, MsgArg* pResult);
void GM_vMCGetUserName(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C3C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C440(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C488(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C48C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C4B8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C4D8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C4F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C594(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C5F0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C634(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C698(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C6E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C748(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C784(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C790(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C79C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C7AC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C7B0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C7EC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C81C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C864(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C8AC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C8F0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C94C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C950(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C988(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C9A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007C9F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CA4C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CACC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CBCC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CC0C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CC90(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CD1C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CD58(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CD98(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CDF0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CDF4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE1C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE20(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE58(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CE7C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007CF4C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D028(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D0E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D160(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D1A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D25C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D260(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D264(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D268(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D26C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D270(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D2A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D2D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D2D4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D380(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D3B4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D3D8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D408(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D40C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D410(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D414(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D418(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D41C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D420(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D424(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D428(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D598(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D6D8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D6DC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D6E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D708(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D76C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D7A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D7E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D810(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D924(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D938(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D964(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D968(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D9D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007D9E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DA6C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAB0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAD0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAD4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DAE8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB04(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB28(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB2C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB30(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB34(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB38(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB3C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DB60(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DBD8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DC50(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DCD4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DD60(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DDEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE10(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE34(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE58(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DE7C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DEA0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DEC4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DEE8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DF0C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007DF30(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E0BC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E0D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E0F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E128(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E174(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E194(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E200(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E204(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E288(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E28C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E354(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E358(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E3D4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E51C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E548(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E574(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E5A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E5CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E5F8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E624(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E650(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E67C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E744(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E748(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E74C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E798(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E79C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E818(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E85C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8B4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8C0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8C4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8DC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E8F0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E904(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E92C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E93C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E9A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E9A4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007E9A8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EA14(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EA70(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EB70(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EC70(MsgArg* pArgs, MsgArg* pResult);
void fn_8007ECFC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007ED88(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EDDC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE40(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE7C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE80(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EE90(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EF54(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EF9C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EFA0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007EFEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F088(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F0D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F2C0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F5CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F640(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F724(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F784(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F7D0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F81C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F87C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007F8A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FA60(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FCC0(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FCD4(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FCE8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FD0C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FEAC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FED8(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FEEC(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF3C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF4C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF6C(MsgArg* pArgs, MsgArg* pResult);
void fn_8007FF8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80080054(MsgArg* pArgs, MsgArg* pResult);
void fn_8008017C(MsgArg* pArgs, MsgArg* pResult);
void fn_800801C0(MsgArg* pArgs, MsgArg* pResult);
void fn_800801D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80080208(MsgArg* pArgs, MsgArg* pResult);
void fn_80080300(MsgArg* pArgs, MsgArg* pResult);
void fn_80080304(MsgArg* pArgs, MsgArg* pResult);
void fn_80080334(MsgArg* pArgs, MsgArg* pResult);
void fn_80080358(MsgArg* pArgs, MsgArg* pResult);
void fn_80080388(MsgArg* pArgs, MsgArg* pResult);
void fn_800804D8(MsgArg* pArgs, MsgArg* pResult);
void fn_800804E4(MsgArg* pArgs, MsgArg* pResult);
void fn_8008052C(MsgArg* pArgs, MsgArg* pResult);
void fn_800805C4(MsgArg* pArgs, MsgArg* pResult);
void fn_800805F0(MsgArg* pArgs, MsgArg* pResult);
void fn_800805F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80080654(MsgArg* pArgs, MsgArg* pResult);
void fn_800807D0(MsgArg* pArgs, MsgArg* pResult);
void fn_800807DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80080828(MsgArg* pArgs, MsgArg* pResult);
void fn_80080878(MsgArg* pArgs, MsgArg* pResult);
void fn_800809F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080AA0(MsgArg* pArgs, MsgArg* pResult);
void fn_80080AD0(MsgArg* pArgs, MsgArg* pResult);
void fn_80080AE8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080BB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C2C(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C60(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C74(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C84(MsgArg* pArgs, MsgArg* pResult);
void fn_80080C98(MsgArg* pArgs, MsgArg* pResult);
void fn_80080CA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80080CC8(MsgArg* pArgs, MsgArg* pResult);
void fn_800810BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800810D8(MsgArg* pArgs, MsgArg* pResult);
void fn_800810F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081158(MsgArg* pArgs, MsgArg* pResult);
void fn_800811E4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081270(MsgArg* pArgs, MsgArg* pResult);
void fn_800812B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800812F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081330(MsgArg* pArgs, MsgArg* pResult);
void fn_80081370(MsgArg* pArgs, MsgArg* pResult);
void fn_800813B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800813F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081430(MsgArg* pArgs, MsgArg* pResult);
void fn_80081470(MsgArg* pArgs, MsgArg* pResult);
void fn_800814B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800814F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081530(MsgArg* pArgs, MsgArg* pResult);
void fn_800815E0(MsgArg* pArgs, MsgArg* pResult);
void fn_80081634(MsgArg* pArgs, MsgArg* pResult);
void fn_80081688(MsgArg* pArgs, MsgArg* pResult);
void fn_800816DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081718(MsgArg* pArgs, MsgArg* pResult);
void fn_80081754(MsgArg* pArgs, MsgArg* pResult);
void fn_80081790(MsgArg* pArgs, MsgArg* pResult);
void fn_800817CC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081808(MsgArg* pArgs, MsgArg* pResult);
void fn_80081844(MsgArg* pArgs, MsgArg* pResult);
void fn_80081880(MsgArg* pArgs, MsgArg* pResult);
void fn_800818BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800818F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081934(MsgArg* pArgs, MsgArg* pResult);
void fn_80081970(MsgArg* pArgs, MsgArg* pResult);
void fn_800819FC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081A54(MsgArg* pArgs, MsgArg* pResult);
void fn_80081B04(MsgArg* pArgs, MsgArg* pResult);
void fn_80081B50(MsgArg* pArgs, MsgArg* pResult);
void fn_80081BD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081BF4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081C18(MsgArg* pArgs, MsgArg* pResult);
void fn_80081CA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80081CF8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081CFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80081D50(MsgArg* pArgs, MsgArg* pResult);
void fn_80081DB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081F98(MsgArg* pArgs, MsgArg* pResult);
void fn_80081FA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80081FBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082608(MsgArg* pArgs, MsgArg* pResult);
void fn_80082620(MsgArg* pArgs, MsgArg* pResult);
void fn_8008266C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082680(MsgArg* pArgs, MsgArg* pResult);
void fn_800826C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80082708(MsgArg* pArgs, MsgArg* pResult);
void fn_80082758(MsgArg* pArgs, MsgArg* pResult);
void fn_80082790(MsgArg* pArgs, MsgArg* pResult);
void fn_800827D0(MsgArg* pArgs, MsgArg* pResult);
void fn_80082800(MsgArg* pArgs, MsgArg* pResult);
void fn_8008281C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082828(MsgArg* pArgs, MsgArg* pResult);
void fn_80082928(MsgArg* pArgs, MsgArg* pResult);
void fn_8008293C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082978(MsgArg* pArgs, MsgArg* pResult);
void fn_8008297C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082980(MsgArg* pArgs, MsgArg* pResult);
void fn_800829D4(MsgArg* pArgs, MsgArg* pResult);
void fn_800829E0(MsgArg* pArgs, MsgArg* pResult);
void fn_800829EC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A10(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A44(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A48(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A50(MsgArg* pArgs, MsgArg* pResult);
void fn_80082A94(MsgArg* pArgs, MsgArg* pResult);
void fn_80082C74(MsgArg* pArgs, MsgArg* pResult);
void fn_80082CA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80082CA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80082CDC(MsgArg* pArgs, MsgArg* pResult);
void fn_80082D14(MsgArg* pArgs, MsgArg* pResult);
void fn_80082D3C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082D98(MsgArg* pArgs, MsgArg* pResult);
void fn_80082DA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80082E5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80082F68(MsgArg* pArgs, MsgArg* pResult);
void fn_80083068(MsgArg* pArgs, MsgArg* pResult);
void fn_8008311C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083354(MsgArg* pArgs, MsgArg* pResult);
void fn_80083358(MsgArg* pArgs, MsgArg* pResult);
void fn_800833A4(MsgArg* pArgs, MsgArg* pResult);
void fn_800833C4(MsgArg* pArgs, MsgArg* pResult);
void fn_800833D0(MsgArg* pArgs, MsgArg* pResult);
void fn_800833F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083414(MsgArg* pArgs, MsgArg* pResult);
void fn_80083430(MsgArg* pArgs, MsgArg* pResult);
void fn_80083480(MsgArg* pArgs, MsgArg* pResult);
void fn_80083494(MsgArg* pArgs, MsgArg* pResult);
void fn_800834DC(MsgArg* pArgs, MsgArg* pResult);
void fn_80083550(MsgArg* pArgs, MsgArg* pResult);
void fn_800835B8(MsgArg* pArgs, MsgArg* pResult);
void fn_800835BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800835C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800835D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083658(MsgArg* pArgs, MsgArg* pResult);
void fn_80083860(MsgArg* pArgs, MsgArg* pResult);
void fn_80083890(MsgArg* pArgs, MsgArg* pResult);
void fn_8008389C(MsgArg* pArgs, MsgArg* pResult);
void fn_800838A0(MsgArg* pArgs, MsgArg* pResult);
void fn_800838A4(MsgArg* pArgs, MsgArg* pResult);
void fn_800838A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800838AC(MsgArg* pArgs, MsgArg* pResult);
void fn_800838B0(MsgArg* pArgs, MsgArg* pResult);
void fn_800838B4(MsgArg* pArgs, MsgArg* pResult);
void fn_800838B8(MsgArg* pArgs, MsgArg* pResult);
void fn_800838BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800838C0(MsgArg* pArgs, MsgArg* pResult);
void fn_800838C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083904(MsgArg* pArgs, MsgArg* pResult);
void fn_80083908(MsgArg* pArgs, MsgArg* pResult);
void fn_8008390C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083910(MsgArg* pArgs, MsgArg* pResult);
void fn_80083914(MsgArg* pArgs, MsgArg* pResult);
void fn_80083918(MsgArg* pArgs, MsgArg* pResult);
void fn_8008391C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083920(MsgArg* pArgs, MsgArg* pResult);
void fn_80083924(MsgArg* pArgs, MsgArg* pResult);
void fn_80083928(MsgArg* pArgs, MsgArg* pResult);
void fn_8008392C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083930(MsgArg* pArgs, MsgArg* pResult);
void fn_80083934(MsgArg* pArgs, MsgArg* pResult);
void fn_80083964(MsgArg* pArgs, MsgArg* pResult);
void fn_80083970(MsgArg* pArgs, MsgArg* pResult);
void fn_80083974(MsgArg* pArgs, MsgArg* pResult);
void fn_80083A44(MsgArg* pArgs, MsgArg* pResult);
void fn_80083A48(MsgArg* pArgs, MsgArg* pResult);
void fn_80083A4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80083BA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80083BC8(MsgArg* pArgs, MsgArg* pResult);
void fn_80083BFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80083E48(MsgArg* pArgs, MsgArg* pResult);
void fn_80083E70(MsgArg* pArgs, MsgArg* pResult);
void fn_80083E94(MsgArg* pArgs, MsgArg* pResult);
void fn_80083EB8(MsgArg* pArgs, MsgArg* pResult);
void fn_80083EBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80083EE0(MsgArg* pArgs, MsgArg* pResult);
void fn_80083F54(MsgArg* pArgs, MsgArg* pResult);
void fn_80083F60(MsgArg* pArgs, MsgArg* pResult);
void fn_80084008(MsgArg* pArgs, MsgArg* pResult);
void fn_8008410C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084158(MsgArg* pArgs, MsgArg* pResult);
void fn_8008415C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084160(MsgArg* pArgs, MsgArg* pResult);
void fn_80084190(MsgArg* pArgs, MsgArg* pResult);
void fn_800841C0(MsgArg* pArgs, MsgArg* pResult);
void fn_80084208(MsgArg* pArgs, MsgArg* pResult);
void fn_8008422C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084258(MsgArg* pArgs, MsgArg* pResult);
void fn_80084288(MsgArg* pArgs, MsgArg* pResult);
void fn_800842AC(MsgArg* pArgs, MsgArg* pResult);
void fn_800842D0(MsgArg* pArgs, MsgArg* pResult);
void fn_80084354(MsgArg* pArgs, MsgArg* pResult);
void fn_80084458(MsgArg* pArgs, MsgArg* pResult);
void fn_8008449C(MsgArg* pArgs, MsgArg* pResult);
void fn_800844E0(MsgArg* pArgs, MsgArg* pResult);
void fn_80084544(MsgArg* pArgs, MsgArg* pResult);
void fn_80084578(MsgArg* pArgs, MsgArg* pResult);
void fn_800845D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084614(MsgArg* pArgs, MsgArg* pResult);
void fn_80084678(MsgArg* pArgs, MsgArg* pResult);
void fn_800846C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800846D4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084704(MsgArg* pArgs, MsgArg* pResult);
void fn_80084750(MsgArg* pArgs, MsgArg* pResult);
void fn_80084754(MsgArg* pArgs, MsgArg* pResult);
void fn_8008478C(MsgArg* pArgs, MsgArg* pResult);
void fn_800847BC(MsgArg* pArgs, MsgArg* pResult);
void fn_800847E0(MsgArg* pArgs, MsgArg* pResult);
void fn_800848E4(MsgArg* pArgs, MsgArg* pResult);
void fn_800848E8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084918(MsgArg* pArgs, MsgArg* pResult);
void fn_80084940(MsgArg* pArgs, MsgArg* pResult);
void fn_80084984(MsgArg* pArgs, MsgArg* pResult);
void fn_800849C8(MsgArg* pArgs, MsgArg* pResult);
void fn_800849F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084AA8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084B88(MsgArg* pArgs, MsgArg* pResult);
void fn_80084BE4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084BE8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084C88(MsgArg* pArgs, MsgArg* pResult);
void fn_80084CFC(MsgArg* pArgs, MsgArg* pResult);
void fn_80084D6C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084DF4(MsgArg* pArgs, MsgArg* pResult);
void fn_80084E7C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084E90(MsgArg* pArgs, MsgArg* pResult);
void fn_80084EC8(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F04(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F08(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F0C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F3C(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F40(MsgArg* pArgs, MsgArg* pResult);
void fn_80084F84(MsgArg* pArgs, MsgArg* pResult);

// The handlers, by message number (FE_InitGameMessages fills it).
#define FE_NUM_MESSAGES 770
MsgHandler gFEMessageHandlers[FE_NUM_MESSAGES];

// Runs front-end message nMsg: the handler in gFEMessageHandlers[nMsg] gets the message's values
// (pArgs) and its answer (pResult). uiProcessInterface.c sends the menu UI's messages here while
// the front end runs (game type 3). The slot is not checked: slots without a handler are NULL.
void FE_RunGameMessage(int nMsg, MsgArg* pArgs, MsgArg* pResult) {
    gFEMessageHandlers[nMsg](pArgs, pResult);
}

// Fills gFEMessageHandlers, the front end's messages by number (called from FE_Manager.c's
// fn_800773F8 as the front end starts): every slot NULL, then 612 handlers of this file and the
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
    gFEMessageHandlers[8] = GM_vMessage8_Empty;
    gFEMessageHandlers[9] = GM_vStartDemo;
    gFEMessageHandlers[10] = GM_vMessage10_Empty;
    gFEMessageHandlers[12] = GM_vSetCourse;
    gFEMessageHandlers[13] = GM_vSelectSingleHole;
    gFEMessageHandlers[14] = GM_vSetHoleSet;
    gFEMessageHandlers[15] = GM_vGetGolferAttribute;
    gFEMessageHandlers[16] = GM_vGetGolferName;
    gFEMessageHandlers[17] = GM_vMessage17_Empty;
    gFEMessageHandlers[18] = GM_vInitCustomRound;
    gFEMessageHandlers[19] = GM_vGetDemoSetupFlag;
    gFEMessageHandlers[20] = GM_vSetPlayerGolfer;
    gFEMessageHandlers[21] = GM_vGetNumPlayers;
    gFEMessageHandlers[22] = GM_vSetPlayerController;
    gFEMessageHandlers[23] = GM_vGetPlayerGolfer;
    gFEMessageHandlers[24] = GM_vGetGameMode;
    gFEMessageHandlers[25] = GM_vMessage25_Returns1;
    gFEMessageHandlers[26] = GM_vHideCharacter;
    gFEMessageHandlers[27] = GM_vSetCharState;
    gFEMessageHandlers[28] = GM_vGetGolferLastName;
    gFEMessageHandlers[29] = GM_vMCGetUserName;
    gFEMessageHandlers[30] = fn_8007C440;
    gFEMessageHandlers[31] = fn_8007C488;
    gFEMessageHandlers[32] = fn_8007C48C;
    gFEMessageHandlers[33] = fn_8007C4B8;
    gFEMessageHandlers[34] = fn_8007C4D8;
    gFEMessageHandlers[35] = fn_8007C4F8;
    gFEMessageHandlers[36] = fn_8007C594;
    gFEMessageHandlers[37] = fn_8007C5F0;
    gFEMessageHandlers[38] = fn_8007C634;
    gFEMessageHandlers[39] = fn_8007C784;
    gFEMessageHandlers[40] = fn_8007C698;
    gFEMessageHandlers[41] = fn_8007C6E4;
    gFEMessageHandlers[42] = fn_8007C748;
    gFEMessageHandlers[43] = fn_8007C790;
    gFEMessageHandlers[44] = fn_8007C79C;
    gFEMessageHandlers[45] = fn_8007C7AC;
    gFEMessageHandlers[46] = LadderMenu_StartEvent;
    gFEMessageHandlers[47] = fn_8007C7B0;
    gFEMessageHandlers[48] = fn_8007C7EC;
    gFEMessageHandlers[49] = fn_8007C81C;
    gFEMessageHandlers[50] = fn_8007C8AC;
    gFEMessageHandlers[51] = fn_8007C8F0;
    gFEMessageHandlers[52] = fn_8007C950;
    gFEMessageHandlers[53] = fn_8007C988;
    gFEMessageHandlers[54] = fn_8007C9A4;
    gFEMessageHandlers[537] = fn_8007C9F8;
    gFEMessageHandlers[55] = fn_8007CA4C;
    gFEMessageHandlers[56] = fn_8007CACC;
    gFEMessageHandlers[57] = fn_8007CBCC;
    gFEMessageHandlers[58] = fn_8007CC0C;
    gFEMessageHandlers[59] = fn_8007CC90;
    gFEMessageHandlers[60] = fn_8007CD1C;
    gFEMessageHandlers[61] = fn_8007CD58;
    gFEMessageHandlers[62] = fn_8007CD98;
    gFEMessageHandlers[63] = fn_8007CDF4;
    gFEMessageHandlers[64] = fn_8007CE1C;
    gFEMessageHandlers[65] = fn_8007CE20;
    gFEMessageHandlers[66] = fn_8007CE58;
    gFEMessageHandlers[67] = fn_8007CE7C;
    gFEMessageHandlers[68] = fn_8007CF4C;
    gFEMessageHandlers[69] = fn_8007D0E0;
    gFEMessageHandlers[70] = fn_8007D160;
    gFEMessageHandlers[71] = fn_8007D1A4;
    gFEMessageHandlers[72] = fn_8007D380;
    gFEMessageHandlers[73] = fn_8007D3B4;
    gFEMessageHandlers[74] = fn_8007D3D8;
    gFEMessageHandlers[75] = fn_8007D41C;
    gFEMessageHandlers[76] = fn_8007D420;
    gFEMessageHandlers[77] = fn_8007D424;
    gFEMessageHandlers[78] = fn_8007D428;
    gFEMessageHandlers[79] = fn_8007D598;
    gFEMessageHandlers[80] = fn_8007D6D8;
    gFEMessageHandlers[81] = fn_8007D6E0;
    gFEMessageHandlers[82] = fn_8007D708;
    gFEMessageHandlers[83] = fn_8007D76C;
    gFEMessageHandlers[84] = fn_8007D7A0;
    gFEMessageHandlers[85] = fn_8007D7E4;
    gFEMessageHandlers[86] = fn_8007D810;
    gFEMessageHandlers[87] = fn_8007D924;
    gFEMessageHandlers[88] = fn_8007D938;
    gFEMessageHandlers[89] = fn_8007D964;
    gFEMessageHandlers[90] = fn_8007D968;
    gFEMessageHandlers[91] = fn_8007D9D0;
    gFEMessageHandlers[92] = fn_8007DA6C;
    gFEMessageHandlers[94] = fn_8007DAD0;
    gFEMessageHandlers[95] = fn_8007DAD4;
    gFEMessageHandlers[96] = fn_8007DAE8;
    gFEMessageHandlers[97] = fn_8007DB04;
    gFEMessageHandlers[98] = fn_8007DB28;
    gFEMessageHandlers[99] = fn_8007DB2C;
    gFEMessageHandlers[100] = fn_8007DB30;
    gFEMessageHandlers[101] = fn_8007DB34;
    gFEMessageHandlers[102] = fn_8007DB38;
    gFEMessageHandlers[103] = fn_8007DB3C;
    gFEMessageHandlers[104] = fn_8007DB60;
    gFEMessageHandlers[105] = fn_8007DBD8;
    gFEMessageHandlers[106] = fn_8007DC50;
    gFEMessageHandlers[107] = fn_8007DCD4;
    gFEMessageHandlers[108] = fn_8007DD60;
    gFEMessageHandlers[109] = fn_8007DDEC;
    gFEMessageHandlers[110] = fn_8007DE10;
    gFEMessageHandlers[111] = fn_8007DE34;
    gFEMessageHandlers[112] = fn_8007DE58;
    gFEMessageHandlers[113] = fn_8007DE7C;
    gFEMessageHandlers[114] = fn_8007DEA0;
    gFEMessageHandlers[115] = fn_8007DEC4;
    gFEMessageHandlers[116] = fn_8007DEE8;
    gFEMessageHandlers[117] = fn_8007DF0C;
    gFEMessageHandlers[118] = fn_8007DF30;
    gFEMessageHandlers[119] = fn_8007E0BC;
    gFEMessageHandlers[120] = fn_8007E0D0;
    gFEMessageHandlers[121] = fn_8007E0F8;
    gFEMessageHandlers[122] = fn_8007E174;
    gFEMessageHandlers[123] = fn_8007E194;
    gFEMessageHandlers[124] = fn_8007E200;
    gFEMessageHandlers[125] = fn_8007E204;
    gFEMessageHandlers[126] = fn_8007E288;
    gFEMessageHandlers[127] = fn_8007E28C;
    gFEMessageHandlers[128] = fn_8007E358;
    gFEMessageHandlers[129] = fn_8007E3D4;
    gFEMessageHandlers[130] = fn_8007E51C;
    gFEMessageHandlers[131] = fn_8007E548;
    gFEMessageHandlers[132] = fn_8007E574;
    gFEMessageHandlers[133] = fn_8007E5A0;
    gFEMessageHandlers[134] = fn_8007E5CC;
    gFEMessageHandlers[135] = fn_8007E5F8;
    gFEMessageHandlers[136] = fn_8007E624;
    gFEMessageHandlers[137] = fn_8007E650;
    gFEMessageHandlers[138] = fn_8007E67C;
    gFEMessageHandlers[139] = fn_8007E744;
    gFEMessageHandlers[140] = fn_8007E748;
    gFEMessageHandlers[141] = fn_8007E74C;
    gFEMessageHandlers[142] = fn_8007E798;
    gFEMessageHandlers[143] = fn_8007E79C;
    gFEMessageHandlers[144] = fn_8007E818;
    gFEMessageHandlers[145] = fn_8007E85C;
    gFEMessageHandlers[146] = fn_8007E8B4;
    gFEMessageHandlers[147] = fn_8007E8C0;
    gFEMessageHandlers[148] = fn_8007E8C4;
    gFEMessageHandlers[149] = fn_8007E8DC;
    gFEMessageHandlers[150] = fn_8007E8F0;
    gFEMessageHandlers[152] = fn_8007E92C;
    gFEMessageHandlers[153] = fn_8007E93C;
    gFEMessageHandlers[154] = fn_8007E9A0;
    gFEMessageHandlers[155] = fn_8007E9A4;
    gFEMessageHandlers[156] = fn_8007E9A8;
    gFEMessageHandlers[157] = fn_8007E9BC;
    gFEMessageHandlers[158] = fn_8007EA14;
    gFEMessageHandlers[159] = fn_8007EA70;
    gFEMessageHandlers[160] = fn_8007EB70;
    gFEMessageHandlers[161] = fn_8007EC70;
    gFEMessageHandlers[162] = fn_8007ECFC;
    gFEMessageHandlers[163] = fn_8007ED88;
    gFEMessageHandlers[164] = fn_8007EDDC;
    gFEMessageHandlers[165] = fn_8007EE40;
    gFEMessageHandlers[166] = fn_8007EE7C;
    gFEMessageHandlers[167] = fn_8007EE90;
    gFEMessageHandlers[168] = fn_8007EF54;
    gFEMessageHandlers[169] = fn_8007EF9C;
    gFEMessageHandlers[170] = fn_8007EFA0;
    gFEMessageHandlers[171] = fn_8007F088;
    gFEMessageHandlers[172] = fn_8007F2C0;
    gFEMessageHandlers[173] = fn_8007F5CC;
    gFEMessageHandlers[174] = fn_8007F640;
    gFEMessageHandlers[175] = fn_8007F724;
    gFEMessageHandlers[176] = fn_8007F784;
    gFEMessageHandlers[177] = fn_8007F7D0;
    gFEMessageHandlers[178] = fn_8007F81C;
    gFEMessageHandlers[179] = fn_8007F87C;
    gFEMessageHandlers[180] = fn_8007F8A0;
    gFEMessageHandlers[181] = fn_8007FA60;
    gFEMessageHandlers[182] = fn_8007FCC0;
    gFEMessageHandlers[183] = fn_8007FCD4;
    gFEMessageHandlers[184] = fn_8007FCE8;
    gFEMessageHandlers[185] = fn_8007FD0C;
    gFEMessageHandlers[186] = fn_8007FEAC;
    gFEMessageHandlers[187] = fn_8007FED8;
    gFEMessageHandlers[188] = fn_8007FEEC;
    gFEMessageHandlers[189] = fn_8007FF3C;
    gFEMessageHandlers[190] = fn_8007FF4C;
    gFEMessageHandlers[191] = fn_8007FF6C;
    gFEMessageHandlers[192] = fn_8007FF8C;
    gFEMessageHandlers[193] = fn_80080054;
    gFEMessageHandlers[194] = fn_8008017C;
    gFEMessageHandlers[195] = fn_800801C0;
    gFEMessageHandlers[196] = fn_800801D4;
    gFEMessageHandlers[197] = fn_80080208;
    gFEMessageHandlers[198] = fn_80080300;
    gFEMessageHandlers[199] = fn_80080304;
    gFEMessageHandlers[200] = fn_80080334;
    gFEMessageHandlers[201] = fn_80080358;
    gFEMessageHandlers[202] = fn_80080388;
    gFEMessageHandlers[203] = fn_800804D8;
    gFEMessageHandlers[204] = fn_800804E4;
    gFEMessageHandlers[205] = fn_800805C4;
    gFEMessageHandlers[206] = fn_800805F0;
    gFEMessageHandlers[207] = fn_800805F4;
    gFEMessageHandlers[208] = fn_80080654;
    gFEMessageHandlers[209] = fn_800807D0;
    gFEMessageHandlers[151] = fn_8007E904;
    gFEMessageHandlers[93] = fn_8007DAB0;
    gFEMessageHandlers[210] = fn_800807DC;
    gFEMessageHandlers[211] = fn_80080828;
    gFEMessageHandlers[212] = fn_80080878;
    gFEMessageHandlers[213] = fn_800809F8;
    gFEMessageHandlers[214] = fn_80080AA0;
    gFEMessageHandlers[215] = fn_80080AD0;
    gFEMessageHandlers[216] = fn_8007D410;
    gFEMessageHandlers[217] = fn_8007D40C;
    gFEMessageHandlers[218] = fn_8007D408;
    gFEMessageHandlers[219] = fn_80080AE8;
    gFEMessageHandlers[220] = fn_80080BB8;
    gFEMessageHandlers[221] = fn_80080C2C;
    gFEMessageHandlers[222] = fn_8007D414;
    gFEMessageHandlers[223] = fn_80080C60;
    gFEMessageHandlers[224] = fn_80080C74;
    gFEMessageHandlers[225] = fn_80080C84;
    gFEMessageHandlers[226] = fn_80080C98;
    gFEMessageHandlers[227] = fn_80080CA8;
    gFEMessageHandlers[228] = fn_80080CC8;
    gFEMessageHandlers[229] = fn_800810BC;
    gFEMessageHandlers[230] = fn_800810D8;
    gFEMessageHandlers[231] = fn_800810F4;
    gFEMessageHandlers[232] = fn_80081158;
    gFEMessageHandlers[233] = fn_800812B0;
    gFEMessageHandlers[234] = fn_80081330;
    gFEMessageHandlers[235] = fn_80081370;
    gFEMessageHandlers[236] = fn_800813B0;
    gFEMessageHandlers[237] = fn_800813F0;
    gFEMessageHandlers[238] = fn_80081430;
    gFEMessageHandlers[239] = fn_80081470;
    gFEMessageHandlers[240] = fn_800814B0;
    gFEMessageHandlers[241] = fn_800814F0;
    gFEMessageHandlers[242] = fn_80081530;
    gFEMessageHandlers[243] = fn_800815E0;
    gFEMessageHandlers[244] = fn_80081634;
    gFEMessageHandlers[245] = fn_80081718;
    gFEMessageHandlers[246] = fn_80081790;
    gFEMessageHandlers[247] = fn_800817CC;
    gFEMessageHandlers[248] = fn_80081808;
    gFEMessageHandlers[249] = fn_80081844;
    gFEMessageHandlers[250] = fn_80081880;
    gFEMessageHandlers[251] = fn_800818BC;
    gFEMessageHandlers[252] = fn_800818F8;
    gFEMessageHandlers[253] = fn_80081934;
    gFEMessageHandlers[254] = fn_80081970;
    gFEMessageHandlers[255] = fn_800819FC;
    gFEMessageHandlers[256] = fn_80081A54;
    gFEMessageHandlers[257] = fn_80081B04;
    gFEMessageHandlers[258] = fn_80081B50;
    gFEMessageHandlers[259] = fn_80081BD4;
    gFEMessageHandlers[260] = fn_80081BF4;
    gFEMessageHandlers[261] = fn_80081C18;
    gFEMessageHandlers[262] = fn_80081CA4;
    gFEMessageHandlers[263] = fn_80081CF8;
    gFEMessageHandlers[264] = fn_80081CFC;
    gFEMessageHandlers[265] = fn_80081D50;
    gFEMessageHandlers[266] = fn_80081DB8;
    gFEMessageHandlers[267] = fn_80081F98;
    gFEMessageHandlers[268] = fn_80081FA8;
    gFEMessageHandlers[269] = fn_80081FBC;
    gFEMessageHandlers[270] = fn_80082608;
    gFEMessageHandlers[271] = fn_80082620;
    gFEMessageHandlers[272] = fn_8008266C;
    gFEMessageHandlers[273] = fn_80082680;
    gFEMessageHandlers[274] = fn_80082708;
    gFEMessageHandlers[275] = fn_80082758;
    gFEMessageHandlers[276] = fn_80082790;
    gFEMessageHandlers[277] = fn_800827D0;
    gFEMessageHandlers[278] = fn_80082800;
    gFEMessageHandlers[279] = fn_8008281C;
    gFEMessageHandlers[280] = fn_80082828;
    gFEMessageHandlers[281] = fn_80082928;
    gFEMessageHandlers[282] = fn_8008293C;
    gFEMessageHandlers[283] = fn_8007D2D4;
    gFEMessageHandlers[284] = fn_80082978;
    gFEMessageHandlers[285] = fn_8008297C;
    gFEMessageHandlers[286] = fn_8007EE80;
    gFEMessageHandlers[287] = fn_8007D25C;
    gFEMessageHandlers[288] = fn_80082980;
    gFEMessageHandlers[289] = fn_8008299C;
    gFEMessageHandlers[290] = fn_800829D4;
    gFEMessageHandlers[291] = fn_800829E0;
    gFEMessageHandlers[292] = fn_8007D268;
    gFEMessageHandlers[293] = fn_8007D028;
    gFEMessageHandlers[294] = fn_800829EC;
    gFEMessageHandlers[295] = fn_80082A10;
    gFEMessageHandlers[296] = fn_80082A44;
    gFEMessageHandlers[297] = fn_80082A48;
    gFEMessageHandlers[298] = fn_80082A4C;
    gFEMessageHandlers[299] = fn_8007D260;
    gFEMessageHandlers[300] = fn_80082A50;
    gFEMessageHandlers[301] = fn_8007D418;
    gFEMessageHandlers[302] = fn_8007D264;
    gFEMessageHandlers[303] = fn_8007D270;
    gFEMessageHandlers[304] = fn_80082A94;
    gFEMessageHandlers[305] = fn_8007E354;
    gFEMessageHandlers[306] = fn_80082C74;
    gFEMessageHandlers[307] = fn_80082CA4;
    gFEMessageHandlers[308] = fn_80082CA8;
    gFEMessageHandlers[309] = fn_80082CDC;
    gFEMessageHandlers[310] = fn_80082D14;
    gFEMessageHandlers[311] = fn_80082D3C;
    gFEMessageHandlers[312] = fn_8007D26C;
    gFEMessageHandlers[313] = fn_8007D2A4;
    gFEMessageHandlers[314] = fn_8007D2D0;
    gFEMessageHandlers[315] = fn_80082D98;
    gFEMessageHandlers[316] = fn_80082DA8;
    gFEMessageHandlers[317] = fn_80082DBC;
    gFEMessageHandlers[318] = fn_80082E10;
    gFEMessageHandlers[319] = fn_80082E5C;
    gFEMessageHandlers[320] = fn_80082F68;
    gFEMessageHandlers[321] = fn_80083068;
    gFEMessageHandlers[322] = fn_8007D6DC;
    gFEMessageHandlers[323] = fn_8008311C;
    gFEMessageHandlers[324] = fn_80083354;
    gFEMessageHandlers[325] = fn_8007CDF0;
    gFEMessageHandlers[326] = fn_8007C94C;
    gFEMessageHandlers[327] = fn_80083358;
    gFEMessageHandlers[328] = fn_800833C4;
    gFEMessageHandlers[329] = fn_800833D0;
    gFEMessageHandlers[330] = fn_800833F4;
    gFEMessageHandlers[331] = fn_80083414;
    gFEMessageHandlers[332] = fn_80083430;
    gFEMessageHandlers[333] = fn_80083480;
    gFEMessageHandlers[334] = fn_80083494;
    gFEMessageHandlers[335] = fn_800834A8;
    gFEMessageHandlers[336] = fn_800834DC;
    gFEMessageHandlers[337] = fn_800834E8;
    gFEMessageHandlers[766] = fn_80083550;
    gFEMessageHandlers[338] = fn_800835B8;
    gFEMessageHandlers[339] = fn_800835BC;
    gFEMessageHandlers[340] = fn_800835C8;
    gFEMessageHandlers[341] = fn_800835D4;
    gFEMessageHandlers[342] = fn_80083658;
    gFEMessageHandlers[343] = fn_80083860;
    gFEMessageHandlers[344] = fn_80083890;
    gFEMessageHandlers[345] = fn_8008389C;
    gFEMessageHandlers[346] = fn_800838A0;
    gFEMessageHandlers[347] = fn_800838A4;
    gFEMessageHandlers[348] = fn_800838A8;
    gFEMessageHandlers[349] = fn_800838AC;
    gFEMessageHandlers[350] = fn_800838B0;
    gFEMessageHandlers[351] = fn_800838B4;
    gFEMessageHandlers[352] = fn_800838B8;
    gFEMessageHandlers[353] = fn_800838BC;
    gFEMessageHandlers[354] = fn_800838C0;
    gFEMessageHandlers[355] = fn_800838C4;
    gFEMessageHandlers[356] = fn_80083904;
    gFEMessageHandlers[357] = fn_80083908;
    gFEMessageHandlers[358] = fn_8008390C;
    gFEMessageHandlers[359] = fn_80083910;
    gFEMessageHandlers[360] = fn_80083914;
    gFEMessageHandlers[361] = fn_80083918;
    gFEMessageHandlers[362] = fn_8008391C;
    gFEMessageHandlers[363] = fn_80083920;
    gFEMessageHandlers[364] = fn_80083924;
    gFEMessageHandlers[365] = fn_80083928;
    gFEMessageHandlers[366] = fn_8008392C;
    gFEMessageHandlers[367] = fn_80083930;
    gFEMessageHandlers[368] = fn_80083934;
    gFEMessageHandlers[369] = fn_80083964;
    gFEMessageHandlers[370] = fn_80083970;
    gFEMessageHandlers[371] = fn_80083974;
    gFEMessageHandlers[372] = fn_80083A44;
    gFEMessageHandlers[374] = fn_80081688;
    gFEMessageHandlers[375] = fn_800811E4;
    gFEMessageHandlers[380] = fn_80083A48;
    gFEMessageHandlers[381] = GM_vGetNumCrAPItems;
    gFEMessageHandlers[382] = GM_vGetCrAPItemValue;
    gFEMessageHandlers[383] = GM_vGetCrAPItemColor;
    gFEMessageHandlers[399] = fn_80083A4C;
    gFEMessageHandlers[404] = GM_vCrAPMessage404_Empty;
    gFEMessageHandlers[405] = GM_vCrAPMessage405_Empty;
    gFEMessageHandlers[406] = GM_vSetCRAPSlider;
    gFEMessageHandlers[407] = GM_vGetCRAPItem;
    gFEMessageHandlers[408] = GM_vCrAPMessage408_Empty;
    gFEMessageHandlers[409] = GM_vGetCRAPSlider;
    gFEMessageHandlers[410] = GM_vGetNumCrAPGeometries;
    gFEMessageHandlers[411] = GM_vCrAPMessage411_Zero;
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
    gFEMessageHandlers[534] = GM_vCrAPMessage534_Empty;
    gFEMessageHandlers[535] = GM_vSetCrAPCameraIdleState;
    gFEMessageHandlers[538] = GM_vHasMenuGolfer;
    gFEMessageHandlers[539] = GM_vCrAPMessage539_Empty;
    gFEMessageHandlers[542] = GM_vGetUseProfileCopy;
    gFEMessageHandlers[547] = GM_vRandomizeCrAPGolferInIdleShot;
    gFEMessageHandlers[554] = GM_vRandomizeCrAPLookInFaceShot;
    gFEMessageHandlers[557] = GM_vGetDPadHeld;
    gFEMessageHandlers[562] = GM_vIsMenuGolferReady;
    gFEMessageHandlers[687] = GM_vIsGolferLoaderIdle;
    gFEMessageHandlers[566] = GM_vCrAPMessage566_Empty;
    gFEMessageHandlers[458] = PGALeaderboard_GetRow;
    gFEMessageHandlers[459] = PGALeaderboard_GetNumRows;
    gFEMessageHandlers[466] = PGASchedule_GetRow;
    gFEMessageHandlers[467] = PGASchedule_Build;
    gFEMessageHandlers[520] = PGATourMsg_GetLastEventLine;
    gFEMessageHandlers[551] = PGATourMsg_IsSeasonOver;
    gFEMessageHandlers[552] = PGASeasonWrapUp_GetLine;
    gFEMessageHandlers[553] = PGADriver_ShowCalendar_AdvanceSeason;
    gFEMessageHandlers[559] = PGATourMsg_GetTestText;
    gFEMessageHandlers[560] = PGATourMsg_Get25;
    gFEMessageHandlers[563] = PGASponsor_SignNext;
    gFEMessageHandlers[742] = PGATourMsg_SetStatsDirty;
    gFEMessageHandlers[743] = PGATourMsg_SetScoresDirty;
    gFEMessageHandlers[472] = fn_80083BFC;
    gFEMessageHandlers[465] = fn_80083BC8;
    gFEMessageHandlers[473] = fn_80083D88;
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
    gFEMessageHandlers[511] = Calendar_DoNothing;
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
    gFEMessageHandlers[518] = fn_80083BA4;
    gFEMessageHandlers[536] = FE_GetNextRealtimeEventInfo;
    gFEMessageHandlers[544] = FE_GetDateTimeIfClockEarly;
    gFEMessageHandlers[543] = GM_vCRAPCreatingLogo;
    gFEMessageHandlers[587] = GM_vSetLogoShape;
    gFEMessageHandlers[588] = GM_vGetLogoShape;
    gFEMessageHandlers[598] = GM_vCrAPMessage598_Empty;
    gFEMessageHandlers[565] = fn_8007D9E4;
    gFEMessageHandlers[567] = fn_8007C3C8;
    gFEMessageHandlers[568] = FE_Sqrt;
    gFEMessageHandlers[569] = LadderMenu_SetNodePos;
    gFEMessageHandlers[573] = LadderMenu_GetNodePos;
    gFEMessageHandlers[575] = LadderMenu_GetEventText;
    gFEMessageHandlers[576] = LadderMenu_MoveCursor;
    gFEMessageHandlers[579] = LadderMenu_GetNodeEventN0;
    gFEMessageHandlers[580] = LadderMenu_GetFirstAndCursorNodePos;
    gFEMessageHandlers[592] = fn_80083E48;
    gFEMessageHandlers[593] = fn_80083E70;
    gFEMessageHandlers[594] = fn_80083E94;
    gFEMessageHandlers[595] = fn_80083EB8;
    gFEMessageHandlers[596] = LadderMenu_GetAngleBetween;
    gFEMessageHandlers[599] = fn_800816DC;
    gFEMessageHandlers[600] = fn_80081270;
    gFEMessageHandlers[605] = TrophyRoom_GetTourTrophy;
    gFEMessageHandlers[606] = TrophyRoom_GetTourTrophyText;
    gFEMessageHandlers[608] = fn_8007F0D0;
    gFEMessageHandlers[609] = GM_vCrAPMessage609_Empty;
    gFEMessageHandlers[610] = fn_8007E128;
    gFEMessageHandlers[611] = fn_80083EBC;
    gFEMessageHandlers[612] = fn_80083F60;
    gFEMessageHandlers[623] = fn_80084008;
    gFEMessageHandlers[624] = fn_8008410C;
    gFEMessageHandlers[630] = fn_80083F54;
    gFEMessageHandlers[629] = fn_80083EE0;
    gFEMessageHandlers[633] = fn_80084160;
    gFEMessageHandlers[653] = fn_80084190;
    gFEMessageHandlers[654] = fn_800841C0;
    gFEMessageHandlers[748] = fn_80084208;
    gFEMessageHandlers[618] = fn_80084158;
    gFEMessageHandlers[619] = fn_8008415C;
    gFEMessageHandlers[607] = TrophyRoom_CountEventsInMonth;
    gFEMessageHandlers[613] = TrophyRoom_GetPlaceholderText;
    gFEMessageHandlers[614] = fn_8008422C;
    gFEMessageHandlers[615] = fn_80084258;
    gFEMessageHandlers[616] = fn_80084288;
    gFEMessageHandlers[617] = fn_800842AC;
    gFEMessageHandlers[620] = fn_8008052C;
    gFEMessageHandlers[621] = GM_vIsProfileLogoMade;
    gFEMessageHandlers[622] = GM_vSaveLogo;
    gFEMessageHandlers[627] = fn_800842D0;
    gFEMessageHandlers[628] = fn_80084354;
    gFEMessageHandlers[632] = fn_800826C4;
    gFEMessageHandlers[637] = TrophyRoom_GetIndexMod4;
    gFEMessageHandlers[638] = TrophyRoom_GetMedalDate;
    gFEMessageHandlers[640] = TrophyRoom_GetLadderAward;
    gFEMessageHandlers[641] = TrophyRoom_GetLadderEventCourse;
    gFEMessageHandlers[643] = fn_80084458;
    gFEMessageHandlers[644] = fn_8008449C;
    gFEMessageHandlers[645] = fn_800844E0;
    gFEMessageHandlers[646] = fn_80084544;
    gFEMessageHandlers[647] = fn_80084578;
    gFEMessageHandlers[648] = fn_800845D4;
    gFEMessageHandlers[649] = fn_80084614;
    gFEMessageHandlers[650] = fn_80084678;
    gFEMessageHandlers[655] = fn_800846C8;
    gFEMessageHandlers[657] = GM_vSetCrAPTriggerAnims;
    gFEMessageHandlers[658] = fn_80084704;
    gFEMessageHandlers[663] = GM_vRestartCrAPAnim;
    gFEMessageHandlers[664] = GM_vSetCrAPRenderStateForSubcategory;
    gFEMessageHandlers[665] = GM_vSetCrAPClub;
    gFEMessageHandlers[666] = fn_8008478C;
    gFEMessageHandlers[667] = fn_80084750;
    gFEMessageHandlers[668] = fn_80084754;
    gFEMessageHandlers[671] = fn_800847BC;
    gFEMessageHandlers[672] = fn_800847E0;
    gFEMessageHandlers[670] = TrophyRoom_GetRTEAwardIcon;
    gFEMessageHandlers[674] = TrophyRoom_GetAwardEarnedText;
    gFEMessageHandlers[675] = fn_800848E4;
    gFEMessageHandlers[678] = fn_800848E8;
    gFEMessageHandlers[679] = fn_80084918;
    gFEMessageHandlers[680] = fn_80084940;
    gFEMessageHandlers[681] = fn_80084984;
    gFEMessageHandlers[682] = fn_800849C8;
    gFEMessageHandlers[683] = PGASponsor_GetItemBonus;
    gFEMessageHandlers[684] = fn_800849F8;
    gFEMessageHandlers[685] = GM_vClearGolferCache;
    gFEMessageHandlers[688] = fn_800846D4;
    gFEMessageHandlers[689] = GM_vRandomizeCrAPGolfer;
    gFEMessageHandlers[690] = GM_vRandomizeCrAPOutfit;
    gFEMessageHandlers[686] = fn_8007C864;
    gFEMessageHandlers[691] = LadderMenu_GetNodeState;
    gFEMessageHandlers[692] = fn_80084AA8;
    gFEMessageHandlers[693] = Calendar_IsSherwoodTargetEntered;
    gFEMessageHandlers[694] = Calendar_SetPlayNowFlag;
    gFEMessageHandlers[695] = Calendar_GetPlayNowFlag;
    gFEMessageHandlers[696] = fn_80084B88;
    gFEMessageHandlers[697] = PGASponsor_PickStartingSponsor;
    gFEMessageHandlers[698] = PGASponsor_CollectItems;
    gFEMessageHandlers[699] = PGASponsor_GetItem;
    gFEMessageHandlers[700] = PGASponsor_GetName;
    gFEMessageHandlers[701] = PGATourWins_GetDetails;
    gFEMessageHandlers[704] = fn_800833A4;
    gFEMessageHandlers[708] = Calendar_GetRTEDescription;
    gFEMessageHandlers[711] = GM_vSellCrAPItem;
    gFEMessageHandlers[712] = GM_vGetCrAPCategoryCounts;
    gFEMessageHandlers[713] = GM_vGetNumCrAPSaleItemsOwned;
    gFEMessageHandlers[714] = GM_vRandomizeCrAPBody;
    gFEMessageHandlers[715] = fn_80084BE4;
    gFEMessageHandlers[718] = PGATourMsg_CheckAdvanceTournament;
    gFEMessageHandlers[720] = LadderMenu_GetEventName;
    gFEMessageHandlers[721] = fn_80081754;
    gFEMessageHandlers[722] = fn_800812F0;
    gFEMessageHandlers[723] = fn_8007EFEC;
    gFEMessageHandlers[727] = fn_80084BE8;
    gFEMessageHandlers[725] = GM_vClearQueuedCrAPAnim;
    gFEMessageHandlers[726] = fn_80084C88;
    gFEMessageHandlers[728] = GM_vGetEquippedCrAPItemInSubcategory;
    gFEMessageHandlers[730] = GM_vIsCrAPAnimInGolferLib;
    gFEMessageHandlers[736] = fn_80084CFC;
    gFEMessageHandlers[737] = fn_80084D6C;
    gFEMessageHandlers[739] = GM_vGetCrAPItemSlot;
    gFEMessageHandlers[740] = GM_vRestoreAfterPreview;
    gFEMessageHandlers[741] = GM_vCrAPMessage741_Empty;
    gFEMessageHandlers[746] = fn_80084DF4;
    gFEMessageHandlers[749] = PGATourMsg_DidUserQuit;
    gFEMessageHandlers[751] = fn_80084E7C;
    gFEMessageHandlers[754] = PGASponsor_GetSlot;
    gFEMessageHandlers[755] = PGASponsor_GetTotalItemBonus;
    gFEMessageHandlers[756] = GM_vIsLeapYear;
    gFEMessageHandlers[757] = GM_vIsCrAPItemRemovable;
    gFEMessageHandlers[760] = GM_vPreviewItem;
    gFEMessageHandlers[759] = fn_80084E90;
    gFEMessageHandlers[761] = fn_80084EC8;
    gFEMessageHandlers[762] = fn_80084F04;
    gFEMessageHandlers[763] = fn_80084F08;
    gFEMessageHandlers[764] = fn_80084F0C;
    gFEMessageHandlers[765] = fn_80084F3C;
    gFEMessageHandlers[768] = fn_80084F40;
    gFEMessageHandlers[769] = fn_80084F84;
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
void GM_vMessage8_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 3: sets up the session's players for the game the menus start (fn_80079AD4: CPU
// players, loaded profiles, golfers and bags). DiscCheck.c also calls it directly, with no values.
void GM_vSetupPlayers(MsgArg* pArgs, MsgArg* pResult) {
    fn_80079AD4();
}

// Front-end message 9: the menus start the demo (gSession.bDemo) in game mode pArgs[0]:
// lbl_801D7148.b11 set, the mode set up (GM_SetModeType), the fade to black started and the front
// end's audio stopped (Gaud_ExitFE).
void GM_vStartDemo(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b11 = 1;
    GM_SetModeType((u8)pArgs[0].i);
    gSession.bDemo = 1;
    lbl_801D87C0.bFadeToBlack = 1;
    Gaud_ExitFE();
}

// Front-end message 10: empty in this build.
void GM_vMessage10_Empty(MsgArg* pArgs, MsgArg* pResult) {
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
// golfer's from the current profile, fn_80077A80).
void GM_vGetGolferAttribute(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord = fn_80077A80(pArgs[0].i);

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

    pRecord = fn_80077A80(nGolfer);
    bNick = strcmp(pRecord->szNick, "NA") != 0 && strlen(pRecord->szNick) > 1 && nGolfer != 18;
    if (bNick != 0) {
        sprintf(szName, "%s \"%s\" %s", pRecord->szFirst, pRecord->szNick, pRecord->szLast);
    } else {
        sprintf(szName, "%s %s", pRecord->szFirst, pRecord->szLast);
    }
}

// Front-end message 17: empty in this build.
void GM_vMessage17_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Front-end message 18: a custom round (gpGame->b136) of four holes: course 8's hole 17, course 5's
// hole 4, course 17's hole 2 and course 18's hole 9, the rest unselected. With both demo session
// flags (0x4000 and 0x8000) set instead: commentary off (options.a0[4] 0) and all 18 slots course
// 0's hole 18.
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
    gpGame->b136 = 1;
}

// Front-end message 19: 1 when session flag 0x4000 (the demo set-up) is set.
void GM_vGetDemoSetupFlag(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (gSession.uFlags >> 14) & 1;
}

// Front-end message 20: player pArgs[0] plays golfer pArgs[1] (Session_SetGolfer). In Play Now
// (game mode 5) it first sets lbl_80281ED4->b11703, which keeps fn_80079AD4 from giving player 0
// the created golfer.
void GM_vSetPlayerGolfer(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 5) {
        lbl_80281ED4->b11703 = 1;
    }
    Session_SetGolfer(pArgs[1].i, pArgs[0].i);
}

// Front-end message 21: the session's number of players.
void GM_vGetNumPlayers(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nNumPlayers;
}

// Front-end message 22: player pArgs[0] uses controller pArgs[1]. -1 and 9 give 9 (CONTROLLER_CPU);
// a real controller is also marked in lbl_801D87C0.a2C.
void GM_vSetPlayerController(MsgArg* pArgs, MsgArg* pResult) {
    s32 nController;

    nController = pArgs[1].i;
    if (nController == -1 || nController == 9) {
        gSession.nController[pArgs[0].i] = 9;
        return;
    }
    gSession.nController[pArgs[0].i] = nController;
    lbl_801D87C0.a2C[pArgs[1].i] = 1;
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
void GM_vMessage25_Returns1(MsgArg* pArgs, MsgArg* pResult) {
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
// from the current profile, fn_80077A80).
void GM_vGetGolferLastName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pResult->p)->pStr, fn_80077A80(pArgs[0].i)->szLast);
}

// Front-end message 29: profile name pArgs[3] (0..3) of the save on the memory card in port
// pArgs[1], slot pArgs[2] (MCCardState.aszName, from MC_GetMC) into the string pArgs[0].
void GM_vMCGetUserName(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[1].i, pArgs[2].i);
    strcpy(((MsgString*)pArgs[0].p)->pStr, state.aszName[pArgs[3].i]);
}

// All four of them.
void fn_8007C3C8(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    strcpy(((MsgString*)pArgs[2].p)->pStr, state.aszName[0]);
    strcpy(((MsgString*)pArgs[3].p)->pStr, state.aszName[1]);
    strcpy(((MsgString*)pArgs[4].p)->pStr, state.aszName[2]);
    strcpy(((MsgString*)pArgs[5].p)->pStr, state.aszName[3]);
}

// The card's state: a flag of it, or its free space.
void fn_8007C440(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags & MC_CARD_PRESENT) >> 1;
}

void fn_8007C488(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C48C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008F80C(pArgs[0].i, (u8)pArgs[1].i);
}

void fn_8007C4B8(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
}

void fn_8007C4D8(MsgArg* pArgs, MsgArg* pResult) {
    MC_Disconnect();
}

// MC_LoadUser with a card, a profile slot and a string, then the slot's profile is marked loaded
// (the test never fails: the result is 1 or an error, never 0).
void fn_8007C4F8(MsgArg* pArgs, MsgArg* pResult) {
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
        fn_80077808(pArgs[2].i);
        lbl_801D7148.aLoaded[pArgs[2].i] = 1;
    }
}

void fn_8007C594(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007C5F0(MsgArg* pArgs, MsgArg* pResult) {
    int n;

    n = pArgs[0].i;
    if (n == 11) {
        Gaud_PlayUISound((Misc_RandFunc(0) & 7) + 11);
    } else {
        Gaud_PlayUISound(n);
    }
}

void fn_8007C634(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007C698(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = MC_FormatCard(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// Every player's tee set: 1, 2 or 3 picks tee set 2, 1 or 0.
void fn_8007C6E4(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007C748(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)gpSaveData[pArgs[0].i].createdGolfer.bAvailable != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8007C784(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 30;
}

void fn_8007C790(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007C79C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = pArgs[0].i + 30;
}

void fn_8007C7AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C7B0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = (s8)fn_80077A80(pArgs[0].i)->bAvailable;
}

void fn_8007C7EC(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    if (n == 9) {
        pResult->i = 1;
        return;
    }
    pResult->i = lbl_801D87C0.a1[n];
}

void fn_8007C81C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags & 8) >> 3;
}

void fn_8007C864(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = (state.uFlags & MC_CARD_WRONGDEVICE) >> 4;
}

void fn_8007C8AC(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.nFreeBlocks;
}

// The game save's size on the card.
void fn_8007C8F0(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    fn_80084FF0(0);
    MC_ConnectCard(pos.nPort, pos.nSlot);
    pResult->i = fn_80084FB4(&pos);
    MC_Disconnect();
}

void fn_8007C94C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007C950(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_IsMultitapPluggedIn(pArgs[0].i);
}

void fn_8007C988(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D880C.n4 = pArgs[0].i;
    lbl_801D880C.n0 = 0;
}

void fn_8007C9A4(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007C9F8(MsgArg* pArgs, MsgArg* pResult) {
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

// The on/off options: the menus send and read 1 for on and 2 for off.
void fn_8007CA4C(MsgArg* pArgs, MsgArg* pResult) {
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

// Option a0[4]: the menus' choices 1..6 are the values 5, 0, 1, 2, 3, 4; it is passed on times 0.2.
void fn_8007CACC(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007CBCC(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bGimmes = 1;
        return;
    case 2:
        gSession.options.bGimmes = 0;
        return;
    }
}

void fn_8007CC0C(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007CC90(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007CD1C(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bGimmes) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// A string's width, scaled.
void fn_8007CD58(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

// Save kind 1's size on the card.
void fn_8007CD98(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_ConnectCard(pos.nPort, pos.nSlot);
    fn_80084FF0(1);
    pResult->i = fn_80084FB4(&pos);
    MC_Disconnect();
}

void fn_8007CDF0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007CDF4(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_SelectChallenge(PlayNow_GetGroupFirstChallenge(pArgs[0].i));
}

void fn_8007CE1C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007CE20(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80077A80(pArgs[0].i)->nModelID;
}

void fn_8007CE58(MsgArg* pArgs, MsgArg* pResult) {
    GameMode4_SetEventBonus(pArgs[0].i);
}

// Find golfer pArgs[0]'s bio (golfer 1 has golfer 0's) and hand back its index and its numbers.
void fn_8007CE7C(MsgArg* pArgs, MsgArg* pResult) {
    int i;
    s32 nId;

    nId = pArgs[0].i;
    if (nId == 1) {
        nId = 0;
    }
    for (i = 0; i < FE_NUM_BIOS; i++) {
        if (nId == lbl_80281EC8[i].nId) break;
    }
    pResult->i = i;
    *(s32*)pArgs[1].p = lbl_80281EC8[i].a34[0];
    *(s32*)pArgs[2].p = lbl_80281EC8[i].a34[1];
    *(s32*)pArgs[3].p = lbl_80281EC8[i].a34[2];
    *(s32*)pArgs[4].p = lbl_80281EC8[i].a34[3];
    *(s32*)pArgs[5].p = lbl_80281EC8[i].a34[4];
    *(s32*)pArgs[6].p = lbl_80281EC8[i].a34[5];
    *(s32*)pArgs[7].p = lbl_80281EC8[i].n68;
}

// Bio pArgs[0]'s texts, and its course's name.
void fn_8007CF4C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nBio = pArgs[0].i;

    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80281EC8[nBio].sz4);
    strcpy(((MsgString*)pArgs[2].p)->pStr, lbl_80281EC8[nBio].sz24);
    strcpy(((MsgString*)pArgs[3].p)->pStr, lbl_80281EC8[nBio].sz4C);
    strcpy(((MsgString*)pArgs[4].p)->pStr, lbl_80281EC8[nBio].sz70);
    if (lbl_80281EC8[nBio].nCourse == -1) {
        strcpy(((MsgString*)pArgs[5].p)->pStr, "N/A");
        return;
    }
    strcpy(((MsgString*)pArgs[5].p)->pStr, lbl_80191990[lbl_80281EC8[nBio].nCourse]);
}

// Bio pArgs[0]'s five lines of text; missing lines are blank.
void fn_8007D028(MsgArg* pArgs, MsgArg* pResult) {
    char szText[sizeof(lbl_80281EC8->sz98)];
    int i;
    char* pLine;

    strcpy(szText, lbl_80281EC8[pArgs[0].i].sz98);
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

// Give the created golfer being edited model pArgs[1], in the profile and in its golfer record
// (golfers 30 on are the slots' created golfers).
void fn_8007D0E0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int nSlot = lbl_80281ED4->nSlot;

    pProfile->createdGolfer.nModelID = pArgs[1].i;
    // fake match: reads nSlot again rather than using the local
    gSession.nGolfer[nSlot] = (u8)(lbl_80281ED4->nSlot + FIRST_CREATED_GOLFER);
    gGolferTable[gSession.nGolfer[nSlot]].nModelID = pProfile->createdGolfer.nModelID;
}

// The string at szName + 10 of slot pArgs[0]'s profile (its own field is not proven: see save.h).
void fn_8007D160(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, &gpSaveData[pArgs[0].i].szName[10]);
}

// Set that string to pArgs[1], less its trailing spaces.
void fn_8007D1A4(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    strcpy(&gpSaveData[pArgs[0].i].szName[10], ((MsgString*)pArgs[1].p)->pStr);
    i = strlen(&gpSaveData[pArgs[0].i].szName[10]) - 1;
    while (gpSaveData[pArgs[0].i].szName[10 + i] == ' ') {
        i--;
    }
    gpSaveData[pArgs[0].i].szName[10 + i + 1] = '\0';
}

void fn_8007D25C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D260(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D264(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D268(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D26C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D270(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        lbl_80281ED4->b11702 = 1;
    } else {
        lbl_80281ED4->b11702 = 0;
    }
}

void fn_8007D2A4(MsgArg* pArgs, MsgArg* pResult) {
    if (lbl_80281ED4->b11702 != 0) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_8007D2D0(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether that string has anything but spaces.
void fn_8007D2D4(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007D380(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = FE_GetCurrentProfile()->nCurrentCash;
}

void fn_8007D3B4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80057438(FE_GetCurrentProfile());
}

void fn_8007D3D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameMode4_GetNumEventsWon();
}

void fn_8007D408(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D40C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D410(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D414(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D418(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D41C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D420(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D424(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether golfer pArgs[1] can be picked: 1 when it is unlocked (by any profile or a cheat code, or
// it is a created golfer), 0 when it is locked, -1 when it is not available at all; always 1
// while lbl_80281ED4->b11702 is set.
void fn_8007D428(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    if (lbl_80281ED4->b11702 != 0) {
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

// Whether course pArgs[1] can be picked: 1 when a loaded profile or a cheat code has unlocked it
// (course 23 always), else 0.
void fn_8007D598(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007D6D8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6DC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007D6E0(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        pResult->i = 0;
    } else {
        pResult->i = pArgs[0].i + 30;
    }
}

// Look a value up in the prize table's ranges (-1: in none).
void fn_8007D708(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007D76C(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;

    pProfile = FE_GetCurrentProfile();
    pProfile->nCurrentCash = pArgs[1].i;
}

// Set an attribute of the created golfer being worked on.
void fn_8007D7A0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile;
    int nAttr;

    nAttr = pArgs[1].i;
    pProfile = FE_GetCurrentProfile();
    pProfile->createdGolfer.attr[nAttr] = pArgs[2].i;
}

void fn_8007D7E4(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = 2;
    Gaud_StopMusic();
}

// Show golfer pArgs[0] (FE_setupStreaming), then give the profile's player (unless it is player 0) the
// first n0 (0..3) that no player up to and including it with the same golfer model has.
void fn_8007D810(MsgArg* pArgs, MsgArg* pResult) {
    u8 abFree[4] = {1, 1, 1, 1};
    int i;
    u32 n;
    GolferRecord* pMine;
    GolferRecord* pOther;

    FE_GetCurrentProfile();
    FE_setupStreaming(pArgs[0].i, pArgs[1].i, pArgs[2].i);
    for (i = 0; i <= lbl_80281ED4->nSlot; i++) {
        pMine = fn_80077A80(gSession.nGolfer[lbl_80281ED4->nSlot]);
        pOther = fn_80077A80(gSession.nGolfer[i]);
        if (pMine->nModelID == pOther->nModelID) {
            abFree[gSession.aProfile[i].n0] = 0;
        }
    }
    for (n = 0; n < 4; n++) {
        if (abFree[n] && lbl_80281ED4->nSlot > 0) {
            gSession.aProfile[lbl_80281ED4->nSlot].n0 = n;
            break;
        }
    }
    Session_SetupProfiles();
}

void fn_8007D924(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nSplitScreen = pArgs[0].i;
}

void fn_8007D938(MsgArg* pArgs, MsgArg* pResult) {
    GM_SelectSingleHole((u8)pArgs[0].i - 1);
}

void fn_8007D964(MsgArg* pArgs, MsgArg* pResult) {
}

// Work on slot pArgs[0]'s profile; the golfer shown, if it is golfer 7 or 29, takes that
// profile's look.
void fn_8007D968(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->nSlot = pArgs[0].i;
    if (gpCrAPState->pB4->pChar != NULL &&
        (gpCrAPState->pB4->pChar->nGolferId == 7 || gpCrAPState->pB4->pChar->nGolferId == 29)) {
        Character_ApplyCrAPSettings(gpCrAPState->pB4->pChar, &FE_GetCurrentProfile()->choices);
    }
}

void fn_8007D9D0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->nSlot;
}

// Move on to the next slot with a loaded profile: its number, or -1 past the last player.
void fn_8007D9E4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->nSlot++;
    while (lbl_80281ED4->nSlot < 4 && gpSaveData[lbl_80281ED4->nSlot].bActive == 0) {
        lbl_80281ED4->nSlot++;
    }
    if (lbl_80281ED4->nSlot >= 4 || lbl_80281ED4->nSlot + 1 > gSession.nNumPlayers) {
        pResult->i = -1;
        return;
    }
    pResult->i = lbl_80281ED4->nSlot;
}

// Slot pArgs[0]'s profile name.
void fn_8007DA6C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[pArgs[0].i].szName);
}

void fn_8007DAB0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].bActive;
}

void fn_8007DAD0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DAD4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D87C0.n38;
}

void fn_8007DAE8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D87C0.a2C[pArgs[0].i];
}

void fn_8007DB04(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    if (n < 4) {
        lbl_801D87C0.a2C[n] = 0;
    }
}

void fn_8007DB28(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB2C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB30(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB34(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007DB38(MsgArg* pArgs, MsgArg* pResult) {
}

// A slot's profile's numbers.
void fn_8007DB3C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nRounds;
}

// Slot pArgs[0]'s strokes per stroke-play round.
void fn_8007DB60(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nStrokeRounds != 0) {
        pResult->f = (f32)pProfile->nStrokeRoundStrokes / (f32)pProfile->nStrokeRounds;
        return;
    }
    pResult->f = 0.0f;
}

void fn_8007DBD8(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nPuttHoles != 0) {
        pResult->f = (f32)pProfile->nPutts / (f32)pProfile->nPuttHoles;
        return;
    }
    pResult->f = 0.0f;
}

// Slot pArgs[0]'s average drive.
void fn_8007DC50(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nDrives != 0) {
        pResult->i = (f32)pProfile->nDriveDistance / (f32)pProfile->nDrives;
        return;
    }
    pResult->i = 0;
}

void fn_8007DCD4(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nFairways != 0) {
        pResult->i = 100.0f * (f32)pProfile->nFairwaysHit / (f32)pProfile->nFairways;
        return;
    }
    pResult->i = 0;
}

void fn_8007DD60(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    if (pProfile->nHoles != 0) {
        pResult->i = 100.0f * ((f32)pProfile->nGreensHit / (f32)pProfile->nHoles);
        return;
    }
    pResult->i = 0;
}

void fn_8007DDEC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nLongestDrive;
}

void fn_8007DE10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nLongestPutt;
}

void fn_8007DE34(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nHolesInOne;
}

void fn_8007DE58(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nAlbatrosses;
}

void fn_8007DE7C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nEagles;
}

void fn_8007DEA0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nBirdies;
}

void fn_8007DEC4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nPars;
}

void fn_8007DEE8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nBogeys;
}

void fn_8007DF0C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nDoubleBogeys;
}

// Step the profile's player's n0 on (0..3, wrapping) to the next one that no player up to and
// including it with the same golfer model has (fn_8007D810), stopping if it comes round to where
// it started; then Character_RequestClothesUpdateFE for the golfer shown.
void fn_8007DF30(MsgArg* pArgs, MsgArg* pResult) {
    u8 abFree[4] = {1, 1, 1, 1};
    int i;
    s8 nStart;
    GolferRecord* pMine;
    GolferRecord* pOther;

    for (i = 0; i <= lbl_80281ED4->nSlot; i++) {
        pMine = fn_80077A80(gSession.nGolfer[lbl_80281ED4->nSlot]);
        pOther = fn_80077A80(gSession.nGolfer[i]);
        if (pMine->nModelID == pOther->nModelID) {
            abFree[gSession.aProfile[i].n0] = 0;
        }
    }
    nStart = gSession.aProfile[lbl_80281ED4->nSlot].n0;
    gSession.aProfile[lbl_80281ED4->nSlot].n0++;
    if (gSession.aProfile[lbl_80281ED4->nSlot].n0 > 3) {
        gSession.aProfile[lbl_80281ED4->nSlot].n0 = 0;
    }
    while (!abFree[gSession.aProfile[lbl_80281ED4->nSlot].n0]) {
        gSession.aProfile[lbl_80281ED4->nSlot].n0++;
        if (gSession.aProfile[lbl_80281ED4->nSlot].n0 > 3) {
            gSession.aProfile[lbl_80281ED4->nSlot].n0 = 0;
        }
        if (gSession.aProfile[lbl_80281ED4->nSlot].n0 == nStart) break;
    }
    Character_RequestClothesUpdateFE(gpCrAPState->pB4->nIndex);
}

void fn_8007E0BC(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nPinSet = pArgs[0].i;
}

void fn_8007E0D0(MsgArg* pArgs, MsgArg* pResult) {
    fn_800142A4(pArgs[0].i);
}

void fn_8007E0F8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aAward[pArgs[1].i].bWon;
}

void fn_8007E128(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

    pResult->i = pProfile->aAward[pArgs[0].i + 23].bWon;
}

void fn_8007E174(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gEarningsTable.a9B4[pArgs[0].i];
}

// The mulligan rule: none in game mode 7, any number in mode 9, else the one picked.
void fn_8007E194(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007E200(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether club pArgs[1] is in player pArgs[0]'s golfer's bag; the player's bag is also set to the
// golfer's.
void fn_8007E204(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord = fn_80077A80(gSession.nGolfer[pArgs[0].i]);
    s32 n = pArgs[1].i;

    n = pRecord->uBagMask & (1 << n);   // fake match: one local for the club and the result (register order)
    pResult->i = n;
    gSession.uBag[pArgs[0].i] = pRecord->uBagMask;
}

void fn_8007E288(MsgArg* pArgs, MsgArg* pResult) {
}

// Put club pArgs[1] into player pArgs[0]'s bag or take it out (not with session flag 0x4000). A
// created golfer, or any golfer in game mode 4, keeps the new bag in its record.
void fn_8007E28C(MsgArg* pArgs, MsgArg* pResult) {
    GolferRecord* pRecord;
    s32 nClub;

    pRecord = fn_80077A80(gSession.nGolfer[pArgs[0].i]);
    nClub = pArgs[1].i;
    if (!(gSession.uFlags & 0x4000)) {
        gSession.uBag[pArgs[0].i] ^= 1 << nClub;
        if (gSession.nGolfer[pArgs[0].i] >= FIRST_CREATED_GOLFER || Game_GetMode() == 4) {
            Game_GetMode();     // the original calls it again and ignores the result
            pRecord->uBagMask = gSession.uBag[pArgs[0].i];
        }
    }
}

void fn_8007E354(MsgArg* pArgs, MsgArg* pResult) {
}

// The wind option: menu choices 1-4 are wind settings 0-3 (calm to gusty).
void fn_8007E358(MsgArg* pArgs, MsgArg* pResult) {
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

// Option a7[0]: menu choice 1 turns it on, 2 off.
void fn_8007E3D4(MsgArg* pArgs, MsgArg* pResult) {
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

// Record nKind, place pArgs[1]: its value, and its holder's name into pArgs[2]. pArgs[0] is the
// course; past the last course it is the all-time records.
void fn_8007E458(int nKind, MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i < NUM_COURSE_RECORDS) {
        pResult->i = gSession.aCourseRecord[pArgs[0].i].aRecord[nKind][pArgs[1].i].nValue;
        strcpy(((MsgString*)pArgs[2].p)->pStr,
               gSession.aCourseRecord[pArgs[0].i].aRecord[nKind][pArgs[1].i].szName);
        return;
    }
    pResult->i = gSession.recA[nKind][pArgs[1].i].nValue;
    strcpy(((MsgString*)pArgs[2].p)->pStr, gSession.recA[nKind][pArgs[1].i].szName);
}

void fn_8007E51C(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(0, pArgs, pResult);
}

void fn_8007E548(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(1, pArgs, pResult);
}

void fn_8007E574(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(2, pArgs, pResult);
}

void fn_8007E5A0(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(3, pArgs, pResult);
}

void fn_8007E5CC(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(4, pArgs, pResult);
}

void fn_8007E5F8(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(5, pArgs, pResult);
}

void fn_8007E624(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(6, pArgs, pResult);
}

void fn_8007E650(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E458(7, pArgs, pResult);
}

// Load the replay at card position pArgs[0..2]: 1 when it loaded. Player 0 gets slot 0's created
// golfer when that slot's profile is loaded, else the replay's golfer (golfer 0 for a created
// one); the replay's course is set up for the session.
void fn_8007E67C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pos.n8 = pArgs[2].i;
    pResult->i = MC_LoadReplay(&pos) == 0;
    if (pResult->i != 0) {
        if (lbl_801D7148.aLoaded[0] == 1) {
            Session_SetGolfer(FIRST_CREATED_GOLFER, 0);
        } else if (gReplayData.player.golfer.nIndex >= FIRST_CREATED_GOLFER) {
            Session_SetGolfer(0, 0);
        } else {
            Session_SetGolfer(gReplayData.player.golfer.nIndex, 0);
        }
        GM_SetCurrentCourse(gReplayData.nCourse);
        lbl_80281ED4->b0 = 0;
    }
}

void fn_8007E744(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E748(MsgArg* pArgs, MsgArg* pResult) {
}

// How many challenge groups in a row, from the first, have a medal.
void fn_8007E74C(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 29; i++, n++) {
        if (gpSaveData[pArgs[0].i].aMedal[i] == 3) break;
    }
    pResult->i = n;
}

void fn_8007E798(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether one of the four names on the card at pArgs[0], pArgs[1] is pArgs[2].
void fn_8007E79C(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007E818(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pResult->i = MC_GetNumUser(&pos);
}

void fn_8007E85C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = fn_800A1164(pArgs[0].i, pArgs[1].i, ((MsgString*)pArgs[2].p)->pStr, pArgs[3].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

void fn_8007E8B4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 1;
}

void fn_8007E8C0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E8C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.aLoaded[pArgs[0].i];
}

void fn_8007E8DC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b0F;
}

void fn_8007E8F0(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b0F = pArgs[0].i;
}

void fn_8007E904(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.uFlags & 0x4000) {
        pResult->i = 60;
        return;
    }
    pResult->i = 30;
}

void fn_8007E92C(MsgArg* pArgs, MsgArg* pResult) {
    gpCrAPState->b83 = pArgs[0].i;
}

// Test the password typed in (the string pArgs[0], pArgs[1] characters of it): TRUE when it
// unlocked something.
void fn_8007E93C(MsgArg* pArgs, MsgArg* pResult) {
    char szCode[0x20];          // the size is unknown: the frame leaves 0x20 bytes for it

    strncpy(szCode, ((MsgString*)pArgs[0].p)->pStr, pArgs[1].i);
    szCode[pArgs[1].i] = '\0';
    pResult->i = PasswordManager_TestPassword(szCode);
}

void fn_8007E9A0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E9A4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007E9A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b11;
}

void fn_8007E9BC(MsgArg* pArgs, MsgArg* pResult) {
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

// Whether the save on the card in port pArgs[0], slot pArgs[1] holds replay pArgs[2].
void fn_8007EA14(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;
    u32 uBit = pArgs[2].i;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = BitArray_TestBit(state.aReplayUsed, uBit);
}

// Option a0[1]: the menus' choices 1..6 are the values 5, 0, 1, 2, 3, 4; it is passed on times 0.2.
void fn_8007EA70(MsgArg* pArgs, MsgArg* pResult) {
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

// Option a0[0], the same way.
void fn_8007EB70(MsgArg* pArgs, MsgArg* pResult) {
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

// Option a0[1] as the menus' choice (1..6).
void fn_8007EC70(MsgArg* pArgs, MsgArg* pResult) {
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

// Option a0[0] as the menus' choice (1..6).
void fn_8007ECFC(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007ED88(MsgArg* pArgs, MsgArg* pResult) {
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

// The wind option: calm (0) to gusty (3), shown as 1 to 4.
void fn_8007EDDC(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007EE40(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a7[0]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_8007EE7C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8007EE80(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->profile.createdGolfer.nModelID = pArgs[1].i;
}

// Save the profile being worked on into slot pArgs[0] and mark the slot loaded (with session flag
// 0x4000, only into a slot that has none). A profile without a TOUR card level gets level 1,
// unless lbl_801D7148.b18 is set.
void fn_8007EE90(MsgArg* pArgs, MsgArg* pResult) {
    s32 nSlot = pArgs[0].i;

    if (!(gSession.uFlags & 0x4000) || lbl_801D7148.aLoaded[nSlot] == 0) {
        Mem_cpy(&gpSaveData[nSlot], &lbl_80281ED4->profile, sizeof(SaveProfile));
        gpSaveData[nSlot].bActive = 1;
        if (gpSaveData[nSlot].nTourCardLevel == 0 && lbl_801D7148.b18 == 0) {
            gpSaveData[nSlot].nTourCardLevel = 1;
        }
        lbl_801D7148.aLoaded[nSlot] = 1;
        fn_80077808(nSlot);
    }
}

// Slot pArgs[0]'s money, and its golfer's TOUR career winnings.
void fn_8007EF54(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = gpSaveData[pArgs[0].i].nTotalCash;
    *(u32*)pArgs[2].p = gpSaveData[pArgs[0].i].tour.aStats[PGA_USER_GOLFER].nCareerWinnings;
}

void fn_8007EF9C(MsgArg* pArgs, MsgArg* pResult) {
}

// How many of the first 23 awards the slot's profile has won.
void fn_8007EFA0(MsgArg* pArgs, MsgArg* pResult) {
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

// How many of the awards after the first 23 the slot's profile has won.
void fn_8007EFEC(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007F088(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_vGetAllTimeRecordsHeld(&gpSaveData[pArgs[0].i]);
}

// Profile pArgs[0]'s progress, into the values pArgs[1..7] point at: ladder awards won, PGA TOUR
// tournaments won, the bonus progress, aB1CC bits set, real-time event awards won and aSponsor
// entries set (pArgs[5] is not used).
void fn_8007F0D0(MsgArg* pArgs, MsgArg* pResult) {
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
        if (BitArray_TestBit(gpSaveData[nProfile].aB1CC, i)) {
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

// Profile pArgs[0]'s stats, into the values pArgs[1..9] point at: the best round, the longest
// drive and putt, nHolesInOne, two zeros, the game progress, the golfers unlocked and the courses
// unlocked. The course list names course 0 twice (and not 4 or 7), so the count is one less; all
// 18 rewards unlocked add one back.
void fn_8007F2C0(MsgArg* pArgs, MsgArg* pResult) {
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

// How many of the first 71 par-5 holes the slot's profile has eagled (fn_800588F4's kind 0).
void fn_8007F5CC(MsgArg* pArgs, MsgArg* pResult) {
    int n;
    int i;

    n = 0;
    for (i = 0; i < 71; i++) {
        if (fn_800588F4(&gpSaveData[pArgs[0].i], 0, i) == 1) {
            n++;
        }
    }
    pResult->i = n;
}

// How many ladder events the slot's profile has won.
void fn_8007F640(MsgArg* pArgs, MsgArg* pResult) {
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

// One for a TOUR card, plus one per challenge group with a medal.
void fn_8007F724(MsgArg* pArgs, MsgArg* pResult) {
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

// How many challenge groups have medal 2.
void fn_8007F784(MsgArg* pArgs, MsgArg* pResult) {
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

// How many challenge groups have medal 1.
void fn_8007F7D0(MsgArg* pArgs, MsgArg* pResult) {
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

// One for a TOUR card, plus one per challenge group with the best medal (0).
void fn_8007F81C(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007F87C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].nTourCardLevel;
}

// Shows the replay profile pArgs[0] saved with award pArgs[1] (awards 0, 6, 9, 3 and 13 have one).
// SaveProfile.aReplay is Replay[5]: copying the struct member gives the original's copy order.
void fn_8007F8A0(MsgArg* pArgs, MsgArg* pResult) {
    Replay replay0;
    Replay replay6;
    Replay replay9;
    Replay replay3;
    Replay replay13;

    lbl_80281ED4->b0 = 1;
    lbl_80281ED4->n1061C = pArgs[1].i;
    switch (pArgs[1].i) {
    case 0:
        replay0 = gpSaveData[pArgs[0].i].aReplay[0];
        fn_8007739C(&replay0);
        break;
    case 6:
        replay6 = gpSaveData[pArgs[0].i].aReplay[1];
        fn_8007739C(&replay6);
        break;
    case 9:
        replay9 = gpSaveData[pArgs[0].i].aReplay[2];
        fn_8007739C(&replay9);
        break;
    case 3:
        replay3 = gpSaveData[pArgs[0].i].aReplay[3];
        fn_8007739C(&replay3);
        break;
    case 13:
        replay13 = gpSaveData[pArgs[0].i].aReplay[4];
        fn_8007739C(&replay13);
        break;
    }
}

// Saves the working profile (lbl_80281ED4) into slot pArgs[0]: its name, created golfer and its
// bytes from 0x54C0 up to 0xB634. A new slot keeps its money and gets n1C and 25,000 more. The
// all-time records held under the slot's old name, and its saved replays, take the new name (the
// course records were meant to: see the EA bug below).
void fn_8007FA60(MsgArg* pArgs, MsgArg* pResult) {
    char szOld[0x20];           // the size is unknown (0x20 gives the original's frame)
    int  nSlot;
    int  nMoney;
    int  k;
    int  j;
    int  i;

    nMoney = 0;
    nSlot = pArgs[0].i;
    if (!lbl_801D7148.aLoaded[nSlot]) {
        nMoney = gpSaveData[nSlot].nCurrentCash;
    }
    strcpy(szOld, gpSaveData[nSlot].szName);
    strcpy(gpSaveData[nSlot].szName, lbl_80281ED4->profile.szName);
    memcpy(&gpSaveData[nSlot].createdGolfer, &lbl_80281ED4->profile.createdGolfer, sizeof(GolferRecord));
    memcpy(gpSaveData[nSlot].unk54C0, lbl_80281ED4->profile.unk54C0, 0x5500 - 0x54C0);
    memcpy(&gpSaveData[nSlot].choices, &lbl_80281ED4->profile.choices, 0xB634 - 0x5500);
    if (!lbl_801D7148.aLoaded[nSlot]) {
        nMoney = lbl_801D7148.n1C + nMoney;
        gpSaveData[nSlot].nCurrentCash = nMoney + 25000;
    }
    gpSaveData[nSlot].bActive = 1;
    if (gpSaveData[nSlot].nTourCardLevel == 0) {
        gpSaveData[nSlot].nTourCardLevel = 1;
    }
    lbl_801D7148.aLoaded[nSlot] = 1;
    fn_80077808(nSlot);

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

void fn_8007FCC0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.nMode;
}

void fn_8007FCD4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.nMode = pArgs[0].i;
}

void fn_8007FCE8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].bChanged;
}

// The most rewards any one profile has unlocked, or the cheat codes have, if that is more.
void fn_8007FD0C(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8007FEAC(MsgArg* pArgs, MsgArg* pResult) {
    fn_800907AC(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_8007FED8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b18;
}

// Set b18; clearing it gives slot 0's profile (and its backup) TOUR card level 1 if it has none.
void fn_8007FEEC(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b18 = pArgs[0].i;
    if (lbl_801D7148.b18 == 0 && gpSaveData[0].nTourCardLevel == 0) {
        if (lbl_801D7148.p658[0].nTourCardLevel < 1) {
            lbl_801D7148.p658[0].nTourCardLevel = 1;
        }
        gpSaveData[0].nTourCardLevel = 1;
    }
}

void fn_8007FF3C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpCrAPState->bHidden;
}

void fn_8007FF4C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nController[pArgs[0].i];
}

// What unlocks a course.
void fn_8007FF6C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gEarningsTable.aCoursePrice[pArgs[0].i].nPrice;
}

// Make the profile being worked on a new one named "USER<n>" and save it into slot pArgs[0]. A
// slot that had no profile gets 25000 more money, plus n1C.
void fn_8007FF8C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nSlot = pArgs[0].i;
    char szName[16];

    sprintf(szName, "USER%d", nSlot + 1);
    fn_80057ED0(&lbl_80281ED4->profile, szName);
    if (lbl_801D7148.aLoaded[nSlot] == 0) {
        lbl_80281ED4->profile.nCurrentCash += lbl_801D7148.n1C + 25000;
    }
    lbl_80281ED4->profile.bActive = 1;
    if (lbl_80281ED4->profile.nTourCardLevel == 0) {
        lbl_80281ED4->profile.nTourCardLevel = 1;
    }
    lbl_801D7148.aLoaded[nSlot] = 1;
    Mem_cpy(&gpSaveData[nSlot], &lbl_80281ED4->profile, sizeof(SaveProfile));
    fn_80077808(nSlot);
}

// Hole pArgs[1]'s par on course pArgs[0]. Below 0 it is the custom round being edited (slot n3,
// round n4); 22 and 24..29 are built rounds, whose holes come from other courses.
void fn_80080054(MsgArg* pArgs, MsgArg* pResult) {
    int nCourse;
    int nCourseArg = pArgs[0].i;

    if (nCourseArg <= -1) {
        nCourse = gpSaveData[lbl_80281ED4->n3].aSavedRound[lbl_80281ED4->n4].nCourse[pArgs[1].i];
        pResult->i = fn_800D2ABC(
            nCourse, gpSaveData[lbl_80281ED4->n3].aSavedRound[lbl_80281ED4->n4].nHoleNum[pArgs[1].i]);
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

// The saved replay's course, hole and golfer.
void fn_8008017C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, lbl_80191990[gReplayData.nCourse]);
}

void fn_800801C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gReplayData.nHole;
}

void fn_800801D4(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, gReplayData.player.golfer.szLast);
}

// The prize for beating golfer pArgs[0] in the current game mode: the stroke prize in modes 0 and
// 1, the skins value in mode 2, the ladder event's in mode 4 (the last event's past event 24);
// the golfer's rating picks the row. Other modes leave pResult alone.
void fn_80080208(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80080300(MsgArg* pArgs, MsgArg* pResult) {
}

// A challenge group's best medal (0 best, 3 none).
void fn_80080304(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aMedal[pArgs[1].i];
}

void fn_80080334(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].createdGolfer.nModelID;
}

void fn_80080358(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    MC_Disconnect();
}

// Like fn_8007D428 (without its b11702 override), with the golfers of lbl_801894E8 unlocked
// instead of the profiles' unlocks.
void fn_80080388(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    if ((s8)gGolferTable[pArgs[1].i].bAvailable != -1) {
        pResult->i = 0;
        if (pArgs[1].i >= FIRST_CREATED_GOLFER) {
            pResult->i = 1;
        }
        for (i = 0; i < 16; i++) {
            if (pArgs[1].i == lbl_801894E8[i]) {
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

void fn_800804D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800804E4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_UserHasEagledHole(pArgs[0].i, pArgs[1].i, pArgs[2].i - 1);
}

// For the working slot's par-5 hole pArgs[0] (course), pArgs[1] (hole, from 1): the date it was
// eagled, unpacked into pArgs[2..4] (fn_80078620), or zeros when it has not been eagled.
void fn_8008052C(MsgArg* pArgs, MsgArg* pResult) {
    int nA = pArgs[0].i;
    int nB = pArgs[1].i - 1;
    int* pA = pArgs[2].p;
    int* pB = pArgs[3].p;
    int* pC = pArgs[4].p;

    if (GM_UserHasEagledHole(lbl_80281ED4->nSlot, nA, nB)) {
        fn_80078620(GM_GetPar5EagleDate(lbl_80281ED4->nSlot, nA, nB), pA, pB, pC);
        return;
    }
    *pA = 0;
    *pB = 0;
    *pC = 0;
}

void fn_800805C4(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = FE_MOVIE_CREDITS;
    Gaud_StopMusic();
}

void fn_800805F0(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether slot pArgs[0]'s profile or a cheat code has unlocked course pArgs[1].
void fn_800805F4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    if (gpSaveData[pArgs[0].i].aCourseUnlocked[pArgs[1].i] != 0) {
        pResult->i = 1;
    }
    if (lbl_80281DF4->aCourseUnlocked[pArgs[1].i] != 0) {
        pResult->i = 1;
    }
}

// How far the rewards go for slot pArgs[0]: the number of the last one its profile or a cheat
// code has unlocked (0: none).
void fn_80080654(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_800807D0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Whether slot pArgs[0] has a loaded profile with custom round pArgs[1] in use.
void fn_800807DC(MsgArg* pArgs, MsgArg* pResult) {
    int b = 0;

    if (gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 != 0 && gpSaveData[pArgs[0].i].bActive != 0) {
        b = 1;
    }
    pResult->i = b;
}

// The name of slot pArgs[0]'s custom round pArgs[1].
void fn_80080828(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[2].p)->pStr, gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].szName);
}

// Name slot pArgs[0]'s custom round pArgs[1]: blank it, then copy the first pArgs[3] characters
// of pArgs[2] (each place is blanked again first).
void fn_80080878(MsgArg* pArgs, MsgArg* pResult) {
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

// Set hole pArgs[2] of slot pArgs[0]'s custom round pArgs[1]: course pArgs[3] (-1 instead clears
// the round's n0, its in-use flag) and hole number pArgs[4].
void fn_800809F8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[3].i != -1) {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nCourse[pArgs[2].i] = pArgs[3].i;
    } else {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = 0;
    }
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nHoleNum[pArgs[2].i] = pArgs[4].i;
}

void fn_80080AA0(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n15 = pArgs[2].i;
}

// A string's first character.
void fn_80080AD0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = ((MsgString*)pArgs[0].p)->pStr[0];
}

// Turn the point (*pArgs[1], *pArgs[2]) by pArgs[0] degrees.
void fn_80080AE8(MsgArg* pArgs, MsgArg* pResult) {
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

// Hole pArgs[2] of slot pArgs[0]'s custom round pArgs[1]: its course and hole number.
void fn_80080BB8(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[3].p = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nCourse[pArgs[2].i];
    *(s32*)pArgs[4].p = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nHoleNum[pArgs[2].i];
}

void fn_80080C2C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n15;
}

void fn_80080C60(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n4;
}

void fn_80080C74(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n4 = pArgs[0].i;
}

void fn_80080C84(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n5;
}

void fn_80080C98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n5 = pArgs[0].i;
}

void fn_80080CA8(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[2].p = 0;
    *(s32*)pArgs[3].p = 0;
    *(s32*)pArgs[4].p = 0;
}

// Fill entry pArgs[2] of slot pArgs[0]'s saved round pArgs[1] with a random hole: a random course
// from a list of 20 (course 0 twice, no 4 or 7) that this profile or the cheat codes have unlocked,
// and a random hole number 0..17, drawn again while the round already holds that course and hole.
void fn_80080CC8(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_800810BC(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.aCPU[pArgs[0].i] = pArgs[1].i;
}

void fn_800810D8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.aCPU[pArgs[0].i];
}

// Option n14: menu choices 1-3 are the values 0-2.
void fn_800810F4(MsgArg* pArgs, MsgArg* pResult) {
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

// Option n18: menu choices 1-3 are the values 0-2, applied at once.
void fn_80081158(MsgArg* pArgs, MsgArg* pResult) {
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

// Option n20, the same way.
void fn_800811E4(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80081270(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bPuttingGrid = 1;
        return;
    case 2:
        gSession.options.bPuttingGrid = 0;
        return;
    }
}

void fn_800812B0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[0] = 1;
        return;
    case 2:
        gSession.options.a24[0] = 0;
        return;
    }
}

void fn_800812F0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[1] = 1;
        return;
    case 2:
        gSession.options.a24[1] = 0;
        return;
    }
}

void fn_80081330(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[2] = 1;
        return;
    case 2:
        gSession.options.a24[2] = 0;
        return;
    }
}

void fn_80081370(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[3] = 1;
        return;
    case 2:
        gSession.options.a24[3] = 0;
        return;
    }
}

void fn_800813B0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[4] = 1;
        return;
    case 2:
        gSession.options.a24[4] = 0;
        return;
    }
}

void fn_800813F0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[5] = 1;
        return;
    case 2:
        gSession.options.a24[5] = 0;
        return;
    }
}

void fn_80081430(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[6] = 1;
        return;
    case 2:
        gSession.options.a24[6] = 0;
        return;
    }
}

void fn_80081470(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.a24[7] = 1;
        return;
    case 2:
        gSession.options.a24[7] = 0;
        return;
    }
}

void fn_800814B0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bBoostEnabled = 1;
        return;
    case 2:
        gSession.options.bBoostEnabled = 0;
        return;
    }
}

void fn_800814F0(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
        gSession.options.bSpinEnabled = 1;
        return;
    case 2:
        gSession.options.bSpinEnabled = 0;
        return;
    }
}

// Option a0[2]: the menus' choices 1..6 are the values 5, 0, 1, 2, 3, 4.
void fn_80081530(MsgArg* pArgs, MsgArg* pResult) {
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

// Option n14 as the menus' choice (1..3).
void fn_800815E0(MsgArg* pArgs, MsgArg* pResult) {
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

// Option n18 as the menus' choice (1..3).
void fn_80081634(MsgArg* pArgs, MsgArg* pResult) {
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

// Option n20 as the menus' choice (1..3).
void fn_80081688(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_800816DC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bPuttingGrid) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081718(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[0]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081754(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[1]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081790(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[2]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_800817CC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[3]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081808(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[4]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081844(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[5]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081880(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[6]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_800818BC(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.a24[7]) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_800818F8(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bBoostEnabled) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

void fn_80081934(MsgArg* pArgs, MsgArg* pResult) {
    switch (gSession.options.bSpinEnabled) {
    case 1:
        pResult->i = 1;
        return;
    case 0:
        pResult->i = 2;
        return;
    }
}

// Option a0[2] as the menus' choice (1..6).
void fn_80081970(MsgArg* pArgs, MsgArg* pResult) {
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

// Pick the saved custom round the holes come from.
void fn_800819FC(MsgArg* pArgs, MsgArg* pResult) {
    gpGame->b136 = pArgs[0].i;
    gpGame->nSaveSlot = pArgs[1].i;
    gpGame->nSaveCourse = pArgs[2].i;
    if (gpGame->b136 != 0) {
        GM_SetupCustomHoleSelection();
        GM_InitializeCurrentHoleToFirstSelected();
    }
}

// Whether backup row pArgs[0] holds a profile no player slot is using.
void fn_80081A54(MsgArg* pArgs, MsgArg* pResult) {
    u8 bUsed = 0;
    int i;

    for (i = 0; i < 4; i++) {
        if (lbl_801D7148.aBackup[i] == pArgs[0].i) {
            bUsed = 1;
        }
    }
    if (bUsed) {
        pResult->i = 0;
        return;
    }
    pResult->i = lbl_801D7148.p658[pArgs[0].i].bActive;
}

// The name in backup row pArgs[0]'s profile.
void fn_80081B04(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_801D7148.p658[pArgs[0].i].szName);
}

// Load slot pArgs[1] from backup row pArgs[0] (the rows are swapped first when they differ), and
// mark the slot loaded.
void fn_80081B50(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRow = pArgs[0].i;
    s32 nSlot = pArgs[1].i;

    lbl_801D7148.aLoaded[nSlot] = 1;
    if (nSlot != nRow) {
        fn_800779BC(nSlot, nRow);
    }
    Mem_cpy(&gpSaveData[nSlot], &lbl_801D7148.p658[nSlot], sizeof(SaveProfile));
    lbl_801D7148.aBackup[nSlot] = nSlot;
}

void fn_80081BD4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077780();
}

void fn_80081BF4(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077808(pArgs[0].i);
}

// Option n1C: menu choices 1-3 are the values 0-2, applied at once.
void fn_80081C18(MsgArg* pArgs, MsgArg* pResult) {
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

// Option n1C as the menus' choice (1..3).
void fn_80081CA4(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80081CF8(MsgArg* pArgs, MsgArg* pResult) {
}

// Slot pArgs[0]'s created golfer: set its ball type (pArgs[4]) and n54C2 (pArgs[5]); -1 keeps one.
void fn_80081CFC(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[4].i >= 0) {
        gpSaveData[pArgs[0].i].nGolferBallType = pArgs[4].i;
    }
    if (pArgs[5].i >= 0) {
        gpSaveData[pArgs[0].i].n54C2 = pArgs[5].i;
    }
}

// The same two, read back (read signed); pArgs[1..3] are cleared.
void fn_80081D50(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = 0;
    *(s32*)pArgs[2].p = 0;
    *(s32*)pArgs[3].p = 0;
    *(s32*)pArgs[4].p = (s8)gpSaveData[pArgs[0].i].nGolferBallType;
    *(s32*)pArgs[5].p = (s8)gpSaveData[pArgs[0].i].n54C2;
}

// Slot pArgs[0]'s created golfer: a level 1..4 per attribute group (below 50, 50, 75, 100) into
// the words pArgs[1..5] point at: power (4 once the tour card is at level 6), ball striking and
// approach together, putting, spin and recovery.
void fn_80081DB8(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80081F98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n1 = pArgs[0].i;
}

void fn_80081FA8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n1;
}

// Build the list of pairs fn_80082620 reads back, n10620 of them: slot pArgs[0]'s created golfer's
// saved attributes against six values (pArgs[1..6]). Where the saved attribute has reached 50, 75
// or 100 and the value passed for it is still under that mark, the pair (group, 2, 3 or 4 by mark)
// is added. Groups: 1 power (50 and 75 only), 2 ball striking and approach (both reached, either
// value under), 3 putting, 4 spin, 5 recovery.
void fn_80081FBC(MsgArg* pArgs, MsgArg* pResult) {
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

    lbl_80281ED4->n10620 = 0;
    for (i = 0; i < 15; i++) {
        lbl_80281ED4->a10621[i][0] = -1;
        lbl_80281ED4->a10621[i][1] = -1;
    }
    if (fPower < 50.0f && nPower >= 50) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 1;
        lbl_80281ED4->a10621[n][1] = 2;
        n++;
    }
    if (fPower < 75.0f && nPower >= 75) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 1;
        lbl_80281ED4->a10621[n][1] = 3;
        n++;
    }
    if (nStriking >= 50 && nApproach >= 50 && (fStriking < 50.0f || fApproach < 50.0f)) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 2;
        lbl_80281ED4->a10621[n][1] = 2;
        n++;
    }
    if (nStriking >= 75 && nApproach >= 75 && (fStriking < 75.0f || fApproach < 75.0f)) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 2;
        lbl_80281ED4->a10621[n][1] = 3;
        n++;
    }
    if (nStriking >= 100 && nApproach >= 100 && (fStriking < 100.0f || fApproach < 100.0f)) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 2;
        lbl_80281ED4->a10621[n][1] = 4;
        n++;
    }
    if (fPutting < 50.0f && nPutting >= 50) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 3;
        lbl_80281ED4->a10621[n][1] = 2;
        n++;
    }
    if (fPutting < 75.0f && nPutting >= 75) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 3;
        lbl_80281ED4->a10621[n][1] = 3;
        n++;
    }
    if (fPutting < 100.0f && nPutting >= 100) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 3;
        lbl_80281ED4->a10621[n][1] = 4;
        n++;
    }
    if (fSpin < 50.0f && nSpin >= 50) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 4;
        lbl_80281ED4->a10621[n][1] = 2;
        n++;
    }
    if (fSpin < 75.0f && nSpin >= 75) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 4;
        lbl_80281ED4->a10621[n][1] = 3;
        n++;
    }
    if (fSpin < 100.0f && nSpin >= 100) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 4;
        lbl_80281ED4->a10621[n][1] = 4;
        n++;
    }
    if (fRecovery < 50.0f && nRecovery >= 50) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 5;
        lbl_80281ED4->a10621[n][1] = 2;
        n++;
    }
    if (fRecovery < 75.0f && nRecovery >= 75) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 5;
        lbl_80281ED4->a10621[n][1] = 3;
        n++;
    }
    if (fRecovery < 100.0f && nRecovery >= 100) {
        lbl_80281ED4->n10620++;
        lbl_80281ED4->a10621[n][0] = 5;
        lbl_80281ED4->a10621[n][1] = 4;
        n++;
    }
}

void fn_80082608(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n10620;
}

// The two values of pair pArgs[0] into the words pArgs[1] and pArgs[2] point at.
void fn_80082620(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = lbl_80281ED4->a10621[pArgs[0].i][0];
    *(s32*)pArgs[2].p = lbl_80281ED4->a10621[pArgs[0].i][1];
}

void fn_8008266C(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n10620 = 0;
}

void fn_80082680(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A218C(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_800826C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A2194(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_80082708(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_NumEASaveGames(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = MC_GetNumEATitles();
}

void fn_80082758(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_EASaveExists(pArgs[0].i);
}

void fn_80082790(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, MC_GetEASaveName(pArgs[0].i));
}

// Add a payout to player slot 0's money (when pArgs[1] is set); the front end keeps the amount.
void fn_800827D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nAmount;

    nAmount = pArgs[0].i;
    lbl_801D7148.n1C = nAmount;
    if (pArgs[1].i != 0) {
        gpSaveData->nCurrentCash += nAmount;
    }
}

void fn_80082800(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

void fn_8008281C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

// The card in slot pArgs[0], pArgs[1]: whether its sectors are not 8 KB, and its error flags
// (bad encoding, not a memory card, I/O error, broken).
void fn_80082828(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80082928(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

void fn_8008293C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[pArgs[0].i]);
}

void fn_80082978(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008297C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082980(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

// The letter for a number: 0 is "A".
void fn_8008299C(MsgArg* pArgs, MsgArg* pResult) {
    sprintf(((MsgString*)pArgs[2].p)->pStr, "%c", pArgs[0].i + 'A');
}

void fn_800829D4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

void fn_800829E0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 4;
}

void fn_800829EC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80077968(pArgs[0].i);
}

void fn_80082A10(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    strcpy(((MsgString*)pArgs[0].p)->pStr, "");
}

void fn_80082A44(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082A48(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082A4C(MsgArg* pArgs, MsgArg* pResult) {
}

// The free directory entries on the card in slot pArgs[0], pArgs[1].
void fn_80082A50(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState state;

    MC_GetMC(&state, pArgs[0].i, pArgs[1].i);
    pResult->i = state.nFreeFiles;
}

// Record pArgs[2] of table pArgs[0] in the record list pArgs[1] (0, 1: recC; 16, 17, 13: recB):
// its value, and its holder's name into the string pArgs[3].
void fn_80082A94(MsgArg* pArgs, MsgArg* pResult) {
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

// Empty a player slot: no profile in it, none loaded.
void fn_80082C74(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].bActive = 0;
    lbl_801D7148.aLoaded[pArgs[0].i] = 0;
}

void fn_80082CA4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80082CA8(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_GetRewards(pArgs[0].i, pArgs[1].p, pArgs[2].p, pArgs[3].p);
}

void fn_80082CDC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

void fn_80082D14(MsgArg* pArgs, MsgArg* pResult) {
    gSession.options.rows[pArgs[0].i][pArgs[1].i] = pArgs[2].i;
}

// Music row pArgs[0]'s flag for track pArgs[1], and the track's two lines of text.
void fn_80082D3C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.options.rows[pArgs[0].i][pArgs[1].i];
    ((MsgString*)pArgs[2].p)->pStr = lbl_801F846C[pArgs[1].i].sz0;
    ((MsgString*)pArgs[3].p)->pStr = lbl_801F846C[pArgs[1].i].szSong;
}

void fn_80082D98(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281ED4->n3 = pArgs[0].i;
}

// The game's title.
void fn_80082DA8(MsgArg* pArgs, MsgArg* pResult) {
    ((MsgString*)pArgs[0].p)->pStr = "TIGER WOODS PGA TOUR\xAE 2004";
}

void fn_80082DBC(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_8009EE28(pArgs[0].i, pArgs[1].i) == MC_ERR_BADDATA) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_80082E10(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;
    s32 n;

    nError = MC_DeleteSaveGame(pArgs[0].i, pArgs[1].i);
    n = 1;
    if (nError != 0) {
        n = nError;
    }
    pResult->i = n;
}

// The four music rows' switches and option b7E as the menus' choices (1 on, 2 off), and n80.
void fn_80082E5C(MsgArg* pArgs, MsgArg* pResult) {
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

// Set the four music rows' switches and option b7E from the menus' choices (1 on, 2 off) and n80,
// then apply them.
void fn_80082F68(MsgArg* pArgs, MsgArg* pResult) {
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

// Whether choice pArgs[0] is available: 1 always, 2 and 3 once a loaded profile or a cheat code
// has unlocked course 21 or 22.
void fn_80083068(MsgArg* pArgs, MsgArg* pResult) {
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

// For the current ladder event's challenge: the most a single opponent's skins are worth over the
// holes played (the front nine, the back nine or both; each hole's skin at the opponent's rating),
// and each opponent playing the player's own golfer gets the next of its four looks.
void fn_8008311C(MsgArg* pArgs, MsgArg* pResult) {
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

    pRecord = fn_80077A80(gSession.nGolfer[lbl_80281ED4->nSlot]);
    for (i = 0; i < nOpponents; i++) {
        pOther = fn_80077A80(PlayNow_GetOpponent(nChallenge, i));
        if (pRecord->nModelID == pOther->nModelID) {
            nLook = gSession.aProfile[lbl_80281ED4->nSlot].n0 + 1;
            if (nLook == 4) {
                nLook = 0;
            }
            gSession.aProfile[i + 1].n0 = nLook;
        }
    }
}

void fn_80083354(MsgArg* pArgs, MsgArg* pResult) {
}

// Queue a bio movie (which bio is not set here: nBio keeps what the queue entry held), or the
// credits for pArgs[0] -1; then Gaud_StopMusic.
void fn_80083358(MsgArg* pArgs, MsgArg* pResult) {
    FEMovie* pMovie;

    pMovie = fn_800770FC();
    pMovie->nKind = FE_MOVIE_BIO;
    if (pArgs[0].i == -1) {
        pMovie->nKind = FE_MOVIE_CREDITS;
    }
    Gaud_StopMusic();
}

void fn_800833A4(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_StopMusic();
}

void fn_800833C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800833D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = pArgs[0].i;
    gSession.nGolfer[n] = (u8)(n + 30);
}

// No player slot has a backup row.
void fn_800833F4(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.aBackup[0] = -1;
    lbl_801D7148.aBackup[1] = -1;
    lbl_801D7148.aBackup[2] = -1;
    lbl_801D7148.aBackup[3] = -1;
}

void fn_80083414(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.aBackup[pArgs[0].i] = -1;
}

// The pad in port pArgs[0] is a WaveBird (its SI device type).
void fn_80083430(MsgArg* pArgs, MsgArg* pResult) {
    if (Input_iGetPadType(pArgs[0].i) == 0x8B100000) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
}

void fn_80083480(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_801D7148.b10;
}

void fn_80083494(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D7148.b10 = pArgs[0].i;
}

void fn_800834A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009F728(pArgs[0].i);
}

void fn_800834DC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// For the card in slot pArgs[0], pArgs[1]: fn_8009D3DC's answer, and MC_BlocksNeededForSave's with kind 0.
void fn_800834E8(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = fn_8009D3DC(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[3].p = MC_BlocksNeededForSave(pArgs[0].i, pArgs[1].i, 0, 0);
    MC_Disconnect();
}

// The same for an EA Sports Bio save: the new files it needs, and MC_BlocksNeededForSave's with kind 3.
void fn_80083550(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[2].p = fn_8009D50C(pArgs[0].i, pArgs[1].i);
    *(s32*)pArgs[3].p = MC_BlocksNeededForSave(pArgs[0].i, pArgs[1].i, 0, 3);
    MC_Disconnect();
}

void fn_800835B8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800835BC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800835C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Give player pArgs[0] their golfer's bag: a created golfer's own, else the default bag.
void fn_800835D4(MsgArg* pArgs, MsgArg* pResult) {
    s32 nPlayer = pArgs[0].i;
    GolferRecord* pRecord = fn_80077A80(gSession.nGolfer[nPlayer]);

    if (gSession.nGolfer[nPlayer] < FIRST_CREATED_GOLFER) {
        gSession.uBag[nPlayer] = 0x02A7FC44;
        return;
    }
    gSession.uBag[nPlayer] = pRecord->uBagMask;
}

// Empties profile pArgs[0]'s saved round pArgs[1]: no holes, and n0 cleared.
void fn_80083658(MsgArg* pArgs, MsgArg* pResult) {
    int i;

    for (i = 0; i < 18; i++) {
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nHoleNum[i] = -1;
        gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].nCourse[i] = 0;
    }
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = 0;
}

void fn_80083860(MsgArg* pArgs, MsgArg* pResult) {
    gpSaveData[pArgs[0].i].aSavedRound[pArgs[1].i].n0 = pArgs[2].i;
}

void fn_80083890(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 150;
}

void fn_8008389C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838A8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838AC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838B8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838BC(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838C0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800838C4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strcmp(((MsgString*)pArgs[0].p)->pStr, ((MsgString*)pArgs[1].p)->pStr);
}

void fn_80083904(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083908(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008390C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083910(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083914(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083918(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008391C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083920(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083924(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083928(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008392C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083930(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083934(MsgArg* pArgs, MsgArg* pResult) {
    FE_SetProfileLeftHanded(lbl_80281ED4->nSlot, pArgs[0].i);
}

void fn_80083964(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 0.0f;
}

void fn_80083970(MsgArg* pArgs, MsgArg* pResult) {
}

// Three values out: 50, 50 and a level of 25..250 (pArgs[3] mod 10, plus one, times 25), the
// level going to the one of the three pArgs[2] mod 3 picks.
void fn_80083974(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80083A44(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083A48(MsgArg* pArgs, MsgArg* pResult) {
}

// Fill string pArgs[0] with pArgs[1] asterisks (a hidden entry).
void fn_80083A4C(MsgArg* pArgs, MsgArg* pResult) {
    char szStars[64] = "";
    int i;
    int nLen;

    nLen = pArgs[1].i;
    for (i = 0; i < nLen; i++) {
        szStars[i] = '*';
    }
    szStars[i + 1] = '\0';      // EA bug: one past the stars; the buffer is zeroed anyway
    strcpy(((MsgString*)pArgs[0].p)->pStr, szStars);
}

void fn_80083BA4(MsgArg* pArgs, MsgArg* pResult) {
    GameMode26_SetTargetScore(pArgs[0].i);
}

void fn_80083BC8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
    if (pResult->i == 0) {
        fn_801102AC();
    }
}

// Start the current game mode's event (modes 11, 5, 23, 26, 22, 24) and answer fn_80110180. In
// modes 5 and 11 player 0 first gets golfer 0, or with profile 0 loaded the created golfer (not
// when b11703 is set).
void fn_80083BFC(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 11) {
        if (Game_GetMode() == 5 || Game_GetMode() == 11) {
            if (gpSaveData[0].bActive) {
                lbl_801D7148.aBackup[0] = 0;
                if (lbl_80281ED4->b11703 == 0) {
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
                lbl_801D7148.aBackup[0] = 0;
                if (lbl_80281ED4->b11703 == 0) {
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

// The disc read's state for the menus (100: fn_80110450 says so), like GameUICommands.c's
// IG_vGetDiscDriveStatus for the drive.
void fn_80083D88(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80083E48(MsgArg* pArgs, MsgArg* pResult) {
    fn_80123FF8();
    Gba_SetState(0);
}

void fn_80083E70(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(6);
}

void fn_80083E94(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(8);
}

void fn_80083EB8(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80083EBC(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(18);
}

// The first time only (bit 1 of the working profile's a10548 not yet set): run fn_801240A8 and
// fn_8012409C, set the bit and answer 1; else 0.
void fn_80083EE0(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = FE_GetCurrentProfile();

    if (!fn_80058304(pProfile, 1)) {
        fn_801240A8();
        fn_8012409C();
        fn_800582C4(pProfile, 1, 1);
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80083F54(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// The Game Boy Advance link's state (Gba_GetState) as the menus number it; other states leave
// the result as it was.
void fn_80083F60(MsgArg* pArgs, MsgArg* pResult) {
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

// Read five values over the Game Boy Advance link (gbacable.c) into the words pArgs[0..4] point
// at: fn_80124174's after request 0x70, then fn_80124190's after requests 0xB0 with 0 to 3. It
// stops once the link's state (Gba_GetState) is 18 or -1. Then, if fn_80124224 says so, a state
// of 7 or 9 becomes 12 or 13.
void fn_80084008(MsgArg* pArgs, MsgArg* pResult) {
    u32 i;

    if (Gba_GetState() != 18 && Gba_GetState() != -1) {
        fn_80123CBC(0x70, 0);
        if (Gba_GetState() != 18 && Gba_GetState() != -1) {
            *(s32*)pArgs[0].p = fn_80124174();
            for (i = 0; i < 4; i++) {
                fn_80123CBC(0xB0, i);
                if (Gba_GetState() == 18 || Gba_GetState() == -1) break;
                *(s32*)pArgs[1 + i].p = fn_80124190();
            }
        }
    }
    fn_801241D4(0);
    if (fn_80124224()) {
        if (Gba_GetState() == 7) {
            Gba_SetState(12);
        } else if (Gba_GetState() == 9) {
            Gba_SetState(13);
        }
        fn_8012421C(0);
    }
}

void fn_8008410C(MsgArg* pArgs, MsgArg* pResult) {
    s32 n;

    n = *(s32*)pArgs[0].p;
    pResult->i = n + fn_8012411C();
    fn_80124138(pResult->i);
}

void fn_80084158(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008415C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084160(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801241CC();
}

void fn_80084190(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80124174();
}

// Whether gbacable.c's Gba_GetState answers 18.
void fn_800841C0(MsgArg* pArgs, MsgArg* pResult) {
    if (Gba_GetState() == 18) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80084208(MsgArg* pArgs, MsgArg* pResult) {
    Gba_SetState(5);
}

// ---- the EA Sports Bio screens (EASportsBio.c does the work) ----

void fn_8008422C(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125600(pArgs[0].i, pArgs[1].i);
}

void fn_80084258(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125648(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

void fn_80084288(MsgArg* pArgs, MsgArg* pResult) {
    fn_8012566C(pArgs[0].i);
}

void fn_800842AC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80125680(pArgs[0].i);
}

void fn_800842D0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125354(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    nError = fn_801253F0(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    pResult->i = nError == 0;
}

// The EA Sports Bio requests for the card in slot pArgs[0], pArgs[1]: unless fn_80125194 answers 0
// or -18, fn_80125280 first (only for -43 and -44), then fn_801252D0; the first error ends it,
// else fn_801251EC runs. The answer is 1, or the error; the profile's n11704 keeps it too.
void fn_80084354(MsgArg* pArgs, MsgArg* pResult) {
    s32 aPos[2];
    s32 nError;

    aPos[0] = pArgs[0].i;
    aPos[1] = pArgs[1].i;
    nError = fn_80125194(aPos[0], aPos[1]);
    if (nError != 0 && nError != -18) {
        if (nError == -43 || nError == -44) {
            nError = fn_80125280(pArgs[0].i, pArgs[1].i);
            lbl_80281ED4->n11704 = nError;
            if (nError != 0) {
                pResult->i = (nError != 0) ? nError : 1;
                return;
            }
        }
        nError = fn_801252D0(pArgs[0].i, pArgs[1].i);
        lbl_80281ED4->n11704 = nError;
        if (nError != 0) {
            pResult->i = (nError != 0) ? nError : 1;
            lbl_80281ED4->n11704 = nError;
            return;
        }
    }
    nError = fn_801251EC(aPos);
    pResult->i = (nError != 0) ? nError : 1;
    lbl_80281ED4->n11704 = nError;
}

// For the card in port pArgs[0], slot pArgs[1]: TRUE when the EA Sports Bio on it opened, or
// failed with -18 (fn_80125118).
void fn_80084458(MsgArg* pArgs, MsgArg* pResult) {
    s32 aPos[2];

    aPos[0] = pArgs[0].i;
    aPos[1] = pArgs[1].i;
    pResult->i = fn_80125118(aPos);
}

// For the card in slot pArgs[0], pArgs[1]: fn_801255C4's answer.
void fn_8008449C(MsgArg* pArgs, MsgArg* pResult) {
    s32 aPos[2];

    aPos[0] = pArgs[0].i;
    aPos[1] = pArgs[1].i;
    pResult->i = fn_801255C4(aPos);
}

void fn_800844E0(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125434(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    if (nError != 0) {
        pResult->i = 0;
        return;
    }
    pResult->i = nError == 0;
}

void fn_80084544(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_IsBioLoaded();
}

// 0 when card slot 0, 0 answers fn_80125354 with error -43 (then fn_801253F0 runs anyway), else 1.
void fn_80084578(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_80125354(0, 0) == -43) {
        pResult->i = 0;
    } else {
        pResult->i = 1;
    }
    fn_801253F0(0, 0);
}

void fn_800845D4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125528(pArgs[0].i, pArgs[1].i);
}

// 1 when fn_80125354 answers error -18 for card slot 0, 0; fn_801253F0 follows when it succeeds.
void fn_80084614(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80084678(MsgArg* pArgs, MsgArg* pResult) {
    s32 nError;

    nError = fn_80125280(pArgs[0].i, pArgs[1].i);
    lbl_80281ED4->n11704 = nError;
    pResult->i = nError == 0;
}

void fn_800846C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_800846D4(MsgArg* pArgs, MsgArg* pResult) {
    s32* pN;

    pN = pArgs[0].p;
    *pN = fn_80125928();
}

// Whether an EA Sports Bio reward is waiting (fn_801256B8); its message is set up either way.
void fn_80084704(MsgArg* pArgs, MsgArg* pResult) {
    EASBio_eReward eReward = fn_801256B8();

    if (eReward != -1) {
        pResult->i = 1;
    } else {
        pResult->i = 0;
    }
    EASBio_SetCurrentRewardMessage(eReward);
}

void fn_80084750(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084754(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801254EC();
    pResult->i = fn_801254B8();
}

void fn_8008478C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_80124BDC();
}

void fn_800847BC(MsgArg* pArgs, MsgArg* pResult) {
    fn_80084FF0(pArgs[0].i);
}

// Runs memory-card operation pArgs[0] of the picked set on the card pArgs[1], pArgs[2]. Each
// operation takes its own payload (core/memcard.h): ops 0 and 1 answer whether they succeeded
// (0 is success), ops 2..4 answer their result.
void fn_800847E0(MsgArg* pArgs, MsgArg* pResult) {
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
        pResult->i = fn_800850AC(&pos0) == 0;
        break;
    case 1:
        posStr.pos.nPort = nPort;
        posStr.pos.nSlot = nSlot;
        posStr.pos.n8 = n8;
        posStr.szC = szName;
        pResult->i = fn_80085070(&posStr) == 0;
        break;
    case 3:
        pos3.nPort = nPort;
        pos3.nSlot = nSlot;
        pos3.n8 = n8;
        pResult->i = fn_80085034(&pos3);
        break;
    case 2:
        card.nPort = nPort;
        card.nSlot = nSlot;
        pResult->i = fn_80084FF8(&card);
        break;
    case 4:
        cardName.nPort = nPort;
        cardName.nSlot = nSlot;
        cardName.szName = szName;
        pResult->i = fn_80084FB4(&cardName);
        break;
    }
}

void fn_800848E4(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_800848E8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetNumGroups();
}

void fn_80084918(MsgArg* pArgs, MsgArg* pResult) {
    PlayNow_SelectGroup(pArgs[0].i - 1);
}

void fn_80084940(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupName(pArgs[1].i - 1));
}

void fn_80084984(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupDescription(pArgs[1].i - 1));
}

// A challenge group's best medal, the group counted from 1.
void fn_800849C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpSaveData[pArgs[0].i].aMedal[pArgs[1].i - 1];
}

// Set up course pArgs[0] (10000: the mixed round, built once) and answer fn_80110180, inverted
// when fn_8011027C says so.
void fn_800849F8(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 10000) {
        if (gpGame->b137 == 0) {
            gpGame->b137 = 1;
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
        gpGame->b137 = 0;
    }
}

// pArgs[0] 0: set up game mode 26, or mode 22 in variant 0 or 1 (pArgs[1] 1, 2). 1: game mode
// 22's n4 is 5, 10 or 15 (pArgs[1] 0, 1, 2).
void fn_80084AA8(MsgArg* pArgs, MsgArg* pResult) {
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

// Passes the message on to one of three handlers, by pArgs[0].
void fn_80084B88(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80084BE4(MsgArg* pArgs, MsgArg* pResult) {
}

// Music commands: 0 turns music row 0 and its track pArgs[1] on and plays the track, 1 calls
// Gaud_StopMusic, 2 calls Gaud_RestartMusic unless Gaud_GetMusicStatus says not to.
void fn_80084BE8(MsgArg* pArgs, MsgArg* pResult) {
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

// A name typed as nothing but spaces becomes "User <n>" for the working slot.
void fn_80084C88(MsgArg* pArgs, MsgArg* pResult) {
    char* szName = ((MsgString*)pArgs[0].p)->pStr;
    int n = 0;
    int i;

    for (i = 0; szName[i] != '\0'; i++) {
        if (szName[i] != ' ') {
            n++;
        }
    }
    if (n == 0) {
        sprintf(szName, "User %d", lbl_80281ED4->nSlot + 1);
    }
}

// Checks the card pArgs[0], pArgs[1] with MC_GetUser: 1 when it succeeds, -1 when the file read
// back is not a good save, else 0.
void fn_80084CFC(MsgArg* pArgs, MsgArg* pResult) {
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

// Copy a string, cut to eight characters and "..." when it is longer than 12.
void fn_80084D6C(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 12) {
        ((MsgString*)pArgs[1].p)->pStr[11] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[10] = '.';
        ((MsgString*)pArgs[1].p)->pStr[9] = '.';
        ((MsgString*)pArgs[1].p)->pStr[8] = '.';
    }
}

// The same, to 28 characters and "..." when it is longer than 32.
void fn_80084DF4(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[1].p)->pStr, ((MsgString*)pArgs[0].p)->pStr);
    if (strlen(((MsgString*)pArgs[0].p)->pStr) > 32) {
        ((MsgString*)pArgs[1].p)->pStr[31] = '\0';
        ((MsgString*)pArgs[1].p)->pStr[30] = '.';
        ((MsgString*)pArgs[1].p)->pStr[29] = '.';
        ((MsgString*)pArgs[1].p)->pStr[28] = '.';
    }
}

void fn_80084E7C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = lbl_80281ED4->n11704;
}

void fn_80084E90(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80125194(0, 0);
}

void fn_80084EC8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8009D390(pArgs[0].i, pArgs[1].i);
}

void fn_80084F04(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F08(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F0C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800A27F4();
}

void fn_80084F3C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80084F40(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_801252D0(pArgs[0].i, pArgs[1].i) == 0;
}

void fn_80084F84(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = EASBio_GetCurrentRewardMessage();
}

// The memory-card operations of the set picked (lbl_80281FFC), given each operation's own payload
// (core/memcard.h, MCOpCard).
s32 fn_80084FB4(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[4](pArg);
}

void fn_80084FF0(int n) {
    lbl_80281FFC = n;
}

s32 fn_80084FF8(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[2](pArg);
}

s32 fn_80085034(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[3](pArg);
}

s32 fn_80085070(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[1](pArg);
}

s32 fn_800850AC(void* pArg) {
    return lbl_8018C7D8[lbl_80281FFC].apfn[0](pArg);
}

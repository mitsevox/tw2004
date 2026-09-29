// GameUICommands.c (our name): the in-game UI messages, the questions and orders the menu UI can
// send while a round is on (session game types 4 to 8; uiProcessInterface.c's UI_RunGameMessage routes
// them here). IG_InitGameMessages fills gIGMessageHandlers, 214 slots with 212 handlers (0 and 119
// stay NULL), and IG_RunGameMessage runs one by number. Most answer a question about the round (a
// player's name and state, the score, the wind, the leaderboard, Battle mode's clubs, the PGA TOUR
// event) or act on it (pause, restart the hole, leave). TW07's UI_Core/InGame/APT_IG_GameMessages.c
// (a file TW06 also lists) has the same handlers, GM_v... / IG_v..., in the same order for long
// runs: the EA names here come from it. This build's own file name is not known.

#include "game.h"
#include "game/frontend.h"
#include "game/earnings.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/rte.h"
#include "core/memcard.h"
#include "core/startup.h"
#include "frontend/fe.h"

// The in-game UI messages by number (IG_InitGameMessages fills it; slots 0 and 119 stay NULL).
MsgHandler gIGMessageHandlers[UI_NUM_ROUND_COMMANDS];

// in reverse address order: CodeWarrior lays .sbss out last-defined-first
s32 gSpeedGolfLogCycle;         // -1..10, one step per GM_vGetSpeedGolfLogEntry call; the event
                                // answer it writes is always overwritten after
u8  gAlternateGolferUp;         // mode 26: the player the next golfer-up question answers (0 / 1)

// The message handlers, in address order (IG_InitGameMessages gives their numbers).
void GM_vGetRoundUIKind(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerShortName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerFirstName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerHoleScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerHolePoints(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCourseIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCurrentHoleIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCurrentGolferIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vExitGame(MsgArg* pArgs, MsgArg* pResult);
void GM_vClosePauseMenu(MsgArg* pArgs, MsgArg* pResult);
void GM_vPauseGame(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumberGolfersPlaying(MsgArg* pArgs, MsgArg* pResult);
void GM_vPostShotUIFinished(MsgArg* pArgs, MsgArg* pResult);
void GM_vPostShotUIStart(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerClubIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerShotTypeIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerStanceIndex(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerTargetDistance(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetClubDistance(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerCurrentLie(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerCurrentLieAngle(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerWindDirection(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerWindSpeed(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetScoringMethod(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetElapsedTimeSeconds(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumberMatchWins(MsgArg* pArgs, MsgArg* pResult);
void GM_vRestartCurHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage131_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferUserMoney(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsPostShotCameraDone(MsgArg* pArgs, MsgArg* pResult);
void GM_vScorecardGetPar(MsgArg* pArgs, MsgArg* pResult);
void GM_vHoleSelected(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetMatchWins(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSkinWins(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGameModeType(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfTotalTimeScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetModeValue(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLiePercentage(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetHoleRating(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTeeYardage(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerTee(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCurrentSkinAmount(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCurrentSkinNum(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLieModifier(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRelativeScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRoundScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_LastName(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_Position(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_Tied(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_Score(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_Hole(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_RoundScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_PlayerPosition(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_PlayerTied(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_Entries(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfHoleScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolfersUserName(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsGolferCPU(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPuttHelp(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_Winnings(MsgArg* pArgs, MsgArg* pResult);
void GM_vLeaderboard_PlayerWinnings(MsgArg* pArgs, MsgArg* pResult);
void GM_vLessonStopWaiting(MsgArg* pArgs, MsgArg* pResult);
void GM_vReturnTimer(MsgArg* pArgs, MsgArg* pResult);
void GM_vQuickScorecard(MsgArg* pArgs, MsgArg* pResult);
void GM_vControllerReconnected(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSlotUserName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumHolesSelected(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetDemoMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsPlayNowChallenge(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsPlayNowSpeedGolf(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayNowMedalMark(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayNowScoreToTarget(MsgArg* pArgs, MsgArg* pResult);
void GM_vShowAnalysisTip(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTipStat(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetOption(MsgArg* pArgs, MsgArg* pResult);
void GM_vConcede_Hole(MsgArg* pArgs, MsgArg* pResult);
void GM_vReportCaddieTipWindow(MsgArg* pArgs, MsgArg* pResult);
void GM_vDisableCaddieTips(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCMemforReplay(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCConnect(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCDisconnect(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_Multitap(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCExists(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCFormatted(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCfreeMem(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCGetNumReplays(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCfunction(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCFormat(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_SaveReplay(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_ReplaceReplay(MsgArg* pArgs, MsgArg* pResult);
void GM_vCurrentReplayValid(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumHolesRemaining(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerUIZoomedIn(MsgArg* pArgs, MsgArg* pResult);
void GM_vPracticeNextHole(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage92_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerShotSetup(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerBackSwing(MsgArg* pArgs, MsgArg* pResult);
void GM_vLessonsQuit(MsgArg* pArgs, MsgArg* pResult);
void GM_vLessonsContinue(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferController(MsgArg* pArgs, MsgArg* pResult);
void GM_vTimerOut(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerUIElevator(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfWinner(MsgArg* pArgs, MsgArg* pResult);
void GM_vFullRound(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfPrizeScores(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetChallengeHolesLeft(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage103_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage104_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetEventNameAndText(MsgArg* pArgs, MsgArg* pResult);
void GM_vPlayUISound(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfPoints(MsgArg* pArgs, MsgArg* pResult);
void GM_vGM_NextHoleDetails(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfRoundScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vCanControllerUseMenu(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCLookAtCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_ReplaySpaceNeeded(MsgArg* pArgs, MsgArg* pResult);
void GM_vPauseMenuClosed(MsgArg* pArgs, MsgArg* pResult);
void GM_vQuickCheer(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage115_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTargetShotInfo(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetTargetHoleStat(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsDemoSetup(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsPausedForController(MsgArg* pArgs, MsgArg* pResult);
void GM_vShotClockOut(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage122_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetStringLength(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRecordName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRecordScore(MsgArg* pArgs, MsgArg* pResult);
void GM_vFormatWithCommas(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetWrapupData(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRecordHolderName(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRecordValue(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetYardsToPin(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetSpeedGolfLogEntry(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage133_Return7(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetHoleContestWon(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsLadderEvent(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetRandInt(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage137_Return0(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage138_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage139_Return2And1(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage140_Return2(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage141_ReturnArg(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetLetter(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCGetCardErrors(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetCourseHoleName(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetMessageQueHeld(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCFreeFiles(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage147_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vAllowConcede(MsgArg* pArgs, MsgArg* pResult);
void IG_vEndGameLoop(MsgArg* pArgs, MsgArg* pResult);
void IG_vHoleContest_GetWinnerShotKind(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_GetGameName(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage153_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIsOnlineEvent(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCIsSaveCorrupt(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCDeleteSave(MsgArg* pArgs, MsgArg* pResult);
void GM_vSetUITimeFactor(MsgArg* pArgs, MsgArg* pResult);
void IG_vPlayCheerAndComment(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayoff(MsgArg* pArgs, MsgArg* pResult);
void IG_vPlayTextFall(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerCut(MsgArg* pArgs, MsgArg* pResult);
void GM_vRewardDisplayStarting(MsgArg* pArgs, MsgArg* pResult);
void IG_vMuteForScoreCard(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_MCGetSaveNeeds(MsgArg* pArgs, MsgArg* pResult);
void GM_vIG_OnlineSendChat(MsgArg* pArgs, MsgArg* pResult);
void GM_vOnlineMode(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage195_Return0(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetLongDriveStat(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage170_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage172_Empty(MsgArg* pArgs, MsgArg* pResult);
void IG_vIsDemoSetup(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage175_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetNumberClubs(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetClubAvailable(MsgArg* pArgs, MsgArg* pResult);
void GM_vAddRemoveClub(MsgArg* pArgs, MsgArg* pResult);
void IG_vSwapDiscReloadHole(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetDiscDriveStatus(MsgArg* pArgs, MsgArg* pResult);
void IG_vCloseMenuCheckDisc(MsgArg* pArgs, MsgArg* pResult);
void IG_vResumeCheckDisc(MsgArg* pArgs, MsgArg* pResult);
void GM_vIGMessage183_Empty(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetChallengeName(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetChallengeDescription(MsgArg* pArgs, MsgArg* pResult);
void IG_vAcceptOnlineInput(MsgArg* pArgs, MsgArg* pResult);
void IG_vBattle_GetNumClubsStart(MsgArg* pArgs, MsgArg* pResult);
void IG_vBattle_GetClubAddable(MsgArg* pArgs, MsgArg* pResult);
void IG_v_PGATourWin(MsgArg* pArgs, MsgArg* pResult);
void IG_v_PGATour_GetCheckInfo(MsgArg* pArgs, MsgArg* pResult);
void IG_vPgaTour_GetCurrEventName(MsgArg* pArgs, MsgArg* pResult);
void IG_vPlayShotScoreAnimSound(MsgArg* pArgs, MsgArg* pResult);
void IG_vPlayIngGameCommentaryRewardSound(MsgArg* pArgs, MsgArg* pResult);
void IG_vRewardCommentaryIsPlaying(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetStringSize(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetLocalUserIndex(MsgArg* pArgs, MsgArg* pResult);
void IG_vShow_Putting_Tip(MsgArg* pArgs, MsgArg* pResult);
void IG_vShotClockAction(MsgArg* pArgs, MsgArg* pResult);
void IG_vNumRemovableClubsLeft(MsgArg* pArgs, MsgArg* pResult);
void IG_vIsGameBreakerOn(MsgArg* pArgs, MsgArg* pResult);
void IG_vOnline_CheckPause(MsgArg* pArgs, MsgArg* pResult);
void IG_vBattleGolf_ShowClubUI(MsgArg* pArgs, MsgArg* pResult);
void IG_vBattleGolf_GetHoleWinner(MsgArg* pArgs, MsgArg* pResult);
void IG_IsGamePaused(MsgArg* pArgs, MsgArg* pResult);
void IG_vPGATour_EndofHole_message(MsgArg* pArgs, MsgArg* pResult);
void IG_vKeypopEnabled(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetRealTimeMode(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetCurrentPlayerNumberStrokes(MsgArg* pArgs, MsgArg* pResult);
void IG_vGetDisqualifiedGolfer(MsgArg* pArgs, MsgArg* pResult);
void IG_vLeaderboard_WasCut(MsgArg* pArgs, MsgArg* pResult);
void IG_vLeaderboard_PlayerWasCut(MsgArg* pArgs, MsgArg* pResult);

// This file's helpers.
u8   IG_IsControllerInPlay(int nController);
void OnlineGolf_OnEndOfGame(int a, int b);
u8   OnlineGolf_bIsOnlineGame(void);
void OnlineGolf_SendChatData(int nPlayer, char* sz);
void Gaud_StartPlaylist14Comment(int nMsg, int a);
void Gaud_StartPlaylist19Comment(int nMsg, int a);
void Gaud_StartPlaylist17Comment(int nMsg, int a);
void Gaud_StartPlaylist16Comment(int nMsg, int a);
void Gaud_StartPlaylist15Comment(int nMsg, int a);
void Gaud_StartPlaylist20Comment(int nMsg, int a);
void Gaud_StartPlaylist18Comment(int nMsg, int a);

// Other files' functions no header declares yet.
f32   GM_GetGolferDistanceToPin(int nPlayer);                 // GameMode.c
void  GM_GolferConcede_Hole(int nPlayer);                     // GameMode.c
void  GM_RestartHole(void);                                   // GameMode.c
void  GameModeBattle_AddClub(int nPlayer, int nClub);         // GameModeBattle.c
int   GameModeBattle_CanAddClub(int nPlayer, int nClub);
s32   GameModeBattle_GetHoleWinner(void);
u8    GameModeBattle_ShowEndOfHole_ClubAddRemove_UI(void);
int   GameModeBattle_NumRemovableClubsLeft(int nPlayer);
u8    GameModeBattle_RemoveClub(int nPlayer, int nClub);
void  Character_ReopenTextureFiles(void);
void  fn_80062B84(int a);
void  fn_8006F4E0(void);
s32   MC_CallActionFnMemoryRequired(CardPos* pPos);
void  MC_ConnectCard(s32 nPort, s32 nSlot);
s32   MC_SaveReplay(MCCardPos* pPos);
s32   fn_800A0610(s32 nPort, s32 nSlot, s32 n);
void  Gaud_TextFall(void);
void  Gaud_PlayTextDitty(int n);
void  Gaud_FireQuickCheer(void);
void  Gaud_Pause(int a);
void  Gaud_PlayGameUISound(u8 a, int b);
s32   Gaud_RewardCommentaryIsPlaying(void);
void  Gaud_RestartMusic(void);
u8    GolfCamera_IsPostShotCamDone(View* pView);
u8    GolfCamera_IsCameraTrackingPlayer(View* pView);
void  BreakLine_GetCaddyTipInfo(int nView, f32* pLong, f32* pSide); // GoBreakLine.c
s32   GM_GetHoleIndexHandicap(int nHole);
s32   GM_GetCurrentCourseTotalYardage(int nTeeSet);
s32   GM_GetCurrentCourseFront9Yardage(int nTeeSet);
s32   GM_GetCurrentCourseBack9Yardage(int nTeeSet);
s32   GM_GetCurrentCourseFront9Par(void);
s32   GM_GetCurrentCourseBack9Par(void);
f32   GM_Earnings_GetCourseModifier(void);
char* HoleContest_GetPlaceName(int nPlayer);
s32   HoleContest_GetPlaceDistance(int nPlayer);
u8    HoleContest_IsWonThisRound(void);
s32   HoleContest_WasEverWon(void);
s32   HoleContest_GetWinnerShotKind(void);
int   GM_GetNextSelectedHole(void);
int   GM_GetPlayerRoundScoreThroughHole(int nPlayer, int nHoles);
void  GUI_OpenPauseMenu(void);
void  GUI_SetEndOfGamePending(void);
void  GUI_PostShotUIStart(int i);
void  GUI_PostShotUIFinished(int i);
void  GUI_CaddieTipWindowIsOpen(void);
void  GUI_CaddieTipWindowClosed(void);
void  GUI_SetMessageQueHeld(u8 b);
void  GUI_StartAwardUI(void);
void  GUI_MuteForScoreCard(void);
void  GUI_SetUnreadFlag(u8 b);
void  GUI_QueueTip(int n);
int   GameAnalysis_PickTip(void);
f32   GameAnalysis_GetTipStat(int nPlayer, u32 nStat);
s32   GameModeBattle_GetNumberStartingClubs(int nPlayer);
int   GM_BestBallMode_GetTeamRelativeScore(int nPlayer, u8 bCurrent);
u8    PlayNow_IsSpeedGolf(void);
s32   PlayNow_GetMedalMark(int k);
int   PlayNow_GetScoreToTarget(void);
char* PlayNow_GetGroupName(int nGroup);
char* PlayNow_GetGroupDescription(int nGroup);
int   PlayNow_GetHolesLeft(void);
void  PlayNow_Restart(void);
void  GameModePractice_FinishHole(void);
PgaTour_WinInfo* GameModeDriverPGATour_GetWinInfo(void);
s32   GameModeDriverPGATour_DisplayEndOfHoleMessage(char* pDst);
s32   GM_RealtimeMode_GetSelectedEvent(s32* pRound);
int   GameModeSkillZoneBase_GetCupCount(void);
void  GameModeSkillZoneBase_TimerOut(void);
void  GameModeSkillZoneBase_ShotClockOut(void);
s32   GameModeSkillZoneBase_GetShotEarned(s32 nPlayer);
s32   GameModeSkillZoneBase_GetTimeEarned(s32 nPlayer);
s32   GameModeSkillZoneBase_GetDriveMultiplier(s32 nPlayer);
s32   GameModeSkillZoneBase_GetExtraBallsEarned(s32 nPlayer);
s32   GameModeSkillZoneCapture_GetTargettedRingOwner(int nPlayer);
s32   GameModeSkillZoneCapture_GetTargettedCapturedRing(int nPlayer);
s32   GameModeSkillZoneCapture_GetRingOwnerFromIndex(int i);
s32   GameModeSkillZoneCapture_GetCapturedRingFromIndex(s32 p0);
s32   GameModeSkillZoneCapture_GetMadeMoneyFromIndex(int n);
s8    GameModeSkillZoneHorse_GetCurrentLeaderRing(void);
s32   GameModeSkillZoneHorse_GetLastShotExceeded(void);
void  GameModeSkillZoneTimed_TenSecWarning(void);
s32   GameModeSkins_CurrentHoleNumberSkins(void);
s32   SpeedGolf_GetTotalTimeScore(int nPlayer);
s32   SpeedGolf_GetHoleScore(int nPlayer, int nHole, s32* pWon);
s32   SpeedGolfPoints_GetNamesAndPoints(char* szName1, s32* pPoints1, char* szName2, s32* pPoints2);
void  SpeedGolf_TimerOut(void);
void  SpeedGolf_GetPrizeScores(s32* p0, s32* p1, s32* p2);
s32   SpeedGolf_GetWinner(s32* pMoney);
s32   SpeedGolf_GetRoundScore(char* szName, s32* pSeconds, s32* pStrokes, s32* pScore);
s32   GameMode12_GetShotMultiplier(int nPlayer);
s32   GameMode12_GetBonusMeter(int nPlayer);
s32   GameMode12_GetHolePoints(int nPlayer);
s32   GameMode12_GetRoundPoints(int nPlayer);
s32   GameMode12_GetNumScoredSurfaces(int nPlayer);   // GameMode12.c defines it without the (unused) player
s32   GameMode12_GetScoredSurface(int nPlayer, int i);
s32   GameMode12_GetScoredSurfaceHits(int nPlayer, int i);
void  GameMode12_ListScoredSurfaces(int nPlayer);
void  Lessons_StopWaiting(void);
void  Lessons_RestartLesson(void);
void  Lessons_ChooseQuit(void);
void  Lessons_ChooseContinue(void);
s32   GameMode22_GetVariant(void);
s32   GameMode22_GetHoleRecordIndex(s32 n);

// Run in-game UI message nCmd: the handler in gIGMessageHandlers[nCmd] gets the message's values
// (pArgs) and its answer (pResult). The slot is not checked: 0 and 119 are NULL.
void IG_RunGameMessage(int nCmd, MsgArg* pArgs, MsgArg* pResult) {
    gIGMessageHandlers[nCmd](pArgs, pResult);
}

// Fill gIGMessageHandlers, the in-game UI messages by number (called from GO_vInitIG): every slot
// NULL, then 212 handlers; slots 0 and 119 stay NULL.
void IG_InitGameMessages(void) {
    int i;

    for (i = 0; i < UI_NUM_ROUND_COMMANDS; i++) {
        gIGMessageHandlers[i] = NULL;
    }
    gIGMessageHandlers[1] = GM_vGetRoundUIKind;
    gIGMessageHandlers[2] = GM_vGetPlayerName;
    gIGMessageHandlers[3] = GM_vGetPlayerHoleScore;
    gIGMessageHandlers[4] = GM_vGetCourseIndex;
    gIGMessageHandlers[5] = GM_vGetCurrentHoleIndex;
    gIGMessageHandlers[6] = GM_vGetCurrentGolferIndex;
    gIGMessageHandlers[7] = GM_vExitGame;
    gIGMessageHandlers[8] = GM_vClosePauseMenu;
    gIGMessageHandlers[9] = GM_vPauseGame;
    gIGMessageHandlers[10] = GM_vGetNumberGolfersPlaying;
    gIGMessageHandlers[11] = GM_vPostShotUIFinished;
    gIGMessageHandlers[12] = GM_vPostShotUIStart;
    gIGMessageHandlers[13] = GM_vGetPlayerClubIndex;
    gIGMessageHandlers[14] = GM_vGetPlayerShotTypeIndex;
    gIGMessageHandlers[15] = GM_vGetPlayerStanceIndex;
    gIGMessageHandlers[16] = GM_vGetPlayerTargetDistance;
    gIGMessageHandlers[17] = GM_vGetClubDistance;
    gIGMessageHandlers[18] = GM_vGetPlayerCurrentLie;
    gIGMessageHandlers[19] = GM_vGetPlayerCurrentLieAngle;
    gIGMessageHandlers[20] = GM_vGetPlayerWindDirection;
    gIGMessageHandlers[21] = GM_vGetPlayerWindSpeed;
    gIGMessageHandlers[22] = GM_vGetScoringMethod;
    gIGMessageHandlers[23] = GM_vGetElapsedTimeSeconds;
    gIGMessageHandlers[24] = GM_vGetNumberMatchWins;
    gIGMessageHandlers[25] = GM_vRestartCurHole;
    gIGMessageHandlers[26] = GM_vGetGolferUserMoney;
    gIGMessageHandlers[27] = GM_vIsPostShotCameraDone;
    gIGMessageHandlers[28] = GM_vScorecardGetPar;
    gIGMessageHandlers[29] = GM_vHoleSelected;
    gIGMessageHandlers[30] = GM_vGetMatchWins;
    gIGMessageHandlers[31] = GM_vGetSkinWins;
    gIGMessageHandlers[32] = GM_vGetGameModeType;
    gIGMessageHandlers[33] = GM_vGetSpeedGolfTotalTimeScore;
    gIGMessageHandlers[34] = GM_vGetLiePercentage;
    gIGMessageHandlers[35] = GM_vGetHoleRating;
    gIGMessageHandlers[36] = GM_vGetTeeYardage;
    gIGMessageHandlers[37] = GM_vGetPlayerTee;
    gIGMessageHandlers[38] = GM_vGetCurrentSkinAmount;
    gIGMessageHandlers[39] = GM_vGetCurrentSkinNum;
    gIGMessageHandlers[40] = GM_vGetLieModifier;
    gIGMessageHandlers[41] = GM_vGetRelativeScore;
    gIGMessageHandlers[42] = GM_vGetRoundScore;
    gIGMessageHandlers[43] = GM_vLeaderboard_LastName;
    gIGMessageHandlers[44] = GM_vLeaderboard_Position;
    gIGMessageHandlers[45] = GM_vLeaderboard_Tied;
    gIGMessageHandlers[46] = GM_vLeaderboard_Score;
    gIGMessageHandlers[47] = GM_vLeaderboard_Hole;
    gIGMessageHandlers[48] = GM_vLeaderboard_RoundScore;
    gIGMessageHandlers[49] = GM_vLeaderboard_PlayerPosition;
    gIGMessageHandlers[50] = GM_vLeaderboard_PlayerTied;
    gIGMessageHandlers[51] = GM_vLeaderboard_Entries;
    gIGMessageHandlers[52] = GM_vGetSpeedGolfHoleScore;
    gIGMessageHandlers[53] = GM_vGetGolfersUserName;
    gIGMessageHandlers[54] = GM_vIsGolferCPU;
    gIGMessageHandlers[55] = GM_vGetPuttHelp;
    gIGMessageHandlers[56] = GM_vLeaderboard_Winnings;
    gIGMessageHandlers[57] = GM_vLeaderboard_PlayerWinnings;
    gIGMessageHandlers[58] = GM_vLessonStopWaiting;
    gIGMessageHandlers[59] = GM_vReturnTimer;
    gIGMessageHandlers[60] = GM_vQuickScorecard;
    gIGMessageHandlers[61] = GM_vControllerReconnected;
    gIGMessageHandlers[62] = GM_vGetSlotUserName;
    gIGMessageHandlers[63] = GM_vGetNumHolesSelected;
    gIGMessageHandlers[64] = GM_vGetDemoMode;
    gIGMessageHandlers[65] = GM_vIsPlayNowChallenge;
    gIGMessageHandlers[66] = GM_vIsPlayNowSpeedGolf;
    gIGMessageHandlers[67] = GM_vGetPlayNowMedalMark;
    gIGMessageHandlers[68] = GM_vGetPlayNowScoreToTarget;
    gIGMessageHandlers[69] = GM_vShowAnalysisTip;
    gIGMessageHandlers[70] = GM_vGetTipStat;
    gIGMessageHandlers[71] = GM_vGetOption;
    gIGMessageHandlers[72] = GM_vSetOption;
    gIGMessageHandlers[73] = GM_vConcede_Hole;
    gIGMessageHandlers[74] = GM_vReportCaddieTipWindow;
    gIGMessageHandlers[75] = GM_vDisableCaddieTips;
    gIGMessageHandlers[76] = GM_vIG_MCMemforReplay;
    gIGMessageHandlers[77] = GM_vIG_MCConnect;
    gIGMessageHandlers[78] = GM_vIG_MCDisconnect;
    gIGMessageHandlers[79] = GM_vIG_Multitap;
    gIGMessageHandlers[80] = GM_vIG_MCExists;
    gIGMessageHandlers[81] = GM_vIG_MCFormatted;
    gIGMessageHandlers[82] = GM_vIG_MCfreeMem;
    gIGMessageHandlers[83] = GM_vIG_MCGetNumReplays;
    gIGMessageHandlers[84] = GM_vIG_MCfunction;
    gIGMessageHandlers[85] = GM_vIG_MCFormat;
    gIGMessageHandlers[86] = GM_vIG_SaveReplay;
    gIGMessageHandlers[87] = GM_vIG_ReplaceReplay;
    gIGMessageHandlers[88] = GM_vCurrentReplayValid;
    gIGMessageHandlers[89] = GM_vGetNumHolesRemaining;
    gIGMessageHandlers[90] = GM_vGetPlayerUIZoomedIn;
    gIGMessageHandlers[91] = GM_vPracticeNextHole;
    gIGMessageHandlers[92] = GM_vIGMessage92_Empty;
    gIGMessageHandlers[93] = GM_vGetPlayerShotSetup;
    gIGMessageHandlers[94] = GM_vLessonsQuit;
    gIGMessageHandlers[95] = GM_vLessonsContinue;
    gIGMessageHandlers[96] = GM_vGetGolferController;
    gIGMessageHandlers[97] = GM_vTimerOut;
    gIGMessageHandlers[98] = GM_vGetPlayerUIElevator;
    gIGMessageHandlers[99] = GM_vGetSpeedGolfWinner;
    gIGMessageHandlers[100] = GM_vFullRound;
    gIGMessageHandlers[101] = GM_vGetSpeedGolfPrizeScores;
    gIGMessageHandlers[102] = GM_vGetChallengeHolesLeft;
    gIGMessageHandlers[103] = GM_vIGMessage103_Empty;
    gIGMessageHandlers[104] = GM_vIGMessage104_Empty;
    gIGMessageHandlers[105] = GM_vGetEventNameAndText;
    gIGMessageHandlers[106] = GM_vPlayUISound;
    gIGMessageHandlers[107] = GM_vGetSpeedGolfPoints;
    gIGMessageHandlers[108] = GM_vGM_NextHoleDetails;
    gIGMessageHandlers[109] = GM_vGetSpeedGolfRoundScore;
    gIGMessageHandlers[110] = GM_vCanControllerUseMenu;
    gIGMessageHandlers[111] = GM_vIG_MCLookAtCard;
    gIGMessageHandlers[112] = GM_vIG_ReplaySpaceNeeded;
    gIGMessageHandlers[113] = GM_vPauseMenuClosed;
    gIGMessageHandlers[114] = GM_vQuickCheer;
    gIGMessageHandlers[115] = GM_vIGMessage115_Return0;
    gIGMessageHandlers[116] = GM_vGetModeValue;
    gIGMessageHandlers[117] = GM_vGetTargetShotInfo;
    gIGMessageHandlers[118] = GM_vGetTargetHoleStat;
    gIGMessageHandlers[120] = GM_vIsPausedForController;
    gIGMessageHandlers[121] = GM_vShotClockOut;
    gIGMessageHandlers[122] = GM_vIGMessage122_Empty;
    gIGMessageHandlers[123] = GM_vGetStringLength;
    gIGMessageHandlers[124] = GM_vGetRecordName;
    gIGMessageHandlers[125] = GM_vGetRecordScore;
    gIGMessageHandlers[126] = GM_vFormatWithCommas;
    gIGMessageHandlers[127] = GM_vGetWrapupData;
    gIGMessageHandlers[128] = GM_vGetRecordHolderName;
    gIGMessageHandlers[129] = GM_vGetRecordValue;
    gIGMessageHandlers[130] = GM_vGetYardsToPin;
    gIGMessageHandlers[131] = GM_vIGMessage131_Empty;
    gIGMessageHandlers[132] = GM_vGetSpeedGolfLogEntry;
    gIGMessageHandlers[133] = GM_vIGMessage133_Return7;
    gIGMessageHandlers[134] = GM_vGetHoleContestWon;
    gIGMessageHandlers[135] = GM_vIsLadderEvent;
    gIGMessageHandlers[136] = GM_vGetRandInt;
    gIGMessageHandlers[137] = GM_vIGMessage137_Return0;
    gIGMessageHandlers[138] = GM_vIGMessage138_Empty;
    gIGMessageHandlers[139] = GM_vIGMessage139_Return2And1;
    gIGMessageHandlers[140] = GM_vIGMessage140_Return2;
    gIGMessageHandlers[141] = GM_vIGMessage141_ReturnArg;
    gIGMessageHandlers[142] = GM_vGetLetter;
    gIGMessageHandlers[143] = GM_vIG_MCGetCardErrors;
    gIGMessageHandlers[144] = GM_vGetCourseHoleName;
    gIGMessageHandlers[145] = GM_vSetMessageQueHeld;
    gIGMessageHandlers[146] = GM_vIG_MCFreeFiles;
    gIGMessageHandlers[147] = GM_vIGMessage147_Empty;
    gIGMessageHandlers[148] = GM_vAllowConcede;
    gIGMessageHandlers[149] = IG_vEndGameLoop;
    gIGMessageHandlers[150] = IG_vHoleContest_GetWinnerShotKind;
    gIGMessageHandlers[151] = GM_vIG_GetGameName;
    gIGMessageHandlers[152] = GM_vGetPlayerShortName;
    gIGMessageHandlers[153] = GM_vIGMessage153_Empty;
    gIGMessageHandlers[154] = GM_vIsOnlineEvent;
    gIGMessageHandlers[155] = GM_vIG_MCIsSaveCorrupt;
    gIGMessageHandlers[156] = GM_vIG_MCDeleteSave;
    gIGMessageHandlers[157] = GM_vSetUITimeFactor;
    gIGMessageHandlers[158] = IG_vPlayCheerAndComment;
    gIGMessageHandlers[159] = GM_vGetPlayoff;
    gIGMessageHandlers[160] = IG_vPlayTextFall;
    gIGMessageHandlers[161] = GM_vGetPlayerCut;
    gIGMessageHandlers[162] = GM_vRewardDisplayStarting;
    gIGMessageHandlers[163] = IG_vMuteForScoreCard;
    gIGMessageHandlers[164] = GM_vMCHadIOError;
    gIGMessageHandlers[165] = GM_vGetPlayerBackSwing;
    gIGMessageHandlers[166] = GM_vIG_MCGetSaveNeeds;
    gIGMessageHandlers[167] = GM_vIG_OnlineSendChat;
    gIGMessageHandlers[168] = GM_vOnlineMode;
    gIGMessageHandlers[169] = IG_vGetLongDriveStat;
    gIGMessageHandlers[170] = GM_vIGMessage170_Empty;
    gIGMessageHandlers[171] = GM_vIsDemoSetup;
    gIGMessageHandlers[172] = GM_vIGMessage172_Empty;
    gIGMessageHandlers[173] = IG_vIsDemoSetup;
    gIGMessageHandlers[174] = GM_vGetPlayerFirstName;
    gIGMessageHandlers[175] = GM_vIGMessage175_Empty;
    gIGMessageHandlers[176] = GM_vGetNumberClubs;
    gIGMessageHandlers[177] = GM_vGetClubAvailable;
    gIGMessageHandlers[178] = GM_vAddRemoveClub;
    gIGMessageHandlers[180] = IG_vSwapDiscReloadHole;
    gIGMessageHandlers[179] = IG_vGetDiscDriveStatus;
    gIGMessageHandlers[181] = IG_vCloseMenuCheckDisc;
    gIGMessageHandlers[197] = IG_vResumeCheckDisc;
    gIGMessageHandlers[182] = GM_vGetPlayerHolePoints;
    gIGMessageHandlers[184] = IG_vGetChallengeName;
    gIGMessageHandlers[185] = IG_vGetChallengeDescription;
    gIGMessageHandlers[183] = GM_vIGMessage183_Empty;
    gIGMessageHandlers[186] = IG_vAcceptOnlineInput;
    gIGMessageHandlers[187] = IG_vBattle_GetNumClubsStart;
    gIGMessageHandlers[188] = IG_vBattle_GetClubAddable;
    gIGMessageHandlers[189] = IG_v_PGATourWin;
    gIGMessageHandlers[190] = IG_v_PGATour_GetCheckInfo;
    gIGMessageHandlers[191] = IG_vPgaTour_GetCurrEventName;
    gIGMessageHandlers[192] = IG_vPlayShotScoreAnimSound;
    gIGMessageHandlers[193] = IG_vPlayIngGameCommentaryRewardSound;
    gIGMessageHandlers[194] = IG_vRewardCommentaryIsPlaying;
    gIGMessageHandlers[195] = GM_vIGMessage195_Return0;
    gIGMessageHandlers[196] = IG_vGetStringSize;
    gIGMessageHandlers[198] = IG_vGetLocalUserIndex;
    gIGMessageHandlers[199] = IG_vShow_Putting_Tip;
    gIGMessageHandlers[200] = IG_vShotClockAction;
    gIGMessageHandlers[201] = IG_vNumRemovableClubsLeft;
    gIGMessageHandlers[202] = IG_vIsGameBreakerOn;
    gIGMessageHandlers[203] = IG_vOnline_CheckPause;
    gIGMessageHandlers[204] = IG_vBattleGolf_ShowClubUI;
    gIGMessageHandlers[205] = IG_vBattleGolf_GetHoleWinner;
    gIGMessageHandlers[206] = IG_IsGamePaused;
    gIGMessageHandlers[207] = IG_vPGATour_EndofHole_message;
    gIGMessageHandlers[208] = IG_vKeypopEnabled;
    gIGMessageHandlers[209] = IG_vGetRealTimeMode;
    gIGMessageHandlers[210] = IG_vGetCurrentPlayerNumberStrokes;
    gIGMessageHandlers[211] = IG_vGetDisqualifiedGolfer;
    gIGMessageHandlers[212] = IG_vLeaderboard_WasCut;
    gIGMessageHandlers[213] = IG_vLeaderboard_PlayerWasCut;
}

// Whether controller nController has a player in play: always 1 outside session game type 6; in it,
// 1 when some player on that controller, or any CPU player, is not waiting (GS_WAIT; in mode 7 his
// state does not matter).
u8 IG_IsControllerInPlay(int nController) {
    int i;

    if (gSession.nGameType != 6) {
        return 1;
    }
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (((s8)GOLFERSTATE_GetCurrentState(i) != GS_WAIT || Game_GetMode() == 7) &&
            (nController == PLAYER(i)->nController || Player_IsCPU(i))) {
            return 1;
        }
    }
    return 0;
}

// Message 1: which kind of round the menus are in: 3 speed golf (mode 8), 2 the two-player
// speed-golf modes (6, 7), 1 split screen, else 0.
void GM_vGetRoundUIKind(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 8) {
        pResult->i = 3;
        return;
    }
    if (Game_GetMode() == 6 || Game_GetMode() == 7) {
        pResult->i = 2;
        return;
    }
    if (gSession.nSplitScreen != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Message 2: the name player pArgs[0] is shown by, into the string pArgs[1]: a created golfer's
// last name, else the golfer's nickname when he has one ("NA" = none), else his last name (Cedric
// the Entertainer is shown as "CEDRIC").
void GM_vGetPlayerName(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfer = gSession.nGolfer[pArgs[0].i];

    if (nGolfer >= FIRST_CREATED_GOLFER) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gGolferTable[nGolfer].szLast);
        return;
    }
    if (strcmp(gPlayers[pArgs[0].i].golfer.szNick, "NA") != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szNick);
        return;
    }
    if (stricmp(gPlayers[pArgs[0].i].golfer.szLast, "the entertainer") == 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "CEDRIC");
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szLast);
}

// Message 152: GM_vGetPlayerName without the "CEDRIC" case; a last name longer than 10 letters is
// cut to its first four.
void GM_vGetPlayerShortName(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfer = gSession.nGolfer[pArgs[0].i];
    char szName[32];

    if (nGolfer >= FIRST_CREATED_GOLFER) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gGolferTable[nGolfer].szLast);
        return;
    }
    if (strcmp(gPlayers[pArgs[0].i].golfer.szNick, "NA") != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szNick);
        return;
    }
    if (strlen(gPlayers[pArgs[0].i].golfer.szLast) > 10) {
        strcpy(szName, gPlayers[pArgs[0].i].golfer.szLast);
        szName[4] = '\0';
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szLast);
}

// Message 174: player pArgs[0]'s first name (a created golfer's from the golfer table) into the
// string pArgs[1].
void GM_vGetPlayerFirstName(MsgArg* pArgs, MsgArg* pResult) {
    int nGolfer = gSession.nGolfer[pArgs[0].i];

    if (nGolfer >= FIRST_CREATED_GOLFER) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, gGolferTable[nGolfer].szFirst);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gPlayers[pArgs[0].i].golfer.szFirst);
}

// Message 3: player pArgs[0]'s strokes on hole pArgs[1] (0..17); "hole" 18 is the front nine, 19
// the back nine, 20 the round. In best ball (mode 19) a hole is the team's score.
void GM_vGetPlayerHoleScore(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i == 18) {
        pResult->i = GM_GetPlayerRoundScoreThroughHole(pArgs[0].i, 9);
    } else if (pArgs[1].i == 19) {
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i) - GM_GetPlayerRoundScoreThroughHole(pArgs[0].i, 9);
    } else if (pArgs[1].i == 20) {
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i);
    } else if (Game_GetMode() == 19) {
        pResult->i = GM_BestBallMode_GetTeamHoleScore((u8)pArgs[0].i, pArgs[1].i);
    } else {
        pResult->i = gPlayers[pArgs[0].i].nStrokes[pArgs[1].i];
    }
}

// Message 182: the same as GM_vGetPlayerHoleScore, but a single hole answers the mode's points on
// it (nModePoints); the 18/19/20 totals are still strokes.
void GM_vGetPlayerHolePoints(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i == 18) {
        pResult->i = GM_GetPlayerRoundScoreThroughHole(pArgs[0].i, 9);
    } else if (pArgs[1].i == 19) {
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i) - GM_GetPlayerRoundScoreThroughHole(pArgs[0].i, 9);
    } else if (pArgs[1].i == 20) {
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i);
    } else {
        pResult->i = gPlayers[pArgs[0].i].nModePoints[pArgs[1].i];
    }
}

// Message 4: the round's course (Game_GetCourse); -1 when any of gpGame->bRandom18, bDream18 or
// nRegionalRound is set (TW07 tests compilation and random courses here), -2 for a custom course
// (bCustomRound).
void GM_vGetCourseIndex(MsgArg* pArgs, MsgArg* pResult) {
    if (gpGame->bRandom18 != 0 || gpGame->bDream18 != 0 || gpGame->nRegionalRound != 0) {
        pResult->i = -1;
        return;
    }
    if (gpGame->bCustomRound != 0) {
        pResult->i = -2;
        return;
    }
    pResult->i = Game_GetCourse();
}

// Message 5: the current hole, 0..17 in the round (Game_CurHoleIndex).
void GM_vGetCurrentHoleIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_CurHoleIndex();
}

// Message 6: the player whose turn it is (lbl_80282278); in mode 26 (two-player long drive) the
// answer alternates 0, 1, 0... each time it is asked (gAlternateGolferUp).
void GM_vGetCurrentGolferIndex(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 26) {
        pResult->i = gAlternateGolferUp;
        gAlternateGolferUp = 1 - gAlternateGolferUp;
        return;
    }
    pResult->i = lbl_80282278;
}

// Message 7: leave the round: in mode 9 the end of the game is set pending
// (GUI_SetEndOfGamePending), in any other the fade to black starts (gUIState.bFadeToBlack); in
// the PGA TOUR (mode 23) GM_PgaTourSim_SetUserQuit(0, 1). The online branch
// (OnlineGolf_bIsOnlineGame, always 0 in this build) never runs.
void GM_vExitGame(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 9) {
        GUI_SetEndOfGamePending();
    } else {
        gUIState.bFadeToBlack = 1;
    }
    if (Game_GetMode() == 23) {
        GM_PgaTourSim_SetUserQuit(0, 1);
    }
    if (OnlineGolf_bIsOnlineGame()) {
        fn_80062B84(6);
        OnlineGolf_OnEndOfGame(0, 1);
    }
}

// Message 8: the pause menu closes (GUI_PauseMenuClosed); unless the round is fading to black to
// end, the lesson restarts (Lessons_RestartLesson, lesson mode only); when it is, a hole load asked
// for is cancelled (fn_8006F4E0).
void GM_vClosePauseMenu(MsgArg* pArgs, MsgArg* pResult) {
    GUI_PauseMenuClosed();
    if (gUIState.bFadeToBlack == 0) {
        Lessons_RestartLesson();
        return;
    }
    fn_8006F4E0();
}

// Message 9: pause: notes whether the scorecard was down (gbPausedWithoutScoreCard), pauses the
// sound and opens the pause menu.
void GM_vPauseGame(MsgArg* pArgs, MsgArg* pResult) {
    gbPausedWithoutScoreCard = GUI_ScoreCardUp() == 0;
    Gaud_Pause(1);
    GUI_OpenPauseMenu();
}

// Message 10: how many golfers play the round (gNumPlayersSetUp).
void GM_vGetNumberGolfersPlaying(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gNumPlayersSetUp;
}

// Message 11: GUI_PostShotUIFinished for player pArgs[0].
void GM_vPostShotUIFinished(MsgArg* pArgs, MsgArg* pResult) {
    GUI_PostShotUIFinished(pArgs[0].i);
}

// Message 12: GUI_PostShotUIStart for player pArgs[0].
void GM_vPostShotUIStart(MsgArg* pArgs, MsgArg* pResult) {
    GUI_PostShotUIStart(pArgs[0].i);
}

// Message 13: player pArgs[0]'s club (Player.nClub).
void GM_vGetPlayerClubIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nClub;
}

// Message 14: player pArgs[0]'s kind of shot (Player.nShotKind).
void GM_vGetPlayerShotTypeIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nShotKind;
}

// Message 15: player pArgs[0]'s stance, as EA calls it: Player.nTrajectory (0 low, 1 normal, 2
// high).
void GM_vGetPlayerStanceIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nTrajectory;
}

// Message 16: player pArgs[0]'s distance to the pin (GM_GetGolferDistanceToPin), as a float.
void GM_vGetPlayerTargetDistance(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = GM_GetGolferDistanceToPin(pArgs[0].i);
}

// Message 17: how far player pArgs[0] can hit club pArgs[2] for kind of shot pArgs[1]
// (AI_MaxDistance), as a float.
void GM_vGetClubDistance(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = AI_MaxDistance(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// Message 18: the lie of player pArgs[0]'s ball (ball.nLie), or 99 when the ball is on surface 151.
void GM_vGetPlayerCurrentLie(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].ball.nSurface == 151) {
        pResult->i = 99;
        return;
    }
    pResult->i = gPlayers[pArgs[0].i].ball.nLie;
}

// Message 19: the lie angle of player pArgs[0]'s ball (ball.nLieAngle).
void GM_vGetPlayerCurrentLieAngle(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].ball.nLieAngle;
}

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283B10), before the 2 pi GM_vGetPlayerWindDirection uses first; its body is unknown.
static f32 GameUICommands_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Message 20: the wind's direction against player pArgs[0]'s aim, as a float 0..8 (eighths of a
// turn).
void GM_vGetPlayerWindDirection(MsgArg* pArgs, MsgArg* pResult) {
    f32 fAim = gPlayers[pArgs[0].i].fAim;
    f32 vWind[3];
    f32 fAngle;

    Wind_GetPhysicsWindVelocity(vWind);
    fAngle = atan2f(-vWind[0], vWind[2]) - fAim;
    while (fAngle < 0.0f) {
        fAngle += TWOPI;
    }
    while (fAngle > TWOPI) {
        fAngle -= TWOPI;
    }
    pResult->f = 8.0f * (fAngle / TWOPI);
}

// Message 21: the wind's speed (Wind_GetPhysicsWindVelocity).
void GM_vGetPlayerWindSpeed(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Wind_GetPhysicsWindVelocity(NULL);
}

// Message 22: the round's scoring method (GM_GetScoringType: gpGame->nScoringType).
void GM_vGetScoringMethod(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetScoringType();
}

// Message 23: the time spent on the hole so far (GM_GetElapsedHoleTime).
void GM_vGetElapsedTimeSeconds(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetElapsedHoleTime();
}

// Message 24: player pArgs[0]'s mode points on hole pArgs[1] (nModePoints; in match play 1 = hole
// won).
void GM_vGetNumberMatchWins(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nModePoints[pArgs[1].i];
}

// Message 25: restart the hole: replay off, event 2 for the player up, GM_RestartHole; a PlayNow
// challenge restarts too, and the long-drive modes (22, 26) restart their music.
void GM_vRestartCurHole(MsgArg* pArgs, MsgArg* pResult) {
    gSession.bReplay = 0;
    gReplayData.bF10 = 0;
    EVENT_Trigger(lbl_80282278, 2, NULL, 0);
    GM_RestartHole();
    if (PlayNow_IsChallengeRunning()) {
        PlayNow_Restart();
    }
    if (Game_GetMode() == 26 || Game_GetMode() == 22) {
        Gaud_RestartMusic();
    }
}

// Message 131: empty in this build.
void GM_vIGMessage131_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Message 26: the money in player pArgs[0]'s save profile (n6C), 0 when the profile is not active.
void GM_vGetGolferUserMoney(MsgArg* pArgs, MsgArg* pResult) {
    if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
        pResult->i = 0;
        return;
    }
    pResult->i = gpSaveData[gPlayers[pArgs[0].i].nIndex].nCurrentCash;
}

// Message 27: whether the post-shot camera is done with player pArgs[0] (1; always for players 5
// and up): 1 when he is not in a reaction animation (nCurState not 9, 11 or 12), or when the
// camera's script has ended (GolfCamera_IsPostShotCamDone) and either it is not tracking him
// (GolfCamera_IsCameraTrackingPlayer) or his animation is paused or has less than the camera
// tuning's f170 left.
void GM_vIsPostShotCameraDone(MsgArg* pArgs, MsgArg* pResult) {
    u8 bPostShotCamDone;        // the locals are TW07's
    u8 bCamTrackingPlayer;
    u8 bAnimPaused;             // the golfer's uFlags bit 0
    u8 bNotReactionAnim;        // the golfer's nCurState is not 9, 11 or 12
    u8 bAnimAlmostDone;         // his animation has less than the camera tuning's f170 left

    if (pArgs[0].i >= 5) {
        pResult->i = 1;
        return;
    }
    bPostShotCamDone = GolfCamera_IsPostShotCamDone(
            ViewController_GetCameraControl(gPlayers[pArgs[0].i].nView[0]));
    bCamTrackingPlayer = GolfCamera_IsCameraTrackingPlayer(
            ViewController_GetCameraControl(gPlayers[pArgs[0].i].nView[0]));
    bAnimPaused = fn_80062C1C(gPlayers[pArgs[0].i].pChar);
    bNotReactionAnim = gPlayers[pArgs[0].i].pChar->nCurState != 9 &&
                       gPlayers[pArgs[0].i].pChar->nCurState != 11 &&
                       gPlayers[pArgs[0].i].pChar->nCurState != 12;
    bAnimAlmostDone = fn_80062C28(gPlayers[pArgs[0].i].pChar) < gpCamTuning->f170;
    if ((bPostShotCamDone && (!bCamTrackingPlayer || bAnimPaused || bAnimAlmostDone)) || bNotReactionAnim) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Message 28: hole pArgs[0]'s par; "hole" 18 is the front nine, 19 the back nine, 20 the round
// (from the tee set of player pArgs[1]).
void GM_vScorecardGetPar(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 18) {
        pResult->i = GM_GetCurrentCourseFront9Par();
    } else if (pArgs[0].i == 19) {
        pResult->i = GM_GetCurrentCourseBack9Par();
    } else if (pArgs[0].i == 20) {
        pResult->i = GM_GetCurrentCourseTotalPar(gSession.nTeeSet[pArgs[1].i]);
    } else {
        pResult->i = GM_GetHoleIndexPar(pArgs[0].i);
    }
}

// Message 29: whether the round plays hole pArgs[0] (gpGame->bHoleSelected).
void GM_vHoleSelected(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gpGame->bHoleSelected[pArgs[0].i];
}

// Message 30: the holes player pArgs[0] has won (match play).
void GM_vGetMatchWins(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nHolesWon;
}

// Message 31: the skins player pArgs[0] has won (Player.nSkinsTotal, TW06 skinwins).
void GM_vGetSkinWins(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nSkinsTotal;
}

// Message 32: the game mode (Game_GetMode).
void GM_vGetGameModeType(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Game_GetMode();
}

// Message 33: player pArgs[0]'s speed-golf time score summed over all 18 holes
// (SpeedGolf_GetTotalTimeScore).
void GM_vGetSpeedGolfTotalTimeScore(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolf_GetTotalTimeScore(pArgs[0].i);
}

// Message 116: one of the special modes' values for player pArgs[0], picked by pArgs[1]: 1..4
// GameMode12's hole points, round points, shot multiplier and bonus meter; 9 and 13..19, 21, 26 the
// target games' (13 also sounds the ten-second warning and answers 0); 27
// GameModeSkillZoneBase_GetCupCount; the rest Player fields (nDD8, aDC4, nDC0, nDDC, nDE0, nE88,
// nE8C, nE94; 25 is aDC4[1] + 5). Any other pArgs[1] leaves the answer as it was.
void GM_vGetModeValue(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].nSkillZonePoints;
        return;
    case 1:
        pResult->i = GameMode12_GetHolePoints(pArgs[0].i);
        return;
    case 2:
        pResult->i = GameMode12_GetRoundPoints(pArgs[0].i);
        return;
    case 3:
        pResult->i = GameMode12_GetShotMultiplier(pArgs[0].i);
        return;
    case 4:
        pResult->i = GameMode12_GetBonusMeter(pArgs[0].i);
        return;
    case 5:
        pResult->i = gPlayers[pArgs[0].i].nSkillZonePoints;
        return;
    case 6:
        pResult->i = gPlayers[pArgs[0].i].aSkillZoneStats[4];
        return;
    case 7:
        pResult->i = gPlayers[pArgs[0].i].aSkillZoneStats[0];
        return;
    case 8:
        pResult->i = gPlayers[pArgs[0].i].nBalls;
        return;
    case 9:
        pResult->i = GameModeSkillZoneBase_CountGreensHit(pArgs[0].i);
        return;
    case 10:
        pResult->i = gPlayers[pArgs[0].i].nSkillZoneLongestDrive;
        return;
    case 11:
        pResult->i = gPlayers[pArgs[0].i].aSkillZoneStats[3];
        return;
    case 12:
        pResult->i = gPlayers[pArgs[0].i].nBullseyes;
        return;
    case 13:
        GameModeSkillZoneTimed_TenSecWarning();
        pResult->i = 0;
        return;
    case 14:
        pResult->i = GameModeSkillZoneCapture_GetTargettedRingOwner(pArgs[0].i);
        return;
    case 15:
        pResult->i = GameModeSkillZoneCapture_GetTargettedCapturedRing(pArgs[0].i);
        return;
    case 16:
        pResult->i = GameModeSkillZoneCapture_GetTotalTargetsHit(pArgs[0].i);
        return;
    case 17:
        pResult->i = GameModeSkillZoneCapture_GetRingOwnerFromIndex(pArgs[0].i);
        return;
    case 18:
        pResult->i = GameModeSkillZoneCapture_GetCapturedRingFromIndex(pArgs[0].i);
        return;
    case 19:
        pResult->i = GameModeSkillZoneCapture_GetMadeMoneyFromIndex(pArgs[0].i);
        return;
    case 20:
        pResult->i = gPlayers[pArgs[0].i].nHorseLetters;
        return;
    case 21:
        pResult->i = GameModeSkillZoneHorse_GetCurrentLeaderRing();
        return;
    case 22:
        pResult->i = gPlayers[pArgs[0].i].nBestHitStreak;
        return;
    case 23:
        pResult->i = gPlayers[pArgs[0].i].nSteals;
        return;
    case 24:
        pResult->i = gPlayers[pArgs[0].i].aSkillZoneStats[1];
        return;
    case 25:
        pResult->i = gPlayers[pArgs[0].i].aSkillZoneStats[1] + 5;
        return;
    case 26:
        pResult->i = GameModeSkillZoneHorse_GetLastShotExceeded();
        return;
    case 27:
        pResult->i = GameModeSkillZoneBase_GetCupCount();
        return;
    }
}

// Message 34: the lie of player pArgs[0]'s ball as a percentage: the surface's share of speed kept
// (f00) plus ball.f70 times RECOVERY / 100, times 100.
void GM_vGetLiePercentage(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 100.0f *
                 (0.01f * (gPlayers[pArgs[0].i].ball.f70 *
                           (s8)Golfer_GetAttribute(&gPlayers[pArgs[0].i], ATTR_RECOVERY, ATTR_TOTAL)) +
                  gSurfaceTypes[gPlayers[pArgs[0].i].ball.nSurface].f00);
}

// Message 35: hole pArgs[0]'s rating (GM_GetHoleIndexHandicap: the course table's nRating for that hole).
void GM_vGetHoleRating(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetHoleIndexHandicap(pArgs[0].i);
}

// Message 36: hole pArgs[0]'s length from tee set pArgs[1]; "hole" 18 is the front nine, 19 the
// back nine, 20 the round.
void GM_vGetTeeYardage(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 18) {
        pResult->i = GM_GetCurrentCourseFront9Yardage(pArgs[1].i);
    } else if (pArgs[0].i == 19) {
        pResult->i = GM_GetCurrentCourseBack9Yardage(pArgs[1].i);
    } else if (pArgs[0].i == 20) {
        pResult->i = GM_GetCurrentCourseTotalYardage(pArgs[1].i);
    } else {
        pResult->i = GM_GetHoleIndexTeeDistance(pArgs[0].i, pArgs[1].i);
    }
}

// Message 37: the tee set player pArgs[0] plays from (gSession.nTeeSet).
void GM_vGetPlayerTee(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gSession.nTeeSet[pArgs[0].i];
}

// Message 38: the value of the skin on this hole (GameModeSkins_CurrentHoleValue).
void GM_vGetCurrentSkinAmount(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeSkins_CurrentHoleValue();
}

// Message 39: how many skins are at stake on this hole (GameModeSkins_CurrentHoleNumberSkins).
void GM_vGetCurrentSkinNum(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeSkins_CurrentHoleNumberSkins();
}

// Message 40: how much the lie of player pArgs[0]'s ball can vary: the surface's range (f04) times
// (100 - RECOVERY).
void GM_vGetLieModifier(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = gSurfaceTypes[gPlayers[pArgs[0].i].ball.nSurface].f04 *
                 (100.0f - (s8)Golfer_GetAttribute(&gPlayers[pArgs[0].i], ATTR_RECOVERY, ATTR_TOTAL));
}

// Message 41: player pArgs[0]'s score, as the mode counts it: in Stableford (18)
// GM_GetPlayerRoundScore, in the PGA TOUR (23) the entrant's score to par, best ball (19) the
// team's, else his cumulative score to par.
void GM_vGetRelativeScore(MsgArg* pArgs, MsgArg* pResult) {
    switch (Game_GetMode()) {
    case 18:
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i);
        return;
    case 23:
        pResult->i = GM_PgaTourSim_GetRelativeScoreFromEntrantID(pArgs[0].i, 0, 0);
        return;
    case 19:
        pResult->i = GM_BestBallMode_GetTeamRelativeScore(pArgs[0].i, 1);
        return;
    default:
        pResult->i = GM_GetGolferRelativeCumulativeScore(pArgs[0].i, 1);
        return;
    }
}

// Message 42: player pArgs[0]'s score in round pArgs[1] of the event (the PGA TOUR, mode 23: from
// the simulation; else Player.nRoundScore).
void GM_vGetRoundScore(MsgArg* pArgs, MsgArg* pResult) {
    switch (Game_GetMode()) {
    case 23:
        pResult->i = GM_PgaTourSim_GetRoundScoreFromEntrantID(pArgs[0].i, 0, pArgs[1].i);
        return;
    }
    pResult->i = gPlayers[pArgs[0].i].nRoundScore[pArgs[1].i];
}

// Message 43: the PGA TOUR leaderboard: the name of the golfer on row pArgs[0], into the string
// pArgs[1].
void GM_vLeaderboard_LastName(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);
    s32 nGolfer = GM_PgaTourSim_GetGolferIDFromEntrantID(0, nEntrant);

    strcpy(((MsgString*)pArgs[1].p)->pStr, GM_PgaTourSim_GetNameFromGolferID(0, nGolfer));
}

// Message 44: the place of the golfer on leaderboard row pArgs[0].
void GM_vLeaderboard_Position(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_GetScoreRankFromEntrantID(0, nEntrant);
}

// Message 45: whether the golfer on leaderboard row pArgs[0] shares his place (GM_PgaTourSim_IsEntrantTied).
void GM_vLeaderboard_Tied(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_IsEntrantTied(0, nEntrant);
}

// Message 46: the score to par of the golfer on leaderboard row pArgs[0]
// (GM_PgaTourSim_GetRelativeScoreFromEntrantID, its flag set for every entrant but the user).
void GM_vLeaderboard_Score(MsgArg* pArgs, MsgArg* pResult) {
    int nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_GetRelativeScoreFromEntrantID(0, nEntrant, !GM_PgaTourSim_IsEntrantUser(0, nEntrant));
}

// Message 47: the hole the golfer on leaderboard row pArgs[0] is on.
void GM_vLeaderboard_Hole(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_GetCurrentHoleFromEntrantID(0, nEntrant);
}

// Message 48: the score of the golfer on leaderboard row pArgs[0] in round pArgs[1].
void GM_vLeaderboard_RoundScore(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_PgaTourSim_GetRoundScoreFromEntrantID(0, GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i), pArgs[1].i);
}

// Message 49: the player's place on the PGA TOUR leaderboard (entrant 0).
void GM_vLeaderboard_PlayerPosition(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_PgaTourSim_GetScoreRankFromEntrantID(0, 0);
}

// Message 50: whether the player (entrant 0) shares his place on the leaderboard
// (GM_PgaTourSim_IsEntrantTied).
void GM_vLeaderboard_PlayerTied(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_PgaTourSim_IsEntrantTied(0, 0);
}

// Message 51: how many entrants the PGA TOUR leaderboard has; 0 outside the PGA TOUR.
void GM_vLeaderboard_Entries(MsgArg* pArgs, MsgArg* pResult) {
    if (GM_Currently_PgaTourMode()) {
        pResult->i = GM_PgaTourSim_GetNumEntrants(0);
        return;
    }
    pResult->i = 0;
}

// Message 52: SpeedGolf_GetHoleScore for player pArgs[0] and hole pArgs[1]; pArgs[2] points to
// where whether he won points on it goes.
void GM_vGetSpeedGolfHoleScore(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolf_GetHoleScore(pArgs[0].i, pArgs[1].i, (s32*)pArgs[2].p);
}

// Message 53: the name of player pArgs[0]'s save profile into the string pArgs[1], or "User <n>"
// when none is loaded in his slot.
void GM_vGetGolfersUserName(MsgArg* pArgs, MsgArg* pResult) {
    int nSlot = gPlayers[pArgs[0].i].nIndex;
    char szName[32];

    if (gFEState.aLoaded[nSlot] == 0) {
        sprintf(szName, "User %d", nSlot + 1);
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[nSlot].szName);
}

// Message 54: whether player pArgs[0] is a CPU (1 or 0).
void GM_vIsGolferCPU(MsgArg* pArgs, MsgArg* pResult) {
    if (Player_IsCPU(pArgs[0].i)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Message 55: the caddie's putt read for player pArgs[0]'s view (BreakLine_GetCaddyTipInfo): how far past or
// short, and how far to the side, into the floats pArgs[1] and pArgs[2] point to.
void GM_vGetPuttHelp(MsgArg* pArgs, MsgArg* pResult) {
    // port: the studio passes the addresses of the two answers as 32-bit words
    BreakLine_GetCaddyTipInfo(gPlayers[pArgs[0].i].nView[0], (f32*)pArgs[1].i, (f32*)pArgs[2].i);
}

// A tournament entrant in a profile: GetEntrantMCPtr's body (PGATourSimulation.c), pasted in.
// Written out as gpSaveData[nPlayer].tour.field.aEntrant[n] it adds the entrant's offset last.
static inline PgaEntrantMC* Tour_EntrantMC(PlayerNumber_t nPlayer, int nEntrant) {
    return &gpSaveData[nPlayer].tour.field.aEntrant[nEntrant];
}

// Message 56: the winnings (PgaEntrantMC.nWinnings, profile 0) of the PGA TOUR entrant on
// leaderboard row pArgs[0].
void GM_vLeaderboard_Winnings(MsgArg* pArgs, MsgArg* pResult) {
    PlayerNumber_t nPlayer = PLR_1_e;

    pResult->i = Tour_EntrantMC(nPlayer, GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i))->nWinnings;
}

// Message 57: the winnings of the player (entrant 0) in save profile pArgs[0].
void GM_vLeaderboard_PlayerWinnings(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    pResult->i = pProfile->tour.field.aEntrant[0].nWinnings;
}

// Message 58: in lesson mode, the lesson stops waiting and goes on (Lessons_StopWaiting).
void GM_vLessonStopWaiting(MsgArg* pArgs, MsgArg* pResult) {
    Lessons_StopWaiting();
}

// Message 59: call the mode's pfnSetTimer with player pArgs[0] and time pArgs[1] (modes 6, 7, 8 and
// 13 store it for the hole).
void GM_vReturnTimer(MsgArg* pArgs, MsgArg* pResult) {
    gpGame->pfnSetTimer(pArgs[0].i, pArgs[1].i);
}

// Message 60: whether gSession.bDemo is set (1 or 0); EA's name says the quick scorecard, which in
// this build is the demo flag.
void GM_vQuickScorecard(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.bDemo != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Message 61: controller pArgs[0] is present again: the pulled-controller pause is lifted
// (GUI_OnControllerPresent) and, in lesson mode, the lesson restarts.
void GM_vControllerReconnected(MsgArg* pArgs, MsgArg* pResult) {
    GUI_OnControllerPresent(pArgs[0].i);
    Lessons_RestartLesson();
}

// Message 62: the name of save profile slot pArgs[0] into the string pArgs[1], or "User <n>" when
// none is loaded there.
void GM_vGetSlotUserName(MsgArg* pArgs, MsgArg* pResult) {
    int nSlot = pArgs[0].i;
    char szName[32];

    if (gFEState.aLoaded[nSlot] == 0) {
        sprintf(szName, "User %d", nSlot + 1);
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[nSlot].szName);
}

// Message 63: how many holes the round plays (GM_GetNumHolesInRound).
void GM_vGetNumHolesSelected(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetNumHolesInRound();
}

// Message 64: whether gSession.bDemo is set (1 or 0): the demo is running.
void GM_vGetDemoMode(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.bDemo != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Message 65: whether a PlayNow challenge (GameMode5) is being played (1 or 0).
void GM_vIsPlayNowChallenge(MsgArg* pArgs, MsgArg* pResult) {
    if (PlayNow_IsChallengeRunning()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Message 66: whether the game mode is speed golf (PlayNow_IsSpeedGolf).
void GM_vIsPlayNowSpeedGolf(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_IsSpeedGolf();
}

// Message 67: the mark for medal pArgs[0] of the PlayNow challenge (PlayNow_GetMedalMark).
void GM_vGetPlayNowMedalMark(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetMedalMark(pArgs[0].i);
}

// Message 68: the PlayNow challenge score against its target (PlayNow_GetScoreToTarget).
void GM_vGetPlayNowScoreToTarget(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetScoreToTarget();
}

// Queues the statistic tip GameAnalysis_PickTip picks for the HUD (command 69); nothing when it
// picks none (14).
void GM_vShowAnalysisTip(MsgArg* pArgs, MsgArg* pResult) {
    int nTip = GameAnalysis_PickTip();

    if (nTip != 14) {
        GUI_QueueTip(nTip);
    }
}

// Player pArgs[0]'s statistic pArgs[1] for the tips, as a float (command 70):
// GameAnalysis_GetTipStat's fairways, greens in regulation, putts, pars, birdies and the like.
void GM_vGetTipStat(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = GameAnalysis_GetTipStat(pArgs[0].i, pArgs[1].i);
}

// One game option for the in-round menu, picked by pArgs[0] (command 71): 0 vibration
// (options.a7[0], which fn_8002EBA4 sets), 1 gimmes, 2 the effects level, 3 the commentary level in
// the menu's count (levels 0..4 answer 2..6, level 5 answers 1), 4 options.a7[1], 5 a24[6], 6
// a24[4], 7 the music level, 8 a24[1]. The on/off options answer 0 or 1; another pArgs[0] leaves
// *pResult alone.
void GM_vGetOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        if (gSession.options.a7[0] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 1:
        if (gSession.options.bGimmes != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 2:
        pResult->i = (s8)gSession.options.a0[0];
        return;
    case 3:
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
        break;
    case 4:
        if (gSession.options.a7[1] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 5:
        if (gSession.options.a24[6] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 6:
        if (gSession.options.a24[4] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 7:
        pResult->i = (s8)gSession.options.a0[1];
        return;
    case 8:
        if (gSession.options.a24[1] != 0) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    }
}

// Sets one game option from the in-round menu: pArgs[0] picks it, pArgs[1] is the value (command
// 72). 0 vibration (fn_8002EBA4: on for all four controllers or off), 1 gimmes, 2 the effects level
// (and the mixer, 0.2 x level), 3 the commentary level from the menu's 1..6 (1 is level 5, 2..6 are
// levels 0..4; the mixer is set even for another value), 4 options.a7[1], 5 the music level (stored
// only: the music volume is not set here). Past 4 the numbering is not GM_vGetOption's.
void GM_vSetOption(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        if (pArgs[1].i != 0) {
            fn_8002EBA4((u8*)&gSession.options, 1);
            return;
        }
        fn_8002EBA4((u8*)&gSession.options, 0);
        return;
    case 1:
        if (pArgs[1].i != 0) {
            gSession.options.bGimmes = 1;
            return;
        }
        gSession.options.bGimmes = 0;
        return;
    case 2:
        gSession.options.a0[0] = pArgs[1].i;
        Gaud_SetSfxLevel(0.2f * (s8)gSession.options.a0[0]);
        return;
    case 3:
        switch (pArgs[1].i) {
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
        return;
    case 4:
        if (pArgs[1].i != 0) {
            gSession.options.a7[1] = 1;
            return;
        }
        gSession.options.a7[1] = 0;
        return;
    case 5:
        gSession.options.a0[1] = pArgs[1].i;
        return;
    }
}

// The player whose turn it is concedes the hole (command 73); the replay flag (gSession.bReplay) is
// cleared first.
void GM_vConcede_Hole(MsgArg* pArgs, MsgArg* pResult) {
    gSession.bReplay = 0;
    GM_GolferConcede_Hole(lbl_80282278);
}

// The UI reports the caddie tip window (command 74): pArgs[0] 1 closed (GUI_CaddieTipWindowClosed:
// the HUD comes back on the next check); anything else open: the player up's HUD and target info
// are hidden (fn_80062C80, message 0x1E with 0) and the swing is held (GUI_CaddieTipWindowIsOpen).
void GM_vReportCaddieTipWindow(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 1) {
        GUI_CaddieTipWindowClosed();
        return;
    }
    GUI_ToggleUI(lbl_80282278, 0);
    fn_80062C80(gPlayers[lbl_80282278].nUISlot, 0);
    GUI_CaddieTipWindowIsOpen();
}

// Turns the full caddie tips off for the player whose turn it is (command 75): sets his save
// profile's bCaddieTipsOff, when the profile is in use, and CTIP_ShowCaddieTip then shows only
// short tips.
void GM_vDisableCaddieTips(MsgArg* pArgs, MsgArg* pResult) {
    if (gpSaveData[gPlayers[lbl_80282278].nIndex].bActive != 0) {
        gpSaveData[gPlayers[lbl_80282278].nIndex].bCaddieTipsOff = 1;
    }
}

// The space a replay save still needs on the card in port pArgs[0], slot pArgs[1] (command 76): the
// card is looked at, the replay file type's card operations are picked (MC_SetCurrentFileType(2); TW07's
// MC_SetCurrentFileType) and their memory-required one asked (MC_CallActionFnMemoryRequired; TW07's
// MC_CallActionFnMemoryRequired).
void GM_vIG_MCMemforReplay(MsgArg* pArgs, MsgArg* pResult) {
    CardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_ConnectCard(pos.nPort, pos.nSlot);
    MC_SetCurrentFileType(2);
    pResult->i = MC_CallActionFnMemoryRequired(&pos);
    MC_Disconnect();
}

void GM_vIG_MCConnect(MsgArg* pArgs, MsgArg* pResult) {
    MC_Connect();
}

void GM_vIG_MCDisconnect(MsgArg* pArgs, MsgArg* pResult) {
    MC_Disconnect();
}

// Whether a multitap is plugged into port pArgs[0] (command 79).
void GM_vIG_Multitap(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_IsMultitapPluggedIn(pArgs[0].i);
}

// Whether a card is in port pArgs[0], slot pArgs[1], by its noted state (MC_GetMC; command 80).
void GM_vIG_MCExists(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = (card.uFlags & MC_CARD_PRESENT) != 0;
}

// Whether the card in port pArgs[0], slot pArgs[1] is formatted (flag 0x08, MC_CARD_FORMATTED;
// command 81).
void GM_vIG_MCFormatted(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = (card.uFlags & MC_CARD_FORMATTED) != 0;
}

// The free space on the card in port pArgs[0], slot pArgs[1], in whole sectors (command 82).
void GM_vIG_MCfreeMem(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = card.nFreeBlocks;
}

// How many replays the save on the card in port pArgs[0], slot pArgs[1] holds, 0 on an error
// (command 83): the menus' own command GM_vMCGetNumReplays, run with the same arguments.
void GM_vIG_MCGetNumReplays(MsgArg* pArgs, MsgArg* pResult) {
    GM_vMCGetNumReplays(pArgs, pResult);
}

// Asks for pArgs[0] to be handed back to the UI a little later (command 84): uiProcessInterface.c
// counts three UI updates (gUIDelayedHint.nFrames), then sends it to the UI as hint 0x24 in a round
// (0x23 in the menus). The menus' GM_vMCfunction does the same.
void GM_vIG_MCfunction(MsgArg* pArgs, MsgArg* pResult) {
    gUIDelayedHint.nValue = pArgs[0].i;
    gUIDelayedHint.nFrames = 0;
}

// Formats the card in port pArgs[0], slot pArgs[1] (command 85); MC_FormatCard's result.
void GM_vIG_MCFormat(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = MC_FormatCard(pArgs[0].i, pArgs[1].i);
}

// Saves the replay in memory (gReplayData) to the card in port pArgs[0], slot pArgs[1] as replay
// pArgs[2] (-1: the first free one) (command 86); MC_SaveReplay's result. The spin input of the
// player up is switched off first (swing.bCanSpin).
void GM_vIG_SaveReplay(MsgArg* pArgs, MsgArg* pResult) {
    MCCardPos pos;
    s32 nPlayer;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    pos.n8 = pArgs[2].i;
    nPlayer = lbl_80282278;
    if (nPlayer < 5) {
        gPlayers[nPlayer].swing.bCanSpin = 0;
    }
    pResult->i = MC_SaveReplay(&pos);
}

// Saves the replay in memory over replay pArgs[2] of the save on the card in port pArgs[0], slot
// pArgs[1] (command 87; fn_800A0610's result). The spin input of the player up is switched off
// first (swing.bCanSpin).
void GM_vIG_ReplaceReplay(MsgArg* pArgs, MsgArg* pResult) {
    if (lbl_80282278 < 5) {
        gPlayers[lbl_80282278].swing.bCanSpin = 0;
    }
    pResult->i = fn_800A0610(pArgs[0].i, pArgs[1].i, pArgs[2].i);
}

// Whether the replay in memory (gReplayData, with bF10 set) is of the current hole of the current
// course (command 88).
void GM_vCurrentReplayValid(MsgArg* pArgs, MsgArg* pResult) {
    if (gReplayData.bF10 != 0 && gReplayData.nHole == Game_GetCurHoleNum() &&
        gReplayData.nCourse == Game_GetCourse()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// How many of the round's selected holes are left, the current one included (command 89;
// GM_GetNumHolesRemainingInRound).
void GM_vGetNumHolesRemaining(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_GetNumHolesRemainingInRound();
}

// Whether the player is in the zoom-to-aim camera.
void GM_vGetPlayerUIZoomedIn(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)GOLFERSTATE_GetCurrentState(pArgs[0].i) == GS_ZOOM) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Practice mode: the player ends the hole early and goes on (command 91;
// GameModePractice_FinishHole).
void GM_vPracticeNextHole(MsgArg* pArgs, MsgArg* pResult) {
    GameModePractice_FinishHole();
}

// Empty in this build: the UI's command 92, between GM_vPracticeNextHole and
// GM_vGetPlayerShotSetup.
void GM_vIGMessage92_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether the player is lining up a shot (states 2 to 4, 8 or 10) and has not started the swing.
void GM_vGetPlayerShotSetup(MsgArg* pArgs, MsgArg* pResult) {
    int nState = (s8)GOLFERSTATE_GetCurrentState(pArgs[0].i);

    // fake match: states 2 to 4 tested as one unsigned compare
    if (((u32)(nState - GS_SHOT_SETUP) <= GS_ELEVATOR - GS_SHOT_SETUP || nState == GS_KNEE_CAM ||
         nState == GS_SWING) &&
        gPlayers[pArgs[0].i].swing.nState == SW_IDLE_SWING) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Whether player pArgs[0]'s swing is under way: back swing, back fidget or down swing (swing.nState
// SW_BACK_SWING .. SW_DOWN_SWING) (command 165).
void GM_vGetPlayerBackSwing(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].swing.nState == 2 || gPlayers[pArgs[0].i].swing.nState == 3 ||
        gPlayers[pArgs[0].i].swing.nState == 1) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The lessons: the player chose to stop after lesson 7 (command 94; Lessons_ChooseQuit).
void GM_vLessonsQuit(MsgArg* pArgs, MsgArg* pResult) {
    Lessons_ChooseQuit();
}

// The lessons: the player chose to go on to lesson 8 (command 95; Lessons_ChooseContinue).
void GM_vLessonsContinue(MsgArg* pArgs, MsgArg* pResult) {
    Lessons_ChooseContinue();
}

// Who controls the player (CONTROLLER_CPU for the AI).
void GM_vGetGolferController(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].nController;
}

// The HUD clock ran out (command 97): a target mode's timer (GameModeSkillZoneBase_TimerOut), else
// speed golf's (SpeedGolf_TimerOut).
void GM_vTimerOut(MsgArg* pArgs, MsgArg* pResult) {
    if (GM_Currently_SkillZoneMode()) {
        GameModeSkillZoneBase_TimerOut();
        return;
    }
    SpeedGolf_TimerOut();
}

// Whether the player is in the elevator camera.
void GM_vGetPlayerUIElevator(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)GOLFERSTATE_GetCurrentState(pArgs[0].i) == GS_ELEVATOR) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Speed golf's winner (-1 none), with the prize money into *pArgs[0] (command 99).
// SpeedGolf_GetWinner pays the prize each time it is asked.
void GM_vGetSpeedGolfWinner(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolf_GetWinner((s32*)pArgs[0].p);
}

// Whether the round plays every hole.
void GM_vFullRound(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_FullRoundOfGolf();
}

// Speed golf's three prize limits for the course into *pArgs[0], *pArgs[1] and *pArgs[2]: a round
// under them wins 1000, 2500 and 5000 (command 101; SpeedGolf_GetPrizeScores).
void GM_vGetSpeedGolfPrizeScores(MsgArg* pArgs, MsgArg* pResult) {
    SpeedGolf_GetPrizeScores((s32*)pArgs[0].p, (s32*)pArgs[1].p, (s32*)pArgs[2].p);
}

// The holes left in the Play Now challenge group being played (command 102; PlayNow_GetHolesLeft).
void GM_vGetChallengeHolesLeft(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetHolesLeft();
}

// Empty in this build: the UI's command 103.
void GM_vIGMessage103_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Empty in this build: the UI's command 104.
void GM_vIGMessage104_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// The event being played, its name into pArgs[0] and its description into pArgs[1] (command 105):
// the real-time event's while one runs, else the Play Now challenge group's.
void GM_vGetEventNameAndText(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRound;

    if (GM_Currently_RealtimeMode()) {
        strcpy(((MsgString*)pArgs[0].p)->pStr,
               GameModeDriverRTE_GetName(GM_RealtimeMode_GetSelectedEvent(&nRound)));
        strcpy(((MsgString*)pArgs[1].p)->pStr,
               GameModeDriverRTE_GetDescription(GM_RealtimeMode_GetSelectedEvent(&nRound)));
        return;
    }
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupName(PlayNow_GetCurrentGroup()));
    strcpy(((MsgString*)pArgs[1].p)->pStr, PlayNow_GetGroupDescription(PlayNow_GetCurrentGroup()));
}

// Plays UI sound pArgs[1] (command 106; Gaud_PlayGameUISound, which does not use pArgs[0]).
void GM_vPlayUISound(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_PlayGameUISound(pArgs[0].i, pArgs[1].i);
}

// Two-player speed golf: the players' names into pArgs[0] and pArgs[2], their points into *pArgs[1]
// and *pArgs[3]; answers who gained points on the current hole, 0, 1 or -1 for neither (command
// 107; SpeedGolfPoints_GetNamesAndPoints).
void GM_vGetSpeedGolfPoints(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolfPoints_GetNamesAndPoints(((MsgString*)pArgs[0].p)->pStr, (s32*)pArgs[1].p,
                             ((MsgString*)pArgs[2].p)->pStr, (s32*)pArgs[3].p);
}

// The round's next hole after the current one: its number (-1: none), and into pArgs its par and
// its length from player 0's tees.
void GM_vGM_NextHoleDetails(MsgArg* pArgs, MsgArg* pResult) {
    int nHole;
    s32 nPar = 0;
    s32 nLength = 0;

    nHole = GM_GetNextSelectedHole();

    if (nHole != -1) {
        nPar = GM_GetHoleIndexPar(nHole);
        nLength = GM_GetHoleIndexTeeDistance(nHole, gSession.nTeeSet[0]);
    }
    pResult->i = nHole;
    *(s32*)pArgs[0].p = nPar;
    *(s32*)pArgs[1].p = nLength;
}

// Speed golf's round score so far; the current hole's seconds, strokes and score into *pArgs[1],
// *pArgs[2] and *pArgs[3] (command 109; SpeedGolf_GetRoundScore). pArgs[0]'s string goes in as its
// first argument, which it does not read: the score is player 0's.
void GM_vGetSpeedGolfRoundScore(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolf_GetRoundScore(((MsgString*)pArgs[0].p)->pStr, (s32*)pArgs[1].p, (s32*)pArgs[2].p,
                             (s32*)pArgs[3].p);
}

// Whether a player with controller pArgs[0] may use the menu now (command 110; IG_IsControllerInPlay).
void GM_vCanControllerUseMenu(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = IG_IsControllerInPlay(pArgs[0].i);
}

// Looks at the card in port pArgs[0], slot pArgs[1] once, so its noted state is fresh
// (MC_ConnectCard), and ends the card work (command 111).
void GM_vIG_MCLookAtCard(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    MC_Disconnect();
}

// The same as GM_vIG_MCMemforReplay (command 76), under command 112: the space a replay save still
// needs on the card in port pArgs[0], slot pArgs[1].
void GM_vIG_ReplaySpaceNeeded(MsgArg* pArgs, MsgArg* pResult) {
    CardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_ConnectCard(pos.nPort, pos.nSlot);
    MC_SetCurrentFileType(2);
    pResult->i = MC_CallActionFnMemoryRequired(&pos);
    MC_Disconnect();
}

// The pause menu closed (command 113): the game's sound resumes (Gaud_Pause(0)),
// GUI_PauseMenuClosed runs, and, unless the fade to black is running, the lesson starts over
// (Lessons_RestartLesson: in the lessons only).
void GM_vPauseMenuClosed(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_Pause(0);
    GUI_PauseMenuClosed();
    if (gUIState.bFadeToBlack == 0) {
        Lessons_RestartLesson();
    }
}

// A quick cheer from the crowd (command 114; Gaud_FireQuickCheer).
void GM_vQuickCheer(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_FireQuickCheer();
}

// The UI's command 115: always answers 0 in this build.
void GM_vIGMessage115_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// The target modes' HUD numbers for player pArgs[0], picked by pArgs[2] (command 117): 0 make mode
// 12's list of the surfaces scored on this shot, 1 its length, 2 entry pArgs[1] of it, 3 that
// entry's hits; 4 the points, 5 the seconds and 6 the balls the last shot earned, 7 the bonus
// multiplier. Another pArgs[2] leaves *pResult alone.
void GM_vGetTargetShotInfo(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[2].i) {
    case 0:
        GameMode12_ListScoredSurfaces(pArgs[0].i);
        return;
    case 1:
        pResult->i = GameMode12_GetNumScoredSurfaces(pArgs[0].i);
        return;
    case 2:
        pResult->i = GameMode12_GetScoredSurface(pArgs[0].i, pArgs[1].i);
        return;
    case 3:
        pResult->i = GameMode12_GetScoredSurfaceHits(pArgs[0].i, pArgs[1].i);
        return;
    case 4:
        pResult->i = GameModeSkillZoneBase_GetShotEarned(pArgs[0].i);
        return;
    case 5:
        pResult->i = GameModeSkillZoneBase_GetTimeEarned(pArgs[0].i);
        return;
    case 6:
        pResult->i = GameModeSkillZoneBase_GetExtraBallsEarned(pArgs[0].i);
        return;
    case 7:
        pResult->i = GameModeSkillZoneBase_GetDriveMultiplier(pArgs[0].i);
        return;
    }
}

// A player's number for the target modes, picked by pArgs[1] (command 118): 0 his scoring shots on
// the current hole (Player.nHoleHits, counted by modes 12, 13, 14 and 17). Another pArgs[1] leaves
// *pResult alone.
void GM_vGetTargetHoleStat(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].nHoleHits[Game_CurHoleIndex()];
        return;
    }
}

// Whether the session runs the demo set-up (gSession.uFlags 0x4000) (command 171).
void GM_vIsDemoSetup(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.uFlags & 0x4000) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Whether the round (game type 6) is paused for a pulled controller (gSession.nPaused 2, or 3,
// which nothing in this build sets) (command 120).
void GM_vIsPausedForController(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.nGameType == 6 && (gSession.nPaused == 2 || gSession.nPaused == 3)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The shot clock ran out (command 121): in a target mode, GameModeSkillZoneBase_ShotClockOut; else
// nothing.
void GM_vShotClockOut(MsgArg* pArgs, MsgArg* pResult) {
    if (GM_Currently_SkillZoneMode()) {
        GameModeSkillZoneBase_ShotClockOut();
    }
}

// Empty in this build: the UI's command 122.
void GM_vIGMessage122_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// A string's length.
void GM_vGetStringLength(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

// A record holder's name into pArgs[3]; pArgs[2] is the place (0..4) (command 124). pArgs[0] 3: the
// hole contest's result table; 0: the all-time records of kind pArgs[1] (recA); else by mode: 16,
// 17 and 13 the current hole's records (recB kinds 0, 1, 2), 22 the long-drive hole's (recC, by the
// mode's variant 0 or 1), any other the course's records of kind pArgs[1].
void GM_vGetRecordName(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 3) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", HoleContest_GetPlaceName(pArgs[2].i));
        return;
    }
    if (pArgs[0].i == 0) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recA[pArgs[1].i][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 16) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recB[Game_GetCurHoleNum()][0][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 17) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recB[Game_GetCurHoleNum()][1][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 13) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recB[Game_GetCurHoleNum()][2][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 22 && GameMode22_GetVariant() == 0) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
                gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][0][pArgs[2].i].szName);
        return;
    }
    if (Game_GetMode() == 22 && GameMode22_GetVariant() == 1) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
                gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][1][pArgs[2].i].szName);
        return;
    }
    sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
            gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].szName);
}

// The value of the record GM_vGetRecordName names, with the same arguments (command 125); for the
// hole contest (pArgs[0] 3) the place's drive length or distance from the pin in feet, -1 for none.
void GM_vGetRecordScore(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 3) {
        pResult->i = HoleContest_GetPlaceDistance(pArgs[2].i);
        return;
    }
    if (pArgs[0].i == 0) {
        pResult->i = gSession.recA[pArgs[1].i][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 16) {
        pResult->i = gSession.recB[Game_GetCurHoleNum()][0][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 17) {
        pResult->i = gSession.recB[Game_GetCurHoleNum()][1][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 13) {
        pResult->i = gSession.recB[Game_GetCurHoleNum()][2][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 22 && GameMode22_GetVariant() == 0) {
        pResult->i = gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][0][pArgs[2].i].nValue;
        return;
    }
    if (Game_GetMode() == 22 && GameMode22_GetVariant() == 1) {
        pResult->i = gSession.recC[GameMode22_GetHoleRecordIndex(Game_GetCurHoleNum())][1][pArgs[2].i].nValue;
        return;
    }
    pResult->i = gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].nValue;
}

// pArgs[0] printed into pArgs[1] with thousands commas, "12,345" (command 126; UI_GetMoneyString).
void GM_vFormatWithCommas(MsgArg* pArgs, MsgArg* pResult) {
    UI_GetMoneyString(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// The end-of-round money screen's numbers for player pArgs[0], picked by pArgs[1] (command 127): 0
// the points, 1 the payout, 2..5 what the course, pin set, tee and TOUR card multipliers added, 6
// money.n38, 7 money.n3C, 8..15 money.n0 to n1C (8 the payout, 9 a tournament's prize, 10 bonuses,
// 11 a ladder prize, 12 to 15 money.n10 to n1C, the match, skins and speed golf money); 16 his
// profile's money less this payout; 100 the course multiplier less 1; 101 the hole's pin set; 102
// the tee multiplier's row (EARN_MULT_TEE's 2 - tee set; tee set 3 as 1); 103 the TOUR card
// percentage for his card level; 104 that level, 1..6 (0 counts as 1); 200 2 when any of money.n4
// to n1C is set, else 1. 16, 103 and 104 answer 0 without a profile in use.
void GM_vGetWrapupData(MsgArg* pArgs, MsgArg* pResult) {
    int nMult;

    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].money.nBase;
        return;
    case 2:
        pResult->i = gPlayers[pArgs[0].i].money.nCourse;
        return;
    case 4:
        pResult->i = gPlayers[pArgs[0].i].money.nTee;
        return;
    case 3:
        pResult->i = gPlayers[pArgs[0].i].money.n2C;
        return;
    case 6:
        pResult->i = gPlayers[pArgs[0].i].money.n38;
        return;
    case 5:
        pResult->i = gPlayers[pArgs[0].i].money.nTourCard;
        return;
    case 1:
        pResult->i = gPlayers[pArgs[0].i].money.n24;
        return;
    case 7:
        pResult->i = gPlayers[pArgs[0].i].money.n3C;
        return;
    case 8:
        pResult->i = gPlayers[pArgs[0].i].money.n0;
        return;
    case 9:
        pResult->i = gPlayers[pArgs[0].i].money.n4;
        return;
    case 10:
        pResult->i = gPlayers[pArgs[0].i].money.n8;
        return;
    case 11:
        pResult->i = gPlayers[pArgs[0].i].money.nC;
        return;
    case 12:
        pResult->i = gPlayers[pArgs[0].i].money.n10;
        return;
    case 13:
        pResult->i = gPlayers[pArgs[0].i].money.n14;
        return;
    case 14:
        pResult->i = gPlayers[pArgs[0].i].money.n18;
        return;
    case 15:
        pResult->i = gPlayers[pArgs[0].i].money.n1C;
        return;
    case 16:
        if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
            pResult->i = 0;
            return;
        }
        // fake match: the (int) keeps CW from reusing the first gPlayers index, as the original does
        pResult->i = gpSaveData[gPlayers[pArgs[0].i].nIndex].nCurrentCash
                - gPlayers[(int)pArgs[0].i].money.n24;
        return;
    case 100:
        pResult->i = (s32)GM_Earnings_GetCourseModifier() - 1;
        return;
    case 102:
        switch (gSession.nTeeSet[pArgs[0].i]) {
        case 0:
            pResult->i = 2;
            return;
        case 1:
            pResult->i = 1;
            return;
        case 2:
            pResult->i = 0;
            return;
        case 3:
            pResult->i = 1;
            return;
        }
        break;
    case 101:
        switch (gpGame->nPinSet[Game_CurHoleIndex()]) {
        case 0:
            pResult->i = 0;
            return;
        case 1:
            pResult->i = 1;
            return;
        case 2:
            pResult->i = 2;
            return;
        case 3:
            pResult->i = 3;
            return;
        }
        break;
    case 103:
        if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
            pResult->i = 0;
            return;
        }
        // EA bug: a level outside 0..6 leaves nMult unset (here and in case 104). nMult is the
        // index into the whole multiplier table (23..28: the TOUR card group).
        switch (gpSaveData[gPlayers[pArgs[0].i].nIndex].nTourCardLevel) {
        case 0:
        case 1:
            nMult = EARN_MULT_TOUR;
            break;
        case 2:
            nMult = EARN_MULT_TOUR + 1;
            break;
        case 3:
            nMult = EARN_MULT_TOUR + 2;
            break;
        case 4:
            nMult = EARN_MULT_TOUR + 3;
            break;
        case 5:
            nMult = EARN_MULT_TOUR + 4;
            break;
        case 6:
            nMult = EARN_MULT_TOUR + 5;
            break;
        }
        GameMode4_IsEventRunning();
        pResult->i = gEarningsTable.aMult[nMult];
        return;
    case 104:
        if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
            pResult->i = 0;
            return;
        }
        switch (gpSaveData[gPlayers[pArgs[0].i].nIndex].nTourCardLevel) {
        case 0:
        case 1:
            nMult = 1;
            break;
        case 2:
            nMult = 2;
            break;
        case 3:
            nMult = 3;
            break;
        case 4:
            nMult = 4;
            break;
        case 5:
            nMult = 5;
            break;
        case 6:
            nMult = 6;
            break;
        }
        GameMode4_IsEventRunning();
        pResult->i = nMult;
        return;
    case 200:
        if (gPlayers[pArgs[0].i].money.n4 != 0 || gPlayers[pArgs[0].i].money.n8 != 0 ||
            gPlayers[pArgs[0].i].money.nC != 0 || gPlayers[pArgs[0].i].money.n10 != 0 ||
            gPlayers[pArgs[0].i].money.n14 != 0 || gPlayers[pArgs[0].i].money.n18 != 0 ||
            gPlayers[pArgs[0].i].money.n1C != 0) {
            pResult->i = 2;
            return;
        }
        pResult->i = 1;
        break;
    }
}

// A record holder's name into pArgs[3], and its length into pArgs[3]'s nLen (command 128): pArgs[0]
// 0 the all-time records (recA), else the course's; pArgs[1] is the kind, pArgs[2] the place.
void GM_vGetRecordHolderName(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s", gSession.recA[pArgs[1].i][pArgs[2].i].szName);
    } else {
        // EA bug: passes the whole record to "%s", not its name
        sprintf(((MsgString*)pArgs[3].p)->pStr, "%s",
                gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i]);
    }
    ((MsgString*)pArgs[3].p)->nLen = strlen(((MsgString*)pArgs[3].p)->pStr);
}

// A record's value: pArgs[0] 0 is the all-time records, else the course's.
void GM_vGetRecordValue(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        pResult->i = gSession.recA[pArgs[1].i][pArgs[2].i].nValue;
        return;
    }
    pResult->i = gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].nValue;
}

// The distance from player pArgs[0]'s ball to the pin, a float (command 130;
// GameAnalysis_GetCurrentDistanceToPin).
void GM_vGetYardsToPin(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = GameAnalysis_GetCurrentDistanceToPin(pArgs[0].i);
}

// Entry pArgs[0] of speed golf's event log: its player into *pArgs[1] and its event into *pArgs[2];
// outside the log (0 to gSpeedGolfEventLogCount - 1) 0 and -1 (command 132). Each call also steps
// lbl_80281EDC through -1..10; the value it writes into *pArgs[2] first is always overwritten.
void GM_vGetSpeedGolfLogEntry(MsgArg* pArgs, MsgArg* pResult) {
    gSpeedGolfLogCycle++;
    if (gSpeedGolfLogCycle > 10) {
        gSpeedGolfLogCycle = -1;
    }
    *(s32*)pArgs[1].p = 0;
    *(s32*)pArgs[2].p = gSpeedGolfLogCycle;
    if (pArgs[0].i < 0 || pArgs[0].i >= gSpeedGolfEventLogCount) {
        *(s32*)pArgs[1].p = 0;
        *(s32*)pArgs[2].p = -1;
        return;
    }
    *(s32*)pArgs[1].p = gSpeedGolfEventLog[pArgs[0].i].nPlayer;
    *(s32*)pArgs[2].p = gSpeedGolfEventLog[pArgs[0].i].nEvent;
}

// The UI's command 133: always answers 7 in this build.
void GM_vIGMessage133_Return7(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

// Whether a hole contest was won, picked by pArgs[0] (command 134): 0 this round
// (HoleContest_IsWonThisRound), 1 at any time since the game started (HoleContest_WasEverWon).
// Another pArgs[0] leaves *pResult alone.
void GM_vGetHoleContestWon(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 0:
        if (HoleContest_IsWonThisRound()) {
            pResult->i = 1;
            return;
        }
        pResult->i = 0;
        return;
    case 1:
        pResult->i = HoleContest_WasEverWon();
        return;
    }
}

// Whether a ladder event is being played (command 135; GameMode4_IsEventRunning).
void GM_vIsLadderEvent(MsgArg* pArgs, MsgArg* pResult) {
    if (GameMode4_IsEventRunning()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// A random number from stream 1 of Misc_RandFunc (command 136).
void GM_vGetRandInt(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Misc_RandFunc(1);
}

// The UI's command 137: always answers 0 in this build.
void GM_vIGMessage137_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Empty in this build: the UI's command 138.
void GM_vIGMessage138_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// The UI's command 139: always writes 2 into *pArgs[0] and 1 into *pArgs[1] in this build.
void GM_vIGMessage139_Return2And1(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

// The UI's command 140: always answers 2 in this build.
void GM_vIGMessage140_Return2(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

// The UI's command 141: writes pArgs[0] into *pArgs[1] and 0 into *pArgs[2] (the menus'
// GM_vFEMessage288_ReturnArg does the same).
void GM_vIGMessage141_ReturnArg(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

// The letter for number pArgs[0], 0 is "A", into pArgs[2] (command 142; the menus' GM_vFEGetLetter, run
// with the same arguments).
void GM_vGetLetter(MsgArg* pArgs, MsgArg* pResult) {
    GM_vFEGetLetter(pArgs, pResult);
}

// The card in port pArgs[0], slot pArgs[1]: into pArgs[2..6] whether its sectors are not 8 KB, and
// its encoding, wrong-device, I/O-error and broken flags.
void GM_vIG_MCGetCardErrors(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    if (card.nSectorSize != 0x2000) {
        *(s32*)pArgs[2].p = 1;
    } else {
        *(s32*)pArgs[2].p = 0;
    }
    if (card.uFlags & MC_CARD_ENCODING) {
        *(s32*)pArgs[3].p = 1;
    } else {
        *(s32*)pArgs[3].p = 0;
    }
    if (card.uFlags & MC_CARD_WRONGDEVICE) {
        *(s32*)pArgs[4].p = 1;
    } else {
        *(s32*)pArgs[4].p = 0;
    }
    if (card.uFlags & MC_CARD_IOERROR) {
        *(s32*)pArgs[5].p = 1;
    } else {
        *(s32*)pArgs[5].p = 0;
    }
    if (card.uFlags & MC_CARD_BROKEN) {
        *(s32*)pArgs[6].p = 1;
        return;
    }
    *(s32*)pArgs[6].p = 0;
}

// The name of the round's course into pArgs[1] (command 144): a custom round's saved name, "Random
// 18", "Dream 18" or a regional round's region, else the course's name, with "Sherwood CC" for
// course 7 (the skills course) and for hole indexes 15 and up. With the demo set-up
// (gSession.uFlags 0x4000) a custom round shows the course's name instead.
void GM_vGetCourseHoleName(MsgArg* pArgs, MsgArg* pResult) {
    if ((gSession.uFlags & 0x4000) && gpGame->bCustomRound != 0) {
        if (gpGame->nCurCourse == 7 || Game_CurHoleIndex() >= 15) {
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Sherwood CC");
            return;
        }
        strcpy(((MsgString*)pArgs[1].p)->pStr, gCourseNames[gpGame->nCurCourse]);
        return;
    }
    if (gpGame->bCustomRound != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr,
               gpSaveData[gpGame->nSaveSlot].aSavedRound[gpGame->nSaveCourse].szName);
        return;
    }
    if (gpGame->bRandom18 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Random 18");
        return;
    }
    if (gpGame->bDream18 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Dream 18");
        return;
    }
    if (gpGame->nRegionalRound != 0) {
        // the regions follow the courses: region 1 is 24
        switch (gpGame->nRegionalRound + 23) {
        case 24:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "US Northwest");
            return;
        case 25:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "US Southwest");
            return;
        case 26:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "US East");
            return;
        case 27:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Europe");
            return;
        case 28:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Pacific");
            return;
        case 29:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "S. Hemisphere");
            return;
        default:
            strcpy(((MsgString*)pArgs[1].p)->pStr, "");
            return;
        }
    }
    if (gpGame->nCurCourse == 7 || Game_CurHoleIndex() >= 15) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Sherwood CC");
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gCourseNames[gpGame->nCurCourse]);
}

// The UI holds the HUD's message queue (pArgs[0] nonzero) or lets it go (command 145;
// GUI_SetMessageQueHeld: while held, no HUD message shows).
void GM_vSetMessageQueHeld(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        GUI_SetMessageQueHeld(1);
        return;
    }
    GUI_SetMessageQueHeld(0);
}

// Command 146: the free directory entries (files) left on the memory card in port pArgs[0], slot
// pArgs[1].
void GM_vIG_MCFreeFiles(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = card.nFreeFiles;
}

// Command 147: does nothing (empty in this build).
void GM_vIGMessage147_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 148: whether the player whose turn it is may concede: 0 while his post-shot HUD is
// animating, once the mode's pfnHoleFinished says his hole is over, or with his ball in the cup
// (lie 12); else 1.
void GM_vAllowConcede(MsgArg* pArgs, MsgArg* pResult) {
    if (GUI_IsPostShotUIAnimating(lbl_80282278)) {
        pResult->i = 0;
        return;
    }
    if (gpGame->pfnHoleFinished(lbl_80282278, 1)) {
        pResult->i = 0;
        return;
    }
    if (gPlayers[lbl_80282278].ball.nLie == 12) {
        pResult->i = 0;
        return;
    }
    pResult->i = 1;
}

// Command 149: ends the main loop: gSession.nC 2, which fn_8006D01C (gomainloop.c) checks each
// frame.
void IG_vEndGameLoop(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

// Command 150: how close the hole contest winner's ball is (HoleContest_GetWinnerShotKind: 1 in the
// cup, 2 within a foot of the pin, else 0).
void IG_vHoleContest_GetWinnerShotKind(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = HoleContest_GetWinnerShotKind();
}

// Command 151: the game's title, "TIGER WOODS PGA TOUR(R) 2004", pointed to by the string argument
// (as the front end's GM_vGetGameName).
void GM_vIG_GetGameName(MsgArg* pArgs, MsgArg* pResult) {
    ((MsgString*)pArgs[0].p)->pStr = "TIGER WOODS PGA TOUR\xAE 2004";
}

// Command 153: does nothing (empty in this build).
void GM_vIGMessage153_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 154: whether the event is played online: always 0 in this build.
void GM_vIsOnlineEvent(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Command 155: whether the save file on the card in port pArgs[0], slot pArgs[1] is bad data (the
// front end's command GM_vMCIsSaveCorrupt).
void GM_vIG_MCIsSaveCorrupt(MsgArg* pArgs, MsgArg* pResult) {
    GM_vMCIsSaveCorrupt(pArgs, pResult);
}

// Command 156: deletes the game's save from the card in port pArgs[0], slot pArgs[1] (the front
// end's command GM_vMCDeleteSave): the result is MC_DeleteSaveGame's error, or 1 once it is gone.
void GM_vIG_MCDeleteSave(MsgArg* pArgs, MsgArg* pResult) {
    GM_vMCDeleteSave(pArgs, pResult);
}

// Command 157: sets the UI's time factor gGameEffects.fUITimeFactor to the float pArgs[0]; a
// negative one becomes 1 (written back into pArgs[0] too). Nothing else in this build reads it.
void GM_vSetUITimeFactor(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].f < 0.0f) {
        pArgs[0].f = 1.0f;
    }
    gGameEffects.fUITimeFactor = pArgs[0].f;
}

// Command 158: a quick cheer from the crowd (Gaud_FireQuickCheer) and commentary line pArgs[0]
// (16-bit) of playlist 14.
void IG_vPlayCheerAndComment(MsgArg* pArgs, MsgArg* pResult) {
    int nMsg = pArgs[0].i;

    Gaud_FireQuickCheer();
    Gaud_StartPlaylist14Comment((u16)nMsg, 0);
}

// Command 159: whether a playoff is being played (gpGame->bInPlayoff).
void GM_vGetPlayoff(MsgArg* pArgs, MsgArg* pResult) {
    if (gpGame->bInPlayoff != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 160: plays the text-fall sound (Gaud_TextFall).
void IG_vPlayTextFall(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_TextFall();
}

// Command 161: whether player pArgs[0] missed the cut (Player.bPlayerCut).
void GM_vGetPlayerCut(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].bPlayerCut != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 162: an award is about to show: every help tip is hidden (GUI_HideAllHelpTips) and the
// award HUD starts (GUI_StartAwardUI).
void GM_vRewardDisplayStarting(MsgArg* pArgs, MsgArg* pResult) {
    GUI_HideAllHelpTips();
    GUI_StartAwardUI();
}

// Command 163: the scorecard is up: the game's sounds go quiet as for the scorecard
// (GUI_MuteForScoreCard).
void IG_vMuteForScoreCard(MsgArg* pArgs, MsgArg* pResult) {
    GUI_MuteForScoreCard();
}

// Command 166: for the card in port pArgs[0], slot pArgs[1], the new files and the blocks a game
// save needs, into *pArgs[2] and *pArgs[3] (the front end's command GM_vMCGetSaveNeeds).
void GM_vIG_MCGetSaveNeeds(MsgArg* pArgs, MsgArg* pResult) {
    GM_vMCGetSaveNeeds(pArgs, pResult);
}

// Command 167: sends chat text pArgs[0] from the player whose turn it is (OnlineGolf_SendChatData,
// empty in this build).
void GM_vIG_OnlineSendChat(MsgArg* pArgs, MsgArg* pResult) {
    OnlineGolf_SendChatData(lbl_80282278, ((MsgString*)pArgs[0].p)->pStr);
}

// Command 168: whether this is an online game: always 0 in this build.
void GM_vOnlineMode(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Command 195: always answers 0.
void GM_vIGMessage195_Return0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Command 169: a long-drive contest number of player pArgs[0] (modes 22 and 26 keep them), picked
// by pArgs[1]: 0 drives taken (nDrivesTaken), 1 the average length of the drives that count
// (nAverageDrive), 2 nBestDrive, 3..6 how many drives of kinds 1, 3, 4 and 5 (nBonusDrives,
// nSandDrives, nSurfacePenaltyDrives, nPenaltyDrives), 7 the points (nDriveScore), 8 the number of
// players. Any other pArgs[1] leaves the result alone.
void IG_vGetLongDriveStat(MsgArg* pArgs, MsgArg* pResult) {
    Player* pPlayer = &gPlayers[pArgs[0].i];

    switch (pArgs[1].i) {
    case 0:
        pResult->i = pPlayer->nDrivesTaken;
        return;
    case 1:
        pResult->i = pPlayer->nAverageDrive;
        return;
    case 2:
        pResult->i = pPlayer->nBestDrive;
        return;
    case 3:
        pResult->i = pPlayer->nBonusDrives;
        return;
    case 4:
        pResult->i = pPlayer->nSandDrives;
        return;
    case 5:
        pResult->i = pPlayer->nSurfacePenaltyDrives;
        return;
    case 6:
        pResult->i = pPlayer->nPenaltyDrives;
        return;
    case 7:
        pResult->i = pPlayer->nDriveScore;
        return;
    case 8:
        pResult->i = gSession.nNumPlayers;
        return;
    }
}

// Command 170: does nothing (empty in this build).
void GM_vIGMessage170_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 172: does nothing (empty in this build).
void GM_vIGMessage172_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 173: whether gSession.uFlags has both 0x4000 (the demo set-up) and 0x8000.
void IG_vIsDemoSetup(MsgArg* pArgs, MsgArg* pResult) {
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 175: does nothing (empty in this build).
void GM_vIGMessage175_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 176: how many clubs player pArgs[0] has in the bag.
void GM_vGetNumberClubs(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Bag_CountClubs(pArgs[0].i);
}

// Command 177: whether club pArgs[1] is in player pArgs[0]'s bag.
void GM_vGetClubAvailable(MsgArg* pArgs, MsgArg* pResult) {
    if (Bag_HasClub(pArgs[0].i, pArgs[1].i)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 178, Battle mode: puts club pArgs[2] into player pArgs[0]'s bag when pArgs[1] is set,
// else takes it out (GameModeBattle_RemoveClub keeps a required club).
void GM_vAddRemoveClub(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        GameModeBattle_AddClub(pArgs[0].i, pArgs[2].i);
        return;
    }
    GameModeBattle_RemoveClub(pArgs[0].i, pArgs[2].i);
}

// Command 180: asks for the other disc and loads the hole again: player 0's view runs camera script
// 3 (CameraController_HoldFadeColor, zero vector), the golfer waits (GS_WAIT), the other disc is
// asked for and waited on (fn_801102AC), the characters' texture files are reopened, a hole load is requested
// (fn_8006F4B4) and the view's camera script goes back to 0. The result is always 0.
void IG_vSwapDiscReloadHole(MsgArg* pArgs, MsgArg* pResult) {
    View* pView = ViewController_GetCameraControl(gPlayers[0].nView[0]);
    f32 vZero[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    pResult->i = 0;
    if (pResult->i == 0) {
        CameraController_HoldFadeColor(pView, vZero);
        GOLFERSTATE_Set(GS_WAIT, 0);
        fn_801102AC();
        Character_ReopenTextureFiles();
        fn_8006F4B4();
        pView->script.nFade = 0;
    }
}

// Command 179: the disc drive's state for the disc-swap screen: motor stopped 0 (disc 2 in the
// drive) or 1 (disc 1), wrong disc 2 (disc 2) or 3 (disc 1), busy 4, anything else 5; 100 once the
// disc change has finished (fn_80110450).
void IG_vGetDiscDriveStatus(MsgArg* pArgs, MsgArg* pResult) {
    switch (DVDGetDriveStatus()) {
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

// Command 181: does what command 8 does (GM_vClosePauseMenu: the pause menu closes; the lesson restarts,
// or while fading to black the hole load request is dropped), then answers whether the current
// hole's file is on the disc in the drive (fn_80110180 with fn_80110178's hole check on); if it is
// not, the hole load request is dropped (fn_8006F4E0).
void IG_vCloseMenuCheckDisc(MsgArg* pArgs, MsgArg* pResult) {
    GM_vClosePauseMenu(NULL, NULL);
    fn_80110178(1);
    pResult->i = fn_80110180();
    fn_80110178(0);
    if (pResult->i == 0) {
        fn_8006F4E0();
    }
}

// Command 197: does what command 113 does (GM_vPauseMenuClosed: the sounds resume, the pause menu closes,
// the lesson restarts unless fading to black), drops the hole load request unless a Play Now
// challenge is running (fn_8006F4E0), then answers whether the current hole's file is on the disc
// in the drive (fn_80110180 with fn_80110178's hole check on).
void IG_vResumeCheckDisc(MsgArg* pArgs, MsgArg* pResult) {
    GM_vPauseMenuClosed(NULL, NULL);
    if (!PlayNow_IsChallengeRunning()) {
        fn_8006F4E0();
    }
    fn_80110178(1);
    pResult->i = fn_80110180();
    fn_80110178(0);
}

// Command 183: does nothing (empty in this build).
void GM_vIGMessage183_Empty(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 184: the name of Play Now challenge group pArgs[1] (PlayNow_GetGroupName), copied into
// string pArgs[0].
void IG_vGetChallengeName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupName(pArgs[1].i));
}

// Command 185: the description of Play Now challenge group pArgs[1] (PlayNow_GetGroupDescription),
// copied into string pArgs[0].
void IG_vGetChallengeDescription(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupDescription(pArgs[1].i));
}

// Command 186: takes an online player's menu input; empty in this build, which has no online play.
void IG_vAcceptOnlineInput(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 187, Battle mode: how many clubs player pArgs[0] started with
// (GameModeBattle_GetNumberStartingClubs: the count GameModeBattle_SaveClubSetup kept).
void IG_vBattle_GetNumClubsStart(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_GetNumberStartingClubs(pArgs[0].i);
}

// Command 188, Battle mode: whether player pArgs[0] may take club pArgs[1]: one he started with and
// no longer has (GameModeBattle_CanAddClub).
void IG_vBattle_GetClubAddable(MsgArg* pArgs, MsgArg* pResult) {
    // the caller tests only the low byte of the result
    if ((u8)GameModeBattle_CanAddClub(pArgs[0].i, pArgs[1].i) != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 189: whether the player was paid in the PGA TOUR event just finished
// (GameModeDriverPGATour_GetWinInfo's bPlaced).
void IG_v_PGATourWin(MsgArg* pArgs, MsgArg* pResult) {
    if (GameModeDriverPGATour_GetWinInfo()->bPlaced != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 190: the text of the PGA TOUR prize cheque, into the five string arguments: the player's
// name, the money won ("$" and the amount with commas), the event's name, its end date
// (CalDate_ToString), and the place with its ordinal suffix ("22nd place").
void IG_v_PGATour_GetCheckInfo(MsgArg* pArgs, MsgArg* pResult) {
    PgaTour_WinInfo* pTour = GameModeDriverPGATour_GetWinInfo();
    int nPlace;

    strcpy(((MsgString*)pArgs[0].p)->pStr, gpSaveData->szName);
    ((MsgString*)pArgs[1].p)->pStr[0] = '$';
    UI_GetMoneyString(pTour->nWinnings, ((MsgString*)pArgs[1].p)->pStr + 1);
    strcpy(((MsgString*)pArgs[2].p)->pStr, GameModeDriverPGATour_GetName(gpSaveData->tour.nEvent));
    CalDate_ToString(GameModeDriverPGATour_GetEndDate(gpSaveData->tour.nEvent), ((MsgString*)pArgs[3].p)->pStr);
    nPlace = pTour->nPosition;
    if (nPlace > 100) {
        nPlace = pTour->nPosition % 100;
    }
    if (nPlace > 20) {
        nPlace %= 10;
    }
    if (nPlace == 1) {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%dst", pTour->nPosition);
    } else if (nPlace == 2) {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%dnd", pTour->nPosition);
    } else if (nPlace == 3) {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%drd", pTour->nPosition);
    } else {
        sprintf(((MsgString*)pArgs[4].p)->pStr, "%dth", pTour->nPosition);
    }
    strcat(((MsgString*)pArgs[4].p)->pStr, " place");
}

// Command 191: the current PGA TOUR event's name, copied into string pArgs[0].
void IG_vPgaTour_GetCurrEventName(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr,
           GameModeDriverPGATour_GetName(GameModeDriverPGATour_GetCurrentEventID()));
}

// Command 192: plays the text ditty (Gaud_PlayTextDitty) of a score animation: pArgs[0] the
// animation's kind (0, 2..13; others none), pArgs[1] 0 for the first ditty of its pair (2, 8, 12,
// 6, 2, 2, 2, 0, 4, 4, 4, 4, 10) or 2 for the second (one more); 1 plays none. Nothing in modes 22
// and 26.
void IG_vPlayShotScoreAnimSound(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 26 || Game_GetMode() == 22) return;
    switch (pArgs[1].i) {
    case 0:
        switch (pArgs[0].i) {
        case 0:
            Gaud_PlayTextDitty(2);
            return;
        case 2:
            Gaud_PlayTextDitty(8);
            return;
        case 3:
            Gaud_PlayTextDitty(12);
            return;
        case 4:
            Gaud_PlayTextDitty(6);
            return;
        case 5:
            Gaud_PlayTextDitty(2);
            return;
        case 6:
            Gaud_PlayTextDitty(2);
            return;
        case 7:
            Gaud_PlayTextDitty(2);
            return;
        case 8:
            Gaud_PlayTextDitty(0);
            return;
        case 9:
            Gaud_PlayTextDitty(4);
            return;
        case 10:
            Gaud_PlayTextDitty(4);
            return;
        case 11:
            Gaud_PlayTextDitty(4);
            return;
        case 12:
            Gaud_PlayTextDitty(4);
            return;
        case 13:
            Gaud_PlayTextDitty(10);
            return;
        }
        break;
    case 1:
        break;
    case 2:
        switch (pArgs[0].i) {
        case 0:
            Gaud_PlayTextDitty(3);
            return;
        case 2:
            Gaud_PlayTextDitty(9);
            return;
        case 3:
            Gaud_PlayTextDitty(13);
            return;
        case 4:
            Gaud_PlayTextDitty(7);
            return;
        case 5:
            Gaud_PlayTextDitty(3);
            return;
        case 6:
            Gaud_PlayTextDitty(3);
            return;
        case 7:
            Gaud_PlayTextDitty(3);
            return;
        case 8:
            Gaud_PlayTextDitty(1);
            return;
        case 9:
            Gaud_PlayTextDitty(5);
            return;
        case 10:
            Gaud_PlayTextDitty(5);
            return;
        case 11:
            Gaud_PlayTextDitty(5);
            return;
        case 12:
            Gaud_PlayTextDitty(5);
            return;
        case 13:
            Gaud_PlayTextDitty(11);
            break;
        }
        break;
    }
}

// Command 193: plays reward commentary line pArgs[1] (16-bit) from the playlist kind pArgs[0]
// picks: kinds 1 and 2 playlist 14, 6 playlist 18, 7 20, 8 15, 9 16, 10 17, 11 19; other kinds play
// nothing.
void IG_vPlayIngGameCommentaryRewardSound(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
    case 2:
        Gaud_StartPlaylist14Comment((u16)pArgs[1].i, 0);
        return;
    case 6:
        Gaud_StartPlaylist18Comment((u16)pArgs[1].i, 0);
        return;
    case 7:
        Gaud_StartPlaylist20Comment((u16)pArgs[1].i, 0);
        return;
    case 8:
        Gaud_StartPlaylist15Comment((u16)pArgs[1].i, 0);
        return;
    case 9:
        Gaud_StartPlaylist16Comment((u16)pArgs[1].i, 0);
        return;
    case 10:
        Gaud_StartPlaylist17Comment((u16)pArgs[1].i, 0);
        return;
    case 11:
        Gaud_StartPlaylist19Comment((u16)pArgs[1].i, 0);
        return;
    }
}

// Command 194: whether the reward commentary is still playing (Gaud_RewardCommentaryIsPlaying).
void IG_vRewardCommentaryIsPlaying(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Gaud_RewardCommentaryIsPlaying();
}

// Command 196: the width of string pArgs[0] (UFont.c fn_80012C30) times 512, as a float.
void IG_vGetStringSize(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

// Command 198: the local user's player index: always 0 in this build.
void IG_vGetLocalUserIndex(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// Command 199: whether the putting tip is shown: on/off game option a24[1], which the menus set
// (FE_MessageTable.c GM_vSetPuttingTipOption).
void IG_vShow_Putting_Tip(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.options.a24[1] != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 200: the shot clock's action; empty in this build.
void IG_vShotClockAction(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 201, Battle mode: how many clubs player pArgs[0] may still take out
// (GameModeBattle_NumRemovableClubsLeft).
void IG_vNumRemovableClubsLeft(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_NumRemovableClubsLeft(pArgs[0].i);
}

// Command 202: whether a GameBreaker is on: the GameBreaker letterbox is up (GameEffects_IsLetterboxOn).
void IG_vIsGameBreakerOn(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameEffects_IsLetterboxOn();
}

// Command 203: an online game's pause check; empty in this build, which has no online play.
void IG_vOnline_CheckPause(MsgArg* pArgs, MsgArg* pResult) {
}

// Command 204, Battle mode: whether the club screen shows: a club is to be taken after the last
// hole (GameModeBattle_ShowEndOfHole_ClubAddRemove_UI).
void IG_vBattleGolf_ShowClubUI(MsgArg* pArgs, MsgArg* pResult) {
    if (GameModeBattle_ShowEndOfHole_ClubAddRemove_UI() != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 205, Battle mode: the winner of the last hole (GameModeBattle_GetHoleWinner; 5 nobody).
void IG_vBattleGolf_GetHoleWinner(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_GetHoleWinner();
}

// Command 206: whether the game is paused (gSession.nPaused: the pause menu, or a pulled
// controller).
void IG_IsGamePaused(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.nPaused != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 207: the PGA TOUR message after the player's hole (whether he made the cut, or the
// playoff score to beat), into string pArgs[0] by GameModeDriverPGATour_DisplayEndOfHoleMessage; 1
// when there is one.
void IG_vPGATour_EndofHole_message(MsgArg* pArgs, MsgArg* pResult) {
    if ((u8)GameModeDriverPGATour_DisplayEndOfHoleMessage(((MsgString*)pArgs[0].p)->pStr) != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 208: sets (pArgs[0] nonzero) or clears the HUD flag GUI_SetUnreadFlag keeps; nothing in
// this build reads it.
void IG_vKeypopEnabled(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        GUI_SetUnreadFlag(0);
        return;
    }
    GUI_SetUnreadFlag(1);
}

// Command 209: whether a real-time event is being played (GM_Currently_RealtimeMode).
void IG_vGetRealTimeMode(MsgArg* pArgs, MsgArg* pResult) {
    if (GM_Currently_RealtimeMode() != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Command 210: player pArgs[0]'s strokes on hole pArgs[1] (0..17), or for pArgs[1] 18 the front
// nine's score (GM_GetPlayerRoundScoreThroughHole to 9), 19 the back nine's (the round less the
// front nine), 20 the round's (GM_GetPlayerRoundScore).
void IG_vGetCurrentPlayerNumberStrokes(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i == 18) {
        pResult->i = GM_GetPlayerRoundScoreThroughHole(pArgs[0].i, 9);
    } else if (pArgs[1].i == 19) {
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i) - GM_GetPlayerRoundScoreThroughHole(pArgs[0].i, 9);
    } else if (pArgs[1].i == 20) {
        pResult->i = GM_GetPlayerRoundScore(pArgs[0].i);
    } else {
        pResult->i = gPlayers[pArgs[0].i].nStrokes[pArgs[1].i];
    }
}

// Command 211: the disqualified golfer: always -1 (none) in this build.
void IG_vGetDisqualifiedGolfer(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = -1;
}

// Command 212: whether the golfer on row pArgs[0] of player 0's PGA TOUR leaderboard missed the cut
// (GM_PgaTourSim_GetEntrantIDFromScoreRow, GM_PgaTourSim_GetWasCutFromEntrantID).
void IG_vLeaderboard_WasCut(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_GetWasCutFromEntrantID(0, nEntrant);
}

// Command 213: whether player pArgs[0] missed the cut in the PGA TOUR event
// (GM_PgaTourSim_GetWasCutFromEntrantID of entrant 0, the player's own entry).
void IG_vLeaderboard_PlayerWasCut(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_PgaTourSim_GetWasCutFromEntrantID(pArgs[0].i, 0);
}

// The round's scoring kind, gpGame->nScoringType: 0 strokes, 1 holes won, 2 skins (the kinds
// HoleScore.c's leads count by).
s32 GM_GetScoringType(void) {
    return gpGame->nScoringType;
}

// How many holes the round plays (the holes set in gpGame->bHoleSelected).
s32 GM_GetNumHolesInRound(void) {
    s32 n = 0;
    int i;

    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            n++;
        }
    }
    return n;
}

// How many of the round's holes are left from the current one on.
s32 GM_GetNumHolesRemainingInRound(void) {
    s32 n = 0;
    int i;

    for (i = gpGame->nCurHole; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            n++;
        }
    }
    return n;
}

// An online game's step when the game ends: the end-of-game command (GM_vExitGame) calls it with (0,
// 1) in an online game. Empty in this build, which has no online play.
void OnlineGolf_OnEndOfGame(int a, int b) {
}

// Whether this is an online game: always 0, the GameCube build has no online play. When set, its
// callers skip the mid-hole flyby, keep the pad from advancing the post-shot HUD, fix the ball
// simulation's time budget and stop the demo timer.
u8 OnlineGolf_bIsOnlineGame(void) {
    return 0;
}

// Sends chat text sz from player nPlayer to the online opponents. Empty in this build, which has no
// online play.
void OnlineGolf_SendChatData(int nPlayer, char* sz) {
}

// Plays commentary line nMsg of playlist 14 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kinds 1 and 2, and the cheer command for its line.
void Gaud_StartPlaylist14Comment(int nMsg, int a) {
    Gaud_StartComment(14, nMsg, a);
}

// Plays commentary line nMsg of playlist 19 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kind 11.
void Gaud_StartPlaylist19Comment(int nMsg, int a) {
    Gaud_StartComment(19, nMsg, a);
}

// Plays commentary line nMsg of playlist 17 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kind 10.
void Gaud_StartPlaylist17Comment(int nMsg, int a) {
    Gaud_StartComment(17, nMsg, a);
}

// Plays commentary line nMsg of playlist 16 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kind 9.
void Gaud_StartPlaylist16Comment(int nMsg, int a) {
    Gaud_StartComment(16, nMsg, a);
}

// Plays commentary line nMsg of playlist 15 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kind 8.
void Gaud_StartPlaylist15Comment(int nMsg, int a) {
    Gaud_StartComment(15, nMsg, a);
}

// Plays commentary line nMsg of playlist 20 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kind 7.
void Gaud_StartPlaylist20Comment(int nMsg, int a) {
    Gaud_StartComment(20, nMsg, a);
}

// Plays commentary line nMsg of playlist 18 (Gaud_StartComment; a goes on as its third argument).
// The UI's commentary command uses it for kind 6.
void Gaud_StartPlaylist18Comment(int nMsg, int a) {
    Gaud_StartComment(18, nMsg, a);
}

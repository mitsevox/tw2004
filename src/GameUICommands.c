// GameUICommands.c (our name): the in-game UI messages, the questions and orders the menu UI can
// send while a round is on (session game types 4 to 8; uiProcessInterface.c's fn_8008F568 routes
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
s32 gSpeedGolfLogCycle;         // -1..10, one step per fn_800894E8 call; the event answer it writes
                                // is always overwritten after
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
void GM_vNoOp(MsgArg* pArgs, MsgArg* pResult);
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
void GM_vCommand92_Empty(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerShotSetup(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetPlayerBackSwing(MsgArg* pArgs, MsgArg* pResult);
void GM_vLessonsQuit(MsgArg* pArgs, MsgArg* pResult);
void GM_vLessonsContinue(MsgArg* pArgs, MsgArg* pResult);
void GM_vGetGolferController(MsgArg* pArgs, MsgArg* pResult);
void GM_vTimerOut(MsgArg* pArgs, MsgArg* pResult);
void fn_8008823C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008828C(MsgArg* pArgs, MsgArg* pResult);
void fn_800882C0(MsgArg* pArgs, MsgArg* pResult);
void fn_800882F4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088324(MsgArg* pArgs, MsgArg* pResult);
void fn_80088354(MsgArg* pArgs, MsgArg* pResult);
void fn_80088358(MsgArg* pArgs, MsgArg* pResult);
void fn_8008835C(MsgArg* pArgs, MsgArg* pResult);
void fn_800883FC(MsgArg* pArgs, MsgArg* pResult);
void fn_80088428(MsgArg* pArgs, MsgArg* pResult);
void fn_80088474(MsgArg* pArgs, MsgArg* pResult);
void fn_800884F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80088538(MsgArg* pArgs, MsgArg* pResult);
void fn_80088570(MsgArg* pArgs, MsgArg* pResult);
void fn_800885A0(MsgArg* pArgs, MsgArg* pResult);
void fn_800885F8(MsgArg* pArgs, MsgArg* pResult);
void fn_80088634(MsgArg* pArgs, MsgArg* pResult);
void fn_80088654(MsgArg* pArgs, MsgArg* pResult);
void fn_80088660(MsgArg* pArgs, MsgArg* pResult);
void fn_80088730(MsgArg* pArgs, MsgArg* pResult);
void fn_8008879C(MsgArg* pArgs, MsgArg* pResult);
void fn_800887C4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088804(MsgArg* pArgs, MsgArg* pResult);
void fn_80088830(MsgArg* pArgs, MsgArg* pResult);
void fn_80088834(MsgArg* pArgs, MsgArg* pResult);
void fn_8008886C(MsgArg* pArgs, MsgArg* pResult);
void fn_80088AD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088CC4(MsgArg* pArgs, MsgArg* pResult);
void fn_80088CF0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089324(MsgArg* pArgs, MsgArg* pResult);
void fn_80089414(MsgArg* pArgs, MsgArg* pResult);
void fn_800894B4(MsgArg* pArgs, MsgArg* pResult);
void fn_800894E8(MsgArg* pArgs, MsgArg* pResult);
void fn_80089584(MsgArg* pArgs, MsgArg* pResult);
void fn_80089590(MsgArg* pArgs, MsgArg* pResult);
void fn_80089600(MsgArg* pArgs, MsgArg* pResult);
void fn_80089648(MsgArg* pArgs, MsgArg* pResult);
void fn_8008967C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089688(MsgArg* pArgs, MsgArg* pResult);
void fn_8008968C(MsgArg* pArgs, MsgArg* pResult);
void fn_800896A8(MsgArg* pArgs, MsgArg* pResult);
void fn_800896B4(MsgArg* pArgs, MsgArg* pResult);
void fn_800896D0(MsgArg* pArgs, MsgArg* pResult);
void fn_800896F0(MsgArg* pArgs, MsgArg* pResult);
void fn_800897F0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089A50(MsgArg* pArgs, MsgArg* pResult);
void fn_80089A8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089AD0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089AD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80089B78(MsgArg* pArgs, MsgArg* pResult);
void fn_80089B8C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BBC(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BD0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BD4(MsgArg* pArgs, MsgArg* pResult);
void fn_80089BE0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C00(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C20(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C4C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089C84(MsgArg* pArgs, MsgArg* pResult);
void fn_80089CAC(MsgArg* pArgs, MsgArg* pResult);
void fn_80089CCC(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D04(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D28(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D48(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D68(MsgArg* pArgs, MsgArg* pResult);
void fn_80089D98(MsgArg* pArgs, MsgArg* pResult);
void fn_80089DA4(MsgArg* pArgs, MsgArg* pResult);
void fn_80089DB0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E5C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E60(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E64(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E98(MsgArg* pArgs, MsgArg* pResult);
void fn_80089E9C(MsgArg* pArgs, MsgArg* pResult);
void fn_80089ED0(MsgArg* pArgs, MsgArg* pResult);
void fn_80089F24(MsgArg* pArgs, MsgArg* pResult);
void fn_80089F6C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A010(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A0CC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A128(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A184(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A188(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A1C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A208(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A20C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A240(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A294(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A2E0(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A468(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A4A8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A690(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A758(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A788(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A7C8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A7D4(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A800(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A804(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A838(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A86C(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A870(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A8B8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A8E8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A914(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A964(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A9A0(MsgArg* pArgs, MsgArg* pResult);
void fn_8008A9E8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008AAAC(MsgArg* pArgs, MsgArg* pResult);
void fn_8008AAB8(MsgArg* pArgs, MsgArg* pResult);
void fn_8008AB04(MsgArg* pArgs, MsgArg* pResult);

// This file's helpers.
u8   IG_IsControllerInPlay(int nController);
void fn_8008AC3C(int a, int b);
u8   fn_8008AC40(void);
void fn_8008AC48(int nPlayer, char* sz);
void fn_8008AC4C(int nMsg, int a);
void fn_8008AC78(int nMsg, int a);
void fn_8008ACA4(int nMsg, int a);
void fn_8008ACD0(int nMsg, int a);
void fn_8008ACFC(int nMsg, int a);
void fn_8008AD28(int nMsg, int a);
void fn_8008AD54(int nMsg, int a);

// Other files' functions no header declares yet.
f32   GM_GetGolferDistanceToPin(int nPlayer);                 // GameManager.c
void  GM_GolferConcede_Hole(int nPlayer);                     // GameManager.c
void  GM_RestartHole(void);                                   // GameManager.c
void  GameModeBattle_AddClub(int nPlayer, int nClub);         // GameModeBattle.c
int   GameModeBattle_CanAddClub(int nPlayer, int nClub);
s32   GameModeBattle_GetWinner(void);
u8    GameModeBattle_ShowEndOfHole_ClubAddRemove_UI(void);
int   GameModeBattle_NumRemovableClubsLeft(int nPlayer);
u8    GameModeBattle_RemoveClub(int nPlayer, int nClub);
void  Character_ReopenTextureFiles(void);
void  fn_80062B84(int a);
void  fn_8006F4E0(void);
s32   fn_80084FB4(CardPos* pPos);
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
u8    fn_800C6E44(View* pView);
u8    fn_800C708C(View* pView);
void  fn_800C9038(int nView, f32* pLong, f32* pSide);    // GoBreakLine.c
s32   fn_800D2B4C(int nPlayer);
s32   fn_800D2D40(int nTeeSet);
s32   fn_800D2DA0(int nTeeSet);
s32   fn_800D2E00(int nTeeSet);
s32   fn_800D2E60(void);
s32   fn_800D2EB0(void);
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
s32   fn_800E8114(int nPlayer);
int   GM_BestBallMode_GetTeamRelativeScore(int nPlayer, u8 bCurrent);
u8    PlayNow_IsSpeedGolf(void);
s32   PlayNow_GetMedalMark(int k);
int   PlayNow_GetScoreToTarget(void);
char* PlayNow_GetGroupName(int nId);
char* PlayNow_GetGroupDescription(int nId);
int   PlayNow_GetHolesLeft(void);
void  PlayNow_Restart(void);
void  GameModePractice_FinishHole(void);
PgaTour_WinInfo* GameModeDriverPGATour_GetWinInfo(void);
s32   GameModeDriverPGATour_DisplayEndOfHoleMessage(char* pDst);
s32   GM_RealtimeMode_GetSelectedEvent(s32* pRound);
int   fn_800F1960(void);
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
    gIGMessageHandlers[92] = GM_vCommand92_Empty;
    gIGMessageHandlers[93] = GM_vGetPlayerShotSetup;
    gIGMessageHandlers[94] = GM_vLessonsQuit;
    gIGMessageHandlers[95] = GM_vLessonsContinue;
    gIGMessageHandlers[96] = GM_vGetGolferController;
    gIGMessageHandlers[97] = GM_vTimerOut;
    gIGMessageHandlers[98] = fn_8008823C;
    gIGMessageHandlers[99] = fn_8008828C;
    gIGMessageHandlers[100] = fn_800882C0;
    gIGMessageHandlers[101] = fn_800882F4;
    gIGMessageHandlers[102] = fn_80088324;
    gIGMessageHandlers[103] = fn_80088354;
    gIGMessageHandlers[104] = fn_80088358;
    gIGMessageHandlers[105] = fn_8008835C;
    gIGMessageHandlers[106] = fn_800883FC;
    gIGMessageHandlers[107] = fn_80088428;
    gIGMessageHandlers[108] = fn_80088474;
    gIGMessageHandlers[109] = fn_800884F0;
    gIGMessageHandlers[110] = fn_80088538;
    gIGMessageHandlers[111] = fn_80088570;
    gIGMessageHandlers[112] = fn_800885A0;
    gIGMessageHandlers[113] = fn_800885F8;
    gIGMessageHandlers[114] = fn_80088634;
    gIGMessageHandlers[115] = fn_80088654;
    gIGMessageHandlers[116] = GM_vGetModeValue;
    gIGMessageHandlers[117] = fn_80088660;
    gIGMessageHandlers[118] = fn_80088730;
    gIGMessageHandlers[120] = fn_800887C4;
    gIGMessageHandlers[121] = fn_80088804;
    gIGMessageHandlers[122] = fn_80088830;
    gIGMessageHandlers[123] = fn_80088834;
    gIGMessageHandlers[124] = fn_8008886C;
    gIGMessageHandlers[125] = fn_80088AD4;
    gIGMessageHandlers[126] = fn_80088CC4;
    gIGMessageHandlers[127] = fn_80088CF0;
    gIGMessageHandlers[128] = fn_80089324;
    gIGMessageHandlers[129] = fn_80089414;
    gIGMessageHandlers[130] = fn_800894B4;
    gIGMessageHandlers[131] = GM_vNoOp;
    gIGMessageHandlers[132] = fn_800894E8;
    gIGMessageHandlers[133] = fn_80089584;
    gIGMessageHandlers[134] = fn_80089590;
    gIGMessageHandlers[135] = fn_80089600;
    gIGMessageHandlers[136] = fn_80089648;
    gIGMessageHandlers[137] = fn_8008967C;
    gIGMessageHandlers[138] = fn_80089688;
    gIGMessageHandlers[139] = fn_8008968C;
    gIGMessageHandlers[140] = fn_800896A8;
    gIGMessageHandlers[141] = fn_800896B4;
    gIGMessageHandlers[142] = fn_800896D0;
    gIGMessageHandlers[143] = fn_800896F0;
    gIGMessageHandlers[144] = fn_800897F0;
    gIGMessageHandlers[145] = fn_80089A50;
    gIGMessageHandlers[146] = fn_80089A8C;
    gIGMessageHandlers[147] = fn_80089AD0;
    gIGMessageHandlers[148] = fn_80089AD4;
    gIGMessageHandlers[149] = fn_80089B78;
    gIGMessageHandlers[150] = fn_80089B8C;
    gIGMessageHandlers[151] = fn_80089BBC;
    gIGMessageHandlers[152] = GM_vGetPlayerShortName;
    gIGMessageHandlers[153] = fn_80089BD0;
    gIGMessageHandlers[154] = fn_80089BD4;
    gIGMessageHandlers[155] = fn_80089BE0;
    gIGMessageHandlers[156] = fn_80089C00;
    gIGMessageHandlers[157] = fn_80089C20;
    gIGMessageHandlers[158] = fn_80089C4C;
    gIGMessageHandlers[159] = fn_80089C84;
    gIGMessageHandlers[160] = fn_80089CAC;
    gIGMessageHandlers[161] = fn_80089CCC;
    gIGMessageHandlers[162] = fn_80089D04;
    gIGMessageHandlers[163] = fn_80089D28;
    gIGMessageHandlers[164] = fn_800834A8;
    gIGMessageHandlers[165] = GM_vGetPlayerBackSwing;
    gIGMessageHandlers[166] = fn_80089D48;
    gIGMessageHandlers[167] = fn_80089D68;
    gIGMessageHandlers[168] = fn_80089D98;
    gIGMessageHandlers[169] = fn_80089DB0;
    gIGMessageHandlers[170] = fn_80089E5C;
    gIGMessageHandlers[171] = fn_8008879C;
    gIGMessageHandlers[172] = fn_80089E60;
    gIGMessageHandlers[173] = fn_80089E64;
    gIGMessageHandlers[174] = GM_vGetPlayerFirstName;
    gIGMessageHandlers[175] = fn_80089E98;
    gIGMessageHandlers[176] = fn_80089E9C;
    gIGMessageHandlers[177] = fn_80089ED0;
    gIGMessageHandlers[178] = fn_80089F24;
    gIGMessageHandlers[180] = fn_80089F6C;
    gIGMessageHandlers[179] = fn_8008A010;
    gIGMessageHandlers[181] = fn_8008A0CC;
    gIGMessageHandlers[197] = fn_8008A128;
    gIGMessageHandlers[182] = GM_vGetPlayerHolePoints;
    gIGMessageHandlers[184] = fn_8008A188;
    gIGMessageHandlers[185] = fn_8008A1C8;
    gIGMessageHandlers[183] = fn_8008A184;
    gIGMessageHandlers[186] = fn_8008A208;
    gIGMessageHandlers[187] = fn_8008A20C;
    gIGMessageHandlers[188] = fn_8008A240;
    gIGMessageHandlers[189] = fn_8008A294;
    gIGMessageHandlers[190] = fn_8008A2E0;
    gIGMessageHandlers[191] = fn_8008A468;
    gIGMessageHandlers[192] = fn_8008A4A8;
    gIGMessageHandlers[193] = fn_8008A690;
    gIGMessageHandlers[194] = fn_8008A758;
    gIGMessageHandlers[195] = fn_80089DA4;
    gIGMessageHandlers[196] = fn_8008A788;
    gIGMessageHandlers[198] = fn_8008A7C8;
    gIGMessageHandlers[199] = fn_8008A7D4;
    gIGMessageHandlers[200] = fn_8008A800;
    gIGMessageHandlers[201] = fn_8008A804;
    gIGMessageHandlers[202] = fn_8008A838;
    gIGMessageHandlers[203] = fn_8008A86C;
    gIGMessageHandlers[204] = fn_8008A870;
    gIGMessageHandlers[205] = fn_8008A8B8;
    gIGMessageHandlers[206] = fn_8008A8E8;
    gIGMessageHandlers[207] = fn_8008A914;
    gIGMessageHandlers[208] = fn_8008A964;
    gIGMessageHandlers[209] = fn_8008A9A0;
    gIGMessageHandlers[210] = fn_8008A9E8;
    gIGMessageHandlers[211] = fn_8008AAAC;
    gIGMessageHandlers[212] = fn_8008AAB8;
    gIGMessageHandlers[213] = fn_8008AB04;
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

// Message 4: the round's course (Game_GetCourse); -1 when any of gpGame->b137..b139 is set (TW07
// tests compilation and random courses here), -2 for a custom course (b136).
void GM_vGetCourseIndex(MsgArg* pArgs, MsgArg* pResult) {
    if (gpGame->b137 != 0 || gpGame->b138 != 0 || gpGame->b139 != 0) {
        pResult->i = -1;
        return;
    }
    if (gpGame->b136 != 0) {
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
// (GUI_SetEndOfGamePending), in any other the fade to black starts (lbl_801D87C0.bFadeToBlack); in
// the PGA TOUR (mode 23) fn_80117DE8(0, 1). The online branch (fn_8008AC40, always 0 in this build)
// never runs.
void GM_vExitGame(MsgArg* pArgs, MsgArg* pResult) {
    if (Game_GetMode() == 9) {
        GUI_SetEndOfGamePending();
    } else {
        lbl_801D87C0.bFadeToBlack = 1;
    }
    if (Game_GetMode() == 23) {
        fn_80117DE8(0, 1);
    }
    if (fn_8008AC40()) {
        fn_80062B84(6);
        fn_8008AC3C(0, 1);
    }
}

// Message 8: the pause menu closes (GUI_PauseMenuClosed); unless the round is fading to black to
// end, the lesson restarts (Lessons_RestartLesson, lesson mode only); when it is, a hole load asked
// for is cancelled (fn_8006F4E0).
void GM_vClosePauseMenu(MsgArg* pArgs, MsgArg* pResult) {
    GUI_PauseMenuClosed();
    if (lbl_801D87C0.bFadeToBlack == 0) {
        Lessons_RestartLesson();
        return;
    }
    fn_8006F4E0();
}

// Message 9: pause: notes whether the scorecard was down (lbl_80281F18), pauses the sound and opens
// the pause menu.
void GM_vPauseGame(MsgArg* pArgs, MsgArg* pResult) {
    lbl_80281F18 = GUI_ScoreCardUp() == 0;
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

// Message 19: the lie angle of player pArgs[0]'s ball (ball.n6C).
void GM_vGetPlayerCurrentLieAngle(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].ball.n6C;
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

    Wind_Get(vWind);
    fAngle = atan2f(-vWind[0], vWind[2]) - fAim;
    while (fAngle < 0.0f) {
        fAngle += TWOPI;
    }
    while (fAngle > TWOPI) {
        fAngle -= TWOPI;
    }
    pResult->f = 8.0f * (fAngle / TWOPI);
}

// Message 21: the wind's speed (Wind_Get).
void GM_vGetPlayerWindSpeed(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Wind_Get(NULL);
}

// Message 22: the round's scoring method (fn_8008AB40: gpGame->n4).
void GM_vGetScoringMethod(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008AB40();
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
void GM_vNoOp(MsgArg* pArgs, MsgArg* pResult) {
}

// Message 26: the money in player pArgs[0]'s save profile (n6C), 0 when the profile is not active.
void GM_vGetGolferUserMoney(MsgArg* pArgs, MsgArg* pResult) {
    if (gpSaveData[gPlayers[pArgs[0].i].nIndex].bActive != 1) {
        pResult->i = 0;
        return;
    }
    pResult->i = gpSaveData[gPlayers[pArgs[0].i].nIndex].n6C;
}

// Message 27: whether the post-shot camera is done with player pArgs[0] (1; always for players 5
// and up): 1 when he is not in a reaction animation (nCurState not 9, 11 or 12), or when the
// camera's script has ended (fn_800C6E44) and either it is not tracking him (fn_800C708C) or his
// animation is paused or has less than the camera tuning's f170 left.
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
    bPostShotCamDone = fn_800C6E44(ViewController_GetCameraControl(gPlayers[pArgs[0].i].nView[0]));
    bCamTrackingPlayer = fn_800C708C(ViewController_GetCameraControl(gPlayers[pArgs[0].i].nView[0]));
    bAnimPaused = fn_80062C1C(gPlayers[pArgs[0].i].pChar);
    bNotReactionAnim = gPlayers[pArgs[0].i].pChar->nCurState != 9 &&
                       gPlayers[pArgs[0].i].pChar->nCurState != 11 &&
                       gPlayers[pArgs[0].i].pChar->nCurState != 12;
    bAnimAlmostDone = fn_80062C28(gPlayers[pArgs[0].i].pChar) < lbl_80281F78->f170;
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
        pResult->i = fn_800D2E60();
    } else if (pArgs[0].i == 19) {
        pResult->i = fn_800D2EB0();
    } else if (pArgs[0].i == 20) {
        pResult->i = fn_800D2FB4(gSession.nTeeSet[pArgs[1].i]);
    } else {
        pResult->i = Course_GetHolePar(pArgs[0].i);
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

// Message 31: the skins player pArgs[0] has won (Player.n274, TW06 skinwins).
void GM_vGetSkinWins(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = gPlayers[pArgs[0].i].n274;
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
// target games' (13 also sounds the ten-second warning and answers 0); 27 fn_800F1960; the rest
// Player fields (nDD8, aDC4, nDC0, nDDC, nDE0, nE88, nE8C, nE94; 25 is aDC4[1] + 5). Any other
// pArgs[1] leaves the answer as it was.
void GM_vGetModeValue(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].nDD8;
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
        pResult->i = gPlayers[pArgs[0].i].nDD8;
        return;
    case 6:
        pResult->i = gPlayers[pArgs[0].i].aDC4[4];
        return;
    case 7:
        pResult->i = gPlayers[pArgs[0].i].aDC4[0];
        return;
    case 8:
        pResult->i = gPlayers[pArgs[0].i].nDC0;
        return;
    case 9:
        pResult->i = GameModeSkillZoneBase_CountGreensHit(pArgs[0].i);
        return;
    case 10:
        pResult->i = gPlayers[pArgs[0].i].nDDC;
        return;
    case 11:
        pResult->i = gPlayers[pArgs[0].i].aDC4[3];
        return;
    case 12:
        pResult->i = gPlayers[pArgs[0].i].nDE0;
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
        pResult->i = gPlayers[pArgs[0].i].nE88;
        return;
    case 21:
        pResult->i = GameModeSkillZoneHorse_GetCurrentLeaderRing();
        return;
    case 22:
        pResult->i = gPlayers[pArgs[0].i].nE8C;
        return;
    case 23:
        pResult->i = gPlayers[pArgs[0].i].nE94;
        return;
    case 24:
        pResult->i = gPlayers[pArgs[0].i].aDC4[1];
        return;
    case 25:
        pResult->i = gPlayers[pArgs[0].i].aDC4[1] + 5;
        return;
    case 26:
        pResult->i = GameModeSkillZoneHorse_GetLastShotExceeded();
        return;
    case 27:
        pResult->i = fn_800F1960();
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

// Message 35: hole pArgs[0]'s rating (fn_800D2B4C: the course table's n04 for that hole).
void GM_vGetHoleRating(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800D2B4C(pArgs[0].i);
}

// Message 36: hole pArgs[0]'s length from tee set pArgs[1]; "hole" 18 is the front nine, 19 the
// back nine, 20 the round.
void GM_vGetTeeYardage(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 18) {
        pResult->i = fn_800D2DA0(pArgs[1].i);
    } else if (pArgs[0].i == 19) {
        pResult->i = fn_800D2E00(pArgs[1].i);
    } else if (pArgs[0].i == 20) {
        pResult->i = fn_800D2D40(pArgs[1].i);
    } else {
        pResult->i = fn_800D2C30(pArgs[0].i, pArgs[1].i);
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

    strcpy(((MsgString*)pArgs[1].p)->pStr, fn_80118E30(0, nGolfer));
}

// Message 44: the place of the golfer on leaderboard row pArgs[0].
void GM_vLeaderboard_Position(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_GetScoreRankFromEntrantID(0, nEntrant);
}

// Message 45: whether the golfer on leaderboard row pArgs[0] shares his place (fn_80119808).
void GM_vLeaderboard_Tied(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = fn_80119808(0, nEntrant);
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

// Message 50: whether the player (entrant 0) shares his place on the leaderboard (fn_80119808).
void GM_vLeaderboard_PlayerTied(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_80119808(0, 0);
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

    if (lbl_801D7148.aLoaded[nSlot] == 0) {
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

// Message 55: the caddie's putt read for player pArgs[0]'s view (fn_800C9038): how far past or
// short, and how far to the side, into the floats pArgs[1] and pArgs[2] point to.
void GM_vGetPuttHelp(MsgArg* pArgs, MsgArg* pResult) {
    // port: the studio passes the addresses of the two answers as 32-bit words
    fn_800C9038(gPlayers[pArgs[0].i].nView[0], (f32*)pArgs[1].i, (f32*)pArgs[2].i);
}

// A tournament entrant in a profile: GetEntrantMCPtr's body (PGATourSimulation.c), pasted in.
// Written out as gpSaveData[nPlayer].tour.field.aEntrant[n] it adds the entrant's offset last.
static inline PgaEntrantMC* Tour_EntrantMC(PlayerNumber_t nPlayer, int nEntrant) {
    return &gpSaveData[nPlayer].tour.field.aEntrant[nEntrant];
}

// Message 56: the winnings (PgaEntrantMC.n18, profile 0) of the PGA TOUR entrant on leaderboard row
// pArgs[0].
void GM_vLeaderboard_Winnings(MsgArg* pArgs, MsgArg* pResult) {
    PlayerNumber_t nPlayer = PLR_1_e;

    pResult->i = Tour_EntrantMC(nPlayer, GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i))->n18;
}

// Message 57: the winnings of the player (entrant 0) in save profile pArgs[0].
void GM_vLeaderboard_PlayerWinnings(MsgArg* pArgs, MsgArg* pResult) {
    SaveProfile* pProfile = &gpSaveData[pArgs[0].i];

    pResult->i = pProfile->tour.field.aEntrant[0].n18;
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

    if (lbl_801D7148.aLoaded[nSlot] == 0) {
        sprintf(szName, "User %d", nSlot + 1);
        strcpy(((MsgString*)pArgs[1].p)->pStr, szName);
        return;
    }
    strcpy(((MsgString*)pArgs[1].p)->pStr, gpSaveData[nSlot].szName);
}

// Message 63: how many holes the round plays (fn_8008AB4C).
void GM_vGetNumHolesSelected(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008AB4C();
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
    fn_80062C80(gPlayers[lbl_80282278].nC58, 0);
    GUI_CaddieTipWindowIsOpen();
}

// Turns the full caddie tips off for the player whose turn it is (command 75): sets his save
// profile's b522F, when the profile is in use, and CTIP_ShowCaddieTip then shows only short tips.
void GM_vDisableCaddieTips(MsgArg* pArgs, MsgArg* pResult) {
    if (gpSaveData[gPlayers[lbl_80282278].nIndex].bActive != 0) {
        gpSaveData[gPlayers[lbl_80282278].nIndex].b522F = 1;
    }
}

// The space a replay save still needs on the card in port pArgs[0], slot pArgs[1] (command 76): the
// card is looked at, the replay file type's card operations are picked (fn_80084FF0(2); TW07's
// MC_SetCurrentFileType) and their memory-required one asked (fn_80084FB4; TW07's
// MC_CallActionFnMemoryRequired).
void GM_vIG_MCMemforReplay(MsgArg* pArgs, MsgArg* pResult) {
    CardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_ConnectCard(pos.nPort, pos.nSlot);
    fn_80084FF0(2);
    pResult->i = fn_80084FB4(&pos);
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
    pResult->i = (card.uFlags & 0x08) != 0;
}

// The free space on the card in port pArgs[0], slot pArgs[1], in whole sectors (command 82).
void GM_vIG_MCfreeMem(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = card.nFreeBlocks;
}

// How many replays the save on the card in port pArgs[0], slot pArgs[1] holds, 0 on an error
// (command 83): the menus' own command fn_8007E9BC, run with the same arguments.
void GM_vIG_MCGetNumReplays(MsgArg* pArgs, MsgArg* pResult) {
    fn_8007E9BC(pArgs, pResult);
}

// Asks for pArgs[0] to be handed back to the UI a little later (command 84): uiProcessInterface.c
// counts three UI updates (lbl_801D880C.n0), then sends it to the UI as hint 0x24 in a round (0x23
// in the menus). The menus' fn_8007C988 does the same.
void GM_vIG_MCfunction(MsgArg* pArgs, MsgArg* pResult) {
    lbl_801D880C.n4 = pArgs[0].i;
    lbl_801D880C.n0 = 0;
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
// fn_8008AC00).
void GM_vGetNumHolesRemaining(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8008AC00();
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
void GM_vCommand92_Empty(MsgArg* pArgs, MsgArg* pResult) {
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
void fn_8008823C(MsgArg* pArgs, MsgArg* pResult) {
    if ((s8)GOLFERSTATE_GetCurrentState(pArgs[0].i) == GS_ELEVATOR) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008828C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolf_GetWinner((s32*)pArgs[0].p);
}

// Whether the round plays every hole.
void fn_800882C0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_FullRoundOfGolf();
}

void fn_800882F4(MsgArg* pArgs, MsgArg* pResult) {
    SpeedGolf_GetPrizeScores((s32*)pArgs[0].p, (s32*)pArgs[1].p, (s32*)pArgs[2].p);
}

void fn_80088324(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = PlayNow_GetHolesLeft();
}

void fn_80088354(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80088358(MsgArg* pArgs, MsgArg* pResult) {
}

// The event's name and description: a real-time event's, else the challenge's.
void fn_8008835C(MsgArg* pArgs, MsgArg* pResult) {
    s32 nRound;

    if (GM_Currently_RealtimeMode()) {
        strcpy(((MsgString*)pArgs[0].p)->pStr,
               GameModeDriverRTE_GetName(GM_RealtimeMode_GetSelectedEvent(&nRound)));
        strcpy(((MsgString*)pArgs[1].p)->pStr,
               GameModeDriverRTE_GetDescription(GM_RealtimeMode_GetSelectedEvent(&nRound)));
        return;
    }
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupName(fn_800EAC7C()));
    strcpy(((MsgString*)pArgs[1].p)->pStr, PlayNow_GetGroupDescription(fn_800EAC7C()));
}

void fn_800883FC(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_PlayGameUISound(pArgs[0].i, pArgs[1].i);
}

void fn_80088428(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolfPoints_GetNamesAndPoints(((MsgString*)pArgs[0].p)->pStr, (s32*)pArgs[1].p,
                             ((MsgString*)pArgs[2].p)->pStr, (s32*)pArgs[3].p);
}

// The round's next hole after the current one: its number (-1: none), and into pArgs its par and
// its length from player 0's tees.
void fn_80088474(MsgArg* pArgs, MsgArg* pResult) {
    int nHole;
    s32 nPar = 0;
    s32 nLength = 0;

    nHole = GM_GetNextSelectedHole();

    if (nHole != -1) {
        nPar = Course_GetHolePar(nHole);
        nLength = fn_800D2C30(nHole, gSession.nTeeSet[0]);
    }
    pResult->i = nHole;
    *(s32*)pArgs[0].p = nPar;
    *(s32*)pArgs[1].p = nLength;
}

void fn_800884F0(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = SpeedGolf_GetRoundScore(((MsgString*)pArgs[0].p)->pStr, (s32*)pArgs[1].p, (s32*)pArgs[2].p,
                             (s32*)pArgs[3].p);
}

void fn_80088538(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = IG_IsControllerInPlay(pArgs[0].i);
}

void fn_80088570(MsgArg* pArgs, MsgArg* pResult) {
    MC_ConnectCard(pArgs[0].i, pArgs[1].i);
    MC_Disconnect();
}

// The same as GM_vIG_MCMemforReplay.
void fn_800885A0(MsgArg* pArgs, MsgArg* pResult) {
    CardPos pos;

    pos.nPort = pArgs[0].i;
    pos.nSlot = pArgs[1].i;
    MC_ConnectCard(pos.nPort, pos.nSlot);
    fn_80084FF0(2);
    pResult->i = fn_80084FB4(&pos);
    MC_Disconnect();
}

void fn_800885F8(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_Pause(0);
    GUI_PauseMenuClosed();
    if (lbl_801D87C0.bFadeToBlack == 0) {
        Lessons_RestartLesson();
    }
}

void fn_80088634(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_FireQuickCheer();
}

void fn_80088654(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// The modes' questions, picked by pArgs[2].
void fn_80088660(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80088730(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[1].i) {
    case 0:
        pResult->i = gPlayers[pArgs[0].i].nD70[Game_CurHoleIndex()];
        return;
    }
}

void fn_8008879C(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.uFlags & 0x4000) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_800887C4(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.nGameType == 6 && (gSession.nPaused == 2 || gSession.nPaused == 3)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80088804(MsgArg* pArgs, MsgArg* pResult) {
    if (GM_Currently_SkillZoneMode()) {
        GameModeSkillZoneBase_ShotClockOut();
    }
}

void fn_80088830(MsgArg* pArgs, MsgArg* pResult) {
}

// A string's length.
void fn_80088834(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = strlen(((MsgString*)pArgs[0].p)->pStr);
}

// A record holder's name: pArgs[0] 3 is the contest's, 0 the all-time records' (kind pArgs[1]),
// else the mode's records or the course's; pArgs[2] is the place.
void fn_8008886C(MsgArg* pArgs, MsgArg* pResult) {
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

// The same records' values.
void fn_80088AD4(MsgArg* pArgs, MsgArg* pResult) {
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

// Print a number with commas.
void fn_80088CC4(MsgArg* pArgs, MsgArg* pResult) {
    fn_800907AC(pArgs[0].i, ((MsgString*)pArgs[1].p)->pStr);
}

// A player's money for the round by kind, and the multipliers that made it, picked by pArgs[1].
void fn_80088CF0(MsgArg* pArgs, MsgArg* pResult) {
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
        pResult->i = gpSaveData[gPlayers[pArgs[0].i].nIndex].n6C - gPlayers[(int)pArgs[0].i].money.n24;
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
        fn_801025F4();
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
        fn_801025F4();
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

// A record holder's name, with its string's length: pArgs[0] 0 is the all-time records, else the
// course's.
void fn_80089324(MsgArg* pArgs, MsgArg* pResult) {
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
void fn_80089414(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        pResult->i = gSession.recA[pArgs[1].i][pArgs[2].i].nValue;
        return;
    }
    pResult->i = gSession.aCourseRecord[Game_GetCourse()].aRecord[pArgs[1].i][pArgs[2].i].nValue;
}

// The ball's distance from the pin.
void fn_800894B4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = fn_800D0478(pArgs[0].i);
}

// Entry pArgs[0] of the speed-golf event log, into pArgs[1] (the player) and pArgs[2] (the event).
void fn_800894E8(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80089584(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 7;
}

void fn_80089590(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80089600(MsgArg* pArgs, MsgArg* pResult) {
    if (fn_801025F4()) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089648(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Misc_RandFunc(1);
}

void fn_8008967C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80089688(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008968C(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[0].p = 2;
    *(s32*)pArgs[1].p = 1;
}

void fn_800896A8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 2;
}

void fn_800896B4(MsgArg* pArgs, MsgArg* pResult) {
    *(s32*)pArgs[1].p = pArgs[0].i;
    *(s32*)pArgs[2].p = 0;
}

void fn_800896D0(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008299C(pArgs, pResult);
}

// The card in port pArgs[0], slot pArgs[1]: into pArgs[2..6] whether its sectors are not 8 KB, and
// its encoding, wrong-device, I/O-error and broken flags.
void fn_800896F0(MsgArg* pArgs, MsgArg* pResult) {
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

// The course's name, or the round's: a custom round's saved name, "Random 18", "Dream 18" or a
// region's courses.
void fn_800897F0(MsgArg* pArgs, MsgArg* pResult) {
    if ((gSession.uFlags & 0x4000) && gpGame->b136 != 0) {
        if (gpGame->nCurCourse == 7 || Game_CurHoleIndex() >= 15) {
            strcpy(((MsgString*)pArgs[1].p)->pStr, "Sherwood CC");
            return;
        }
        strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[gpGame->nCurCourse]);
        return;
    }
    if (gpGame->b136 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr,
               gpSaveData[gpGame->nSaveSlot].aSavedRound[gpGame->nSaveCourse].szName);
        return;
    }
    if (gpGame->b137 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Random 18");
        return;
    }
    if (gpGame->b138 != 0) {
        strcpy(((MsgString*)pArgs[1].p)->pStr, "Dream 18");
        return;
    }
    if (gpGame->b139 != 0) {
        // the regions follow the courses: region 1 is 24
        switch (gpGame->b139 + 23) {
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
    strcpy(((MsgString*)pArgs[1].p)->pStr, lbl_80191990[gpGame->nCurCourse]);
}

void fn_80089A50(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i != 0) {
        GUI_SetMessageQueHeld(1);
        return;
    }
    GUI_SetMessageQueHeld(0);
}

// The card's free directory entries.
void fn_80089A8C(MsgArg* pArgs, MsgArg* pResult) {
    MCCardState card;

    MC_GetMC(&card, pArgs[0].i, pArgs[1].i);
    pResult->i = card.nFreeFiles;
}

void fn_80089AD0(MsgArg* pArgs, MsgArg* pResult) {
}

// Whether the player whose turn it is may still play: nothing holds him, the hole is not over for
// him and his ball is not in the cup.
void fn_80089AD4(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_80089B78(MsgArg* pArgs, MsgArg* pResult) {
    gSession.nC = 2;
}

void fn_80089B8C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = HoleContest_GetWinnerShotKind();
}

// The game's title.
void fn_80089BBC(MsgArg* pArgs, MsgArg* pResult) {
    ((MsgString*)pArgs[0].p)->pStr = "TIGER WOODS PGA TOUR\xAE 2004";
}

void fn_80089BD0(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80089BD4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80089BE0(MsgArg* pArgs, MsgArg* pResult) {
    fn_80082DBC(pArgs, pResult);
}

void fn_80089C00(MsgArg* pArgs, MsgArg* pResult) {
    fn_80082E10(pArgs, pResult);
}

void fn_80089C20(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].f < 0.0f) {
        pArgs[0].f = 1.0f;
    }
    gGameEffects.f54 = pArgs[0].f;
}

void fn_80089C4C(MsgArg* pArgs, MsgArg* pResult) {
    int nMsg = pArgs[0].i;

    Gaud_FireQuickCheer();
    fn_8008AC4C((u16)nMsg, 0);
}

void fn_80089C84(MsgArg* pArgs, MsgArg* pResult) {
    if (gpGame->bInPlayoff != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089CAC(MsgArg* pArgs, MsgArg* pResult) {
    Gaud_TextFall();
}

// Whether the player missed the cut.
void fn_80089CCC(MsgArg* pArgs, MsgArg* pResult) {
    if (gPlayers[pArgs[0].i].bPlayerCut != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089D04(MsgArg* pArgs, MsgArg* pResult) {
    GUI_HideAllHelpTips();
    GUI_StartAwardUI();
}

void fn_80089D28(MsgArg* pArgs, MsgArg* pResult) {
    GUI_MuteForScoreCard();
}

void fn_80089D48(MsgArg* pArgs, MsgArg* pResult) {
    fn_800834E8(pArgs, pResult);
}

void fn_80089D68(MsgArg* pArgs, MsgArg* pResult) {
    fn_8008AC48(lbl_80282278, ((MsgString*)pArgs[0].p)->pStr);
}

void fn_80089D98(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_80089DA4(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

// A player's value of the modes' own, picked by pArgs[1] (8: the number of players).
void fn_80089DB0(MsgArg* pArgs, MsgArg* pResult) {
    Player* pPlayer = &gPlayers[pArgs[0].i];

    switch (pArgs[1].i) {
    case 0:
        pResult->i = pPlayer->nEA0;
        return;
    case 1:
        pResult->i = pPlayer->nEC0;
        return;
    case 2:
        pResult->i = pPlayer->nEA8;
        return;
    case 3:
        pResult->i = pPlayer->nECC;
        return;
    case 4:
        pResult->i = pPlayer->nED4;
        return;
    case 5:
        pResult->i = pPlayer->nED8;
        return;
    case 6:
        pResult->i = pPlayer->nEDC;
        return;
    case 7:
        pResult->i = pPlayer->nEBC;
        return;
    case 8:
        pResult->i = gSession.nNumPlayers;
        return;
    }
}

void fn_80089E5C(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80089E60(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_80089E64(MsgArg* pArgs, MsgArg* pResult) {
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_80089E98(MsgArg* pArgs, MsgArg* pResult) {
}

// The clubs in a player's bag.
void fn_80089E9C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Bag_CountClubs(pArgs[0].i);
}

// Whether a club is in a player's bag.
void fn_80089ED0(MsgArg* pArgs, MsgArg* pResult) {
    if (Bag_HasClub(pArgs[0].i, pArgs[1].i)) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Battle mode: add a club to a player's bag (pArgs[1] set) or take it out.
void fn_80089F24(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[1].i != 0) {
        GameModeBattle_AddClub(pArgs[0].i, pArgs[2].i);
        return;
    }
    GameModeBattle_RemoveClub(pArgs[0].i, pArgs[2].i);
}

void fn_80089F6C(MsgArg* pArgs, MsgArg* pResult) {
    View* pView = ViewController_GetCameraControl(gPlayers[0].nView[0]);
    f32 vZero[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    pResult->i = 0;
    if (pResult->i == 0) {
        fn_80063CBC(pView, vZero);
        GOLFERSTATE_Set(GS_WAIT, 0);
        fn_801102AC();
        Character_ReopenTextureFiles();
        fn_8006F4B4();
        pView->script.nCamera = 0;
    }
}

// The disc drive's state for the menus (100: fn_80110450 says so).
void fn_8008A010(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8008A0CC(MsgArg* pArgs, MsgArg* pResult) {
    GM_vClosePauseMenu(NULL, NULL);
    fn_80110178(1);
    pResult->i = fn_80110180();
    fn_80110178(0);
    if (pResult->i == 0) {
        fn_8006F4E0();
    }
}

void fn_8008A128(MsgArg* pArgs, MsgArg* pResult) {
    fn_800885F8(NULL, NULL);
    if (!PlayNow_IsChallengeRunning()) {
        fn_8006F4E0();
    }
    fn_80110178(1);
    pResult->i = fn_80110180();
    fn_80110178(0);
}

void fn_8008A184(MsgArg* pArgs, MsgArg* pResult) {
}

// A challenge's first line of text.
void fn_8008A188(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupName(pArgs[1].i));
}

// Its second line.
void fn_8008A1C8(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr, PlayNow_GetGroupDescription(pArgs[1].i));
}

void fn_8008A208(MsgArg* pArgs, MsgArg* pResult) {
}

void fn_8008A20C(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_800E8114(pArgs[0].i);
}

// Battle mode: whether a player may take a club.
void fn_8008A240(MsgArg* pArgs, MsgArg* pResult) {
    // the caller tests only the low byte of the result
    if ((u8)GameModeBattle_CanAddClub(pArgs[0].i, pArgs[1].i) != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A294(MsgArg* pArgs, MsgArg* pResult) {
    if (GameModeDriverPGATour_GetWinInfo()->bPlaced != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The PGA TOUR result screen: the player's name, the money won ("$1,234"), the tournament's name
// and end date, and the place ("1st place").
void fn_8008A2E0(MsgArg* pArgs, MsgArg* pResult) {
    PgaTour_WinInfo* pTour = GameModeDriverPGATour_GetWinInfo();
    int nPlace;

    strcpy(((MsgString*)pArgs[0].p)->pStr, gpSaveData->szName);
    ((MsgString*)pArgs[1].p)->pStr[0] = '$';
    fn_800907AC(pTour->nWinnings, ((MsgString*)pArgs[1].p)->pStr + 1);
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

// The current PGA TOUR event's name.
void fn_8008A468(MsgArg* pArgs, MsgArg* pResult) {
    strcpy(((MsgString*)pArgs[0].p)->pStr,
           GameModeDriverPGATour_GetName(GameModeDriverPGATour_GetCurrentEventID()));
}

// Pass on to Gaud_PlayTextDitty a number for pArgs[0] and pArgs[1] (0 or 2); nothing in modes 22 and 26.
void fn_8008A4A8(MsgArg* pArgs, MsgArg* pResult) {
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

// Play sound pArgs[1] of kind pArgs[0]. The sound numbers are 16-bit.
void fn_8008A690(MsgArg* pArgs, MsgArg* pResult) {
    switch (pArgs[0].i) {
    case 1:
    case 2:
        fn_8008AC4C((u16)pArgs[1].i, 0);
        return;
    case 6:
        fn_8008AD54((u16)pArgs[1].i, 0);
        return;
    case 7:
        fn_8008AD28((u16)pArgs[1].i, 0);
        return;
    case 8:
        fn_8008ACFC((u16)pArgs[1].i, 0);
        return;
    case 9:
        fn_8008ACD0((u16)pArgs[1].i, 0);
        return;
    case 10:
        fn_8008ACA4((u16)pArgs[1].i, 0);
        return;
    case 11:
        fn_8008AC78((u16)pArgs[1].i, 0);
        return;
    }
}

void fn_8008A758(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = Gaud_RewardCommentaryIsPlaying();
}

void fn_8008A788(MsgArg* pArgs, MsgArg* pResult) {
    pResult->f = 512.0f * fn_80012C30(((MsgString*)pArgs[0].p)->pStr);
}

void fn_8008A7C8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = 0;
}

void fn_8008A7D4(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.options.a24[1] != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A800(MsgArg* pArgs, MsgArg* pResult) {
}

// Battle mode: how many clubs a player may still take out.
void fn_8008A804(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_NumRemovableClubsLeft(pArgs[0].i);
}

void fn_8008A838(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = fn_8003DCAC();
}

void fn_8008A86C(MsgArg* pArgs, MsgArg* pResult) {
}

// Battle mode: whether a club is to be taken: the last hole had a winner and the game goes on.
void fn_8008A870(MsgArg* pArgs, MsgArg* pResult) {
    if (GameModeBattle_ShowEndOfHole_ClubAddRemove_UI() != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// Battle mode: the winner.
void fn_8008A8B8(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GameModeBattle_GetWinner();
}

// Whether the game is paused.
void fn_8008A8E8(MsgArg* pArgs, MsgArg* pResult) {
    if (gSession.nPaused != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A914(MsgArg* pArgs, MsgArg* pResult) {
    if ((u8)GameModeDriverPGATour_DisplayEndOfHoleMessage(((MsgString*)pArgs[0].p)->pStr) != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

void fn_8008A964(MsgArg* pArgs, MsgArg* pResult) {
    if (pArgs[0].i == 0) {
        GUI_SetUnreadFlag(0);
        return;
    }
    GUI_SetUnreadFlag(1);
}

// Whether a real-time event is being played.
void fn_8008A9A0(MsgArg* pArgs, MsgArg* pResult) {
    if (GM_Currently_RealtimeMode() != 0) {
        pResult->i = 1;
        return;
    }
    pResult->i = 0;
}

// The same as GM_vGetPlayerHoleScore, without mode 19's count.
void fn_8008A9E8(MsgArg* pArgs, MsgArg* pResult) {
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

void fn_8008AAAC(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = -1;
}

// Whether the golfer on a leaderboard row missed the cut.
void fn_8008AAB8(MsgArg* pArgs, MsgArg* pResult) {
    s32 nEntrant = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, pArgs[0].i);

    pResult->i = GM_PgaTourSim_GetWasCutFromEntrantID(0, nEntrant);
}

void fn_8008AB04(MsgArg* pArgs, MsgArg* pResult) {
    pResult->i = GM_PgaTourSim_GetWasCutFromEntrantID(pArgs[0].i, 0);
}

s32 fn_8008AB40(void) {
    return gpGame->n4;
}

// How many holes the round plays.
s32 fn_8008AB4C(void) {
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
s32 fn_8008AC00(void) {
    s32 n = 0;
    int i;

    for (i = gpGame->nCurHole; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            n++;
        }
    }
    return n;
}

void fn_8008AC3C(int a, int b) {
}

u8 fn_8008AC40(void) {
    return 0;
}

void fn_8008AC48(int nPlayer, char* sz) {
}

void fn_8008AC4C(int nMsg, int a) {
    Gaud_StartComment(14, nMsg, a);
}

void fn_8008AC78(int nMsg, int a) {
    Gaud_StartComment(19, nMsg, a);
}

void fn_8008ACA4(int nMsg, int a) {
    Gaud_StartComment(17, nMsg, a);
}

void fn_8008ACD0(int nMsg, int a) {
    Gaud_StartComment(16, nMsg, a);
}

void fn_8008ACFC(int nMsg, int a) {
    Gaud_StartComment(15, nMsg, a);
}

void fn_8008AD28(int nMsg, int a) {
    Gaud_StartComment(20, nMsg, a);
}

void fn_8008AD54(int nMsg, int a) {
    Gaud_StartComment(18, nMsg, a);
}

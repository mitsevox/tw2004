// GameModeReplay.c (TW06's GameModeReplay): game mode 10, playing back a saved shot (gReplayData):
// the saved course, hole, pins, tees, wind, weather and player are put back and the shot starts
// again.
// The file ends with the target games' target list (gSkillZoneCups, gSkillZoneNumCups points):
// GameModeSkillZoneBase_GetCupCount, GetCupPosition and AddCup are TW07's
// GameMode_SkillZoneBase.cpp methods of those names, which TW07's source order puts right before
// SortCupsByDistanceFromTee, GameTargets.c's first function. They belong to GameTargets.c's EA file;
// they stay here because the bytes cannot prove where the split falls (see GameTargets.c).

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

// This file's .sbss (game.h).
s8 gSkillZoneNumCups;                   // how many targets gSkillZoneCups holds

void GameModeReplay_LoadHole(void);
void GameModeReplay_RestartHole(void);
void GameModeReplay_StartGamePreData(void);
void GameModeReplay_SetupNextGolfer(void);
void GameModeReplay_RestoreWeather(void);
u8   GameModeReplay_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeReplay_GameFinished(u8 bCheck);
void GameModeReplay_EndGame(void);

f32 gSkillZoneCups[40][4];              // the target games' targets (x, y, z, w = 1)

// Mode 10's setup (GM_SetModeType): this file's callbacks; the flags b273, b276 (re-plan as the
// swing begins), b27B, b27C, b27D, b27F, b280 (the mid-hole flyover) and b281 (tutorial tips)
// cleared; one player, no mulligans, no split screen, and hole 0 of the round made current.
void GameModeReplay_Init(void) {
    gpGame->pfnInit = GameModeReplay_Init;
    gpGame->pfnSetupNextGolfer = GameModeReplay_SetupNextGolfer;
    gpGame->pfnHoleFinished = GameModeReplay_HoleFinished;
    gpGame->pfnGameFinished = GameModeReplay_GameFinished;
    gpGame->pfnStartGamePreData = GameModeReplay_StartGamePreData;
    gpGame->pfnEndGame = GameModeReplay_EndGame;
    gpGame->pfnLoadHole = GameModeReplay_LoadHole;
    gpGame->pfnRestartHole = GameModeReplay_RestartHole;
    gpGame->b273 = 0;
    gpGame->b276 = 0;
    gpGame->b27B = 0;
    gpGame->b27C = 0;
    gpGame->b27D = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->n4 = 0;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    GM_SetCurrentHole(0);
    gSession.nSplitScreen = 0;
    gSession.nNumPlayers = 1;
}

// Mode 10's hole start (pfnLoadHole): the saved wind (direction and speed) and the saved course
// settings nF1A, nF1C (the fairway setting) and nF1E go back in (fn_80055C40, fn_80055CAC,
// fn_80055CD0).
void GameModeReplay_LoadHole(void) {
    Wind_Set(gReplayData.nWindDir, gReplayData.nWindSpeed);
    fn_80055C40(gReplayData.nF1A);
    fn_80055CAC(gReplayData.nF1C);
    fn_80055CD0(gReplayData.nF1E);
}

// Mode 10's hole restart (pfnRestartHole): the saved weather is forced again
// (GameModeReplay_RestoreWeather).
void GameModeReplay_RestartHole(void) {
    GameModeReplay_RestoreWeather();
}

// Sets the session's pin set from the replay and returns it (an inline in EA's source).
static inline s8 Replay_SetPinSet(void) {
    return gSession.nPinSet = gReplayData.nPinSet;
}

// Mode 10's round start (pfnStartGamePreData): every hole's pin set and the session's from the
// replay; the saved course with the saved hole as the only one selected; one player; the replay
// flag (gSession.bReplay) set; the saved weather option (nF12), with its amount (nF14 / 100) forced
// for option 3; player 0's tee set.
void GameModeReplay_StartGamePreData(void) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->nPinSet[i] = Replay_SetPinSet();
    }
    GM_SetCurrentCourse(gReplayData.nCourse);
    GM_SelectHoleSet(0);
    GM_SelectSingleHole(gReplayData.nHole);
    Session_SetNumPlayers(1);
    gSession.bReplay = 1;
    // fake match: a no-op cast of &gSession; written plainly the address is scheduled
    // differently (96.9%)
    ((Session*)&gSession)->options.nWeather = gReplayData.nF12;
    if (gReplayData.nF12 == 3) {
        PlayNow_ForceWeather(gReplayData.nF14 / 100.0f);
    }
    gSession.nTeeSet[0] = gReplayData.nTeeSet;
    Replay_SetPinSet();
}

// Mode 10's next turn (pfnSetupNextGolfer): player 0 is put back as they were before the saved shot
// (golfer data, attribute changes, score, shot and swing blocks, flags, aim points, this hole's
// strokes, the ball, dropped again where it lay) with the saved club and shot type. The shot is
// then saved again (REPLAY_Save with the replay flag off, so it saves), the saved timing values,
// seed, wind and weather put back over what that save wrote, the random seed set from it, and
// player 0 switched to the pre-shot state (1).
void GameModeReplay_SetupNextGolfer(void) {
    Ball ball;
    f32 fF08;
    f32 fF0C;
    u32 nSeed;
    s16 nWindDir;
    s16 nWindSpeed;
    s16 nF12;
    s16 nF14;

    Mem_cpy(&gPlayers[0].golfer, &gReplayData.player.golfer, sizeof(gPlayers[0].golfer));
    Mem_cpy(gPlayers[0].attrMod, gReplayData.player.attrMod, 0xC);
    // port: the score block from nStrokes to b30C (0x154..0x30C), copied whole
    Mem_cpy(gPlayers[0].nStrokes, gReplayData.player.nStrokes, 0x1B8);
    // b30C up to the shot block at 0x354: the flags and the round's money. Sized as the distance
    // port: between the two fields the copy scores 96.4%
    Mem_cpy(&gPlayers[0].bHitObject, &gReplayData.player.bHitObject, 0x48);
    // port: the shot block from nClub to nShotKind2 (0x354..0x3B0), copied whole
    Mem_cpy(&gPlayers[0].nClub, &gReplayData.player.nClub, 0x5C);
    Mem_cpy(&gPlayers[0].nShotKind2, &gReplayData.player.nShotKind2, 4);
    // port: the swing data up to (not including) its byte 0x630
    Mem_cpy(&gPlayers[0].swing, &gReplayData.player.swing, 0x630);
    gPlayers[0].uFlags = gReplayData.player.uFlags;
    LLMath_CopyVec(gReplayData.player.vBall, gPlayers[0].vBall);
    LLMath_CopyVec(gReplayData.player.vPreShot, gPlayers[0].vPreShot);
    LLMath_CopyVec(gReplayData.player.vTarget, gPlayers[0].vTarget);
    LLMath_CopyVec(gReplayData.player.vTargetCopy, gPlayers[0].vTargetCopy);
    LLMath_CopyVec(gReplayData.player.vTarget2, gPlayers[0].vTarget2);
    LLMath_CopyVec(gReplayData.player.vA44, gPlayers[0].vA44);
    gPlayers[0].nStrokes[Game_CurHoleIndex()] = gReplayData.nStrokes;
    gPlayers[0].fDistance = gReplayData.player.fDistance;
    gPlayers[0].fDistance2 = gReplayData.player.fDistance2;
    Mem_cpy(&gPlayers[0].ball, &gReplayData.player.ball, sizeof(Ball));
    Physics_InitBall(&ball, gReplayData.player.ball.vPos, 0);
    gPlayers[0].ball.pCourse = ball.pCourse;
    gPlayers[0].ball.nPlayer = 0;
    Physics_DropBall(&ball, gReplayData.player.ball.vPos);
    Character_SelectGameClub(gPlayers[0].pChar, gPlayers[0].nClub);
    Character_SelectGameShotType(gPlayers[0].pChar, gPlayers[0].nShotKind);
    gPlayers[0].swing.bUIInit = 0;
    fF08 = gReplayData.fF08;
    gSession.bReplay = 0;
    fF0C = gReplayData.fF0C;
    nSeed = gReplayData.nSeed;
    nWindDir = gReplayData.nWindDir;
    nWindSpeed = gReplayData.nWindSpeed;
    nF12 = gReplayData.nF12;
    nF14 = gReplayData.nF14;
    REPLAY_Save(0);
    gSession.bReplay = 1;
    gReplayData.fF08 = fF08;
    gReplayData.fF0C = fF0C;
    gSession.nSeed = nSeed;
    gReplayData.nSeed = nSeed;
    gReplayData.nWindDir = nWindDir;
    gReplayData.nWindSpeed = nWindSpeed;
    gReplayData.nF12 = nF12;
    gReplayData.nF14 = nF14;
    Misc_SetSeedFunc(0, nSeed);
    GOLFERSTATE_Switch(1, 0);
}

// For a replay whose weather option (nF12) is 1, 2 or 3, forces its saved weather amount (nF14 /
// 100) again (PlayNow_ForceWeather).
void GameModeReplay_RestoreWeather(void) {
    if (gReplayData.nF12 == 1 || gReplayData.nF12 == 2 || gReplayData.nF12 == 3) {
        PlayNow_ForceWeather(gReplayData.nF14 / 100.0f);
    }
}

// Mode 10's hole-over test (pfnHoleFinished): always 1.
u8 GameModeReplay_HoleFinished(int nPlayer, u8 bCheck) {
    return 1;
}

// Mode 10's game-over test (pfnGameFinished): always 1.
u8 GameModeReplay_GameFinished(u8 bCheck) {
    return 1;
}

// Mode 10's end of game (pfnEndGame): the game loop ends (gSession.bEndLoop).
void GameModeReplay_EndGame(void) {
    gSession.bEndLoop = 1;
}

// fake match: the original's .sdata2 has a 0.0f here (0x80284694, after 100.0f and before 1.0f)
// that no code loads; where it came from is unknown (a stripped function's constant would be
// stripped with it). Defined as data, kept through the linker's dead-stripping, so the file's
// constants line up.
#pragma force_active on
const f32 lbl_80284694 = 0.0f;
#pragma force_active reset

// How many targets the target games' list holds (gSkillZoneNumCups). With the next two functions it
// belongs to GameTargets.c's EA file (TW07 GameMode_SkillZoneBase.cpp) though it sits in this one.
int GameModeSkillZoneBase_GetCupCount(void) {
    return gSkillZoneNumCups;
}

// Copies target i's position (x, y, z and w = 1, four floats) from the target games' list
// (gSkillZoneCups) to pOut.
void GameModeSkillZoneBase_GetCupPosition(int i, f32* pOut) {
    LLMath_CopyVec(gSkillZoneCups[i], pOut);
}

// Adds a target at (x, y, z) (w = 1) to the end of the target games' list and counts it. In a
// target game (GM_Currently_SkillZoneMode) each pin position the course stream brings (fn_800347B4,
// GoTerrain.c) becomes a target. Nothing checks the list's 40 entries.
void GameModeSkillZoneBase_AddCup(f32 x, f32 y, f32 z) {
    gSkillZoneCups[gSkillZoneNumCups][0] = x;
    gSkillZoneCups[gSkillZoneNumCups][1] = y;
    gSkillZoneCups[gSkillZoneNumCups][2] = z;
    gSkillZoneCups[gSkillZoneNumCups][3] = 1.0f;
    gSkillZoneNumCups++;
}

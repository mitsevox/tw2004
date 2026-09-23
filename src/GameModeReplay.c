// GameModeReplay.c (TW06's GameModeReplay): game mode 10, playing back a saved shot (gReplayData):
// the saved course, hole, wind and player are put back and the shot starts again. Also the target
// games' target list (lbl_80211D38, lbl_80282360 points); the rest of their shared code is
// GameTargets.c.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

void  fn_800E14E0(int nCourse);
void  fn_800E1404(int nHole);
void  fn_800ED6F8(f32 x0);
void  fn_8006BF60(int nPlayer);

void fn_800F1388(void);
void fn_800F1404(void);
void fn_800F1424(void);
void fn_800F15AC(void);
void fn_800F18C8(void);
u8   fn_800F193C(int nPlayer, u8 bCheck);
u8   fn_800F1944(u8 bCheck);
void fn_800F194C(void);

// TW06: GameModeReplay::Init. Mode 10 starts: one player, no mulligans, the saved shot's hole.
void fn_800F125C(void) {
    gpGame->pfnInit = fn_800F125C;
    gpGame->pfnSetupNextGolfer = fn_800F15AC;
    gpGame->pfnHoleFinished = fn_800F193C;
    gpGame->pfnGameFinished = fn_800F1944;
    gpGame->pfn1EC = fn_800F1424;
    gpGame->pfnEndGame = fn_800F194C;
    gpGame->pfn1E4 = fn_800F1388;
    gpGame->pfn224 = fn_800F1404;
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
    fn_800E1480(0);
    gSession.nSplitScreen = 0;
    gSession.nNumPlayers = 1;
}

// TW06: GameModeReplay::LoadHole. The saved wind and conditions.
void fn_800F1388(void) {
    Wind_Set(gReplayData.nWindDir, gReplayData.nWindSpeed);
    fn_80055C40(gReplayData.nF1A);
    fn_80055CAC(gReplayData.nF1C);
    fn_80055CD0(gReplayData.nF1E);
}

// TW06: GameModeReplay::RestartHole.
void fn_800F1404(void) {
    fn_800F18C8();
}

// Sets the session's pin set from the replay and returns it (an inline in EA's source).
static inline s8 Replay_SetPinSet(void) {
    return gSession.nPinSet = gReplayData.nPinSet;
}

// TW06: GameModeReplay::StartGamePreData. The saved course, hole, pins and tees.
void fn_800F1424(void) {
    int i;
    for (i = 0; i < 18; i++) {
        gpGame->nPinSet[i] = Replay_SetPinSet();
    }
    fn_800E14E0(gReplayData.nCourse);
    fn_800E1260(0);
    fn_800E1404(gReplayData.nHole);
    Session_SetNumPlayers(1);
    gSession.bReplay = 1;
    // fake match: a no-op cast of &gSession; written plainly the address is scheduled
    // differently (96.9%)
    ((Session*)&gSession)->options.nC = gReplayData.nF12;
    if (gReplayData.nF12 == 3) {
        fn_800ED6F8(gReplayData.nF14 / 100.0f);
    }
    gSession.nTeeSet[0] = gReplayData.nTeeSet;
    Replay_SetPinSet();
}

// TW06: GameModeReplay::SetupNextGolfer. Put player 0 back as they were before the shot, then
// start it.
void fn_800F15AC(void) {
    Ball ball;
    f32 fF08;
    f32 fF0C;
    u32 nSeed;
    s16 nWindDir;
    s16 nWindSpeed;
    s16 nF12;
    s16 nF14;

    Mem_cpy(&gPlayers[0].golfer, &gReplayData.player.golfer, 0x140);
    Mem_cpy(gPlayers[0].attrMod, gReplayData.player.attrMod, 0xC);
    Mem_cpy(gPlayers[0].nStrokes, gReplayData.player.nStrokes, 0x1B8);
    // b30C up to the shot block at 0x354: the flags and the round's money. Sized as the distance
    // port: between the two fields the copy scores 96.4%
    Mem_cpy(&gPlayers[0].b30C, &gReplayData.player.b30C, 0x48);
    Mem_cpy(&gPlayers[0].nClub, &gReplayData.player.nClub, 0x5C);
    Mem_cpy(&gPlayers[0].nShotKind2, &gReplayData.player.nShotKind2, 4);
    Mem_cpy(&gPlayers[0].swing, &gReplayData.player.swing, 0x630);
    gPlayers[0].uFlags = gReplayData.player.uFlags;
    Vec_Copy(&gReplayData.player.fBallX, &gPlayers[0].fBallX);
    Vec_Copy(gReplayData.player.vPreShot, gPlayers[0].vPreShot);
    Vec_Copy(&gReplayData.player.fTargetX, &gPlayers[0].fTargetX);
    Vec_Copy(gReplayData.player.vTargetCopy, gPlayers[0].vTargetCopy);
    Vec_Copy(gReplayData.player.vTarget2, gPlayers[0].vTarget2);
    Vec_Copy(gReplayData.player.vA44, gPlayers[0].vA44);
    gPlayers[0].nStrokes[Game_CurHoleIndex()] = gReplayData.nStrokes;
    gPlayers[0].fDistance = gReplayData.player.fDistance;
    gPlayers[0].fDistance2 = gReplayData.player.fDistance2;
    Mem_cpy(&gPlayers[0].ball, &gReplayData.player.ball, sizeof(Ball));
    fn_80055AA8(&ball, gReplayData.player.ball.vPos, 0);
    gPlayers[0].ball.pCourse = ball.pCourse;
    gPlayers[0].ball.nPlayer = 0;
    Physics_DropBall(&ball, gReplayData.player.ball.vPos);
    fn_8001C774(gPlayers[0].pChar, gPlayers[0].nClub);
    fn_8001C724(gPlayers[0].pChar, gPlayers[0].nShotKind);
    gPlayers[0].swing.bUIInit = 0;
    fF08 = gReplayData.fF08;
    gSession.bReplay = 0;
    fF0C = gReplayData.fF0C;
    nSeed = gReplayData.nSeed;
    nWindDir = gReplayData.nWindDir;
    nWindSpeed = gReplayData.nWindSpeed;
    nF12 = gReplayData.nF12;
    nF14 = gReplayData.nF14;
    fn_8006BF60(0);
    gSession.bReplay = 1;
    gReplayData.fF08 = fF08;
    gReplayData.fF0C = fF0C;
    gSession.nSeed = nSeed;
    gReplayData.nSeed = nSeed;
    gReplayData.nWindDir = nWindDir;
    gReplayData.nWindSpeed = nWindSpeed;
    gReplayData.nF12 = nF12;
    gReplayData.nF14 = nF14;
    fn_8000B1D4(0, nSeed);
    GOLFERSTATE_Switch(1, 0);
}

void fn_800F18C8(void) {
    if (gReplayData.nF12 == 1 || gReplayData.nF12 == 2 || gReplayData.nF12 == 3) {
        fn_800ED6F8(gReplayData.nF14 / 100.0f);
    }
}

// TW06: GameModeReplay::HoleFinished.
u8 fn_800F193C(int nPlayer, u8 bCheck) {
    return 1;
}

// TW06: GameModeReplay::GameFinished.
u8 fn_800F1944(u8 bCheck) {
    return 1;
}

// TW06: GameModeReplay::EndGame.
void fn_800F194C(void) {
    gSession.unk11[1] = 1;
}

// fake match: the original's .sdata2 has a 0.0f here (0x80284694, after 100.0f and before 1.0f)
// that no code loads; where it came from is unknown (a stripped function's constant would be
// stripped with it). Defined as data, kept through the linker's dead-stripping, so the file's
// constants line up.
#pragma force_active on
const f32 lbl_80284694 = 0.0f;
#pragma force_active reset

// The target games' target list.
int fn_800F1960(void) {
    return lbl_80282360;
}

void fn_800F196C(int i, f32* pOut) {
    Vec_Copy(lbl_80211D38[i], pOut);
}

void fn_800F199C(f32 x, f32 y, f32 z) {
    lbl_80211D38[lbl_80282360][0] = x;
    lbl_80211D38[lbl_80282360][1] = y;
    lbl_80211D38[lbl_80282360][2] = z;
    lbl_80211D38[lbl_80282360][3] = 1.0f;
    lbl_80282360++;
}

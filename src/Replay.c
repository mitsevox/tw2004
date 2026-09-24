// Replay.c (EA's name, from its asserts; TW06): the shot replay. Before a shot it saves the
// player as he was and the conditions (gReplayData, game.h, and the buffer at lbl_80281E48),
// keeps the spin he put on the ball, and puts it all back to replay the shot or to take it back.

#include "game.h"
#include "terrain.h"

int  fn_80055CA4(void);                 // Ball.c
int  fn_80055CC8(void);                 // Ball.c
int  fn_80055D34(void);                 // Ball.c
int  fn_80055F78(void);                 // Ball.c: the wind's direction
f32  fn_80055F80(void);                 // Ball.c: the wind's speed
f32  fn_8006C630(void);                 // gomainloop.c

void fn_8006BED4(void) {
    lbl_80281E48 = fn_80009B34(sizeof(ReplayBuffer), 2, 0, "Replay.c", 77);
    lbl_80281E48->b1525C = 0;
}

void fn_8006BF20(void) {
    fn_80009E70(lbl_80281E48);
    lbl_80281E48 = NULL;
}

void fn_8006BF4C(void) {
    gReplayData.bF10 = 0;
}

// Before nPlayer's shot: save everything a replay of it needs (a new random seed, the player, his
// save profile, the record tables, the UI queues and the conditions), unless a saved replay is
// playing. A CPU's shot, or one with uFlags bit 3 set, turns in-flight replays off.
void fn_8006BF60(int nPlayer) {
    s32 nMode;

    if (gSession.bReplay) return;
    if (Player_IsCPU(nPlayer)) {
        gReplayData.bF10 = 0;
        return;
    }
    if (gPlayers[nPlayer].uFlags & 8) {
        gReplayData.bF10 = 0;
        return;
    }
    gSession.nSeed = Rand_Next(0);
    fn_8000B1D4(0, gSession.nSeed);
    gReplayData.nSeed = gSession.nSeed;
    Mem_cpy(&gReplayData.player, &gPlayers[nPlayer], sizeof(Player));
    Mem_cpy(&lbl_80281E48->profile, &gpSaveData[nPlayer], sizeof(SaveProfile));
    Mem_cpy(&lbl_80281E48->records, gSession.aCourseRecord, sizeof(SaveRecords));
    lbl_80281E48->aQueueCount[0] = lbl_802822B8;
    lbl_80281E48->aQueueCount[1] = lbl_802822B4;
    lbl_80281E48->aQueueCount[2] = lbl_802822B0;
    lbl_80281E48->aQueueCount[3] = lbl_802822AC;
    lbl_80281E48->aQueueCount[4] = lbl_802822A8;
    lbl_80281E48->aQueueCount[5] = lbl_802822A0;
    lbl_80281E48->aQueueCount[6] = lbl_8028229C;
    lbl_80281E48->aQueueCount[7] = lbl_80282298;
    lbl_80281E48->aQueueCount[8] = lbl_80282294;
    lbl_80281E48->aQueueCount[9] = lbl_80282290;
    lbl_80281E48->aQueueCount[10] = lbl_8028228C;
    lbl_80281E48->aQueueCount[11] = lbl_80282288;
    gReplayData.nCourse = Game_GetCourse();
    gReplayData.nHole = fn_80015464();
    gReplayData.nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    gReplayData.nTeeSet = gSession.nTeeSet[nPlayer];
    gReplayData.nPinSet = Game_CurrentPinSet();
    gReplayData.nWindDir = fn_80055F78();
    gReplayData.nWindSpeed = fn_80055F80();
    nMode = gSession.options.nC;
    gReplayData.nF12 = nMode;
    if (nMode == 2) {
        if (fn_80035574()) {
            gReplayData.nF12 = 3;
        } else {
            gReplayData.nF12 = 4;
        }
    }
    gReplayData.nF14 = 100.0f * fn_8006C630();
    gReplayData.nF1A = fn_80055CA4();
    gReplayData.nF1C = fn_80055CC8();
    gReplayData.nF1E = fn_80055D34();
    gReplayData.fF08 = 0.0f;
    gReplayData.fF0C = 0.0f;
    gSession.bReplay = 0;
    gReplayData.bF10 = 1;
}

// While in-flight replays are on, the saved player uses this controller.
void fn_8006C28C(int nPlayer, int nController) {
    if (gReplayData.bF10) {
        gReplayData.player.nController = nController;
    }
}

// While in-flight replays are on, keep the spin put on the ball.
void fn_8006C2A8(int nPlayer, f32 fForwardSpin, f32 fSideSpin) {
    if (gReplayData.bF10) {
        gReplayData.fF08 = fForwardSpin;
        gReplayData.fF0C = fSideSpin;
    }
}

// The saved spin, or none when in-flight replays are off.
void fn_8006C2C8(int nPlayer, f32* pForwardSpin, f32* pSideSpin) {
    if (gReplayData.bF10) {
        *pForwardSpin = gReplayData.fF08;
        *pSideSpin = gReplayData.fF0C;
    } else {
        *pForwardSpin = 0.0f;
        *pSideSpin = 0.0f;
    }
}

// Starts replaying nPlayer's saved shot: the seed, the player (keeping his ball, uFlags and fEEC,
// and setting uFlags bit 0), the record tables, his save profile and the UI queues come back.
void REPLAY_Play(int nPlayer) {
    Ball ball;
    u32 uFlags;
    f32 fEEC;

    if (gReplayData.bF10) {
        fn_8000B1D4(0, gReplayData.nSeed);
        Mem_cpy(&ball, &gPlayers[nPlayer].ballBefore, sizeof(Ball));
        uFlags = gPlayers[nPlayer].uFlags;
        fEEC = gPlayers[nPlayer].fEEC;
        Mem_cpy(&gPlayers[nPlayer], &gReplayData.player, sizeof(Player));
        Mem_cpy(&gPlayers[nPlayer].ballBefore, &ball, sizeof(Ball));
        gPlayers[nPlayer].uFlags = uFlags;
        gPlayers[nPlayer].fEEC = fEEC;
        gPlayers[nPlayer].uFlags |= 1;
        gPlayers[nPlayer].swing.unk630 = 0;
        Mem_cpy(gSession.aCourseRecord, &lbl_80281E48->records, sizeof(SaveRecords));
        Mem_cpy(&gpSaveData[nPlayer], &lbl_80281E48->profile, sizeof(SaveProfile));
        lbl_802822B8 = lbl_80281E48->aQueueCount[0];
        lbl_802822B4 = lbl_80281E48->aQueueCount[1];
        lbl_802822B0 = lbl_80281E48->aQueueCount[2];
        lbl_802822AC = lbl_80281E48->aQueueCount[3];
        lbl_802822A8 = lbl_80281E48->aQueueCount[4];
        lbl_802822A0 = lbl_80281E48->aQueueCount[5];
        lbl_8028229C = lbl_80281E48->aQueueCount[6];
        lbl_80282298 = lbl_80281E48->aQueueCount[7];
        lbl_80282294 = lbl_80281E48->aQueueCount[8];
        lbl_80282290 = lbl_80281E48->aQueueCount[9];
        lbl_8028228C = lbl_80281E48->aQueueCount[10];
        lbl_80282288 = lbl_80281E48->aQueueCount[11];
        gSession.bReplay = 1;
    }
}

// Ends a saved replay's playback.
void fn_8006C4A0(void) {
    if (gSession.bReplay) {
        gSession.bReplay = 0;
    }
}

// Puts back what fn_8006BF60 saved before nPlayer's shot (the player whole, his save profile, the
// record tables and the UI queues), then fn_800335F8(1).
void REPLAY_Restore(int nPlayer) {
    if (gReplayData.bF10) {
        Mem_cpy(&gPlayers[nPlayer], &gReplayData.player, sizeof(Player));
        Mem_cpy(&gpSaveData[nPlayer], &lbl_80281E48->profile, sizeof(SaveProfile));
        Mem_cpy(gSession.aCourseRecord, &lbl_80281E48->records, sizeof(SaveRecords));
        lbl_802822B8 = lbl_80281E48->aQueueCount[0];
        lbl_802822B4 = lbl_80281E48->aQueueCount[1];
        lbl_802822B0 = lbl_80281E48->aQueueCount[2];
        lbl_802822AC = lbl_80281E48->aQueueCount[3];
        lbl_802822A8 = lbl_80281E48->aQueueCount[4];
        lbl_802822A0 = lbl_80281E48->aQueueCount[5];
        lbl_8028229C = lbl_80281E48->aQueueCount[6];
        lbl_80282298 = lbl_80281E48->aQueueCount[7];
        lbl_80282294 = lbl_80281E48->aQueueCount[8];
        lbl_80282290 = lbl_80281E48->aQueueCount[9];
        lbl_8028228C = lbl_80281E48->aQueueCount[10];
        lbl_80282288 = lbl_80281E48->aQueueCount[11];
        gPlayers[nPlayer].swing.unk630 = 0;
        fn_800335F8(1);
    }
}

void fn_8006C5E0(void) {
    if (!gSession.bReplay) {
        lbl_80281E48->b1525C = 1;
    }
}

void fn_8006C608(void) {
    if (!gSession.bReplay) {
        lbl_80281E48->b1525C = 0;
    }
}
